
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/locale.h>
#include <proto/diskfont.h>
#include <diskfont/diskfonttag.h>
#include <proto/graphics.h>
#include <proto/keymap.h>
#include <proto/Amigainput.h>
#include <proto/icon.h>
#include <proto/wb.h>
#include <proto/intuition.h>
#include <intuition/pointerclass.h>

struct Library 				*AmosExtensionBase = NULL;
struct AmosExtensionIFace	*IAmosExtension = NULL;

struct Library				*DataTypesBase = NULL;
struct DataTypesIFace		*IDataTypes = NULL;

extern struct Library		 	*DOSBase;
extern struct DOSIFace		*IDOS;

extern struct Library			*RetroModeBase;
extern struct RetroModeIFace	*IRetroMode;


struct Library 			*AHIBase = NULL;
struct AHIIFace			*IAHI = NULL;

struct Library 			*AslBase = NULL;
struct AslIFace			*IAsl = NULL;

struct LocaleIFace		*ILocale  = NULL;
struct Library			*LocaleBase = NULL;

struct Library			*DiskfontBase = NULL;
struct DiskfontIFace		*IDiskfont = NULL;

struct Library			*GadToolsBase = NULL;
struct DiskfontIFace		*IGadTools = NULL;

struct KeymapIFace		*IKeymap = NULL;
struct Library			*KeymapBase = NULL;

struct Locale			*_locale = NULL;
ULONG				*codeset_page = NULL;

struct Library 			* RetroModeBase = NULL;
struct RetroModeIFace 	*IRetroMode = NULL;

struct WorkbenchIFace	*IWorkbench = NULL;
struct Library			*WorkbenchBase = NULL;

struct IconIFace		*IIcon = NULL;
struct Library			*IconBase = NULL;

struct Library			*IntuitionBase = NULL;
struct IntuitionIFace		*IIntuition = NULL;

struct Library			*GraphicsBase = NULL;
struct GraphicsIFace		*IGraphics = NULL;

struct Library			*LayersBase = NULL;
struct LayersIFace		*ILayers = NULL;


BOOL open_lib( const char *name, int ver , const char *iname, int iver, struct Library **base, struct Interface **interface)
{
	*interface = NULL;
	*base = OpenLibrary( name , ver);
	if (*base)
	{
		 *interface = GetInterface( *base,  iname , iver, TAG_END );
		if (!*interface) printf("Unable to getInterface %s for %s %ld!\n",iname,name,ver);
	}
	else
	{
	   	printf("Unable to open the %s %ld!\n",name,ver);
	}
	return (*interface) ? TRUE : FALSE;
}


struct Library	*WindowBase = NULL;
struct WindowIFace *IWindow = NULL;

struct Library	*ListBrowserBase = NULL;
struct WindowIFace *IListBrowser = NULL;

struct Library	*LayoutBase = NULL;
struct WindowIFace *ILayout = NULL;

struct Library	*StringBase = NULL;
struct WindowIFace *IString = NULL;

struct Library	*ChooserBase = NULL;
struct WindowIFace *IChooser = NULL;

struct Library	*RequesterBase = NULL;
struct WindowIFace *IRequester = NULL;

struct Library		*AIN_Base = NULL;
struct AIN_IFace	*IAIN = NULL;

struct MsgPort *appport = NULL;
struct MsgPort	*io_msgport = NULL;
struct IOStdReq *io_req = NULL ;

bool startProg()
{
	if ( ! open_lib( "asl.library", 0L , "main", 1, &AslBase, (struct Interface **) &IAsl  ) ) return FALSE;
	if ( ! open_lib( "datatypes.library", 0L , "main", 1, &DataTypesBase, (struct Interface **) &IDataTypes  ) ) return FALSE;
	if ( ! open_lib( "locale.library", 53 , "main", 1, &LocaleBase, (struct Interface **) &ILocale  ) ) return FALSE;
	if ( ! open_lib( "keymap.library", 53, "main", 1, &KeymapBase, (struct Interface **) &IKeymap) ) return FALSE;
	if ( ! open_lib( "diskfont.library", 50L, "main", 1, &DiskfontBase, (struct Interface **) &IDiskfont  ) ) return FALSE;
	if ( ! open_lib( "retromode.library", 1L , "main", 1, &RetroModeBase, (struct Interface **) &IRetroMode  ) ) return FALSE;
	if ( ! open_lib( "AmigaInput.library", 50L , "main", 1, &AIN_Base, (struct Interface **) &IAIN  ) ) return FALSE;
	if ( ! open_lib( "gadtools.library", 53L , "main", 1, &GadToolsBase, (struct Interface **) &IGadTools  ) ) return FALSE;
	if ( ! open_lib( "intuition.library", 51L , "main", 1, &IntuitionBase, (struct Interface **) &IIntuition  ) ) return FALSE;
	if ( ! open_lib( "graphics.library", 54L , "main", 1, &GraphicsBase, (struct Interface **) &IGraphics  ) ) return FALSE;
	if ( ! open_lib( "layers.library", 54L , "main", 1, &LayersBase, (struct Interface **) &ILayers  ) ) return FALSE;
	if ( ! open_lib( "workbench.library", 53 , "main", 1, &WorkbenchBase, (struct Interface **) &IWorkbench ) ) return FALSE;
	if ( ! open_lib( "icon.library", 53, "main", 1, &IconBase, (struct Interface **) &IIcon) ) return FALSE;

// Classes

	if ( ! open_lib( "window.class", 54L , "main", 1, &WindowBase, (struct Interface **) &IWindow  ) ) return FALSE;
	if ( ! open_lib( "listbrowser.gadget", 53 , "main", 1, &ListBrowserBase, (struct Interface **) &IListBrowser ) ) return FALSE;
	if ( ! open_lib( "layout.gadget", 53, "main", 1, &LayoutBase, (struct Interface **) &ILayout) ) return FALSE;
	if ( ! open_lib( "string.gadget", 53, "main", 1, &StringBase, (struct Interface **) &IString ) ) return FALSE;
	if ( ! open_lib( "chooser.gadget", 53, "main", 1, &ChooserBase, (struct Interface **) &IChooser) ) return FALSE;
	if ( ! open_lib( "requester.class", 53, "main", 1, &RequesterBase, (struct Interface **) &IRequester ) ) return FALSE;

// resources

	appport = (MsgPort*) AllocSysObjectTags(ASOT_PORT, TAG_DONE);
	if ( ! appport ) return FALSE;

	io_msgport = (MsgPort*) AllocSysObjectTags(ASOT_PORT, TAG_DONE);
	if ( ! io_msgport ) return FALSE;

	io_req = (struct IOStdReq *) AllocSysObjectTags(ASOT_IOREQUEST,
							ASOIOR_ReplyPort, io_msgport, 
							ASOIOR_Size, sizeof(struct IOStdReq),
							TAG_END );

	if ( ! io_req ) return FALSE;

	return TRUE;
}

void endProg()
{

// resources

	if (io_msgport) FreeSysObject(ASOT_PORT, io_msgport);
	if (appport) FreeSysObject(ASOT_PORT, appport);
	if (io_req) FreeSysObject(ASOT_IOREQUEST,io_req);


// Classes

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IWindow) DropInterface((struct Interface*) IWindow);  IWindow= 0;
	if (WindowBase) CloseLibrary(WindowBase); WindowBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IListBrowser) DropInterface((struct Interface*) IListBrowser); IListBrowser = 0;
	if (ListBrowserBase) CloseLibrary(ListBrowserBase); ListBrowserBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (ILayout) DropInterface((struct Interface*) ILayout); ILayout = 0;
	if (LayoutBase) CloseLibrary(LayoutBase); LayoutBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IString) DropInterface((struct Interface*) IString); IString = 0;
	if (StringBase) CloseLibrary(StringBase); StringBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IChooser) DropInterface((struct Interface*) IChooser); IChooser = 0;
	if (ChooserBase) CloseLibrary(ChooserBase); ChooserBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IRequester) DropInterface((struct Interface*) IRequester); IRequester = 0;
	if (RequesterBase) CloseLibrary(RequesterBase); RequesterBase = 0;

// Libs

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IIcon) DropInterface((struct Interface*) IIcon); IIcon = 0;
	if (IconBase) CloseLibrary(IconBase); IconBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IWorkbench) DropInterface((struct Interface*) IWorkbench); IWorkbench = 0;
	if (WorkbenchBase) CloseLibrary(WorkbenchBase); WorkbenchBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IAIN) DropInterface((struct Interface*) IAIN); IAIN = 0;
	if (AIN_Base) CloseLibrary(AIN_Base); AIN_Base = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IAsl) DropInterface((struct Interface*) IAsl); IAsl = 0;
	if (AslBase) CloseLibrary(AslBase); AslBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IDataTypes) DropInterface((struct Interface*) IDataTypes); IDataTypes = 0;
	if (DataTypesBase) CloseLibrary(DataTypesBase); DataTypesBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (ILocale) DropInterface((struct Interface*) ILocale); ILocale = 0;
	if (LocaleBase) CloseLibrary(LocaleBase); LocaleBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IKeymap) DropInterface((struct Interface*) IKeymap); IKeymap = 0;
	if (KeymapBase) CloseLibrary(KeymapBase); KeymapBase = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (DiskfontBase) CloseLibrary(DiskfontBase); DiskfontBase = 0;
	if (IDiskfont) DropInterface((struct Interface*) IDiskfont); IDiskfont = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (RetroModeBase) CloseLibrary(RetroModeBase); RetroModeBase = 0;
	if (IRetroMode) DropInterface((struct Interface*) IRetroMode); IRetroMode = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (IntuitionBase) CloseLibrary(IntuitionBase); IntuitionBase = 0;
	if (IIntuition) DropInterface((struct Interface*) IIntuition); IIntuition = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (LayersBase) CloseLibrary(LayersBase); LayersBase = 0;
	if (ILayers) DropInterface((struct Interface*) ILayers); ILayers = 0;

	printf("%s:%d\n",__FUNCTION__,__LINE__);

	if (GraphicsBase) CloseLibrary(GraphicsBase); GraphicsBase = 0;
	if (IGraphics) DropInterface((struct Interface*) IGraphics); IGraphics = 0;
}
