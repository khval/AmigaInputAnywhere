

typedef struct 
{
	int k_value;
	int q_value;
	int o_value;
	char *name;
	char *action;
} button_type ;

typedef struct 
{
	char *name;
	int  value;
} axes_type ;

#define joypad_buttons 16
#define joypad_axis 2

typedef struct
{
	button_type button[ joypad_buttons ];
	axes_type	axis[ joypad_axis ];

} config_type;

extern config_type config;

extern struct ai_type *init_ainput();
extern int req(char *title,  char *body, char *buttons, ULONG image);

