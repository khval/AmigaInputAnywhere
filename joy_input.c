
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

#include "main.h"
#include "joy_input.h"

extern struct ai_type *aio;

int axis_max_value = 0;

int l_mouse_x = 0;
int l_mouse_y = 0;
int mouse_x = 0;
int mouse_y = 0;

int MQualifier = 0;
int KQualifier = 0;
int mouse_left = 0;
int mouse_right = 0;

#include "readconfig.h"
#include "writeconfig.h"
#include "init_amigainput.h"
#include "mem.h"

#define free_mem(x) if (x) FreeVec(x); x = NULL;

void put_key_event(struct IOStdReq *io_req, int code)
{
	struct InputEvent *my_event;

	my_event = (struct InputEvent*) allocShared( sizeof(struct InputEvent) );

	if (!my_event) goto q;

        io_req -> io_Command =IND_ADDEVENT;
        io_req -> io_Flags = IOB_QUICK;				// set if quick I/O is possible
        io_req -> io_Length = sizeof(struct InputEvent);
        io_req -> io_Data = (void *) my_event;

	my_event -> ie_Class =IECLASS_RAWKEY;
	my_event -> ie_SubClass = 0;
	my_event -> ie_Code = code;
	my_event -> ie_Qualifier = KQualifier;

	DoIO( (struct IORequest *) io_req);

	free_mem( my_event );
q:
	return;
}


/*
        IECLASS_RAWMOUSE events will be merged if their qualifiers
        match. The ie_X and ie_Y delta values will be added up.
        A relationship between qualifiers and the ie_Code value is
        assumed: the ie_Code value of the first event is copied
        to all following input events which share the same set of
        qualifiers and are of the IECLASS_RAWMOUSE type.
*/


void put_mouse_event(struct IOStdReq *io_req, int x,int y)
{
	struct InputEvent *my_event;

	my_event = (struct InputEvent*) allocShared(sizeof(struct InputEvent) );
	if (!my_event) goto q;

        io_req -> io_Command =IND_ADDEVENT;
        io_req -> io_Flags = IOB_QUICK;				// set if quick I/O is possible
        io_req -> io_Length = sizeof(struct InputEvent);
        io_req -> io_Data = (void *) my_event;

	my_event -> ie_Class =IECLASS_RAWMOUSE;
	my_event -> ie_position.ie_xy.ie_y = y;
	my_event -> ie_position.ie_xy.ie_x = x;
	my_event -> ie_Qualifier = MQualifier;

	DoIO( (struct IORequest *) io_req);

	free_mem( my_event );
q:
	return;
}

void put_mouse_key_event(struct IOStdReq *io_req,int code)
{
	struct InputEvent *my_event;

	if (!io_req) return;

	my_event = (struct InputEvent*) allocShared(sizeof(struct InputEvent));
	if (!my_event) return;

        io_req -> io_Command =IND_ADDEVENT;
        io_req -> io_Flags = IOB_QUICK;				// set if quick I/O is possible
        io_req -> io_Length = sizeof(struct InputEvent);
        io_req -> io_Data = (void *) my_event;

	my_event -> ie_Class	= IECLASS_RAWMOUSE;
	my_event -> ie_Code	= code;
	my_event -> ie_Qualifier = MQualifier;

	DoIO( (struct IORequest *) io_req);

	free_mem( my_event );
}

enum
{
	pad_zero,
	pad_up,
	pad_down,
	pad_left,
	pad_right
};


#define inpad(v,a,b,c) (v==a)||(v==b)||(v==c)

int dig_pad[4];

void mouse_xy_emu(int n, int value)
{
	switch (n&1)
	{
		case 0: mouse_x = value / 2560 ; break;
		case 1: mouse_y = value / 2560 ; break;
	}
}


void send_x_arrow_keys(struct IOStdReq *kio,int xpad)
{
	int n;

	if (xpad>0)
	{
		n = xpad - 1;
		if (dig_pad[n]==0)
		{
			KQualifier |= config.button[n].q_value; 	// set qualifier;
			put_key_event(kio,config.button[n].k_value); 
//			printf("%d %x %x\n",n, buttons[n].code[0], KQualifier);
			dig_pad[n] = 1;
		}
	}
	else // if not pressed send unpress event
	{
		for (n=pad_left-1;n<=pad_right-1;n++)
		{
			if (dig_pad[n])
			{
				KQualifier &= ~ config.button[n].q_value; // remove qualifier;
				put_key_event(kio,config.button[n].k_value | IECODE_UP_PREFIX ); 
//				printf("%d %x %x\n",n, buttons[n].code[0], KQualifier);
				dig_pad[n]=0;
			}
		}
	}
}

void send_y_arrow_keys(struct IOStdReq *kio,int ypad)
{
	int n;
	if (ypad>0)
	{
		n = ypad - 1;
		if (dig_pad[n]==0)
		{

			KQualifier |= config.button[n].q_value; 	// set qualifier;
			put_key_event(kio,config.button[n].k_value); 
//			printf("%d %x %x\n",n, config.button[n].k_code, KQualifier);
			dig_pad[n] = 1;
		}
	}
	else // if not pressed send unpress event
	{
		for (n=pad_up-1;n<=pad_down-1;n++)
		{
			if (dig_pad[n])	// the key that where pressed 
			{
				KQualifier &= ~ config.button[n].q_value; // remove qualifier;
				put_key_event( kio, config.button[n].k_value | IECODE_UP_PREFIX );
//				printf("%d %x %x\n",n, config.button[n].k_value, KQualifier);
				dig_pad[n]=0;
			}
		}
	}
}

void axis_to_key(struct IOStdReq *kio,int n, int value)
{
	int	xpad = pad_zero; 
	int 	ypad = pad_zero;

	if (value>axis_max_value) axis_max_value = value;
	if (-value>axis_max_value) axis_max_value = -value;

	if ((value>(-axis_max_value/2)) && (value<(axis_max_value/2)) ) value = 0;

	switch (n&1)
	{
		case 0:	if (value<0)
				{ xpad = pad_left; }
				else if (value>0) 
				{ xpad = pad_right; }

				send_x_arrow_keys(kio,xpad);
				break;

		case 1:	if (value<0)
				{ ypad = pad_up; }
				else if (value>0) 
				{ ypad = pad_down; }

				send_y_arrow_keys(kio,ypad);
				break;
	}
}

int joy_input(struct IOStdReq *kio)
{
	int xpad,ypad,n,q,cfg,code;
	int *Qualifier;

	AIN_InputEvent *ain_mess;

	l_mouse_x = mouse_x;
	l_mouse_y = mouse_y;

	if (!kio) return 0;

	if (aio)	
	{
	Printf("%s:%ld\n",__FUNCTION__,__LINE__);

		while ( ain_mess = AIN_GetEvent (aio -> controller))
		{

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);

			switch (ain_mess -> Type)
			{

	Printf("ain_mess -> Value: %d\n",ain_mess -> Value);

				case AINET_AXIS: // axes

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);
/*	
					n = ain_mess -> Index / 2;

					cfg = 0;

					if (n<2) cfg = config.axis[n].value;


					switch(cfg)
					{
						case 0: mouse_xy_emu(ain_mess -> Index & 1, ain_mess -> Value ); break;
						case 1: axis_to_key(kio,ain_mess -> Index & 1, ain_mess -> Value); break;
					}
*/
					break;

				case AINET_BUTTON:

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);
/*
					n = ain_mess -> Index - 1 ;
//					printf("Button %d value %d\n", n - 3 , ain_mess -> Value ); 

					cfg	= config.button[n].o_value;
					code	= config.button[n].k_value;
					q	= config.button[n].q_value;

					switch(cfg)
					{
						case 0:	Qualifier = &KQualifier ; break;
						case 1:	Qualifier = &MQualifier ; break;
						default:	printf("ERROR: Bad config not mouse or keyboard event?\n");
					}

					switch (ain_mess -> Value) // set qualifier
					{
						case 1:	*Qualifier |= q; 
								break;

						case 0:	*Qualifier &= ~ q; 
 								code |= IECODE_UP_PREFIX;
								break;
					}

					switch(cfg)
					{
						case 0:	put_key_event(kio,code); break;
						case 1:	put_mouse_key_event(kio,code); break;
						default:	printf("ERROR: Bad config not mouse or keyboard event?\n");
					}
*/
					break;
//     8.1.2
//     7.0.3
//     6.5.4
				case AINET_HAT:

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);
/*
					xpad = pad_zero; 
					ypad = pad_zero;

					if (inpad(ain_mess -> Value,8,1,2)) ypad = pad_up;

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);

					if (inpad(ain_mess -> Value,6,5,4)) ypad = pad_down;

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);

					if (inpad(ain_mess -> Value,8,7,6)) xpad = pad_left;

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);

					if (inpad(ain_mess -> Value,2,3,4)) xpad = pad_right;

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);

					send_x_arrow_keys(kio,xpad);

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);

					send_y_arrow_keys(kio,ypad);

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);
*/

					break;

			}

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);

			AIN_FreeEvent(aio -> controller,ain_mess);
		}
	}

	Printf("%s:%ld\n",__FUNCTION__,__LINE__);

//	put_mouse_event(kio,mouse_x,mouse_y);

	return 0;
}



