

char *button_array[]=
	{
		"Keyboard",
		"Mouse button1",
		"Mouse button2",
		"Mouse Button3",
		NULL
	};

char *axis_array[]=
	{
		"Mouse XY",
		"Keyboard",
		NULL
	};

void init_configmenu(Object **win)
{
	*win = WindowObject,
				
		/* these tags describe the window  */
		WA_IDCMP, IDCMP_RAWKEY | IDCMP_GADGETUP  | IDCMP_GADGETDOWN,
		WA_Top, 20,
		WA_Left, 20,
		WA_Width, 300,
		WA_DepthGadget, TRUE,
		WA_DragBar, TRUE,
		WA_CloseGadget, FALSE,
		WA_Activate, TRUE,
		WA_Title, "Assign AmigaInput",
		WA_ScreenTitle, "",
	
		WINDOW_IconifyGadget, TRUE,
		WINDOW_AppPort, appport,
		WINDOW_AppWindow, TRUE,

		WINDOW_Icon, IIcon->GetDiskObject( "PROGDIR:CatFloppy" ),
		WINDOW_IconTitle, "CatFloppy",

		/* Below is the layout of the window  */		
		WINDOW_ParentGroup,	
			VGroupObject,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_BevelStyle, BVS_THIN,
					
			StartVGroup, TAligned,

				StartHGroup, TAligned,

					StartVGroup,
						StartMember, RInfo(index_s_text1,"Button 1"),
						StartMember, RInfo(index_s_text2,"Button 2"),
						StartMember, RInfo(index_s_text3,"Button 3"),
						StartMember, RInfo(index_s_text4,"Axis 1"),
						StartMember, RInfo(index_s_text4,"Axis 2"),
					EndGroup,
					CHILD_WeightedHeight, 0,

					StartVGroup,
						StartMember, SChooser(index_o_button1,"",button_array),
						StartMember, SChooser(index_o_button2,"",button_array),
						StartMember, SChooser(index_o_button3,"",button_array),
						StartMember, SChooser(index_o_axis1,"",axis_array),
						StartMember, SChooser(index_o_axis2,"",axis_array),
					EndGroup,
					CHILD_WeightedHeight, 0,

					StartVGroup,
						StartMember, SButton( index_b_button1, config.button[0].name ),
						StartMember, SButton( index_b_button2, config.button[1].name ),
						StartMember, SButton( index_b_button3, config.button[2].name ),
						StartMember, SButton( index_b_button4, config.button[3].name ),
						StartMember, SButton( index_b_button5, config.button[4].name ),
						StartMember, SButton( index_b_button6, config.button[5].name ),
						StartMember, SButton( index_b_button7, config.button[6].name ),
						StartMember, SButton( index_b_button8, config.button[7].name ),
						StartMember, SButton( index_b_button9, config.button[8].name ),
						StartMember, SButton( index_b_button10, config.button[9].name ),
						StartMember, SButton( index_b_button11, config.button[10].name ),
						StartMember, SButton( index_b_button12, config.button[11].name ),

					EndGroup,
					CHILD_WeightedHeight, 0,

				EndGroup,

			EndGroup,	
			CHILD_WeightedHeight, 0,

		EndGroup,
		CHILD_WeightedHeight, 0,
	EndWindow;
}
