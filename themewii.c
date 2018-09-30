#include <stdio.h>
#include <stdlib.h>
#include <ogcsys.h>
#include <string.h> //for parsing parameters
#include <ogc/isfs.h>
#include <errno.h>
#include <gccore.h>

#include "menu.h"
#include "video.h"
#include "wpad.h"
#include "gecko.h"

void __exception_setreload(int);

int main(int argc, char **argv){
	int mode = 1, ret;
	bool foundneek = false;
	u32 ios;
	
	InitGecko();
	USBGeckoOutput();
	gprintf("\n\nargc[%i] \n\n", argc);
	// Parse parameters
	if(argc>1){
		int i;
		for(i=1; i<argc; i++){
			if(strcmp("-i=", argv[i])==0){ // 249 real nand 56 neek nand
				ios=atoi(strchr(argv[i], '=')+1);
				gprintf("\n\nios [%i] \n\n", ios);
			}
			if(strcmp("-m=",argv[i])==0){	// sd = 1 usb = 2  neek = 3
				mode=atoi(strchr(argv[i], '=')+1);
				gprintf("\n\nmode [%i] \n\n", mode);
			}
		}
	}
	
	__exception_setreload(5);
	
	// Initialize system
	Video_Init();
	Wpad_Init();

	// Initialize console if DEBUG_MODE
	gprintf("themewii main mode(%d) \n");	

	// Menu loop
	ret = Menu_Loop(mode);
	
	if(ret == MENU_EXIT_TO_MENU)
	{
		__Draw_Message("Exiting to the System Menu ....",0);
		sleep(2);
		__Finish_ALL_GFX();
		SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
	}
	
	exit(0); // exit to hbc
}
