#ifndef _MENU_H_
#define _MENU_H_

#define MENU_SELECT_THEME   0
#define MENU_CONFIG        	1
#define MENU_SORT_GAMES		2
#define MENU_SHOW_THEME		3
#define MENU_Install_Theme  4
#define MENU_HOME			5
#define MENU_MANAGE_DEVICE	6
#define MENU_DOWNLOAD		7
#define MENU_ORIG_THEME     8
#define MENU_EXIT			9
#define MENU_EXIT_TO_MENU   10
#define Menu_Start_Themeing 11
#define MENU_MAKE_THEME     12
#define MENU_Select_Ios     13

#define MAXTHEMES		200

#define THEMEWII_VERSION		2.0
#define HOTSPOT_LEFT		MAXHOTSPOTS-2
#define HOTSPOT_RIGHT		MAXHOTSPOTS-1

#include "video.h"

// themelist buffer & variables
typedef struct{
	char* title;
    MRCtex* banner;
	u8 *region;
	u32 version;
	int type;
} ModTheme; //32 bytes!

ModTheme ThemeList[MAXTHEMES];

typedef struct{
	u8 *region;
	u32 version;
}CurthemeStats;

CurthemeStats curthemestats;

u32 Versionsys;
bool foundneek;



// Prototypes
int Menu_Loop(int);


#endif
