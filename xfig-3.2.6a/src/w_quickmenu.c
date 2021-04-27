#include "fig.h"
#include "figx.h"
#include "resources.h"
#include "main.h"
#include "mode.h"
#include "object.h"
#include "d_text.h"
#include "f_read.h"
#include "f_util.h"
#include "u_create.h"
#include "u_fonts.h"
#include "u_pan.h"
#include "u_redraw.h"
#include "u_search.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_cmdpanel.h"
#include "w_digitize.h"
#include "w_drawprim.h"
#include "w_export.h"
#include "w_file.h"
#include "w_help.h"
#include "w_icons.h"
#include "w_indpanel.h"
#include "w_layers.h"
#include "w_msgpanel.h"
#include "w_mousefun.h"
#include "w_print.h"
#include "w_rulers.h"
#include "w_srchrepl.h"
#include "w_util.h"
#include "w_setup.h"
#include "w_style.h"
#include "w_zoom.h"
#include "w_snap.h"
#include "e_delete.h"
#include "f_load.h"
#include "u_bound.h"
#include "u_draw.h"
#include "u_free.h"
#include "u_list.h"
#include "u_translate.h"
#include "w_cursor.h"
#include "w_modepanel.h"

#ifndef XAW3D1_5E
#include "w_menuentry.h"
#endif
#ifdef I18N
#include "d_text.h"
#endif  /* I18N */

#include <X11/IntrinsicP.h> /* XtResizeWidget() */

/* internal features and definitions */

DeclareStaticArgs(12);

#define menu_item_bitmap_width 9
#define menu_item_bitmap_height 8
static unsigned char menu_item_bitmap_bits[] = {
   0x00, 0x01, 0x80, 0x01, 0xc0, 0x00, 0x60, 0x00,
   0x31, 0x00, 0x1b, 0x00, 0x0e, 0x00, 0x04, 0x00
};

static Pixmap menu_item_bitmap = None;

/* Widgets holding the ascii values for the string-based settings */

Widget	bal_delay;
Widget	n_freehand_resolution;
Widget	n_recent_files;
Widget	max_colors;
Widget	image_ed;
Widget	spell_chk;
Widget	browser;
Widget	pdfview;

/* global settings */

Widget	global_popup = (Widget) 0;
Widget	global_panel;

static Widget	create_quick_menu(int menu_num, Widget beside);

static int	off_paste_x,off_paste_y;
static int	orig_paste_x,orig_paste_y;

static Widget	character_map_popup = (Widget) 0;
static Widget	character_map_panel, close_but;

#ifdef XAW3D1_5E
#else
#ifdef XAW3D1_5E
#else
/* popup message over command button when mouse enters it */
static void     cmd_balloon_trigger(Widget widget, XtPointer closure, XEvent *event, Boolean *continue_to_dispatch);
static void     cmd_unballoon(Widget widget, XtPointer closure, XEvent *event, Boolean *continue_to_dispatch);

/* popup message over filename window when mouse enters it */
static void     filename_balloon_trigger(Widget widget, XtPointer closure, XEvent *event, Boolean *continue_to_dispatch);
static void     filename_unballoon(Widget widget, XtPointer closure, XEvent *event, Boolean *continue_to_dispatch);
#endif /* XAW3D1_5E */


quick_menu_info main_tiles[] = {
    {"Circle/Ellipse (Radius)", "ellipse", "Circle tile"},
    {"Rectangular Box", "recbox", "Rectangle Box tile"},
    {"Arc Drawing", "arc", "Arc tile"},
    {"Closed Interpolated Spline", "closedspline", "Closed Spline tile"},
    {"Open Interpolated Spline", "openspline", "Open Spline tile"},
    {"Polyline", "polyline", "Polyline tile"},
    {"Polygon", "polygon", "Polygon tile"},
    {"Regular Polygon", "regpoly", "Reg Polygon tile"},
    {"Polyline", "polyline", "Polyline tile"},
    {"Text", "text", "Text tile"},
};
#define		NUM_CMD_MENUS  (sizeof(main_tiles) / sizeof(quick_menu_info))

String  keyinput =
        "Ctrl<Key>Q: PlaceMenu()\n\
	<Key>Escape: CloseMenu()";

static XtActionsRec     menu_actions[] =
		    {
			{"xMenuPopup", (XtActionProc) popup_menu},
			{"PlaceMenu", (XtActionProc) place_menu},
			{"CloseMenu", (XtActionProc) close_menu},
		    };

void
init_quick_menu(Widget tool, char *filename)
{
    register int    i;
    Widget	    beside = NULL;
    DeclareArgs(11);

    FirstArg(XtNborderWidth, 0);
    NextArg(XtNcolormap, tool_cm);
    NextArg(XtNdefaultDistance, 0);
    NextArg(XtNhorizDistance, 0);
    NextArg(XtNvertDistance, 0);
    NextArg(XtNleft, XtChainLeft);
    NextArg(XtNright, XtChainLeft);
    NextArg(XtNtop, XtChainTop);
    NextArg(XtNbottom, XtChainTop);
    cmd_form = XtCreateWidget("commands", formWidgetClass, tool, Args, ArgCount);

    for (i = 0; i < NUM_CMD_MENUS; ++i) {
	beside = create_quick_menu(i, beside);
    }

    /* now setup the filename label widget to the right of the command menu buttons */

    FirstArg(XtNlabel, filename);
    NextArg(XtNfromHoriz, cmd_form);
    NextArg(XtNhorizDistance, -INTERNAL_BW);
    NextArg(XtNfont, bold_font);
    NextArg(XtNjustify, XtJustifyLeft);
    NextArg(XtNwidth, NAMEPANEL_WD);
    NextArg(XtNheight, CMD_BUT_HT+INTERNAL_BW);
    NextArg(XtNtop, XtChainTop);
    NextArg(XtNbottom, XtChainTop);
    NextArg(XtNborderWidth, INTERNAL_BW);
    name_panel = XtCreateManagedWidget("file_name", labelWidgetClass, tool,
				      Args, ArgCount);
#ifndef XAW3D1_5E
    /* popup balloon when mouse passes over filename */
    XtAddEventHandler(name_panel, EnterWindowMask, False,
		      filename_balloon_trigger, (XtPointer) name_panel);
    XtAddEventHandler(name_panel, LeaveWindowMask, False,
		      filename_unballoon, (XtPointer) name_panel);
#endif
    /* add actions to position the menus if the user uses an accelerator */
    refresh_view_menu();
}

// void
// add_cmd_actions(void)
// {
//     XtAppAddActions(tool_app, menu_actions, XtNumber(menu_actions));
// }

static Widget
create_quick_menu(int menu_num, Widget beside)
{
	register quick_menu_info *menu;

	menu = &main_tiles[menu_num];
	FirstArg(XtNborderWidth, INTERNAL_BW);
	NextArg(XtNfont, button_font);
	NextArg(XtNwidth, CMD_BUT_WD);
	NextArg(XtNheight, CMD_BUT_HT);
	NextArg(XtNvertDistance, 0);
	NextArg(XtNhorizDistance, -INTERNAL_BW);
	NextArg(XtNlabel, menu->label);
	NextArg(XtNfromHoriz, beside);
	NextArg(XtNmenuName, menu->menu_name);

	XtOverrideTranslations(quick_menu_info,
			XtParseTranslationTable(keyinput));

	/* make button to popup each menu */
	menu->widget = XtCreateManagedWidget(menu->label, menuButtonWidgetClass,
					   cmd_form, Args, ArgCount);
	XtAddEventHandler(menu->widget, EnterWindowMask, False,
			  enter_cmd_but, (XtPointer) menu);

	/* now the menu itself */
	menu->menuwidget = create_menu_item(menu);

#ifndef XAW3D1_5E
	/* popup when mouse passes over button */
	XtAddEventHandler(menu->widget, EnterWindowMask, False,
			  cmd_balloon_trigger, (XtPointer) menu);
	XtAddEventHandler(menu->widget, LeaveWindowMask, False,
			  cmd_unballoon, (XtPointer) menu);
#endif

	return menu->widget;
}

Widget
create_menu_item(quick_menu_info *menup)
{
	int	i;
	Widget	menu, entry;
	DeclareArgs(5);

	FirstArg(XtNallowShellResize, True);
	menu = XtCreatePopupShell(menup->menu_name, simpleMenuWidgetClass,
				menup->widget, Args, ArgCount);
	/* make the menu items */
	for (i = 0; menup->menu[i].name != NULL; i++) {
	    if ((intptr_t) menup->menu[i].name == -1) {
		/* put in a separator line */
		FirstArg(XtNlineWidth, 2);
		(void) XtCreateManagedWidget("line", smeLineObjectClass,
					menu, Args, ArgCount);
		/* and add recently loaded files to the menu */
		//rebuild_file_menu(menu);
	    } else if (strcmp(menup->menu[i].name, "-") == 0) {
		/* put in a separator line */
		(void) XtCreateManagedWidget("line", smeLineObjectClass, menu, NULL, 0);
	    } else {
		/* normal menu entry */
		FirstArg(XtNvertSpace, 10);
		/* leave space for the checkmark bitmap */
		if (menup->menu[i].checkmark) {
		    NextArg(XtNleftMargin, 12);
		}
#ifndef XAW3D1_5E
		NextArg(XtNunderline, menup->menu[i].u_line); /* any underline */
		entry = XtCreateManagedWidget(menup->menu[i].name, figSmeBSBObjectClass,
					menu, Args, ArgCount);
#else
		entry = XtCreateManagedWidget(menup->menu[i].name, smeBSBObjectClass,
					menu, Args, ArgCount);
#endif
		XtAddCallback(entry, XtNcallback, menup->menu[i].func,
					(XtPointer) menup->widget);
	    }
	}
	return menu;
}

//change this to open the menu when clicked
void
setup_quick_menu(void)
{
    register int    i;
    register quick_menu_info *menu;
    DeclareArgs(2);

    XDefineCursor(tool_d, XtWindow(cmd_form), arrow_cursor);

    for (i = 0; i < NUM_CMD_MENUS; ++i) {
	menu = &main_tiles[i];
	FirstArg(XtNfont, button_font); /* label font */
	if ( menu->menu )
	    NextArg(XtNleftBitmap, menu_arrow);     /* use menu arrow for pull-down */
	SetValues(menu->widget);
    }
}

/* come here when the mouse passes over a button in the command panel */

static	Widget cmd_balloon_popup = (Widget) 0;
static	XtIntervalId balloon_id = (XtIntervalId) 0;
static	Widget balloon_w;
static	XtPointer clos;

static void cmd_balloon(Widget w, XtPointer closure, XtPointer call_data);

static void
cmd_balloon_trigger(Widget widget, XtPointer closure, XEvent *event, Boolean *continue_to_dispatch)
{
	if (!appres.showballoons)
		return;
	balloon_w = widget;
	clos = closure;
	/* if an old balloon is still up, destroy it */
	if ((balloon_id != 0) || (cmd_balloon_popup != (Widget) 0)) {
		cmd_unballoon((Widget) 0, (XtPointer) 0, (XEvent*) 0, (Boolean*) 0);
	}
	balloon_id = XtAppAddTimeOut(tool_app, appres.balloon_delay,
			(XtTimerCallbackProc) cmd_balloon, (XtPointer) NULL);
}

static void
cmd_balloon(Widget w, XtPointer closure, XtPointer call_data)
{
	Position  x, y;
	Dimension wid, ht;
	quick_menu_info *menu= (quick_menu_info *) clos;
	Widget box, balloons_label;

	/* get width and height of this button */
	FirstArg(XtNwidth, &wid);
	NextArg(XtNheight, &ht);
	GetValues(balloon_w);
	/* find middle and lower edge */
	XtTranslateCoords(balloon_w, wid/2, ht+2, &x, &y);
	/* put popup there */
	FirstArg(XtNx, x);
	NextArg(XtNy, y);
	cmd_balloon_popup = XtCreatePopupShell("cmd_balloon_popup",overrideShellWidgetClass,
				tool, Args, ArgCount);
	FirstArg(XtNborderWidth, 0);
	NextArg(XtNhSpace, 0);
	NextArg(XtNvSpace, 0);
	NextArg(XtNorientation, XtorientVertical);
	box = XtCreateManagedWidget("box", boxWidgetClass, cmd_balloon_popup, Args, ArgCount);

	/* put left/right mouse button labels as message */
	FirstArg(XtNborderWidth, 0);
	NextArg(XtNlabel, menu->hint);
	balloons_label = XtCreateManagedWidget("label", labelWidgetClass,
				    box, Args, ArgCount);

	XtPopup(cmd_balloon_popup,XtGrabNone);
	XtRemoveTimeOut(balloon_id);
	balloon_id = (XtIntervalId) 0;
}

/* come here when the mouse leaves a button in the command panel */

static void
cmd_unballoon(Widget widget, XtPointer closure, XEvent *event, Boolean *continue_to_dispatch)
{
    if (balloon_id) {
	XtRemoveTimeOut(balloon_id);
    }
    balloon_id = (XtIntervalId) 0;
    if (cmd_balloon_popup != (Widget) 0) {
	XtDestroyWidget(cmd_balloon_popup);
	cmd_balloon_popup = (Widget) 0;
    }
}
#endif /* XAW3D1_5E */

static void
enter_cmd_but(Widget widget, XtPointer closure, XEvent *event, Boolean *continue_to_dispatch)
{
    quick_menu_info *menu = (quick_menu_info *) closure;
    draw_mousefun(menu->hint, "", "");
}

static void
popup_menu(Widget w, XEvent *event, String *params, Cardinal *nparams)
{
    int		which;

    which = locate_menu(params, nparams);
    if (which < 0)
	return;
    XtPopupSpringLoaded(main_tiles[which].menuwidget);
}

static void
place_menu(Widget w, XEvent *event, String *params, Cardinal *nparams)
{
    Position	x, y;
    Dimension	height;
    int		which;

    which = locate_menu(params, nparams);
    if (which < 0)
	return;
    /* get the height of the menu button on the command panel */
    FirstArg(XtNheight, &height);
    GetValues(main_tiles[which].widget);
    XtTranslateCoords(main_tiles[which].widget, (Position) 0, height+4, &x, &y);
    /* position the popup menu just under the button */
    FirstArg(XtNx, x);
    NextArg(XtNy, y);
    SetValues(main_tiles[which].menuwidget);
}

int
locate_menu(String *params, Cardinal *nparams)
{
    int		which;

    if (*nparams < 1)
	return -1;

    /* find which menu the user wants */
    for (which=0; which<NUM_CMD_MENUS; which++)
	if (strcmp(params[0],main_tiles[which].menu_name)==0)
	    break;
    if (which >= NUM_CMD_MENUS)
	return -1;
    return which;
}
