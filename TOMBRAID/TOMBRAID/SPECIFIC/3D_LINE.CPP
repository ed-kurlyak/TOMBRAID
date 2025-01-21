//#include "3Dglodef.h"

#include "3D_LINE.H"
#include "vars.h"
#include "types.h"
#include "winmain.h"



/******************************************************************************
 *					DRAW LINE using coords and Color
 *****************************************************************************/
void	draw_line( int x1, int y1, int x2, int y2, char color	)
{
	short int	buf[5];

	buf[0] = (short int)x1;
	buf[1] = (short int)y1;
	buf[2] = (short int)x2;
	buf[3] = (short int)y2;
	buf[4] = (short int)color;
	draw_poly_line( buf );
}

/******************************************************************************
 *					DRAW LINE  using Draw.list structure
 *****************************************************************************/
void	draw_poly_line( short int *iptr )
{
	int	i,frak,fraksum,xdif,ydif,xadd,yadd;
	int	x1,y1,x2,y2;
	char	lcolor;
	char	*drawptr;


	x1 = (int)( *(iptr+0) );
	y1 = (int)( *(iptr+1) );
	x2 = (int)( *(iptr+2) );
	y2 = (int)( *(iptr+3) );
	lcolor = (char)( *(iptr+4) );
	if ( x2 < x1 )
	{											/*swap end coords so x1<x2*/
		i = x2;
		x2 = x1;
		x1 = i;
		i = y2;
		y2 = y1;
		y1 = i;
	}
	if ( x2<0 || x1>phd_winxmax )			 	/* only draw n clip if can see*/
		return;
	if ( x1<0 )
	{							 				/* is Left clipping required ?*/
		y1 -= ((y2-y1)*x1)/(x2-x1);
		x1=0;
  	}
  	if ( x2>phd_winxmax )
	{					 						/* is Right clipping required ?*/
  		y2=y1+((y2-y1)*(phd_winxmax-x1))/(x2-x1);
  		x2=phd_winxmax;
  	}
 	if ( y2 < y1 )
	{						 					/* swap end coords so y1<y2*/
  		i = x2;
  		x2 = x1;
  		x1 = i;
  		i = y2;
  		y2 = y1;
  		y1 = i;
  	}
 	if ( y2<0 || y1 >phd_winymax )				/*only cont if line on screen*/
		return;
  	if (y1<0)
	{											/*is Top clipping required ?*/
  		x1-=((x2-x1)*y1)/(y2-y1);
  		y1=0;
  	}
 	if (y2>phd_winymax)
	{											/*is bottom Clipping required ?*/
 		x2=x1+((x2-x1)*(phd_winymax-y1))/(y2-y1);
 		y2=phd_winymax;
 	}
	drawptr = phd_winptr+(phd_scrwidth*y1)+x1;
	xdif = x2-x1;
	ydif = y2-y1;
	if ( xdif||ydif ) 			   				/*if x1<>x2 OR y1<>y2 then draw line*/
	{
		if (xdif>=0)							/*else plot single pixel*/
			xadd = 1;
		else
		{
			xadd = -1;
			xdif = -xdif;
		}
		if ( ydif>=0 )
			yadd = phd_scrwidth;
		else
		{
			yadd = -phd_scrwidth;
			ydif = -ydif;
		}
		fraksum=0;
		if (xdif++ < ydif++)
		{
	  		frak = ((xdif<<16)/ydif);			/* draw pixels vertically*/
	  		for ( i=0; i<ydif; i++)
			{
	  			*(drawptr) = lcolor; 			/*set colour*/
	  			drawptr += yadd;
	  			fraksum += frak;  				/* add on fraction*/
	  			if (fraksum>=65536)
				{								/* if overflow*/
	  				drawptr += xadd;   			/* inc/dec x position*/
	  				fraksum -= 65536;
	  			}
	  		}
	  	}
	  	else
		{
	  		frak=((ydif<<16)/xdif);				/* draw pixels horizontally*/
	  		for(i=0; i<xdif; i++)
			{
	  			*(drawptr) = lcolor;
	  			drawptr += xadd;
	  			fraksum += frak;
	  			if (fraksum>=65536)
				{
	  				drawptr += yadd;
	  				fraksum -= 65536;
	  			}
	  		}
	  	}
	}
  	else *drawptr = lcolor;
}

