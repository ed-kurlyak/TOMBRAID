/*
typedef struct phdspritestruct {
		uint16	tpage;
		uint16  offset;
		uint16	width;
		uint16	height;
		sint16	x1;
		sint16	y1;
		sint16	x2;
		sint16	y2;
} PHDSPRITESTRUCT;
*/

//#define MAX_SPRITES 512

//PHDSPRITESTRUCT  g_PhdSpriteInfo[MAX_SPRITES];		// pointers to Scaley Sprites

void insert_sprite(int nZDepth,int nX1,int nY1,int nX2,int nY2,int nSprite,int nShade);
void S_DrawSprite(unsigned int dwFlags,int nX,int nY,int nZ,short int nSprite,short int nShade,short int nScale);
void	S_DrawPickup(short int x, short int y, short int object_num);
short int	*ins_room_sprite( short int *objptr, int num );
void	S_DrawSpriteSize( int x, int y, int z, int w, int h, short int sprnum, short int shade, unsigned short int flags );

unsigned long TextLight[];
unsigned long TextDark[];
void S_DrawScreenSprite2d( int sx, int sy, int z, int scaleH, int scaleV, short int sprnum, short int shade, unsigned short int flags);
void S_DrawScreenSprite( int sx, int sy, int z, int scaleH, int scaleV, short int sprnum, short int shade, unsigned short int flags );


void	draw_scaled_spriteC( short int *inptr );
