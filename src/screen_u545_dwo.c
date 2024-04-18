#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "picowalker-defs.h"
#include "peripherals.h"

#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"


#define AMOLED_WIDTH 368
#define AMOLED_HEIGHT 448

// 4 is max scale that fits in amoled
#define SCREEN_SCALE 4

// we have 256kiB ram, screen buffer is 483kiB at 24bpp
// and 322kiB at 16bpp so we can't write a full amoled
// screen at once
//
// pw screen scaled up x4 is 192k, this leaves us with
// 64k left for other stuff. Plenty of space
#define AMOLED_BUFFER_LEN (192*1024)
uint8_t AMOLED_BUFFER[AMOLED_BUFFER_LEN] = {0};

// map pw colour indices to 565 colours, dark to light
const uint16_t colour_map[4] = {
    0xe75b, // white
    0xbe16, // light grey
    0x7c0e, // dark grey
    0x5289, // black
};

extern OSPI_HandleTypeDef SCREEN_OSPI_HANDLE;

enum screen_cmd {
    CMD_NOP             = 0x00,
    CMD_SWRST           = 0x01,
    CMD_READ_ID         = 0x04,
    CMD_READ_SELFTEST   = 0x0f,
    CMD_SLEEP_OUT       = 0x11,
    CMD_INVERT_OFF      = 0x20,
    CMD_INVERT_ON       = 0x21,
    CMD_ALL_OFF         = 0x22,
    CMD_ALL_ON          = 0x23,
    CMD_DISPLAY_OFF     = 0x28,
    CMD_DISPLAY_ON      = 0x29,
    CMD_COL_SET         = 0x2a,
    CMD_PAGE_SET        = 0x2b,
    CMD_WRITE_START     = 0x2c,
    CMD_TE_OFF          = 0x34,
    CMD_TE_ON           = 0x35,
    CMD_IDLE_OFF        = 0x38,
    CMD_IDLE_ON         = 0x39,
    CMD_PIXEL_FORMAT    = 0x3a,
    CMD_WRITE_CONTINUE  = 0x3c,
    CMD_WRITE_TE_LINE   = 0x44,
    CMD_SET_BRIGHTNESS  = 0x51,
    CMD_WRITE_CTRL_DSP1 = 0x53,
    CMD_WRITE_CTRL_DPS2 = 0x55,
    CMD_SPI_MODE_CTRL   = 0xc4,
};

typedef struct amoled_s {
    int offset_x;
    int offset_y;
    size_t true_width;
    size_t true_height;
} amoled_t;

typedef struct screen_area_s {
    int x;
    int y;
    int width;
    int height;
} screen_area_t;

static amoled_t amoled = {0};

/**
 * Writes a command to the screen via OSPI.
 *
 * Command is sent in OSPI address phase as 24 bits.
 * OSPI instruction is `0x02` so the screen knows to expect
 * 1-line data and 1-line address.
 *
 * @param cmd Command to send to screen
 * @param len Length of the parameters to send
 * @param params Array of parameters to send
 */
void amoled_send_cmd(uint8_t cmd, size_t len, uint8_t params[len]) {

    // See https://github.com/STMicroelectronics/stm32u5xx_hal_driver/blob/f1946d68701260860cdb5eae9f7bc5c3ff7f1cd1/Src/stm32u5xx_hal_ospi.c#L58
    // see also AN5050 section 6.2
    // see also STM32 demo code https://www.dwo.net.cn/pd.jsp?id=11944&fromColId=0#_pp=0_316_1_-1

    OSPI_HandleTypeDef *hospi = &SCREEN_OSPI_HANDLE;
    OSPI_RegularCmdTypeDef ospi_cmd = {
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG, // always common in regular mode
                                                     //
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .AddressMode = HAL_OSPI_ADDRESS_1_LINE,
        .AddressSize = HAL_OSPI_ADDRESS_24_BITS,
        .DataMode = HAL_OSPI_DATA_1_LINE, // commands are in 1-wire mode

        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,
        .SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD,
        .DQSMode = HAL_OSPI_DQS_DISABLE,
        .DummyCycles = 0,

        .Instruction = 0x02,
        .Address = ((uint32_t)cmd) << 8, // ignores top 8 bits in 24-bit mode
        .NbData = len,
    };

    // If we dont do this and have no parameters, it messes up the HAL state machine
    if(len == 0) {
        ospi_cmd.DataMode = HAL_OSPI_DATA_NONE;
    }

    // This transfers the instruction and address if we have no data
    // else it sets up the transfer frame and sends everything on the next
    // transmit call
    if( HAL_OSPI_Command(hospi, &ospi_cmd, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK ) {
        Error_Handler();
    }

    if(len > 0) {
        if( HAL_OSPI_Transmit(hospi, params, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            Error_Handler();
        }
    }

}

/**
 * Sends pixel data to the screen with the given command
 * Sends data in 4-line mode so we use instruction `0x32`.
 *
 * Note: This can break at lower clock speeds (<5MHz)
 * because the HAL breaks up the data into separate packers
 * if its waiting too long.
 * At least thats what i observed with a logic analyser.
 *
 * @param len Number of bytes to send
 * @param data The data to send
 * @param cmd The command to use when sending
 */
void amoled_send_data(size_t len, uint8_t data[len], uint8_t cmd) {
    OSPI_HandleTypeDef *hospi = &SCREEN_OSPI_HANDLE;
    OSPI_RegularCmdTypeDef ospi_cmd = {
        .OperationType = HAL_OSPI_OPTYPE_COMMON_CFG, // always common in regular mode
        .InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE,
        .InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS,
        .InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE,

        .AddressMode = HAL_OSPI_ADDRESS_1_LINE,
        .AddressSize = HAL_OSPI_ADDRESS_24_BITS,
        .AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE,

        .AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE,

        .DataMode = HAL_OSPI_DATA_4_LINES, // pixels in 4-line mode
        //.DataMode = HAL_OSPI_DATA_1_LINE,
        .DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE,

        .SIOOMode = HAL_OSPI_SIOO_INST_ONLY_FIRST_CMD, // SIOO still sends address
        .DummyCycles = 0,
        .DQSMode = HAL_OSPI_DQS_DISABLE,

        .Instruction = 0x32, // 0x32 for 4-wire mode
        //.Instruction = 0x02, // 0x02 for 1-wire mode
        .Address = (cmd<<8),
        //.NbData = 16,
        .NbData = len,
    };

    if(len == 0 || data == 0) {
        return;
    }

    if( HAL_OSPI_Command(hospi, &ospi_cmd, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK ) {
        Error_Handler();
        return;
    }

    // Note: Needs high clock speeds (100MHz/16) to complete all in one write
    if( HAL_OSPI_Transmit(hospi, data, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        Error_Handler();
        return;
    }
}

/**
 * Sets an area of the screen to a solid 16-bit colour.
 *
 * It breaks up data into separate chunks that fit into
 * the global pixel buffer.
 *
 * The data needs to be sent twice for some reason, I
 * can't find any mention of it in the documentation
 * apart from in the STM32 demo code
 *
 * @param x_start First column to start writing to
 * @param y_start First row to start writing to
 * @param width Width of area to write
 * @param height Height of area to write
 * @param buf The buffer to send to the screen
 * @param len Number of bytes to send
 *
 */
void amoled_draw_buffer(int x_start, int y_start, int width, int height,
        size_t len, uint8_t buf[len]) {

    uint8_t params[4] = {0};
    int x_end = x_start + width - 1;
    int y_end = y_start + height - 1;

    // BE
    params[0] = x_start>>8;
    params[1] = x_start&0xff;
    params[2] = x_end>>8; // end
    params[3] = x_end&0xff;
    amoled_send_cmd(CMD_COL_SET, 4, params);

    params[0] = y_start>>8;
    params[1] = y_start&0xff;
    params[2] = y_end>>8; // end
    params[3] = y_end&0xff;
    amoled_send_cmd(CMD_PAGE_SET, 4, params);

    size_t n_bytes = width*height*2; // 2 bytes per pixel
    if(n_bytes > len) {
        Error_Handler();
    }

    // send it in batches if needed
    amoled_send_data(len, buf, CMD_WRITE_START);

    // send it again with 0x3c, doesn't work without it
    amoled_send_data(len, buf, CMD_WRITE_CONTINUE);

    // send nop to say we are done
    amoled_send_cmd(CMD_NOP, 0, params);

    // Delay may not be necessary
    HAL_Delay(10);
}

/**
 * Sets an area of the screen to a solid 16-bit colour.
 *
 * It breaks up data into separate chunks that fit into
 * the global pixel buffer.
 *
 * The data needs to be sent twice for some reason, I
 * can't find any mention of it in the documentation
 * apart from in the STM32 demo code
 *
 * @param x_start First column to start writing to
 * @param y_start First row to start writing to
 * @param width Width of area to write
 * @param height Height of area to write
 * @param colour 565 colour value to write
 *
 */
void amoled_draw_block(int x_start, int y_start, int width, int height, uint32_t colour) {

    size_t n_bytes = 2*width*height;
    size_t max_bytes = n_bytes < AMOLED_BUFFER_LEN ? n_bytes : AMOLED_BUFFER_LEN;

    for(size_t i = 0; i < max_bytes; i+=2) {
        SCREEN_BUFFER[i+0] = colour>>8;
        SCREEN_BUFFER[i+1] = colour&0xff;
    }

    size_t total_height = 0;
    while(n_bytes > 0) {
        size_t bytes_to_send = n_bytes < AMOLED_BUFFER_LEN ? n_bytes : AMOLED_BUFFER_LEN;
        size_t this_height = bytes_to_send / (2*width);
        bytes_to_send = this_height * (2*width); // make sure it aligns

        amoled_draw_buffer(x_start, y_start+total_height, width, this_height, bytes_to_send, AMOLED_BUFFER);
        n_bytes -= bytes_to_send;
        total_height += this_height;
    }
}



/**
 * Decode a 2bpp pokewalker-encoded image.
 * Needs to rotate by 90 degrees clockwise
 */
void decode_img(pw_img_t *pw_img, size_t out_len, uint8_t out_buf[out_len]) {

    uint8_t pixel_value, bpu, bpl;
    size_t row, col, stride = pw_img->height;

    pw_img->size = pw_img->width * pw_img->height * 2/8;

    // quit if the output buffer can't hold all the data
    if(out_len < pw_img->size * 2*SCREEN_SCALE*SCREEN_SCALE) {
        Error_Handler();
    }

    // i = number of bytes into image
    for(size_t i = 0; i < pw_img->size; i+= 2) {
        bpu = pw_img->data[i+0];
        bpl = pw_img->data[i+1];

        // j = index of pixel in chunk
        for(size_t j = 0; j < 8; j++) {
            pixel_value  = ((bpu>>j) & 1) << 1;
            pixel_value |= ((bpl>>j) & 1);

            // transform coords
            size_t x_normal = (i/2)%pw_img->width;
            size_t y_normal = 8*(i/(2*pw_img->width)) + j;
            col = pw_img->height - y_normal;
            row = x_normal;

            // now we have pixel coordinate, write to all pixels
            // that need the colour
            for(size_t py = 0; py < SCREEN_SCALE; py++) {
                for(size_t px = 0; px < SCREEN_SCALE; px++) {
                    size_t base_index = SCREEN_SCALE*((SCREEN_SCALE*row+py)*stride) + SCREEN_SCALE*col+px;

                    out_buf[2*base_index+0] = colour_map[pixel_value]>>8;
                    out_buf[2*base_index+1] = colour_map[pixel_value]&0xff;
                    if(2*base_index > out_len) Error_Handler();
                }
            }
        }
    }
}

/**
 * Transforms an area from pokewalker coordinates to amoled coordinates
 * performs rotation, scale and translation
 * 
 * @param pw_area The starting coordinates and width/height of the area, in pokewalker coordinates
 * @param a Amoled struct containing data about the amoled, only use offset
 * @return The starting coords and width/height in amoled coordinates
 */
screen_area_t transform_pw_to_amoled(screen_area_t pw_area, amoled_t a) {
    screen_area_t amoled_area = {0};
    amoled_area.x = (SCREEN_HEIGHT - pw_area.height - pw_area.width)*SCREEN_SCALE + a.offset_x;
    amoled_area.y = (x)*SCREEN_SCALE + a.offset_y;
    amoled_area.width = pw_area.height * SCREEN_SCALE;
    amoled_area.height = pw_area.width * SCREEN_SCALE;
    return amoled_area;
}


/*
 * ==========================================
 * Driver functions
 * ==========================================
 */

void pw_screen_init() {
    uint8_t params[4] = {0};

    // HAL_OSPI_MspInit sets up the peripheral

    // Hardware reset
    HAL_GPIO_WritePin(SCREEN_RST_PORT, SCREEN_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(5); // ms
    HAL_GPIO_WritePin(SCREEN_RST_PORT, SCREEN_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(15); // ms
    HAL_GPIO_WritePin(SCREEN_RST_PORT, SCREEN_RST_PIN, GPIO_PIN_SET);

    HAL_Delay(50); // max 150ms in sleep-out

    params[0] = 0x00;
    amoled_send_cmd(CMD_SLEEP_OUT, 1, params);

    HAL_Delay(120);

    //params[0] = 0x01;
    //params[1] = 0x66;
    //amoled_send_cmd(CMD_WRITE_TE_LINE, 2, params);

    //params[0] = 0x00;
    //amoled_send_cmd(CMD_TE_ON, 1, params);

    // TODO: Check if 0x55 works again
    params[0] = 0xd5; // 16 bpp
    //params[0] = 0x55; // 565 encoded 16bpp
    amoled_send_cmd(CMD_PIXEL_FORMAT, 1, params);

    params[0] = 0x20;
    amoled_send_cmd(CMD_WRITE_CTRL_DSP1, 1, params);

    HAL_Delay(10);

    params[0] = 0x00;
    amoled_send_cmd(CMD_SET_BRIGHTNESS, 1, params);

    HAL_Delay(10);

    amoled_send_cmd(CMD_DISPLAY_ON, 0, params);

    HAL_Delay(10);

    params[0] = 0xff; // max brightness
    amoled_send_cmd(CMD_SET_BRIGHTNESS, 1, params);
    HAL_Delay(5);

    // Clear whole screen
    amoled_draw_block(0, 0, AMOLED_WIDTH, AMOLED_HEIGHT, 0x0000);

    // Find offset to center pw screen
    // note that pw is landscape, but amoled is portrait.
    // 4x scaled screen does not fit in amoled portrait
    int x_off = (AMOLED_WIDTH-SCREEN_SCALE*SCREEN_HEIGHT)/2;
    int y_off = (AMOLED_HEIGHT-SCREEN_SCALE*SCREEN_WIDTH)/2;
    amoled.offset_x = x_off;
    amoled.offset_y = y_off;

    amoled_send_cmd(CMD_NOP, 0, params);

    // set pokewalker draw area to pokewalker black
    amoled_draw_block(x_off, y_off, SCREEN_SCALE*SCREEN_HEIGHT, SCREEN_SCALE*SCREEN_WIDTH, colour_map[SCREEN_BLACK]);

}

void pw_screen_draw_img(pw_img_t *img, screen_pos_t x, screen_pos_t y) {

    // Put decoded, transformed image in `AMOLED_BUFFER`
    decode_img(img, AMOLED_BUFFER_LEN, AMOLED_BUFFER);

    // Transform image area to amoled coordinates
    screen_area_t amoled_area = transform_pw_to_amoled((screen_area_t){
        .x = x,
        .y = y,
        .width = img->width,
        .height = img->height,
    }, amoled);
    amoled_draw_buffer(
            //((SCREEN_HEIGHT-img->height-y)*SCREEN_SCALE)+amoled.offset_x, (x*SCREEN_SCALE)+amoled.offset_y,
            //img->height*SCREEN_SCALE, img->width*SCREEN_SCALE,
            amoled_area.x, amoled_area.y,
            amoled_area.width, amoled_area.height,
            2*amoled_area.width*amoled_area.height,
            AMOLED_BUFFER);
}

void pw_screen_clear_area(screen_pos_t x, screen_pos_t y,
                          screen_pos_t w, screen_pos_t h) {
    
    screen_area_t amoled_area = transform_pw_to_amoled((screen_area_t){
        .x = x,
        .y = y,
        .width = w,
        .height = h,
    }, amoled);

    amoled_draw_block(
        amoled_area.x, amoled_area.y,
        amoled_area.width, amoled_area.height,
        colour_map[SCREEN_WHITE]
    );
}

void pw_screen_draw_horiz_line(screen_pos_t x, screen_pos_t y,
                               screen_pos_t len, screen_colour_t c) {

    screen_area_t amoled_area = transform_pw_to_amoled((screen_area_t){
        .x = x,
        .y = y,
        .width = len,
        .height = 1,
    }, amoled);
    amoled_draw_block(
        amoled_area.x, amoled_area.y,
        amoled_area.width, amoled_area.height,
        colour_map[c]);
}

void pw_screen_draw_text_box(screen_pos_t x1, screen_pos_t y1,
                             screen_pos_t x2, screen_pos_t y2,
                             screen_colour_t c) {

    // assume y2 > y1 and x2 > x1
    size_t width = x2 - x1 + 1;
    size_t height = y2 - y1 + 1;

    screen_area_t amoled_area = {0}, pw_area = {0};

    // top bar
    pw_area = {.x = x1, .y = y1, .width = width, .height = 1};
    amoled_area = transform_pw_to_amoled(pw_area, amoled)
    amoled_draw_block(
        amoled_area.x, amoled_area.y,
        amoled_area.width, amoled_area.height,
        colour_map[c]);

    // bottom bar
    pw_area = {.x = x1, .y = y2, .width = width, .height = 1};
    amoled_area = transform_pw_to_amoled(pw_area, amoled)
    amoled_draw_block(
        amoled_area.x, amoled_area.y,
        amoled_area.width, amoled_area.height,
        colour_map[c]);

    // left bar
    pw_area = {.x = x1, .y = y1, .width = 1, .height = height};
    amoled_area = transform_pw_to_amoled(pw_area, amoled)
    amoled_draw_block(
        amoled_area.x, amoled_area.y,
        amoled_area.width, amoled_area.height,
        colour_map[c]);

    // right bar
    pw_area = {.x = x2, .y = y1, .width = 1, .height = height};
    amoled_area = transform_pw_to_amoled(pw_area, amoled)
    amoled_draw_block(
        amoled_area.x, amoled_area.y,
        amoled_area.width, amoled_area.height,
        colour_map[c]);
}

void pw_screen_clear() {
    screen_area_t amoled_area = transform_pw_to_amoled((screen_area_t){.x=0, .y=0, .width=SCREEN_WIDTH, .height=SCREEN_HEIGHT}, amoled);
    amoled_draw_block(
        amoled_area.x, amoled_area.y,
        amoled_area.width, amoled_area.height,
        colour_map[SCREEN_WHITE]
    );

}

void pw_screen_fill_area(screen_pos_t x, screen_pos_t y,
                         screen_pos_t w, screen_pos_t h, screen_colour_t c) {

    screen_area_t amoled_area = transform_pw_to_amoled((screen_area_t){.x=x, .y=y, .width=w, .height=h}, amoled);
    amoled_draw_block(
        amoled_area.x, amoled_area.y,
        amoled_area.width, amoled_area.height,
        colour_map[c]
    );
}

