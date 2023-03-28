/********************************************************************************************************************
 * LVGLDisplay
 *
 * Uses the LovyanGFX library in combination with the LGVL library to initialize an ILI9341 display with
 * an XT2046 touch panel. This class handles the hardware configuration to display the UI that is generated
 * and managed in the qems_ui.h file.
 *
 *******************************************************************************************************************/
#ifndef QEMS_DISPLAY_H_H
#define QEMS_DISPLAY_H_H

#define LGFX_USE_V1

#include <LovyanGFX.hpp>
#include <lvgl.h>

// ------------------------------------------------------------------------------------------------------------------
// C++ Class definition for the display configuration
// ------------------------------------------------------------------------------------------------------------------

/**
 * @brief lovayn GFX display configuration.
 */
class ILI9341Display : public lgfx::LGFX_Device {

  public:
    lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    lgfx::Touch_XPT2046 _touch_instance;

    ILI9341Display(void) {
        {
            auto cfg = _bus_instance.config();

            cfg.spi_host = VSPI_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 40000000;
            cfg.freq_read = 16000000;
            cfg.spi_3wire = false;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk = 18;
            cfg.pin_mosi = 23;
            cfg.pin_miso = 19;
            cfg.pin_dc = 21;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        {
            auto cfg = _panel_instance.config();

            cfg.pin_cs = 16;
            cfg.pin_rst = 17;
            cfg.pin_busy = -1;

            cfg.panel_width = 240;
            cfg.panel_height = 320;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            _panel_instance.config(cfg);
        }

        {
            auto cfg = _light_instance.config();

            cfg.pin_bl = 32;
            cfg.invert = false;
            cfg.freq = 44100;
            cfg.pwm_channel = 7;

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        {
            auto cfg = _touch_instance.config();

            cfg.x_min = 0;
            cfg.x_max = 239;
            cfg.y_min = 0;
            cfg.y_max = 319;
            cfg.pin_int = 34;
            cfg.bus_shared = true;
            cfg.offset_rotation = 0;

            cfg.spi_host = VSPI_HOST;
            cfg.freq = 1000000;
            cfg.pin_sclk = 18;
            cfg.pin_mosi = 23;
            cfg.pin_miso = 19;
            cfg.pin_cs = 22;

            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }

        setPanel(&_panel_instance);
    }
};

// ------------------------------------------------------------------------------------------------------------------
// Configuration values for the LGVL library
// ------------------------------------------------------------------------------------------------------------------
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * 10];


/**
 *  Set the touchscreen calibration data, the actual data for your display can be acquired using the Generic -> Touch_calibrate example from the TFT_eSPI
 * library
 */
static uint16_t tourchCalibrationData[] = {423, 3558, 445, 336, 3736, 3632, 3718, 318};

/**
 * Creates the display used by the programm
 */
static ILI9341Display display;

/**
 * @brief display specific (ILI9341) code to flush data to the display
 * */
static void ui_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    display.startWrite();
    display.setAddrWindow(area->x1, area->y1, w, h);
    // tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    display.writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
    display.endWrite();

    lv_disp_flush_ready(disp);
}

/**
 * @brief display specific (ILI9341 + XPT2046) code to read data from the touch pad.
 * */
static void ui_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data) {
    uint16_t touchX, touchY;

    bool touched = display.getTouch(&touchX, &touchY);

    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print("Data x ");
        Serial.println(touchX);

        Serial.print("Data y ");
        Serial.println(touchY);
    }
}

static void ui_disp_init() {
    display.begin();        /* TFT init */
    display.setRotation(3); /* Landscape orientation, flipped */
    Serial.println("TFT initialized...");

    display.setTouchCalibrate(tourchCalibrationData);
    Serial.println("Touch screen calibrated...");

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = ui_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = ui_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    Serial.println("LVGL callbacks initialized...");
}

#endif