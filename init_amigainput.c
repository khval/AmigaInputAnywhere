
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/Amigainput.h>
#include <proto/intuition.h>
#include <proto/requester.h>

#include "main.h"
#include "mem.h"
#include "joy_input.h"

extern int req(const char *title,  const char *body, const char *buttons, ULONG image);

// becose AmigaInput does not support -lauto yet :-(

extern struct Library		*AIN_Base;
extern struct AIN_IFace	*IAIN;

typedef struct
{
	APTR	context;
	int		get_count;
	uint32  	count;
	int		id;
} enumPacket;

enumPacket userdata;

static BOOL get_joy (AIN_Device *device, struct ai_type *joy)
{
	BOOL ret = FALSE;
	int connected = 0;

	if (joy==NULL) return FALSE;
	if (joy ->controller == NULL) return FALSE;

	Printf("devce Type %ld \tID %lx \tdevce Name %s \n",device->Type , device -> DeviceID,device -> DeviceName);

	if (device->Type == AINDT_JOYSTICK) 
	{
		AIN_Query(joy ->controller, device -> DeviceID,AINQ_CONNECTED,0,&connected, sizeof(connected) );
		if (connected)
		{
			ret = TRUE;
			joy -> id = device -> DeviceID;
		}
	}

	return ret;
}

void close_ainput( struct ai_type **ai_io )
{
	int res;
	struct TagItem AI_tags[3];

	if (*ai_io == NULL)
	{
		printf("Nothing to cleanup after Amiga Input... exit quickly...\n");
		return;
	}

	if ((ai_io[0] ->controller) && (ai_io[0] -> handle))
	{
		Printf("disable Events\n");
		
		res =  AIN_SetDeviceParameter(ai_io[0] -> controller,ai_io[0] -> handle,AINDP_EVENT,FALSE);
		Delay(1);


		Printf("ReleaseDevice\n");

 		AIN_ReleaseDevice( ai_io[0] ->controller,ai_io[0]->handle );


	}

	if (ai_io[0] -> controller)
	{
		Printf("delete amiga input context\n");

		AIN_DeleteContext( ai_io[0]->controller );
		ai_io[0] -> controller = NULL;
	}

	if (ai_io[0] -> msgport)
	{
		Printf("free amiga input msgport\n");

		FreeSysObject(ASOT_PORT, ai_io[0] -> msgport );
		ai_io[0] -> msgport = NULL;
	}

	Printf("free amiga input IO\n");

	FreeVec( *ai_io );
	*ai_io = NULL;
}



struct TagItem AIN_Tags[]=
	{
		{AINCC_Port,0},
		{TAG_END,0}
	};

/*
struct TagItem
{
    Tag   ti_Tag;  
    ULONG ti_Data; 
};
*/

struct TagItem AI_tags[3];

struct ai_type *init_ainput()
{
	int			ret = 0;
	struct ai_type	*ai_io = NULL;
	int			res = 0;
	int			fail_count = 0;
	int			connected;

	ai_io = (ai_type *) allocShared(sizeof(struct ai_type));
	if (!ai_io) return NULL;

	ai_io->msgport = (MsgPort*) AllocSysObjectTags(ASOT_PORT, TAG_DONE);
	if (!ai_io->msgport) goto fail_whit_error;

	ai_io->controller = AIN_CreateContext (1, NULL);
	if (!ai_io->controller) goto fail_whit_error;

	ai_io -> id = -1;
	AIN_EnumDevices(ai_io->controller, (void *) get_joy, ai_io );

	if (ai_io -> id == -1)
	{
		req("AmigaInput failed",  "You need to configure AmigaInput first,\n\ncheck out the prefs program\n", "Exit", 3);
		goto fail_whit_out_error;
	}
	else
	{
		 printf("Using device ID %x\n",ai_io -> id);
	}

	ai_io-> handle = AIN_ObtainDevice(ai_io->controller, ai_io -> id );
	if (!ai_io->handle) goto fail_whit_error; 

	AIN_Tags[0].ti_Data = (ULONG) ai_io->msgport;
	AIN_Set(ai_io->controller,AIN_Tags);

	res =  AIN_SetDeviceParameter(ai_io -> controller,ai_io -> handle,AINDP_EVENT,TRUE);

	printf("Amiga Input Ready!\n");
	return ai_io;

	fail_whit_error:

	req("AmigaInput failed",  "general error\n", "Exit", 3);

	fail_whit_out_error:

	close_ainput(&ai_io);
	return NULL;
}
