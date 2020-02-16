
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include "mem.h"
#include "main.h"

config_type	config =
{
	{
		{0,0,0,"Up",NULL},			// 0
		{0,0,0,"Down",NULL},		// 1
		{0,0,0,"Left",NULL}, 		// 2 
		{0,0,0,"Right",NULL},		// 3

		{0,0,0,"Button 1",NULL},	// 4
		{0,0,0,"Button 2",NULL},	// 5
		{0,0,0,"Button 3",NULL},	// 6
		{0,0,0,"Button 4",NULL},	// 7
		{0,0,0,"Button 5",NULL},	// 8
		{0,0,0,"Button 6",NULL},	// 9
		{0,0,0,"Button 7",NULL},	// 10
		{0,0,0,"Button 8",NULL},	// 11
		{0,0,0,"Button 9",NULL},	// 12
		{0,0,0,"Button 10",NULL},	// 13
		{0,0,0,"Button 11",NULL},	// 14
		{0,0,0,"Button 12",NULL}	// 15

	},
	{
		{"Axis 1",0},
		{"Axis 2",0}
	}
};

void set_action_text(button_type *button)
{
	if (button -> o_value == 0)
	{
		switch (button -> q_value)
		{
			case IEQUALIFIER_LSHIFT:		sprintf( button -> action, "L SHIFT");	break;
			case IEQUALIFIER_RSHIFT:		sprintf( button -> action, "R SHIFT");	break;
			case IEQUALIFIER_CAPSLOCK:		sprintf( button -> action, "CAPSLOCK");	break;
			case IEQUALIFIER_CONTROL:		sprintf( button -> action, "CTRL");		break;
			case IEQUALIFIER_LALT:			sprintf( button -> action, "L ALT");		break;
			case IEQUALIFIER_RALT:			sprintf( button -> action, "R ALT");		break;
			case IEQUALIFIER_LCOMMAND:	sprintf( button -> action, "L Amiga");	break;
			case IEQUALIFIER_RCOMMAND:	sprintf( button -> action, "R Amiga");	break;

			default:

				if (button -> q_value)
				{
					sprintf( button -> action, "Code %d Qualifier %d", button -> k_value,button -> q_value );
				}
				else
				{
					sprintf( button -> action, "Code %d", button -> k_value );
				}
				break;
		}
	}
	else
	{
		sprintf( button -> action, "Mouse button %d                         ",button -> k_value - 103);
	}
}

void init_buttons()
{
	int n; 

	for (n=0;n<joypad_buttons;n++)
	{
		config.button[n].action =(char *) allocShared( 100 );
		set_action_text(&config.button[n]);
	}
}

void update_actions()
{
	int n; 

	for (n=0;n<joypad_buttons;n++)
	{
		set_action_text(&config.button[n]);
	}
}
