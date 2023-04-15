/*
 * lcd.h
 *
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
#include "displaybitmaps.h"
#include "sm.h"

#define RF_SIGNAL_ROW  0
#define RF_SIGNAL_COL  0

#define RF_SIGNAL_WIDTH 20
#define RF_SIGNAL_HEIGHT 17

#define MESSAGE_BOX_ROW 0
#define MESSAGE_BOX_COL 50

#define MESSAGE_BOX_WIDTH 17
#define MESSAGE_BOX_HEIGHT 17


#define BAT_SYMBOL_ROW   0
#define BAT_SYMBOL_COL   100

#define BAT_SYMBOL_WIDTH 23
#define BAT_SYMBOL_HEIGHT 17

#define WIFI_COL 90
#define WIFI_ROW 0

#define WIFI_SYMBOL_WIDTH 8
#define WIFI_SYMBOL_HEIGHT 17

#define BLUETOOTH_COL 75
#define BLUETOOTH_ROW 0

#define BLUETOOTH_SYMBOL_WIDTH 8
#define BLUETOOTH_SYMBOL_HEIGHT 17

void highlight_menu();
void unify_switch_case1(int ptt);
void unify_switch_case2(int ptt);
void unify_switch_case3(int ptt);
void unify_switch_case4(int ptt);

void unify_switch_case5(int ptt);
void unify_switch_case6(int ptt);
void unify_switch_case7(int ptt);
void unify_switch_case8(int ptt);
void unify_switch_case9(int ptt);
void unify_switch_case10(int ptt);
void unify_switch_case11(int ptt);
void unify_switch_case12(int ptt);
void unify_switch_case13(int ptt);
void unify_switch_case14(int ptt);
void unify_switch_case15(int ptt);
void unify_switch_case16(int ptt);
void unify_back_cursor();

uint8_t unify_PTT_switch();
uint8_t unify_SQUELCH_switch();
uint8_t unify_EMERGENCY_switch();
uint8_t unify_display_switch();

void battery_monitaring_levels(const uint8_t level[]);
void current_monirting(void);
void unify_gps_cursor();
void unify_scr_opt();
void unify_gps_opt();
void unify_pre_store_screen();

void unify_lcd_init();
void unify_logo_screen();
void unify_custom_indicators();
//void unify_home_menu_screen1();
//void unify_home_menu_screen1(event_t msg);
void unify_scr_mainmenu_screen2();
void unify_scr_callmenu_screen3();
void unify_indi_radio_screen4();
void unify_scr_msgmenu_screen5();
void unify_Radio_id();

void unify_scr_info_menu();
void unify_scr_messgae(char *msg);
void unify_scr_opt_menu();
void unify_scr_mainmenu_black();
void unify_scr_info_opt();
void power_amplifer();

void unify_scr_backmenu();
//void unify_Radio_contacts_menu();


void unify_custom_indicators1(uint8_t Battery_full_bits);
void unify_call_cursor();
void unify_msg_cursor();
void clear_display();
void unify_next_page(void);
void unify_pre_store_cursor1(void);
void unify_wifi_cursor(void);
void unify_keypad_menu_screen(void);
void unify_pre_store_cursor2(void);
void unify_radio1_cursor(void);
void unify_radio2_cursor(void);
void unify_radio3_cursor(void);
void unify_radio4_cursor(void);
void unify_radio5_cursor(void);
void unify_group_cursor(void);
void unify_allcall_cursor(void);
void unify_allcall_cursor(void);
void unify_dialcall_cursor(void);
void unify_indi_cursor(void);
void PTT_ON(void);
void PTT_OFF(void);
void right_cursor(void);
void Emergency_ON(void);
void  Emergency_OFF(void);

void unify_home_screen(int ch,int squl,int ptt,int txpwr);

#endif /* INC_LCD_H_ */


/********************************* END OF FILE ********************************/
/******************************************************************************/

