#include "bbuffer.h"
#include "const.h"
#include "vars.h"
//#include "header.h"
#include "screen.h"
#include <conio.h>
#include <memory.h>


        //register color address VGA
    #define PALETTE_MASK            0x3c6
    #define PALETTE_REGISTER_RD     0x3c7    //register VGA for reading
    #define PALETTE_REGISTER_WR     0x3c8    //register VGA for writing
    #define PALETTE_DATA            0x3c9    //color data 3 byte following


//int Screen_Width;
//int Screen_Height;

unsigned char * phd_winptr_my = 0;

char * video_buffer =(char*) 0xA0000L;

#define VGA_INPUT_STATUS_1   0x3DA
#define VGA_VSYNC_MASK 0x08

void Wait_For_Vsync(void)
{
        

    while (inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK)
    {
    }
    while (! (inp(VGA_INPUT_STATUS_1) & VGA_VSYNC_MASK) )
    {
    }
}


void Create_Water_Palette()
{
        // water color (R, G, B). 1.0 means pass-through, 0.0 means no value at all.
        // [0.6, 0.7, 1.0] is original DOS version filter,
        // [0.45, 1.0, 1.0] is default TombATI filter.

        /*
        //tr1 water color
        unsigned char r = 255 * 0.6f; //153 - 0x99
        unsigned char g = 255 * 0.7f; // 178 - 0xB2
        unsigned char b = 255 * 1.0f; // 255 - 0xFF
        */

        // tr ati water color
        /*
        unsigned char r = 255 * 0.45f; //114 - 0x72
        unsigned char g = 255 * 1.0f; // 255 - 0xFF
        unsigned char b = 255 * 1.0f; // 255 - 0xFF
        */

        //palette_t pal[256];
/*
        for (int i = 0; i < 256; i++)
        {
            */
                /*
                pal[i].red = (unsigned char) (GameNormalPalette[i].r * 0.6f);
                pal[i].green = (unsigned char) (GameNormalPalette[i].g * 0.7f);
                pal[i].blue =  (unsigned char) (GameNormalPalette[i].b * 1.0f);
                */

                /*
                pal[i].red = (unsigned char)(GameNormalPalette[i].r * 2 / 3);
                pal[i].green = (unsigned char)(GameNormalPalette[i].g * 2 / 3);
                pal[i].blue = (unsigned char)GameNormalPalette[i].b;
                */

                //pal[i].red = GameWaterPalette[i].r;
                //pal[i].green = GameWaterPalette[i].g;
                //pal[i].blue = GameWaterPalette[i].b;
                /*
        }

*/



    //one after another r,g,b

    for(int i = 0; i<256; i++)
    {
        //register color has 6 bit - 64
        //picture has color 8 bit - 256
        //convert i.e. 256/64 = 4

        unsigned char r = GameWaterPalette[i].r/4;
        unsigned char g = GameWaterPalette[i].g/4;
        unsigned char b = GameWaterPalette[i].b/4;

                outp(PALETTE_MASK, 0xff);
        outp(PALETTE_REGISTER_WR, i);
        outp(PALETTE_DATA, r);
                outp(PALETTE_DATA, g);
        outp(PALETTE_DATA, b);
    }   

}

void Create_Normal_Palette()
{
    /*
        palette_t pal[256];

        for (int i = 0; i < 256; i++)
        {
                pal[i].red = GameNormalPalette[i].r;
                pal[i].green = GameNormalPalette[i].g;
                pal[i].blue = GameNormalPalette[i].b;
        }
*/

        for(int i = 0; i<256; i++)
    {
        //register color has 6 bit - 64
        //picture has color 8 bit - 256
        //convert i.e. 256/64 = 4

        unsigned char r = GameNormalPalette[i].r/4;
        unsigned char g = GameNormalPalette[i].g/4;
        unsigned char b = GameNormalPalette[i].b/4;

        outp(PALETTE_MASK, 0xff);
        outp(PALETTE_REGISTER_WR, i);
        outp(PALETTE_DATA, r);
        outp(PALETTE_DATA, g);
        outp(PALETTE_DATA, b);
    }   

}


void set_mode();

#pragma aux set_mode = \
    "mov ax, 4f02h" \
    "mov bx, 0101h" \
    "int 10h" \
    modify [ax bx]

void Create_BackBuffer()
{

        //Screen_Width = Screen_GetResWidth();
        //Screen_Height = Screen_GetResHeight();


/*
    _asm {
            mov ax, 0x4f02
            mov bx, 0x101 //set resolution 640x480
            int 10h
        };
*/
  

  set_mode();

    phd_winptr_my = new unsigned char[307200]; //640 * 480 = 307200
   

    int back_lpitch = 640;




        // Create_Normal_Palette();
}

void Clear_BackBuffer()
{
        memset(phd_winptr_my, 0, 307200);

        //char *phd_winptr_my = NULL;

        //phd_winptr_my = (char *)dibdc->surface;

        //очищаем m_BackBuffer (экран)
        /*
        for (int x = 0; x < Screen_Width; x++)
        {
                for (int y = 0; y < Screen_Height; y++)
                {
                        int Index = y * Screen_Width + x;

                        phd_winptr_my[Index + 0] = 0;
                }
        }
                */
}


void set_vpage4();

#pragma aux set_vpage4 = \
    "mov ax, 4f05h" \
    "mov bx, 0" \
    "mov dx, 4" \
    "int 10h" \
    modify [ax bx dx]


void set_vpage3();

#pragma aux set_vpage3 = \
    "mov ax, 4f05h" \
    "mov bx, 0" \
    "mov dx, 3" \
    "int 10h" \
    modify [ax bx dx]



void set_vpage2();

#pragma aux set_vpage2 = \
    "mov ax, 4f05h" \
    "mov bx, 0" \
    "mov dx, 2" \
    "int 10h" \
    modify [ax bx dx]


void set_vpage1();

#pragma aux set_vpage1 = \
    "mov ax, 4f05h" \
    "mov bx, 0" \
    "mov dx, 1" \
    "int 10h" \
    modify [ax bx dx]


void set_vpage0();

#pragma aux set_vpage0 = \
    "mov ax, 4f05h" \
    "mov bx, 0" \
    "mov dx, 0" \
    "int 10h" \
    modify [ax bx dx]




        /*
        не правильно ax затирается

#pragma aux set_page = \
        "mov ax, 4f05h" \
        "mov bx, 0" \
        "mov dx, 1" \
        "int 10h" \
        parm [ax] \
        modify [ax bx dx]
        */

//обязательное предварительное обявление функции
//без этого не работает
void set_page(int page);
//правильно ax НЕ затирается
#pragma aux set_page = \
        "mov cx, ax" \
        "mov ax, 4f05h" \
        "mov bx, 0" \
        "mov dx, cx" \
        "int 10h" \
        parm [ax] \
        modify [ax bx dx]

//              set_page(1);

void Present_BackBuffer()
{

    //Wait_For_Vsync();
        
        /*
        for ( short int i = 0; i < 5; i++)
        {
            if(i < 4)
            {
                        
                
                _asm
                {
                        mov ax, 0x4f05  //функцию установки видеостраницы
                        mov dx, i   //номер видео страницы от 0 до 4
                        mov bx, 0x0 
                        int 10h
                    }
                    
        
                  //  set_page(i);
                    
                memcpy((unsigned char*)video_buffer, phd_winptr_my + i * 65536, 65536);
            }
            
            if(i == 4)
            {
                        
                
                _asm {
                        mov ax, 0x4f05  //функцию установки видеостраницы
                        mov dx, 4   //номер видео страницы от 0 до 4
                        mov bx, 0x0 
                        int 10h
                    }

                    
        
                   // set_vpage();

                memcpy((unsigned char*)video_buffer, phd_winptr_my + 4 * 65536, 45056);
            }
        } //for ( short int i = 0; i < 5; i++)

        */      
    
                /*
        не правильно ax затирается

#pragma aux set_page = \
        "mov ax, 4f05h" \
        "mov bx, 0" \
        "mov dx, 1" \
        "int 10h" \
        parm [ax] \
        modify [ax bx dx]
        */

        /*
        //правильно ax НЕ затирается
#pragma aux set_page = \
        "mov cx, ax" \
        "mov ax, 4f05h" \
        "mov bx, 0" \
        "mov dx, ax" \
        "int 10h" \
        parm [ax] \
        modify [ax bx dx]

                set_page(1);

                */


        set_vpage0();
		//set_page(0);

        memcpy((unsigned char*)video_buffer, phd_winptr_my, 65536);
        set_vpage1();
		//set_page(1);

        memcpy((unsigned char*)video_buffer, phd_winptr_my + 1 * 65536, 65536);


        set_vpage2();
        //set_page(2);

        memcpy((unsigned char*)video_buffer, phd_winptr_my + 2 * 65536, 65536);


        set_vpage3();
		//set_page(3);

        memcpy((unsigned char*)video_buffer, phd_winptr_my + 3 * 65536, 65536);

        set_vpage4();
        //set_page(4);

        memcpy((unsigned char*)video_buffer, phd_winptr_my + 4 * 65536, 45056);

        
}

void set_text_mode();

#pragma aux set_text_mode = \
    "mov ax, 0003h" \
    "int 10h" \
    modify [ax]

void Delete_BackBuffer()
{


    delete [] phd_winptr_my;

/*
            _asm {
            mov ah, 0x0;
            mov al,0x03; //set text video mode
            int 10h;  //set video mode call interrupt
        }
        */

        set_text_mode();

}
