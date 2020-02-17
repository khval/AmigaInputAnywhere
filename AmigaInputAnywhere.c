#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/Amigainput.h>
#include <proto/timer.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <proto/requester.h>
#include <proto/commodities.h>

#include <exec/types.h>
#include <exec/ports.h>
#include <exec/interfaces.h>
#include <exec/libraries.h>
#include <exec/devices.h>
#include <devices/input.h>

#define ALL_REACTION_CLASSES
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/icclass.h>
#include <intuition/gadgetclass.h>

#include <utility/tagitem.h> 

void update_cfg_list();

#include "joy_input.h"
#include "prefs.h"
#include "configfile.h"
#include "main.h"
#include "mem.h"

char *files[max_files];

extern int files_count;
extern struct List list_files;

enum
{
	index_null,
	index_s_text1,
	index_s_text2,
	index_o_axis1,
	index_o_axis2,
	ga_files_list,
	ga_config_list,
	ga_new_config,
	ga_edit_config,
	ga_close_config,
	ga_save_config,
	ga_delete_config,
	ga_quit,
	index_end
};


enum
{
	win_null,
	win_main,
	win_configmenu,
	win_end
};

Object		layout[win_end];
struct Window	*win[win_end];

Object		*Objects[index_end];
char			info_str[100];
char			*cfg_filename = NULL;
int			 gad_height;
int			wh,ww;

struct Screen	*scr;
struct Gadget	*GadgetHit;
struct List list_buttons;

struct ai_type *aio = NULL;

extern void close_ainput( struct ai_type **ai_io );
extern int get_code (int code_nr);

void open_menu(int layout_nr, void (*fn) (Object **win_layout) );
int close_menu(int layout_nr);

/* a simple button - there are lots more macros to be found like this in reaction/reaction_macros.h */

#define SButton(n,txt) Objects[n] =ButtonObject, \
	GA_ID, n, \
	GA_Text, txt, \
	GA_RelVerify, TRUE, \
	ButtonEnd

#define ROButton(n,txt) Objects[n] =ButtonObject, \
	GA_ID, n, \
	GA_Text, txt, \
	GA_RelVerify, TRUE, \
	GA_ReadOnly,TRUE, \
	ButtonEnd

#define DButton(n,txt) Objects[n] =ButtonObject, \
	GA_ID, n, \
	GA_Text, txt, \
	GA_RelVerify, TRUE, \
	GA_Disabled, TRUE, \
	ButtonEnd

#define SString(n,txt) Objects[n] = StringObject, \
	GA_ID, n, \
	GA_Text, txt, \
	GA_RelVerify, TRUE, \
	StringEnd 

#define ROString(n,txt) Objects[n] = StringObject, \
	GA_ID, n, \
	GA_Text, txt, \
	GA_RelVerify, TRUE, \
	GA_ReadOnly, TRUE, \
	StringEnd 

#define RLabel(n,text) \
        Objects[n] = LabelObject, \
            LABEL_Text, text, \
        LabelEnd

#define RInfo(n,txt) Objects[n] = StringObject, \
	GA_ID, n, \
	STRINGA_TextVal, txt, \
	GA_RelVerify, TRUE, \
	GA_ReadOnly, TRUE, \
	StringEnd 

#define SFuel(n,txt) Objects[n] = FuelGaugeObject,  \
	GA_ID, n,	\
	GA_Text, txt, \
	FUELGAUGE_Ticks, 80, \
	FUELGAUGE_ShortTicks, 2, \
	GA_RelVerify, TRUE, \
	ButtonEnd 

#define SChooser(n,txt,array,num) Objects[n] = ChooserObject, \
	GA_ID, n, \
	GA_Text, txt, \
	GA_RelVerify, TRUE, \
	CHOOSER_LabelArray, array, \
	CHOOSER_Selected, num, \
	ChooserEnd 

#define RDisable(index,win_nr) \
	if ( win[win_nr])	SetGadgetAttrs( (struct Gadget *) Objects[index], \
		win[win_nr],  NULL,  GA_Disabled, TRUE, TAG_DONE); 

#define REnable(index,win_nr) \
	if ( win[win_nr]) SetGadgetAttrs( (struct Gadget *) Objects[index], \
		 win[win_nr], NULL, GA_Disabled, FALSE, TAG_DONE); 

#define xxRDetach(index,win_nr) \
	if ( win[win_nr])	SetGadgetAttrs( (struct Gadget *) Objects[index], win[win_nr], NULL, \
		 LISTBROWSER_Labels, ~0, TAG_DONE); 

#define RDetach(index,win_nr) \
	if (win[win_nr]) SetGadgetAttrs( (struct Gadget *) Objects[index], win[win_nr] , NULL, \
		LISTBROWSER_Labels, ~0, \
		TAG_DONE); 

#define RAttach(index,win_nr,list) \
	if (win[win_nr]) SetGadgetAttrs( (struct Gadget *) Objects[index], win[win_nr], NULL, \
	LISTBROWSER_Labels, list, \
	LISTBROWSER_MultiSelect, FALSE, \
	TAG_DONE); 

int req(const char *title,  const char *body, const char *buttons, ULONG image)
{  
        Object *req = 0;                                // the requester itself
        int reply;                                      // the button that was clicked by the user

	// fill in the requester structure
	req = NewObject(REQUESTER_GetClass(), NULL, 
		REQ_Type,       REQTYPE_INFO,
		REQ_TitleText,  (ULONG)title,
		REQ_BodyText,   (ULONG)body,
		REQ_GadgetText, (ULONG) buttons ,
		REQ_Image,      image,
		TAG_DONE);
        
	if (req) 
	{ 
		reply = IDoMethod(req, RM_OPENREQ, NULL, NULL, NULL);     
		DisposeObject(req);
	}  else printf("[request] Failed to allocate requester\n");

	return( reply ); // give the button number back to the caller
}


#include "layout/main.i"
#include "layout/config.i"

void set_cfg_filename( const char *name)
{
	const char *fmt = "progdir:config/%s";

	if (cfg_filename) 
	{
//		printf("%s\n",cfg_filename);
		 FreeVec(cfg_filename);
		cfg_filename = NULL ;
	}

	if (cfg_filename = (char *) allocShared( strlen(fmt) + strlen(name) + 1 ) )
	{
		sprintf(cfg_filename,fmt,name);
		ReadConfig(cfg_filename);
	}
}


int event(int id, int code)
{
	char *fmt;


	switch(id)
	{
		case index_o_axis1:		config.axis[0].value=code; break;
		case index_o_axis2:		config.axis[1].value=code; break;

		case ga_edit_config:
//							printf("**0**\n");
							RDisable(ga_edit_config, win_main);
							open_menu(win_configmenu,init_configmenu); 
							break;

		case ga_new_config:
//							printf("**1**\n");

							RDetach(ga_config_list,win_configmenu);		
							update_cfg_list(); 
							break;

		case ga_close_config:
//							printf("**2**\n");

							close_menu(win_configmenu);
							REnable(ga_edit_config, win_main);
							break;

		case ga_save_config:
//							printf("**3**\n");

							if  (cfg_filename)
							{
								WriteConfig(cfg_filename);
							}
							break;

		case ga_files_list:
//							printf("**4**\n");

							if (win[win_configmenu])
							{
								close_menu(win_configmenu);
								REnable(ga_edit_config, win_main);
							}

							set_cfg_filename(files[code]);
							update_cfg_list();
							break;

		case ga_config_list:
//							printf("**5**\n");
	
							RDetach(ga_config_list,win_configmenu);
							RDisable(ga_config_list,win_configmenu);

							get_code ( code );

							update_cfg_list(); 

							RAttach(ga_config_list,win_configmenu,&list_buttons);
							REnable(ga_config_list,win_configmenu);

							break;

		case ga_quit:	return TRUE;

		default:			printf("event id %x code %x\n",id,code);
							break;
	}
	return FALSE;
}


int get_r_event( int (*fn_event) (int id,int code) )
{
	int n,result,code;
	int done = FALSE;

	for (n=1;n<win_end;n++)
	{

		if ( layout[n] != 0 )
		{
			while ((result = RA_HandleInput( (Object *) layout[n] ,&code)) != WMHI_LASTMSG)
			{
				Delay(1);

//				printf("win %d\n",n);
//				printf("result %04X\n",result);

				switch(result & WMHI_CLASSMASK)
				{
					case WMHI_GADGETUP:
//						printf("gadget up\n");

						done = fn_event(result & WMHI_GADGETMASK,code / 0x10000);

//						printf("gadget done\n");
						break;

					case WMHI_ICONIFY:
//						printf("Inconify\n");

						if (RA_Iconify( (Object *) layout[ win_main ] ))
						win[ win_main ] = NULL;
						break;
								 
					case WMHI_UNICONIFY:
//						printf("uniconify\n");

						win[ win_main ] = RA_OpenWindow( (Object *) layout[ win_main] );
						break;

					case WMHI_CLOSEWINDOW:

//						printf("closewindow\n");
						done = TRUE;

//						printf("Closewindow %x\n",result & ~ WMHI_CLASSMASK );

						break;
				}
			}
		}
	}
	return done;
}



int close_menu(int layout_nr)
{
	if (layout[ layout_nr ])
	{
		DisposeObject( (Object *) layout[ layout_nr ] );
		layout[ layout_nr ]	= 0;
		win[ layout_nr ]	= 0;
	}
}

void open_menu(int layout_nr, void (*fn) (Object **win_layout) )
{
	printf("layout %d\n",layout_nr);

	if (layout[ layout_nr ] == 0)
	{
		printf("%s:%d\n",__FUNCTION__,__LINE__);

		fn( (Object **) &layout[ layout_nr ] ); 

		printf("%s:%d\n",__FUNCTION__,__LINE__);

		win[layout_nr] = RA_OpenWindow( (Object *) layout[ layout_nr ] );

		printf("win[%d] is %08x\n",layout_nr,win[layout_nr]);
	}
}

unsigned int DefaultIDCMP= 0x200|0x40|0x20;

char *R_get_str(int index,int win_nr)
{
	char *text = NULL;

	if (GadgetHit = (struct Gadget *) Objects[index] )
	{
		SetGadgetAttrs( 
			GadgetHit, 
			win[win_nr], 
			NULL, 
			STRINGA_TextVal, &text, 
			TAG_DONE); 
	}

	return text;
}

void R_put_str(int id,int win_nr,char *text)
{
	if (GadgetHit =  (struct Gadget *) Objects[id] )
	{
		SetGadgetAttrs( 
			GadgetHit,
			win[win_nr],
			NULL,
			STRINGA_TextVal, text,
			TAG_DONE);
	}
}


struct RastPort *rp;

void update_actions();

void update_cfg_list()
{
	RDetach(ga_config_list,win_configmenu);

	update_actions();
	FreeListBrowserList(&list_buttons);

	make_cfg_list(&list_buttons);
	RAttach(ga_config_list,win_configmenu,list_buttons);
}

void init_buttons();

extern bool startProg();
extern void endProg();

int main_safe();

int main()
{
	ULONG ret;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (startProg() == FALSE)
	{
	printf("%s:%d\n",__FUNCTION__,__LINE__);

		endProg();

	printf("%s:%d\n",__FUNCTION__,__LINE__);

		return 1;
	}

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	ret = main_safe();

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	endProg();

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	return ret;
}

void init_vars()
{
	int i;

	for (i=win_null;i<win_end;i++)
	{
		win[i] = NULL;
		layout[i] = NULL;
	}

	NewList(&list_buttons);
	NewList(&list_files);
}

int main_safe()
{
	char	*pbak;
	char *a;
	char *c;
	char *e;
	int 	i,x,y;
	int 	ev;
	int	gh;

	char	*tmp_str;
	int	format_ww;
	int 	rows;
	int	centerw;
	int 	quit		= FALSE;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	init_vars();

	scr = LockPubScreen(NULL);
	if (!scr) return FALSE;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

//  check if path exists!

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	make_cfg_files_list(&list_files);

	printf("%s:%d\n",__FUNCTION__,__LINE__);

 	init_buttons();

	set_cfg_filename( "Default");

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	printf("cfg_filename: %s\n",cfg_filename);

	ReadConfig(cfg_filename);

	printf("files_count: %d\n", files_count);

	printf("%s:%d\n",__FUNCTION__,__LINE__);
	Delay(1);

	update_cfg_list();

	printf("%s:%d\n",__FUNCTION__,__LINE__);
	Delay(1);

	open_menu(win_main,init_mainmenu);

	printf("%s:%d\n",__FUNCTION__,__LINE__);
	Delay(1);

	if (!OpenDevice("input.device",0, (struct IORequest *) io_req,0L))
	{

		printf("%s:%d\n",__FUNCTION__,__LINE__);
		Delay(1);

		if ( aio = init_ainput() )
		{
			ULONG sigs =0L;
			ULONG joy_sig = 1L << aio -> msgport -> mp_SigBit;
			ULONG connected = 0;
			ULONG joyEvent;

printf("%s:%d\n",__FUNCTION__,__LINE__);

			do
			{
				sigs = SetSignal(0L, 0L);

				if ( sigs & joy_sig )
				{
					printf("got joystick event\n");
					if (aio -> connected) joy_input(io_req);
					SetSignal(0L, joy_sig);		// clear signal
				}
				else
				{
					AIN_Query( aio -> handle , aio -> id, AINQ_CONNECTED, 0, &(aio -> connected), sizeof(ULONG) );

					printf("connected: %d\n", aio -> connected);

					if (aio -> connected == FALSE) 
					{
						printf("quiting...\n");
						quit =true;
					}
				}

				if (!quit) quit = get_r_event(event);
				Delay(1);
			}
			while(!quit);

			close_ainput( &aio );
		}
		CloseDevice( (struct IORequest *) io_req);
	}

	if (scr) 
	{
		UnlockPubScreen(NULL,scr);
		scr = NULL;
	}

	for (i=1;i<win_end;i++)	 close_menu(i);

	return 0;

}
