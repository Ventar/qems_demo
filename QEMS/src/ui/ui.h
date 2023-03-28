// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.2.2
// LVGL version: 8.2.0
// Project name: qems

#ifndef _QEMS_UI_H
#define _QEMS_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

#include "ui_events.h"
extern lv_obj_t * ui_Screen_Loading;
extern lv_obj_t * ui_S1P_Border;
extern lv_obj_t * ui_S1SP_Loading;
extern lv_obj_t * ui_S1L_Name;
extern lv_obj_t * ui_S1L_Info;
extern lv_obj_t * ui_S1I_CO2;
extern lv_obj_t * ui_Screen_Data;
extern lv_obj_t * ui_S2P_Header;
extern lv_obj_t * ui_S2I_Icon;
extern lv_obj_t * ui_S2L_Header;
extern lv_obj_t * ui_S2P_Content;
extern lv_obj_t * ui_S2L_Time;
extern lv_obj_t * ui_S2L_Date;
extern lv_obj_t * ui_S2L_Cost_Save;
extern lv_obj_t * ui_S2L_CO2_Save;
extern lv_obj_t * ui_S2L_Load;
extern lv_obj_t * ui_S2I_CO2_Save;
extern lv_obj_t * ui_S2I_Cost_Save;
extern lv_obj_t * ui_Screen_Settings;
extern lv_obj_t * ui_S3P_Border;
extern lv_obj_t * ui_S3L_IP_Name;
extern lv_obj_t * ui_S3L_IP_Data;
extern lv_obj_t * ui_S3L_WiFi_Name;
extern lv_obj_t * ui_S3L_WiFi_Data;
extern lv_obj_t * ui_S3B_Back;
extern lv_obj_t * ui_S3L_Back;
extern lv_obj_t * ui_S3B_Reboot;
extern lv_obj_t * ui_S3L_Reboot;
extern lv_obj_t * ui_S3B_WiFi_Reset;
extern lv_obj_t * ui_S3L_WiFi_Reset;
extern lv_obj_t * ui_S3B_Factory_Reset;
extern lv_obj_t * ui_S3L_Back1;
extern lv_obj_t * ui_S3P_Header;
extern lv_obj_t * ui_S3I_Icon;
extern lv_obj_t * ui_S3L_Header;
extern lv_obj_t * ui_Screen_WiFi;
extern lv_obj_t * ui_S4P_Border;
extern lv_obj_t * ui_S4L_Info;
extern lv_obj_t * ui_S4L_Info2;
extern lv_obj_t * ui_S4L_Info1;
extern lv_obj_t * ui_S4P_Header;
extern lv_obj_t * ui_S4I_Icon;
extern lv_obj_t * ui_S4L_Header;
extern lv_obj_t * ui_S4SP_Spinner;
extern lv_obj_t * ui_Screen_Upload;
extern lv_obj_t * ui_S5P_Border;
extern lv_obj_t * ui_S5L_Info_1;
extern lv_obj_t * ui_S4L_Info4;
extern lv_obj_t * ui_S5L_IP;
extern lv_obj_t * ui_S5P_Header;
extern lv_obj_t * ui_S5I_Icon;
extern lv_obj_t * ui_S5L_Header;
extern lv_obj_t * ui_S5SP_Spinner;
extern lv_obj_t * ui____initial_actions0;


LV_IMG_DECLARE(ui_img_leaf_green_png);    // assets\leaf_green.png
LV_IMG_DECLARE(ui_img_leaf_white_png);    // assets\leaf_white.png
LV_IMG_DECLARE(ui_img_co2_save_png);    // assets\co2_save.png
LV_IMG_DECLARE(ui_img_cost_save_png);    // assets\cost_save.png
LV_IMG_DECLARE(ui_img_gears_white_png);    // assets\gears_white.png
LV_IMG_DECLARE(ui_img_1164483863);    // assets\leaf-svgrepo-com.png


LV_FONT_DECLARE(ui_font_MonoFonto18);
LV_FONT_DECLARE(ui_font_Raleway16);
LV_FONT_DECLARE(ui_font_Raleway24);
LV_FONT_DECLARE(ui_font_Raleway20);
LV_FONT_DECLARE(ui_font_Raleway18);


void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif