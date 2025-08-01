
typedef struct xbuf_x
{
	int Xleft;
	int Xright;
} XBUF_X;

typedef struct xbuf_xg
{
	int Xleft;
	int Gleft;
	int Xright;
	int Gright;
} XBUF_XG;

typedef struct xbuf_xguv
{
	int Xleft;
	int Gleft;
	int Uleft;
	int Vleft;
	int Xright;
	int Gright;
	int Uright;
	int Vright;
} XBUF_XGUV;

typedef struct xbuf_persp
{
	int Xleft;
	int Gleft;
	int UOZleft;
	int VOZleft;
	int OOZleft;
	int Xright;
	int Gright;
	int UOZright;
	int VOZright;
	int OOZright;
} XBUF_PERSP;

typedef struct xbuf_persp_fp
{
	int Xleft;
	int Gleft;
	float UOZleft;
	float VOZleft;
	float OOZleft;
	int Xright;
	int Gright;
	float UOZright;
	float VOZright;
	float OOZright;
} XBUF_PERSP_FP;

// XBUF_PERSP	xbuffer[];

#define MAX_SCANLINES 1200					  // maximum height of screen
extern "C" XBUF_PERSP xbuffer[MAX_SCANLINES]; // Edge buffer for Polygon Draw

/*** 3d_xouta.s ***/

extern "C" void __cdecl flatA(int min_y, int max_y, char color);
extern "C" void __cdecl gourA(int min_y, int max_y, short int colbase);
extern "C" void __cdecl transA(int min_y, int max_y, short int colbase);
extern "C" void __cdecl gtmapA(int min_y, int max_y, char *tptr);
extern "C" void __cdecl wgtmapA(int min_y, int max_y, char *tptr);

/*
extern "C" void	__fastcall flatA( int min_y, int max_y, char color );
extern "C" void	__fastcall gourA( int min_y, int max_y, short int colbase );
extern "C" void	__fastcall transA( int min_y, int max_y, short int colbase );
extern "C" void	__fastcall gtmapA( int min_y, int max_y, char *tptr );
extern "C" void	__fastcall wgtmapA( int min_y, int max_y, char *tptr );
*/

void draw_poly_flat(short int *iptr);
void draw_poly_trans(short int *iptr);
void draw_poly_gouraud(short int *iptr);
void draw_poly_gtmap(short int *iptr);
void draw_poly_wgtmap(short int *iptr);
int xgen_x(short int *iptr);
int xgen_xg(short int *iptr);
int xgen_xguv(short int *iptr);
int xgen_xguvpersp_fp(short int *iptr);
void gtmap_persp32_fp(int ymin, int ymax, unsigned char *tptr);
void wgtmap_persp16_fp(int ymin, int ymax, char *tptr);
void draw_poly_gtmap_persp(short int *iptr);
void draw_poly_wgtmap_persp(short int *iptr);

// extern void (*poly_draw_routines[])(short int *);
void (*poly_draw_routines[])(short int *);
// void	phd_SortPolyList( int number,int buffer[][10]);
void phd_SortPolyList(int number, int buffer[][2]);
// void	do_quickysorty( int left, int right, int buffer[][10] );
void do_quickysorty(int left, int right, int buffer[][2]);
void phd_PrintPolyList(void *ptr);