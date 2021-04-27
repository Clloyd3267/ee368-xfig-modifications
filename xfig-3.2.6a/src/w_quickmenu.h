/* width of a command button */
#define CMD_BUT_WD 60
#define CMD_BUT_HT 22


/* def for menu */

typedef struct {
    char  *name;		/* name e.g. 'Save' */
    int	   u_line;		/* which character to underline (-1 means none) */
    void  (*func)();		/* function that is called for menu choice */
    Boolean checkmark;		/* whether a checkmark is put in the left bitmap space */
} tile_def ;

/* cmd panel menu definitions */

#define CMD_LABEL_LEN	16

typedef struct quick_menu_struct {
    char	    label[CMD_LABEL_LEN];	/* label on the button */
    char	    menu_name[CMD_LABEL_LEN];	/* command name for resources */
    char	    hint[CMD_LABEL_LEN];	/* label for mouse func and balloon */
    menu_def	   *menu;			/* menu */
    Widget	    widget;			/* button widget */
    Widget	    menuwidget;			/* menu widget */
}	quick_menu_info;

extern quick_menu_info tiles[];

extern void init_quick_menu();
extern Widget create_quick_menu(quick_menu_info *menup);
extern Widget create_menu_item(quick_menu_info *menup);
extern void     init_quick_menu(Widget tool, char *filename);
void setup_quick_menu(void);

