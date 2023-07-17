#include "lvgl.h"

lv_obj_t *label;

void lvgl_display_init_task(void *pvParams)
{
    /* Change the active screen's background color */
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /* Create a white label, set its text and align it to the center */
    label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(label, &lv_font_montserrat_24, 0); // Font size 24
    lv_label_set_text(label, "Followers: 0");
    lv_obj_set_style_text_color(lv_scr_act(), lv_palette_main(LV_PALETTE_ORANGE), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    vTaskDelete(NULL);
}

void lvgl_display_update(int num)
{
    lv_label_set_text_fmt(label, "Followers: %d", num);
}