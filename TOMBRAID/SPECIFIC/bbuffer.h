#ifndef _BACKBUFFER_
#define _BACKBUFFER_


extern unsigned char * phd_winptr_my;

void Create_Water_Palette();
void Create_Normal_Palette();
void Create_BackBuffer();
void Clear_BackBuffer();
void Present_BackBuffer();
void Delete_BackBuffer();

#endif
