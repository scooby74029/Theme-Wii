/* menu.c
 *
 * ThemeWii Wii theme installer based on the gui of mighty channels by scooby74029
 *
 * Triiforce(Mighty Channels) mod by Marc
 *
 * Copyright (c) 2009 The Lemon Man, Nicksasa & WiiPower
 *
 * Distributed under the terms of the GNU General Public License (v2)
 * See http://www.gnu.org/licenses/gpl-2.0.txt for more info.
 *
 *********************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gccore.h>
#include <ogc/lwp_watchdog.h>
#include <wiiuse/wpad.h>
#include <asndlib.h>
#include <network.h> //for network
#include <sys/errno.h>
#include <sys/stat.h> //for mkdir
#include <sys/types.h>
#include <sys/dir.h>
#include <malloc.h>
#include <ogcsys.h>
#include <dirent.h>
#include <unistd.h>

#include "tools.h"
#include "lz77.h"
#include "config.h"
#include "fat_mine.h"
#include "video.h"
#include "http.h"
#include "langs.h"
#include "wpad.h"
#include "menu.h"
#include "gecko.h"
#include "rijndael.h"
#include "sys.h"
#include "themedatabase.h"
#include "http.h"
#include "miniunz.h"
#include "unzip.h"
#include "fileops.h"
#include "tools2.h"


//Menu images
#include "themewii_arrows_png.h"
#include "themewii_background_png.h"
#include "themewii_container_png.h"
#include "themewii_container_wide_png.h"
#include "themewii_empty_png.h"
#include "themewii_loading_png.h"
#include "themewii_numbers_png.h"

#define MAX_TEXTURES	6
#define TEX_ARROWS		0
#define TEX_BACKGROUND	1
#define TEX_CONTAINER	2
#define TEX_EMPTY		3
#define TEX_LOADING		4
#define TEX_NUMBERS		5
static MRCtex* textures[MAX_TEXTURES];


// Constants
#define ARROWS_X		        24
#define ARROWS_Y		        210
#define ARROWS_WIDTH	        20 
#define THEMEWII_PATH		    "config/themewii/"
#define IMAGES_PREFIX		    "images"
#define THEMEWII_CONFIG_FILE	"themewii.cfg"

#define EMPTY			-1

#define BLACK	0x000000FF
#define YELLOW	0xFFFF00FF
#define WHITE	0xFFFFFFFF
#define ORANGE	0xeab000ff
#define RED    0xFF0000FF

#define KNVERSIONS    17

static u32 themecnt=0, defaultios = 249;
static s16* orden;

u8 commonkey[16] = { 0xeb, 0xe4, 0x2a, 0x22, 0x5e, 0x85, 0x93, 0xe4, 0x48,
     0xd9, 0xc5, 0x45, 0x73, 0x81, 0xaa, 0xf7
  };

// Variables for themewii
static u16 pages, page, maxPages;
static int selectedtheme=0, movingGame=-1;
static const char** lang;
static int loadingAnim=0;
static bool wideScreen=false;
static bool saveconfig=false;
static char tempString[256];
static int thememode = 0;
dirent_t *ent;

static FILE *fp;
static char *outdir;
const char *ImageDownloads[50] = {
"https://www.mediafire.com/convkey/d26e/b4qqm51p42sby6u6g.jpg",//"http://ge.tt/1BZEDnr2/v/30", //"https://image.ibb.co/dpw8u9/bleach.png",
"http://ge.tt/1BZEDnr2/v/34", //"https://image.ibb.co/mYrK1p/blue.png",
"https://image.ibb.co/mSgxnU/conduit.png",
"https://image.ibb.co/ijgK1p/constantine.png",
"https://image.ibb.co/g496gp/drwho.png",
"https://image.ibb.co/maz6gp/evildead.png",
"https://image.ibb.co/e2EHnU/fullmetal.png",
"https://image.ibb.co/bzbmgp/gaara.png",
"https://image.ibb.co/frrxnU/goldensun.png",
"https://image.ibb.co/ipA1E9/green.png",
"https://image.ibb.co/dLtcnU/halo.png",
"https://image.ibb.co/jOJA7U/hundred.png",
"https://image.ibb.co/nrgmgp/imports.png",
"https://image.ibb.co/bwaCMp/kingdomhearts.png",
"https://image.ibb.co/j2TA7U/luigi.png",
"https://image.ibb.co/gauTu9/martinabel.png",
"https://image.ibb.co/iSyXMp/metroid1.png",
"https://image.ibb.co/gHeYSU/metroid2.png",
"https://image.ibb.co/hw1f7U/mortal.png",
"https://image.ibb.co/gZd4Z9/okami.png",
"https://image.ibb.co/iFitSU/orange.png",
"https://image.ibb.co/iGFU1p/pink.png",
"https://image.ibb.co/fCrf7U/punchout.png",
"https://image.ibb.co/byzBE9/purple.png",
"https://image.ibb.co/iBrPZ9/red.png",
"https://image.ibb.co/g0Xyu9/saw.png",
"https://image.ibb.co/eM0jZ9/shadow.png",
"https://image.ibb.co/iaf7nU/simpsons.png",
"https://image.ibb.co/eFpNMp/storm.png",
"https://image.ibb.co/g4c91p/tmnt.png",
"https://image.ibb.co/nzLjZ9/tomb.png",
"https://image.ibb.co/e9QJu9/transformers.png",
"https://image.ibb.co/joyGgp/white.png",
"https://image.ibb.co/kceYSU/wolverine.png",
"https://image.ibb.co/mPq7nU/yellow.png",
"https://image.ibb.co/kWSbgp/zeldatp.png",
};

extern GXRModeObj *vmode;
extern u32* framebuffer;

const u8 COLS[]={3, 4};
#define ROWS 3
const u8 FIRSTCOL[]={136, 112};
#define FIRSTROW 96
const u8 SEPARACIONX[]={180, 136};
#define SEPARACIONY 112
const u8 ANCHOIMAGEN[]={154, 116};
#define ALTOIMAGEN 90

#define ALIGN32(x) (((x) + 31) & ~31)

typedef struct {
  u16 type;
  u16 name_offset;
  u32 data_offset; // == absolut offset från U.8- headerns början
  u32 size; // last included file num for directories
} U8_node;
 
typedef struct{
  u32 tag; // 0x55AA382D "U.8-"
  u32 rootnode_offset; // offset to root_node, always 0x20.
  u32 header_size; // size of header from root_node to end of string table.
  u32 data_offset; // offset to data -- this is rootnode_offset + header_size, aligned to 0x40.
  u8 zeroes[16];
}U8_archive_header;





char *getregion(u32 num);
char *getsysvernum(u32 num);
const char *getdevicename(int index);
void __Load_Config(void);
int __DownloadDBfile(char *ThemeFile);
int __DownloadDBpng();

void __Draw_Loading(void){
 	MRC_Draw_Tile(575,390, textures[TEX_LOADING], 24, loadingAnim);
	MRC_Render_Box(575,390);

	loadingAnim+=1;
	if(loadingAnim==16)
		loadingAnim=0;
}

void __Draw_Page(int selected){
	int i, j, x, y, containerWidth, theme;

	containerWidth=textures[TEX_CONTAINER]->width/2;

	// Background
	MRC_Draw_Texture(0, 0, textures[TEX_BACKGROUND]);
	
	sprintf(tempString, "IOS_%d v_%d", IOS_GetVersion(), IOS_GetRevision());
	MRC_Draw_String(100, 430, WHITE, tempString);
	
	sprintf(tempString, "System_Menu v%s_%s", getsysvernum(Versionsys), getregion(Versionsys));
	MRC_Draw_String(420, 430, WHITE, tempString);
	
	if(themecnt==0 || pages==0){
		return;
	}

	// themes
	theme=COLS[wideScreen]*ROWS*page;
	y=FIRSTROW;
	for(i=0; i<ROWS; i++){
		x=FIRSTCOL[wideScreen];
		for(j=0; j<COLS[wideScreen]; j++){
			if(movingGame!=theme){
				if(orden[theme]==EMPTY){
					MRC_Draw_Texture(x, y, textures[TEX_EMPTY]);
					MRC_Draw_Tile(x, y, textures[TEX_CONTAINER], containerWidth, 0);
				}else if(selected==i*COLS[wideScreen]+j){
					MRC_Draw_Texture(x, y, ThemeList[orden[theme]].banner);
					MRC_Draw_Tile(x, y, textures[TEX_CONTAINER], containerWidth, 1);
				}else{
					MRC_Draw_Texture(x, y, ThemeList[orden[theme]].banner);
					MRC_Draw_Tile(x, y, textures[TEX_CONTAINER], containerWidth, 0);
				}
			}
			theme++;
			x+=SEPARACIONX[wideScreen];
		}
		y+=SEPARACIONY;
	}

	// Page number
	x=(page+1<10? 300 : 292);
	sprintf(tempString, "%d", page+1);
	for(i=0; i<strlen(tempString); i++){
		MRC_Draw_Tile(x, 404, textures[TEX_NUMBERS], 8, tempString[i]-48);
		x+=8;
	}
	MRC_Draw_Tile(x, 404, textures[TEX_NUMBERS], 8, 10);
	x+=10;
	sprintf(tempString, "%d", maxPages);
	for(i=0; i<strlen(tempString); i++){
		MRC_Draw_Tile(x, 404, textures[TEX_NUMBERS], 8, tempString[i]-48);
		x+=8;
	}
	//MRC_Draw_String(300, 404, 0x808080FF, tempString);
	//MRC_Draw_String(310, 404, 0xd0d0d0FF, tempString);
	//sprintf(tempString, "hotspot=%d", selected);
	//MRC_Draw_String(40, 444, 0xFFFF00FF, tempString);

	if(movingGame>-1){
		if(orden[movingGame]==EMPTY){
			MRC_Draw_Texture(Wpad_GetWiimoteX(), Wpad_GetWiimoteY(), textures[TEX_EMPTY]);
		}else{
			MRC_Draw_Texture(Wpad_GetWiimoteX(), Wpad_GetWiimoteY(), ThemeList[orden[movingGame]].banner);
		}
	}

	// Arrows
	MRC_Draw_Tile(ARROWS_X, ARROWS_Y, textures[TEX_ARROWS], ARROWS_WIDTH, 0+(page>0)+(page>0 && selected==HOTSPOT_LEFT));
	MRC_Draw_Tile(640-ARROWS_X, ARROWS_Y, textures[TEX_ARROWS], ARROWS_WIDTH, 3+(page+1<maxPages)+(page+1<maxPages && selected==HOTSPOT_RIGHT));
}

void __Draw_Button(int hot, const char* text, bool selected){
	hotSpot button=Wpad_GetHotSpotInfo(hot);
	int textX=button.x+(button.width-strlen(text)*8)/2;
	u32 color;
	if(selected){
		MRC_Draw_Box(button.x, button.y, button.width, button.height/2, 0x3c7291ff);
		MRC_Draw_Box(button.x, button.y+button.height/2, button.width, button.height/2, 0x2d6483ff);
		color=WHITE;
	}else{
		MRC_Draw_Box(button.x, button.y, button.width, button.height/2, 0xe3e3e3ff);
		MRC_Draw_Box(button.x, button.y+button.height/2, button.width, button.height/2, 0xd8d8d8ff);
		MRC_Draw_Box(button.x, button.y, button.width, 1, 0xb6b4c5ff);
		MRC_Draw_Box(button.x, button.y+button.height-1, button.width, 1, 0xb6b4c5ff);
		MRC_Draw_Box(button.x, button.y, 1, button.height, 0xb6b4c5ff);
		MRC_Draw_Box(button.x+button.width-1, button.y, 1, button.height, 0xb6b4c5ff);
		color=0x404040ff;
	}
	MRC_Draw_String(textX, button.y+button.height/2-8, color, text);
}

void __Draw_Window(int width, int height, const char* title){
	int x=(640-width)/2;
	int y=(480-height)/2-32;
	__Draw_Page(-1);
	MRC_Draw_Box(0, 0, 640, 480, BLACK);

	MRC_Draw_Box(x, y, width, 32, YELLOW);
	MRC_Draw_Box(x, y+32, width, height, WHITE-0x20);

	MRC_Draw_String(x+(width-strlen(title)*8)/2, y+8, BLACK, title);
}

void __Draw_Message(const char* title, int ret){
	int i;

	MRC_Draw_Texture(0, 0, textures[TEX_BACKGROUND]);
	sprintf(tempString, "IOS_%d v_%d", IOS_GetVersion(), IOS_GetRevision());
	MRC_Draw_String(100, 430, WHITE, tempString);
	
	sprintf(tempString, "System_Menu v%s_%s", getsysvernum(Versionsys), getregion(Versionsys));
	MRC_Draw_String(420, 430, WHITE, tempString);
	MRC_Draw_Box(0, 150, 640, 48, WHITE-0x20);
	for(i=0;i<16;i++){
		MRC_Draw_Box(0, 200-16+i, 640, 1, i*2);
		MRC_Draw_Box(0, 200+48+16-i, 640, 1, i*2);
	}
	MRC_Draw_String((640-strlen(title)*8)/2, 165, BLACK, title);

	if(ret<0){
		sprintf(tempString, "ret=%d", ret);
		MRC_Draw_String(540, 216, 0xff0000ff, tempString);
	}

	MRC_Render_Screen();
	
	if(ret!=0)
		Wpad_WaitButtons();
}
#define QUESTION_BUTTON_X			100
#define QUESTION_BUTTON_Y			240
#define QUESTION_BUTTON_SEPARATION	32
#define QUESTION_BUTTON_WIDTH		200
#define QUESTION_BUTTON_HEIGHT		48
bool __Question_Window(const char* title, const char* text, const char* a1, const char* a2){
	int i, hotSpot, hotSpotPrev;
	bool ret=false, repaint=true;

	// Create/restore hotspots
	Wpad_CleanHotSpots();
	for(i=0; i<2; i++)
		Wpad_AddHotSpot(i,
			QUESTION_BUTTON_X+i*(QUESTION_BUTTON_WIDTH+QUESTION_BUTTON_SEPARATION),
			QUESTION_BUTTON_Y,
			QUESTION_BUTTON_WIDTH,
			QUESTION_BUTTON_HEIGHT,
			i, i,
			!i, !i
		);


	__Draw_Window(512, 128, title);
	MRC_Draw_String(100, 200, BLACK, text);

	// Loop
	hotSpot=hotSpotPrev=-1;

	ret=MENU_SELECT_THEME;
	for(;;){
		hotSpot=Wpad_Scan();

		// If hot spot changed
		if((hotSpot!=hotSpotPrev && hotSpot<2) || repaint){
			hotSpotPrev=hotSpot;

			__Draw_Button(0, a1, hotSpot==0);
			__Draw_Button(1, a2, hotSpot==1);

			repaint=false;
		}
		MRC_Draw_Cursor(Wpad_GetWiimoteX(), Wpad_GetWiimoteY(), 0);

		if(((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A)) && hotSpot!=-1){
			if(hotSpot==0)
				ret=true;
			break;
		}
	}

	return ret;
}


void findnumpages(void){
	int i;
	maxPages=0;
	for(i=MAXTHEMES-1; i>-1; i--){
		if(orden[i]!=EMPTY){
			break;
		}
	}

	while(maxPages*COLS[wideScreen]*ROWS<=i){
		maxPages++;
	}

	if(movingGame!=-1)
		maxPages++;

	if(maxPages>50)
		maxPages=50;
}
/*void __Save_Changes(void){
	int i, j, configsize;
	unsigned char *outBuffer;

	configsize=1+15+themecnt*(2+4+7+3); //1=cfgversion + 15=reserved   +   (2=position   4=id   7=config   3=reserved)=16
	//#ifdef DEBUG_MODE
	gprintf("Saving changes... (%d bytes)\n", configsize);
	//#endif
	outBuffer=allocate_memory(configsize);

	for(i=0; i<16; i++)
		outBuffer[i]=0;
	outBuffer[0]=THEMEWII_VERSION;

	j=16;
	for(i=0; i<MAXTHEMES; i++){
		__Draw_Loading();
		if(orden[i]!=EMPTY){
			outBuffer[j]=i%256;
			outBuffer[j+1]=i/256;
			outBuffer[j+2]=ThemeList[orden[i]].id[0];
			outBuffer[j+3]=ThemeList[orden[i]].id[1];
			outBuffer[j+4]=ThemeList[orden[i]].id[2];
			outBuffer[j+5]=ThemeList[orden[i]].id[3];

			outBuffer[j+6]=ThemeList[orden[i]].videoMode;
			outBuffer[j+7]=ThemeList[orden[i]].videoPatch;
			outBuffer[j+8]=ThemeList[orden[i]].language;
			outBuffer[j+9]=ThemeList[orden[i]].hooktype;
			outBuffer[j+10]=ThemeList[orden[i]].ocarina;
			outBuffer[j+11]=ThemeList[orden[i]].debugger;
			outBuffer[j+12]=ThemeList[orden[i]].bootMethod;
			outBuffer[j+13]=0;
			outBuffer[j+14]=0;
			outBuffer[j+15]=0;
			j+=16;
		}
	}

	// Save changes to FAT
	Fat_SaveFile(THEMEWII_PATH THEMEWII_CONFIG_FILE, (void *)&outBuffer, configsize);

	saveconfig=false;
}*/



/* Constants */
#define MAX_FILELIST_LEN	65535
#define MAX_FILEPATH_LEN	256

char *Sversion(u32 num){
	switch(num){
		case 0: return "00000042";// usa
		break;
		case 1: return "00000072";
		break;
		case 2: return "0000007b";
		break;
		case 3: return "00000087";
		break;
		case 4: return "00000097";// usa
		break;
		case 5: return "00000045";// pal
		break;
		case 6: return "00000075";
		break;
		case 7: return "0000007e";
		break;
		case 8: return "0000008a";
		break;
		case 9: return "0000009a";// pal
		break;
		case 10: return "00000040";// jpn
		break;
		case 11: return "00000070";
		break;
		case 12: return "00000078";
		break;
		case 13: return "00000084";
		break;
		case 14: return "00000094";// jpn
		break;
		case 15: return "0000008d";// kor
		break;
		case 16: return "00000081";
		break;
		case 17: return "0000009d";// kor
		break;
		default: return "UNK";
		break;
	}
}

char *getapplist(int j){
	switch(j){
		case 1: return "00000042";// usa
		break;
		case 2: return "00000072";
		break;
		case 3: return "0000007b";
		break;
		case 4: return "00000087";
		break;
		case 5: return "00000097";// usa
		break;
		case 6: return "00000045";// pal
		break;
		case 7: return "00000075";
		break;
		case 8: return "0000007e";
		break;
		case 9: return "0000008a";
		break;
		case 10: return "0000009a";// pal
		break;
		case 11: return "00000040";// jpn
		break;
		case 12: return "00000070";
		break;
		case 13: return "00000078";
		break;
		case 14: return "00000084";
		break;
		case 15: return "00000094";// jpn
		break;
		case 16: return "0000008d";// kor
		break;
		case 17: return "00000081";
		break;
		case 18: return "0000009d";// kor
		break;
		default: return "UNK";
		break;
	}
}
char *getappname(u32 Versionsys){
	switch(Versionsys){
		case 289: return "00000042";// usa
		break;
		case 417: return "00000072";
		break;
		case 449: return "0000007b";
		break;
		case 481: return "00000087";
		break;
		case 513: return "00000097";// usa
		break;
		case 290: return "00000045";// pal
		break;
		case 418: return "00000075";
		break;
		case 450: return "0000007e";
		break;
		case 482: return "0000008a";
		break;
		case 514: return "0000009a";// pal
		break;
		case 288: return "00000040";// jpn
		break;
		case 416: return "00000070";
		break;
		case 448: return "00000078";
		break;
		case 480: return "00000084";
		break;
		case 512: return "00000094";// jpn
		break;
		case 486: return "0000008d";// kor
		break;
		case 454: return "00000081";
		break;
		case 518: return "0000009d";// kor
		break;
		default: return "UNK";
		break;
	}
}

char *Sversion2(u32 num){
	switch(num){
		case 0:{
			//curthemestats.version = 32;
			//curthemestats.region = (u8*)85;
			return "00000042.app";// usa
		}
		break;
		case 1:{
			//curthemestats.version = 40;
			//curthemestats.region = (u8*)85;
			return "00000072.app";
		}
		break;
		case 2:{
			//curthemestats.version = 41;
			//curthemestats.region = (u8*)85;
			return "0000007b.app";
		}
		break;
		case 3:{
			//curthemestats.version = 42;
			//curthemestats.region = (u8*)85;
			return "00000087.app";
		}
		break;
		case 4:{
			//curthemestats.version = 43;
			//curthemestats.region = (u8*)85;
			return "00000097.app";// usa
		}
		break;
		case 5:{
			//curthemestats.version = 32;
			//curthemestats.region = (u8*)69;
			return "00000045.app";// pal
		}
		break;
		case 6:{
			//curthemestats.version = 40;
			//curthemestats.region = (u8*)69;
			return "00000075.app";
		}
		break;
		case 7:{
			//curthemestats.version = 41;
			//curthemestats.region = (u8*)69;
			return "0000007e.app";
		}
		break;
		case 8:{
			//curthemestats.version = 42;
			//curthemestats.region = (u8*)69;
			return "0000008a.app";
		}
		break;
		case 9:{
			//curthemestats.version = 43;
			//curthemestats.region = (u8*)69;
			return "0000009a.app";// pal
		}
		break;
		case 10:{
			//curthemestats.version = 32;
			//curthemestats.region = (u8*)74;
			return "00000040.app";// jpn
		}
		break;
		case 11:{
			//curthemestats.version = 40;
			//curthemestats.region = (u8*)74;
			return "00000070.app";
		}
		break;
		case 12:{
			//curthemestats.version = 41;
			//curthemestats.region = (u8*)74;
			return "00000078.app";
		}
		break;
		case 13:{
			//curthemestats.version = 42;
			//curthemestats.region = (u8*)74;
			return "00000084.app";
		}
		break;
		case 14:{
			//curthemestats.version = 43;
			//curthemestats.region = (u8*)74;
			return "00000094.app";// jpn
		}
		break;
		case 15:{
			//curthemestats.version = 41;
			//curthemestats.region = (u8*)75;
			return "0000008d.app";// kor
		}
		break;
		case 16:{
			//curthemestats.version = 42;
			//curthemestats.region = (u8*)75;
			return "00000081.app";
		}
		break;
		case 17:{
			//curthemestats.version = 43;
			//curthemestats.region = (u8*)75;
			return "0000009d.app";// kor
		}
		break;
		default: return "UNK";
		break;
	}
}



u32 checkcustom(u32 m){
	s32 rtn;
	u32 nandfilecnt,j;
	char *CHECK;
	dirent_t *filelist;
			
		rtn = getdir("/title/00000001/00000002/content",&filelist,&nandfilecnt);
		gprintf("rtn(%d) filecnt =%d \n",rtn,nandfilecnt);
		int k;
		for(k = 0;k <nandfilecnt ;k++){
			gprintf("name = %s k=%d \n",filelist[k].name,k);
			for(j = 0;j < KNVERSIONS;j++){
				CHECK = Sversion2(j);
				gprintf("check =%s j = %d\n",CHECK,j);
				if(strcmp(filelist[k].name,CHECK) != 0)
					continue;
				else{
					gprintf("equal j = %d \n",j);
					gprintf("check = %s \n",CHECK);
					switch(j){
						case 0: return 289;
						break;
						case 1: return 417;
						break;
						case 2: return 449;
						break;
						case 3: return 481;
						break;
						case 4: return 513;
						break;
						case 5: return 290;
						break;
						case 6: return 418;
						break;
						case 7: return 450;
						break;
						case 8: return 482;
						break;
						case 9: return 514;
						break;
						case 10: return 288;
						break;
						case 11: return 416;
						break;
						case 12: return 448;
						break;
						case 13: return 480;
						break;
						case 14: return 512;
						break;
						case 15: return 454;
						break;
						case 16: return 486;
						break;
						case 17: return 518;
						break;
						
					}
				}
			}
			if(k >= 6)
				break;
		}
	return 0;
}
	
bool checknandapp(){
	gprintf("check nandapp():\n");
	switch(Versionsys)
	{
		case 288:{
			curthemestats.version = 32;
			curthemestats.region = (u8*)74;
		}
		break;
		case 289:{
			curthemestats.version = 32;
			curthemestats.region = (u8*)85;
		}
		break;
		case 290:{
			curthemestats.version = 32;
			curthemestats.region = (u8*)69;
		}
		break;
		case 416:{
			curthemestats.version = 40;
			curthemestats.region = (u8*)74;
		}
		break;
		case 417:{
			curthemestats.version = 40;
			curthemestats.region = (u8*)85;
		}
		break;
		case 418:{
			curthemestats.version = 40;
			curthemestats.region = (u8*)69;

		}
		break;
		case 448:{
			curthemestats.version = 41;
			curthemestats.region = (u8*)74;
		}
		break;
		case 449:{
			curthemestats.version = 41;
			curthemestats.region = (u8*)85;
		}
		break;
		case 450:{
			curthemestats.version = 41;
			curthemestats.region = (u8*)69;
		}
		break;
		case 454:{
			curthemestats.version = 41;
			curthemestats.region = (u8*)75;
		}
		break;
		case 480:{
			curthemestats.version = 42;
			curthemestats.region = (u8*)74;
		}
		break;
		case 481:{
			curthemestats.version = 42;
			curthemestats.region = (u8*)85;
		}
		break;
		case 482:{
			curthemestats.version = 42;
			curthemestats.region = (u8*)69;
		}
		break;
		case 486:{
			curthemestats.version = 42;
			curthemestats.region = (u8*)75;
		}
		break;
		case 512:{
			curthemestats.version = 43;
			curthemestats.region = (u8*)74;
		}
		break;
		case 513:{
			curthemestats.version = 43;
			curthemestats.region = (u8*)85;
		}
		break;
		case 514:{
			curthemestats.version = 43;
			curthemestats.region = (u8*)69;
		}
		break;
		case 518:{
			curthemestats.version = 43;
			curthemestats.region = (u8*)75;
		}
		break;
		default:
			gprintf("default case going to custom sysmenu \n");
			return 0;
		break;
	}
	//gprintf("cur theme .region(%c)  .version(%d) \n",curthemestats.region,curthemestats.version);
	
	return 1;
}
char *getdownloadregion(u32 num){
    switch(num)
    {
    case 289:
    case 417:
    case 449:
    case 481:
    case 513:
        return "U";
        break;
    case 290:
    case 418:
    case 450:
    case 482:
    case 514:
        return "E";
        break;
    case 288:
    case 416:
    case 448:
    case 480:
    case 512:
        return "J";
        break;
    case 486:
    case 454:
    case 518:
        return "K";
        break;
    default:
        return "UNK";
        break;
    }
}
char *getregion(u32 num){
    switch(num)
    {
    case 289:
    case 417:
    case 449:
    case 481:
    case 513:
        return "Usa";
        break;
    case 290:
    case 418:
    case 450:
    case 482:
    case 514:
        return "Pal";
        break;
    case 288:
    case 416:
    case 448:
    case 480:
    case 512:
        return "Jpn";
        break;
    case 486:
    case 454:
    case 518:
        return "Kor";
        break;
    default:
        return "UNK";
        break;
    }
}
char *getsysvernum(u32 num){
    switch(num)
    {
    case 288:
    case 289:
    case 290:
        return "3.2";
        break;
    case 416:
    case 417:
    case 418:
        return "4.0";
        break;
    case 448:
    case 449:
    case 450:
	case 454:
        return "4.1";
        break;
    case 480:
    case 481:
    case 482:
	case 486:
        return "4.2";
        break;
    case 512:
    case 513:
    case 514:
	case 518:
        return "4.3";
        break;
    default:
        return "UNK";
        break;
    }
}


u32 GetSysMenuVersion(){
    //Get sysversion from TMD
    u64 TitleID = 0x0000000100000002LL;
    u32 tmd_size;
    s32 r = ES_GetTMDViewSize(TitleID, &tmd_size);
    if(r<0)
    {
        gprintf("error getting TMD views Size. error %d\n",r);
        return 0;
    }

    tmd_view *rTMD = (tmd_view*)memalign( 32, (tmd_size+31)&(~31) );
    if( rTMD == NULL )
    {
        gprintf("error making memory for tmd views\n");
        return 0;
    }
    memset(rTMD,0, (tmd_size+31)&(~31) );
    r = ES_GetTMDView(TitleID, (u8*)rTMD, tmd_size);
    if(r<0)
    {
        gprintf("error getting TMD views. error %d\n",r);
        free( rTMD );
        return 0;
    }
    u32 version = rTMD->title_version;
    if(rTMD)
    {
        free(rTMD);
    }
    return version;
}

int getcurrentregion(){
	int ret = 0;
	
	if(curthemestats.region == (u8*)69)
		ret = 1;
	else if(curthemestats.region == (u8*)74)
		ret = 2;
	else if(curthemestats.region == (u8*)75)
		ret = 3;
	else if(curthemestats.region == (u8*)85)
		ret = 4;
	else 
		ret = -97;
		
	return ret;
}
int getinstallregion(){
	int ret = 0;
	ModTheme *Thetheme = &ThemeList[orden[selectedtheme]];
	
	if(Thetheme->region == (u8*)69)
		ret = 1;
	else if(Thetheme->region == (u8*)74)
		ret = 2;
	else if(Thetheme->region == (u8*)75)
		ret = 3;
	else if(Thetheme->region == (u8*)85)
		ret = 4;
	else
		ret = -99;
		
	return ret;
}
s32 filelist_retrieve(int i){
	char *filelist = memalign(32, 256);
    char *dirpath = memalign(32, 256);
    //char dirpath2[ISFS_MAXPATH + 1];
    //char *ptr = filelist;
    //struct stat filestat;
    DIR *dir;
    s32 start = 0;


    /* Generate dirpath */

    
     sprintf(dirpath, "Fat:/themes");
     gprintf("Path: %s\n",dirpath);
    
	themecnt = 0;
	int fu; 
	
	for(fu = 0;fu < MAXTHEMES;fu++){
		if(DBThemelist[fu] == NULL)
			break;
		themecnt+=1;
	}
	//cnt2 = themecnt;
	int ff;
	for(ff = 0;ff < themecnt;ff++){
		ThemeList[ff].title = DBThemelist[ff];
		ThemeList[ff].type = 10;
		gprintf("theme =%s .type%d %d \n",ThemeList[ff].title, ThemeList[ff].type,ff);
	}
	
    gprintf("\n\nthemecnt [%i] \n\n",themecnt);
    dir = opendir(dirpath);
    if (!dir)
    {
        gprintf("could not open dir(%s)\n",dirpath);
        return -1;
    }
    
	u32 x = 0, y;
    struct dirent *entry = NULL;
    
    while((entry = readdir(dir))) // If we get EOF, the expression is 0 and
                                     // the loop stops. 
    {
		if(strncmp(entry->d_name, ".", 1) != 0 && strncmp(entry->d_name, "..", 2) != 0)
		x+=1;
    }
	themecnt  = themecnt + x;
	
	
	gprintf("\n\nthemecnt [%i] ff[%i] \n\n",themecnt,ff); 
    
    closedir(dir);
	ent = allocate_memory(sizeof(dirent_t) * x);
	x = 0;
	dir = opendir(dirpath);
	while((entry = readdir(dir)))
	{
		gprintf("start next while \n");
		if(strncmp(entry->d_name, ".", 1) != 0 && strncmp(entry->d_name, "..", 2) != 0)
		{	
			strcpy(ent[x].name, entry->d_name);
			ThemeList[ff].title = ent[x].name;
			ThemeList[ff].type = 20;
			gprintf("ent[x].name %s \n",ent[x].name);
			gprintf("ThemeList[x].title %s \n",ThemeList[ff].title);
			x += 1;
			ff += 1;
		}
	}
	
	closedir(dir);
	
	gprintf("x = %i themecnt = %i \n",x,themecnt);
	int cnt;
	
	for(cnt = 0; cnt < themecnt - 1;) {
		gprintf("\n\nThemeList[cnt].title[%s] \n\n", ThemeList[cnt].title);
		cnt++;
	}
    return 0;
}


void __Load_Config(void){
	int ret, i, j, k;
	s16* posiciones=allocate_memory(sizeof(u16)*themecnt);

	orden=allocate_memory(sizeof(u16)*MAXTHEMES);

	for(i=0; i<themecnt; i++)
		posiciones[i]=-1;

	
	// Ordenar los juegos segun archivo de configuracion
	char *archivoLeido=NULL;

	ret = Fat_ReadFile(THEMEWII_PATH THEMEWII_CONFIG_FILE, (void *)&archivoLeido);

	if(ret>0){
		// Parse config file
		i=16;
		while(i<ret){
			j=archivoLeido[i]+archivoLeido[i+1]*256;

			for(k=0; k<4; k++)
				tempString[k]=archivoLeido[i+2+k];
			tempString[4]='\0';

			for(k=0; k<themecnt; k++){
				/*if(strcmp(tempString, ThemeList[k].id)==0){
					//printf("%s found at %d\n", tempString, k);
					ThemeList[k].videoMode=archivoLeido[i+6];
					ThemeList[k].videoPatch=archivoLeido[i+7];
					ThemeList[k].language=archivoLeido[i+8];
					ThemeList[k].hooktype=archivoLeido[i+9];
					ThemeList[k].ocarina=archivoLeido[i+10];
					ThemeList[k].debugger=archivoLeido[i+11];
					ThemeList[k].bootMethod=archivoLeido[i+12];
					//ThemeList[k].ios=archivoLeido[i+13];

					posiciones[k]=j;
					break;
				}*/
			}
			i+=16;
		}
		free(archivoLeido);
	}

	
	//Initialize empty
	for(i=0; i<MAXTHEMES; i++)
		orden[i]=EMPTY;

	//Place games
	for(i=0; i<themecnt; i++)
		if(posiciones[i]!=-1)
			orden[posiciones[i]]=i;

	//Check if some game was not placed
	for(i=0; i<themecnt; i++){
		if(posiciones[i]==-1){
			for(j=0; j<MAXTHEMES; j++){
				if(orden[j]==EMPTY){
					posiciones[i]=j;
					break;
				}
			}
		}
		orden[posiciones[i]]=i;
	}
	free(posiciones);

	selectedtheme=0;
	page=0;
	pages=0;
	findnumpages();
}

void __Free_Channel_Images(void){
	int i;

	for(i=0; i<MAXTHEMES; i++){
		//printf("%d: %d\n", i, orden[i]);

		if(i==pages*COLS[wideScreen]*ROWS)
			break;
		if(orden[i]!=EMPTY){
			MRC_Free_Texture(ThemeList[orden[i]].banner);
		}
	}

	pages=0;
	page=0;
}

void __Finish_ALL_GFX(void){
	int i;

	__Free_Channel_Images();

	for(i=0; i<MAX_TEXTURES; i++){
		MRC_Free_Texture(textures[i]);
	}
	MRC_Finish();

	saveconfig=false;
}



void __Load_Images_From_Page(void){
	void *imgBuffer=NULL;
	int i, max, ret, theme;

	//#ifdef DEBUG_MODE
	gprintf("Loading images...\n");
	//#endif

	max=COLS[wideScreen]*ROWS;


	for(i=0; i<max; i++){
		theme=orden[max*pages+i];
		if(theme!=EMPTY){
			__Draw_Loading();

			// Load image from FAT
			if(ThemeList[theme].type == 20)
				sprintf(tempString,"Fat:/" THEMEWII_PATH IMAGES_PREFIX "/%s.png", ThemeList[theme].title);
			if(ThemeList[theme].type == 10)
				sprintf(tempString,"Fat:/" THEMEWII_PATH "DBimages/%sMain.png", DBlistpng[theme]);
			ret=Fat_ReadFile(tempString, &imgBuffer);
			//gprintf("ret from fat read images %d\n",ret);
			
			// Decode image
			if(ret>0){
				ThemeList[theme].banner=MRC_Load_Texture(imgBuffer);
				free(imgBuffer);
			}
			else{
				ThemeList[theme].banner=__Create_No_Banner(ThemeList[theme].title, ANCHOIMAGEN[wideScreen], ALTOIMAGEN);
			}

			MRC_Resize_Texture(ThemeList[theme].banner, ANCHOIMAGEN[wideScreen], ALTOIMAGEN);
			__MaskBanner(ThemeList[theme].banner);
			MRC_Center_Texture(ThemeList[theme].banner, 1);
		}
		//MRC_Draw_Texture(64, 440, configuracionJuegos[theme].banner);
	}
	pages++;
}




void __Load_Skin_From_FAT(void){
	const char* fileNames[MAX_TEXTURES]={
		"_arrows", "_background", (wideScreen? "_container_wide" : "_container"), "_empty",
		"_loading", "_numbers"};
	const u8* defaultTextures[MAX_TEXTURES]={
		themewii_arrows_png, themewii_background_png, (wideScreen? themewii_container_wide_png : themewii_container_png), themewii_empty_png,
		themewii_loading_png, themewii_numbers_png};

	int i, ret;
	char *imgData = NULL;

	for(i=0; i<MAX_TEXTURES; i++){
		sprintf(tempString, THEMEWII_PATH "themewii%s.png", fileNames[i]);
		ret = Fat_ReadFile(tempString, (void*)&imgData);
		if(ret>0){
			textures[i]=MRC_Load_Texture(imgData);
			free(imgData);
		}else{
			textures[i]=MRC_Load_Texture((void *)defaultTextures[i]);
		}
	}


	//Resize containers if widescreen
	if(wideScreen){
		MRC_Resize_Texture(textures[TEX_EMPTY], ANCHOIMAGEN[1], ALTOIMAGEN);
	}
	__MaskBanner(textures[TEX_EMPTY]);
	MRC_Center_Texture(textures[TEX_ARROWS], 6);
	MRC_Center_Texture(textures[TEX_CONTAINER], 2);
	MRC_Center_Texture(textures[TEX_EMPTY], 1);
}


/* Constant */
#define BLOCK_SIZE	0x1000
#define CHUNKS 1000000

/* Macros */
#define round_up(x,n)	(-(-(x) & -(n)))

u32 filesize(FILE * file){
	u32 curpos, endpos;
	
	if(file == NULL)
		return 0;
	
	curpos = ftell(file);
	fseek(file, 0, 2);
	endpos = ftell(file);
	fseek(file, curpos, 0);
	
	return endpos;
}

s32 InstallFile(FILE * fp){

	char * data;
	s32 ret, nandfile, ios = 2;
	u32 length = 0,numchunks, cursize, i;
	char filename[ISFS_MAXPATH] ATTRIBUTE_ALIGN(32);
	u32 newtmdsize ATTRIBUTE_ALIGN(32);
	u64 newtitleid ATTRIBUTE_ALIGN(32);
	signed_blob * newtmd;
	tmd_content * newtmdc, * newtmdcontent = NULL;
	
	char * tite = (char *)memalign(32,256);
	sprintf(tite,"[+] Installing Menu Files !");
	__Draw_Message(tite,0);
	sleep(2);
		
	newtitleid = 0x0000000100000000LL + ios;
		
	ES_GetStoredTMDSize(newtitleid, &newtmdsize);
	newtmd = (signed_blob *) memalign(32, newtmdsize);
	memset(newtmd, 0, newtmdsize);
	ES_GetStoredTMD(newtitleid, newtmd, newtmdsize);
	newtmdc = TMD_CONTENTS((tmd *) SIGNATURE_PAYLOAD(newtmd));
		
	for(i = 0; i < ((tmd *) SIGNATURE_PAYLOAD(newtmd))->num_contents; i++)
	{
		if(newtmdc[i].index == 1)
		{
			newtmdcontent = &newtmdc[i];
				
			if(newtmdc[i].type & 0x8000) //Shared content! This is the hard part :P.
				return -1;
			else //Not shared content, easy
				sprintf(filename, "/title/00000001/%08x/content/%08x.app", ios, newtmdcontent->cid);
	
				break;
		}
		else if(i == (((tmd *) SIGNATURE_PAYLOAD(newtmd))->num_contents) - 1)
			return -1;
	}

	free(newtmd);
	
	nandfile = ISFS_Open(filename, ISFS_OPEN_RW);
	ISFS_Seek(nandfile, 0, SEEK_SET);
	
	length = filesize(fp);
	gprintf("length of file %d \n",length);
	numchunks = length/CHUNKS + ((length % CHUNKS != 0) ? 1 : 0);
	
	gprintf("[+] Total parts: %d\n", numchunks);
	
	for(i = 0; i < numchunks; i++)
	{
		wiilight(1);
		data = memalign(32, CHUNKS);
		if(data == NULL)
		{
			return -1;
		}
		char *ms = memalign(32,256);
		
		sprintf(ms,"Installing part %d",i+1);

		gprintf("	Installing part %d\n",(i + 1) );

		__Draw_Message(ms,0);
		
		ret = fread(data, 1, CHUNKS, fp);
		if (ret < 0) 
		{
			gprintf("[-] Error reading from SD! (ret = %d)\n\n", ret);
			
			gprintf("	Press any button to continue...\n");
			return -1;
		}
		else
		{
			cursize = ret;
		}

		ret = ISFS_Write(nandfile, data, cursize);
		if(ret < 0)
		{
			gprintf("[-] Error writing to NAND! (ret = %d)\n\n", ret);
			gprintf("	Press any button to continue...\n");
			return ret;
		}
		free(data);
		wiilight(0);
	}
	
	ISFS_Close(nandfile);

	
	
	return 0;
}

int __Start_Install(){
	gprintf("install theme start! \n");
	char filepath[1024];
	FILE *fp = NULL;
	u8 *data;
	u32 length;
	int i;
	char *start = memalign(32,256);
	sprintf(start,"Starting Custom Theme Installation !");
	__Draw_Message(start,0);
	sleep(2);
	
	
		sprintf(filepath, "Fat:/modthemes/%s",ThemeList[orden[selectedtheme]].title);
		gprintf("filepath (%s) \n",filepath);
		fp = fopen(filepath, "rb");
		
        if (!fp)
        {
            gprintf("[+] File Open Error not on SD!\n");
        }
		
        length = filesize(fp);
        data = allocate_memory(length);
        memset(data,0,length);
        fread(data,1,length,fp);
		if(length <= 0)
        {
            printf("[-] Unable to read file !! \n");
            return MENU_HOME;
        }
        else
        {
            for(i = 0; i < length;)
            {
                if(data[i] == 83)
                {
                    if(data[i+6] == 52)  // 4
                    {
                        if(data[i+8] == 48)  // 0
                        {
                            if(data[i+28] == 85)  // usa
                            {
                                ThemeList[orden[selectedtheme]].version = 40;
                                ThemeList[orden[selectedtheme]].region = (u8*)85;
                                break;

                            }
                            else if(data[i+28] == 74)  //jap
                            {
                                ThemeList[orden[selectedtheme]].version = 40;
                                ThemeList[orden[selectedtheme]].region = (u8*)74;
                                break;
                            }
                            else if(data[i+28] == 69)  // pal
                            {
                                ThemeList[orden[selectedtheme]].version = 40;
                                ThemeList[orden[selectedtheme]].region = (u8*)69;
                                break;
                            }
                        }
                        else
                        {
                            if(data[i+8] == 49)  // 4.1
                            {
                                if(data[i+31] == 85)  // usa
                                {
                                    ThemeList[orden[selectedtheme]].version = 41;
                                    ThemeList[orden[selectedtheme]].region = (u8*)85;
                                    break;

                                }
                                else if(data[i+31] == 74)  //jap
                                {
                                    ThemeList[orden[selectedtheme]].version = 41;
                                    ThemeList[orden[selectedtheme]].region = (u8*)74;
                                    break;
                                }
                                else if(data[i+31] == 69)  // pal
                                {
                                    ThemeList[orden[selectedtheme]].version = 41;
                                    ThemeList[orden[selectedtheme]].region = (u8*)69;
                                    break;
                                }
                                else if(data[i+31] == 75)  // kor
                                {
                                    ThemeList[orden[selectedtheme]].version = 41;
                                    ThemeList[orden[selectedtheme]].region = (u8*)75;
                                    break;
                                }

                            }
                            else
                            {
                                if(data[i+8] == 50)  // 4.2
                                {
                                    if(data[i+28] == 85)  // usa
                                    {
                                        ThemeList[orden[selectedtheme]].version = 42;
                                        ThemeList[orden[selectedtheme]].region = (u8*)85;
                                        break;
                                    }
                                    else if(data[i+28] == 74)  // jap
                                    {
                                        ThemeList[orden[selectedtheme]].version = 42;
                                        ThemeList[orden[selectedtheme]].region = (u8*)74;
                                        break;
                                    }
                                    else if(data[i+28] == 69)  // pal
                                    {
                                        ThemeList[orden[selectedtheme]].version = 42;
										ThemeList[orden[selectedtheme]].region = (u8*)69;
                                        break;
                                    }
                                    else if(data[i+28] == 75)  // kor
                                    {
                                        ThemeList[orden[selectedtheme]].version = 42;
                                        ThemeList[orden[selectedtheme]].region = (u8*)75;
                                        break;
                                    }
                                }
                                else
                                {
                                    if(data[i+8] == 51) // 4.3
                                    {
                                        if(data[i+28] == 85)  // usa
                                        {
                                            ThemeList[orden[selectedtheme]].version = 43;
                                            ThemeList[orden[selectedtheme]].region = (u8*)85;
                                            break;
                                        }
                                        else if(data[i+28] == 74)  //jap
                                        {
                                            ThemeList[orden[selectedtheme]].version = 42;
                                            ThemeList[orden[selectedtheme]].region = (u8*)74;
                                            break;
                                        }
                                        else if(data[i+28] == 69)  // pal
                                        {
                                            ThemeList[orden[selectedtheme]].version = 43;
                                            ThemeList[orden[selectedtheme]].region = (u8*)69;
                                            break;
                                        }
                                        else if(data[i+28] == 75)  // kor
                                        {
                                            ThemeList[orden[selectedtheme]].version = 43;
                                            ThemeList[orden[selectedtheme]].region = (u8*)75;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if(data[i+6] == 51)  // 3
                        {
                            if(data[i+8] == 50)  // 2
                            {
                                if(data[i+28] == 85)  // usa
                                {
                                    ThemeList[orden[selectedtheme]].version = 32;
                                    ThemeList[orden[selectedtheme]].region = (u8*)85;
                                    break;
                                }
                                else
                                {
                                    if(data[i+28] == 69)  // pal
                                    {
                                        ThemeList[orden[selectedtheme]].version = 32;
                                        ThemeList[orden[selectedtheme]].region = (u8*)69;
                                        break;
                                    }
                                    else
                                    {
                                        if(data[i+28] == 74)  // jap
                                        {
                                            ThemeList[orden[selectedtheme]].version = 32;
                                            ThemeList[orden[selectedtheme]].region = (u8*)74;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                i++;
            }

        }


        int current;
		current = getcurrentregion();
		int install;
		install = getinstallregion();
		
		gprintf("current(%d) install(%d) \n",current,install);
        gprintf("install theme .version(%d) .region(%c) \n",ThemeList[orden[selectedtheme]].version,ThemeList[orden[selectedtheme]].region);
        gprintf("cur theme .version(%d) .region(%c) \n",curthemestats.version,curthemestats.region);
		 if(curthemestats.version != ThemeList[orden[selectedtheme]].version)
        {
            const char *badversion = "Install can not continue system versions differ ! Press any button to exit.";
			__Draw_Message(badversion,1);  
            free(data);
            fclose(fp);
            sysHBC();
        }
        else if(current != install)
        {
            const char *badregion = "Install can not continue system regions differ ! Press any button to exit.";
			__Draw_Message(badregion,1);
            free(data);
            fclose(fp);
            sysHBC();
        }
		
        free(data);
        fclose(fp);
		
        fp = fopen(filepath, "rb");
        if (!fp)
        {
            gprintf("[+] File Open Error not on SD!\n");
        }
	
	// Install 
    InstallFile(fp);

    // Close file 
    if (fp)
        fclose(fp);
		
	char *done = (char*)memalign(32,256);
	sprintf(done,"Your Custom Theme has been installed !");
	__Draw_Message(done,0);
	sleep(2);
	
	return MENU_EXIT_TO_MENU;
}


int __Select_Theme(void){
	int i, j, hotSpot, hotSpotPrev, ret;
	ret=MENU_EXIT;

	if(themecnt==0)
		return MENU_HOME;

	// Create/restore hotspots
	for(i=0;i<ROWS;i++){
		for(j=0;j<COLS[wideScreen];j++){
			int pos=i*COLS[wideScreen]+j;
			Wpad_AddHotSpot(pos,
				FIRSTCOL[wideScreen]-ANCHOIMAGEN[wideScreen]/2+j*SEPARACIONX[wideScreen],
				FIRSTROW-ALTOIMAGEN/2+i*SEPARACIONY,
				ANCHOIMAGEN[wideScreen],
				ALTOIMAGEN,
				(i==0? COLS[wideScreen]*(ROWS-1)+j : pos-COLS[wideScreen]),
				(i==ROWS-1? j : pos+COLS[wideScreen]),
				(j==0? pos+COLS[wideScreen]-1 : pos-1),
				(j==COLS[wideScreen]-1? pos-COLS[wideScreen]+1 : pos+1)
			);
		}
	}
	Wpad_AddHotSpot(HOTSPOT_LEFT, 0,160,32,88, HOTSPOT_LEFT, HOTSPOT_LEFT, HOTSPOT_RIGHT, 0);
	Wpad_AddHotSpot(HOTSPOT_RIGHT, 640-32,160,32,88, HOTSPOT_RIGHT, HOTSPOT_RIGHT,(COLS[wideScreen]*2)-1, HOTSPOT_LEFT);
	hotSpot=hotSpotPrev=-1;

	// Load images from actual page
	if(pages<page+1)
		__Load_Images_From_Page();
	__Draw_Page(-1);


	// Select game loop
	for(;;){
		hotSpot=Wpad_Scan();

		// If hot spot changed
		if(movingGame>-1){
			__Draw_Page(hotSpot);
		}
		else if(hotSpot!=hotSpotPrev){
			hotSpotPrev=hotSpot;
			__Draw_Page(hotSpot);
		}

		if(hotSpot>=0 && hotSpot<=COLS[wideScreen]*ROWS){
			selectedtheme=page*COLS[wideScreen]*ROWS+hotSpot;
		}
		else{
			selectedtheme=-1;
		}

		MRC_Draw_Cursor(Wpad_GetWiimoteX(), Wpad_GetWiimoteY(), (movingGame>-1));

		if(movingGame==-1){
			if(((WPAD_ButtonsDown(WPAD_CHAN_0) & (WPAD_BUTTON_A | WPAD_BUTTON_B | WPAD_BUTTON_1)) || (PAD_ButtonsDown(0) & (PAD_BUTTON_A | PAD_TRIGGER_Z))) && hotSpot>-1 && hotSpot<COLS[wideScreen]*ROWS && orden[selectedtheme]!=EMPTY){
				if(WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_B){
					movingGame=selectedtheme;
					findnumpages();
				}else if((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A)){
					ret=MENU_SHOW_THEME;
					break;
				}
			}
		}
		else{
			//moving game
			if(!(WPAD_ButtonsHeld(WPAD_CHAN_0) & WPAD_BUTTON_B)){
				if(selectedtheme!=-1 && selectedtheme!=movingGame){
					u16 copia0=orden[movingGame];
					u16 copia1=orden[selectedtheme];

					orden[movingGame]=copia1;
					orden[selectedtheme]=copia0;
					saveconfig=true;
				}
				movingGame=-1;
				findnumpages();
				__Draw_Page(hotSpot);
			}
		}

		if(WPAD_ButtonsDown(WPAD_CHAN_0) || PAD_ButtonsDown(0)){
			if(page>=0 && (((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_MINUS) || \
			(PAD_ButtonsDown(0) & PAD_TRIGGER_L)) || (hotSpot==HOTSPOT_LEFT && \
			((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) \
			& PAD_BUTTON_A))))){
				gprintf("page = %d maxpages = %d pages = %d\n",page,maxPages,pages);
				if (page == 0)
					page = maxPages -1;
				else
					page-=1;
				ret=MENU_SELECT_THEME;
				break;
			}else if(page<maxPages && (((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_PLUS) || (PAD_ButtonsDown(0) & PAD_TRIGGER_R)) || (hotSpot==HOTSPOT_RIGHT && ((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A))))){
				
				if (page >= maxPages-1)
					page = 1;
				else 
					page+=1;
					
				ret=MENU_SELECT_THEME;
				break;
			}else if(((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_HOME) || (PAD_ButtonsDown(0) & PAD_BUTTON_START))){
				ret=MENU_HOME;
				break;
			}else if((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_B) || (PAD_ButtonsDown(0) & PAD_BUTTON_B)){
				ret=MENU_SELECT_THEME;
				break;
			}

			//if(WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_2)
			//	MRC_Capture();

		}

	}

	return ret;
}


char *getsavename(u32 idx){
    switch(idx)
    {
    case 289:
        return "42";// usa
        break;
    case 417:
        return "72";
        break;
    case 449:
        return "7b";
        break;
    case 481:
        return "87";
        break;
    case 513:
        return "97";// usa
        break;
    case 290:
        return "45";// pal
        break;
    case 418:
        return "75";
        break;
    case 450:
        return "7e";
        break;
    case 482:
        return "8a";
        break;
    case 514:
        return "9a";// pal
        break;
    case 288:
        return "40";// jpn
        break;
    case 416:
        return "70";
        break;
    case 448:
        return "78";
        break;
    case 480:
        return "84";
        break;
    case 512:
        return "94";// jpn
        break;
    case 486:
        return "8d";// kor
        break;
    case 454:
        return "81";
        break;
    case 518:
        return "9d";// kor
        break;
    default:
        return "UNK";
        break;
    }
}
void get_title_key(signed_blob *s_tik, u8 *key){
    static u8 iv[16] ATTRIBUTE_ALIGN(0x20);
    static u8 keyin[16] ATTRIBUTE_ALIGN(0x20);
    static u8 keyout[16] ATTRIBUTE_ALIGN(0x20);

    const tik *p_tik;
    p_tik = (tik*) SIGNATURE_PAYLOAD(s_tik);
    u8 *enc_key = (u8 *) &p_tik->cipher_title_key;
    memcpy(keyin, enc_key, sizeof keyin);
    memset(keyout, 0, sizeof keyout);
    memset(iv, 0, sizeof iv);
    memcpy(iv, &p_tik->titleid, sizeof p_tik->titleid);

    aes_set_key(commonkey);
    aes_decrypt(iv, keyin, keyout, sizeof keyin);

    memcpy(key, keyout, sizeof keyout);
}
void decrypt_buffer(u16 index, u8 *source, u8 *dest, u32 len){
    static u8 iv[16];
    memset(iv, 0, 16);
    memcpy(iv, &index, 2);
    aes_decrypt(iv, source, dest, len);
}
const char *getPath(int ind){
    switch(ind){
    case 0:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/cetk";
        break;
    case 1:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/tmd.";
        break;
    case 2:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/0000003f";
        break;
    case 3:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000042";
        break;
    case 4:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000045";
        break;
    case 5:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/0000006f";
        break;
    case 6:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000072";
        break;
    case 7:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000075";
        break;
    case 8:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000078";
        break;
    case 9:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/0000007b";
        break;
    case 10:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/0000007e";
        break;
    case 11:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000081";
        break;
    case 12:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000084";
        break;
    case 13:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000087";
        break;
    case 14:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/0000008a";
        break;
    case 15:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/0000008d";
        break;
    case 16:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000094";
        break;
    case 17:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/00000097";
        break;
    case 18:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/0000009a";
        break;
    case 19:
        return "http://nus.cdn.shop.wii.com/ccs/download/0000000100000002/0000009d";
        break;
    default:
        return "ERROR";
        break;
    }
}
int getslot(int num){
    switch(num)
    {
    case 288:
        return 2;
        break;
    case 289:
        return 3;
        break;
    case 290:
        return 4;
        break;
    case 416:
        return 5;
        break;
    case 417:
        return 6;
        break;
    case 418:
        return 7;
        break;
    case 448:
        return 8;
        break;
    case 449:
        return 9;
        break;
    case 450:
        return 10;
        break;
    case 454:
        return 11;
        break;
    case 480:
        return 12;
        break;
    case 481:
        return 13;
        break;
    case 482:
        return 14;
        break;
    case 486:
        return 15;
        break;
    case 512:
        return 16;
        break;
    case 513:
        return 17;
        break;
    case 514:
        return 18;
        break;
    case 518:
        return 19;
        break;
    default:
        return -1;
        break;
    }
}
int __downloadApp(int downloadonly)  {



	s32 rtn;
    u32 dvers;
    bool customversion = false;
    int ret, retries;
    int counter;
    char *savepath = (char*)memalign(32,256);
    char *oldapp = (char*)memalign(32,256);
	char *tmpstr = (char*)malloc(256);
	char *tmpstr2 = (char*)malloc(256);
	signed_blob * s_tik = NULL;
    signed_blob * s_tmd = NULL;
	FILE *f = NULL;
restart:	
	__Draw_Loading();
	
    dvers = GetSysMenuVersion();
    gprintf("dvers =%d \n",dvers);
    if(dvers > 518) 
        customversion = true;
    
	sprintf(tmpstr2,"%s:/config/themewii/origtheme/%s.app", getdevicename(thememode),getappname(dvers));
	if(!Fat_CheckFile(tmpstr2)){
		sprintf(tmpstr,"Downloading %s for System Menu v%d ",getappname(dvers),dvers);
		__Draw_Message(tmpstr,0);
		sleep(2);
		//free(tmpstr);
		__Draw_Loading();
		sprintf(tmpstr,"%s:/temp",getdevicename(thememode));
		gprintf("tmpstr (%s) \n",tmpstr);
		Fat_MakeDir(tmpstr);

		sprintf(tmpstr,"Initializing  Network ....");
		__Draw_Message(tmpstr,0);
		//free(tmpstr);
		sleep(2);
		
		for(;;){
			retries = 0;
			
	retry:
			__Draw_Loading();
			ret=net_init();
			if(ret<0 && ret!=-EAGAIN){
			   sprintf(tmpstr,"Connection Failed !! Returning to SysMenu !!");
				__Draw_Message(tmpstr,0);
				net_deinit();
				sleep(2);
				
				retries++;
				goto retry;
				
			}
			if(ret==0){
				sprintf(tmpstr,"Connection Complete !!");
				__Draw_Message(tmpstr,0);
				sleep(5);
				break;
			}
			if(retries == 5)
				goto end;
		}
		
				
		for(counter = 0; counter < 3; counter++)
		{	
			
			sprintf(tmpstr,"DownLoading Files .....");
			__Draw_Message(tmpstr,0);
			sleep(2);
			__Draw_Loading();
			char *path = (char*)memalign(32,256);
			int aa = getslot(dvers);


			if(counter == 0){

				sprintf(path,"%s",getPath(counter));
				gprintf("Dowloading %s ....",path);
			}
			if(counter == 1){

				sprintf(path,"%s%d",getPath(counter),dvers);
				gprintf("Dowloading %s ....",path);
			}
			if(counter == 2){

				sprintf(path,"%s",getPath(aa));
				gprintf("Dowloading %s ....",path);
			}
			u32 outlen=0;
			u32 http_status=0;
			u32 Maxsize = 99999999;


			ret = http_request(path, Maxsize);
			if(ret == 0 ){
				free(path);
				gprintf("download failed !! ret(%d)\n",ret);
				net_deinit();
				counter = 0;
				goto retry;
			}
			else{
				sprintf(tmpstr,"DownLoad Complete !");
				__Draw_Message(tmpstr,0);
				sleep(2);
			}
			free(path);

			u8* outbuf = (u8*)malloc(outlen);
			if(counter == 0){
				ret = http_get_result(&http_status, (u8 **)&s_tik, &outlen);
			}
			if(counter == 1){
				ret = http_get_result(&http_status, (u8 **)&s_tmd, &outlen);
			}
			if(counter == 2){
				ret = http_get_result(&http_status, &outbuf, &outlen);
			}

			gprintf("ret(%d) get result \n",ret);
			sprintf(tmpstr,"Decrypting files ....");
			__Draw_Message(tmpstr,0);
			sleep(2);
				
			//set aes key
			u8 key[16];
			u16 index;
			get_title_key(s_tik, key);
			aes_set_key(key);
			u8* outbuf2 = (u8*)malloc(outlen);

			if(counter == 2){
				if(outlen>0){//suficientes bytes
				index = 01;
				//then decrypt buffer
				decrypt_buffer(index,outbuf,outbuf2,outlen);
				sprintf(savepath,"%s:/temp/000000%s.app", getdevicename(thememode),getsavename(dvers));
				gprintf("savepath %s \n",savepath);
				ret = Fat_SaveFile(savepath, (void *)&outbuf2,outlen);
				if(ret < 0)
					gprintf("Unable to save file .\n");
				else
					gprintf("File Saved \n");
				}
			}
			sprintf(tmpstr,"Decryption  Complete !");
			__Draw_Message(tmpstr,0);
			sleep(2);
				

			if(outbuf!=NULL)
				free(outbuf);
			
			if(counter > 3)
				counter = 0;
			
			
		}
		net_deinit();
		
		sleep(1);
		if(downloadonly == 0){
			sprintf(oldapp,"/title/00000001/00000002/%s.app",getappname(dvers));
			
			if(ISFS_Delete(oldapp) != 0){
				sprintf(tmpstr,"Could not Delete %s .app. This will not effect anything.",getappname(dvers));
				__Draw_Message(tmpstr,0);
				sleep(3);
			}
			else
				 gprintf("deleted %s \n",oldapp);
			 
		}
		sprintf(savepath,"%s:/temp/000000%s.app", getdevicename(thememode),getsavename(dvers));
		gprintf("savepath %s \n",savepath);
		f = fopen(savepath,"rb");
		if(!f)
		{
			gprintf("could not open %s \n",savepath);
			return -1;
		}
		
		if(downloadonly == 0){
			printf("\nInstalling %s.app ....",getappname(dvers));
			__Draw_Loading();
			rtn = InstallFile(f);
			if(rtn < 0){
				if(f)
					fclose(f);
				return rtn;
			}
			
			/* Close file */
			if(f)
				fclose(f);
		}	
		int error;
		
		sprintf(tmpstr2,"%s:/config/themewii/origtheme", getdevicename(thememode));
		
		error = CreateSubfolder(tmpstr2);
		if(!error)
			gprintf("    ERROR!");
		else
			gprintf("    Successfully made folder %s !",tmpstr2);
			
		sprintf(tmpstr2,"%s:/config/themewii/origtheme/%s.app", getdevicename(thememode),getappname(dvers));
		
		error = CopyFile(savepath,tmpstr2);
		if (error != 1) {
			gprintf("    ERROR!");
		} 
		else {
			gprintf("    Successfully Copied!");
		}
		//const char *dnn = "[+] Done ! .... Press any button to return to the Menu.";
		//__Draw_Message(dnn,1);
		
		error = remove(savepath);
		
		if (error != 0) {
			gprintf("    ERROR!");
		} 
		else {
			gprintf("    Successfully deleted!");
		}
	}
	else{
		sprintf(tmpstr,"Installing %s for System Menu v%d from cache .",getappname(dvers),dvers);
		__Draw_Message(tmpstr,0);
		sleep(2);
		
		f = fopen(tmpstr2,"rb");
		if(!f)
		{
			gprintf("could not open %s \n",savepath);
			return -1;
		}
		printf("\nInstalling %s.app ....",getappname(dvers));
	   __Draw_Loading();
		rtn = InstallFile(f);
		if(rtn < 0){
			if(f)
				fclose(f);
			return rtn;
		}
		
		/* Close file */
		if(f)
			fclose(f);
			
	}
	
	if(downloadonly == 0){
		const char *dn = "[+] Done Installing Original .App .";
		__Draw_Message(dn,0);
		sleep(2);
		
		free(oldapp);
		free(savepath);
		free(tmpstr);
		free(tmpstr2);
		
		return sys_loadmenu();
	}
	else{
		const char *dn = "[+] Done Downloading .App .";
		__Draw_Message(dn,0);
		sleep(2);
	}
	
end:

	free(oldapp);
	free(savepath);
	free(tmpstr);
	free(tmpstr2);
	
	return MENU_SELECT_THEME;
}


#define PROJECTION_HEIGHT 64
int __Show_Theme(){
	void* imageBuffer;
	MRCtex *themeImage, *projection;
	int i, j, ret;
	char *c, *r, a;
	ModTheme *thetheme=&ThemeList[orden[selectedtheme]];
	
	// BLACK SCREEN
	/*a=160;
	for(i=0; i<480; i++){
		if(a<255 && ((i<208 && i%4==0) || i%8==0))
			a++;
		MRC_Draw_Box(0, i, 640, 1, a);
	}*/

	
	// ANOTHER SCREEN FADE TYPE
	a=200;
	for(i=0; i<480; i++){
		if(a<255 && ((i<100 && i%4==0) || (i>200 && i%8==0)))
			a++;
		MRC_Draw_Box(0, i, 640, 1, 0x20202000+a);
	}


	// Load image from FAT
	//if(ThemeList[selectedtheme].type == 20)
	//	sprintf(tempString,"%s:/" THEMEWII_PATH IMAGES_PREFIX "/%s.png",getdevicename(thememode) , thetheme->title);
	if(ThemeList[selectedtheme].type == 10)
		sprintf(tempString,"Fat:/" THEMEWII_PATH "DBimages/%sMain.png" ,DBlistpng[orden[selectedtheme]]);
		
	gprintf("tempstring %s \n",tempString);
	ret=Fat_ReadFile(tempString, &imageBuffer);
	// Decode image
	if(ret>0){
		themeImage=MRC_Load_Texture(imageBuffer);
		free(imageBuffer);

		MRC_Resize_Texture(themeImage, (wideScreen? 580 : 640), 340);
		//__MaskBanner(themeImage);
		//MRC_Center_Texture(themeImage, 1);

		projection=allocate_memory(sizeof(MRCtex));
		projection->buffer=allocate_memory(themeImage->width*PROJECTION_HEIGHT*4);
		projection->width=themeImage->width;
		projection->height=PROJECTION_HEIGHT;
		//MRC_Center_Texture(projection, 1);
		projection->alpha=true;
	
		a=128;
		r=(projection->buffer);
		for(i=0; i<PROJECTION_HEIGHT; i++){
			c=(themeImage->buffer)+(((themeImage->height-1)-i*2)*themeImage->width)*4;
			for(j=0; j<themeImage->width; j++){
				r[0]=c[0];
				r[1]=c[1];
				r[2]=c[2];
				r[3]=a;
				c+=4;
				r+=4;
			}
			if(a>4)
				a-=4;
		}
	
		MRC_Draw_Texture(40, 40, themeImage);
		//MRC_Draw_Texture(175, 275, projection);
		MRC_Draw_String((640-strlen(thetheme->title))/2, 400, WHITE, thetheme->title);
		//MRC_Draw_String(30, 330, WHITE, "By ");
		MRC_Draw_String(40, 400, WHITE, "[A]-Install Theme");
		MRC_Draw_String(560, 400, WHITE, "[B]-Back");
		MRC_Free_Texture(themeImage);
		MRC_Free_Texture(projection);
	}
	else{
		MRC_Draw_String(30, 360, WHITE, "[A]-Install Theme");
		MRC_Draw_String(30, 390, WHITE, "[B]-Back");
		//MRC_Draw_String(30, 360, WHITE, "By ");
		MRC_Draw_String((640-strlen(thetheme->title)*8)/2, 250, WHITE, thetheme->title);
	}
	MRC_Render_Screen();
	int answer;
	
	ret=MENU_SELECT_THEME;
	for(;;){
		WPAD_ScanPads();
		PAD_ScanPads();
		if(WPAD_ButtonsDown(WPAD_CHAN_0) || PAD_ButtonsDown(0)){
			if((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A)){
				if(thetheme->type == 20)
					ret=MENU_Install_Theme;
				if(thetheme->type == 10){
					answer = __Question_Window("Theme-Wii", "Would you like to Download the .mym file","Yes", "No");
					if(answer == false)
						ret = MENU_SELECT_THEME;
					else
						ret = Menu_Start_Themeing;	
				}
				gprintf("theme->type = %d ret %d \n",thetheme->type, ret);
				break;
			}
			if((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_B) || (PAD_ButtonsDown(0) & PAD_BUTTON_B)){
				ret = MENU_SELECT_THEME;
				break;
			}
		}
		
	}

	return ret;
}


//#define HOME_BUTTON_X			170
//#define HOME_BUTTON_Y			100
//#define HOME_BUTTON_WIDTH		200
//#define HOME_BUTTON_HEIGHT		30
//#define HOME_BUTTON_SEPARATION	5
#define HOME_BUTTON_X			208
#define HOME_BUTTON_Y			120
#define HOME_BUTTON_WIDTH		228
#define HOME_BUTTON_HEIGHT		40
#define HOME_BUTTON_SEPARATION	4
int __Home(void){
	int i, hotSpot, hotSpotPrev, ret;
	bool repaint=true;
	
	// Create/restore hotspots
	Wpad_CleanHotSpots();
	for(i=0; i<5; i++){
		Wpad_AddHotSpot(i,
			HOME_BUTTON_X,
			100+i*(HOME_BUTTON_HEIGHT+HOME_BUTTON_SEPARATION),
			HOME_BUTTON_WIDTH,
			HOME_BUTTON_HEIGHT,
			(i==0? 5 : i-1),
			(i==6? 0 : i+1),
			i, i
		);
	}


	__Draw_Window(HOME_BUTTON_WIDTH+44, 320, "Options");

	sprintf(tempString, "IOS_%d v_%d", IOS_GetVersion(), IOS_GetRevision());
	MRC_Draw_String(250, 380, BLACK, tempString);
	//MRC_Draw_String(10, 440, 0x505050ff, lang[9+thememode]);
	sprintf(tempString, "ThemeWii v_%d", THEMEWII_VERSION);
	MRC_Draw_String(210, 350, BLACK, tempString);

	// Loop
	hotSpot=hotSpotPrev=-1;

	ret=MENU_SELECT_THEME;
	for(;;){
		hotSpot=Wpad_Scan();

		// If hot spot changed
		if((hotSpot!=hotSpotPrev && hotSpot<6) || repaint){
			hotSpotPrev=hotSpot;

			__Draw_Button(0, "Theme Device Menu", hotSpot == 0);
			__Draw_Button(1, "Install Original Theme", hotSpot == 1);
			__Draw_Button(2, "Download Database Images", hotSpot == 2);
			__Draw_Button(3, "Exit <HBC>", hotSpot == 3);
			__Draw_Button(4, "Exit <Sys-Menu>", hotSpot == 4);
			repaint=false;
		}
		MRC_Draw_Cursor(Wpad_GetWiimoteX(), Wpad_GetWiimoteY(), 0);
		gprintf("hotSpot = %d \n",hotSpot);
		if(((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A)) && hotSpot>-1 && hotSpot<5){
			if(hotSpot == 0)
				ret = MENU_MANAGE_DEVICE;
			else if(hotSpot == 1)
				ret = MENU_ORIG_THEME;
			else if(hotSpot == 2)
				ret = MENU_DOWNLOAD;
			else if(hotSpot == 3)
				ret = MENU_EXIT;
			else if(hotSpot == 4)
				ret = MENU_EXIT_TO_MENU;
			break;
		}else if(((WPAD_ButtonsDown(WPAD_CHAN_0) & (WPAD_BUTTON_HOME | WPAD_BUTTON_B)) || (PAD_ButtonsDown(0) & (PAD_BUTTON_START | PAD_BUTTON_B)))){
			ret = MENU_SELECT_THEME;
			break;
		}else{
			repaint = true;
		}
	}

	return ret;
}
int __DownloadDBfile(char *ThemeFile){
	char *fatpath = malloc(256);
	char *tmp = malloc(256);
	char *msg = malloc(256);
	int ret = -1,i,retries;
	
	sprintf(tmp,"Fat:/config/themewii/mym_out");
	if(Fat_CheckFile(tmp) == 1)
		ret = remove(tmp);
	gprintf("ret remove mym_out = %d \n",ret);
	
	sprintf(tmp,"Fat:/config/themewii/DBlist");
	if(!Fat_CheckFile(tmp))
		Fat_MakeDir(tmp);
	
	int retrycheck = 0;
	
retrycheck:		

	for(i = 0;i < themecnt;i++){
		if(ThemeFile == DBThemelist[i]){
			//sprintf(tempString, "http://themewii.googlecode.com/files/%s4%s.mym",DBThemelistDL[i],getdownloadregion(Versionsys));
			sprintf(tmp,"Fat:/config/themewii/DBlist/%s4%s.mym",DBThemelistDL[i],getdownloadregion(Versionsys));
		}
	}
	if(!Fat_CheckFile(tmp)){
		if(retrycheck == 1)
			sprintf(tmp,"Fat:/config/themewii/DBlist/%s4.mym",DBThemelistDL[i]);
		if(retrycheck == 2)
			sprintf(tmp,"Fat:/config/themewii/DBlist/%s.mym",DBThemelistDL[i]);
		
		retrycheck+=1;
	
		if(retrycheck < 3)
			goto retrycheck;
	
	
	
	sprintf(msg,"Downloading %s .....",ThemeFile);
	__Draw_Message(msg,0);
	sleep(2);
	
	for(;;){
		ret = net_init();
		if(ret < 0 && ret != -EAGAIN){
			

			sprintf(tempString, "ERROR: I can't connect to network.");
			__Draw_Message(tempString, ret);

			return ret;
		}
		if(ret == 0) 
			break;
		sleep(5);
	}
	for(i = 0;i < themecnt;i++){
		if(ThemeFile == DBThemelist[i]){
			sprintf(tempString, "http://themewii.googlecode.com/files/%s4%s.mym",DBThemelistDL[i],getdownloadregion(Versionsys));
			sprintf(fatpath,"%s:/config/themewii/DBlist/%s4%s.mym",getdevicename(thememode),DBThemelistDL[i],getdownloadregion(Versionsys));
		}
	}
	retries = 0;
	
retry:	
		
	gprintf("tempString(%s) \n",tempString);
	if(!Fat_CheckFile(fatpath)){
			
			
		u32 outlen=0;
        u32 http_status=0;
        u32 Maxsize = 99999999;
		u8* outbuf=NULL;

        ret = http_request(tempString, Maxsize);
        if(ret == 0 )
        {
            //free(tempString);
            gprintf("download failed !! ret(%d)\n",ret);
            //printf("Failed !! ret(%d)\n",ret);
            sleep(3);
            for(i = 0;i < themecnt;i++){
				if(ThemeFile == DBThemelist[i]){
					sprintf(tempString, "http://themewii.googlecode.com/files/%s4.mym",DBThemelistDL[i]);
					sprintf(fatpath,"%s:/config/themewii/DBlist/%s4%s.mym",getdevicename(thememode),DBThemelistDL[i],getdownloadregion(Versionsys));
				}
			}
			retries+=1;
			if(retries == 2){
				for(i = 0;i < themecnt;i++){
					if(ThemeFile == DBThemelist[i]){
						sprintf(tempString, "http://themewii.googlecode.com/files/%s.mym",DBThemelistDL[i]);
						sprintf(fatpath,"%s:/config/themewii/DBlist/%s.mym",getdevicename(thememode),DBThemelistDL[i]);
					}
				}
			}
			if(retries < 5)
				goto retry;
			else
				return ret;
        }
        else
            gprintf("Complete !! \n\n");
		
		ret = http_get_result(&http_status, &outbuf, &outlen); 
		
		
		if(outlen>64){//suficientes bytes
			Fat_SaveFile(fatpath, (void *)&outbuf,outlen);
		}

		if(outbuf!=NULL)
			free(outbuf);

	}
	net_deinit();
	sprintf(msg,"Complete ...");
	__Draw_Message(msg,0);
	sleep(2);
	}
	else{
		sprintf(msg,"Found %s.mym file using it .",ThemeFile);
		__Draw_Message(msg,0);
		sleep(2);
	}
	return MENU_MAKE_THEME;
}
int __DownloadDBpng(){
	gprintf("In download db png \n");
	char *fatpath = (char*)malloc(256);
	char *tmpstr = (char*)malloc(256);
	int i,j,ret, error = 0;
	
	sprintf(tmpstr,"Starting Theme DataBase preview download ....");
	__Draw_Message(tmpstr,0);
	sleep(2);
	
	gprintf("thememode = %d \n",thememode);
	sprintf(tmpstr,"Fat:/config/themewii/DBimages");
	if(!Fat_CheckFile(tmpstr))
		Fat_MakeDir(tmpstr);
	
	if(!CheckFile(tmpstr))
		CreateSubfolder(tmpstr);
		
	int retries = 0;
	
	for(;;){
		__Draw_Loading();
		ret=net_init();
		if(ret<0 && ret!=-EAGAIN){
			

			sprintf(tempString, "ERROR: I can't connect to network . (returned %d)\n",ret);
			__Draw_Message(tempString, 0);
			sleep(2);
			retries+=1;
		}
		if(ret==0) //consigo conexion
			break;
		if(retries == 5){
			sleep(2);
			goto end;
		}
	}
	
	gprintf("themecnt = %d \n",themecnt);
	__Draw_Loading();
	for(i = 0;i < themecnt;i++){
		__Draw_Loading();
		sprintf(tmpstr,"Downloading %d of %d Theme Previews .",i + 1,themecnt-1);
		__Draw_Message(tmpstr,0);
		sleep(1);
		__Draw_Loading();
		sprintf(tempString, "%s", ImageDownloads[i]);
		gprintf("tempString(%s) \n",tempString);
		sprintf(fatpath,"Fat:/config/themewii/DBimages/%sMain.png", DBlistpng[i]);
		gprintf("fatpath(%s) \n",fatpath);
		__Draw_Loading();
		if(!Fat_CheckFile(fatpath)){
			
			
			u32 outlen=0;
			u32 http_status=0;
			u32 Maxsize = 99999999;
			u8* outbuf=NULL;
			__Draw_Loading();
			ret = http_request(tempString, Maxsize);
			if(ret == 0 )
			{
				gprintf("download failed !! ret(%d)\n",ret);
				sprintf(tmpstr,"Download %d of %d Theme Previews Failed !",i + 1,themecnt - 1);
				__Draw_Message(tmpstr,0);
				sleep(3);
				error+=1;
				goto end2;
				//continue;
			}
			else
				gprintf("Complete !! \n\n");
			__Draw_Loading();
			ret = http_get_result(&http_status, &outbuf, &outlen); 
			
			if(outlen>64){//suficientes bytes
				//sprintf(fatpath,"%s:/config/themewii/DBimages/%s", getdevicename(thememode), DBlistpng[i]);
				gprintf("fatpath(%s) \n",fatpath);
				ret = Fat_SaveFile(fatpath, (void *)&outbuf,outlen);
				if(ret < 0){
				sprintf(tmpstr,"Saving Preview Falied !");
				__Draw_Message(tmpstr,0);
				sleep(2);
				error+=1;
				//continue;
				}
				else
					gprintf("File Saved \n");
			}
			__Draw_Loading();	
			if(outbuf!=NULL)
				free(outbuf);
			__Draw_Loading();	
		}
	end2:
		__Draw_Loading();
		if(DBlistpng[i] == 0){
			gprintf("db list[i] null \n");
			break;
		}
		__Draw_Loading();
		if(i == 3){
			gprintf("i = %d\n",i);
			break;
		}
		__Draw_Loading();
	}
	
	net_deinit();

	gprintf("Exiting dbpng \n");
	sprintf(tmpstr,"Theme DataBase preview download .... Complete .");
	__Draw_Message(tmpstr,0);
	sleep(3);
	if(error == 0)
		return 0;
	else
		return error;
	
end:
	sprintf(tmpstr,"One or more Theme Previews was not downloaded .");
	__Draw_Message(tmpstr,0);
	sleep(2);
	sprintf(tmpstr,"Please retry download from home button menu .");
	__Draw_Message(tmpstr,0);
	
	return 0;
}
const char *getdevicename(int index){
	switch(index)
	{
		case 1: return "sd";
		break;
		case 2: return "usb";
		//case 3: return "neek"; 
		break;
		default: return "ukn";
	}
}
bool Comparefolders(const char *src, const char *dest){
    if(!src || !dest)
        return false;

    char *folder1 = strchr(src, ':');
    char *folder2 = strchr(dest, ':');

	if(!folder1 || !folder2)
        return false;

	int position1 = folder1-src+1;
	int position2 = folder2-dest+1;

	char temp1[50];
	char temp2[50];

	snprintf(temp1, position1, "%s", src);
	snprintf(temp2, position2, "%s", dest);

    if(strcasecmp(temp1, temp2) == 0)
        return true;

    return false;
}


void write_file(void* data, size_t size, char* name){
	FILE *out;
	out = fopen(name, "wb");
	fwrite(data, 1, size, out);
	fclose(out);	
}
int Undo_U8_archive(void){
	U8_archive_header header;
	U8_node root_node;
	u32 tag;
	u32 num_nodes;
	U8_node* nodes;
	u8* string_table;
	size_t rest_size;
	unsigned int i;//,j;
	u32 data_offset;
	u16 dir_stack[16];
	int dir_index = 0;
 
	fread(&header, 1, sizeof header, fp);
	tag = be32((u8*) &header.tag);
	if (tag != 0x55AA382D) {
	 gprintf("No U8 tag");
	 return -3; 
	}
 
	fread(&root_node, 1, sizeof(root_node), fp);
	num_nodes = be32((u8*) &root_node.size) - 1;
	gprintf("Number of files: %d\n", num_nodes);
 
	nodes = malloc(sizeof(U8_node) * (num_nodes));
	fread(nodes, 1, num_nodes * sizeof(U8_node), fp);
 
	data_offset = be32((u8*) &header.data_offset);
	rest_size = data_offset - sizeof(header) - (num_nodes+1)*sizeof(U8_node);
 
	string_table = malloc(rest_size);
	fread(string_table, 1, rest_size, fp);
	
	//char *file = malloc(256);
	//char *save = malloc(256);
		
	for (i = 0; i < num_nodes; i++) {
		U8_node* node = &nodes[i];   
		u16 type = be16((u8*)&node->type);
		u16 name_offset = be16((u8*)&node->name_offset);
		u32 my_data_offset = be32((u8*)&node->data_offset);
		u32 size = be32((u8*)&node->size);
		char* name = (char*) &string_table[name_offset];
		u8* file_data;
		//char *no_ext_name = malloc(256);
		//char *check_name = malloc(256);
		//char workingname[] = {0};
		//int len = strlen(name);
		//int result = -1;
		
		//gprintf("dir_index(%d) \n",dir_index);
		if (type == 0x0100) {
		  // Directory
		  mkdir(name, 0777);
		  chdir(name);
		  dir_stack[++dir_index] = size;
		  gprintf("type(%d)  %*s%s/\n", dir_index, "", name);
		}
		else {
		  // Normal file
	 
			if (type != 0x0000) {
				gprintf("Unknown type");
				return -3;
			}
		 
			fseek(fp, my_data_offset, SEEK_SET);
			file_data = malloc(size);
			fread(file_data, 1, size, fp);
			write_file(file_data, size, name);
			free(file_data);
			gprintf("type(%d)  %*s %s (%d bytes)\n", dir_index, "", name, size);
			
		}
	 
		while (dir_stack[dir_index] == i+2 && dir_index > 0) {
		  chdir("..");
		  dir_index--;
		}
	}
	return 0;
}

int Undo_U8(char *Filepath, char* Filesavepath){

	outdir = Filesavepath;
	gprintf("Extracting files to %s.\n", outdir);
	fp = fopen(Filepath, "rb");
 
	mkdir(outdir, 0777);
	chdir(outdir);
 
	Undo_U8_archive();
 
	fclose(fp);
 
	return 0;
}
int __Theme_Make(char *ThemeFile){
	gprintf("Starting Theme Making !! \n");
	char *msg = malloc(256);
	char *tmpstr = malloc(256);
	char *tmpstr2 = malloc(256);
	char *tmpstr3 = malloc(256);
	s32 start = 0;
	int result;
	int retries, k, ret,dircnt = 0;
	unzFile DBtheme;
	char *file[100];
	
	sprintf(tmpstr, "Fat:/config/themewii/origtheme/%s.app", getappname(Versionsys));
	tmpstr2 = "Fat:/config/themewii/app_out";
	tmpstr3 = "Fat:/config/themewii/app_out/www.arc";
	gprintf("tmpstr = %s \ntmpstr2 = %s\n",tmpstr,tmpstr2);
	
	if(!Fat_CheckFile(tmpstr)){
		sprintf(msg,"Downloading Baseapp %s.app for Sys-Menu_%s %s...", getappname(Versionsys), getsysvernum(Versionsys), getregion(Versionsys));
		__Draw_Message(msg,0);
		sleep(2);
		__downloadApp(1);
		sprintf(msg,"Downloading Baseapp %s.app for Sys-Menu_%s %s... Complete .", getappname(Versionsys), getsysvernum(Versionsys), getregion(Versionsys));
		__Draw_Message(msg,0);
		sleep(3);
	}
	
	if(!Fat_CheckFile(tmpstr3)){
	
		sprintf(msg,"Extracting Baseapp %s.app for Sys-Menu_%s %s...", getappname(Versionsys), getsysvernum(Versionsys), getregion(Versionsys));
		__Draw_Message(msg,0);
		sleep(3);
		
		result = Undo_U8(tmpstr,tmpstr2);
		
		sprintf(tmpstr,"Fat:/config/themewii/app_out");
		gprintf("tmpstr(%s) \n",tmpstr);
		DIR *dir;
		//int foldercomp,dirptrnum = 0,kk;
		char *dirptr = memalign(32, 256);
		//char fileptr[MAX_FILELIST_LEN];
		//static char filelist[MAX_FILELIST_LEN];
		//char *ptr = dirptr;
		//char *ptr2 = fileptr;
		//struct stat filestat;
			
		dir = opendir(tmpstr);
		if (!dir)
		{
			gprintf("could not open dir(%s)\n",tmpstr);
			return -1;
		}
		else
			gprintf("Dir open .....\n");
		u32 x = 0, y;
		struct dirent *entry = NULL;
	    
		while((entry = readdir(dir))) // If we get EOF, the expression is 0 and
								  // the loop stops. 
		{
			//if(strncmp(entry->d_name, ".", 1) != 0 && strncmp(entry->d_name, "..", 2) != 0)
			dircnt+=1;
		}
		//closedir(dir);
		gprintf("\ndircnt {%i] \n", dircnt);
	
		while((entry = readdir(dir)))
		{
				
			if(dir->dirData != NULL)
			{
				
				gprintf("ptrdir = %s \n",entry->d_name);
			}
		//	}
		//	if ((filestat.st_mode & S_IFDIR) == 0){
		//		gprintf("fileptr = %s\n",ptr);
				
		//		ptr+=strlen(ptr)+1;
		//		
		//	}
			
		}	
		u32 cnt, idx = 0;
		//if(!dircnt){
		//	gprintf("\n	[-] No Files Found .\n");
		//	return -1;
		//}
		/* Move to start entry */
		//for (cnt = 0; cnt < start; cnt++)
		//	idx += strlen(dirptr + idx) + 1;
		/* Show entries */
		for (cnt = 0; cnt < dircnt; cnt++)
		{
			/* Print filename */
			//file[cnt]= dirptr+idx;
			/* Move to the next entry */
			//idx += strlen(dirptr + idx) + 1;
			//gprintf("file = %s  %d \n",file[cnt],cnt);
			
		}
		sprintf(msg,"Extraction of Baseapp Complete .");
		__Draw_Message(msg,0);
		sleep(2);
		
		
	}
	
	sprintf(msg,"Baseapp Found in Cache Using it .");
	__Draw_Message(msg,0);
	sleep(3);
		
	sprintf(msg,"Extracting %s for Sys-Menu_%s %s...",ThemeFile, getsysvernum(Versionsys), getregion(Versionsys));
	__Draw_Message(msg,0);
	sleep(2);
	for(k = 0;k < themecnt;k++){
		if(ThemeFile == DBThemelist[k])
			sprintf(tmpstr,"Fat:/config/themewii/DBlist/%s4%s.mym",DBThemelistDL[k],getdownloadregion(Versionsys));
	}
			
	gprintf("tmpstr =%s \n",tmpstr);
	DBtheme = unzOpen(tmpstr);
	retries = 0;
retry:		
	ret = extractZip(DBtheme,0,1,NULL,thememode);
	gprintf("ret extractzip(%d) \n",ret);
	if(ret < 0){
		for(k = 0;k < themecnt;k++){
			if(ThemeFile == DBThemelist[k])
				sprintf(tmpstr,"Fat:/config/themewii/DBlist/%s4.mym",DBThemelistDL[k]);		
		}
		DBtheme = unzOpen(tmpstr);
		retries+=1;
		if(retries >= 2){
			for(k = 0;k < themecnt;k++){
				if(ThemeFile == DBThemelist[k])
					sprintf(tmpstr,"Fat:/config/themewii/DBlist/%s.mym",DBThemelistDL[k]);		
			}
			DBtheme = unzOpen(tmpstr);
		}
		goto retry;
	}
	sprintf(msg,"Extraction of Mym file Complete .");
	__Draw_Message(msg,0);
	sleep(2);
	
	sprintf(msg,"This is it so Far for Now ..... .....");
	__Draw_Message(msg,0);
	sleep(2);
	
	return MENU_SELECT_THEME;
}

int __Select_Theme_Device(void){
	int i, hotSpot, hotSpotPrev, ret;
	bool repaint=true;
	
	const char *langs[2] = {
		"Sd Card",
		"Usb Device",
	};
	
	// Create/restore hotspots
	Wpad_CleanHotSpots();
	for(i=0; i<3; i++){
		Wpad_AddHotSpot(i,
			HOME_BUTTON_X,
			180+i*(HOME_BUTTON_HEIGHT+HOME_BUTTON_SEPARATION),
			HOME_BUTTON_WIDTH,
			HOME_BUTTON_HEIGHT,
			(i == 0 ? 1 : i - 1),
			(i == 1 ? 0 : i + 1),
			i, i
		);
	}

	// Background
	MRC_Draw_Texture(0, 0, textures[TEX_BACKGROUND]);

	MRC_Draw_String(95, 100, BLACK, "Select Theme List Device :");
	MRC_Draw_String(95, 130, BLACK, "Theme Database file will be saved here also .");
	MRC_Draw_String(95, 360, BLACK, "[A]-Selection/Install    [B]-Back    [HOME/start]-Options/Exit");
	
	sprintf(tempString, "IOS_%d v_%d", IOS_GetVersion(), IOS_GetRevision());
	MRC_Draw_String(95, 430, WHITE, tempString);
	
	sprintf(tempString, "System_Menu v%s_%s", getsysvernum(Versionsys), getregion(Versionsys));
	MRC_Draw_String(420, 430, WHITE, tempString);

	// Loop
	hotSpot=hotSpotPrev=-1;

	ret=0;
	for(;;){
		hotSpot=Wpad_Scan();

		// If hot spot changed
		if((hotSpot!=hotSpotPrev && hotSpot<3) || repaint){
			hotSpotPrev=hotSpot;

			for(i=0; i<2; i++){
				__Draw_Button(i, langs[i], hotSpot==i);
			}
			repaint=false;
		}
		MRC_Draw_Cursor(Wpad_GetWiimoteX(), Wpad_GetWiimoteY(), 0);
		
		if(((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_HOME) || (PAD_ButtonsDown(0) & PAD_BUTTON_START))){
			exit(0);
		}
		if(((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A))){
			ret = hotSpot + 1;	
			break;
		}
		else{
			repaint = true;
		}
	}
	return ret;
}
/*  Get List of installed IOS  */
s32 __u8Cmp(const void *a, const void *b)
{
    return *(u8 *)a-*(u8 *)b;
}
u8 *get_ioslist(u32 *cnt)
{
    u64 *buf = 0;
    s32 i, res;
    u32 tcnt = 0, icnt;
    u8 *ioses = NULL;
gprintf("\n\nget ioses \n\n");

    //Get stored IOS versions.
    res = ES_GetNumTitles(&tcnt);
    if(res < 0)
    {
        printf("\nES_GetNumTitles: Error! (result = %d)\n", res);
        return 0;
    }
    buf = memalign(32, sizeof(u64) * tcnt);
    res = ES_GetTitles(buf, tcnt);
    if(res < 0)
    {
        printf("\nES_GetTitles: Error! (result = %d)\n", res);
        if (buf) free(buf);
        return 0;
    }
gprintf("\n\n first for \n\n");
    icnt = 0;
    for(i = 0; i < tcnt; i++)
    {
        if(*((u32 *)(&(buf[i]))) == 1 && (u32)buf[i] > 200 && (u32)buf[i] < 0x100)
        {
            
		  icnt++;
            ioses = (u8 *)realloc(ioses, sizeof(u8) * icnt);
            ioses[icnt - 1] = (u8)buf[i];
        }
    }

    ioses = (u8 *)malloc(sizeof(u8) * icnt);
    icnt = 0;

    for(i = 0; i < tcnt; i++)
    {
        if(*((u32 *)(&(buf[i]))) == 1 && (u32)buf[i] > 200 && (u32)buf[i] < 0x100)
        {
            if((u32)buf[i] >= 224 && (u32)buf[i] <= 250)
		  {
		  icnt++;
            ioses[icnt - 1] = (u8)buf[i];
		  }
        }
	   //gprintf("\n\nioses [%s] \n\n", ioses[icnt - 1]);
    }
    free(buf);
    qsort(ioses, icnt, 1, __u8Cmp);

    *cnt = icnt;
    return ioses;
}
int __Select_Ios(u32 defaultios)
{
	u32 Cios;
	int hotSpot, hotSpotPrev;
	bool repaint=true;
	u32 selection = 0;
	u32 ioscount, i;
	u8 *list = get_ioslist(&ioscount);
	char *tmpstr = memalign(32, 256);
	char tmpstr2[1024]; //= memalign(32, 256);
	gprintf("\n\nfor ioslist \n");
	
	for (i=0; i<ioscount; i++)
    {
        // Default to default_ios if found, else the loaded IOS
        if (list[i] == defaultios)
        {
            selection = i;
            break;
        }
        if (list[i] == IOS_GetVersion())
        {
            selection = i;
        }
	  // sprintf(tmpstr, "\n\nlist[i] (%s) (%i) \n\n",list[i],i);
	  // gprintf(tmpstr);
    }
			gprintf("\n\n Create/restore hotspots \n\nz");
			
	Wpad_CleanHotSpots();
	for(i=0; i<ioscount; i++){
		Wpad_AddHotSpot(i,
			HOME_BUTTON_X,
			180+i*(HOME_BUTTON_HEIGHT+HOME_BUTTON_SEPARATION),
			HOME_BUTTON_WIDTH,
			HOME_BUTTON_HEIGHT,
			(i == 0 ? ioscount-1 : i - 1),
			(i == ioscount ? 0 : i + 1),
			i, i
		);
	}
	gprintf("\n\nabove background \n\n");
	// Background
	MRC_Draw_Texture(0, 0, textures[TEX_BACKGROUND]);

	MRC_Draw_String(95, 100, BLACK, "Select Ios :");
	MRC_Draw_String(95, 130, BLACK, "Theme Database file will be saved here also .");
	
	MRC_Draw_String(95, 370, BLACK, "[A]-Selection    [B]-Back    [HOME/start]-Options/Exit");
	
	sprintf(tmpstr, "IOS_%d v_%i", IOS_GetVersion(), IOS_GetRevision());
	MRC_Draw_String(95, 430, WHITE, tmpstr);
	
	sprintf(tmpstr, "System_Menu v%s_%s", getsysvernum(Versionsys), getregion(Versionsys));
	MRC_Draw_String(420, 430, WHITE, tmpstr);

	// Loop
	hotSpot=hotSpotPrev=-1;


	for(;;){
		hotSpot=Wpad_Scan();

		// If hot spot changed
		if((hotSpot!=hotSpotPrev && hotSpot<ioscount + 1) || repaint){
			hotSpotPrev=hotSpot;
gprintf("\n\n for in for before drawbutton ioscount[%i] \n\n", ioscount);
			for(i = 0; i < ioscount; i++){
				sprintf(tmpstr2, "%i", list[i]);
				gprintf("\n\ntmpstr2[%s] \n\n", tmpstr2);
				__Draw_Button(i, tmpstr2, hotSpot==i);
			}
			repaint=false;
		}
		MRC_Draw_Cursor(Wpad_GetWiimoteX(), Wpad_GetWiimoteY(), 0);
		
		if(((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_HOME) || (PAD_ButtonsDown(0) & PAD_BUTTON_START))){
			exit(0);
		}
		if(((WPAD_ButtonsDown(WPAD_CHAN_0) & WPAD_BUTTON_A) || (PAD_ButtonsDown(0) & PAD_BUTTON_A))){
			gprintf("\n\n hotSpot Abutton [%i] \n\n", hotSpot);
			break;
		}
		else{
			repaint = true;
		}
	}
	selection = hotSpot;
	Cios = list[selection];
	return Cios;
}
int Menu_Loop(int Mode){
	
	int ret=0;
	char *tmpstr = malloc(256);
	u32 newver, Cios = 0;
	
	gprintf("themewii start main loop Mode[%i]\n", Mode);	

	// Check console language
	ret=CONF_GetLanguage();
	if(ret>0 && ret<=CONF_LANG_DUTCH)
		lang=texts[ret-1];
	else
		lang=texts[0];

	// Check if widescreen
	if(CONF_GetAspectRatio()==CONF_ASPECT_16_9)
		wideScreen=true;

	// Init MRC graphics
	MRC_Init();
	textures[1]=MRC_Load_Texture((void *)themewii_background_png);
	textures[4]=MRC_Load_Texture((void *)themewii_loading_png);
	Versionsys = GetSysMenuVersion();
	gprintf("versionsys(%d) \n",Versionsys);
    newver = checknandapp();
	if(newver == 0){
		newver = checkcustom(Versionsys);
		Versionsys = newver;
	}
	gprintf("2::versionsys(%d) \n",Versionsys);
	
	ret=MENU_Select_Ios;
	
	for(;;){
		if(ret == MENU_Select_Ios) //13
		{
			Cios = __Select_Ios(defaultios);
			gprintf("\n Cios [%i] \n", Cios);
			Wpad_Disconnect();
			sleep(1);
			ret = IOS_ReloadIOS(Cios);
			gprintf("\n\nret [%i] \n\n", ret);
			Wpad_Init();
			sleep(1);
			ret = MENU_MANAGE_DEVICE;
		}
		else if(ret == MENU_SELECT_THEME) //0
		{
			ret = filelist_retrieve(thememode);
			
			// Load skin images
			MRC_Free_Texture(textures[1]);
			MRC_Free_Texture(textures[4]);
	
			__Load_Skin_From_FAT();
			
			__Load_Config();
			
			ret = __Select_Theme();
		}
		else if(ret == MENU_ORIG_THEME) //8
		{
			ret = __downloadApp(0);
		}
		else if(ret == MENU_Install_Theme) //4
		{
			ret = __Start_Install();
		}
		else if(ret == MENU_SHOW_THEME) //3
		{
			ret=__Show_Theme();
		}
		else if(ret == MENU_HOME) //5
		{
			ret = __Home();
		}
		else if(ret == MENU_DOWNLOAD) //7
		{
			__DownloadDBpng();
			__Free_Channel_Images();
			Fat_Unmount();
			if(Fat_Mount(thememode)<0){
				__Draw_Message(lang[12], -1);
			}
			else
				gprintf("fat mounted %d \n",thememode);
			
			ret = filelist_retrieve(thememode);
			
			__Load_Config();
			
			ret = MENU_SELECT_THEME;
		}
		
		else if(ret == Menu_Start_Themeing) //11
		{
			ret = __DownloadDBfile(ThemeList[selectedtheme].title);//;
		}
		else if(ret == MENU_MAKE_THEME) //12
		{
			ret = __Theme_Make(ThemeList[selectedtheme].title);
		}
		else if(ret == MENU_MANAGE_DEVICE) //6
		{
			//Fat_Unmount();
			thememode =  __Select_Theme_Device();
			gprintf("thememode = %d \n",thememode);
			//__Free_Channel_Images();
			themecnt = 0;
			
			if(Fat_Mount(thememode)<0){
				__Draw_Message(lang[12], -1);
			}
			else
				gprintf("fat mounted %d \n",thememode);
				
			sprintf(tmpstr,"Fat:/config/themewii");
			gprintf("tmpstr = %s \n",tmpstr);
			if(!Fat_CheckFile(tmpstr))
				Fat_MakeDir(tmpstr);
				
			
			
			ret = MENU_SELECT_THEME;
		}
		else if((ret == MENU_EXIT) || (ret == MENU_EXIT_TO_MENU)) // 9, 10
		{
			break;
		}
	}
	gprintf("ret = %d \n",ret);
	
	//if(saveconfig)
	//	__Save_Changes();

		
	return 0;
}
