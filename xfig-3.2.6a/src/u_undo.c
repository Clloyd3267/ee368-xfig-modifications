/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985-1988 by Supoj Sutanthavibul
 * Parts Copyright (c) 1989-2007 by Brian V. Smith
 * Parts Copyright (c) 1991 by Paul King
 * Parts Copyright (c) 1995 by C. Blanc and C. Schlick
 *
 * Any party obtaining a copy of these files is granted, free of charge, a
 * full and unrestricted irrevocable, world-wide, paid up, royalty-free,
 * nonexclusive right and license to deal in this software and documentation
 * files (the "Software"), including without limitation the rights to use,
 * copy, modify, merge, publish distribute, sublicense and/or sell copies of
 * the Software, and to permit persons who receive copies from any such
 * party to do so, with the only requirement being that the above copyright
 * and this permission notice remain intact.
 *
 */

/**************** IMPORTS ****************/

#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "e_addpt.h"
#include "e_arrow.h"
#include "e_convert.h"
#include "u_draw.h"
#include "u_elastic.h"
#include "u_list.h"
#include "u_redraw.h"
#include "u_undo.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_file.h"
#include "w_layers.h"
#include "w_msgpanel.h"
#include "w_setup.h"

#include "e_deletept.h"
#include "e_scale.h"
#include "f_read.h"
#include "u_bound.h"
#include "u_free.h"
#include "u_markers.h"
#include "u_translate.h"
#include "w_cmdpanel.h"
#include "w_indpanel.h"
#include "w_color.h"

extern void	swap_depths(void);	/* w_layers.c */
extern void	swap_counts(void);	/* w_layers.c */

/*************** EXPORTS *****************/

/*
 * Object_tails *usually* points to the last object in each linked list in
 * objects.  The exceptions occur when multiple objects are added to a figure
 * (e.g. file read, break compound, undo delete region).  In these cases,
 * the added objects are appended to the object lists (and saved_objects is
 * set up to point to the new objects) but object_tails is not changed.
 * This speeds up a subsequent undo operation which need only set
 * all the "next" fields of objects pointed to by object_tails to NULL.
 */

F_compound	saved_objects = {0, 0, { 0, 0 }, { 0, 0 },
				NULL, NULL, NULL, NULL, NULL, NULL, NULL};
F_compound	object_tails = {0, 0, { 0, 0 }, { 0, 0 },
				NULL, NULL, NULL, NULL, NULL, NULL, NULL};
F_arrow		*saved_for_arrow = (F_arrow *) NULL;
F_arrow		*saved_back_arrow = (F_arrow *) NULL;
F_line		*latest_line;		/* for undo_join (line) */
F_spline	*latest_spline;		/* for undo_join (spline) */

//----------------------- Modifications by Kyle Bielby -----------------------//
// int		last_action = F_NULL

// top of stack is index 0 and bottom of stack is index 4
int last_action[] = {F_NULL, F_NULL, F_NULL, F_NULL, F_NULL};

// redo action stack
int redo_action_stack[] = {F_NULL, F_NULL, F_NULL, F_NULL, F_NULL};

// F_compound object to store the last five objects
F_compound active_object = {0, 0, { 0, 0 }, { 0, 0 },
				NULL, NULL, NULL, NULL, NULL, NULL, NULL};

static int last_five_objects[5];  // NOTE not sure if this is needed

F_spline last_spline_object;

F_spline * nextSavedSpline;
F_ellipse * nextSavedEllipse;
F_line * nextSavedLine;
F_text * nextSavedText;
F_arc * nextSavedArc;
F_compound * nextSavedCompound;

//----------------------- End Modifications by Kyle Bielby -------------------//


/*************** LOCAL *****************/

static int	last_object;
static F_pos	last_position, new_position;  // KAB may need to change this to an F_pos * for multiple positions
static int	last_arcpointnum;
static F_point *last_prev_point, *last_selected_point, *last_next_point;
static F_sfactor  *last_selected_sfactor;
static F_linkinfo *last_links;
static F_arrow    *last_for_arrow, *last_back_arrow;
static int	last_linkmode;
static double	last_origin_tension, last_extremity_tension;


void undo_add (void);
void undo_delete (void);
void undo_move (void);
void undo_change (void);
void undo_glue (void);
void undo_break (void);
void undo_load (void);
void undo_scale (void);
void undo_addpoint (void);
void undo_deletepoint (void);
void undo_add_arrowhead (void);
void undo_delete_arrowhead (void);
void undo_convert (void);
void undo_open_close (void);
void undo_join_split (void);
void set_action_object (int action, int object);
void swap_newp_lastp (void);

void
undo(void)
{
    /* turn off Compose key LED */
    setCompLED(0);

    // switch (last_action) {
    switch (last_action[0]) {  // check action contained at the top of the stack
      case F_ADD:
				undo_add();
				break;
      case F_DELETE:
				undo_delete();
				break;
      case F_MOVE:
				undo_move();
				break;
      case F_EDIT:
				undo_change();
				break;
      case F_GLUE:
				undo_glue();
				break;
      case F_BREAK:
				undo_break();
				break;
      case F_LOAD:
				undo_load();
				break;
      case F_SCALE:
				undo_scale();
				break;
      case F_ADD_POINT:
				undo_addpoint();
				break;
      case F_DELETE_POINT:
	undo_deletepoint();
	break;
      case F_ADD_ARROW_HEAD:
	undo_add_arrowhead();
	break;
      case F_DELETE_ARROW_HEAD:
	undo_delete_arrowhead();
	break;
      case F_CONVERT:
	undo_convert();
	break;
      case F_OPEN_CLOSE:
	undo_open_close();
	break;
      case F_JOIN:
      case F_SPLIT:
	undo_join_split();
	break;
    default:
	put_msg("Nothing to UNDO");
	return;
    }
}

void undo_join_split(void)
{
    F_line	    swp_l;
    F_spline	    swp_s;
    if (last_five_objects[0] == O_POLYLINE) {
			new_l = saved_objects.lines;		/* the original */
			old_l = latest_line;			/* the changed object */
			/* swap old with new */
			bcopy((char*)old_l, (char*)&swp_l, sizeof(F_line));
			bcopy((char*)new_l, (char*)old_l, sizeof(F_line));
			bcopy((char*)&swp_l, (char*)new_l, sizeof(F_line));
			/* this assumes that the object are at the end of the objects list */
			/* correct the depth counts if necessary */
			if (!new_l->next && old_l->next){ /* join undo */
        add_depth(O_POLYLINE, old_l->next->depth);
      }
			else if (new_l->next && !old_l->next){ /* split undo */
        remove_depth(O_POLYLINE, new_l->next->depth);
      }
			set_action_object(F_JOIN, O_POLYLINE);
			redisplay_lines(new_l, old_l);
    	} else {
				new_s = saved_objects.splines;		/* the original */
				old_s = latest_spline;			/* the changed object */
				/* swap old with new */
				bcopy((char*)old_s, (char*)&swp_s, sizeof(F_spline));
				bcopy((char*)new_s, (char*)old_s, sizeof(F_spline));
				bcopy((char*)&swp_s, (char*)new_s, sizeof(F_spline));
				/* this assumes that the object are at the end of the objects list */
				/* correct the depth counts if necessary */
				if (!new_s->next && old_s->next){ /* join undo */
          add_depth(O_SPLINE, old_s->next->depth);
        }
				else if (new_s->next && !old_s->next){ /* split undo */
          remove_depth(O_SPLINE, new_s->next->depth);
        }
				set_action_object(F_JOIN, O_SPLINE);
				redisplay_splines(new_s, old_s);
    }
}

void undo_addpoint(void)
{
    if (last_five_objects[0] == O_POLYLINE)
	linepoint_deleting(saved_objects.lines, last_prev_point,
			   last_selected_point);
    else
	splinepoint_deleting(saved_objects.splines, last_prev_point,
			     last_selected_point);
}

void undo_deletepoint(void)
{
    // last_action = F_NULL;	/* to avoid doing a clean-up during adding */ // KAB not sure if I need to modify this
		// pop_undo_stack_action(); // KAB added to pop last action

    if (last_five_objects[0] == O_POLYLINE) {
	linepoint_adding(saved_objects.lines, last_prev_point,
			 last_selected_point);
	/* turn back on all relevant markers */
	update_markers(new_objmask);

    } else {	/* last_five_objects[0] is a spline */
	splinepoint_adding(saved_objects.splines, last_prev_point,
			 last_selected_point, last_next_point,
			 last_selected_sfactor->s);
    }

    last_next_point = NULL;
}

void undo_break(void)
{
    cut_objects(&objects, &object_tails);
    /* remove the depths from this compound because they'll be added in right after */
    remove_compound_depth(saved_objects.compounds);
    list_add_compound(&objects.compounds, saved_objects.compounds);
    // last_action = F_GLUE;  // KAB
		// pop_undo_stack_action();
    toggle_markers_in_compound(saved_objects.compounds);
    mask_toggle_compoundmarker(saved_objects.compounds);
}

void undo_glue(void)
{
    list_delete_compound(&objects.compounds, saved_objects.compounds);
    tail(&objects, &object_tails);
    append_objects(&objects, saved_objects.compounds, &object_tails); // KAB
    /* add the depths from this compound because they weren't added by the append_objects() */
    add_compound_depth(saved_objects.compounds);
    // last_action = F_BREAK; // KAB
		// pop_undo_stack_action();
    mask_toggle_compoundmarker(saved_objects.compounds);
    toggle_markers_in_compound(saved_objects.compounds);
    if (cur_mode != F_GLUE && cur_mode != F_BREAK)
	set_tags(saved_objects.compounds, 0);
}

void undo_convert(void)
{
    switch (last_five_objects[0]) {
      case O_POLYLINE:
	if (saved_objects.lines->type == T_BOX ||
	    saved_objects.lines->type == T_ARCBOX)
		box_2_box(latest_line);
	else
		spline_line(saved_objects.splines);
	break;
      case O_SPLINE:
	line_spline(saved_objects.lines, saved_objects.splines->type);
	break;
    }
}

void undo_add_arrowhead(void)
{
    switch (last_five_objects[0]) {
      case O_POLYLINE:
				delete_linearrow(saved_objects.lines, last_prev_point, last_selected_point);
				break;
      case O_SPLINE:

				delete_splinearrow(saved_objects.splines,
			  last_prev_point, last_selected_point);
				break;
      case O_ARC:
				delete_arcarrow(saved_objects.arcs, last_arcpointnum);
				break;
      default:
				return;
    }
		// pop_undo_stack_action();
}

void undo_delete_arrowhead(void)
{
    switch (last_five_objects[0]) {
      case O_POLYLINE:
				if (saved_for_arrow)
	    		saved_objects.lines->for_arrow = saved_for_arrow;
				if (saved_back_arrow)
	    		saved_objects.lines->back_arrow = saved_back_arrow;
				redisplay_line(saved_objects.lines);
				break;
      case O_SPLINE:
				if (saved_for_arrow)
	    		saved_objects.splines->for_arrow = saved_for_arrow;
				if (saved_back_arrow)
	    		saved_objects.splines->back_arrow = saved_back_arrow;
				redisplay_spline(saved_objects.splines);
				break;
      case O_ARC:
				if (saved_for_arrow)
	    		saved_objects.arcs->for_arrow = saved_for_arrow;
				if (saved_back_arrow)
	    		saved_objects.arcs->back_arrow = saved_back_arrow;
				redisplay_arc(saved_objects.arcs);
				break;
      default:
				return;
    }
}

/*
 * saved_objects.xxxx contains a pointer to the original object,
 * saved_objects.xxxx->next points to the changed object.
 */

void undo_change(void)
{
    char	   *swp_comm;
    F_compound	    swp_c;
    F_line	    swp_l;
    F_spline	    swp_s;
    F_ellipse	    swp_e;
    F_arc	    swp_a;
    F_text	    swp_t;

    // last_action = F_NULL;	/* to avoid a clean-up during "unchange" */ // KAB
		// pop_undo_stack_action();
    switch (last_five_objects[0]) {
      case O_POLYLINE:
				new_l = saved_objects.lines;		/* the original */
				old_l = saved_objects.lines->next;	/* the changed object */
				/* account for depths */
				remove_depth(O_POLYLINE, old_l->depth);
				add_depth(O_POLYLINE, new_l->depth);
				/* swap old with new */
				bcopy((char*)old_l, (char*)&swp_l, sizeof(F_line));
				bcopy((char*)new_l, (char*)old_l, sizeof(F_line));
				bcopy((char*)&swp_l, (char*)new_l, sizeof(F_line));
				/* but keep the next pointers unchanged */
				swp_l.next = old_l->next;
				old_l->next = new_l->next;
				new_l->next = swp_l.next;
				set_action_object(F_EDIT, O_POLYLINE);
				redisplay_lines(new_l, old_l);
				break;
      case O_ELLIPSE:
				new_e = saved_objects.ellipses;
				old_e = saved_objects.ellipses->next;
				/* account for depths */
				remove_depth(O_ELLIPSE, old_e->depth);
				add_depth(O_ELLIPSE, new_e->depth);
				/* swap old with new */
				bcopy((char*)old_e, (char*)&swp_e, sizeof(F_ellipse));
				bcopy((char*)new_e, (char*)old_e, sizeof(F_ellipse));
				bcopy((char*)&swp_e, (char*)new_e, sizeof(F_ellipse));
				/* but keep the next pointers unchanged */
				swp_e.next = old_e->next;
				old_e->next = new_e->next;
				new_e->next = swp_e.next;
				set_action_object(F_EDIT, O_ELLIPSE);
				redisplay_ellipses(new_e, old_e);
				break;
      case O_TXT:
				new_t = saved_objects.texts;
				old_t = saved_objects.texts->next;
				/* account for depths */
				remove_depth(O_TXT, old_t->depth);
				add_depth(O_TXT, new_t->depth);
				/* swap old with new */
				bcopy((char*)old_t, (char*)&swp_t, sizeof(F_text));
				bcopy((char*)new_t, (char*)old_t, sizeof(F_text));
				bcopy((char*)&swp_t, (char*)new_t, sizeof(F_text));
				/* but keep the next pointers unchanged */
				swp_t.next = old_t->next;
				old_t->next = new_t->next;
				new_t->next = swp_t.next;
				set_action_object(F_EDIT, O_TXT);
				redisplay_texts(new_t, old_t);
				break;
      case O_SPLINE:
				new_s = saved_objects.splines;
				old_s = saved_objects.splines->next;
				/* account for depths */
				remove_depth(O_SPLINE, old_s->depth);
				add_depth(O_SPLINE, new_s->depth);
				/* swap old with new */
				bcopy((char*)old_s, (char*)&swp_s, sizeof(F_spline));
				bcopy((char*)new_s, (char*)old_s, sizeof(F_spline));
				bcopy((char*)&swp_s, (char*)new_s, sizeof(F_spline));
				/* but keep the next pointers unchanged */
				swp_s.next = old_s->next;
				old_s->next = new_s->next;
				new_s->next = swp_s.next;
				set_action_object(F_EDIT, O_SPLINE);
				redisplay_splines(new_s, old_s);
				break;
      case O_ARC:
				new_a = saved_objects.arcs;
				old_a = saved_objects.arcs->next;
				/* account for depths */
				remove_depth(O_ARC, old_a->depth);
				add_depth(O_ARC, new_a->depth);
				/* swap old with new */
				bcopy((char*)old_a, (char*)&swp_a, sizeof(F_arc));
				bcopy((char*)new_a, (char*)old_a, sizeof(F_arc));
				bcopy((char*)&swp_a, (char*)new_a, sizeof(F_arc));
				/* but keep the next pointers unchanged */
				swp_a.next = old_a->next;
				old_a->next = new_a->next;
				new_a->next = swp_a.next;
				set_action_object(F_EDIT, O_ARC);
				redisplay_arcs(new_a, old_a);
				break;
      case O_COMPOUND:
	new_c = saved_objects.compounds;
	old_c = saved_objects.compounds->next;
	/* account for depths */
	remove_compound_depth(old_c);
	add_compound_depth(new_c);
	/* swap old with new */
	bcopy((char*)old_c, (char*)&swp_c, sizeof(F_compound));
	bcopy((char*)new_c, (char*)old_c, sizeof(F_compound));
	bcopy((char*)&swp_c, (char*)new_c, sizeof(F_compound));
	/* but keep the next pointers unchanged */
	swp_c.next = old_c->next;
	old_c->next = new_c->next;
	new_c->next = swp_c.next;
	set_action_object(F_EDIT, O_COMPOUND);
	redisplay_compounds(new_c, old_c);
	break;
      case O_FIGURE:
	/* swap saved figure comments with current */
	swp_comm = objects.comments;
	objects.comments = saved_objects.comments;
	saved_objects.comments = swp_comm;
	set_action_object(F_EDIT, O_FIGURE);
	break;
      case O_ALL_OBJECT:
	swp_c = objects;
	objects = saved_objects;
	saved_objects = swp_c;
	new_c = &objects;
	old_c = &saved_objects;
	/* account for depths */
	remove_compound_depth(old_c);
	add_compound_depth(new_c);
	set_action_object(F_EDIT, O_ALL_OBJECT);
	set_modifiedflag();
	redisplay_zoomed_region(0, 0, BACKX(CANVAS_WD), BACKY(CANVAS_HT));
	break;
    }
}

/*
 * When a single object is created, it is appended to the appropriate list
 * in objects.	It is also placed in the appropriate list in saved_objects.
 *
 * When a number of objects are created (usually by reading them in from
 * a file or undoing a remove-all action), they are appended to the lists in
 * objects and also saved in saved_objects.  The pointers in object_tails
 * will be set to point to the last members of the lists in objects prior to
 * the appending.
 *
 * Note: The read operation will set the pointers in object_tails while the
 * remove-all operation will zero pointers in objects.
 */

void undo_add(void)
{
    int		    xmin, ymin, xmax, ymax;
    char	    ctemp[PATH_MAX];

    switch (last_five_objects[0]) {
      case O_POLYLINE:
				display_lines();
				list_delete_line(&objects.lines, saved_objects.lines);
				redisplay_line(saved_objects.lines);

				saved_objects.lines = last_line(objects.lines);
				// list_delete_line(&objects.lines, active_object.lines);
				// redisplay_line(active_object.lines);
				// active_object.lines = last_line(objects.lines);
				display_lines();
				break;
      case O_ELLIPSE:
				list_delete_ellipse(&objects.ellipses, saved_objects.ellipses);
				redisplay_ellipse(saved_objects.ellipses);

				saved_objects.ellipses = last_ellipse(objects.ellipses);
				// list_delete_ellipse(&objects.ellipses, active_object.ellipses);
				// redisplay_ellipse(active_object.ellipses);
				// active_object.ellipses = last_ellipse(objects.ellipses);
				break;
      case O_TXT:
				list_delete_text(&objects.texts, saved_objects.texts);
				redisplay_text(saved_objects.texts);

				saved_objects.texts = last_text(objects.texts);
				// list_delete_text(&objects.texts, active_object.texts);
				// redisplay_text(active_object.texts);
				// active_object.texts = last_text(objects.texts);
				break;
      case O_SPLINE:
				display_object_splines(); // KAB remove after testing
				list_delete_spline(&objects.splines, saved_objects.splines);
				redisplay_spline(saved_objects.splines);

				saved_objects.splines = last_spline(objects.splines);
				// list_delete_spline(&objects.splines, active_object.splines);
				// redisplay_spline(active_object.splines);
				// active_object.splines = last_spline(objects.splines);
				// list_delete_spline(&objects.splines, last_spline(&objects.splines)));
				display_object_splines(); // KAB remove after testing
				break;
      case O_ARC:
				list_delete_arc(&objects.arcs, saved_objects.arcs);
				redisplay_arc(saved_objects.arcs);

				saved_objects.arcs = last_arc(objects.arcs);
				// list_delete_arc(&objects.arcs, active_object.arcs);
				// redisplay_arc(active_object.arcs);
				// active_object.arcs = last_arc(objects.arcs);
				break;
      case O_COMPOUND:
				list_delete_compound(&objects.compounds, saved_objects.compounds);
				redisplay_compound(saved_objects.compounds);

				saved_objects.compounds = last_compound(objects.compounds);
				// list_delete_compound(&objects.compounds, active_object.compounds);
				// redisplay_compound(active_object.compounds);
				// active_object.compounds = last_compound(objects.compounds);
				break;
      case O_ALL_OBJECT:
				// TODO not sure how to handle this object
				cut_objects(&objects, &object_tails);
				compound_bound(&saved_objects, &xmin, &ymin, &xmax, &ymax);
				redisplay_zoomed_region(xmin, ymin, xmax, ymax);
				break;
    }

    // last_action = F_DELETE;
		pop_undo_stack_action();
		pop_last_object();
}

void undo_delete(void)
{
    char	   *swp_comm;
    int		    xmin, ymin, xmax, ymax;
    char	    ctemp[PATH_MAX];

    switch (last_five_objects[0]) {
      case O_POLYLINE:
				nextSavedLine = saved_objects.lines->next;
				list_add_line(&objects.lines, saved_objects.lines);
				redisplay_line(saved_objects.lines);
				saved_objects.lines = nextSavedLine;
				break;
      case O_ELLIPSE:
				display_object_ellipse();
				nextSavedEllipse = saved_objects.ellipses->next;
				list_add_ellipse(&objects.ellipses, saved_objects.ellipses);
				redisplay_ellipse(saved_objects.ellipses);
				saved_objects.ellipses = nextSavedEllipse;
				display_object_ellipse();
				break;
      case O_TXT:
				nextSavedText = saved_objects.texts->next;
				list_add_text(&objects.texts, saved_objects.texts);
				redisplay_text(saved_objects.texts);
				saved_objects.texts = nextSavedText;
				break;
      case O_SPLINE:
				nextSavedSpline = saved_objects.splines->next;
				list_add_spline(&objects.splines, saved_objects.splines);
				redisplay_spline(saved_objects.splines);
				saved_objects.splines = nextSavedSpline;
				break;
      case O_ARC:
				nextSavedArc = saved_objects.arcs->next;
				list_add_arc(&objects.arcs, saved_objects.arcs);
				redisplay_arc(saved_objects.arcs);
				saved_objects.arcs = nextSavedArc;
				break;
      case O_COMPOUND:
				nextSavedCompound = saved_objects.compounds->next;
				list_add_compound(&objects.compounds, saved_objects.compounds);
				redisplay_compound(saved_objects.compounds);
				saved_objects.compounds = nextSavedCompound;
				break;
      case O_FIGURE:
        /* swap saved figure comments with current */
        swp_comm = objects.comments;
        objects.comments = saved_objects.comments;
        saved_objects.comments = swp_comm;
        /* swap colors*/
        swap_colors();
        /* restore objects*/
        saved_objects.next = NULL;
        compound_bound(&saved_objects, &xmin, &ymin, &xmax, &ymax);
        tail(&objects, &object_tails);
        append_objects(&objects, &saved_objects, &object_tails);
        redisplay_zoomed_region(xmin, ymin, xmax, ymax);
        break;
      case O_ALL_OBJECT:
				saved_objects.next = NULL;
				compound_bound(&saved_objects, &xmin, &ymin, &xmax, &ymax);
				tail(&objects, &object_tails);
				append_objects(&objects, &saved_objects, &object_tails);
				redisplay_zoomed_region(xmin, ymin, xmax, ymax);
    }
    // last_action = F_ADD; //KAB
		// pop_undo_stack_action();
}

void undo_move(void)
{
    int		    dx, dy;
    int		    xmin1, ymin1, xmax1, ymax1;
    int		    xmin2, ymin2, xmax2, ymax2;
    int		    dum;

    dx = last_position.x - new_position.x;
    dy = last_position.y - new_position.y;
    switch (last_five_objects[0]) {
      case O_POLYLINE:
	line_bound(saved_objects.lines, &xmin1, &ymin1, &xmax1, &ymax1);
	translate_line(saved_objects.lines, dx, dy);
	line_bound(saved_objects.lines, &xmin2, &ymin2, &xmax2, &ymax2);
	adjust_links(last_linkmode, last_links, dx, dy, 0, 0, 1.0, 1.0, False);
	redisplay_regions(xmin1, ymin1, xmax1, ymax1,
			  xmin2, ymin2, xmax2, ymax2);
	break;
      case O_ELLIPSE:
	ellipse_bound(saved_objects.ellipses, &xmin1, &ymin1, &xmax1, &ymax1);
	translate_ellipse(saved_objects.ellipses, dx, dy);
	ellipse_bound(saved_objects.ellipses, &xmin2, &ymin2, &xmax2, &ymax2);
	redisplay_regions(xmin1, ymin1, xmax1, ymax1,
			  xmin2, ymin2, xmax2, ymax2);
	break;
      case O_TXT:
	text_bound(saved_objects.texts, &xmin1, &ymin1, &xmax1, &ymax1,
		&dum,&dum,&dum,&dum,&dum,&dum,&dum,&dum);
	translate_text(saved_objects.texts, dx, dy);
	text_bound(saved_objects.texts, &xmin2, &ymin2, &xmax2, &ymax2,
		&dum,&dum,&dum,&dum,&dum,&dum,&dum,&dum);
	redisplay_regions(xmin1, ymin1, xmax1, ymax1,
			  xmin2, ymin2, xmax2, ymax2);
	break;
      case O_SPLINE:
	spline_bound(saved_objects.splines, &xmin1, &ymin1, &xmax1, &ymax1);
	translate_spline(saved_objects.splines, dx, dy);
	spline_bound(saved_objects.splines, &xmin2, &ymin2, &xmax2, &ymax2);
	redisplay_regions(xmin1, ymin1, xmax1, ymax1,
			  xmin2, ymin2, xmax2, ymax2);
	break;
      case O_ARC:
	arc_bound(saved_objects.arcs, &xmin1, &ymin1, &xmax1, &ymax1);
	translate_arc(saved_objects.arcs, dx, dy);
	arc_bound(saved_objects.arcs, &xmin2, &ymin2, &xmax2, &ymax2);
	redisplay_regions(xmin1, ymin1, xmax1, ymax1,
			  xmin2, ymin2, xmax2, ymax2);
	break;
      case O_COMPOUND:
	compound_bound(saved_objects.compounds, &xmin1, &ymin1, &xmax1, &ymax1);
	translate_compound(saved_objects.compounds, dx, dy);
	compound_bound(saved_objects.compounds, &xmin2, &ymin2, &xmax2, &ymax2);
	adjust_links(last_linkmode, last_links, dx, dy, 0, 0, 1.0, 1.0, False);
	redisplay_regions(xmin1, ymin1, xmax1, ymax1,
			  xmin2, ymin2, xmax2, ymax2);
	break;
    }
    swap_newp_lastp();
}

void undo_load(void)
{
    F_compound	    temp;
    char	    ctemp[PATH_MAX];

    /* swap objects in current figure/figure we're restoring */
    temp = objects;
    objects = saved_objects;
    saved_objects = temp;
    /* swap filenames */
    strcpy(ctemp, cur_filename);
    update_cur_filename(save_filename);
    strcpy(save_filename, ctemp);
    /* restore colors for the figure we are restoring */
    swap_depths();
    swap_counts();
    swap_colors();
    colors_are_swapped = False;
    /* in case current figure doesn't have the colors shown in the fill/pen colors */
    current_memory = -1;
    show_pencolor();
    show_fillcolor();
    /* redisply that figure */
    redisplay_canvas();
    // last_action = F_LOAD; //KAB
		// pop_undo_stack_action();
}

void undo_scale(void)
{
    float	    scalex, scaley;
    int		    xmin1, ymin1, xmax1, ymax1;
    int		    xmin2, ymin2, xmax2, ymax2;

    compound_bound(saved_objects.compounds, &xmin1, &ymin1, &xmax1, &ymax1);
    scalex = ((float) (last_position.x - fix_x)) / (new_position.x - fix_x);
    scaley = ((float) (last_position.y - fix_y)) / (new_position.y - fix_y);
    scale_compound(saved_objects.compounds, scalex, scaley, fix_x, fix_y);
    compound_bound(saved_objects.compounds, &xmin2, &ymin2, &xmax2, &ymax2);
    redisplay_regions(xmin1, ymin1, xmax1, ymax1,
			  xmin2, ymin2, xmax2, ymax2);
    swap_newp_lastp();
}

void undo_open_close(void)
{
  switch (last_five_objects[0]) {
  case O_POLYLINE:
    if (saved_objects.lines->type == T_POLYGON) {
	saved_objects.lines->for_arrow = last_for_arrow;
	saved_objects.lines->back_arrow = last_back_arrow;
	last_for_arrow = last_back_arrow = NULL;
    }
    toggle_polyline_polygon(saved_objects.lines, last_prev_point,
			    last_selected_point);
    break;
  case O_SPLINE:
    if (saved_objects.splines->type == T_OPEN_XSPLINE) {
	F_sfactor *c_tmp;

	draw_spline(saved_objects.splines, ERASE);
	saved_objects.splines->sfactors->s = last_origin_tension;
	for (c_tmp=saved_objects.splines->sfactors ; c_tmp->next != NULL ;
	    c_tmp=c_tmp->next)
		;
	c_tmp->s = last_extremity_tension;
	saved_objects.splines->type = T_CLOSED_XSPLINE;
	draw_spline(saved_objects.splines, PAINT);
    } else {
	if (closed_spline(saved_objects.splines)) {
	    saved_objects.splines->for_arrow = last_for_arrow;
	    saved_objects.splines->back_arrow = last_back_arrow;
	    last_for_arrow = last_back_arrow = NULL;
	  }
	toggle_open_closed_spline(saved_objects.splines, last_prev_point,
				  last_selected_point);
    }
    break;
  }
}

void swap_newp_lastp(void)
{
    int		    t;		/* swap new_position and last_position	*/

    t = new_position.x;
    new_position.x = last_position.x;
    last_position.x = t;
    t = new_position.y;
    new_position.y = last_position.y;
    last_position.y = t;
}

/*
 * Clean_up should be called before committing a user's request. Clean_up
 * will attempt to free all the allocated memories which resulted from
 * delete/remove action.  It will set the last_action to F_NULL.  Thus this
 * routine should be before set_action_object() and set_last_arrows(),
 * if they are to be called in the same routine.
 */

void clean_up(void)
{
    if (last_action[0] == F_EDIT) {
			// switch (last_five_objects[0]) {
	  	// 	case O_ARC:
	    // 		saved_objects.arcs->next = NULL;
	    // 		free_arc(&saved_objects.arcs);
	    // 		break;
	  	// 	case O_COMPOUND:
	    // 		saved_objects.compounds->next = NULL;
	    // 		free_compound(&saved_objects.compounds);
	    // 		break;
	  	// 	case O_ELLIPSE:
	    // 		saved_objects.ellipses->next = NULL;
	    // 		free_ellipse(&saved_objects.ellipses);
	    // 		break;
	  	// 	case O_POLYLINE:
	    // 		saved_objects.lines->next = NULL;
	    // 		free_line(&saved_objects.lines);
	    // 		break;
	  	// 	case O_SPLINE:
	    // 		saved_objects.splines->next = NULL;
	    // 		free_spline(&saved_objects.splines);
	    // 		break;
	  	// 	case O_TXT:
	    // 		saved_objects.texts->next = NULL;
	    // 		free_text(&saved_objects.texts);
	    // 		break;
	  	// 	case O_FIGURE:
	    // 		free((char *) saved_objects.comments);
	    // 		break;
			// }
		} else if (last_action[0] == F_DELETE || last_action[0] == F_JOIN || last_action[0] == F_SPLIT) {
			switch (last_five_objects[0]) {
	  		case O_ARC:
	    		// free_arc(&saved_objects.arcs);
	    		break;
	  		case O_COMPOUND:
	    		// free_compound(&saved_objects.compounds);
	    		break;
	  		case O_ELLIPSE:
	    		// free_ellipse(&saved_objects.ellipses);
	    		break;
	  		case O_POLYLINE:
	    		// free_line(&saved_objects.lines);
	    		break;
	  		case O_SPLINE:
	    		// free_spline(&saved_objects.splines);
	    		break;
	  		case O_TXT:
	    		// free_text(&saved_objects.texts);
	    		break;
	  		case O_ALL_OBJECT:
	    		free_arc(&saved_objects.arcs);
	    		free_compound(&saved_objects.compounds);
	    		free_ellipse(&saved_objects.ellipses);
	    		free_line(&saved_objects.lines);
	    		free_spline(&saved_objects.splines);
	    		free_text(&saved_objects.texts);
	    		break;
			}
		} else if (last_action[0] == F_DELETE_POINT || last_action[0] == F_ADD_POINT) {
			// if (last_action[0] == F_DELETE_POINT) {
			// 	/**************************************************
	    // 	free((char *) last_selected_point);
	    // 	free((char *) last_selected_sfactor);
			// 	**************************************************/
	    // 	last_next_point = NULL;
			// }
			// last_prev_point = NULL;
			// last_selected_point = NULL;
			// saved_objects.arcs = NULL;
			// saved_objects.compounds = NULL;
			// saved_objects.ellipses = NULL;
			// saved_objects.lines = NULL;
			// saved_objects.splines = NULL;
			// saved_objects.texts = NULL;
		} else if (last_action[0] == F_LOAD) {
			// free_arc(&saved_objects.arcs);
			// free_compound(&saved_objects.compounds);
			// free_ellipse(&saved_objects.ellipses);
			// free_line(&saved_objects.lines);
			// free_spline(&saved_objects.splines);
			// free_text(&saved_objects.texts);
		} else if (last_action[0] == F_GLUE) {
			// saved_objects.compounds = NULL;
		} else if (last_action[0] == F_BREAK) {
			// free((char *) saved_objects.compounds);
			// saved_objects.compounds = NULL;
		} else if (last_action[0] == F_ADD || last_action[0] == F_MOVE) {
			saved_objects.arcs = NULL;
			saved_objects.compounds = NULL;
			saved_objects.ellipses = NULL;
			saved_objects.lines = NULL;
			saved_objects.splines = NULL;
			saved_objects.texts = NULL;
			free_linkinfo(&last_links);
		} else if (last_action[0] == F_CONVERT) {
			if (last_five_objects[0] == O_POLYLINE)
	    	saved_objects.splines = NULL;
			else
	    	saved_objects.lines = NULL;
    } else if (last_action[0] == F_OPEN_CLOSE) {
        saved_objects.splines = NULL;
        saved_objects.lines = NULL;
	free((char *) last_for_arrow);
	free((char *) last_back_arrow);
} else if (last_action[0] == F_ADD_ARROW_HEAD ||
	       last_action[0] == F_DELETE_ARROW_HEAD) {
	saved_objects.splines = NULL;
	saved_objects.lines = NULL;
	saved_objects.arcs = NULL;
	last_prev_point = NULL;
	last_selected_point = NULL;
    }
    // last_action = F_NULL; //KAB
		// pop_undo_stack_action();
}

void set_latestarc(F_arc *arc)
{
	list_add_arc(&saved_objects.arcs, arc);
    // saved_objects.arcs = arc;
}

void set_latestobjects(F_compound *objects)
{
    saved_objects = *objects;
}

void set_latestcompound(F_compound *compound)
{
	list_add_compound(&saved_objects.compounds, compound);
    // saved_objects.compounds = compound;
}

void set_latestellipse(F_ellipse *ellipse)
{
	list_add_ellipse(&saved_objects.ellipses, ellipse);
    // saved_objects.ellipses = ellipse;
}

void set_latestline(F_line *line)
{
	list_add_line(&saved_objects.lines, line);
    // saved_objects.lines = line;
}

void set_latestspline(F_spline *spline)
{
	list_add_spline(&saved_objects.splines, spline);

    // saved_objects.splines = spline;
}

void set_latesttext(F_text *text)
{
	list_add_text(&saved_objects.texts, text);
    // saved_objects.texts = text;
}

void set_last_prevpoint(F_point *prev_point)
{
    last_prev_point = prev_point;
}

void set_last_selectedpoint(F_point *selected_point)
{
    last_selected_point = selected_point;
}

void set_last_selectedsfactor(F_sfactor *selected_sfactor)
{
  last_selected_sfactor = selected_sfactor;
}

void set_last_nextpoint(F_point *next_point)
{
    last_next_point = next_point;
}

void set_last_arcpointnum(int num)
{
    last_arcpointnum = num;
}

void set_lastposition(int x, int y)
{
    last_position.x = x;
    last_position.y = y;
}

void set_newposition(int x, int y)
{
    new_position.x = x;
    new_position.y = y;
}

void set_action(int action) // KAB replaced by push and pop functions
{
    // last_action = action;
		push_undo_stack_action(action);  // add recent action to stack
}

void set_action_object(int action, int object)
{
    // last_action = action;
		push_undo_stack_action(action);
		push_new_object(object);
    // last_object = object;
}

void set_lastlinkinfo(int mode, F_linkinfo *links)
{
    last_linkmode = mode;
    last_links = links;
}

void set_last_tension(double origin, double extremity)
{
  last_origin_tension = origin;
  last_extremity_tension = extremity;
}

void set_last_arrows(F_arrow *forward, F_arrow *backward)
{
      last_for_arrow = forward;
      last_back_arrow = backward;
}

//-------------------- Functions Added by Kyle Bielby ------------------------//
// pop the last action from the undo action stack
void pop_undo_stack_action()
{
	for(int i = 0; i < 5; i ++)
	{
    if(i != 4)
		{
			last_action[i] = last_action[i + 1];  // move stack actions up by index 1
		}
		else if(i == 4) {
			last_action[4] = F_NULL; // set last element in action stack to null
		}
	}
}

// push the given action onto the undo action stack
void push_undo_stack_action(int action) {
	for(int index = 4; index >= 0; index--) {
		if (index == 0) {
			last_action[0] = action;
		}
		else{
			last_action[index] = last_action[index - 1];
		}
  }
}

// pop the last action from the redo action stack
void pop_redo_stack_action() {
	for(int i = 0; i < 5; i++){
	}
	for(int i = 0; i < 5; i++) {
    if(i != 4){
			redo_action_stack[i] = redo_action_stack[i + 1];  // move stack actions up by index 1
		}
		else if(i == 4) {
			redo_action_stack[4] = NULL; // set last element in action stack to null
		}
	}
}

// push the given action onto the undo action stack
void push_redo_stack_action(int action) {
	for(int index = 0; index < 5; index++) {
		if (index != 4) {
			redo_action_stack[index + 1] = redo_action_stack[index];
			if (index == 0) {
				redo_action_stack[0] = action;
			}
		}
  }
}

void push_new_object(int new_object)
{
	for(int index = 4; index >= 0; index--) {
		if (index == 0) {
			last_five_objects[0] = new_object;
		}
		else{
			last_five_objects[index] = last_five_objects[index - 1];
		}
  }
}

void pop_last_object()
{
	for(int i = 0; i < 5; i++)
	{
		if(i != 4)
		{
			last_five_objects[i] = last_five_objects[i + 1];  // move stack actions up by index 1
		}
		else if(i == 4) {
			last_five_objects[4] = NULL; // set last element in action stack to null
		}
	}
}

void set_last_spline_object() {
	F_spline **head = &objects.splines;
	F_spline *this_head = *head;

	while(this_head->next != NULL) {
		this_head = this_head->next;
	}

	last_spline_object = *this_head;
}

// void set_last_spline() {
// 	f_spline * current_spline;
// 	f_spline * previous_spline;
// }

void display_object_splines() {
	F_spline **head = &objects.splines;
	F_spline *this_head = *head;

	while(this_head != NULL) {
		this_head = this_head->next;
	}
}

void display_object_ellipse() {
	F_spline **head = &objects.ellipses;
	F_spline *this_head = *head;

	while(this_head != NULL) {
		this_head = this_head->next;
	}
}

void display_saved_object_splines() {
	F_spline **head = &saved_objects.splines;
	F_spline *this_head = *head;

	while(this_head != NULL) {
		this_head = this_head->next;
	}
}

void display_objects() {
	F_compound * head = &objects;

	while(head->next != NULL) {
		head = head->next;
	}
}

void display_lines() {
	F_line **head = &objects.lines;
	F_line *this_head = *head;

	while(this_head != NULL) {
		this_head = this_head->next;
	}
}
//---------------- End Functions Added by Kyle Bielby ------------------------//
