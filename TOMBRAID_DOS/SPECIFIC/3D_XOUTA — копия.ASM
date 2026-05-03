;******************************************************************************
;*
;*                                              3D Output Routines
;*   24/07/95 11:11
;******************************************************************************

        .486

_DATA segment use32 public 'DATA'

        extern _g_PhdScrwidth:dword
        extern _phd_winptr:dword
        ;extern _g_PhdWinxmax:dword
        extern _g_PhdWinwidth:dword
        ;extern _phd_winheight:dword
        ;extern _g_PhdSpriteInfo:dword
        extern _xbuffer:dword
        extern _gouraud_table:byte
        extern _depthq_table:byte

_DATA ends

;******************************************************************************

;DGROUP GROUP _DATA

_TEXT   SEGMENT PARA PUBLIC USE32 'CODE'

        ASSUME cs:_TEXT
        ASSUME ds:_DATA


;***************************************************************************
;                       Output Flat-Shaded Polygon
;***************************************************************************
_flatA PROC PUBLIC

                pushad
                sub             edx,eax
                jle             @@exit_flatA

                mov             esi,eax
                shl             esi,3
                add     esi,offset[_xbuffer+2]        ; Get Address Xbuffer
      imul    eax,[_g_PhdScrwidth]        ; ebp = Line Pointer
                add             eax,[_phd_winptr]
                mov             ebp,eax
                cld                                                                      ; Clear direction flag!

                mov             bh,bl                            ; make eax = 4 Bytes of Color
                mov             ax,bx
                bswap   eax
                mov             ax,bx

@@lineloop_flatA:
                movsx   ecx,WORD PTR[esi]                        ; Get Xleft
                movsx   ebx,WORD PTR[esi+4]              ; Get Xright
                sub             ebx,ecx                                          ; ebx = Xdif
                jle             @@nextl_flatA
                lea             edi,[ebp+ecx]                            ; edi = Pixptr
                neg             ecx                              ; negate xleft
                and             ecx,3                        ; get pixels to nearest longword
                cmp             ecx,ebx                          ; if this is >= num pixels branch
                jge             @bout_flatA
                sub             ebx,ecx                          ; subtract from num pixels
                rep             stosb                            ; store start number pixels
                mov             ecx,ebx                          ; copy num pixels to ecx
                sar             ecx,2                        ; shift down by 2
                rep             stosd                            ; store longwords
                and             ebx,3                            ; make ebx = 0-3
@bout_flatA:  mov             ecx,ebx                          ; copy to ecx
                rep             stosb                        ; store ecx bytes
@@nextl_flatA: add             esi,8                                            ; increment Xbuffer
                add             ebp,DWORD PTR[_g_PhdScrwidth]     ; Add screen width to LinePtr
                dec             edx
                jnz             @@lineloop_flatA
@@exit_flatA: popad
                ret

_flatA ENDP

;***************************************************************************
;                                       Output Transparent Polygon
;***************************************************************************
T_LPTR  equ     0
T_YDIF  equ 4

_transA PROC PUBLIC

                pushad
                sub             esp,8
                sub             edx,eax
                jle             @@exit_transA
                mov             [esp+T_YDIF],edx

                lea             esi,[_depthq_table]

;;;             and             ebx,31
                cmp             ebx,32
                ja              @@exit_transA

                shl             ebx,8
                add             esi,ebx                                  ; esi points to req DQ table

                mov             ebp,eax
                shl             ebp,3
                add             ebp,offset[_xbuffer+2]

        imul    eax,[_g_PhdScrwidth]
                add             eax,[_phd_winptr]
                mov             [esp],eax                    ; [esp] = Line ptr

                xor             eax,eax                                          ; eax = 0

@@lineloop_transA:
                movsx   edi,WORD PTR[ebp]                        ; edi = Xleft
                test    edi,edi
                jns             @@x1ok_transA
                xor             edi,edi
@@x1ok_transA: movsx   ecx,WORD PTR[ebp+4]          ; ecx = Xright
                cmp             ecx,DWORD PTR[_g_PhdWinwidth]
                jle             @@x2ok_transA
                mov             ecx,DWORD PTR[_g_PhdWinwidth]
@@x2ok_transA: sub             ecx,edi                                          ; ecx = Xdif
                jle             @@nexline_transA

                add             edi,[esp]                                        ; get Pixel Ptr..
                test    edi,1                                            ; align output onto Word Boundary
                jz              @@loop16_transA                     ; by shoving out one pixel
                mov             al,[edi]                         ; or not
                mov             dl,[esi+eax]
                mov             [edi],dl
                dec             ecx
                inc             edi

@@loop16_transA:
                sub             ecx,16
                js              @@runout_transA


                mov             ebx,[edi]                       ; pixels 0-3
                mov             al,bl
                mov             dl,[esi+eax]
                mov             al,bh
                mov             dh,[esi+eax]
                bswap   ebx
                mov             [edi],dx
                mov             al,bh
                mov             dl,[esi+eax]
                mov             al,bl
                mov             dh,[esi+eax]
                mov             [edi+2],dx
                add             edi,4

                mov             ebx,[edi]           ; pixels 4-7
                mov             al,bl
                mov             dl,[esi+eax]
                mov             al,bh
                mov             dh,[esi+eax]
                bswap   ebx
                mov             [edi],dx
                mov             al,bh
                mov             dl,[esi+eax]
                mov             al,bl
                mov             dh,[esi+eax]
                mov             [edi+2],dx
                add             edi,4

                mov             ebx,[edi]           ; pixels 8-11
                mov             al,bl
                mov             dl,[esi+eax]
                mov             al,bh
                mov             dh,[esi+eax]
                bswap   ebx
                mov             [edi],dx
                mov             al,bh
                mov             dl,[esi+eax]
                mov             al,bl
                mov             dh,[esi+eax]
                mov             [edi+2],dx
                add             edi,4

                mov             ebx,[edi]           ; pixels 12-15
                mov             al,bl
                mov             dl,[esi+eax]
                mov             al,bh
                mov             dh,[esi+eax]
                bswap   ebx
                mov             [edi],dx
                mov             al,bh
                mov             dl,[esi+eax]
                mov             al,bl
                mov             dh,[esi+eax]
                mov             [edi+2],dx
                add             edi,4

                jmp             NEAR PTR @@loop16_transA

@@runout_transA:
                add             ecx,16
                jle             @@nexline_transA

@@rout_transA: mov             al,[edi]
                mov             dl,[esi+eax]
                mov             [edi],dl
                inc             edi
                dec             ecx
                jnz             @@rout_transA

@@nexline_transA:
                mov             ebx,DWORD PTR[_g_PhdScrwidth]
                add             ebp,8
                add             [esp],ebx
                dec             DWORD PTR[esp+T_YDIF]
                jnz             @@lineloop_transA

@@exit_transA: add             esp,8
                popad
                ret

_transA ENDP

;*****************************************************************************
;       Output loop for Gouraud-Shaded Polygon
;                       faster than C version By about 50%
;*****************************************************************************
G_GPTR  equ             0
G_YDIF  equ             4
G_XBUF  equ             8
G_LPTR  equ             12

gourA_ PROC PUBLIC
                push    ecx
        push    esi
        push    edi
        push    ebp
        sub     esp,16

                sub             edx,eax                                         ; get Ydif in edx
                jle             @@exit_gourA                          ; If <=0 then Exit
                mov             [esp+G_YDIF],edx                ; save Ydif

                mov             ecx,eax                     ; get position
                shl             ecx,4                       ; in Edge buffer
                add             ecx,offset[_xbuffer]
                mov             [esp+G_XBUF],ecx

        imul    eax,[_g_PhdScrwidth]             ; get Screen Pointer
                add             eax,[_phd_winptr]                ;
                mov             [esp+G_LPTR],eax

        movzx   ebx,bx                                          ; get Gouraud Num..
                shl             ebx,5                                           ; multiply by 5
                add             ebx,offset[_gouraud_table]       ; add on ptr
                mov             [esp],ebx                   ; Save Goraud Table Ptr..

@@lineloop_gourA:
                mov             eax,[esp+G_XBUF]                        ; eax = XBUFptr
                movsx   ebx,WORD PTR [eax+8+2]          ; ebx = Xright
                movsx   edi,WORD PTR [eax+2]            ; edi = Xleft

;               test    ebx,ebx                                         ; if Xright<=0     Nextline
;               jle             @@nexline_gourA
                cmp             edi,ebx                                         ; if Xright<=Xleft Nextline
                jge             @@nexline_gourA
;               cmp             edi,[_g_PhdWinwidth]                     ; if Xleft>=Winwidth Nextline
;               jge             @@nexline_gourA

                mov             ecx,[eax+4]                             ; ecx = Gleft
                mov             ebp,ebx                                         ; save Xright for a while
                mov             edx,[eax+8+4]               ; get Gright in edx

                sub             edx,ecx                                         ; edx = Gdif
                sub             ebx,edi                                         ; ebx = Xdif
                mov             eax,edx
                sar             edx,31
                idiv    ebx                         ; eax = Gadd

                test    edi,edi                                         ; test Xleft
                jns             @@x1on_gourA

                imul    edi,eax                                         ; get Xleft*Gadd
                sub             ecx,edi                     ; ecx = Gleft + Xleft*Gadd
                xor             edi,edi                     ; edi = Xleft = 0
@@x1on_gourA:
;               mov     edx,[_g_PhdWinwidth]          ; get winwidth in edx
;               cmp             edx,ebp                                         ; if Winwidth<Xright
;               jge     @@x2on_gourA
;               mov             ebp,edx                                         ; Make Xright = Winwidth
@@x2on_gourA:
                sub             ebp,edi                         ; ebp =  Xdif
                add             edi,[esp+G_LPTR]                        ; edi = Pixptr
                mov             edx,eax                                         ; EDX = Gadd.lo
                mov             esi,ecx                                         ; ESI = G.lo
                sar             eax,16                      ; AL = Gadd.hi
                sar             ecx,16                      ; CL = G.hi
                shl             edx,16
                shl             esi,16
                add             ecx,[esp]                                       ; add on Gtable Ptr to G.hi


                test    edi,1                                           ; align output onto Word Boundary
                jz              @@loop16_gourA                    ; by shoving out one pixel
                mov             bl,[ecx]                        ; or not
                add             esi,edx
                mov             [edi],bl
                adc             ecx,eax
                dec             ebp
                inc             edi

@@loop16_gourA:
                sub             ebp,16
                js              @@runout_gourA

                mov             bl,[ecx]                ; dump 16 pixels
                add             esi,edx                 ; at once
                adc             ecx,eax
                mov             bh,[ecx]
                add             esi,edx
                mov             [edi],bx
                adc             ecx,eax

                mov             bl,[ecx]
                add             esi,edx
                adc             ecx,eax
                mov             bh,[ecx]
                add             esi,edx
                mov             [edi+2],bx
                adc             ecx,eax

                mov             bl,[ecx]
                add             esi,edx
                adc             ecx,eax
                mov             bh,[ecx]
                add             esi,edx
                mov             [edi+4],bx
                adc             ecx,eax

                mov             bl,[ecx]
                add             esi,edx
                adc             ecx,eax
                mov             bh,[ecx]
                add             esi,edx
                mov             [edi+6],bx
                adc             ecx,eax

                mov             bl,[ecx]
                add             esi,edx
                adc             ecx,eax
                mov             bh,[ecx]
                add             esi,edx
                mov             [edi+8],bx
                adc             ecx,eax

                mov             bl,[ecx]
                add             esi,edx
                adc             ecx,eax
                mov             bh,[ecx]
                add             esi,edx
                mov             [edi+10],bx
                adc             ecx,eax

                mov             bl,[ecx]
                add             esi,edx
                adc             ecx,eax
                mov             bh,[ecx]
                add             esi,edx
                mov             [edi+12],bx
                adc             ecx,eax

                mov             bl,[ecx]
                add             esi,edx
                adc             ecx,eax
                mov             bh,[ecx]
                add             esi,edx
                mov             [edi+14],bx
                adc             ecx,eax

                add             edi,16
                jmp             NEAR PTR @@loop16_gourA

@@runout_gourA:
                add             ebp,16
                jle             @@nexline_gourA

@@rout_gourA: mov             bl,[ecx]            ; Dump remaining Pixels
                add             esi,edx
                mov             [edi],bl
                adc             ecx,eax
                inc             edi
                dec             ebp
                jnz             @@rout_gourA


@@nexline_gourA:
                mov             eax,[_g_PhdScrwidth]              ; add Screen Width to Line Ptr.
                add             DWORD PTR[esp+G_XBUF],16 ; increment Xbuffer
                add             [esp+G_LPTR],eax
                dec             DWORD PTR[esp+G_YDIF]
                jnz             @@lineloop_gourA                               ; Repeat Til YDIF zero

@@exit_gourA: add             esp,16
                pop             ebp
                pop             edi
                pop             esi
                pop             ecx
                ret

gourA_ ENDP


;*****************************************************************************
;                                       Output Gouraud Texture Map..
;*****************************************************************************
GTM_Uadd                equ             0
GTM_Vadd                equ             4
GTM_Gadd                equ             8
GTM_U                   equ             12
GTM_V                   equ             16
GTM_G                   equ             20
GTM_XDIF                equ             24
GTM_XBUF                equ     28
GTM_YDIF                equ     32
GTM_TPTR                equ             36
GTM_LPTR                equ             40

_gtmapA PROC PUBLIC

                pushad
                sub             esp,44
                sub             edx,eax
                jle             @@exit_gtmapA
                mov             [esp+GTM_YDIF],edx          ; save Ydif

                mov             ecx,eax
                shl             ecx,5
                add             ecx,offset[_xbuffer]
                mov             [esp+GTM_XBUF],ecx              ; save Xbuffer Ptr

                imul    eax,[_g_PhdScrwidth]
                add             eax,[_phd_winptr]
                mov             [esp+GTM_LPTR],eax          ; save LinePtr

                mov             [esp+GTM_TPTR],ebx                      ; save Texture Ptr

@@lineloop_gtmapA:
                mov             esi,[esp+GTM_XBUF]          ; get Xbuffer

                mov             edi,[esi]
                mov             ebp,[esi+16]
                sar             edi,16                      ; edi = Xleft
                sar             ebp,16                      ; ebp = Xright
                cmp             ebp,edi
                jle             @@nexline_gtmapA

                mov             ebx,ebp                                         ; ebx = Xdif
                sub             ebx,edi

                mov             ecx,[esi+4]                                     ; ecx = Gleft
                mov             eax,[esi+16+4]                          ; edx = Gright
                sub             eax,ecx                                         ; get Udif
                cdq
                idiv    ebx                                                     ; Uadd = Udif/Xdif
                mov             [esp+GTM_G],ecx
                mov             [esp+GTM_Gadd],eax
                mov             ecx,[esi+8]                                     ; ecx = Uleft
                mov             eax,[esi+16+8]                          ; edx = Uright
                sub             eax,ecx                                         ; get Udif
                cdq
                idiv    ebx                                                     ; Uadd = Udif/Xdif
                mov             [esp+GTM_U],ecx
                mov             [esp+GTM_Uadd],eax
                mov             ecx,[esi+12]                            ; ecx = Vleft
                mov             eax,[esi+16+12]                         ; edx = Vright
                sub             eax,ecx                                         ; get Vdif
                cdq
                idiv    ebx                                                     ; Vadd = Vdif/Xdif
                mov             [esp+GTM_V],ecx
                mov             [esp+GTM_Vadd],eax

                sub             ebp,edi
                add             edi,[esp+GTM_LPTR]              ; edi = Pixptr
                mov             esi,[esp+GTM_TPTR]      ; esi = Texptr
                mov             [esp+GTM_XDIF],ebp              ; [esp] = Xdif

                xor             eax,eax
                xor             ebx,ebx
                mov             ecx,[esp+GTM_U]
                mov             edx,[esp+GTM_V]
                mov             ebp,[esp+GTM_G]
                shl             ecx,16
                shl             edx,16
                shl             ebp,16

                mov             cl,[esp+GTM_Uadd+2]
                mov             ch,[esp+GTM_Vadd+2]
                mov             dh,[esp+GTM_Gadd+2]

                shl             DWORD PTR[esp+GTM_Uadd],16
                shl             DWORD PTR[esp+GTM_Vadd],16
                shl             DWORD PTR[esp+GTM_Gadd],16

                mov             al,[esp+GTM_U+2]
                mov             ah,[esp+GTM_V+2]
                mov             bh,[esp+GTM_G+2]


;  ESI  = Texture Pointer
;  EDI  = Pixel Pointer
;  AL   = U.hi
;  AH   = V.hi
;  BL   = ??
;  BH   = G.hi
;  CL   = U.add.hi
;  CH   = V.add.hi
;  DL   = ??
;  DH   = G.add.hi
;  ECX  = U.lo<<16
;  EDX  = V.lo<<16
;  EBP  = G.lo<<16


@@loop16_gtmapA:
                sub             DWORD PTR[esp+GTM_XDIF],16       ; Blat out 16 pixels...
                js              @@runout_gtmapA

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+1],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+2],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+3],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+4],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+5],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+6],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+7],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+8],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+9],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+10],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh


                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+11],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh


                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+12],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+13],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh


                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+14],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi+15],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh


                add             edi,16
                jmp             @@loop16_gtmapA


@@runout_gtmapA:
                add             DWORD PTR[esp+GTM_XDIF],16
                jle             @@nexline_gtmapA

@@rout_gtmapA:
                mov             bl,[esi+eax]
                mov             dl,[_depthq_table+ebx]
                add             ecx,[esp+GTM_Uadd]
                mov             [edi],dl
                adc             al,cl
                add             edx,[esp+GTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+GTM_Gadd]
                adc             bh,dh
                inc             edi
                dec             DWORD PTR[esp+GTM_XDIF]
                jnz             @@rout_gtmapA

@@nexline_gtmapA:
                mov             eax,[_g_PhdScrwidth]
                add             DWORD PTR[esp+GTM_XBUF],32
                add             [esp+GTM_LPTR],eax
                dec             DWORD PTR[esp+GTM_YDIF]
                jnz             @@lineloop_gtmapA

@@exit_gtmapA: add             esp,44
                popad
                ret

_gtmapA ENDP

;*****************************************************************************
;                                       Output Gouraud Windowed Texture Map..
;*****************************************************************************
WGTM_Uadd               equ             0
WGTM_Vadd               equ             4
WGTM_Gadd               equ             8
WGTM_U                  equ             12
WGTM_V                  equ             16
WGTM_G                  equ             20
WGTM_XDIF               equ             24
WGTM_XBUF               equ     28
WGTM_YDIF               equ     32
WGTM_TPTR               equ             36
WGTM_LPTR               equ             40

_wgtmapA        PROC PUBLIC

                pushad
                sub             esp,44
                sub             edx,eax
                jle             @@exit_wgtmapA
                mov             [esp+WGTM_YDIF],edx          ; save Ydif

                mov             ecx,eax
                shl             ecx,5
                add             ecx,offset[_xbuffer]
                mov             [esp+WGTM_XBUF],ecx                      ; get Xbuffer position

                imul    eax,[_g_PhdScrwidth]
                add             eax,[_phd_winptr]
                mov             [esp+WGTM_LPTR],eax          ; save LinePtr

                mov             [esp+WGTM_TPTR],ebx                      ; save Texture Ptr

@@lineloop_wgtmapA:
                mov             esi,[esp+WGTM_XBUF]          ; get Xbuffer

                mov             edi,[esi]
                mov             ebp,[esi+16]
                sar             edi,16                      ; edi = Xleft
                sar             ebp,16                      ; ebp = Xright
                cmp             ebp,edi
                jle             @@nexline_wgtmapA

                mov             ebx,ebp                                         ; ebx = Xdif
                sub             ebx,edi

                mov             ecx,[esi+4]                                     ; ecx = Gleft
                mov             eax,[esi+16+4]                          ; edx = Gright
                sub             eax,ecx                                         ; get Udif
                cdq
                idiv    ebx                                                     ; Uadd = Udif/Xdif
                mov             [esp+WGTM_G],ecx
                mov             [esp+WGTM_Gadd],eax
                mov             ecx,[esi+8]                                     ; ecx = Uleft
                mov             eax,[esi+16+8]                          ; edx = Uright
                sub             eax,ecx                                         ; get Udif
                cdq
                idiv    ebx                                                     ; Uadd = Udif/Xdif
                mov             [esp+WGTM_U],ecx
                mov             [esp+WGTM_Uadd],eax
                mov             ecx,[esi+12]                            ; ecx = Vleft
                mov             eax,[esi+16+12]                         ; edx = Vright
                sub             eax,ecx                                         ; get Vdif
                cdq
                idiv    ebx                                                     ; Vadd = Vdif/Xdif
                mov             [esp+WGTM_V],ecx
                mov             [esp+WGTM_Vadd],eax

                sub             ebp,edi
                add             edi,[esp+WGTM_LPTR]             ; edi = Pixptr
                mov             esi,[esp+WGTM_TPTR]      ; esi = Texptr
                mov             [esp+WGTM_XDIF],ebp             ; [esp] = Xdif

                xor             eax,eax
                xor             ebx,ebx
                mov             ecx,[esp+WGTM_U]
                mov             edx,[esp+WGTM_V]
                mov             ebp,[esp+WGTM_G]
                shl             ecx,16
                shl             edx,16
                shl             ebp,16

                mov             cl,[esp+WGTM_Uadd+2]
                mov             ch,[esp+WGTM_Vadd+2]
                mov             dh,[esp+WGTM_Gadd+2]

                shl             DWORD PTR[esp+WGTM_Uadd],16
                shl             DWORD PTR[esp+WGTM_Vadd],16
                shl             DWORD PTR[esp+WGTM_Gadd],16

                mov             al,[esp+WGTM_U+2]
                mov             ah,[esp+WGTM_V+2]
                mov             bh,[esp+WGTM_G+2]


;  ESI  = Texture Pointer
;  EDI  = Pixel Pointer
;  AL   = U.hi
;  AH   = V.hi
;  BL   = ??
;  BH   = G.hi
;  CL   = U.add.hi
;  CH   = V.add.hi
;  DL   = ??
;  DH   = G.add.hi
;  ECX  = U.lo<<16
;  EDX  = V.lo<<16
;  EBP  = G.lo<<16


@@loop16_wgtmapA:
                sub             DWORD PTR[esp+WGTM_XDIF],16       ; Blat out 16 pixels...
                js              @@runout_wgtmapA

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp0_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi],dl
@@skp0_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp1_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+1],dl
@@skp1_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp2_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+2],dl
@@skp2_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp3_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+3],dl
@@skp3_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp4_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+4],dl
@@skp4_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp5_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+5],dl
@@skp5_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp6_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+6],dl
@@skp6_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp7_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+7],dl
@@skp7_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp8_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+8],dl
@@skp8_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skp9_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+9],dl
@@skp9_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skpA_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+10],dl
@@skpA_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skpB_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+11],dl
@@skpB_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skpC_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+12],dl
@@skpC_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skpD_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+13],dl
@@skpD_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skpE_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+14],dl
@@skpE_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh

                mov             bl,[esi+eax]
                or              bl,bl
                jz      @@skpF_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi+15],dl
@@skpF_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh



                add             edi,16
                jmp             @@loop16_wgtmapA


@@runout_wgtmapA:
                add             DWORD PTR[esp+WGTM_XDIF],16
                jle             @@nexline_wgtmapA

@@rout_wgtmapA:
                mov             bl,[esi+eax]
                or              bl,bl
                jz              @@skpP_wgtmapA
                mov             dl,[_depthq_table+ebx]
                mov             [edi],dl
@@skpP_wgtmapA: add             ecx,[esp+WGTM_Uadd]
                adc             al,cl
                add             edx,[esp+WGTM_Vadd]
                adc             ah,ch
                add             ebp,[esp+WGTM_Gadd]
                adc             bh,dh
                inc             edi
                dec             DWORD PTR[esp+WGTM_XDIF]
                jnz             @@rout_wgtmapA




@@nexline_wgtmapA:
                mov             eax,[_g_PhdScrwidth]
                add             DWORD PTR[esp+WGTM_XBUF],32
                add             [esp+WGTM_LPTR],eax
                dec             DWORD PTR[esp+WGTM_YDIF]
                jnz             @@lineloop_wgtmapA

@@exit_wgtmapA: add             esp,44
                popad
                ret

_wgtmapA ENDP

;******************************************************************************
_TEXT   ENDS
        END
