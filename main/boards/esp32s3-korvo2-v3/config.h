
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000
#define AUDIO_DEFAULT_OUTPUT_VOLUME 90

#define AUDIO_INPUT_REFERENCE    true

#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_16
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_45
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_9
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_10
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_8

#define AUDIO_CODEC_PA_PIN       GPIO_NUM_48
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_17
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_18
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR
#define AUDIO_CODEC_ES7210_ADDR  ES7210_CODEC_DEFAULT_ADDR

#define BUILTIN_LED_GPIO        GPIO_NUM_NC
#define BOOT_BUTTON_GPIO        GPIO_NUM_5
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

#define DISPLAY_SDA_PIN GPIO_NUM_NC
#define DISPLAY_SCL_PIN GPIO_NUM_NC
#define DISPLAY_WIDTH   280
#define DISPLAY_HEIGHT  240
#define DISPLAY_SWAP_XY true
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y true
#define BACKLIGHT_INVERT false

#define DISPLAY_OFFSET_X  20
#define DISPLAY_OFFSET_Y  0

#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_NC
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

#endif // _BOARD_CONFIG_H_
