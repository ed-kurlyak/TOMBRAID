;******************************************************************************
;*
;*						3D Output Routines
;*   24/07/95 11:11
;******************************************************************************

	.486

_DATA	SEGMENT PARA PUBLIC USE32 'DATA'

	EXTERNDEF C phd_scrwidth:DWORD
	EXTERNDEF C phd_winptr:DWORD
	EXTERNDEF C phd_winxmax:DWORD
	EXTERNDEF C phd_winwidth:DWORD
	EXTERNDEF C phd_winheight:DWORD
	EXTERNDEF C phdsprinfo:DWORD
	EXTERNDEF C xbuffer:DWORD
	EXTERNDEF C gouraud_table:BYTE
	EXTERNDEF C depthq_table:BYTE

_DATA	ENDS

;******************************************************************************

;DGROUP	GROUP _DATA

_TEXT	SEGMENT PARA PUBLIC USE32 'CODE'

	ASSUME cs:_TEXT
	ASSUME ds:_DATA


;***************************************************************************
;			Output Flat-Shaded Polygon
;***************************************************************************
_flatA PROC PUBLIC

		pushad
		sub		edx,eax
		jle		@@exit

		mov		esi,eax
		shl		esi,3
		add     esi,offset[xbuffer+2]        ; Get Address Xbuffer
      imul    eax,[phd_scrwidth]      	 ; ebp = Line Pointer
		add		eax,[phd_winptr]
		mov		ebp,eax
		cld									 ; Clear direction flag!

		mov		bh,bl                   	 ; make eax = 4 Bytes of Color
		mov		ax,bx
		bswap	eax
		mov		ax,bx

@@lineloop:
		movsx	ecx,WORD PTR[esi]			 ; Get Xleft
		movsx	ebx,WORD PTR[esi+4]    	 	 ; Get Xright
		sub		ebx,ecx  					 ; ebx = Xdif
		jle		@@nexl
		lea		edi,[ebp+ecx]				 ; edi = Pixptr
		neg		ecx                      	 ; negate xleft
		and		ecx,3 	                     ; get pixels to nearest longword
		cmp		ecx,ebx   	                 ; if this is >= num pixels branch
		jge		@bout
		sub		ebx,ecx       	           	 ; subtract from num pixels
		rep		stosb           	         ; store start number pixels
		mov		ecx,ebx               	   	 ; copy num pixels to ecx
		sar		ecx,2 	                     ; shift down by 2
		rep		stosd   	                 ; store longwords
		and		ebx,3        	           	 ; make ebx = 0-3
@bout:	mov		ecx,ebx           	       	 ; copy to ecx
		rep		stosb               	     ; store ecx bytes
@@nexl:	add		esi,8						 ; increment Xbuffer
		add		ebp,DWORD PTR[phd_scrwidth]	 ; Add screen width to LinePtr
		dec		edx
		jnz		@@lineloop
@@exit:	popad
		ret

_flatA ENDP

;***************************************************************************
;					Output Transparent Polygon
;***************************************************************************
T_LPTR	equ	0
T_YDIF	equ 4

_transA PROC PUBLIC

		pushad
		sub		esp,8
		sub		edx,eax
		jle		@@exit
		mov		[esp+T_YDIF],edx

		lea		esi,[depthq_table]

;;;		and		ebx,31
		cmp		ebx,32
		ja		@@exit

		shl		ebx,8
		add		esi,ebx     				 ; esi points to req DQ table

		mov		ebp,eax
		shl		ebp,3
		add		ebp,offset[xbuffer+2]

        imul    eax,[phd_scrwidth]
		add		eax,[phd_winptr]
		mov		[esp],eax                    ; [esp] = Line ptr

		xor		eax,eax						 ; eax = 0

@@lineloop:
		movsx	edi,WORD PTR[ebp]			 ; edi = Xleft
		test	edi,edi
		jns		@@x1ok
		xor		edi,edi
@@x1ok:	movsx	ecx,WORD PTR[ebp+4]          ; ecx = Xright
		cmp		ecx,DWORD PTR[phd_winwidth]
		jle		@@x2ok
		mov		ecx,DWORD PTR[phd_winwidth]
@@x2ok: sub		ecx,edi						 ; ecx = Xdif
		jle		@@nexline

		add		edi,[esp]			 		 ; get Pixel Ptr..
		test	edi,1						 ; align output onto Word Boundary
		jz		@@loop16                     ; by shoving out one pixel
		mov		al,[edi]                	 ; or not
		mov		dl,[esi+eax]
		mov		[edi],dl
		dec		ecx
		inc		edi

@@loop16:
		sub		ecx,16
		js		@@runout


		mov		ebx,[edi]			; pixels 0-3
		mov		al,bl
		mov		dl,[esi+eax]
		mov		al,bh
		mov		dh,[esi+eax]
		bswap	ebx
		mov		[edi],dx
		mov		al,bh
		mov		dl,[esi+eax]
		mov		al,bl
		mov		dh,[esi+eax]
		mov		[edi+2],dx
		add		edi,4

		mov		ebx,[edi]           ; pixels 4-7
		mov		al,bl
		mov		dl,[esi+eax]
		mov		al,bh
		mov		dh,[esi+eax]
		bswap	ebx
		mov		[edi],dx
		mov		al,bh
		mov		dl,[esi+eax]
		mov		al,bl
		mov		dh,[esi+eax]
		mov		[edi+2],dx
		add		edi,4

		mov		ebx,[edi]           ; pixels 8-11
		mov		al,bl
		mov		dl,[esi+eax]
		mov		al,bh
		mov		dh,[esi+eax]
		bswap	ebx
		mov		[edi],dx
		mov		al,bh
		mov		dl,[esi+eax]
		mov		al,bl
		mov		dh,[esi+eax]
		mov		[edi+2],dx
		add		edi,4

		mov		ebx,[edi]           ; pixels 12-15
		mov		al,bl
		mov		dl,[esi+eax]
		mov		al,bh
		mov		dh,[esi+eax]
		bswap	ebx
		mov		[edi],dx
		mov		al,bh
		mov		dl,[esi+eax]
		mov		al,bl
		mov		dh,[esi+eax]
		mov		[edi+2],dx
		add		edi,4

		jmp		NEAR PTR @@loop16

@@runout:
		add		ecx,16
		jle		@@nexline

@@rout: mov		al,[edi]
		mov		dl,[esi+eax]
		mov		[edi],dl
		inc		edi
		dec		ecx
		jnz		@@rout

@@nexline:
		mov		ebx,DWORD PTR[phd_scrwidth]
		add		ebp,8
		add		[esp],ebx
		dec		DWORD PTR[esp+T_YDIF]
		jnz		@@lineloop

@@exit:	add		esp,8
		popad
		ret

_transA ENDP

;*****************************************************************************
;     	Output loop for Gouraud-Shaded Polygon
;			faster than C version By about 50%
;*****************************************************************************
G_GPTR	equ		0
G_YDIF	equ		4
G_XBUF	equ		8
G_LPTR	equ		12

_gourA PROC PUBLIC
		push    ecx
        push    esi
        push    edi
        push    ebp
        sub     esp,16

		sub		edx,eax						; get Ydif in edx
		jle		@@exit                  	; If <=0 then Exit
		mov		[esp+G_YDIF],edx        	; save Ydif

		mov		ecx,eax                     ; get position
		shl		ecx,4                       ; in Edge buffer
		add		ecx,offset[xbuffer]
		mov		[esp+G_XBUF],ecx

        imul    eax,[phd_scrwidth]      	; get Screen Pointer
		add		eax,[phd_winptr]        	;
		mov		[esp+G_LPTR],eax

        movzx   ebx,bx						; get Gouraud Num..
		shl		ebx,5						; multiply by 5
		add		ebx,offset[gouraud_table]	; add on ptr
		mov		[esp],ebx                   ; Save Goraud Table Ptr..

@@lineloop:
		mov		eax,[esp+G_XBUF]			; eax = XBUFptr
		movsx	ebx,WORD PTR [eax+8+2]		; ebx = Xright
		movsx	edi,WORD PTR [eax+2]		; edi = Xleft

;		test    ebx,ebx						; if Xright<=0 	   Nextline
;		jle		@@nexline
		cmp		edi,ebx  					; if Xright<=Xleft Nextline
		jge		@@nexline
;		cmp		edi,[phd_winwidth]			; if Xleft>=Winwidth Nextline
;		jge		@@nexline

		mov		ecx,[eax+4]   				; ecx = Gleft
		mov		ebp,ebx						; save Xright for a while
		mov		edx,[eax+8+4]               ; get Gright in edx

		sub		edx,ecx						; edx = Gdif
		sub		ebx,edi						; ebx = Xdif
		mov		eax,edx
		sar		edx,31
		idiv	ebx                         ; eax = Gadd

		test	edi,edi						; test Xleft
		jns		@@x1on

		imul	edi,eax						; get Xleft*Gadd
		sub		ecx,edi                     ; ecx = Gleft + Xleft*Gadd
		xor		edi,edi                     ; edi = Xleft = 0
@@x1on:
;		mov     edx,[phd_winwidth]          ; get winwidth in edx
;		cmp		edx,ebp						; if Winwidth<Xright
;		jge     @@x2on
;		mov		ebp,edx						; Make Xright = Winwidth
@@x2on:
		sub		ebp,edi                   	; ebp =  Xdif
		add		edi,[esp+G_LPTR]			; edi = Pixptr
		mov		edx,eax						; EDX = Gadd.lo
		mov		esi,ecx						; ESI = G.lo
		sar		eax,16                      ; AL = Gadd.hi
		sar		ecx,16                      ; CL = G.hi
		shl		edx,16
		shl		esi,16
		add		ecx,[esp]					; add on Gtable Ptr to G.hi


		test	edi,1						; align output onto Word Boundary
		jz		@@loop16                    ; by shoving out one pixel
		mov		bl,[ecx]                	; or not
		add		esi,edx
		mov		[edi],bl
		adc		ecx,eax
		dec		ebp
		inc		edi

@@loop16:
		sub		ebp,16
		js		@@runout

		mov		bl,[ecx]                ; dump 16 pixels
		add		esi,edx                 ; at once
		adc		ecx,eax
		mov		bh,[ecx]
		add		esi,edx
		mov		[edi],bx
		adc		ecx,eax

		mov		bl,[ecx]
		add		esi,edx
		adc		ecx,eax
		mov		bh,[ecx]
		add		esi,edx
		mov		[edi+2],bx
		adc		ecx,eax

		mov		bl,[ecx]
		add		esi,edx
		adc		ecx,eax
		mov		bh,[ecx]
		add		esi,edx
		mov		[edi+4],bx
		adc		ecx,eax

		mov		bl,[ecx]
		add		esi,edx
		adc		ecx,eax
		mov		bh,[ecx]
		add		esi,edx
		mov		[edi+6],bx
		adc		ecx,eax

		mov		bl,[ecx]
		add		esi,edx
		adc		ecx,eax
		mov		bh,[ecx]
		add		esi,edx
		mov		[edi+8],bx
		adc		ecx,eax

		mov		bl,[ecx]
		add		esi,edx
		adc		ecx,eax
		mov		bh,[ecx]
		add		esi,edx
		mov		[edi+10],bx
		adc		ecx,eax

		mov		bl,[ecx]
		add		esi,edx
		adc		ecx,eax
		mov		bh,[ecx]
		add		esi,edx
		mov		[edi+12],bx
		adc		ecx,eax

		mov		bl,[ecx]
		add		esi,edx
		adc		ecx,eax
		mov		bh,[ecx]
		add		esi,edx
		mov		[edi+14],bx
		adc		ecx,eax

		add		edi,16
		jmp		NEAR PTR @@loop16

@@runout:
		add		ebp,16
		jle		@@nexline

@@rout:	mov		bl,[ecx]            ; Dump remaining Pixels
		add		esi,edx
		mov		[edi],bl
		adc		ecx,eax
		inc		edi
		dec		ebp
		jnz		@@rout


@@nexline:
		mov		eax,[phd_scrwidth]		 ; add Screen Width to Line Ptr.
		add		DWORD PTR[esp+G_XBUF],16 ; increment Xbuffer
		add		[esp+G_LPTR],eax
		dec		DWORD PTR[esp+G_YDIF]
		jnz		@@lineloop				 ; Repeat Til YDIF zero

@@exit: add		esp,16
		pop		ebp
		pop		edi
		pop		esi
		pop		ecx
		ret

_gourA ENDP


;*****************************************************************************
;					Output Gouraud Texture Map..
;*****************************************************************************
GTM_Uadd		equ		0
GTM_Vadd		equ		4
GTM_Gadd		equ		8
GTM_U			equ		12
GTM_V			equ		16
GTM_G			equ		20
GTM_XDIF		equ		24
GTM_XBUF		equ     28
GTM_YDIF		equ     32
GTM_TPTR		equ		36
GTM_LPTR		equ		40

_gtmapA	PROC PUBLIC

		pushad
		sub		esp,44
		sub		edx,eax
		jle		@@exit
		mov		[esp+GTM_YDIF],edx          ; save Ydif

		mov		ecx,eax
		shl		ecx,5
		add		ecx,offset[xbuffer]
		mov		[esp+GTM_XBUF],ecx   		; save Xbuffer Ptr

		imul	eax,[phd_scrwidth]
		add		eax,[phd_winptr]
		mov		[esp+GTM_LPTR],eax          ; save LinePtr

		mov		[esp+GTM_TPTR],ebx			; save Texture Ptr

@@lineloop:
		mov		esi,[esp+GTM_XBUF]          ; get Xbuffer

		mov		edi,[esi]
		mov		ebp,[esi+16]
		sar		edi,16                      ; edi = Xleft
		sar		ebp,16                      ; ebp = Xright
		cmp		ebp,edi
		jle		@@nexline

		mov		ebx,ebp						; ebx = Xdif
		sub		ebx,edi

		mov		ecx,[esi+4]					; ecx = Gleft
		mov		eax,[esi+16+4]				; edx = Gright
		sub		eax,ecx						; get Udif
		cdq
		idiv	ebx							; Uadd = Udif/Xdif
		mov		[esp+GTM_G],ecx
		mov		[esp+GTM_Gadd],eax
		mov		ecx,[esi+8]					; ecx = Uleft
		mov		eax,[esi+16+8]				; edx = Uright
		sub		eax,ecx						; get Udif
		cdq
		idiv	ebx							; Uadd = Udif/Xdif
		mov		[esp+GTM_U],ecx
		mov		[esp+GTM_Uadd],eax
		mov		ecx,[esi+12]				; ecx = Vleft
		mov		eax,[esi+16+12]				; edx = Vright
		sub		eax,ecx						; get Vdif
		cdq
		idiv	ebx							; Vadd = Vdif/Xdif
		mov		[esp+GTM_V],ecx
		mov		[esp+GTM_Vadd],eax

		sub		ebp,edi
		add		edi,[esp+GTM_LPTR]		; edi = Pixptr
		mov		esi,[esp+GTM_TPTR]      ; esi = Texptr
		mov		[esp+GTM_XDIF],ebp		; [esp] = Xdif

		xor		eax,eax
		xor		ebx,ebx
		mov		ecx,[esp+GTM_U]
		mov		edx,[esp+GTM_V]
		mov		ebp,[esp+GTM_G]
		shl		ecx,16
		shl		edx,16
		shl		ebp,16

		mov		cl,[esp+GTM_Uadd+2]
		mov		ch,[esp+GTM_Vadd+2]
		mov		dh,[esp+GTM_Gadd+2]

		shl		DWORD PTR[esp+GTM_Uadd],16
		shl		DWORD PTR[esp+GTM_Vadd],16
		shl		DWORD PTR[esp+GTM_Gadd],16

		mov		al,[esp+GTM_U+2]
		mov		ah,[esp+GTM_V+2]
		mov		bh,[esp+GTM_G+2]


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


@@loop16:
		sub		DWORD PTR[esp+GTM_XDIF],16       ; Blat out 16 pixels...
		js		@@runout

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+1],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+2],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+3],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+4],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+5],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+6],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+7],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+8],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+9],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+10],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh


		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+11],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh


		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+12],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+13],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh


		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+14],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi+15],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh


		add		edi,16
		jmp		@@loop16


@@runout:
		add		DWORD PTR[esp+GTM_XDIF],16
		jle		@@nexline

@@rout:
		mov		bl,[esi+eax]
		mov		dl,[depthq_table+ebx]
		add		ecx,[esp+GTM_Uadd]
		mov		[edi],dl
		adc		al,cl
		add		edx,[esp+GTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+GTM_Gadd]
		adc		bh,dh
		inc		edi
		dec		DWORD PTR[esp+GTM_XDIF]
		jnz		@@rout

@@nexline:
		mov		eax,[phd_scrwidth]
		add		DWORD PTR[esp+GTM_XBUF],32
		add		[esp+GTM_LPTR],eax
		dec		DWORD PTR[esp+GTM_YDIF]
		jnz		@@lineloop

@@exit:	add		esp,44
		popad
		ret

_gtmapA ENDP

;*****************************************************************************
;					Output Gouraud Windowed Texture Map..
;*****************************************************************************
WGTM_Uadd		equ		0
WGTM_Vadd		equ		4
WGTM_Gadd		equ		8
WGTM_U			equ		12
WGTM_V			equ		16
WGTM_G			equ		20
WGTM_XDIF		equ		24
WGTM_XBUF		equ     28
WGTM_YDIF		equ     32
WGTM_TPTR		equ		36
WGTM_LPTR		equ		40

_wgtmapA	PROC PUBLIC

		pushad
		sub		esp,44
		sub		edx,eax
		jle		@@exit
		mov		[esp+WGTM_YDIF],edx          ; save Ydif

		mov		ecx,eax
		shl		ecx,5
		add		ecx,offset[xbuffer]
		mov		[esp+WGTM_XBUF],ecx			 ; get Xbuffer position

		imul	eax,[phd_scrwidth]
		add		eax,[phd_winptr]
		mov		[esp+WGTM_LPTR],eax          ; save LinePtr

		mov		[esp+WGTM_TPTR],ebx			 ; save Texture Ptr

@@lineloop:
		mov		esi,[esp+WGTM_XBUF]          ; get Xbuffer

		mov		edi,[esi]
		mov		ebp,[esi+16]
		sar		edi,16                      ; edi = Xleft
		sar		ebp,16                      ; ebp = Xright
		cmp		ebp,edi
		jle		@@nexline

		mov		ebx,ebp						; ebx = Xdif
		sub		ebx,edi

		mov		ecx,[esi+4]					; ecx = Gleft
		mov		eax,[esi+16+4]				; edx = Gright
		sub		eax,ecx						; get Udif
		cdq
		idiv	ebx							; Uadd = Udif/Xdif
		mov		[esp+WGTM_G],ecx
		mov		[esp+WGTM_Gadd],eax
		mov		ecx,[esi+8]					; ecx = Uleft
		mov		eax,[esi+16+8]				; edx = Uright
		sub		eax,ecx						; get Udif
		cdq
		idiv	ebx							; Uadd = Udif/Xdif
		mov		[esp+WGTM_U],ecx
		mov		[esp+WGTM_Uadd],eax
		mov		ecx,[esi+12]				; ecx = Vleft
		mov		eax,[esi+16+12]				; edx = Vright
		sub		eax,ecx						; get Vdif
		cdq
		idiv	ebx							; Vadd = Vdif/Xdif
		mov		[esp+WGTM_V],ecx
		mov		[esp+WGTM_Vadd],eax

		sub		ebp,edi
		add		edi,[esp+WGTM_LPTR]		; edi = Pixptr
		mov		esi,[esp+WGTM_TPTR]      ; esi = Texptr
		mov		[esp+WGTM_XDIF],ebp		; [esp] = Xdif

		xor		eax,eax
		xor		ebx,ebx
		mov		ecx,[esp+WGTM_U]
		mov		edx,[esp+WGTM_V]
		mov		ebp,[esp+WGTM_G]
		shl		ecx,16
		shl		edx,16
		shl		ebp,16

		mov		cl,[esp+WGTM_Uadd+2]
		mov		ch,[esp+WGTM_Vadd+2]
		mov		dh,[esp+WGTM_Gadd+2]

		shl		DWORD PTR[esp+WGTM_Uadd],16
		shl		DWORD PTR[esp+WGTM_Vadd],16
		shl		DWORD PTR[esp+WGTM_Gadd],16

		mov		al,[esp+WGTM_U+2]
		mov		ah,[esp+WGTM_V+2]
		mov		bh,[esp+WGTM_G+2]


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


@@loop16:
		sub		DWORD PTR[esp+WGTM_XDIF],16       ; Blat out 16 pixels...
		js		@@runout

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp0
		mov		dl,[depthq_table+ebx]
		mov		[edi],dl
@@skp0:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp1
		mov		dl,[depthq_table+ebx]
		mov		[edi+1],dl
@@skp1:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp2
		mov		dl,[depthq_table+ebx]
		mov		[edi+2],dl
@@skp2:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp3
		mov		dl,[depthq_table+ebx]
		mov		[edi+3],dl
@@skp3:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp4
		mov		dl,[depthq_table+ebx]
		mov		[edi+4],dl
@@skp4:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp5
		mov		dl,[depthq_table+ebx]
		mov		[edi+5],dl
@@skp5:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp6
		mov		dl,[depthq_table+ebx]
		mov		[edi+6],dl
@@skp6:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp7
		mov		dl,[depthq_table+ebx]
		mov		[edi+7],dl
@@skp7:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp8
		mov		dl,[depthq_table+ebx]
		mov		[edi+8],dl
@@skp8:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skp9
		mov		dl,[depthq_table+ebx]
		mov		[edi+9],dl
@@skp9:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skpA
		mov		dl,[depthq_table+ebx]
		mov		[edi+10],dl
@@skpA:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skpB
		mov		dl,[depthq_table+ebx]
		mov		[edi+11],dl
@@skpB:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skpC
		mov		dl,[depthq_table+ebx]
		mov		[edi+12],dl
@@skpC:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skpD
		mov		dl,[depthq_table+ebx]
		mov		[edi+13],dl
@@skpD:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skpE
		mov		dl,[depthq_table+ebx]
		mov		[edi+14],dl
@@skpE:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh

		mov		bl,[esi+eax]
		or		bl,bl
		jz      @@skpF
		mov		dl,[depthq_table+ebx]
		mov		[edi+15],dl
@@skpF:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh



		add		edi,16
		jmp		@@loop16


@@runout:
		add		DWORD PTR[esp+WGTM_XDIF],16
		jle		@@nexline

@@rout:
		mov		bl,[esi+eax]
		or		bl,bl
		jz		@@skpP
		mov		dl,[depthq_table+ebx]
		mov		[edi],dl
@@skpP:	add		ecx,[esp+WGTM_Uadd]
		adc		al,cl
		add		edx,[esp+WGTM_Vadd]
		adc		ah,ch
		add		ebp,[esp+WGTM_Gadd]
		adc		bh,dh
		inc		edi
		dec		DWORD PTR[esp+WGTM_XDIF]
		jnz		@@rout




@@nexline:
		mov		eax,[phd_scrwidth]
		add		DWORD PTR[esp+WGTM_XBUF],32
		add		[esp+WGTM_LPTR],eax
		dec		DWORD PTR[esp+WGTM_YDIF]
		jnz		@@lineloop

@@exit:	add		esp,44
		popad
		ret

_wgtmapA ENDP

;******************************************************************************
_TEXT	ENDS
	END
