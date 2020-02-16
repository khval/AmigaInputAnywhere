
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <intuition/intuition.h>

#include "main.h"

struct Window	*win_get_code;
struct IntuiMessage *msg;


char clear_str[]="                      ";

int quit = 0;

enum
{
	type_keyboard,
	type_mouse
};

void change_code(int n,int o,int c,int q)
{
	config.button[n].o_value = o;
	config.button[n].k_value = msg -> Code;
	config.button[n].q_value = msg -> Qualifier & ~ 0x8000;
}

int get_code (int code_nr)
{

	struct RastPort *rp;
	int x,y,w,h;

	win_get_code = OpenWindowTags(NULL, 
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

	Delay(20);

	if (!win_get_code) return 0;

	rp = win_get_code -> RPort;
	w = win_get_code -> Width;
	w -= win_get_code -> BorderLeft;
	w -= win_get_code -> BorderRight;

	h = win_get_code -> Height;
	h -= win_get_code -> BorderTop;
	h -= win_get_code -> BorderBottom;

	x = win_get_code -> BorderLeft;
	y = win_get_code -> BorderTop;

	if (win_get_code)
	{
		while ( 1 )
		{
			SetAPen( rp,   1);

			Move( rp, 0, 0 );
			SetAPen( rp, 2 );
			SetBPen( rp, 1 );
			RectFill( rp, x, y, x+w, y+h );

			Move( rp,   50,    50);
			Text( rp, config.button[code_nr].name,strlen(config.button[code_nr].name) );

			WaitPort( win_get_code -> UserPort);

			if (msg =(IntuiMessage*) GetMsg(win_get_code -> UserPort))
			{
				switch (msg -> Class)
				{
					case IDCMP_RAWKEY:

						if  (!(msg -> Code & IECODE_UP_PREFIX))
						{
							printf("Raw key 0x%X Qualifier 0x%x\n",msg -> Code,msg -> Qualifier & ~ 0x8000);
							change_code( code_nr, type_keyboard, msg -> Code, msg -> Qualifier & ~ 0x8000 );
							quit = 1;
						}
						break;

					case 8:
						if  (!(msg -> Code & IECODE_UP_PREFIX))
						{
							printf("Mouse key 0x%X Qualifier 0x%x\n",msg -> Code,msg -> Qualifier & ~ 0x8000);
							change_code( code_nr, type_mouse, msg -> Code, msg -> Qualifier & ~ 0x8000 );
							quit = 1;
						}
						break;

					case IDCMP_CLOSEWINDOW:
						quit =1;
						break;

					default:	
						printf("Unsupported class %X Code 0x%X Qualifier 0x%x\n",msg -> Class, msg -> Code,msg -> Qualifier);
						break;
				}
				ReplyMsg ((struct Message *) msg);
			}
			if (quit) break;
		}

		CloseWindow( win_get_code );
	}

	return 0;
}
