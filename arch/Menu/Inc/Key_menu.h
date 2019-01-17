#ifndef __KEY_MENU_H__
#define __KEY_MENU_H__

#include "menu.h"

#ifdef KEY_MENU_LOCAL
#define KEY_MENU_DEF     extern
#else
#define KEY_MENU_DEF
#endif


KEY_MENU_DEF int Command_Key_Main(int argc, char **argv);
KEY_MENU_DEF int Command_Key_Main_Menu(void);

#endif
