
#include <proto/exec.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "configfile.h"
#include "mem.h"

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

#include "readconfig.h"

int files_count = 0;

struct List list_files;


void only_name(char *txt)
{
	for (;*txt!=0;txt++) 
	{
		if (*txt == '=') *txt = 0;
	}
}

void make_cfg_files_list(struct List *list)
{
	APTR 			context;
	struct ExamineData *fnode = NULL;
	struct Node *		node;

	if (context = ObtainDirContextTags( EX_StringNameInput, "progdir:config", TAG_END))
	{
		while (fnode = ExamineDir(context))
		{
			if ( EXD_IS_FILE(fnode ) )
			{
//				printf("Read file: %s\n",fnode -> Name );
				files[files_count] = strdup(fnode -> Name);
				files_count ++;

				if (node = AllocListBrowserNode(1,
						LBNA_Column, 0,
							LBNCA_CopyText, TRUE,
							LBNCA_Text, fnode -> Name,
							LBNCA_Editable, FALSE,

						TAG_DONE))
				{
					AddTail(list, node);
				}
			}
		} 
		ReleaseDirContext(context);
	}

	return;
}

void get_number_array(char *txt,int *num)
{
	int c = 0 , n = 0;

	for (c=0;*txt!=0;)
	{
		for(;*txt!=0;txt++)
		{
			if ((*txt>='0')&&(*txt<='9')) { n*=10; n+=(*txt-'0'); }

			if (*txt=='=') n = 0;
			if (*txt==',') { txt++; break; }
		}
		num[c]=n;
		n = 0 ;
		c++;
	}
}

void ReadConfig(char *fname)
{
	char		*e;
	int		buf_size;
	int		n,nn,num;
	int		tmp_n[4];
	FILE		*fd;

	buf_size = 1000;
	e=(char *) allocShared(buf_size);
	if (!e) return;

	if (fd = fopen(fname,"r"))
	{              
		while (!feof(fd))
		{
			fgets(e,buf_size,fd);
			get_number_array(e,tmp_n);
			only_name(e);

			for ( n=0; n < joypad_buttons; n++ )
			{
				if (strcasecmp(config.button[n].name,e)==0)
				{
					for (nn=0;nn<3;nn++)
					{
						switch (nn)
						{
							case 0: config.button[n].o_value = tmp_n[nn]; break;
							case 1: config.button[n].k_value = tmp_n[nn]; break;
							case 2: config.button[n].q_value = tmp_n[nn]; break;
						}
					}
				}
			}


			for ( n=0; n < joypad_axis; n++ )
			{
				if (strcasecmp(config.axis[n].name,e)==0)
				{
					config.axis[n].value = tmp_n[0];
				}
			}
			e[0]=0;

		}
		fclose(fd);
	}
	else
	{
		printf("Can't open file %c%s%c\n",34,fname,34);
	}

	FreeVec (e);
}
