

struct ColumnInfo files_ci[] =
{
	{ 100, "name", 0 },
	{ -1, (STRPTR)~0, -1 }
};

void init_mainmenu(Object **win)
{
	*win = WindowObject,
				
		/* these tags describe the window  */
		
		WA_IDCMP, IDCMP_RAWKEY | IDCMP_GADGETUP  | IDCMP_GADGETDOWN,
		WA_Top, 20,
		WA_Left, 20,
		WA_Width, 200,
		WA_Height, 200,
		WA_DepthGadget, TRUE,
		WA_DragBar, TRUE,
		WA_CloseGadget, TRUE,
		WA_Activate, TRUE,
		WA_Title, "AmigaInputAnywhere",
		WA_ScreenTitle, "AmigaInputAnywhere",
				
						
		/* Below is the layout of the window  */
				
		WINDOW_ParentGroup,	VGroupObject,
			LAYOUT_SpaceOuter, TRUE,
			LAYOUT_BevelStyle, BVS_THIN,
						
			StartVGroup, BAligned,
				StartMember,   Objects[ga_files_list] =ListBrowserObject,
					GA_ID, ga_files_list,	
					GA_RelVerify, TRUE,
					LISTBROWSER_Labels, &list_files,
					LISTBROWSER_ColumnInfo, files_ci,
					LISTBROWSER_ColumnTitles, TRUE,
					LISTBROWSER_MultiSelect, FALSE,
					LISTBROWSER_Separators, TRUE,
					LISTBROWSER_ShowSelected, TRUE,
					LISTBROWSER_Editable, FALSE,
				EndMember,

				StartHGroup,
/*					StartMember, SButton(ga_new_config,"New"), */
					StartMember, SButton(ga_edit_config,"Edit"),
/*					StartMember, SButton(ga_delete_config,"Delete"), */
				EndGroup,
				CHILD_WeightedHeight, 0,

				StartMember, SButton(ga_quit,"Quit"),
				CHILD_WeightedHeight, 0,
			EndGroup,					
		EndGroup,
	EndWindow;
}
