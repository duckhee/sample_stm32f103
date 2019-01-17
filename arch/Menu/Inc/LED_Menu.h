#ifndef __LED_MENU_H__
#define __LED_MENU_H__

#include "main_menu.h"

#ifdef LED_MENU_LOCAL
#define LED_MENU_DEF
#else
#define LED_MENU_DEF   extern
#endif

LED_MENU_DEF int LED_main();
LED_MENU_DEF int LED_main_menu();

// 사용할 때 LED_MAIN_MENU()함수를 불러와서 사용한다.


#endif