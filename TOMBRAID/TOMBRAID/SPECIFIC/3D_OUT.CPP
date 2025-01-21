//#include "3Dglodef.h"

#include "3d_out.h"
#include "types.h"
#include "vars.h"
#include "winmain.h"
#include "3D_LINE.H"
#include "SCALESPR.H"

//extern	XBUF_PERSP	xbuffer[];



XBUF_PERSP 	xbuffer[MAX_SCANLINES] = {0};         	// Edge buffer for Polygon Draw

/* Mask for deciding whether to single or double pixel */
#define SINGLE_MASK 0xffff8000

/****************************************************************************
 *			Static Variables Used In Output Loops Etc...
 ***************************************************************************/
	static	int		xgen_ymin;
	static	int		xgen_ymax;

/****************************************************************************
 *!				Draw Flat Shaded Polygon..
 ***************************************************************************/

/* The assembler functions were written by Paul assuming Watcom
	register passing conventions, and so expect parameters to be passed
	in EAX,EDX,EBX,ECX */

void	draw_poly_flat( short int *iptr )
{
	int colour;

	if ( xgen_x(iptr+1) )
	{
		colour = *iptr;
		__asm
		{
			mov eax,xgen_ymin
			mov edx,xgen_ymax
			mov ebx,colour
			call flatA
		}
	}
}

/****************************************************************************
 *!			Draw Transparently Shaded Polygon
 ***************************************************************************/
void	draw_poly_trans( short int *iptr )
{
	int colour;

	if ( xgen_x(iptr+1) )
	{
		colour = *iptr;
		__asm
		{
			mov eax,xgen_ymin
			mov edx,xgen_ymax
			mov ebx,colour
			call transA
		}
	}
}

/****************************************************************************
 *!				Draw Gouraud Shaded Polygon
 ***************************************************************************/
void	draw_poly_gouraud( short int *iptr )
{
	int colour;

	if ( xgen_xg(iptr+1) )
	{
		colour = *iptr;
		__asm
		{
			mov eax,xgen_ymin
			mov edx,xgen_ymax
			mov ebx,colour
			call gourA
		}
	}
}

/*****************************************************************************
 *				Draw Gouraud Shaded Texture Map..
 ****************************************************************************/
void	draw_poly_gtmap( short int *iptr )
{
	char *texture;

	if ( xgen_xguv(iptr+1) )
	{
		texture = texture_page_ptrs[*(iptr)];
		__asm
		{
			mov eax,xgen_ymin
			mov edx,xgen_ymax
			mov ebx,texture
			call gtmapA
		}
	}
}

/*****************************************************************************
 *			Draw Gouraud Shaded Windowed Texture Map..
 ****************************************************************************/
void	draw_poly_wgtmap( short int *iptr )
{
	char *texture;

	if ( xgen_xguv(iptr+1) )
	{
		texture = texture_page_ptrs[*(iptr)];
		__asm
		{
			mov eax,xgen_ymin
			mov edx,xgen_ymax
			mov ebx,texture
			call wgtmapA
		}
	}
}


/****************************************************************************
 *					Generate Polygon X coords
 ***************************************************************************/

#pragma pack(push,foo1,1)

typedef struct x_info {
	short int	xs;
	short int	ys;
} X_INFO;

#pragma pack(pop,foo1)

int		xgen_x( short int *iptr )
{
	X_INFO			*ptr1,*ptr2;
	XBUF_X			*xptr;
	int				x,x_add;
	int				y1,y2,ydif;
	int				min_y,max_y;
	int				numcoords;


	numcoords = (int)*(iptr++);							// Get Number Coords
	ptr1 = (X_INFO *)iptr;

	ptr2 = ptr1;
	ptr1 += numcoords-1;
	min_y = max_y = ptr1->ys;
	for ( ;numcoords>0;numcoords--,ptr1=ptr2,ptr2++ ) 	// For All Side in Clockwise manner
	{
		y1 = ptr1->ys;
		y2 = ptr2->ys;
		if ( y1<y2 )                        			// We are on RHS of Polygon
		{
			if ( y1<min_y )
				min_y = y1;
			ydif = y2-y1;
			x = ptr1->xs;
			x_add = ((ptr2->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0ffff;
			xptr = ((XBUF_X *)xbuffer) + y1;
			for ( ;y1<y2;y1++,xptr++ )
			{
				x += x_add;
				xptr->Xright = x;
			}
		}
		else if ( y2<y1 )								// We are on LHS of Polygon
		{
			if ( y1>max_y )
				max_y = y1;
			ydif = y1-y2;
			x = ptr2->xs;
			x_add = ((ptr1->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0001;
			xptr = ((XBUF_X *)xbuffer) + y2;
			for ( ;y2<y1;y2++,xptr++ )
			{
				x += x_add;
				xptr->Xleft = x;
			}
		}
	}
	if ( min_y==max_y )
		return(0);
	xgen_ymin = min_y;								// Insert Global Y bounds..
	xgen_ymax = max_y;
	return(1);
}

/****************************************************************************
 *			Generate Polygon X,G coords
 ***************************************************************************/
typedef struct xg_info {
	short int	xs;
	short int	ys;
	short int	shade;
} XG_INFO;

int		xgen_xg( short int *iptr )
{
	XG_INFO			*ptr1,*ptr2;
	XBUF_XG			*xptr;
	int				g,g_add;
	int				x,x_add;
	int				y1,y2,ydif;
	int				min_y,max_y;
	int				numcoords;


	numcoords = (int)*(iptr++);							// Get Number Coords
	ptr1 = (XG_INFO *)iptr;

	ptr2 = ptr1;
	ptr1 += numcoords-1;
	min_y = max_y = ptr1->ys;
	for ( ;numcoords>0;numcoords--,ptr1=ptr2,ptr2++ ) 	// For All Side in Clockwise manner
	{
		y1 = ptr1->ys;
		y2 = ptr2->ys;
		if ( y1<y2 )                        			// We are on RHS of Polygon
		{
			if ( y1<min_y )
				min_y = y1;
			ydif = y2-y1;
			x = ptr1->xs;
			x_add = ((ptr2->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0ffff;
			g = ptr1->shade<<8;                         // Get Lerps and Deltas
			g_add = ((ptr2->shade<<8) - g)/ydif;
			xptr = ((XBUF_XG *)xbuffer) + y1;
			for ( ;y1<y2;y1++,xptr++ )
			{
				x += x_add;
				g += g_add;
				xptr->Xright = x;
				xptr->Gright = g;
			}
		}
		else if ( y2<y1 )								// We are on LHS of Polygon
		{
			if ( y1>max_y )
				max_y = y1;
			ydif = y1-y2;
			x = ptr2->xs;
			x_add = ((ptr1->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0001;
			g = ptr2->shade<<8;                     // Get Lerps and Deltas
			g_add = ((ptr1->shade<<8) - g)/ydif;
			xptr = ((XBUF_XG *)xbuffer) + y2;
			for ( ;y2<y1;y2++,xptr++ )
			{
				x += x_add;
				g += g_add;
				xptr->Xleft = x;
				xptr->Gleft = g;
			}
		}
	}
	if ( min_y==max_y )
		return(0);
	xgen_ymin = min_y;								// Insert Global Y bounds..
	xgen_ymax = max_y;
	return(1);
}

/****************************************************************************
 *			Generate Polygon X,G,U,V coords
 ***************************************************************************/
typedef struct uvg_info {
	short int	xs;
	short int	ys;
	short int	shade;
	unsigned short int	u;
	unsigned short int	v;
} UVG_INFO;

int		xgen_xguv( short int *iptr )
{
	UVG_INFO		*ptr1,*ptr2;
	XBUF_XGUV		*xptr;
	int				g,g_add;
	int				x,x_add;
	int				u,u_add;
	int				v,v_add;
	int				y1,y2,ydif;
	int				min_y,max_y;
	int				numcoords;


	numcoords = (int)*(iptr++);							// Get Number Coords
	ptr1 = (UVG_INFO *)iptr;

	ptr2 = ptr1;
	ptr1 += numcoords-1;
	min_y = max_y = ptr1->ys;
	for ( ;numcoords>0;numcoords--,ptr1=ptr2,ptr2++ ) 	// For All Side in Clockwise manner
	{
		y1 = ptr1->ys;
		y2 = ptr2->ys;
		if ( y1<y2 )                        			// We are on RHS of Polygon
		{
			if ( y1<min_y )
				min_y = y1;

			ydif = y2-y1;

			x = ptr1->xs;
			x_add = ((ptr2->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0ffff;

			g = ptr1->shade<<8;                         // Get Lerps and Deltas
			g_add = ((ptr2->shade<<8) - g)/ydif;
			u = ptr1->u<<8;
			u_add = ((ptr2->u<<8)-u)/ydif;
			v = ptr1->v<<8;
			v_add = ((ptr2->v<<8)-v)/ydif;

			xptr = ((XBUF_XGUV *)xbuffer) + y1;
			for ( ;y1<y2;y1++,xptr++ )
			{
				x += x_add;
				g += g_add;
				u += u_add;
				v += v_add;
				xptr->Xright = x;
				xptr->Gright = g;
				xptr->Uright = u;
				xptr->Vright = v;
			}
		}
		else if ( y2<y1 )								// We are on LHS of Polygon
		{
			if ( y1>max_y )
				max_y = y1;

			ydif = y1-y2;

			x = ptr2->xs;
			x_add = ((ptr1->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0001;

			g = ptr2->shade<<8;                         // Get Lerps and Deltas
			g_add = ((ptr1->shade<<8) - g)/ydif;
			u = ptr2->u<<8;
			u_add = ((ptr1->u<<8)-u)/ydif;
			v = ptr2->v<<8;
			v_add = ((ptr1->v<<8)-v)/ydif;

			xptr = ((XBUF_XGUV *)xbuffer) + y2;
			for ( ;y2<y1;y2++,xptr++ )
			{
				x += x_add;
				g += g_add;
				u += u_add;
				v += v_add;
				xptr->Xleft = x;
				xptr->Gleft = g;
				xptr->Uleft = u;
				xptr->Vleft = v;
			}
		}
	}
	if ( min_y==max_y )
		return(0);
	xgen_ymin = min_y;				// Insert Global Y bounds..
	xgen_ymax = max_y;
	return(1);
}

/****************************************************************************
 ****************************************************************************
 *				Generate Polygon X,G,U,V coords
 ****************************************************************************
 ***************************************************************************/
#pragma pack(push,foo,1)

typedef struct uvgp_info {
	short int	xs;
	short int	ys;
	short int	shade;
	float	ooz;
	float	uoz;
	float	voz;
} UVGP_INFO;

#pragma pack(pop,foo)

int		xgen_xguvpersp_fp( short int *iptr )
{
	UVGP_INFO		*ptr1,*ptr2;
	XBUF_PERSP_FP	*xptr;
	int				g,g_add;
	int				x,x_add;
	float scale;
	float			uoz,uoz_add;
	float			voz,voz_add;
	float			ooz,ooz_add;
	int				y1,y2,ydif;
	int				min_y,max_y;
	int				numcoords;

	numcoords = (int)*(iptr++);							// Get Number Coords
	ptr2 = (UVGP_INFO *)iptr;
	ptr1 = ptr2 + numcoords-1;
	min_y = max_y = ptr1->ys;
	for ( ;numcoords>0;numcoords--,ptr1=ptr2,ptr2++ ) 	// For All Side in Clockwise manner
	{
		y1 = ptr1->ys;
		y2 = ptr2->ys;
		if ( y1<y2 )                        			// We are on RHS of Polygon
		{
			if ( y1<min_y )
				min_y = y1;

			ydif = y2-y1;

			x = ptr1->xs;
			x_add = ((ptr2->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0ffff;

			g = ptr1->shade<<8;                         // Get Lerps and Deltas
			scale = 1.0f/ydif;
			g_add = ((ptr2->shade<<8) - g)/ydif;
			uoz = ptr1->uoz;
			uoz_add = (ptr2->uoz - uoz) * scale;
			voz = ptr1->voz;
			voz_add = (ptr2->voz - voz) * scale;
			ooz = ptr1->ooz;
			ooz_add = (ptr2->ooz - ooz) * scale;

			xptr = ((XBUF_PERSP_FP *)xbuffer) + y1;
			for ( ;y1<y2;y1++,xptr++ )
			{
				x += x_add;
				g += g_add;
				uoz += uoz_add;
				voz += voz_add;
				ooz += ooz_add;
				xptr->Xright = x;
				xptr->Gright = g;
				xptr->UOZright = uoz;
				xptr->VOZright = voz;
				xptr->OOZright = ooz;
			}
		}
		else if ( y2<y1 )								// We are on LHS of Polygon
		{
			if ( y1>max_y )
				max_y = y1;

			ydif = y1-y2;

			x = ptr2->xs;
			x_add = ((ptr1->xs-x)<<16)/ydif;
			x = (x<<16) + 0x0001;

			g = ptr2->shade<<8;                         // Get Lerps and Deltas
			scale = 1.0f/ydif;
			g_add = ((ptr1->shade<<8) - g)/ydif;
			uoz = ptr2->uoz;
			uoz_add = (ptr1->uoz - uoz) * scale;
			voz = ptr2->voz;
			voz_add = (ptr1->voz - voz) * scale;
			ooz = ptr2->ooz;
			ooz_add = (ptr1->ooz - ooz) * scale;

			xptr = ((XBUF_PERSP_FP *)xbuffer) + y2;
			for ( ;y2<y1;y2++,xptr++ )
			{
				x += x_add;
				g += g_add;
				uoz += uoz_add;
				voz += voz_add;
				ooz += ooz_add;
				xptr->Xleft = x;
				xptr->Gleft = g;
				xptr->UOZleft = uoz;
				xptr->VOZleft = voz;
				xptr->OOZleft = ooz;
			}
		}
	}
	if ( min_y==max_y )
		return(0);
	xgen_ymin = min_y;				// Insert Global Y bounds..
	xgen_ymax = max_y;
	return(1);
}

/******************************************************************************
 *						   TextureMapper
 * 		 		Perspective Correction every 32 Pixels
 *****************************************************************************/
void	gtmap_persp32_fp( int ymin, int ymax, unsigned char *tptr )
{
	/* This version does 32 pixels at a time linearly, and adaptively double pixels for each 32 pixel span.
		Should only be used in 640x480 though */
	XBUF_PERSP_FP	*dataptr;
	unsigned char	*lineptr,*pixptr;
	float				ooz,uoz,voz,scale;
	float				oozadd32,uozadd32,vozadd32;
	int				start_g,add_g,add_g2;
	int				end_u,end_v;
	int				start_u,start_v;
	int				add_u,add_v;
	int				start_x,end_x;
	int				x;

	dataptr = ((XBUF_PERSP_FP *)xbuffer)+ymin;
	lineptr = (unsigned char*)phd_winptr + ( ymin*phd_scrwidth );
	for (ymax-=ymin; ymax>0; ymax--, dataptr++, lineptr+=phd_scrwidth)
	{
		start_x = dataptr->Xleft >> 16;
		end_x = dataptr->Xright >> 16;
		x = end_x - start_x;
		if (x <= 0)
			continue;
		
		if ( x>32 )
		{
			start_g = dataptr->Gleft;
			add_g = (dataptr->Gright-start_g) / x;
			add_g2 = add_g<<1;

			scale = 32.0f/x;
			uoz = dataptr->UOZleft;
			uozadd32 = (dataptr->UOZright-uoz) * scale;
			voz = dataptr->VOZleft;
			vozadd32 = (dataptr->VOZright-voz) * scale;
			ooz = dataptr->OOZleft;
			oozadd32 = (dataptr->OOZright-ooz) * scale;

			scale = 256.0f/ooz;
			end_u = (int)(uoz * scale);
			end_v = (int)(voz * scale);

			pixptr = lineptr + start_x;
		
			ooz += oozadd32;
			scale = 256.0f/ooz;
			
			for ( ; x>=32; x-=32)
			{
				start_u = end_u;
				uoz += uozadd32;
				end_u = (int)(uoz*scale);
				add_u = (end_u-start_u)>>5;

				start_v = end_v;
				voz += vozadd32;
				end_v = (int)(voz*scale);
				add_v = (end_v-start_v)>>5;

				// FP divide in parallel with the pixel drawing
				ooz += oozadd32;
				scale = 256.0f/ooz;

				if ((ABS(add_u) + ABS(add_v)) & SINGLE_MASK)
				{
					// Single pixel it
					*(pixptr) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+1) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+2) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+3) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					*(pixptr+4) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+5) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+6) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+7) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					*(pixptr+8) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+9) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+10) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+11) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					*(pixptr+12) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+13) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+14) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+15) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_g += add_g;
					start_u += add_u;
					start_v += add_v;

					*(pixptr+16) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+17) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+18) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+19) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					*(pixptr+20) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+21) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+22) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+23) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					*(pixptr+24) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+25) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+26) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+27) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					*(pixptr+28) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+29) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+30) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					*(pixptr+31) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_g += add_g;
				}
				else
				{
					// Double pixel it
					add_u <<= 1;
					add_v <<= 1;

					*(pixptr) = *(pixptr+1) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					*(pixptr+2) = *(pixptr+3) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					*(pixptr+4) = *(pixptr+5) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					*(pixptr+6) = *(pixptr+7) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					*(pixptr+8) = *(pixptr+9) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					*(pixptr+10) = *(pixptr+11) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					*(pixptr+12) = *(pixptr+13) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					*(pixptr+14) = *(pixptr+15) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					*(pixptr+16) = *(pixptr+17) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					*(pixptr+18) = *(pixptr+19) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					*(pixptr+20) = *(pixptr+21) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					*(pixptr+22) = *(pixptr+23) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					*(pixptr+24) = *(pixptr+25) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					*(pixptr+26) = *(pixptr+27) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					*(pixptr+28) = *(pixptr+29) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					*(pixptr+30) = *(pixptr+31) = depthq_table[(start_g>>16)][ *(tptr + ((start_v>>16)<<8) + (start_u>>16) ) ];
					start_g += add_g2;
				}

				pixptr += 32;
			}

			if (!x)
				continue;

			// Do the odd bit at the end
			scale = 256.0f/dataptr->OOZright;

			start_u = end_u;
			end_u = (int)(dataptr->UOZright*scale);
			add_u = (end_u-start_u)/x;

			start_v = end_v;
			end_v = (int)(dataptr->VOZright*scale);
			add_v = (end_v-start_v)/x;
		}
		else
		{
			/* Special case for spans of 32 pixels or less */
			start_g = dataptr->Gleft;
			add_g = (dataptr->Gright-start_g) / x;

			scale = 256.0f/dataptr->OOZleft;
			start_u = (int)(dataptr->UOZleft * scale);
			start_v = (int)(dataptr->VOZleft * scale);

			scale = 256.0f/dataptr->OOZright;
			end_u = (int)(dataptr->UOZright * scale);
			end_v = (int)(dataptr->VOZright * scale);

			add_u = (end_u-start_u)/x;
			add_v = (end_v-start_v)/x;

			pixptr = lineptr + start_x;
		}

		/* Do bits of span less than 32 pixels */
		if ((ABS(add_u) + ABS(add_v)) & SINGLE_MASK)
		{
			/* Single pixel it */
			for ( ; x>1; x-=2, pixptr+=2)
			{
				pixptr[0] = depthq_table[start_g>>16][ *(tptr + ((start_v>>16)<<8) + (start_u>>16)) ];
				start_u += add_u;
				start_v += add_v;
				start_g += add_g;
				pixptr[1] = depthq_table[start_g>>16][ *(tptr + ((start_v>>16)<<8) + (start_u>>16)) ];
				start_u += add_u;
				start_v += add_v;
				start_g += add_g;
			}
		}
		else
		{
			/* Double pixel it */
			add_u <<= 1;
			add_v <<= 1;
			add_g2 = add_g<<1;

			for ( ; x>1; x-=2, pixptr+=2)
			{
				pixptr[0] = pixptr[1] = depthq_table[start_g>>16][ *(tptr + ((start_v>>16)<<8) + (start_u>>16)) ];
				start_u += add_u;
				start_v += add_v;
				start_g += add_g2;
			}
		}

		
		if (x)
			pixptr[0] = depthq_table[start_g>>16][ *(tptr + ((start_v>>16)<<8) + (start_u>>16)) ];
		
	}
}

/******************************************************************************
 *					  Transparent TextureMapper
 * 		 		Perspective Correction every 32 Pixels
 *****************************************************************************/
void	wgtmap_persp16_fp( int ymin, int ymax, char *tptr )
{
	/* This version does 32 pixels at a time linearly, and adaptively double pixels for each 32 pixel span.
		Should only be used in 640x480 though */
	XBUF_PERSP_FP	*dataptr;
	unsigned char	*lineptr,*pixptr,texel;
	float				ooz,uoz,voz,scale;
	float				oozadd32,uozadd32,vozadd32;
	int				start_g,add_g,add_g2;
	int				end_u,end_v;
	int				start_u,start_v;
	int				add_u,add_v;
	int				start_x,end_x;
	int				x;

	dataptr = ((XBUF_PERSP_FP *)xbuffer)+ymin;
	lineptr = (unsigned char*)phd_winptr + ( ymin*phd_scrwidth );
	for (ymax-=ymin; ymax>0; ymax--, dataptr++, lineptr+=phd_scrwidth)
	{
		start_x = dataptr->Xleft >> 16;
		end_x = dataptr->Xright >> 16;
		x = end_x - start_x;
		if ( x>32 )
		{
			start_g = dataptr->Gleft;
			add_g = (dataptr->Gright-start_g) / x;
			add_g2 = add_g<<1;

			scale = 32.0f/x;
			uoz = dataptr->UOZleft;
			uozadd32 = (dataptr->UOZright-uoz) * scale;
			voz = dataptr->VOZleft;
			vozadd32 = (dataptr->VOZright-voz) * scale;
			ooz = dataptr->OOZleft;
			oozadd32 = (dataptr->OOZright-ooz) * scale;

			scale = 256.0f/ooz;
			end_u = (int)(uoz * scale);
			end_v = (int)(voz * scale);

			pixptr = lineptr + start_x;
		
			ooz += oozadd32;
			scale = 256.0f/ooz;
			
			for ( ; x>=32; x-=32)
			{
				start_u = end_u;
				uoz += uozadd32;
				end_u = (int)(uoz*scale);
				add_u = (end_u-start_u)>>5;

				start_v = end_v;
				voz += vozadd32;
				end_v = (int)(voz*scale);
				add_v = (end_v-start_v)>>5;

				if ((ABS(add_u) + ABS(add_v)) & SINGLE_MASK)
				{
					// FP divide in parallel with the pixel drawing
					ooz += oozadd32;
					scale = 256.0f/ooz;

					// Single pixel it
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+1) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+2) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+3) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+4) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+5) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+6) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+7) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+8) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+9) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+10) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+11) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+12) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+13) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+14) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+15) = depthq_table[(start_g>>16)][texel];
					start_g += add_g;
					start_u += add_u;
					start_v += add_v;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+16) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+17) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+18) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+19) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+20) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+21) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+22) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+23) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+24) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+25) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+26) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+27) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+28) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+29) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+30) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+31) = depthq_table[(start_g>>16)][texel];
					start_g += add_g;
				}
				else
				{
					ooz += oozadd32;
					scale = 256.0f/ooz;

					// Double pixel it
					add_u <<= 1;
					add_v <<= 1;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr) = *(pixptr+1) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+2) = *(pixptr+3) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+4) = *(pixptr+5) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+6) = *(pixptr+7) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+8) = *(pixptr+9) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+10) = *(pixptr+11) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+12) = *(pixptr+13) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+14) = *(pixptr+15) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+16) = *(pixptr+17) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+18) = *(pixptr+19) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+20) = *(pixptr+21) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+22) = *(pixptr+23) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+24) = *(pixptr+25) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+26) = *(pixptr+27) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;

					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+28) = *(pixptr+29) = depthq_table[(start_g>>16)][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						*(pixptr+30) = *(pixptr+31) = depthq_table[(start_g>>16)][texel];
					start_g += add_g2;
				}

				pixptr += 32;
			}

			// Do the odd bit at the end
			if (x)
			{
				scale = 256.0f/dataptr->OOZright;

				start_u = end_u;
				end_u = (int)(dataptr->UOZright*scale);
				add_u = (end_u-start_u)/x;

				start_v = end_v;
				end_v = (int)(dataptr->VOZright*scale);
				add_v = (end_v-start_v)/x;

				if ((ABS(add_u) + ABS(add_v)) & SINGLE_MASK)
				{
					// Single pixel it
					for ( ; x>1; x-=2, pixptr+=2)
					{
						if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
							pixptr[0] = depthq_table[start_g>>16][texel];
						start_u += add_u;
						start_v += add_v;
						start_g += add_g;
						if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
							pixptr[1] = depthq_table[start_g>>16][texel];
						start_u += add_u;
						start_v += add_v;
						start_g += add_g;
					}
				}
				else
				{
					// Double pixel it
					add_u <<= 1;
					add_v <<= 1;

					for ( ; x>1; x-=2, pixptr+=2)
					{
						if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
							pixptr[0] = pixptr[1] = depthq_table[start_g>>16][texel];
						start_u += add_u;
						start_v += add_v;
						start_g += add_g2;
					}
				}

				if (x && (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
					pixptr[0] = depthq_table[start_g>>16][texel];
			}
		}
		else if ( x>0 )
		{
			/* Special case for spans of 32 pixels or less */
			start_g = dataptr->Gleft;
			add_g = (dataptr->Gright-start_g) / x;

			scale = 256.0f/dataptr->OOZleft;
			start_u = (int)(dataptr->UOZleft * scale);
			start_v = (int)(dataptr->VOZleft * scale);

			scale = 256.0f/dataptr->OOZright;
			end_u = (int)(dataptr->UOZright * scale);
			end_v = (int)(dataptr->VOZright * scale);

			add_u = (end_u-start_u)/x;
			add_v = (end_v-start_v)/x;

			pixptr = lineptr + start_x;
			if ((ABS(add_u) + ABS(add_v)) & SINGLE_MASK)
			{
				/* Single pixel it */
				for ( ; x>1; x-=2, pixptr+=2)
				{
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						pixptr[0] = depthq_table[start_g>>16][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						pixptr[1] = depthq_table[start_g>>16][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g;
				}
			}
			else
			{
				/* Double pixel it */
				add_u <<= 1;
				add_v <<= 1;
				add_g2 = add_g<<1;

				for ( ; x>1; x-=2, pixptr+=2)
				{
					if ( (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
						pixptr[0] = pixptr[1] = depthq_table[start_g>>16][texel];
					start_u += add_u;
					start_v += add_v;
					start_g += add_g2;
				}
			}

			if (x && (texel=*(tptr + ((start_v>>16)<<8) + (start_u>>16) )) )
				pixptr[0] = depthq_table[start_g>>16][texel];
		}
	}
}


/*****************************************************************************
 *				Draw Gouraud Shaded Texture Map Perspective-Correct
 ****************************************************************************/
void	draw_poly_gtmap_persp( short int *iptr )
{
	if ( xgen_xguvpersp_fp(iptr+1) )
		gtmap_persp32_fp( xgen_ymin, xgen_ymax, (unsigned char*)texture_page_ptrs[*(iptr)] );
}

void	draw_poly_wgtmap_persp( short int *iptr )
{
	if ( xgen_xguvpersp_fp(iptr+1) )
		wgtmap_persp16_fp( xgen_ymin, xgen_ymax, texture_page_ptrs[*(iptr)] );
}



//------------------------------------------------------------------
// MY CODE !!!!!!!!!!!!!!!!!!!!!!!!!
//------------------------------------------------------------------


/*** pointers to internal renderer's poly draw routines ***/
void	(*poly_draw_routines[])(short int*)=
	{
	draw_poly_gtmap,			 // Gouraud-Shaded Texture-mapped polygon
	draw_poly_wgtmap,			 // Gouraud-Shaded Texture-mapped Transparent polygon
	draw_poly_gtmap_persp, 		 // Perspective Gouraud-Shaded Texture-mapped polygon
	draw_poly_wgtmap_persp,		 // Perspective	Gouraud-Shaded Texture-mapped Transparent polygon
	draw_poly_line,				 // Line draw
    draw_poly_flat,         	 // flat-shaded polygon
	draw_poly_gouraud,      	 // gouraud-shaded polygon
	draw_poly_trans,             // Transparent Shadow polygon
	draw_scaled_spriteC,         // Scaled Sprite Draw
	};


/******************************************************************************
*   	   Sort The Polygon Draw List In Back To Front Order
*****************************************************************************/

//void	phd_SortPolyList( int number,int buffer[][10])
void	phd_SortPolyList( int number,int buffer[][2])
{
	int	i;
	if (number)
	{
		for (i=0; i<number;i++)         // eliminate polygon flicker
			buffer[i][1] += i;
		
		do_quickysorty( 0, number-1, buffer);
	}
}

/******************************************************************************
*   	   Quick Sort Routine  ( Pisses on Shell sort !!! )
*								 but uses recursion!!!!
*****************************************************************************/

void	do_quickysorty( int left, int right, int buffer[][2] )
{
	int		i,j;
	int		compare,swap;
	
	i = left;
	j = right;
	compare = buffer[(left+right)/2][1]; 		/* get middle value*/

	do
	{
		while (buffer[i][1]>compare && i<right)
			++i;  									/* was <x*/
		while ( compare>buffer[j][1] && j>left )
			--j;  									/* was x<*/
		if ( i<=j ) 									/* was ( i<=j )*/
		{
			swap = buffer[i][1];
			buffer[i][1] = buffer[j][1];	/* swap elements*/
			buffer[j][1] = swap;

			swap = buffer[i][0];
			buffer[i][0] = buffer[j][0];
			buffer[j][0] = swap;
						
			i++;
			j--;
		}
	}
	while (i<=j);
		
	if(left<j)
		do_quickysorty(left,j,buffer);
	if(i<right)
		do_quickysorty(i,right,buffer);
}


	/*****************************************************************************
	*			Print Polygon output list
****************************************************************************/
void	phd_PrintPolyList(void* ptr)
	{
	/* Draw onto render surface */
	int		i;
	int	*sptr;
	short int	*iptr;
	short int	routine;
	
	phd_winptr = (char *)ptr;
	
	sptr = (int *)sort3d_buffer;
	for ( i=surfacenum; i>0; i-- )
		{
		iptr = (short int *)( *sptr );
		routine = *(iptr++);
		(*poly_draw_routines[routine])( iptr );
		sptr += 2;
		}
	}

