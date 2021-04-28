#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include "bool.h"
#include "pi.h"

#include "fig2dev.h"
#include "object.h"	/* does #include <X11/xpm.h> */
#include "bound.h"
#include "creationdate.h"

static bool vdx_arrows(int line_thickness, F_arrow *for_arrow, F_arrow *back_arrow,
	F_pos *forw1, F_pos *forw2, F_pos *back1, F_pos *back2, int pen_color);	
static void generate_tile(int number, int colorIndex);
static void vdx_dash(int, double);

#define PREAMBLE "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>"


// Left over from gensvg.c
static unsigned int symbolchar[256]=
{0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0x0020,0x0021,0x2200,0x0023,0x2203,0x0025,
0x0026,0x220B,0x0028,0x0029,0x2217,0x002B,0x002C,0x2212,0x002E,0x002F,0x0030,
0x0031,0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003A,0x003B,
0x003C,0x003D,0x003E,0x003F,0x2245,0x0391,0x0392,0x03A7,0x0394,0x0395,
0x03A6,0x0393,0x0397,0x0399,0x03D1,0x039A,0x039B,0x039C,0x039D,0x039F,0x03A0,
0x0398,0x03A1,0x03A3,0x03A4,0x03A5,0x03C2,0x03A9,0x039E,0x03A8,0x0396,
0x005B,0x2234,0x005D,0x22A5,0x005F,0xF8E5,0x03B1,0x03B2,0x03C7,0x03B4,0x03B5,
0x03D5 /*0x03C6*/,0x03B3,0x03B7,0x03B9,0x03D5,0x03BA,0x03BB,0x03BC,0x03BD,0x03BF,
0x03C0,0x03B8,0x03C1,0x03C3,0x03C4,0x03C5,0x03D6,0x03C9,0x03BE,0x03C8,0x03B6,
0x007B,0x007C,0x007D,0x223C,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0, 0,0,
0,0,0x20AC,0x03D2,0x2032,0x2264,0x2044,0x221E,
0x0192,0x2663,0x2666,0x2665,0x2660,0x2194,0x2190,0x2191,0x2192,0x2193,0x00B0,
0x00B1,0x2033,0x2265,0x00D7,0x221D,0x2202,0x2022,0x00F7,0x2260,0x2261,0x2248,
0x2026,0xF8E6,0xF8E7,0x21B5,0x2135,0x2111,0x211C,0x2118,0x2297,0x2295,0x2205,
0x2229,0x222A,0x2283,0x2287,0x2284,0x2282,0x2286,0x2208,0x2209,0x2220,0x2207,
0xF6DA,0xF6D9,0xF6DB,0x220F,0x221A,0x22C5,0x00AC,0x2227,0x2228,0x21D4,0x21D0,
0x21D1,0x21D2,0x21D3,0x25CA,0x2329,0xF8E8,0xF8E9,0xF8EA,0x2211,0xF8EB,0xF8EC,
0xF8ED,0xF8EE,0xF8EF,0xF8F0,0xF8F1,0xF8F2,0xF8F3,0xF8F4,0,0x232A,0x222B,0x2320,
0xF8F5,0x2321,0xF8F6,0xF8F7,0xF8F8,0xF8F9,0xF8FA,0xF8FB,0xF8FC,0xF8FD,0xF8FE,0
};

// Left over from gensvg.c
static unsigned int dingbatchar[256]=
{0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0x0020,
0x2701,0x2702,0x2703,0x2704,0x260E,0x2706,0x2707,0x2708,0x2709,0x261B,
0x261E,0x270C,0x270D,0x270E,0x270F,0x2710,0x2711,0x2712,0x2713,0x2714,
0x2715,0x2716,0x2717,0x2718,0x2719,0x271A,0x271B,0x271C,0x271D,0x271E,
0x271F,0x2720,0x2721,0x2722,0x2723,0x2724,0x2725,0x2726,0x2727,0x2605,
0x2729,0x272A,0x272B,0x272C,0x272D,0x272E,0x272F,0x2730,0x2731,0x2732,
0x2733,0x2734,0x2735,0x2736,0x2737,0x2738,0x2739,0x273A,0x273B,0x273C,
0x273D,0x273E,0x273F,0x2740,0x2741,0x2742,0x2743,0x2744,0x2745,0x2746,
0x2747,0x2748,0x2749,0x274A,0x274B,0x25CF,0x274D,0x25A0,0x274F,0x2750,
0x2751,0x2752,0x25B2,0x25BC,0x25C6,0x2756,0x25D7,0x2758,0x2759,0x275A,
0x275B,0x275C,0x275D,0x275E,0,0xF8D7,0xF8D8,0xF8D9,0xF8DA,0xF8DB,
0xF8DC,0xF8DD,0xF8DE,0xF8DF,0xF8E0,0xF8E1,0xF8E2,0xF8E3,0xF8E4,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0x2761,0x2762,0x2763,
0x2764,0x2765,0x2766,0x2767,0x2663,0x2666,0x2665,0x2660,0x2460,0x2461,
0x2462,0x2463,0x2464,0x2465,0x2466,0x2467,0x2468,0x2469,0x2776,0x2777,
0x2778,0x2779,0x277A,0x277B,0x277C,0x277D,0x277E,0x277F,0x2780,0x2781,
0x2782,0x2783,0x2784,0x2785,0x2786,0x2787,0x2788,0x2789,0x278A,0x278B,
0x278C,0x278D,0x278E,0x278F,0x2790,0x2791,0x2792,0x2793,0x2794,0x2192,
0x2194,0x2195,0x2798,0x2799,0x279A,0x279B,0x279C,0x279D,0x279E,0x279F,
0x27A0,0x27A1,0x27A2,0x27A3,0x27A4,0x27A5,0x27A6,0x27A7,0x27A8,0x27A9,
0x27AA,0x27AB,0x27AC,0x27AD,0x27AE,0x27AF,0,0x27B1,0x27B2,0x27B3,0x27B4,
0x27B5,0x27B6,0x27B7,0x27B8,0x27B9,0x27BA,0x27BB,0x27BC,0x27BD,0x27BE,0
};

static int	tileno = -1;	/* number of current tile */
static int	pathno = -1;	/* number of current path */
static int	clipno = -1;	/* number of current clip path */

// Left over from gensvg.c
static unsigned int
rgbColorVal(int colorIndex)
{				/* taken from genptk.c */
    unsigned int rgb;
    static unsigned int rgbColors[NUM_STD_COLS] = {
	0x000000, 0x0000ff, 0x00ff00, 0x00ffff, 0xff0000, 0xff00ff,
	0xffff00, 0xffffff, 0x00008f, 0x0000b0, 0x0000d1, 0x87cfff,
	0x008f00, 0x00b000, 0x00d100, 0x008f8f, 0x00b0b0, 0x00d1d1,
	0x8f0000, 0xb00000, 0xd10000, 0x8f008f, 0xb000b0, 0xd100d1,
	0x803000, 0xa14000, 0xb46100, 0xff8080, 0xffa1a1, 0xffbfbf,
	0xffe0e0, 0xffd600
    };

    if (colorIndex == DEFAULT)
	rgb = rgbColors[0];
    else if (colorIndex < NUM_STD_COLS)
	rgb = rgbColors[colorIndex];
    else
	rgb = ((user_colors[colorIndex - NUM_STD_COLS].r & 0xff) << 16)
	    | ((user_colors[colorIndex - NUM_STD_COLS].g & 0xff) << 8)
	    | (user_colors[colorIndex - NUM_STD_COLS].b & 0xff);
    return rgb;
}

// Left over from gensvg.c
static unsigned int
rgbFillVal(int colorIndex, int area_fill)
{
    unsigned int	rgb, r, g, b;
    double	t;
    short	tintflag = 0;

    if (colorIndex == BLACK_COLOR || colorIndex == DEFAULT) {
	if (area_fill > NUMSHADES - 1)
	    area_fill = NUMSHADES - 1;
	area_fill = NUMSHADES - 1 - area_fill;
	colorIndex = WHITE_COLOR;
    }

    rgb = rgbColorVal(colorIndex);

    if (area_fill > NUMSHADES - 1) {
	tintflag = 1;
	area_fill -= NUMSHADES - 1;
    }

    t = (double) area_fill / (NUMSHADES - 1);
    if (tintflag) {
	r = ((rgb & ~0xFFFF) >> 16);
	g = ((rgb & 0xFF00) >> 8);
	b = (rgb & ~0xFFFF00) ;

	r += t * (0xFF-r);
	g += t * (0xff-g);
	b += t * (0xff-b);

	rgb = ((r &0xff) << 16) + ((g&0xff) << 8) + (b&0xff);
    } else {
	rgb = (((int) (t * ((rgb & ~0xFFFF) >> 16)) << 16) +
		((int) (t * ((rgb & 0xFF00) >> 8)) << 8) +
		((int) (t * (rgb & ~0xFFFF00))) );
    }

    return rgb;
}

// Left over from gensvg.c
static double
degrees(double angle)
{
   return -angle / M_PI * 180.0;
}

// Left over from gensvg.c
static double
linewidth_adj(int linewidth)
{
   /* Adjustment as in genps.c */
   return linewidth <= THICK_SCALE ?
		linewidth / 2. : (double)(linewidth-THICK_SCALE);
}

void
genvdx_option(char opt, char *optarg)
{
    switch (opt) {
	case 'G':		/* ignore language and grid */
	case 'L':
	    break;
	case 'z':
	    (void) strcpy (papersize, optarg);
	    paperspec = true;
	    break;
	default:
	    put_msg (Err_badarg, opt, "vdx");
	    exit (1);
    }
}

void
genvdx_start(F_compound *objects)
{
	struct 	paperdef	*pd;
	int     pagewidth = -1, pageheight = -1;
	int     vw, vh;
	char    date_buf[CREATION_TIME_LEN];
	time_t  when;
	char    stime[80];

	fprintf(tfp, "%s\n", PREAMBLE);

	paperspec = true;

	if (paperspec) {
		/* convert paper size from ppi to inches */
		for (pd = paperdef; pd->name != NULL; ++pd)
			if (strcasecmp(papersize, pd->name) == 0) {
				pagewidth = pd->width;
				pageheight = pd->height;
				strcpy(papersize, pd->name);	/* use the "nice" form */
				break;
			}
		if (pagewidth < 0 || pageheight < 0) {
			(void) fprintf(stderr, "Unknown paper size `%s'\n", papersize);
			exit(1);
		}
		if (landscape) {
			vh = pagewidth;
			vw = pageheight;
		}
		else {
			vw = pagewidth;
			vh = pageheight;
		}
	}
	else {
		vw = ceil((urx - llx) * 72. * mag / ppi);
		vh = ceil((ury - lly) * 72. * mag / ppi);
	}

	// Change these to the proper vdx stuff
    	// Visio Document Line
	fputs("<VisioDocument xmlns=\"http://schemas.microsoft.com/visio/2003/core\" ", tfp);
    	fputs("xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n", tfp);
	// Document Properties Lines
	fprintf(tfp, "\t<DocumentPropterties>\n");
	fprintf(tfp, "\t\t<Creator>%s Version %s</Creator>\n", prog, PACKAGE_VERSION);
	if (creation_date(date_buf))
		fprintf(tfp, "\t\t<TimeCreated>%s</TimeCreated>\n", date_buf);
	fprintf(tfp, "\t</DocumentProperties>\n");

	// Pages and Page Sections
	fputs("\t<Pages>\n", tfp);
	fputs("\t\t<PageSheet>\n", tfp);
	fputs("\t\t\t<PageProps>\n", tfp);
	fprintf(tfp, "\t\t\t\t<PageWidth>%d</PageWidth>\n", pagewidth); // Fix these values
	fprintf(tfp, "\t\t\t\t<PageHeight>%d</PageHeight>\n", pageheight);
   	// Comments Section
	if (objects->comments)
		print_comments("<desc>", objects->comments, "</desc>");

	// End of Page Sections
	fputs("\t\t\t</PageProps>\n", tfp);
	fputs("\t\t</PageSheet>\n", tfp);

	// Begin Shapes
	fputs("\t\t<Shapes>\n", tfp);

}

int
genvdx_end(void)
{
	fputs("\t\t</Shapes>\n", tfp);
	fputs("\t</Pages>\n", tfp);
    fputs("</VisioDocument>", tfp);
    return 0;
}

// Left over from gensvg.c
#define	INIT	-9	/* Change this, if pen_color may be negative. */
#define	CLIP	-8

#define	INIT_PAINT(fill_style) \
		if (fill_style > NUMFILLS)

#define	INIT_PAINT_W_CLIP(fill_style, thickness, for_arrow, back_arrow,	\
			  forw1, forw2, back1, back2)			\
					\
	(void) vdx_arrows(thickness, for_arrow, back_arrow,	\
		forw1, forw2, back1, back2, CLIP);		\
	if (fill_style == UNFILLED)				\
		

void
continue_paint_vdx(int fill_style, int pen_color, int fill_color)
{
    if (fill_style > NUMFILLS) {
		generate_tile(fill_style - NUMFILLS, pen_color);
		fprintf(tfp, "\t\t\t\t\t\t<FillColor>#%6.6x</FillColor>\n", rgbColorVal(fill_color));
		fputs("\t\t\t\t\t</Pattern>\n", tfp);
    } else if (fill_style > UNFILLED) {	/* && fill_style <= NUMFILLS */
		// Fill
		fputs("\t\t\t\t<Fill>\n", tfp);
		fprintf(tfp, "\t\t\t\t\t<FillColor>#%6.6x</FillColor>\n", rgbFillVal(fill_color, fill_style));
		fputs("\t\t\t\t</Fill>\n", tfp);
    }
}

void
continue_paint_w_clip_vdx(int fill_style, int pen_color, int fill_color)
{
    if (fill_style > UNFILLED) {
		if (fill_style > NUMFILLS) {
			generate_tile(fill_style - NUMFILLS, pen_color);
		}
		if (fill_style > NUMFILLS) {
			fputs("\t\t\t\t<Fill>\n", tfp);
			fprintf(tfp, "\t\t\t\t\t<FillColor>#%6.6x</FillColor>\n", rgbColorVal(fill_color));
			fputs("\t\t\t\t</Fill>\n", tfp);
		} else {
			fputs("\t\t\t\t<Fill>\n", tfp);
			fprintf(tfp, "\t\t\t\t\t<FillColor>#%6.6x</FillColor>\n", rgbFillVal(fill_color, fill_style));
			fputs("\t\t\t\t</Fill>\n", tfp);
		}
    }
}

void
genvdx_line(F_line *l)
{
	char	chars;
	int		px,py;
	int		px2,py2,width,height,rotation;
	F_point	*p;


	if (l->type == T_PIC_BOX ) {
		// Shape
		fputs("\t\t\t<Shape ", tfp);
		fputs("Name='Image' Type='Shape'>\n", tfp);
		fprintf(tfp, "\t\t\t\t<ImageLink>xlink:href=\"file://%s\"</ImageLink>\n", l->pic->file);
		// Left over from gensvg.c
		p = l->points;
		px = p->x;
		py = p->y;
		px2 = p->next->next->x;
		py2 = p->next->next->y;
		width = px2 - px;
		height = py2 - py;
		rotation = 0;
		if (width<0 && height < 0)
			rotation = 180;
		else if (width < 0 && height >= 0)
			rotation = 90;
		else if (width >= 0 && height <0)
			rotation = 270;
		if (l->pic->flipped) rotation -= 90;
		if (width < 0) {
			px = px2;
			width = -width;
		}
		if (height < 0) {
			py = py2;
			height = -height;
		}
		px2 = px + width/2;
		py2 = py + height/2;
		// XForm
		fputs("\t\t\t\t<XForm>\n", tfp);
		fprintf(tfp, "\t\t\t\t\t<PinX>%d</PinX>\n", px); // x coord
		fprintf(tfp, "\t\t\t\t\t<PinY>%d</PinY>\n", py); // y coord
		fprintf(tfp, "\t\t\t\t\t<Width>%d</Width>\n", width); // width
		fprintf(tfp, "\t\t\t\t\t<Height>%d</Height>\n", height); // height

		if (l->pic->flipped) {
			fprintf(tfp, "\t\t\t\t\t<Rotation>(%d %d %d)</Rotation>\n", rotation, px2, py2); // rotation
			fprintf(tfp, "\t\t\t\t\t<Scale>(-1, 1)</Scale>\n"); // scale
			fprintf(tfp, "\t\t\t\t\t<Translation>(%d, %d)</Translation>\n", -2*px2, 0); // translation
		} else if (rotation !=0) {
			fprintf(tfp, "\t\t\t\t\t<Rotation>(%d %d %d)</Rotation>\n", rotation, px2, py2); // rotation
		}
		fputs("\t\t\t\t</XForm>\n", tfp);
		fputs("\t\t\t</Shape>\n", tfp);

		return;
    }

    if (l->thickness <= 0 && l->fill_style == UNFILLED &&
			!l->for_arrow && !l->back_arrow)
	return;

    /* l->type == T_BOX, T_ARC_BOX, T_POLYGON or T_POLYLINE */

    if (l->type == T_BOX || l->type == T_ARC_BOX || l->type == T_POLYGON) {

	INIT_PAINT(l->fill_style);

	// Shape
	fputs("\t\t\t<Shape ", tfp);
	if (l->type == T_POLYGON) {
		chars = fputs("Name='Polygon' Type='Shape'>\n", tfp);
		// XForm
		fputs("\t\t\t\t<XForm>\n", tfp);
		fputs("\t\t\t\t\t<PolyPoints>", tfp);
	    for (p = l->points; p->next; p = p->next) {
			chars += fprintf(tfp, "%d, %d", p->x , p->y);
	    }
		fputs("</PolyPoints>\n", tfp);
		fputs("\t\t\t\t</XForm>\n", tfp);
	} 
	else {	/* T_BOX || T_ARC_BOX */
		// Left over from gensvg.c
	    px = l->points->next->next->x;
	    py = l->points->next->next->y;
	    width = l->points->x - px;
	    height = l->points->y - py;
	    if (width < 0) {
		px = l->points->x;
		width = -width;
	    }
	    if (height < 0) {
		py = l->points->y;
		height = -height;
	    }
		
		if (l->type == T_BOX) {	
			fputs("Name='Box' Type='Shape'>\n", tfp);
		}
		else if (l->type == T_ARC_BOX) {
			fputs("Name='Arc Box' Type='Shape'>\n", tfp);
		}
		
		// XForm
		fputs("\t\t\t\t<XForm>\n", tfp);
		fprintf(tfp, "\t\t\t\t\t<PinX>%d</PinX>\n", px); // x coord
		fprintf(tfp, "\t\t\t\t\t<PinY>%d</PinY>\n", py); // y coord
		fprintf(tfp, "\t\t\t\t\t<Width>%d</Width>\n", width); // width
		fprintf(tfp, "\t\t\t\t\t<Height>%d</Height>\n", height); // height
		if (l->type == T_ARC_BOX) {
			fprintf(tfp, "\t\t\t\t\t<Radius>%d</Radius>\n", l->radius); // radius
			// fprintf(tfp, " rx=\"%d\"", l->radius);
		}
		fputs("\t\t\t\t</XForm>\n", tfp);
	}

    continue_paint_vdx(l->fill_style, l->pen_color, l->fill_color);
	   
	// Line
	fputs("\t\t\t\t<Line>\n", tfp);
	if (l->thickness) {
		fprintf(tfp, "\t\t\t\t\t<LineColor>#%6.6x</LineColor>\n", rgbColorVal(l->pen_color));
		fprintf(tfp, "\t\t\t\t\t<LineWeight>%dpx</LineWeight>\n", (int) ceil(linewidth_adj(l->thickness)));
		if (l->style > SOLID_LINE)
			vdx_dash(l->style, l->style_val);
	}
	fputs("\t\t\t\t</Line>\n", tfp);
	fputs("\t\t\t</Shape>\n", tfp);

	return;
	}

    if (l->type == T_POLYLINE) {
	bool	has_clip = false;
	// Left over from gensvg.c
	if (l->for_arrow || l->back_arrow) {
	    has_clip = vdx_arrows(l->thickness, l->for_arrow, l->back_arrow,
			    &(l->last[1]), l->last, (F_pos *)l->points->next,
			    (F_pos *)l->points, INIT);
	    if (l->fill_style == UNFILLED && l->thickness <= 0) {
		(void) vdx_arrows(l->thickness, l->for_arrow, l->back_arrow,
			    &(l->last[1]), l->last, (F_pos *)l->points->next,
			    (F_pos *)l->points, l->pen_color);
		return;
	    }
	}
	// Left over from gensvg.c
	if (has_clip) {
	    INIT_PAINT_W_CLIP(l->fill_style, l->thickness, l->for_arrow,
		    l->back_arrow, &(l->last[1]), l->last,
		    (F_pos *)l->points->next, (F_pos *)l->points);
	} else {
	    INIT_PAINT(l->fill_style);
	}

	// Shape
	fputs("\t\t\t<Shape ", tfp);
	fputs("Name='PolyLine' Type='Shape'>\n", tfp);
	// XForm
	fputs("\t\t\t\t<XForm>\n", tfp);
	chars = fputs("\t\t\t\t\t<PolyLinePoints>", tfp);

	for (p = l->points; p; p = p->next) {
	    chars += fprintf(tfp, "%d, %d", p->x , p->y);
	}
	fputs("</PolyLinePoints>\n", tfp);
	fputs("\t\t\t\t</XForm>\n", tfp);

	if (has_clip)
	    continue_paint_w_clip_vdx(l->fill_style, l->pen_color, l->fill_color);
	else
	    continue_paint_vdx(l->fill_style, l->pen_color, l->fill_color);

	// Line
	fputs("\t\t\t\t<Line>\n", tfp);
	if (l->thickness) {
		fprintf(tfp, "\t\t\t\t\t<LineColor>#%6.6x</LineColor>\n", rgbColorVal(l->pen_color));
		fprintf(tfp, "\t\t\t\t\t<LineWeight>%dpx</LineWeight>\n", (int) ceil(linewidth_adj(l->thickness)));
	    if (l->style > SOLID_LINE)
		vdx_dash(l->style, l->style_val);
	}
	fputs("\t\t\t\t</Line>\n", tfp);
	fputs("\t\t\t</Shape>\n", tfp);
	if (l->for_arrow || l->back_arrow)
	    (void) vdx_arrows(l->thickness, l->for_arrow, l->back_arrow,
			&(l->last[1]), l->last, (F_pos *)l->points->next,
			(F_pos *)l->points, l->pen_color);
    }	/* l->type == T_POLYLINE */
	
}
// For circles
void
genvdx_spline( /* not used by fig2dev */
	F_spline *s)
{
    F_point *p;
	
	// Shape
	fputs("\t\t\t<Shape Name='Spline' Type='Shape'>\n", tfp);
	// XForm
	fputs("\t\t\t\t<XForm>\n", tfp);
	fprintf(tfp, "\t\t\t\t\t<PinX>%d</PinX>\n", s->points->x); // x coord
	fprintf(tfp, "\t\t\t\t\t<PinY>%d</PinY>\n", s->points->y); // y coord
	fputs("\t\t\t\t<XForm>\n", tfp);
	// Line
	fputs("\t\t\t\t<Line>\n", tfp);
	fprintf(tfp, "\t\t\t\t\t<LineColor>#%6.6x</LineColor>\n", rgbColorVal(s->pen_color));
	fprintf(tfp, "\t\t\t\t\t<LineWeight>%dpx</LineWeight>\n", (int) ceil(linewidth_adj(s->thickness)));
	fputs("\t\t\t\t</Line>\n", tfp);
	fputs("\t\t\t</Shape>\n", tfp);
}

// For Arcs
void
genvdx_arc(F_arc *a)
{
    bool    has_clip = false;
    double  radius;
    double  x, y, angle, dx, dy;
    F_pos   forw1, forw2, back1, back2;

    if (a->fill_style == UNFILLED && a->thickness <= 0 &&
	    !a->for_arrow && !a->back_arrow)
	return;

	// Shape
	fputs("\t\t\t<Shape Name='Arc' Type='Shape'>\n", tfp);
	// Left over from gensvg.c
    if (a->for_arrow || a->back_arrow) {
	if (a->for_arrow) {
	    forw2.x = a->point[2].x;
	    forw2.y = a->point[2].y;
	    compute_arcarrow_angle(a->center.x, a->center.y,
		    (double) forw2.x, (double) forw2.y, a->direction,
		    a->for_arrow, &(forw1.x), &(forw1.y));
	}
	if (a->back_arrow) {
	    back2.x = a->point[0].x;
	    back2.y = a->point[0].y;
	    compute_arcarrow_angle(a->center.x, a->center.y,
		    (double) back2.x, (double) back2.y, a->direction ^ 1,
		    a->back_arrow, &(back1.x), &(back1.y));
	}
	has_clip = vdx_arrows(a->thickness, a->for_arrow, a->back_arrow,
				&forw1, &forw2, &back1, &back2, INIT);
	if (a->fill_style == UNFILLED && a->thickness <= 0) {
	    (void) vdx_arrows(a->thickness, a->for_arrow, a->back_arrow,
				&forw1, &forw2, &back1, &back2, a->pen_color);
	    return;
	}
    }

    dx = a->point[0].x - a->center.x;
    dy = a->point[0].y - a->center.y;
    radius = sqrt(dx * dx + dy * dy);

    x = (a->point[0].x-a->center.x) * (a->point[2].x-a->center.x) +
		(a->point[0].y-a->center.y) * (a->point[2].y-a->center.y);
    y = (a->point[0].x-a->center.x) * (a->point[2].y-a->center.y) -
		(a->point[0].y-a->center.y) * (a->point[2].x-a->center.x);

    if (x == 0.0 && y == 0.0)
	angle=0.0;
    else
	angle = atan2(y,x);
    if (angle < 0.0) angle += 2.*M_PI;
    angle *= 180./M_PI;
    if (a->direction == 1)
	angle = 360. - angle;

    if (has_clip) {
	INIT_PAINT_W_CLIP(a->fill_style, a->thickness, a->for_arrow,
		a->back_arrow, &forw1, &forw2, &back1, &back2);
    } else {
	INIT_PAINT(a->fill_style);
    }

	// XForm
	fputs("\t\t\t\t<XForm>\n", tfp);
	fprintf(tfp, "\t\t\t\t\t<PinX>%d</PinX>\n", a->point[0].x); // x coord
	fprintf(tfp, "\t\t\t\t\t<PinY>%d</PinY>\n", a->point[0].y); // y coord
	fprintf(tfp, "\t\t\t\t\t<Radius>%ld</Radius>\n", lround(radius));
	fputs("\t\t\t\t<XForm>\n", tfp);


    /* paint the object */
	// Left over from gensvg.c
    if (a->type == T_PIE_WEDGE_ARC)
		fprintf(tfp, " %ld,%ld L",
			lround(a->center.x), lround(a->center.y));
    if (a->type == T_PIE_WEDGE_ARC)
	fputs(" z", tfp);

    if (has_clip)
	continue_paint_w_clip_vdx(a->fill_style, a->pen_color, a->fill_color);
    else
	continue_paint_vdx(a->fill_style, a->pen_color, a->fill_color);

	// Line
	fputs("\t\t\t\t<Line>\n", tfp);

    if (a->thickness) {
	fprintf(tfp, "\t\t\t\t\t<LineColor>#%6.6x</LineColor>\n", rgbColorVal(a->pen_color));
	fprintf(tfp, "\t\t\t\t\t<LineWeight>%dpx</LineWeight>\n", (int) ceil(linewidth_adj(a->thickness)));
	if (a->style > SOLID_LINE)
	    vdx_dash(a->style, a->style_val);
    }
	fputs("\t\t\t\t</Line>\n", tfp);
	fputs("\t\t\t</Shape>\n", tfp);

    if (a->for_arrow || a->back_arrow)
	(void) vdx_arrows(a->thickness, a->for_arrow, a->back_arrow,
			&forw1, &forw2, &back1, &back2, a->pen_color);
}

//For Ellipses
void
genvdx_ellipse(F_ellipse *e)
{
	int cx = e->center.x ;
	int cy = e->center.y ;

	if (e->type == T_CIRCLE_BY_RAD || e->type == T_CIRCLE_BY_DIA) {
		int r = e->radiuses.x ;
		int diam = r*2;
		fputs("\t\t\t<Shape ", tfp);
		if(e->type == T_CIRCLE_BY_RAD) {
			fputs("Name='Circle by Radius' ", tfp);
		}
		else if(e->type == T_CIRCLE_BY_DIA) {
			fputs("Name='Circle by Diameter' ",tfp);
		}
		fputs("Type='Shape'>\n", tfp);


		INIT_PAINT(e->fill_style);

		// XForm
		fputs("\t\t\t\t<XForm>\n", tfp);
		fprintf(tfp, "\t\t\t\t\t<PinX F='Inh'>%d</PinX>\n", cx);
		fprintf(tfp, "\t\t\t\t\t<PinY F='Inh'>%d</PinY>\n", cy);
		fprintf(tfp, "\t\t\t\t\t<Width>%d</Width>\n", diam);
		fprintf(tfp, "\t\t\t\t\t<Height>%d</Height>\n", diam);
		fputs("\t\t\t\t</XForm>\n", tfp);
	}
	else { /* T_ELLIPSE_BY_RAD or T_ELLIPSE_BY_DIA */
		int rx = e->radiuses.x ;
		int ry = e->radiuses.y ;
		fputs("\t\t\t<Shape ", tfp);
		if(e->type == T_ELLIPSE_BY_RAD) {
			fputs("Name='Ellipse by Radius' ", tfp);
		}
		else if(e->type == T_ELLIPSE_BY_DIA) {
			fputs("Name='Ellipse by Diameter' ",tfp);
		}
		fputs("Type='Shape'>\n", tfp);


		INIT_PAINT(e->fill_style);

		// XForm
		fputs("\t\t\t\t<XForm>\n", tfp);
		fprintf(tfp, "\t\t\t\t\t<PinX>%d</PinX>\n", cx); // center x coord
		fprintf(tfp, "\t\t\t\t\t<PinY>%d</PinY>\n", cy); // center y coord
		fprintf(tfp, "\t\t\t\t\t<Width>%d</Width>\n", rx); // width
		fprintf(tfp, "\t\t\t\t\t<Height>%d</Height>\n", ry); // height
		fputs("\t\t\t\t</XForm>\n", tfp);

    } /* end T_CIRCLE... or T_ELLIPSE... */

	// Line
	fputs("\t\t\t\t<Line>\n", tfp);

	continue_paint_vdx(e->fill_style, e->pen_color, e->fill_color);

	if (e->thickness) {
		fprintf(tfp, "\t\t\t\t\t<LineColor>#%6.6x</LineColor>\n", rgbColorVal(e->pen_color));
		fprintf(tfp, "\t\t\t\t\t<LineWeight>%dpx</LineWeight>\n", (int) ceil(linewidth_adj(e->thickness)));
	if (e->style > SOLID_LINE)
		vdx_dash(e->style, e->style_val);
	}
	fputs("\t\t\t\t</Line>\n", tfp);
	fputs("\t\t\t</Shape>\n", tfp);
}

void
genvdx_text(F_text *t)
{
    unsigned char *cp;
    int ch;
    const char *anchor[3] = { "start", "middle", "end" };
    static const char *family[9] = { "Times", "AvantGarde",
	"Bookman", "Courier", "Helvetica", "Helvetica Narrow",
	"New Century Schoolbook", "Palatino", "Times,Symbol"
    };
    int x = t->base_x ;
    int y = t->base_y ;
    int dy = 0;

	// Shape
	fputs("\t\t\t<Shape Name='Text' Type='Shape'>\n", tfp);
	fputs("\t\t\t\t<Text>", tfp);

	// Left over from gensvg.c
    if (t->font == 32) {
	for (cp = (unsigned char *) t->cstring; *cp; cp++) {
		ch=*cp;
	    fprintf(tfp, "&#%d;", symbolchar[ch]);
	}
    }
    else if (t->font == 34) {
	for (cp = (unsigned char *) t->cstring; *cp; cp++) {
		ch=*cp;
	    fprintf(tfp, "&#%d;", dingbatchar[ch]);
	}
    }
    else if (special_text(t)) {
    int supsub=0;
#ifdef NOSUPER
    int old_dy=0;
#endif
    dy=0;
	for (cp = (unsigned char *) t->cstring; *cp; cp++) {
	    ch = *cp;
	    if (( supsub == 2 &&ch == '}' ) || supsub==1) {
#ifdef NOSUPER
		fprintf(tfp,"</tspan><tspan dy=\"%d\">",-dy);
		old_dy=-dy;
#else
		fprintf(tfp,"</tspan>");
#endif
		supsub=0;
		if (ch == '}') {
		  cp++;
		  ch=*cp;
		}
	    }
	   if (ch == '_' || ch == '^') {
		supsub=1;
#ifdef NOSUPER
		if (dy != 0) fprintf(tfp,"</tspan>");
		if (ch == '_') dy=35.;
		if (ch == '^') dy=-50.;
		fprintf(tfp,"<tspan font-size=\"%d\" dy=\"%d\">",
			(int) ceil(t->size * 8), dy+old_dy);
		old_dy=0;
#else
		fprintf(tfp,"<tspan font-size=\"%d\" baseline-shift=\"",
			(int) ceil(t->size * 8));
		if (ch == '_') fprintf(tfp,"sub\">");
		if (ch == '^') fprintf(tfp,"super\">");
#endif
		cp++;
		ch=*cp;
		if (ch == '{' ) {
		  supsub=2;
		  cp++;
		  ch=*cp;
		}
	    }
#ifdef NOSUPER
		else old_dy=0;
#endif
	    if (ch < 128 && ch != 38 && ch != 60 && ch != 62
	    && ch != '$')
		(void)fputc(ch, tfp);
	    else if (ch != '$')
		fprintf(tfp, "&#%d;", ch);
    }
    } else {
	for (cp = (unsigned char *) t->cstring; *cp; cp++) {
	    ch = *cp;
	    if (ch < 128 && ch != 38 && ch != 60 && ch != 62)
		(void)fputc(ch, tfp);
	    else
		fprintf(tfp, "&#%d;", ch);
	}
    }
#ifdef NOSUPER
    if (dy != 0) fprintf(tfp,"</tspan>");
#endif
	
	fputs("</Text>\n", tfp);
	
	// XForm
	fputs("\t\t\t\t<XForm>\n", tfp);
	fprintf(tfp, "\t\t\t\t\t<PinX>%d</PinX>\n", x); // center x coord
	fprintf(tfp, "\t\t\t\t\t<PinY>%d</PinY>\n", y); // center y coord
	fprintf(tfp, "\t\t\t\t\t<Angle>%.0f</Angle>\n", degrees(t->angle));
	fputs("\t\t\t\t</XForm>\n", tfp);

	// Fill
	fputs("\t\t\t\t<Fill>\n", tfp);
	fprintf(tfp, "\t\t\t\t\t<FillColor>#%6.6x</FillColor>\n", rgbColorVal(t->color));
	fputs("\t\t\t\t</Fill>\n", tfp);
	
	// Font
	fputs("\t\t\t\t<Font>\n", tfp);
	fprintf(tfp, "\t\t\t\t\t<Font>%s</Font>\n\t\t\t\t\t<FontSize>%d</FontSize>\n",
		family[t->font / 4],
		(int)ceil(t->size * 12));
		
	fputs("\t\t\t\t</Font>\n", tfp);
	fputs("\t\t\t</Shape>\n", tfp);
}

static void
arrow_path(F_arrow *arrow, F_pos *arrow2, int pen_color, int npoints,
	F_pos points[], int nfillpoints, F_pos *fillpoints
#ifdef DEBUG
	, int nclippoints, F_pos clippoints[]
#endif
	)
{
    int	    i, chars;


    fprintf(tfp, "%d, %d", arrow2->x, arrow2->y);
    chars = fprintf(tfp, "</Arrow>\n\t\t\t\t<%sPoints>",
	    (points[0].x == points[npoints-1].x &&
	     points[0].y == points[npoints-1].y ? "Polygon" : "Polyline"));
    for (i = 0; i < npoints; ++i) {
	chars += fprintf(tfp, " %d,%d", points[i].x ,
		points[i].y );

    }
	fprintf(tfp, "</%sPoints>\n",
	    (points[0].x == points[npoints-1].x &&
	     points[0].y == points[npoints-1].y ? "Polygon" : "Polyline"));
	fputs("\t\t\t\t<Line>\n", tfp);
	fprintf(tfp, "\t\t\t\t\t<LineColor>#%6.6x</LineColor>\n", rgbColorVal(pen_color));
	fprintf(tfp, "\t\t\t\t\t<LineWeight>%dpx</LineWeight>\n", (int) ceil(linewidth_adj(arrow->thickness)));
	fputs("\t\t\t\t</Line>\n", tfp);
	fputs("\t\t\t</Shape>\n", tfp);
}

static bool
vdx_arrows(int line_thickness, F_arrow *for_arrow, F_arrow *back_arrow,
	F_pos *forw1, F_pos *forw2, F_pos *back1, F_pos *back2, int pen_color)
{
    static int		fnpoints, fnfillpoints, fnclippoints;
    static int		bnpoints, bnfillpoints, bnclippoints;
    static F_pos	fpoints[50], ffillpoints[50], fclippoints[50];
    static F_pos	bpoints[50], bfillpoints[50], bclippoints[50];
    int			i;

    if (pen_color == INIT) {
	if (for_arrow) {
	    calc_arrow(forw1->x, forw1->y, forw2->x, forw2->y,
		    line_thickness, for_arrow, fpoints, &fnpoints,
		    ffillpoints, &fnfillpoints, fclippoints, &fnclippoints);
	}
	if (back_arrow) {
	    calc_arrow(back1->x, back1->y, back2->x, back2->y,
		    line_thickness, back_arrow, bpoints, &bnpoints,
		    bfillpoints, &bnfillpoints, bclippoints, &bnclippoints);
	}
	if (fnclippoints || bnclippoints)
	    return true;
	else
	    return false;
    }

	fputs("\t\t\t<Shape Name='ArrowHead' Type='Shape'>\n", tfp);
    if (for_arrow) {
	fputs("\t\t\t\t<Arrow>", tfp);
	arrow_path(for_arrow, forw2, pen_color, fnpoints, fpoints,
		fnfillpoints, ffillpoints
	
#ifdef DEBUG
		, fnclippoints, fclippoints
#endif
		);
	
    }
    if (back_arrow) {
	fputs("\t\t\t\t<Arrow>", tfp);
	arrow_path(back_arrow, back2, pen_color, bnpoints, bpoints,
		bnfillpoints, bfillpoints
#ifdef DEBUG
		, bnclippoints, bclippoints
#endif
		);

    }
    return true;
}

static void
generate_tile(int number, int colorIndex)
{
	// Pattern
	fputs("\t\t\t\t\t<Pattern>\n", tfp);
	fprintf(tfp, "\t\t\t\t\t\t<PatternID>%d</PatternID>\n", ++tileno);
	fprintf(tfp, "\t\t\t\t\t\t<LineWidth>%.2g</LineWidth>\n", 0.5*ppi/80.);
	fprintf(tfp, "\t\t\t\t\t\t<LineColor>#%6.6x</LineColor>\n", rgbColorVal(colorIndex));
}

static void
vdx_dash(int style, double val)
{
	fputs("\t\t\t\t\t<DashArray>", tfp);
	// Left over from gensvg.c
	switch(style) {
	case 1:
	default:
		fprintf(tfp,"%ld %ld", lround(val*10), lround(val*10));
		break;
	case 2:
		fprintf(tfp,"10 %ld", lround(val*10));
		break;
	case 3:
		fprintf(tfp,"%ld %ld 10 %ld", lround(val*10),
			lround(val*5), lround(val*5));
		break;
	case 4:
		fprintf(tfp,"%ld %ld 10 %ld 10 %ld", lround(val*10),
			lround(val*3), lround(val*3), lround(val*3));
		break;
	case 5:
		fprintf(tfp,"%ld %ld 10 %ld 10 %ld 10 %ld", lround(val*10),
			lround(val*3), lround(val*3), lround(val*3),
			lround(val*3));
		break;
	}
	fputs("</DashArray>\n", tfp);
}

/* driver defs */

struct driver dev_vdx = {
    genvdx_option,
    genvdx_start,
    gendev_nogrid,
    genvdx_arc,
    genvdx_ellipse,
    genvdx_line,
    genvdx_spline,
    genvdx_text,
    genvdx_end,
    INCLUDE_TEXT
};
