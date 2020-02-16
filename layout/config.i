

/* struct List list_buttons2; */

const char *axis_array[]=
	{
		"Mouse XY",
		"Keyboard",
		NULL
	};

struct ColumnInfo config_ci[] =
{
	{ 40, "Name", 0 },
	{ 60, "Action code",0 },
	{ -1, (STRPTR)~0, -1 }
};

BOOL make_cfg_list(struct List *list)
{

	struct Node *node;
	WORD n = 0;

	for (n=0; n<joypad_buttons;n++)
	{
		if (node = AllocListBrowserNode(2,

						LBNA_Column, 0,
							LBNCA_CopyText, TRUE,
							LBNCA_Text, config.button[n].name,
							LBNCA_Editable, FALSE,

						LBNA_Column, 1,
							LBNCA_CopyText, TRUE,
							LBNCA_Text, config.button[n].action,
							LBNCA_Editable, FALSE,

						TAG_DONE))
		{
			AddTail(list, node);
		}
		else break;
	}

	return(TRUE);
}


struct Gadget	*gParent = NULL;

void init_configmenu(Object **win)
{

//		IExec -> NewList(&list_buttons2);

		if (*win) 
		{
			printf("window already open\n");
			return ;
		 }

		*win = WindowObject,
				
				/* these tags describe the window 
				 */
		
				WA_IDCMP, IDCMP_RAWKEY | IDCMP_GADGETUP  | IDCMP_GADGETDOWN,
				WA_Top, 20,
				WA_Left, 20,
				WA_Width, 300,
				WA_Height, 400,
				WA_SizeGadget, TRUE,
				WA_DepthGadget, TRUE,
				WA_DragBar, TRUE,
				WA_CloseGadget, FALSE,
				WA_Activate, TRUE,
								
				WA_Title, "Assign buttons to events",
				WA_ScreenTitle, "AmigaInputAnywhere",
				
				/* Turn on gadget help in the window  */
				
				// WINDOW_GadgetHelp, TRUE,
				
				/* Add an iconification gadget. If you have this, you must listen to
				 * WMHI_ICONIFY.
				 */
				 
				WINDOW_IconifyGadget, FALSE,
				
				/* Below is the layout of the window  */
				
				WINDOW_ParentGroup,	 VGroupObject,
					LAYOUT_SpaceOuter, TRUE,
					LAYOUT_BevelStyle, BVS_THIN,
						
					StartVGroup, BAligned,
						StartMember,  Objects[ga_config_list] =ListBrowserObject,
							GA_ID, ga_config_list,	
							GA_RelVerify, TRUE,
							LISTBROWSER_Labels, &list_buttons, // label_list,
							LISTBROWSER_ColumnInfo, config_ci,
							LISTBROWSER_ColumnTitles, TRUE,
							LISTBROWSER_MultiSelect, FALSE,
							LISTBROWSER_Separators, FALSE,
							LISTBROWSER_ShowSelected, TRUE,
							LISTBROWSER_Editable, FALSE,
						EndMember,
						
						StartHGroup,
							StartVGroup,
								StartMember, RInfo(index_s_text1,"Axis 1"),
								StartMember, RInfo(index_s_text2,"Axis 2"),
								StartMember, SButton(ga_save_config,"Save"), 
							EndGroup,
							CHILD_WeightedHeight, 0,

							StartVGroup,
								StartMember, SChooser(index_o_axis1,"",axis_array,config.axis[0].value),
								StartMember, SChooser(index_o_axis2,"",axis_array,config.axis[1].value),
								StartMember, SButton(ga_close_config,"Close"),
							EndGroup,
							CHILD_WeightedHeight, 0,
						EndGroup,
						CHILD_WeightedHeight, 0,

					EndGroup,					
				EndGroup,
			EndWindow;

//printf("axis value 1: %d\n",config.axis[0].value);
//printf("axis value 2: %d\n",config.axis[1].value);

//	RSetAttrO( win_prefs, index_o_axis1, CHOOSER_Selected, value );
//	RSetAttrO( win_prefs, index_o_axis2, CHOOSER_Selected, value );
}

