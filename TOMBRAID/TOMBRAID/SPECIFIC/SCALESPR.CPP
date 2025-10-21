//#include "..\specific\standard.h"
//#include "..\specific\global.h"
//#include "3Dglodef.h"
//#include "..\specific\litesrc.h"

#include "SCALESPR.H"
#include "types.h"
#include "vars.h"
#include "winmain.h"

/*

void insert_sprite(int nZDepth,int nX1,int nY1,int nX2,int nY2,int nSprite,int
nShade)
{
		sort3dptr[0]=(int)info3dptr;
		sort3dptr[1]=nZDepth;
		sort3dptr+=2;
		info3dptr[0]=DRAW_SCALED_SPRITE;
		info3dptr[1]=(short int)nX1;
		info3dptr[2]=(short int)nY1;
		info3dptr[3]=(short int)nX2;
		info3dptr[4]=(short int)nY2;
		info3dptr[5]=nSprite;
		info3dptr[6]=nShade;
		info3dptr+=7;
		surfacenum++;
}



void S_DrawSprite(unsigned int dwFlags,int nX,int nY,int nZ,short int
nSprite,short int nShade,short int nScale)
		{
		int xv,yv,zv,zp;
		int x1,y1,x2,y2;
		PHD_SPRITE* pSpriteInfo;
		



		if (dwFlags&SPRITE_ABS)
				{
				nX-=w2v_matrix[M03];
				nY-=w2v_matrix[M13];
				nZ-=w2v_matrix[M23];
				if (nX<-phd_viewdist || nX>phd_viewdist ||
						nY<-phd_viewdist || nY>phd_viewdist ||
						nZ<-phd_viewdist || nZ>phd_viewdist)
								return;
				zv=w2v_matrix[M20]*nX+w2v_matrix[M21]*nY+w2v_matrix[M22]*nZ;
				if (zv<phd_znear || zv>=phd_zfar)
						return;
				xv=w2v_matrix[M00]*nX+w2v_matrix[M01]*nY+w2v_matrix[M02]*nZ;
				yv=w2v_matrix[M10]*nX+w2v_matrix[M11]*nY+w2v_matrix[M12]*nZ;
				}
		else
				{
				if ((nX|nY|nZ)==0)
						{
						zv=phd_mxptr[M23];
						if (zv<phd_znear || zv>phd_zfar)
								return;
						xv=phd_mxptr[M03];
						yv=phd_mxptr[M13];
						}
				else
						{
						zv=phd_mxptr[M20]*nX+phd_mxptr[M21]*nY+phd_mxptr[M22]*nZ+phd_mxptr[M23];
						if (zv<phd_znear || zv>phd_zfar)
								return;
						xv=phd_mxptr[M00]*nX+phd_mxptr[M01]*nY+phd_mxptr[M02]*nZ+phd_mxptr[M03];
						yv=phd_mxptr[M10]*nX+phd_mxptr[M11]*nY+phd_mxptr[M12]*nZ+phd_mxptr[M13];
						}
				}
		zp=zv/phd_persp;

		pSpriteInfo=g_PhdSpriteInfo+nSprite;
		x1=pSpriteInfo->x1;
		y1=pSpriteInfo->y1;
		x2=pSpriteInfo->x2;
		y2=pSpriteInfo->y2;
		if (dwFlags&SPRITE_SCALE)
				{
				x1=(x1*nScale)<<(W2V_SHIFT-8);
				y1=(y1*nScale)<<(W2V_SHIFT-8);
				x2=(x2*nScale)<<(W2V_SHIFT-8);
				y2=(y2*nScale)<<(W2V_SHIFT-8);
				}
		else
				{
				x1<<=W2V_SHIFT;
				y1<<=W2V_SHIFT;
				x2<<=W2V_SHIFT;
				y2<<=W2V_SHIFT;
				}
		x1=(int)((xv+x1)/zp+phd_centerx); if (x1>=g_PhdWinwidth) return;
		y1=(int)((yv+y1)/zp+phd_centery); if (y1>=phd_winheight) return;
		x2=(int)((xv+x2)/zp+phd_centerx); if (x2<0) return;
		y2=(int)((yv+y2)/zp+phd_centery); if (y2<0) return;

		if (dwFlags&SPRITE_SHADE)
				{
				if (zv>(DPQ_S))
// Allow for depth-cueing of Sprite TART<<W2V_SHIFT
						{
						nShade+=(zv>>W2V_SHIFT)-DPQ_START;
						if (nShade>0x1fff)
								return;
						}
				}
		else
				nShade=0x1000;


		if(dwFlags&0xffffff)
		{
				nShade = dwFlags&0xffffff;
				int r,g,b;
				r = (nShade&0xff)>>3;
				g = ((nShade>>8)&0xff)>>3;
				b = ((nShade>>16)&0xff)>>3;
				nShade = r<<10|g<<5|b;
				InsertSprite(zv,x1,y1,x2,y2,nSprite,nShade,-1,DRAW_TLV_GTA,0);
		}
		else
				InsertSprite(zv,x1,y1,x2,y2,nSprite,nShade,-1,DRAW_TLV_WGT,0);
		



}

*/

/*******************************************************************************
 *				Draw Pickup display....
 ******************************************************************************/

/*
void	S_DrawPickup(short int x, short int y, short int object_num)
{
//	MATRIX cmat;
		static short int yrot;

//	S_MemSet(&cmat, 0, sizeof(MATRIX));
//	SetColorMatrix(&cmat);

//	SetBackColor(128, 128, 128);

		smcr = 128;
		smcg = 128;
		smcb = 128;

		LightCol[M00] = 0xcf0;
		LightCol[M10] = 0x680;
		LightCol[M20] = 0x000;

		LightCol[M01] = 0xcf0;
		LightCol[M11] = 0xcf0;
		LightCol[M21] = 0xcf0;

		LightCol[M02] = 0x0;
		LightCol[M12] = 0x0;
		LightCol[M22] = 0xc00;

		LPos[0].x = 4096<<2;
		LPos[0].y = -4096<<2;
		LPos[0].z = 3072<<2;

		LPos[1].x = -4096<<2;
		LPos[1].y = -4096<<2;
		LPos[1].z = 3072<<2;

		LPos[2].x = 0<<2;
		LPos[2].y = 2048<<2;
		LPos[2].z = 3072<<2;

		phd_PushUnitMatrix();
		phd_mxptr[M03] = 512<<W2V_SHIFT;
		phd_mxptr[M13] = 240<<W2V_SHIFT;
		phd_mxptr[M23] = 1024<<W2V_SHIFT;
		phd_RotY(yrot);
		phd_PutPolygons(meshes[objects[object_num].mesh_index], 1);
		phd_PopMatrix();

		yrot += 512;
}

*/
/*
void	S_DrawPickup( int x, int y, int scale, short int sprnum, short int
shade, uint16 flags )
{
		// Draw sprite at specific screen location with scalable width and
height PHD_SPRITE *sptr; int x1, y1, x2, y2;

		sptr = &g_PhdSpriteInfo[sprnum];
		x1 = ((sptr->x1 * scale)>>16)+x;
		x2 = ((sptr->x2 * scale)>>16)+x;
		y1 = ((sptr->y1 * scale)>>16)+y;
		y2 = ((sptr->y2 * scale)>>16)+y;

		if (x2<0 || y2<0 || x1>=g_PhdWinwidth || y1>=phd_winheight)
				return;

		InsertSprite(200,x1,y1,x2,y2,sprnum,shade,-1,DRAW_TLV_WGT,0);
}
*/
/****************************************************************************
 *				Insert Scaled Sprite Off Room Data...
 ***************************************************************************/

/*
short int	*ins_room_sprite( short int *objptr, int num )
{
		PHD_VBUF		*vn;
		int			x1,y1,x2,y2,zv;
		PHD_SPRITE	*sptr;

		for ( ;num>0;num--,objptr+=2 )
		{
				vn = &vbuf[ *(objptr) ];
				if ( vn->clip<0 )					// If Z
Behind Us Ignore continue; zv =(int) vn->zv/phd_persp; sptr =
&g_PhdSpriteInfo[*(objptr+1)]; x1 = (int)(( vn->xv + (sptr->x1<<W2V_SHIFT) )/zv
+ phd_centerx); y1 = (int)(( vn->yv + (sptr->y1<<W2V_SHIFT) )/zv + phd_centery);
				x2 = (int)(( vn->xv + (sptr->x2<<W2V_SHIFT) )/zv + phd_centerx);
				y2 = (int)(( vn->yv + (sptr->y2<<W2V_SHIFT) )/zv + phd_centery);
				if ( x2<phd_left || y2<phd_top || x1>=phd_right ||
y1>=phd_bottom ) continue;
				InsertSprite(int(vn->zv),x1,y1,x2,y2,objptr[1],int(vn->g),-1,DRAW_TLV_WGT,0);
		}
		return(objptr);
}

void	S_DrawSpriteSize( int x, int y, int z, int w, int h, short int sprnum,
short int shade, uint16 flags )
{
		if (x+w<0 || y+h<0 || x>=g_PhdWinwidth || y>=phd_winheight)
				return;

		InsertSprite(z,x,y,x+w,y+h,sprnum,shade,-1,DRAW_TLV_WGT,0);
}



unsigned long TextLight[] = {
		0xffffff,		// normal
		0x00b0b0,		// 0x70a0a0,		// gold
		0xa0a0a0,		// grey
		0x6060ff,		// red
		0xff8080,		// blue
		0x4080c0,		// bronze
		0x64d1b6,
		0xc0ffc0,		// bronze
};
unsigned long TextDark[] = {
		0x808080,		// normal
		0x005050,		// 0x184040,		// gold
		0x181818,		// grey
		0x000018,		// red
		0x180000,		// blue
		0x001040,		// bronze
		0x1320b6,
		0xc0ffc0,		// bronze
};


void S_DrawScreenSprite2d( int sx, int sy, int z, int scaleH, int scaleV, short
int sprnum, short int shade, uint16 flags)
{
		// Draw sprite at specific screen location with scalable width and
height PHD_SPRITE *sptr; int x1, y1, x2, y2;

		sptr = &g_PhdSpriteInfo[sprnum];
		x1 = ((sptr->x1 * scaleH)>>16) + sx;
		y1 = ((sptr->y1 * scaleV)>>16) + sy;
		x2 = ((sptr->x2 * scaleH)>>16) + sx;
		y2 = ((sptr->y2 * scaleV)>>16) + sy;

		if (x2<0 || y2<0 || x1>=g_PhdWinwidth || y1>=phd_winheight)
				return;

		int r,g,b;
		int nShade1,nShade2;
		b = (TextLight[shade]>>(16+3))&0x1f;
		g = (TextLight[shade]>>(8+3))&0x1f;
		r = (TextLight[shade]>>(3))&0x1f;
				



		nShade1 = r<<10|g<<5|b;

		b = (TextDark[shade]>>(16+3))&0x1f;
		g = (TextDark[shade]>>(8+3))&0x1f;
		r = (TextDark[shade]>>(3))&0x1f;
		



		nShade2 = r<<10|g<<5|b;

		if(flags != 65535)
		{
				InsertSprite(phd_znear+(6<<3),x1,y1,x2,y2,sprnum,nShade1,nShade2,DRAW_TLV_WGT,4);
				InsertSprite(phd_znear+(6<<3),x1+2,y1+2,x2+2,y2+2,sprnum,0,0,DRAW_TLV_WGT,4);
		}
		else
				InsertSprite(phd_znear,x1,y1,x2,y2,sprnum,nShade1,nShade2,DRAW_TLV_WGT,0);

}






void S_DrawScreenSprite( int sx, int sy, int z, int scaleH, int scaleV, short
int sprnum, short int shade, uint16 flags )
{
		// Draw sprite at specific screen location with scalable width and
height PHD_SPRITE *sptr; int x1, y1, x2, y2;

		sptr = &g_PhdSpriteInfo[sprnum];
		x1 = (((sptr->x1>>3) * scaleH)>>16) + sx;
		x2 = (((sptr->x2>>3) * scaleH)>>16) + sx;
		y1 = (((sptr->y1>>3) * scaleV)>>16) + sy;
		y2 = (((sptr->y2>>3) * scaleV)>>16) + sy;

		if (x2<0 || y2<0 || x1>=g_PhdWinwidth || y1>=phd_winheight)
				return;

		InsertSprite(z<<3,x1,y1,x2,y2,sprnum,shade,-1,DRAW_TLV_WGT,0);
}




*/

/****************************************************************************
 *					Draw Scaled Sprite Please!!!
 * INPUT IS.
 *   0. X1 coord
 *   1. Y1 coord
 *   2. X2 coord
 *   3. Y2 coord
 *   4. Sprite Number
 *   5. Shade Of Sprite
 *
 ****************************************************************************/
void draw_scaled_spriteC(short int *inptr)
{
	int x1, y1, x2, y2;
	int xdif;
	int u, ustart, uadd;
	int vstart, vadd, pixadd;
	unsigned char *pixptr, *dqptr;
	unsigned char *ssptr, *slptr;
	unsigned char pix;
	PHD_SPRITE *sptr;

	x1 = (int)*(inptr);		// Get Top Left X
	y1 = (int)*(inptr + 1); // Get Top Left Y
	x2 = (int)*(inptr + 2); // Get Bottom Right X
	y2 = (int)*(inptr + 3); // Get Bottom Right Y

	if (x2 <= x1 || y2 <= y1 || x2 <= 0 || y2 <= 0 || x1 >= g_PhdWinxmax ||
		y1 >= g_PhdWinymax)
		return;

	dqptr = (unsigned char *)&depthq_table[(*(inptr + 5)) >> 8]
										  [0]; // *(inptr+5) = DepthQ*8
	sptr = &g_PhdSpriteInfo[*(inptr + 4)];
	vadd = (int)(((sptr->height << 8) - 0x4000) / (y2 - y1));
	uadd = (int)(((sptr->width << 8) - 0x4000) / (x2 - x1));
	ssptr = (unsigned char *)TexturePagePtrs[sptr->tpage] + sptr->offset;

	ustart = vstart = 0x4000;
	if (x1 < 0) // Clip On Screen Coordinates..
	{
		ustart -= uadd * x1;
		x1 = 0;
	}
	if (y1 < 0)
	{
		vstart -= vadd * y1;
		y1 = 0;
	}
	if (x2 > g_PhdWinxmax)
		x2 = g_PhdWinxmax + 1;
	if (y2 > g_PhdWinymax)
		y2 = g_PhdWinymax + 1;

	x2 -= x1;
	y2 -= y1;
	pixptr = (unsigned char *)phd_winptr + (y1 + g_PhdWinymin) * g_PhdScrwidth +
			 (x1 + g_PhdWinxmin);
	pixadd = g_PhdScrwidth - x2;

	if (dqptr == (unsigned char *)&depthq_table
					 [16][0] /*&& !G_RenderInfo.tUse236Palette*/) // If DepthQ
	{						 // Is At Middle
		for (; y2 > 0; y2--) // We Dont need any
		{					 // DepthQ look up..
			slptr = ssptr + ((vstart >> 16) << 8);
			u = ustart;
			for (xdif = x2; xdif > 0; xdif--)
			{
				if ((pix = *(slptr + (u >> 16))))
					*(pixptr) = pix;
				u += uadd;
				pixptr++;
			}
			pixptr += pixadd;
			vstart += vadd;
		}
	}
	else
	{
		for (; y2 > 0; y2--)
		{
			slptr = ssptr + ((vstart >> 16) << 8); // Use DepthQ
			u = ustart;							   // LookUp Draw....
			for (xdif = x2; xdif > 0; xdif--)
			{
				if ((pix = *(slptr + (u >> 16))))
					*(pixptr) = *(dqptr + pix);
				u += uadd;
				pixptr++;
			}
			pixptr += pixadd;
			vstart += vadd;
		}
	}
}
