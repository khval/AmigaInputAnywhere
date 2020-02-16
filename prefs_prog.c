#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/graphics.h>

#include "main.h"
#include "writeconfig.h"

struct Window	*win;
struct IntuiMessage *msg;

/* struct Message *GetMsg(struct MsgPort *) */
/* struct Message *WaitPort(struct MsgPort *); */

char clear_str[]="                       ";

int n = 0;
int quit = 0;

enum
{
	type_keyboard,
	type_mouse
};

void add_code(int o,int c,int q)
{
	config.button[n].o_value = o;
	config.button[n].k_value = msg -> Code;
	config.button[n].q_value = msg -> Qualifier & ~ 0x8000;

	if (n<joypad_buttons-1) 
	{ n++; }
	else
	{ quit=1; }
}

int main (int args,char **arg)
{
	int	code_nr;

	win =  OpenWindowTags(NULL, 
			WA_Left,0,
			WA_Top,0,
			WA_Width,200,
			WA_Height,100,
			WA_Title,"Press a key or mouse button",
			WA_IDCMP, 
				IDCMP_RAWKEY | 
				IDCMP_MOUSEBUTTONS |
				IDCMP_CLOSEWINDOW,
			WA_Flags, 
				WFLG_DRAGBAR |
				WFLG_RMBTRAP |
				WFLG_DEPTHGADGET |
				WFLG_CLOSEGADGET |
				WFLG_ACTIVATE ,
			TAG_END
		);

	if (win)
	{
		while ( 1 )
		{
			SetAPen( win -> RPort,   1);

			Move( win -> RPort,   50,    50);
			Text( win -> RPort, clear_str,strlen(clear_str) );

			Move( win -> RPort,   50,    50);
			Text( win -> RPort, config.button[n].name,strlen(config.button[n].name) );

			WaitPort( win -> UserPort);

			if (msg =(IntuiMessage *) GetMsg(win -> UserPort))
			{
				switch (msg -> Class)
				{
					case IDCMP_RAWKEY:

						if  (!(msg -> Code & IECODE_UP_PREFIX))
						{
							printf("Raw key 0x%X Qualifier 0x%x\n",msg -> Code,msg -> Qualifier & ~ 0x8000);
						
							add_code( type_keyboard, msg -> Code, msg -> Qualifier & ~ 0x8000 );
						}
						break;

					case IDCMP_CLOSEWINDOW:
						quit =1;
						break;

					case 8:
						if  (!(msg -> Code & IECODE_UP_PREFIX))
						{
							printf("Mouse key 0x%X Qualifier 0x%x\n",msg -> Code,msg -> Qualifier & ~ 0x8000);
							add_code( type_mouse, msg -> Code, msg -> Qualifier & ~ 0x8000 );
						}
						else
						{
							printf("Mouse key 0x%X Qualifier 0x%x\n",msg -> Code & ~ IECODE_UP_PREFIX,msg -> Qualifier & ~ 0x8000);
						}
						break;

					default:	
						printf("Unsupported class %X Code 0x%X Qualifier 0x%x\n",msg -> Class, msg -> Code,msg -> Qualifier);
						break;
				}
				ReplyMsg ((struct Message *) msg);
			}
			if (quit) break;
		}

		CloseWindow(win);
	}

	WriteConfig("progdir:config.txt");

	return 0;
}
