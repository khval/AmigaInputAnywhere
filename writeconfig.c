
#include <proto/exec.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>
#include "main.h"

void WriteConfig(const char *fname)
{
	int		n;
	FILE		*fd;

	if (fd = fopen(fname,"w"))
	{              
//		printf("write file %c%s%c open!\n",34,fname,34);

		fprintf(fd,"; Raw key codes\n\n");

		for ( n=0; n< joypad_buttons; n++ )
		{
				fprintf(fd,"%s= %d,%d,%d\n", 
					config.button[n].name, 
					config.button[n].o_value,
					config.button[n].k_value, 
					config.button[n].q_value);
		}

		for ( n=0; n< joypad_axis; n++ )
		{
				fprintf(fd,"%s= %d\n", 
					config.axis[n].name, 
					config.axis[n].value);
		}

		fclose(fd);
	}
}
