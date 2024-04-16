;	.386
	.486	; I only need .386, but I wanted the 486 cycle timings
	.MODEL FLAT, C

;;--- Types ---

PTRBYTE TYPEDEF PTR BYTE
PTRWORD TYPEDEF PTR WORD
PTRDWORD TYPEDEF PTR DWORD
PTRPROC TYPEDEF PTR PROC

;;--- Constants ---

; Width and height of sections in pixels.
SWIDTH equ 8
SHEIGHT equ 8

LOG2_SWIDTH equ 3
LOG2_SHEIGHT equ 3

;;---

EXTERN pal_tbl:BYTE			;unsigned char pal_tbl[3*256];
EXTERN pal15_tbl:WORD		;unsigned short pal15_tbl[256];
;; NextFrame working storage
				; MemRec nf_mem_buf1;
				; MemRec nf_mem_buf2;
EXTERN nf_buf_cur: PTRBYTE	; unsigned char* nf_buf_cur;
EXTERN nf_buf_prv: PTRBYTE	; unsigned char* nf_buf_prv;

;; NextFrame parameters
EXTERN nf_wqty: BYTE		;unsigned char nf_wqty;	 // (width/SWIDTH)
EXTERN nf_hqty: BYTE		;unsigned char nf_hqty;	 // (height/SHEIGHT)
EXTERN nf_fqty: BYTE		;unsigned char nf_fqty;	 // Number of fields
EXTERN nf_hicolor: DWORD	;unsigned nf_hicolor;	 // HiColor (0:none,1:normal,2:swapped)
;;  <derived quantities>
EXTERN nf_width: DWORD		;unsigned nf_width;	 // wqty * SWIDTH
EXTERN nf_height: DWORD		;unsigned nf_height;	 // hqty * SHEIGHT;
EXTERN nf_new_line: DWORD	;unsigned nf_new_line;	 // width - SWIDTH
EXTERN nf_new_row0: DWORD	;unsigned nf_new_row0;	 // SHEIGHT*width*2-width
EXTERN nf_back_right: DWORD	;unsigned nf_back_right; // (SHEIGHT-1)*width

;; Frame parameters
;;  Portion of current frame which has been updated
;;  and needs to be sent to screen.
;;
EXTERN nf_new_x: DWORD		;unsigned nf_new_x;
EXTERN nf_new_y: DWORD		;unsigned nf_new_y;
EXTERN nf_new_w: DWORD		;unsigned nf_new_w;
EXTERN nf_new_h: DWORD		;unsigned nf_new_h;

	.data

	BYTE "(c) 1997 Interplay Productions.  All Rights Reserved.\n"
	BYTE "This file is confidential and consists of proprietary information\n"
	BYTE "of Interplay Productions.  This file and associated libraries\n"
	BYTE "may not, in whole or in part, be disclosed to third parties,\n"
	BYTE "incorporated into any software product which is not being created\n"
	BYTE "for Interplay Productions, copied or duplicated in any form,\n"
	BYTE "without the prior written permission of Interplay Productions.\n"
	BYTE "Further, you may not reverse engineer, decompile or otherwise\n"
	BYTE "attempt to derive source code of this material.\n",0

	.code


NF_DECOMP_INIT MACRO HI_COLOR_FLAG: REQ

	mov	ax, ds			; Insure es==ds for symantec flat mode
	mov	es, ax

	mov	eax, nf_buf_prv		; DiffBufPtrs = nf_buf_prv - nf_buf_cur
	sub	eax, nf_buf_cur
	mov	DiffBufPtrs, eax

	xor	ebx, ebx		; ebx = nf_fqty (convert to 32-bits)
	mov	bl, nf_fqty

	mov	eax, x			; nf_new_x = x*SWIDTH*2^HI_COLOR_FLAG;
	shl	eax, LOG2_SWIDTH+HI_COLOR_FLAG
	mov	nf_new_x, eax

	mov	eax, w			; nf_new_w = w*SWIDTH*2^HI_COLOR_FLAG;
	shl	eax, LOG2_SWIDTH+HI_COLOR_FLAG
	mov	nf_new_w, eax

	mov	eax, y			; nf_new_y = y*nf_fqty*SHEIGHT;
	shl	eax, LOG2_SHEIGHT
	mul	ebx	;nf_fqty
	mov	nf_new_y, eax

	mov	eax, h			; nf_new_h = h*nf_fqty*SHEIGHT;
	shl	eax, LOG2_SHEIGHT
	mul	ebx	;nf_fqty
	mov	nf_new_h, eax

	mov	eax, nf_new_row0	; new_row = nf_new_row0 - nf_new_w;
	sub	eax, nf_new_w
	mov	new_row, eax

	;; Move to correct place in current buffer
	mov	eax, nf_buf_cur		; tbuf = nf_buf_cur
	mov	tbuf, eax
	.if x || y			; if (x||y)
	  mov	eax, nf_new_y		;  tbuf += nf_new_y*nf_width + nf_new_x;
	  mul	nf_width
	  add	eax, nf_new_x
	  add	tbuf, eax
	.endif

	ENDM	; DECOMP_INIT

DECOMP_BODY MACRO HI_COLOR_FLAG:REQ

	LOCAL HI_COLOR_SCALE
	HI_COLOR_SCALE equ HI_COLOR_FLAG+1
	NF_DECOMP_INIT HI_COLOR_FLAG

	mov	eax, w			; parms_sz = (w*h*nf_fqty)<<1
	mul	h
	mul	ebx	;nf_fqty
	shl	eax, 1
	mov	parms_sz, eax

	; esi indexes comp (to get new section data)
	; edi indexes current screen buffer
	; edx is a frequently used constant
	; ebx indexes section params
	mov	edi, tbuf
	mov	edx, nf_new_line	; width - SWIDTH
	mov	ebx, comp		; Parms index
	mov	esi, ebx
	add	esi, parms_sz		; Skip over flags (w*h*2)

	; Iterate over params and copy new hires data to appropriate sections.
	mov	cl, nf_fqty
ns_0f:	push	ecx
	push	edi
	mov	ch, byte ptr h
ns_0:	mov	cl, byte ptr w
ns_1:	cmp	word ptr [ebx],0
	je	ns_10
	add	edi, SWIDTH*HI_COLOR_SCALE
ns_2:	add	ebx, 2
	dec	cl
	jnz	ns_1
	add	edi, new_row		; SHEIGHT*width - SWIDTH*w
	dec	ch
	jnz	ns_0
	pop	edi
	pop	ecx
	add	edi, nf_width
	dec	cl
	jnz	ns_0f
	jmp	ns_99

	; Copy new data to one section
	; Enter with esi pointing to source data, edi to screen section.

	; Assumes SWIDTH=8 (16-bit data) and SHEIGHT=8
ns_10:
	REPEAT 7
	  REPEAT 2*HI_COLOR_SCALE
	    movsd
	  ENDM
	  add	edi, edx
	ENDM
	REPEAT 2*HI_COLOR_SCALE
	    movsd
	ENDM

	sub	edi, nf_back_right	; (SHEIGHT-1)*width
	jmp	ns_2

ns_99:

	; Iterate over flags and motion source addresses from params
	;  to determine which sections to move.
	; ebx indexes params.
	; esi indexes source from buffer
	;   esi will be computed as +- 16K relative to edi.

	sub	ebx, parms_sz	; Move back to start of section parms
	mov	edi, tbuf
	mov	cl, nf_fqty
	xor	esi, esi
ms_0f:	push	ecx
	push	edi
	mov	ch, byte ptr h
ms_0:	mov	cl, byte ptr w
ms_1:	or	si, [ebx]
	jg	ms_10
	jl	ms_j30
	add	edi, SWIDTH*HI_COLOR_SCALE
ms_2:	add	ebx, 2
	dec	cl
	jnz	ms_1
	add	edi, new_row	; SHEIGHT*width - SWIDTH*w
	dec	ch
	jnz	ms_0
	pop	edi
	pop	ecx
	add	edi, nf_width
	dec	cl
	jnz	ms_0f
	jmp	ms_99

ms_j30:	jmp	ms_30

	; Move one section from current screen to current screen.
	; Enter with
	;  edi pointing to destination screen section,
	;  relative value of source offset in esi.

	; The following assumes SWIDTH==8 and SHEIGHT==8

ms_10:	; Make esi absolute
	lea	esi, [esi*HI_COLOR_SCALE-04000h*HI_COLOR_SCALE+edi]
	REPEAT 7
	  REPEAT 2*HI_COLOR_SCALE
	    movsd
	  ENDM
	  add	esi, edx
	  add	edi, edx
	ENDM
	REPEAT 2*HI_COLOR_SCALE
	    movsd
	ENDM

	sub	edi, nf_back_right	; (SHEIGHT-1)*width
	xor	esi, esi		; Reset esi to zero
	jmp	ms_2


ms_20f:	push	ecx
	push	edi
	mov	ch, byte ptr h
ms_20:	mov	cl, byte ptr w
ms_21:	or	si, [ebx]
	jl	ms_30
	jg	ms_j10
	add	edi, SWIDTH*HI_COLOR_SCALE
ms_22:	add	ebx, 2
	dec	cl
	jnz	ms_21
	add	edi, new_row		; SHEIGHT*width - SWIDTH*w
	dec	ch
	jnz	ms_20
	pop	edi
	pop	ecx
	add	edi, nf_width
	dec	cl
	jnz	ms_20f
	jmp	ms_99

ms_j10:	jmp	ms_10

	; Move one section from previous screen to current screen.
	; Enter with
	;  edi pointing to destination screen section,
	;  relative value of source offset in esi.

	; The following assumes SWIDTH==8 and SHEIGHT==8

ms_30:	; Make esi absolute
	lea	esi, [esi*HI_COLOR_SCALE-0C000h*HI_COLOR_SCALE+edi]
	add	esi, DiffBufPtrs		;  and point to other buffer

	REPEAT 7
	  REPEAT 2*HI_COLOR_SCALE
	    movsd
	  ENDM
	  add	esi, edx
	  add	edi, edx
	ENDM
	REPEAT 2*HI_COLOR_SCALE
	    movsd
	ENDM

	sub	edi, nf_back_right	; (SHEIGHT-1)*width
	xor	esi, esi		; Reset esi to zero
	jmp	ms_22

ms_99:
	ENDM		; DECOMP_BODY

; Non-HiColor versions

; Decompress into subsection of current buffer specified
;  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
;
;void nfDecomp(unsigned char *comp,
;	       unsigned x, unsigned y, unsigned w, unsigned h)
;
nfDecomp PROC USES ESI EDI EBX,	comp:PTRBYTE, x:DWORD, y:DWORD, w:DWORD, h:DWORD
	LOCAL tbuf: PTRBYTE
	LOCAL new_row: DWORD
	LOCAL DiffBufPtrs: DWORD
	LOCAL parms_sz: DWORD
		     
	.if nf_hicolor
	  INVOKE nfHiColorDecomp, comp,x,y,w,h
	  ret
	.endif

	DECOMP_BODY 0 ; Not HiColor

	ret
nfDecomp ENDP

; Decompress into subsection of current buffer specified
;  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
;

;void
;nfHiColorDecomp(unsigned char *comp,
;		 unsigned x, unsigned y, unsigned w, unsigned h)
;
nfHiColorDecomp PROC USES ESI EDI EBX,comp:PTRBYTE,x:DWORD, y:DWORD, w:DWORD, h:DWORD
	LOCAL tbuf: PTRBYTE
	LOCAL new_row: DWORD
	LOCAL DiffBufPtrs: DWORD
	LOCAL parms_sz: DWORD
		     
	DECOMP_BODY 1 ; HiColor

	ret
nfHiColorDecomp ENDP

DECOMP_CHG_BODY MACRO HI_COLOR_FLAG:REQ

	LOCAL HI_COLOR_SCALE
	HI_COLOR_SCALE equ HI_COLOR_FLAG+1
	NF_DECOMP_INIT HI_COLOR_FLAG

	; esi indexes comp (to get new section data)
	; edi indexes current screen buffer
	; edx is a frequently used constant
	; ebx indexes section params
	mov	edi, tbuf
	mov	edx, nf_new_line	; width - SWIDTH
	mov	esi, comp
	mov	ebx, parms

	; Iterate over params and copy new hires data to appropriate sections.

	mov	eax, chgs
	mov	pChgs, eax
	mov	eax, 0
	mov	cl, nf_fqty
ns_0f:	push	ecx
	push	edi
	mov	ch, byte ptr h
ns_0:	mov	cl, byte ptr w
ns_1:	add	ax, ax
	ja	ns_1b
	jz	ns_5
	cmp	word ptr [ebx],0
	je	ns_10
	add	ebx, 2
ns_1b:	add	edi, SWIDTH*HI_COLOR_SCALE
ns_2:	dec	cl
	jnz	ns_1
	add	edi, new_row	; SHEIGHT*width - SWIDTH*w
	dec	ch
	jnz	ns_0
	pop	edi
	pop	ecx
	add	edi, nf_width
	dec	cl
	jnz	ns_0f
	jmp	ns_99

ns_5:	mov	eax, pChgs
	add	pChgs, 2
	mov	ax, [eax]
	jmp	ns_1

	; Copy new data to one section
	; Enter with ds:si pointing to source data, es:di to screen section.

	; Assumes SWIDTH=8 (16-bit data) and SHEIGHT=8
ns_10:
	REPEAT 7
	  REPEAT 2*HI_COLOR_SCALE
	    movsd
	  ENDM
	  add	edi, edx
	ENDM
	REPEAT 2*HI_COLOR_SCALE
	    movsd
	ENDM

	sub	edi, nf_back_right	; (SHEIGHT-1)*width
	add	ebx, 2
	jmp	ns_2

ns_99:

	; Iterate over flags and motion source addresses from params
	;  to determine which sections to move.
	; ebx indexes params.
	; esi indexes source from buffer
	;   esi will be computed as +- 16K relative to edi.

	mov	edi, tbuf
	mov	ebx, parms

	mov	eax, chgs
	mov	pChgs, eax
	mov	eax, 0
	mov	cl, byte ptr nf_fqty
	xor	esi, esi
ms_0f:	push	ecx
	push	edi
	mov	ch, byte ptr h
ms_0:	mov	cl, byte ptr w
ms_1:	add	ax, ax
	ja	ms_1b
	jz	ms_5
	or	si, [ebx]
	jg	ms_10
	jl	ms_j30
	add	ebx, 2
ms_1b:	add	edi, SWIDTH*HI_COLOR_SCALE
ms_2:	dec	cl
	jnz	ms_1
	add	edi, new_row		; SHEIGHT*width - SWIDTH*w
	dec	ch
	jnz	ms_0
	pop	edi
	pop	ecx
	add	edi, nf_width
	dec	cl
	jnz	ms_0f
	jmp	ms_99

ms_5:	mov	eax, pChgs
	add	pChgs, 2
	mov	ax, word ptr [eax]
	jmp	ms_1
	

ms_j30:	jmp	ms_30

	; Move one section from current screen to current screen.
	; Enter with
	;  edi pointing to destination screen section,
	;  relative value of source offset in esi.

	; The following assumes SWIDTH==8 and SHEIGHT==8

ms_10:	; Make esi absolute
	lea	esi, [esi*HI_COLOR_SCALE-04000h*HI_COLOR_SCALE+edi]
	REPEAT 7
	  REPEAT 2*HI_COLOR_SCALE
	    movsd
	  ENDM
	  add	esi, edx
	  add	edi, edx
	ENDM
	REPEAT 2*HI_COLOR_SCALE
	    movsd
	ENDM

	sub	edi, nf_back_right	; (SHEIGHT-1)*width
	xor	esi, esi		; Reset esi to zero
	add	ebx, 2
	jmp	ms_2


ms_20f:	push	ecx
	push	edi
	mov	ch, byte ptr h
ms_20:	mov	cl, byte ptr w
ms_21:	add	ax, ax
	ja	ms_21b
	jz	ms_25
	or	si, [ebx]
	jl	ms_30
	jg	ms_j10
	add	ebx, 2
ms_21b:	add	edi, SWIDTH*HI_COLOR_SCALE
ms_22:	dec	cl
	jnz	ms_21
	add	edi, new_row		; SHEIGHT*width - SWIDTH*w
	dec	ch
	jnz	ms_20
	pop	edi
	pop	ecx
	add	edi, nf_width
	dec	cl
	jnz	ms_20f
	jmp	ms_99

ms_25:	mov	eax, pChgs
	add	pChgs, 2
	mov	ax, [eax]
	jmp	ms_21

ms_j10:	jmp	ms_10

	; Move one section from previous screen to current screen.
	; Enter with
	;  edi pointing to destination screen section,
	;  relative value of source offset in esi.

	; The following assumes SWIDTH==8 and SHEIGHT==8

ms_30:	; Make esi absolute
	lea	esi, [esi*HI_COLOR_SCALE-0C000h*HI_COLOR_SCALE+edi]
	add	esi, DiffBufPtrs		;  and point to other buffer

	REPEAT 7
	  REPEAT 2*HI_COLOR_SCALE
	    movsd
	  ENDM
	  add	esi, edx
	  add	edi, edx
	ENDM
	REPEAT 2*HI_COLOR_SCALE
	    movsd
	ENDM

	sub	edi, nf_back_right	; (SHEIGHT-1)*width
	add	ebx, 2
	xor	esi, esi		; Reset esi to zero
	jmp	ms_22

ms_99:
	ENDM	; DECOMP_CHG_BODY


; Decompress into subsection of current buffer specified
;  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
; Chgs specifies which squares to update.
; Parms are motion parms for squares to update.
;
;void
;nfDecompChg(unsigned short *chgs,
;			unsigned short *parms,
;			unsigned char *comp,
;		        unsigned x, unsigned y, unsigned w, unsigned h)
;
nfDecompChg PROC USES ESI EDI EBX,chgs:PTRWORD,	parms:PTRWORD,comp:PTRBYTE,x:DWORD, y:DWORD, w:DWORD, h:DWORD
	LOCAL tbuf: PTRBYTE
	LOCAL new_row: DWORD
	LOCAL DiffBufPtrs: DWORD
	LOCAL pChgs: PTRBYTE

	.if nf_hicolor
	  INVOKE nfHiColorDecompChg, chgs,parms,comp,x,y,w,h
	  ret
	.endif

	DECOMP_CHG_BODY 0 ; Not HiColor

	ret
nfDecompChg ENDP

; Decompress into subsection of current buffer specified
;  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
; Chgs specifies which squares to update.
; Parms are motion parms for squares to update.
;
;void
;nfHiColorDecompChg(unsigned short *chgs,
;			unsigned short *parms,
;			unsigned char *comp,
;		        unsigned x, unsigned y, unsigned w, unsigned h)
;
nfHiColorDecompChg PROC USES ESI EDI EBX,chgs:PTRWORD,parms:PTRWORD,comp:PTRBYTE,x:DWORD, y:DWORD, w:DWORD, h:DWORD
	LOCAL tbuf: PTRBYTE
	LOCAL new_row: DWORD
	LOCAL DiffBufPtrs: DWORD
	LOCAL pChgs: PTRBYTE

	DECOMP_CHG_BODY 1 ; HiColor
	ret
nfHiColorDecompChg ENDP


.data

; luminace table for palette entries
lum_tbl		DWORD 256 DUP (0)

; signed 8-bit y * nf_width
nfpk_ShiftY	DWORD 256 DUP (0)

; Constant tables

; 8-bit -8:7 x nf_width +  -8:7
nfpk_ShiftP1	LABEL WORD
FOR y, <-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7>
  FOR x, <-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7>
		BYTE x,y
  ENDM
ENDM

; 8-bit to right and below in roughly 0:14*nf_width + -14:14 (-3 cases)
; negative is
;  8-bit to left and above in roughly -14:0*nf_width + -14:14 (-3 cases)
nfpk_ShiftP2	LABEL WORD
FOR y, <0,1,2,3,4,5,6,7>
  FOR x, <8,9,10,11,12,13,14>
		BYTE x,y
  ENDM
ENDM
FOR y, <8,9,10,11,12,13>
  FOR x, <-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1>
		BYTE x,y
  ENDM
  FOR x, <0,1,2,3,4,5,6,7,8,9,10,11,12,13,14>
		BYTE x,y
  ENDM
ENDM
FOR x, <-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1>
		BYTE x,14
ENDM
FOR x, <0,1,2,3,4,5,6,7,8,9,10,11>
		BYTE x,14
ENDM

nfpk_mov4l	LABEL DWORD
;		mov ax, bx,cx
MOV4L_REGS	TEXTEQU <!<0c0h+3,0c0h+1!>>
%FOR m4, MOV4L_REGS
% FOR m3, MOV4L_REGS
%  FOR m2, MOV4L_REGS
%   FOR m1, MOV4L_REGS
	BYTE m2,m1,m4,m3
    ENDM
   ENDM
  ENDM
 ENDM

nfpk_mov8	LABEL DWORD
;		mov ax,	bx/dx/cx/bp
MOV8_REGS	TEXTEQU <!<0c0h+3,0c0h+2,0c0h+1,0c0h+5!>>
%FOR m4, MOV8_REGS
% FOR m3, MOV8_REGS
%   FOR m2, MOV8_REGS
%     FOR m1, MOV8_REGS
	BYTE m2,m1,m4,m3
      ENDM
    ENDM
  ENDM
 ENDM

nfpk_mov4	LABEL DWORD
;		mov al, bl/bh/cl/ch
MOV4_REGS0	TEXTEQU <!<0c0h+3,0c0h+7,0c0h+1,0c0h+5!>>
;		mov ah, bl/bh/cl/ch
MOV4_REGS1	TEXTEQU <!<0e0h+3,0e0h+7,0e0h+1,0e0h+5!>>
%FOR m4, MOV4_REGS1
% FOR m3, MOV4_REGS0
%   FOR m2, MOV4_REGS1
%     FOR m1, MOV4_REGS0
	BYTE m3,m4,m1,m2
      ENDM
    ENDM
  ENDM
 ENDM

.code

; nfPkConfig initializes tables used by nfPkDecomp
;  which are dependent on screen size.
nfPkConfig PROC USES ESI EDI EBX

	; Build ShiftY table
	;
	lea	edi, nfpk_ShiftY
	mov	ebx, nf_width

	mov	eax, 0
	mov	ecx, 128
lp1:	mov	[edi], eax
	add	edi,4
	add	eax,ebx
	dec	ecx
	jne	lp1

	mov	eax, ebx
	shl	eax, 7
	neg	eax
	mov	ecx, 128
lp2:	mov	[edi], eax
	add	edi,4
	add	eax,ebx
	dec	ecx
	jne	lp2

	ret
nfPkConfig ENDP



EXTERN sf_LineWidth: DWORD	;unsigned sf_LineWidth;	// Distance between lines in memory

; Banked screen parameters
EXTERN sf_SetBank: PTRPROC	;unsigned long sf_SetBank;
EXTERN sf_WinGran: DWORD	;unsigned sf_WinGran;
EXTERN sf_WinSize: DWORD	;unsigned long sf_WinSize;
EXTERN sf_WinGranPerSize: DWORD	;unsigned sf_WinGranPerSize;
;{sf_WriteWinPtr and sf_WriteWinLimit replace sf_WriteWinSeg, see mveliba.asm}
EXTERN sf_WriteWinPtr: PTRBYTE	;unsigned char *sf_WriteWinPtr;
EXTERN sf_WriteWinLimit: PTRBYTE ;unsigned char *WriteWinLimit;
EXTERN sf_WriteWin: DWORD	;unsigned sf_WriteWin;

EXTERN opt_hscale_step: DWORD
EXTERN opt_hscale_adj: DWORD


;void mve_ShowFrameField(
;	      unsigned char *buf, unsigned bufw, unsigned bufh,
;	      unsigned sx, unsigned sy, unsigned w, unsigned h,
;	      unsigned dstx, unsigned dsty, unsigned field)

mve_ShowFrameField PROC USES ESI EDI EBX, buf:PTRBYTE, bufw:DWORD, bufh:DWORD, sx:DWORD, sy:DWORD, w:DWORD, h:DWORD, dstx:DWORD, dsty:DWORD, field:DWORD
	LOCAL bank:DWORD
	LOCAL w4:DWORD
	LOCAL new_src_line:DWORD
	LOCAL linestep:DWORD
	LOCAL new_dst_line:DWORD
    
	mov	ax, ds			; Insure es==ds for symantec flat mode
	mov	es, ax

	mov	eax, w			; w4 = w>>2
	shr	eax, 2
	mov	w4, eax

;;; <WIP>
;;; In stretched width mode, we either keep 4/5 (a) of the source pixels,
;;;  or duplicate every fourth pixel to magnify by 5/4 (b).
;;;  In these cases, new_src_line is either bufw-w*5/4 (a) or bufw-w*4/5 (b).
;;; Let ScaleStep be 5 (a) or 3 (b) instead of 4.  This is the amount to advance
;;;  the source after copying 32-bits from source to destination.
;;; The coordinate system used for the source will be a simulated scaled system.
;;; Rather than scale height, I plan to use alternate vertical resolutions.  However,
;;;  it might be a good idea to also provide for scaled height in case we want a
;;;  higher resolution border.
;;; Question: Do we still need to support transferring subrectangles?

	.if	opt_hscale_step==4
	mov	eax, bufw		; new_src_line = bufw - w
	sub	eax, w
	mov	new_src_line, eax
	.else
	mov	eax, opt_hscale_adj
	mov	new_src_line, eax
	.endif
	
	mov	eax, sf_LineWidth	; linestep = sf_LineWidth<<1;
	.if field			; if (field)
	  add	eax, eax		;  linestep <<= 1;
	.endif
	mov	linestep, eax

	sub	eax, w			; new_dst_line = linestep - w;
	mov	new_dst_line, eax

	mov	eax, sy			; buf += sy*bufw + sx
	mul	bufw
	add	eax, sx
	add	buf, eax

	mov	eax, sx			; dstx += sx
	add	dstx, eax

	; <WIP> This is a hack.  We should pass in src x,y of origin
	;  or make dstx/dsty absolute.
	;
	mov	eax, bufw		; if (field && sx >= (bufw>>1)
	shr	eax, 1
	.if field && sx >= eax
	  sub dstx, eax			;  dstx -= bufw>>1
	.endif

	mov	eax, sy			; dsty += sy
	add	dsty, eax

	.if sf_SetBank==0	;------------------


	; dst = WriteWinPtr + (dsty*linestep+dstx)
	mov	edi, sf_WriteWinPtr
	mov	eax, dsty
	mul	linestep
	add	eax, dstx
	add	edi, eax

	.if field & 1
	  add edi, sf_LineWidth;
	.endif

	mov	eax, new_src_line
	mov	edx, new_dst_line
	mov	esi, buf
	mov	ebx, h

	.if opt_hscale_step==3
	sub	edi, 8
sf_lp2a:mov	ecx, w4
	shr	ecx, 2
	ALIGN 4
sf_lp2b:mov	eax, [esi]
	mov	[edi+8], eax
	mov	eax, [esi+3]
	mov	[edi+12], eax
	add	edi, 16
	mov	eax, [esi+6]
	mov	[edi], eax
	mov	eax, [esi+9]
	mov	[edi+4], eax
	add	esi, 12
	dec	ecx
	jnz	sf_lp2b
	; To avoid problem of last pixel coming from next line
	; with arrange for w%16==12, so here is where we copy
	; last 12 pixels.
	mov	eax, [esi]
	mov	[edi+8], eax
	mov	eax, [esi+3]
	mov	[edi+12], eax
	add	edi, 12
	mov	eax, [esi+6]
	mov	[edi+4], eax
	add	esi, 9
	add	esi, new_src_line
	add	edi, edx
	dec	ebx
	jnz	sf_lp2a
	add	edi, 8
	.else
sf_lp:	mov	ecx, w4 ;width/4
	rep movsd
	add	esi, eax
	add	edi, edx
	dec	ebx
	jnz	sf_lp
	.endif

	.else ; sf_SetBank	;------------------


	mov	esi, buf

	; start = dsty * linestep + dstx

	mov	eax, linestep
	mul	dsty
	.if field & 1
	 add	eax, sf_LineWidth
	.endif
	add	eax, dstx
	;  bank = start / WinGran
	;  dst = (start % WinGran) + sf_WriteWinPtr
	mov	edx, 0
	div	sf_WinGran
	mov	bank, eax
	mov	edi, edx
	add	edi, sf_WriteWinPtr

	; Select new bank
	mov	bh, 0
	mov	bl, byte ptr sf_WriteWin
	mov	edx, bank
	call	sf_SetBank
	; eax/edx destroyed by sf_SetBank

sf_0:	; rem = sf_WriteWinLimit - dst
	mov	eax, sf_WriteWinLimit
	sub	eax, edi
	; h2 = (rem+(LineWidth-w))/LineWidth
	add	eax, linestep
	sub	eax, w
	mov	edx, 0
	div	linestep
	; if (h<h2) h2=h
	cmp	h, eax
	jae	skplim
	mov	eax, h
skplim: ; if (h2==0) // No full lines can be transfered.
	or	eax, eax
	jz	sf_2
	; h-= h2
	; Transfer h2 lines to screen
	sub	h, eax
	mov	ebx, new_src_line
	mov	edx, new_dst_line
	.if opt_hscale_step==3
	sub	edi, 8
sf_1a3:	mov	ecx, w4
	shr	ecx, 2
	ALIGN 4
sf_1b3:	mov	ebx, [esi]
	mov	[edi+8], ebx
	mov	ebx, [esi+3]
	mov	[edi+12], ebx
	add	edi, 16
	mov	ebx, [esi+6]
	mov	[edi], ebx
	mov	ebx, [esi+9]
	mov	[edi+4], ebx
	add	esi, 12
	dec	ecx
	jnz	sf_1b3
	; To avoid problem of last pixel coming from next line
	; with arrange for w%16==12, so here is where we copy
	; last 12 pixels.
	mov	ebx, [esi]
	mov	[edi+8], ebx
	mov	ebx, [esi+3]
	mov	[edi+12], ebx
	add	edi, 12
	mov	ebx, [esi+6]
	mov	[edi+4], ebx
	add	esi, 9
	add	esi, new_src_line
	add	edi, edx
	dec	eax
	jnz	sf_1a3
	add	edi, 8
	.else
sf_1:	mov	ecx, w4  ; width/4
	rep movsd
	add	esi, ebx
	add	edi, edx
	dec	eax
	jnz	sf_1
	.endif

sf_2:	; if (h!=0) // There are still lines to be transfered
	;	    //	transfer partial line
	or	eax, h
	jz	sf_9

	; w4a = 0 max (sf_WriteWinLimit-dst)/4
	mov	ecx, sf_WriteWinLimit
	sub	ecx, edi
	sar	ecx, 2
	jns	sf_2b
	mov	ecx, 0
sf_2b:	push	ecx		; Save size of first half
	.if opt_hscale_step==3
	
	ALIGN 4
	or	ecx, ecx
	jz	sf_2c3
sf_2b3:	mov	eax, [esi]
	mov	[edi], eax
	add	esi, 3
	add	edi, 4
	dec	ecx
	jnz	sf_2b3
sf_2c3:

	.else
	rep movsd
	.endif

	; bank += WinSize/WinGran //Assumes WinSize%WinGran==0
	; off -= (WinSize/WinGran)*WinGran == WinSize

	mov	eax, sf_WinGranPerSize
	add	bank, eax
	sub	edi, sf_WinSize

	; Select new bank
	mov	bh, 0
	mov	bl, byte ptr sf_WriteWin
	mov	edx, bank
	call	sf_SetBank
	; eax/edx destroyed by sf_SetBank

	; w4b = w4-w4a // Size of second half
	pop	eax		; Size of first half
	mov	ecx, w4
	sub	ecx, eax	; Size of 2nd half
	.if opt_hscale_step==3
	ALIGN 4
	or	ecx, ecx
	jz	sf_8b3
sf_8a3:	mov	eax, [esi]
	mov	[edi], eax
	add	esi, 3
	add	edi, 4
	dec	ecx
	jnz	sf_8a3
sf_8b3:
	.else
	rep movsd
	.endif
	add	esi, new_src_line
	add	edi, new_dst_line
	;; --h // Count split line
	;; if (h!=0) continue
	dec	h
	jnz	sf_0
sf_9:

	.endif ; sf_SetBank==0	;------------------

	ret

mve_ShowFrameField ENDP

nfHPkDecomp PROC USES ESI EDI EBX, \
		ops:PTRBYTE, comp:PTRBYTE, \
		x:DWORD, y:DWORD, w:DWORD, h:DWORD
	LOCAL tbuf: PTRBYTE
	LOCAL new_row:DWORD
	LOCAL DiffBufPtrs:DWORD

	LOCAL nfpk_back_right: DWORD
	LOCAL wcnt:DWORD
	LOCAL bcomp:PTRBYTE

	LOG_LABEL "StartPkDecomp"

.data
nfhpk_OpTbl label dword
	dword offset nf0	; Prev Same (0)
	dword offset nf1	; No change (and copied to screen) (0)
	dword offset nf2	; Near shift from older part of current buf (1)
	dword offset nf3	; Near shift from newer part of current buf (1)
	dword offset nf4	; Near shift from previous buffer (1)
	dword offset nf5	; Far shift from previous buffer (2)
	dword offset nf6	; Far shift from current buffer (2)
				; [Or if COMPOPS, run of no changes (0)]
	dword offset nf7	;     8x8x1 (10 bytes) or low 4x4x1 (4 bytes)
	dword offset nf8	; 2x2 4x4x1 (16 bytes) or 2x1 4x8x1 (12 bytes) or 1x2 8x4x1 (12 bytes)
	dword offset nf9	;     8x8x2 (20 bytes) or low 4x4x2 (8 bytes) or
				; low 4x8x2 (12 bytes) or low 8x4x2 (12 bytes)
	dword offset nf10	; 2x2 4x4x2 (32 bytes) or 2x1 4x8x2 (24 bytes) or 1x2 4x8x2 (24 bytes)
	dword offset nf11	;     8x8x8 (64 bytes)
	dword offset nf12	; low 4x4x8 (16 bytes)
	dword offset nf13	; 2x2 4x4x0 (ie 2x2x8) (4 bytes)
	dword offset nf14	;     8x8x0 (1 byte)
	dword offset nf15	; mix 8x8x0 (2 bytes)
.code

	NF_DECOMP_INIT	1

	mov	eax, nf_back_right
	sub	eax, SWIDTH*2
	mov	nfpk_back_right, eax

	mov	esi, comp
	mov	edi, tbuf

	xor	eax, eax
	mov	ax, [esi]
	add	eax, esi
	mov	bcomp, eax
	add	esi, 2

nf_StartRow:
	mov	eax, w
	shr	eax, 1
	mov	wcnt,eax
	ALIGN 4
nf_NextPair:
	dec	wcnt
	js	nf_NextRow
	mov	ebx, ops
	mov	al, [ebx]
	inc	ebx
	mov	ops, ebx

	xor	ebx, ebx
	mov	bl, al
	shr	bl, 4
	and	eax, 0Fh
	push	offset nf_NextPair
	push	nfhpk_OpTbl[ebx*4]
	jmp	nfhpk_OpTbl[eax*4]

nf_NextRow:
	add	edi, new_row
	dec	h
	jnz	nf_StartRow
	LOG_LABEL "EndPkDecomp"

	ret

;----------------------------------------
	ALIGN 4
nf0:	; No change from previous buffer
	mov	eax, DiffBufPtrs
	jmp	nf_shift

;----------------------------------------
	ALIGN 4
nf1:	; No change (and copied to screen)
if 0 ;debug
	mov	ebx, 0
	jmp	nf_solid
endif
	add	edi, SWIDTH*2
	retn

;----------------------------------------
	ALIGN 4
nf2:	; Near shift from older part of current buffer
	xor	eax, eax
	mov	ebx, bcomp
	inc	bcomp
	mov	al, [ebx]
	mov	ax, nfpk_ShiftP2[eax*2]
nf_xyc_shift:
	xor	ebx, ebx
	mov	bl, ah
	shl	eax, 24
	sar	eax, 24-1
	add	eax, nfpk_ShiftY[ebx*4]
	jmp	nf_shift

;----------------------------------------
	ALIGN 4
nf3:	; Near shift from newer part of current buffer
	xor	eax, eax
	mov	ebx, bcomp
	inc	bcomp
	mov	al, [ebx]
	mov	ax, nfpk_ShiftP2[eax*2]
	neg	al
	neg	ah
	jmp	nf_xyc_shift

;----------------------------------------
	ALIGN 4
nf4:	; Near shift from previous buffer
	xor	eax, eax
	mov	ebx, bcomp
	inc	bcomp
	mov	al, [ebx]
	mov	ax, nfpk_ShiftP1[eax*2]
	jmp	nf_xyp_shift

;----------------------------------------
	ALIGN 4
nf5:	; Far shift from previous buffer
	mov	ax, [esi]
	add	esi, 2
nf_xyp_shift:
	xor	ebx, ebx
	mov	bl, ah
	shl	eax, 24
	sar	eax, 24-1
	add	eax, nfpk_ShiftY[ebx*4]
	add	eax, DiffBufPtrs
	jmp	nf_shift

;----------------------------------------
	ALIGN 4

nf6:	; Far shift from current buffer
	mov	ax, [esi]
	add	esi, 2
	jmp	nf_xyc_shift

;----------------------------------------
	ALIGN 4
nf_shift:
if 0 ;debug
	mov	ebx, 0
	jmp	nf_solid
endif
	mov	ebx, esi	; save esi
	lea	esi, [edi+eax]
	mov	edx, nf_width

	REPEAT 7
	  mov eax, [esi]
	  mov [edi], eax
	  mov eax, [esi+4]
	  mov [edi+4], eax
	  mov eax, [esi+8]
	  mov [edi+8], eax
	  mov eax, [esi+12]
	  mov [edi+12], eax
	  add	esi, edx
	  add	edi, edx
	ENDM
	mov eax, [esi]
	mov [edi], eax
	mov eax, [esi+4]
	mov [edi+4], eax
	mov eax, [esi+8]
	mov [edi+8], eax
	mov eax, [esi+12]
	mov [edi+12], eax

	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8
	mov	esi, ebx	; restore esi
	retn

;----------------------------------------
	ALIGN 4
nf7:	;     8x8x1 (12 bytes)

	test	word ptr [esi], 08000h
	jnz	nf23

if 0 ;debug
	add	esi, 12
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov8
	lea	edx, byte ptr ds:nf7_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_11-nf7_11)], bl
	mov	[edx+(nf7_12-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_13-nf7_11)], bl
	mov	[edx+(nf7_14-nf7_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_21-nf7_11)], bl
	mov	[edx+(nf7_22-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_23-nf7_11)], bl
	mov	[edx+(nf7_24-nf7_11)], bh

	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_31-nf7_11)], bl
	mov	[edx+(nf7_32-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_33-nf7_11)], bl
	mov	[edx+(nf7_34-nf7_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_41-nf7_11)], bl
	mov	[edx+(nf7_42-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_43-nf7_11)], bl
	mov	[edx+(nf7_44-nf7_11)], bh

	lea	edx, [edx+(nf7_51-nf7_11)]

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_51-nf7_51)], bl
	mov	[edx+(nf7_52-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_53-nf7_51)], bl
	mov	[edx+(nf7_54-nf7_51)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_61-nf7_51)], bl
	mov	[edx+(nf7_62-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_63-nf7_51)], bl
	mov	[edx+(nf7_64-nf7_51)], bh

	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_71-nf7_51)], bl
	mov	[edx+(nf7_72-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_73-nf7_51)], bl
	mov	[edx+(nf7_74-nf7_51)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_81-nf7_51)], bl
	mov	[edx+(nf7_82-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_83-nf7_51)], bl
	mov	[edx+(nf7_84-nf7_51)], bh

	push	ebp
	push	esi
	; load ebx,edx,ecx,ebp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	Trans16	cx, esi+2
	shl	ecx, 16
	Trans16 cx, esi
	mov	esi,nf_width
	mov	edx, ecx

	ror	edx, 16
	mov	ebx, edx
	mov	bx, cx
	mov	ebp, ecx
	mov	bp, dx

	jmp	nf7_0		; flush prefetch
	ALIGN 4
nf7_0:
nf7_11:	mov	[ebp+0], ebx
nf7_12:	mov	[ebp+4], ebx
nf7_13:	mov	[ebp+8], ebx
nf7_14:	mov	[ebp+12], ebx
	add	edi, esi

nf7_21:	mov	[ebp+0], ebx
nf7_22:	mov	[ebp+4], ebx
nf7_23:	mov	[ebp+8], ebx
nf7_24:	mov	[ebp+12], ebx
	add	edi, esi

nf7_31:	mov	[ebp+0], ebx
nf7_32:	mov	[ebp+4], ebx
nf7_33:	mov	[ebp+8], ebx
nf7_34:	mov	[ebp+12], ebx
	add	edi, esi

nf7_41:	mov	[ebp+0], ebx
nf7_42:	mov	[ebp+4], ebx
nf7_43:	mov	[ebp+8], ebx
nf7_44:	mov	[ebp+12], ebx
	add	edi, esi

nf7_51:	mov	[ebp+0], ebx
nf7_52:	mov	[ebp+4], ebx
nf7_53:	mov	[ebp+8], ebx
nf7_54:	mov	[ebp+12], ebx
	add	edi, esi

nf7_61:	mov	[ebp+0], ebx
nf7_62:	mov	[ebp+4], ebx
nf7_63:	mov	[ebp+8], ebx
nf7_64:	mov	[ebp+12], ebx
	add	edi, esi

nf7_71:	mov	[ebp+0], ebx
nf7_72:	mov	[ebp+4], ebx
nf7_73:	mov	[ebp+8], ebx
nf7_74:	mov	[ebp+12], ebx
	add	edi, esi

nf7_81:	mov	[ebp+0], ebx
nf7_82:	mov	[ebp+4], ebx
nf7_83:	mov	[ebp+8], ebx
nf7_84:	mov	[ebp+12], ebx

	pop	esi
	pop	ebp
	add	esi, 12
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf7+16
nf23:	; low 4x4x1 (6 bytes)

if 0 ;debug
	add	esi, 6
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov4l
	lea	edx, byte ptr ds:nf23_11+1

	mov	al, [esi+4]
	and	al, 0fH
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_11-nf23_11)], bl
	mov	[edx+(nf23_12-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_13-nf23_11)], bl
	mov	[edx+(nf23_14-nf23_11)], bh

	mov	al, [esi+4]
	shr	al, 4
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_31-nf23_11)], bl
	mov	[edx+(nf23_32-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_33-nf23_11)], bl
	mov	[edx+(nf23_34-nf23_11)], bh


	mov	al, [esi+5]
	and	al, 0fH
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_51-nf23_11)], bl
	mov	[edx+(nf23_52-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_53-nf23_11)], bl
	mov	[edx+(nf23_54-nf23_11)], bh

	mov	al, [esi+5]
	shr	al, 4
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_71-nf23_11)], bl
	mov	[edx+(nf23_72-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_73-nf23_11)], bl
	mov	[edx+(nf23_74-nf23_11)], bh

	mov	edx, nf_width

	; load ebx,ecx with 00,11 color combinations

	Trans16	cx, esi, 1
	shrd	ebx, ecx, 16
	mov	bx, cx
	Trans16 cx, esi+2
	shrd	eax, ecx, 16
	mov	ax, cx
	mov	ecx, eax

	jmp	nf23_0		; flush prefetch
	ALIGN 4
nf23_0:

nf23_11:mov	eax, ebx
	mov	[edi], eax
	mov	[edi+edx], eax
nf23_12:mov	eax, ebx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
nf23_13:mov	eax, ebx
	mov	[edi+8], eax
	mov	[edi+edx+8], eax
nf23_14:mov	eax, ebx
	mov	[edi+12], eax
	mov	[edi+edx+12], eax
	lea	edi, [edi+edx*2]

nf23_31:mov	eax, ebx
	mov	[edi], eax
	mov	[edi+edx], eax
nf23_32:mov	eax, ebx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
nf23_33:mov	eax, ebx
	mov	[edi+8], eax
	mov	[edi+edx+8], eax
nf23_34:mov	eax, ebx
	mov	[edi+12], eax
	mov	[edi+edx+12], eax
	lea	edi, [edi+edx*2]

nf23_51:mov	eax, ebx
	mov	[edi], eax
	mov	[edi+edx], eax
nf23_52:mov	eax, ebx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
nf23_53:mov	eax, ebx
	mov	[edi+8], eax
	mov	[edi+edx+8], eax
nf23_54:mov	eax, ebx
	mov	[edi+12], eax
	mov	[edi+edx+12], eax
	lea	edi, [edi+edx*2]

nf23_71:mov	eax, ebx
	mov	[edi], eax
	mov	[edi+edx], eax
nf23_72:mov	eax, ebx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
nf23_73:mov	eax, ebx
	mov	[edi+8], eax
	mov	[edi+edx+8], eax
nf23_74:mov	eax, ebx
	mov	[edi+12], eax
	mov	[edi+edx+12], eax
	add	edi, edx

	sub	edi, nfpk_back_right
	add	esi, 6
	retn

;----------------------------------------
	ALIGN 4
nf8:	; 2x2 4x4x1 (24 bytes)

	test	word ptr [esi], 08000h
	jnz	nf24

if 0 ;debug
	add	esi, 24
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov8
	lea	edx, byte ptr ds:nf8_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_11-nf8_11)], bl
	mov	[edx+(nf8_12-nf8_11)], bh
	shr	ebx, 16
	mov	[edx+(nf8_13-nf8_11)], bl
	mov	[edx+(nf8_14-nf8_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_21-nf8_11)], bl
	mov	[edx+(nf8_22-nf8_11)], bh
	shr	ebx, 16
	mov	[edx+(nf8_23-nf8_11)], bl
	mov	[edx+(nf8_24-nf8_11)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_31-nf8_11)], bl
	mov	[edx+(nf8_32-nf8_11)], bh
	shr	ebx, 16
	mov	[edx+(nf8_33-nf8_11)], bl
	mov	[edx+(nf8_34-nf8_11)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_41-nf8_11)], bl
	mov	[edx+(nf8_42-nf8_11)], bh
	shr	ebx, 16
	mov	[edx+(nf8_43-nf8_11)], bl
	mov	[edx+(nf8_44-nf8_11)], bh

	add	edx, nf8_51-nf8_11

	mov	al, [esi+16]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_51-nf8_51)], bl
	mov	[edx+(nf8_52-nf8_51)], bh
	shr	ebx, 16
	mov	[edx+(nf8_53-nf8_51)], bl
	mov	[edx+(nf8_54-nf8_51)], bh

	mov	al, [esi+17]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_61-nf8_51)], bl
	mov	[edx+(nf8_62-nf8_51)], bh
	shr	ebx, 16
	mov	[edx+(nf8_63-nf8_51)], bl
	mov	[edx+(nf8_64-nf8_51)], bh


	mov	al, [esi+22]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_71-nf8_51)], bl
	mov	[edx+(nf8_72-nf8_51)], bh
	shr	ebx, 16
	mov	[edx+(nf8_73-nf8_51)], bl
	mov	[edx+(nf8_74-nf8_51)], bh

	mov	al, [esi+23]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_81-nf8_51)], bl
	mov	[edx+(nf8_82-nf8_51)], bh
	shr	ebx, 16
	mov	[edx+(nf8_83-nf8_51)], bl
	mov	[edx+(nf8_84-nf8_51)], bh


	push	ebp
	push	esi
	; load ebx,edx,ecx,ebp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).

	Trans16 cx, esi+18+2
	shl	ecx, 16
	Trans16 cx, esi+18
	push	ecx

	Trans16 cx, esi+12+2
	shl	ecx, 16
	Trans16 cx, esi+12
	push	ecx

	Trans16 cx, esi+6+2
	shl	ecx, 16
	Trans16 cx, esi+6
	push	ecx

	Trans16	cx, esi+2
	shl	ecx, 16
	Trans16 cx, esi

	mov	esi,nf_width
	mov	edx, ecx
	ror	edx, 16
	mov	ebx, edx
	mov	bx, cx
	mov	ebp, ecx
	mov	bp, dx

	jmp	nf8_0		; flush prefetch
	ALIGN 4
nf8_0:
nf8_11:	mov	[ebp+0], ebx
nf8_12:	mov	[ebp+4], ebx
	add	edi, esi
nf8_13:	mov	[ebp+0], ebx
nf8_14:	mov	[ebp+4], ebx
	add	edi, esi

nf8_21:	mov	[ebp+0], ebx
nf8_22:	mov	[ebp+4], ebx
	add	edi, esi
nf8_23:	mov	[ebp+0], ebx
nf8_24:	mov	[ebp+4], ebx
	add	edi, esi

	pop	ecx
	mov	edx, ecx
	ror	edx, 16
	mov	ebx, edx
	mov	bx, cx
	mov	ebp, ecx
	mov	bp, dx


nf8_31:	mov	[ebp+0], ebx
nf8_32:	mov	[ebp+4], ebx
	add	edi, esi
nf8_33:	mov	[ebp+0], ebx
nf8_34:	mov	[ebp+4], ebx
	add	edi, esi

nf8_41:	mov	[ebp+0], ebx
nf8_42:	mov	[ebp+4], ebx
	add	edi, esi
nf8_43:	mov	[ebp+0], ebx
nf8_44:	mov	[ebp+4], ebx
	add	edi, esi

	lea	eax, [esi*8-8]
	sub	edi, eax

	pop	ecx
	mov	edx, ecx
	ror	edx, 16
	mov	ebx, edx
	mov	bx, cx
	mov	ebp, ecx
	mov	bp, dx

nf8_51:	mov	[ebp+0], ebx
nf8_52:	mov	[ebp+4], ebx
	add	edi, esi
nf8_53:	mov	[ebp+0], ebx
nf8_54:	mov	[ebp+4], ebx
	add	edi, esi

nf8_61:	mov	[ebp+0], ebx
nf8_62:	mov	[ebp+4], ebx
	add	edi, esi
nf8_63:	mov	[ebp+0], ebx
nf8_64:	mov	[ebp+4], ebx
	add	edi, esi

	pop	ecx
	mov	edx, ecx
	ror	edx, 16
	mov	ebx, edx
	mov	bx, cx
	mov	ebp, ecx
	mov	bp, dx

nf8_71:	mov	[ebp+0], ebx
nf8_72:	mov	[ebp+4], ebx
	add	edi, esi
nf8_73:	mov	[ebp+0], ebx
nf8_74:	mov	[ebp+4], ebx
	add	edi, esi

nf8_81:	mov	[ebp+0], ebx
nf8_82:	mov	[ebp+4], ebx
	add	edi, esi
nf8_83:	mov	[ebp+0], ebx
nf8_84:	mov	[ebp+4], ebx

	pop	esi
	pop	ebp
	add	esi, 24
	sub	edi, 8
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf8+16
nf24:	; 2x1 4x8x1 (16 bytes)

	test	word ptr [esi+8], 08000h
	jnz	nf40

if 0 ;debug
	add	esi, 16
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov8
	lea	edx, byte ptr ds:nf24_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_11-nf24_11)], bl
	mov	[edx+(nf24_12-nf24_11)], bh
	shr	ebx, 16
	mov	[edx+(nf24_13-nf24_11)], bl
	mov	[edx+(nf24_14-nf24_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_21-nf24_11)], bl
	mov	[edx+(nf24_22-nf24_11)], bh
	shr	ebx, 16
	mov	[edx+(nf24_23-nf24_11)], bl
	mov	[edx+(nf24_24-nf24_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_31-nf24_11)], bl
	mov	[edx+(nf24_32-nf24_11)], bh
	shr	ebx, 16
	mov	[edx+(nf24_33-nf24_11)], bl
	mov	[edx+(nf24_34-nf24_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_41-nf24_11)], bl
	mov	[edx+(nf24_42-nf24_11)], bh
	shr	ebx, 16
	mov	[edx+(nf24_43-nf24_11)], bl
	mov	[edx+(nf24_44-nf24_11)], bh

	add	edx, nf24_51-nf24_11

	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_51-nf24_51)], bl
	mov	[edx+(nf24_52-nf24_51)], bh
	shr	ebx, 16
	mov	[edx+(nf24_53-nf24_51)], bl
	mov	[edx+(nf24_54-nf24_51)], bh

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_61-nf24_51)], bl
	mov	[edx+(nf24_62-nf24_51)], bh
	shr	ebx, 16
	mov	[edx+(nf24_63-nf24_51)], bl
	mov	[edx+(nf24_64-nf24_51)], bh


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_71-nf24_51)], bl
	mov	[edx+(nf24_72-nf24_51)], bh
	shr	ebx, 16
	mov	[edx+(nf24_73-nf24_51)], bl
	mov	[edx+(nf24_74-nf24_51)], bh

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_81-nf24_51)], bl
	mov	[edx+(nf24_82-nf24_51)], bh
	shr	ebx, 16
	mov	[edx+(nf24_83-nf24_51)], bl
	mov	[edx+(nf24_84-nf24_51)], bh


	push	ebp
	push	esi
	; load ebx,edx,ecx,ebp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).

	Trans16	cx, esi+8+2
	shl	ecx, 16
	Trans16	cx, esi+8
	push	ecx

	Trans16	cx, esi+2
	shl	ecx, 16
	Trans16 cx, esi, 1
	mov	esi,nf_width
	mov	edx, ecx
	ror	edx, 16
	mov	ebx, edx
	mov	bx, cx
	mov	ebp, ecx
	mov	bp, dx

	jmp	nf24_0		; flush prefetch
	ALIGN 4
nf24_0:

nf24_11:mov	[ebp+0], ebx
nf24_12:mov	[ebp+4], ebx
	add	edi, esi
nf24_13:mov	[ebp+0], ebx
nf24_14:mov	[ebp+4], ebx
	add	edi, esi

nf24_21:mov	[ebp+0], ebx
nf24_22:mov	[ebp+4], ebx
	add	edi, esi
nf24_23:mov	[ebp+0], ebx
nf24_24:mov	[ebp+4], ebx
	add	edi, esi

nf24_31:mov	[ebp+0], ebx
nf24_32:mov	[ebp+4], ebx
	add	edi, esi
nf24_33:mov	[ebp+0], ebx
nf24_34:mov	[ebp+4], ebx
	add	edi, esi

nf24_41:mov	[ebp+0], ebx
nf24_42:mov	[ebp+4], ebx
	add	edi, esi
nf24_43:mov	[ebp+0], ebx
nf24_44:mov	[ebp+4], ebx
	add	edi, esi

	lea	eax, [esi*8-8]
	sub	edi, eax

	pop	ecx
	mov	edx, ecx
	ror	edx, 16
	mov	ebx, edx
	mov	bx, cx
	mov	ebp, ecx
	mov	bp, dx

nf24_51:mov	[ebp+0], ebx
nf24_52:mov	[ebp+4], ebx
	add	edi, esi
nf24_53:mov	[ebp+0], ebx
nf24_54:mov	[ebp+4], ebx
	add	edi, esi

nf24_61:mov	[ebp+0], ebx
nf24_62:mov	[ebp+4], ebx
	add	edi, esi
nf24_63:mov	[ebp+0], ebx
nf24_64:mov	[ebp+4], ebx
	add	edi, esi

nf24_71:mov	[ebp+0], ebx
nf24_72:mov	[ebp+4], ebx
	add	edi, esi
nf24_73:mov	[ebp+0], ebx
nf24_74:mov	[ebp+4], ebx
	add	edi, esi

nf24_81:mov	[ebp+0], ebx
nf24_82:mov	[ebp+4], ebx
	add	edi, esi
nf24_83:mov	[ebp+0], ebx
nf24_84:mov	[ebp+4], ebx

	pop	esi
	pop	ebp
	add	esi, 16
	sub	edi, 8
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf8+32
nf40:	; 1x2 8x4x1 (16 bytes)

if 0 ;debug
	add	esi, 16
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov8
	lea	edx, byte ptr ds:nf40_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_11-nf40_11)], bl
	mov	[edx+(nf40_12-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_13-nf40_11)], bl
	mov	[edx+(nf40_14-nf40_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_21-nf40_11)], bl
	mov	[edx+(nf40_22-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_23-nf40_11)], bl
	mov	[edx+(nf40_24-nf40_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_31-nf40_11)], bl
	mov	[edx+(nf40_32-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_33-nf40_11)], bl
	mov	[edx+(nf40_34-nf40_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_41-nf40_11)], bl
	mov	[edx+(nf40_42-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_43-nf40_11)], bl
	mov	[edx+(nf40_44-nf40_11)], bh

	add	edx, nf40_51-nf40_11

	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_51-nf40_51)], bl
	mov	[edx+(nf40_52-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_53-nf40_51)], bl
	mov	[edx+(nf40_54-nf40_51)], bh

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_61-nf40_51)], bl
	mov	[edx+(nf40_62-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_63-nf40_51)], bl
	mov	[edx+(nf40_64-nf40_51)], bh


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_71-nf40_51)], bl
	mov	[edx+(nf40_72-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_73-nf40_51)], bl
	mov	[edx+(nf40_74-nf40_51)], bh

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_81-nf40_51)], bl
	mov	[edx+(nf40_82-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_83-nf40_51)], bl
	mov	[edx+(nf40_84-nf40_51)], bh


	push	ebp
	push	esi
	; load ebx,edx,ecx,ebp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	Trans16	cx, esi+8+2
	shl	ecx, 16
	Trans16 cx, esi+8, 1
	push	ecx

	Trans16	cx, esi+2
	shl	ecx, 16
	Trans16 cx, esi, 1
	mov	esi,nf_width
	mov	edx, ecx
	ror	edx, 16
	mov	ebx, edx
	mov	bx, cx
	mov	ebp, ecx
	mov	bp, dx

	jmp	nf40_0		; flush prefetch
	ALIGN 4
nf40_0:

nf40_11:mov	[ebp+0], ebx
nf40_12:mov	[ebp+4], ebx
nf40_13:mov	[ebp+8], ebx
nf40_14:mov	[ebp+12], ebx
	add	edi, esi

nf40_21:mov	[ebp+0], ebx
nf40_22:mov	[ebp+4], ebx
nf40_23:mov	[ebp+8], ebx
nf40_24:mov	[ebp+12], ebx
	add	edi, esi

nf40_31:mov	[ebp+0], ebx
nf40_32:mov	[ebp+4], ebx
nf40_33:mov	[ebp+8], ebx
nf40_34:mov	[ebp+12], ebx
	add	edi, esi

nf40_41:mov	[ebp+0], ebx
nf40_42:mov	[ebp+4], ebx
nf40_43:mov	[ebp+8], ebx
nf40_44:mov	[ebp+12], ebx
	add	edi, esi

	pop	ecx
	mov	edx, ecx
	ror	edx, 16
	mov	ebx, edx
	mov	bx, cx
	mov	ebp, ecx
	mov	bp, dx

nf40_51:mov	[ebp+0], ebx
nf40_52:mov	[ebp+4], ebx
nf40_53:mov	[ebp+8], ebx
nf40_54:mov	[ebp+12], ebx
	add	edi, esi

nf40_61:mov	[ebp+0], ebx
nf40_62:mov	[ebp+4], ebx
nf40_63:mov	[ebp+8], ebx
nf40_64:mov	[ebp+12], ebx
	add	edi, esi

nf40_71:mov	[ebp+0], ebx
nf40_72:mov	[ebp+4], ebx
nf40_73:mov	[ebp+8], ebx
nf40_74:mov	[ebp+12], ebx
	add	edi, esi

nf40_81:mov	[ebp+0], ebx
nf40_82:mov	[ebp+4], ebx
nf40_83:mov	[ebp+8], ebx
nf40_84:mov	[ebp+12], ebx

	pop	esi
	pop	ebp
	add	esi, 16
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
nf9:	;     8x8x2 (24 bytes)

	test	word ptr [esi], 08000h
	jnz	nf41

	test	word ptr [esi+4], 08000h
	jnz	nf25

if 0 ;debug
	add	esi, 24
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov4
	lea	edx, byte ptr ds:nf9_11+2

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_11-nf9_11)], bh
	mov	[edx+(nf9_12-nf9_11)], bl
	shr	ebx, 16
	mov	[edx+(nf9_13-nf9_11)], bh
	mov	[edx+(nf9_14-nf9_11)], bl

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_15-nf9_11)], bh
	mov	[edx+(nf9_16-nf9_11)], bl
	shr	ebx, 16
	mov	[edx+(nf9_17-nf9_11)], bh
	mov	[edx+(nf9_18-nf9_11)], bl


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_21-nf9_11)], bh
	mov	[edx+(nf9_22-nf9_11)], bl
	shr	ebx, 16
	mov	[edx+(nf9_23-nf9_11)], bh
	mov	[edx+(nf9_24-nf9_11)], bl

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_25-nf9_11)], bh
	mov	[edx+(nf9_26-nf9_11)], bl
	shr	ebx, 16
	mov	[edx+(nf9_27-nf9_11)], bh
	mov	[edx+(nf9_28-nf9_11)], bl


	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_31-nf9_11)], bh
	mov	[edx+(nf9_32-nf9_11)], bl
	shr	ebx, 16
	mov	[edx+(nf9_33-nf9_11)], bh
	mov	[edx+(nf9_34-nf9_11)], bl

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_35-nf9_11)], bh
	mov	[edx+(nf9_36-nf9_11)], bl
	shr	ebx, 16
	mov	[edx+(nf9_37-nf9_11)], bh
	mov	[edx+(nf9_38-nf9_11)], bl


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_41-nf9_11)], bh
	mov	[edx+(nf9_42-nf9_11)], bl
	shr	ebx, 16
	mov	[edx+(nf9_43-nf9_11)], bh
	mov	[edx+(nf9_44-nf9_11)], bl

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_45-nf9_11)], bh
	mov	[edx+(nf9_46-nf9_11)], bl
	shr	ebx, 16
	mov	[edx+(nf9_47-nf9_11)], bh
	mov	[edx+(nf9_48-nf9_11)], bl


	lea	edx, [edx+(nf9_51-nf9_11)]

	mov	al, [esi+16]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_51-nf9_51)], bh
	mov	[edx+(nf9_52-nf9_51)], bl
	shr	ebx, 16
	mov	[edx+(nf9_53-nf9_51)], bh
	mov	[edx+(nf9_54-nf9_51)], bl

	mov	al, [esi+17]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_55-nf9_51)], bh
	mov	[edx+(nf9_56-nf9_51)], bl
	shr	ebx, 16
	mov	[edx+(nf9_57-nf9_51)], bh
	mov	[edx+(nf9_58-nf9_51)], bl


	mov	al, [esi+18]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_61-nf9_51)], bh
	mov	[edx+(nf9_62-nf9_51)], bl
	shr	ebx, 16
	mov	[edx+(nf9_63-nf9_51)], bh
	mov	[edx+(nf9_64-nf9_51)], bl

	mov	al, [esi+19]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_65-nf9_51)], bh
	mov	[edx+(nf9_66-nf9_51)], bl
	shr	ebx, 16
	mov	[edx+(nf9_67-nf9_51)], bh
	mov	[edx+(nf9_68-nf9_51)], bl


	mov	al, [esi+20]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_71-nf9_51)], bh 
	mov	[edx+(nf9_72-nf9_51)], bl
	shr	ebx, 16
	mov	[edx+(nf9_73-nf9_51)], bh
	mov	[edx+(nf9_74-nf9_51)], bl

	mov	al, [esi+21]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_75-nf9_51)], bh
	mov	[edx+(nf9_76-nf9_51)], bl
	shr	ebx, 16
	mov	[edx+(nf9_77-nf9_51)], bh
	mov	[edx+(nf9_78-nf9_51)], bl


	mov	al, [esi+22]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_81-nf9_51)], bh
	mov	[edx+(nf9_82-nf9_51)], bl
	shr	ebx, 16
	mov	[edx+(nf9_83-nf9_51)], bh
	mov	[edx+(nf9_84-nf9_51)], bl

	mov	al, [esi+23]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_85-nf9_51)], bh
	mov	[edx+(nf9_86-nf9_51)], bl
	shr	ebx, 16
	mov	[edx+(nf9_87-nf9_51)], bh
	mov	[edx+(nf9_88-nf9_51)], bl

	push	ebp
	push	esi
	; Load bx,dx,cx,bp with four colors
	Trans16	bx, esi
	Trans16	dx, esi+2
	Trans16 cx, esi+4
	Trans16 bp, esi+6
	mov	esi, nf_width

	jmp	nf9_0		; flush prefetch
	ALIGN 4
nf9_0:

nf9_11:	mov	ax, bx
	shl	eax, 16
nf9_12:	mov	ax, bx
	mov	[edi], eax
nf9_13:	mov	ax, bx
	shl	eax, 16
nf9_14:	mov	ax, bx
	mov	[edi+4], eax
nf9_15:	mov	ax, bx
	shl	eax, 16
nf9_16:	mov	ax, bx
	mov	[edi+8], eax
nf9_17:	mov	ax, bx
	shl	eax, 16
nf9_18:	mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf9_21:	mov	ax, bx
	shl	eax, 16
nf9_22:	mov	ax, bx
	mov	[edi], eax
nf9_23:	mov	ax, bx
	shl	eax, 16
nf9_24:	mov	ax, bx
	mov	[edi+4], eax
nf9_25:	mov	ax, bx
	shl	eax, 16
nf9_26:	mov	ax, bx
	mov	[edi+8], eax
nf9_27:	mov	ax, bx
	shl	eax, 16
nf9_28:	mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf9_31:	mov	ax, bx
	shl	eax, 16
nf9_32:	mov	ax, bx
	mov	[edi], eax
nf9_33:	mov	ax, bx
	shl	eax, 16
nf9_34:	mov	ax, bx
	mov	[edi+4], eax
nf9_35:	mov	ax, bx
	shl	eax, 16
nf9_36:	mov	ax, bx
	mov	[edi+8], eax
nf9_37:	mov	ax, bx
	shl	eax, 16
nf9_38:	mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf9_41:	mov	ax, bx
	shl	eax, 16
nf9_42:	mov	ax, bx
	mov	[edi], eax
nf9_43:	mov	ax, bx
	shl	eax, 16
nf9_44:	mov	ax, bx
	mov	[edi+4], eax
nf9_45:	mov	ax, bx
	shl	eax, 16
nf9_46:	mov	ax, bx
	mov	[edi+8], eax
nf9_47:	mov	ax, bx
	shl	eax, 16
nf9_48:	mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf9_51:	mov	ax, bx
	shl	eax, 16
nf9_52:	mov	ax, bx
	mov	[edi], eax
nf9_53:	mov	ax, bx
	shl	eax, 16
nf9_54:	mov	ax, bx
	mov	[edi+4], eax
nf9_55:	mov	ax, bx
	shl	eax, 16
nf9_56:	mov	ax, bx
	mov	[edi+8], eax
nf9_57:	mov	ax, bx
	shl	eax, 16
nf9_58:	mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf9_61:	mov	ax, bx
	shl	eax, 16
nf9_62:	mov	ax, bx
	mov	[edi], eax
nf9_63:	mov	ax, bx
	shl	eax, 16
nf9_64:	mov	ax, bx
	mov	[edi+4], eax
nf9_65:	mov	ax, bx
	shl	eax, 16
nf9_66:	mov	ax, bx
	mov	[edi+8], eax
nf9_67:	mov	ax, bx
	shl	eax, 16
nf9_68:	mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf9_71:	mov	ax, bx
	shl	eax, 16
nf9_72:	mov	ax, bx
	mov	[edi], eax
nf9_73:	mov	ax, bx
	shl	eax, 16
nf9_74:	mov	ax, bx
	mov	[edi+4], eax
nf9_75:	mov	ax, bx
	shl	eax, 16
nf9_76:	mov	ax, bx
	mov	[edi+8], eax
nf9_77:	mov	ax, bx
	shl	eax, 16
nf9_78:	mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf9_81:	mov	ax, bx
	shl	eax, 16
nf9_82:	mov	ax, bx
	mov	[edi], eax
nf9_83:	mov	ax, bx
	shl	eax, 16
nf9_84:	mov	ax, bx
	mov	[edi+4], eax
nf9_85:	mov	ax, bx
	shl	eax, 16
nf9_86:	mov	ax, bx
	mov	[edi+8], eax
nf9_87:	mov	ax, bx
	shl	eax, 16
nf9_88:	mov	ax, bx
	mov	[edi+12], eax

	pop	esi
	pop	ebp
	add	esi, 24
	sub	edi, nfpk_back_right
	retn

;----------------------------------------
	ALIGN 4
;nf9+16
nf25:	; low 4x4x2 (12 bytes)

if 0 ;debug
	add	esi, 12
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov4
	lea	edx, byte ptr ds:nf25_11+1

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_11-nf25_11)], bl
	mov	[edx+(nf25_12-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_13-nf25_11)], bl
	mov	[edx+(nf25_14-nf25_11)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_21-nf25_11)], bl
	mov	[edx+(nf25_22-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_23-nf25_11)], bl
	mov	[edx+(nf25_24-nf25_11)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_31-nf25_11)], bl
	mov	[edx+(nf25_32-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_33-nf25_11)], bl
	mov	[edx+(nf25_34-nf25_11)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_41-nf25_11)], bl
	mov	[edx+(nf25_42-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_43-nf25_11)], bl
	mov	[edx+(nf25_44-nf25_11)], bh

	push	ebp
	push	esi
	; Load ebx,edx,ecx,ebp with four colors, duplicated in high order.
	Trans16	cx, esi
	shrd	ebx, ecx, 16
	mov	bx, cx
	Trans16	cx, esi+2
	shrd	edx, ecx, 16
	mov	dx, cx
	Trans16	cx, esi+4, 1
	shrd	eax, ecx, 16
	mov	ax, cx
	push	eax
	Trans16	cx, esi+6
	shrd	ebp, ecx, 16
	mov	bp, cx
	pop	ecx
	mov	esi, nf_width

	jmp	nf25_0		; flush prefetch
	ALIGN 4
nf25_0:

nf25_11:mov	eax, ebx
	mov	[edi], eax
	mov	[edi+esi], eax
nf25_12:mov	eax, ebx
	mov	[edi+4], eax
	mov	[edi+esi+4], eax
nf25_13:mov	eax, ebx
	mov	[edi+8], eax
	mov	[edi+esi+8], eax
nf25_14:mov	eax, ebx
	mov	[edi+12], eax
	mov	[edi+esi+12], eax
	lea	edi, [edi+esi*2]

nf25_21:mov	eax, ebx
	mov	[edi], eax
	mov	[edi+esi], eax
nf25_22:mov	eax, ebx
	mov	[edi+4], eax
	mov	[edi+esi+4], eax
nf25_23:mov	eax, ebx
	mov	[edi+8], eax
	mov	[edi+esi+8], eax
nf25_24:mov	eax, ebx
	mov	[edi+12], eax
	mov	[edi+esi+12], eax
	lea	edi, [edi+esi*2]

nf25_31:mov	eax, ebx
	mov	[edi], eax
	mov	[edi+esi], eax
nf25_32:mov	eax, ebx
	mov	[edi+4], eax
	mov	[edi+esi+4], eax
nf25_33:mov	eax, ebx
	mov	[edi+8], eax
	mov	[edi+esi+8], eax
nf25_34:mov	eax, ebx
	mov	[edi+12], eax
	mov	[edi+esi+12], eax
	lea	edi, [edi+esi*2]

nf25_41:mov	eax, ebx
	mov	[edi], eax
	mov	[edi+esi], eax
nf25_42:mov	eax, ebx
	mov	[edi+4], eax
	mov	[edi+esi+4], eax
nf25_43:mov	eax, ebx
	mov	[edi+8], eax
	mov	[edi+esi+8], eax
nf25_44:mov	eax, ebx
	mov	[edi+12], eax
	mov	[edi+esi+12], eax

	add	edi, esi

	pop	esi
	pop	ebp
	add	esi, 12
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf9+32
nf41:	; low 4x8x2 (16 bytes)
	test	word ptr [esi+4], 08000h
	jnz	nf57

if 0 ;debug
	add	esi, 16
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov8
	lea	edx, byte ptr ds:nf41_11+1

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_11-nf41_11)], bl
	mov	[edx+(nf41_12-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_13-nf41_11)], bl
	mov	[edx+(nf41_14-nf41_11)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_21-nf41_11)], bl
	mov	[edx+(nf41_22-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_23-nf41_11)], bl
	mov	[edx+(nf41_24-nf41_11)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_31-nf41_11)], bl
	mov	[edx+(nf41_32-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_33-nf41_11)], bl
	mov	[edx+(nf41_34-nf41_11)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_41-nf41_11)], bl
	mov	[edx+(nf41_42-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_43-nf41_11)], bl
	mov	[edx+(nf41_44-nf41_11)], bh

	lea	edx, [edx+(nf41_51-nf41_11)]

	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_51-nf41_51)], bl
	mov	[edx+(nf41_52-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_53-nf41_51)], bl
	mov	[edx+(nf41_54-nf41_51)], bh

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_61-nf41_51)], bl
	mov	[edx+(nf41_62-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_63-nf41_51)], bl
	mov	[edx+(nf41_64-nf41_51)], bh


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_71-nf41_51)], bl
	mov	[edx+(nf41_72-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_73-nf41_51)], bl
	mov	[edx+(nf41_74-nf41_51)], bh

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_81-nf41_51)], bl
	mov	[edx+(nf41_82-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_83-nf41_51)], bl
	mov	[edx+(nf41_84-nf41_51)], bh

	push	ebp
	push	esi
	; Load ebx,edx,ecx,ebp with four colors, duplicated in high order.
	Trans16	cx, esi, 1
	shrd	ebx, ecx, 16
	mov	bx, cx
	Trans16	cx, esi+2
	shrd	edx, ecx, 16
	mov	dx, cx
	Trans16	cx, esi+4
	shrd	eax, ecx, 16
	mov	ax, cx
	push	eax
	Trans16	cx, esi+6
	shrd	ebp, ecx, 16
	mov	bp, cx
	pop	ecx
	mov	esi, nf_width

	jmp	nf41_0		; flush prefetch
	ALIGN 4
nf41_0:

nf41_11:mov	[ebp+0], ebx
nf41_12:mov	[ebp+4], ebx
nf41_13:mov	[ebp+8], ebx
nf41_14:mov	[ebp+12], ebx
	add	edi, esi

nf41_21:mov	[ebp+0], ebx
nf41_22:mov	[ebp+4], ebx
nf41_23:mov	[ebp+8], ebx
nf41_24:mov	[ebp+12], ebx
	add	edi, esi

nf41_31:mov	[ebp+0], ebx
nf41_32:mov	[ebp+4], ebx
nf41_33:mov	[ebp+8], ebx
nf41_34:mov	[ebp+12], ebx
	add	edi, esi

nf41_41:mov	[ebp+0], ebx
nf41_42:mov	[ebp+4], ebx
nf41_43:mov	[ebp+8], ebx
nf41_44:mov	[ebp+12], ebx
	add	edi, esi

nf41_51:mov	[ebp+0], ebx
nf41_52:mov	[ebp+4], ebx
nf41_53:mov	[ebp+8], ebx
nf41_54:mov	[ebp+12], ebx
	add	edi, esi

nf41_61:mov	[ebp+0], ebx
nf41_62:mov	[ebp+4], ebx
nf41_63:mov	[ebp+8], ebx
nf41_64:mov	[ebp+12], ebx
	add	edi, esi

nf41_71:mov	[ebp+0], ebx
nf41_72:mov	[ebp+4], ebx
nf41_73:mov	[ebp+8], ebx
nf41_74:mov	[ebp+12], ebx
	add	edi, esi

nf41_81:mov	[ebp+0], ebx
nf41_82:mov	[ebp+4], ebx
nf41_83:mov	[ebp+8], ebx
nf41_84:mov	[ebp+12], ebx

	pop	esi
	pop	ebp
	add	esi, 16
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf9+48
nf57:	; low 8x4x2 (16 bytes)
if 0 ;debug
	add	esi, 16
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov4
	lea	edx, byte ptr ds:nf57_11+2

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_11-nf57_11)], bh
	mov	[edx+(nf57_12-nf57_11)], bl
	shr	ebx, 16
	mov	[edx+(nf57_13-nf57_11)], bh
	mov	[edx+(nf57_14-nf57_11)], bl

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_15-nf57_11)], bh
	mov	[edx+(nf57_16-nf57_11)], bl
	shr	ebx, 16
	mov	[edx+(nf57_17-nf57_11)], bh
	mov	[edx+(nf57_18-nf57_11)], bl


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_21-nf57_11)], bh
	mov	[edx+(nf57_22-nf57_11)], bl
	shr	ebx, 16
	mov	[edx+(nf57_23-nf57_11)], bh
	mov	[edx+(nf57_24-nf57_11)], bl

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_25-nf57_11)], bh
	mov	[edx+(nf57_26-nf57_11)], bl
	shr	ebx, 16
	mov	[edx+(nf57_27-nf57_11)], bh
	mov	[edx+(nf57_28-nf57_11)], bl


	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_31-nf57_11)], bh
	mov	[edx+(nf57_32-nf57_11)], bl
	shr	ebx, 16
	mov	[edx+(nf57_33-nf57_11)], bh
	mov	[edx+(nf57_34-nf57_11)], bl

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_35-nf57_11)], bh
	mov	[edx+(nf57_36-nf57_11)], bl
	shr	ebx, 16
	mov	[edx+(nf57_37-nf57_11)], bh
	mov	[edx+(nf57_38-nf57_11)], bl


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_41-nf57_11)], bh
	mov	[edx+(nf57_42-nf57_11)], bl
	shr	ebx, 16
	mov	[edx+(nf57_43-nf57_11)], bh
	mov	[edx+(nf57_44-nf57_11)], bl

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_45-nf57_11)], bh
	mov	[edx+(nf57_46-nf57_11)], bl
	shr	ebx, 16
	mov	[edx+(nf57_47-nf57_11)], bh
	mov	[edx+(nf57_48-nf57_11)], bl

	push	ebp
	push	esi
	; Load bx,dx,cx,bp with four colors
	Trans16	bx, esi, 1
	Trans16	dx, esi+2
	Trans16 cx, esi+4, 1
	Trans16 bp, esi+6
	mov	esi, nf_width

	jmp	nf57_0		; flush prefetch
	ALIGN 4
nf57_0:

nf57_11:mov	ax, bx
	shl	eax, 16
nf57_12:mov	ax, bx
	mov	[edi], eax
	mov	[edi+esi], eax
nf57_13:mov	ax, bx
	shl	eax, 16
nf57_14:mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+esi+4], eax
nf57_15:mov	ax, bx
	shl	eax, 16
nf57_16:mov	ax, bx
	mov	[edi+8], eax
	mov	[edi+esi+8], eax
nf57_17:mov	ax, bx
	shl	eax, 16
nf57_18:mov	ax, bx
	mov	[edi+12], eax
	mov	[edi+esi+12], eax
	lea	edi, [edi+esi*2]

nf57_21:mov	ax, bx
	shl	eax, 16
nf57_22:mov	ax, bx
	mov	[edi], eax
	mov	[edi+esi], eax
nf57_23:mov	ax, bx
	shl	eax, 16
nf57_24:mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+esi+4], eax
nf57_25:mov	ax, bx
	shl	eax, 16
nf57_26:mov	ax, bx
	mov	[edi+8], eax
	mov	[edi+esi+8], eax
nf57_27:mov	ax, bx
	shl	eax, 16
nf57_28:mov	ax, bx
	mov	[edi+12], eax
	mov	[edi+esi+12], eax
	lea	edi, [edi+esi*2]

nf57_31:mov	ax, bx
	shl	eax, 16
nf57_32:mov	ax, bx
	mov	[edi], eax
	mov	[edi+esi], eax
nf57_33:mov	ax, bx
	shl	eax, 16
nf57_34:mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+esi+4], eax
nf57_35:mov	ax, bx
	shl	eax, 16
nf57_36:mov	ax, bx
	mov	[edi+8], eax
	mov	[edi+esi+8], eax
nf57_37:mov	ax, bx
	shl	eax, 16
nf57_38:mov	ax, bx
	mov	[edi+12], eax
	mov	[edi+esi+12], eax
	lea	edi, [edi+esi*2]

nf57_41:mov	ax, bx
	shl	eax, 16
nf57_42:mov	ax, bx
	mov	[edi], eax
	mov	[edi+esi], eax
nf57_43:mov	ax, bx
	shl	eax, 16
nf57_44:mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+esi+4], eax
nf57_45:mov	ax, bx
	shl	eax, 16
nf57_46:mov	ax, bx
	mov	[edi+8], eax
	mov	[edi+esi+8], eax
nf57_47:mov	ax, bx
	shl	eax, 16
nf57_48:mov	ax, bx
	mov	[edi+12], eax
	mov	[edi+esi+12], eax
	add	edi, esi

	pop	esi
	pop	ebp
	add	esi, 16
	sub	edi, nfpk_back_right
	retn

;----------------------------------------
	ALIGN 4
nf10:	; 2x2 4x4x2 (48 bytes)

	test	word ptr [esi], 08000h
	jnz	nf26

if 0 ;debug
	add	esi, 48
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov4
	lea	edx, byte ptr ds:nf10_11+2

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_11-nf10_11)], bh
	mov	[edx+(nf10_12-nf10_11)], bl
	shr	ebx, 16
	mov	[edx+(nf10_13-nf10_11)], bh
	mov	[edx+(nf10_14-nf10_11)], bl

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_15-nf10_11)], bh
	mov	[edx+(nf10_16-nf10_11)], bl
	shr	ebx, 16
	mov	[edx+(nf10_17-nf10_11)], bh
	mov	[edx+(nf10_18-nf10_11)], bl


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_21-nf10_11)], bh
	mov	[edx+(nf10_22-nf10_11)], bl
	shr	ebx, 16
	mov	[edx+(nf10_23-nf10_11)], bh
	mov	[edx+(nf10_24-nf10_11)], bl

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_25-nf10_11)], bh
	mov	[edx+(nf10_26-nf10_11)], bl
	shr	ebx, 16
	mov	[edx+(nf10_27-nf10_11)], bh
	mov	[edx+(nf10_28-nf10_11)], bl


	mov	al, [esi+20]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_31-nf10_11)], bh
	mov	[edx+(nf10_32-nf10_11)], bl
	shr	ebx, 16
	mov	[edx+(nf10_33-nf10_11)], bh
	mov	[edx+(nf10_34-nf10_11)], bl

	mov	al, [esi+21]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_35-nf10_11)], bh
	mov	[edx+(nf10_36-nf10_11)], bl
	shr	ebx, 16
	mov	[edx+(nf10_37-nf10_11)], bh
	mov	[edx+(nf10_38-nf10_11)], bl


	mov	al, [esi+22]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_41-nf10_11)], bh
	mov	[edx+(nf10_42-nf10_11)], bl
	shr	ebx, 16
	mov	[edx+(nf10_43-nf10_11)], bh
	mov	[edx+(nf10_44-nf10_11)], bl

	mov	al, [esi+23]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_45-nf10_11)], bh
	mov	[edx+(nf10_46-nf10_11)], bl
	shr	ebx, 16
	mov	[edx+(nf10_47-nf10_11)], bh
	mov	[edx+(nf10_48-nf10_11)], bl


	lea	edx, [edx+(nf10_51-nf10_11)]

	mov	al, [esi+32]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_51-nf10_51)], bh
	mov	[edx+(nf10_52-nf10_51)], bl
	shr	ebx, 16
	mov	[edx+(nf10_53-nf10_51)], bh
	mov	[edx+(nf10_54-nf10_51)], bl

	mov	al, [esi+33]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_55-nf10_51)], bh
	mov	[edx+(nf10_56-nf10_51)], bl
	shr	ebx, 16
	mov	[edx+(nf10_57-nf10_51)], bh
	mov	[edx+(nf10_58-nf10_51)], bl


	mov	al, [esi+34]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_61-nf10_51)], bh
	mov	[edx+(nf10_62-nf10_51)], bl
	shr	ebx, 16
	mov	[edx+(nf10_63-nf10_51)], bh
	mov	[edx+(nf10_64-nf10_51)], bl

	mov	al, [esi+35]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_65-nf10_51)], bh
	mov	[edx+(nf10_66-nf10_51)], bl
	shr	ebx, 16
	mov	[edx+(nf10_67-nf10_51)], bh
	mov	[edx+(nf10_68-nf10_51)], bl


	mov	al, [esi+44]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_71-nf10_51)], bh
	mov	[edx+(nf10_72-nf10_51)], bl
	shr	ebx, 16
	mov	[edx+(nf10_73-nf10_51)], bh
	mov	[edx+(nf10_74-nf10_51)], bl

	mov	al, [esi+45]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_75-nf10_51)], bh
	mov	[edx+(nf10_76-nf10_51)], bl
	shr	ebx, 16
	mov	[edx+(nf10_77-nf10_51)], bh
	mov	[edx+(nf10_78-nf10_51)], bl


	mov	al, [esi+46]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_81-nf10_51)], bh
	mov	[edx+(nf10_82-nf10_51)], bl
	shr	ebx, 16
	mov	[edx+(nf10_83-nf10_51)], bh
	mov	[edx+(nf10_84-nf10_51)], bl

	mov	al, [esi+47]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_85-nf10_51)], bh
	mov	[edx+(nf10_86-nf10_51)], bl
	shr	ebx, 16
	mov	[edx+(nf10_87-nf10_51)], bh
	mov	[edx+(nf10_88-nf10_51)], bl

	push	ebp
	push	esi
	; Load bx,dx,cx,bp with four colors
	Trans16	bx, esi
	Trans16	dx, esi+2
	Trans16 cx, esi+4
	Trans16 bp, esi+6
	mov	esi, nf_width

	jmp	nf10_0		; flush prefetch
	ALIGN 4
nf10_0:

nf10_11:mov	ax, bx
	shl	eax, 16
nf10_12:mov	ax, bx
	mov	[edi], eax
nf10_13:mov	ax, bx
	shl	eax, 16
nf10_14:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_15:mov	ax, bx
	shl	eax, 16
nf10_16:mov	ax, bx
	mov	[edi], eax
nf10_17:mov	ax, bx
	shl	eax, 16
nf10_18:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_21:mov	ax, bx
	shl	eax, 16
nf10_22:mov	ax, bx
	mov	[edi], eax
nf10_23:mov	ax, bx
	shl	eax, 16
nf10_24:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_25:mov	ax, bx
	shl	eax, 16
nf10_26:mov	ax, bx
	mov	[edi], eax
nf10_27:mov	ax, bx
	shl	eax, 16
nf10_28:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

	; Load bx,dx,cx,bp with four colors
	mov	esi, [esp]
	Trans16	bx, esi+12
	Trans16	dx, esi+14
	Trans16 cx, esi+16
	Trans16 bp, esi+18
	mov	esi, nf_width

nf10_31:mov	ax, bx
	shl	eax, 16
nf10_32:mov	ax, bx
	mov	[edi], eax
nf10_33:mov	ax, bx
	shl	eax, 16
nf10_34:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_35:mov	ax, bx
	shl	eax, 16
nf10_36:mov	ax, bx
	mov	[edi], eax
nf10_37:mov	ax, bx
	shl	eax, 16
nf10_38:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_41:mov	ax, bx
	shl	eax, 16
nf10_42:mov	ax, bx
	mov	[edi], eax
nf10_43:mov	ax, bx
	shl	eax, 16
nf10_44:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_45:mov	ax, bx
	shl	eax, 16
nf10_46:mov	ax, bx
	mov	[edi], eax
nf10_47:mov	ax, bx
	shl	eax, 16
nf10_48:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

	lea	eax, [esi*8-8]
	sub	edi, eax

	; Load bx,dx,cx,bp with four colors
	mov	esi, [esp]
	Trans16	bx, esi+24
	Trans16	dx, esi+26
	Trans16 cx, esi+28
	Trans16 bp, esi+30
	mov	esi, nf_width

nf10_51:mov	ax, bx
	shl	eax, 16
nf10_52:mov	ax, bx
	mov	[edi], eax
nf10_53:mov	ax, bx
	shl	eax, 16
nf10_54:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_55:mov	ax, bx
	shl	eax, 16
nf10_56:mov	ax, bx
	mov	[edi], eax
nf10_57:mov	ax, bx
	shl	eax, 16
nf10_58:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_61:mov	ax, bx
	shl	eax, 16
nf10_62:mov	ax, bx
	mov	[edi], eax
nf10_63:mov	ax, bx
	shl	eax, 16
nf10_64:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_65:mov	ax, bx
	shl	eax, 16
nf10_66:mov	ax, bx
	mov	[edi], eax
nf10_67:mov	ax, bx
	shl	eax, 16
nf10_68:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

	; Load bx,dx,cx,bp with four colors
	mov	esi, [esp]
	Trans16	bx, esi+36
	Trans16	dx, esi+38
	Trans16 cx, esi+40
	Trans16 bp, esi+42
	mov	esi, nf_width

nf10_71:mov	ax, bx
	shl	eax, 16
nf10_72:mov	ax, bx
	mov	[edi], eax
nf10_73:mov	ax, bx
	shl	eax, 16
nf10_74:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_75:mov	ax, bx
	shl	eax, 16
nf10_76:mov	ax, bx
	mov	[edi], eax
nf10_77:mov	ax, bx
	shl	eax, 16
nf10_78:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_81:mov	ax, bx
	shl	eax, 16
nf10_82:mov	ax, bx
	mov	[edi], eax
nf10_83:mov	ax, bx
	shl	eax, 16
nf10_84:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf10_85:mov	ax, bx
	shl	eax, 16
nf10_86:mov	ax, bx
	mov	[edi], eax
nf10_87:mov	ax, bx
	shl	eax, 16
nf10_88:mov	ax, bx
	mov	[edi+4], eax

	pop	esi
	pop	ebp
	add	esi, 48
	sub	edi, 8
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf10+16
nf26:	; 2x1 4x8x2 (32 bytes)

	test	word ptr [esi+16], 08000h
	jnz	nf42

if 0 ;debug
	add	esi, 32
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov4
	lea	edx, byte ptr ds:nf26_11+2

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_11-nf26_11)], bh
	mov	[edx+(nf26_12-nf26_11)], bl
	shr	ebx, 16
	mov	[edx+(nf26_13-nf26_11)], bh
	mov	[edx+(nf26_14-nf26_11)], bl

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_15-nf26_11)], bh
	mov	[edx+(nf26_16-nf26_11)], bl
	shr	ebx, 16
	mov	[edx+(nf26_17-nf26_11)], bh
	mov	[edx+(nf26_18-nf26_11)], bl


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_21-nf26_11)], bh
	mov	[edx+(nf26_22-nf26_11)], bl
	shr	ebx, 16
	mov	[edx+(nf26_23-nf26_11)], bh
	mov	[edx+(nf26_24-nf26_11)], bl

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_25-nf26_11)], bh
	mov	[edx+(nf26_26-nf26_11)], bl
	shr	ebx, 16
	mov	[edx+(nf26_27-nf26_11)], bh
	mov	[edx+(nf26_28-nf26_11)], bl

	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_31-nf26_11)], bh
	mov	[edx+(nf26_32-nf26_11)], bl
	shr	ebx, 16
	mov	[edx+(nf26_33-nf26_11)], bh
	mov	[edx+(nf26_34-nf26_11)], bl

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_35-nf26_11)], bh
	mov	[edx+(nf26_36-nf26_11)], bl
	shr	ebx, 16
	mov	[edx+(nf26_37-nf26_11)], bh
	mov	[edx+(nf26_38-nf26_11)], bl


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_41-nf26_11)], bh
	mov	[edx+(nf26_42-nf26_11)], bl
	shr	ebx, 16
	mov	[edx+(nf26_43-nf26_11)], bh
	mov	[edx+(nf26_44-nf26_11)], bl

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_45-nf26_11)], bh
	mov	[edx+(nf26_46-nf26_11)], bl
	shr	ebx, 16
	mov	[edx+(nf26_47-nf26_11)], bh
	mov	[edx+(nf26_48-nf26_11)], bl


	lea	edx, [edx+(nf26_51-nf26_11)]

	mov	al, [esi+24]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_51-nf26_51)], bh
	mov	[edx+(nf26_52-nf26_51)], bl
	shr	ebx, 16
	mov	[edx+(nf26_53-nf26_51)], bh
	mov	[edx+(nf26_54-nf26_51)], bl

	mov	al, [esi+25]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_55-nf26_51)], bh
	mov	[edx+(nf26_56-nf26_51)], bl
	shr	ebx, 16
	mov	[edx+(nf26_57-nf26_51)], bh
	mov	[edx+(nf26_58-nf26_51)], bl


	mov	al, [esi+26]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_61-nf26_51)], bh
	mov	[edx+(nf26_62-nf26_51)], bl
	shr	ebx, 16
	mov	[edx+(nf26_63-nf26_51)], bh
	mov	[edx+(nf26_64-nf26_51)], bl

	mov	al, [esi+27]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_65-nf26_51)], bh
	mov	[edx+(nf26_66-nf26_51)], bl
	shr	ebx, 16
	mov	[edx+(nf26_67-nf26_51)], bh
	mov	[edx+(nf26_68-nf26_51)], bl


	mov	al, [esi+28]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_71-nf26_51)], bh
	mov	[edx+(nf26_72-nf26_51)], bl
	shr	ebx, 16
	mov	[edx+(nf26_73-nf26_51)], bh
	mov	[edx+(nf26_74-nf26_51)], bl

	mov	al, [esi+29]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_75-nf26_51)], bh
	mov	[edx+(nf26_76-nf26_51)], bl
	shr	ebx, 16
	mov	[edx+(nf26_77-nf26_51)], bh
	mov	[edx+(nf26_78-nf26_51)], bl


	mov	al, [esi+30]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_81-nf26_51)], bh
	mov	[edx+(nf26_82-nf26_51)], bl
	shr	ebx, 16
	mov	[edx+(nf26_83-nf26_51)], bh
	mov	[edx+(nf26_84-nf26_51)], bl

	mov	al, [esi+31]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_85-nf26_51)], bh
	mov	[edx+(nf26_86-nf26_51)], bl
	shr	ebx, 16
	mov	[edx+(nf26_87-nf26_51)], bh
	mov	[edx+(nf26_88-nf26_51)], bl

	push	ebp
	push	esi
	; Load bx,dx,cx,bp with four colors
	Trans16	bx, esi, 1
	Trans16	dx, esi+2
	Trans16 cx, esi+4
	Trans16 bp, esi+6
	mov	esi, nf_width

	jmp	nf26_0		; flush prefetch
	ALIGN 4
nf26_0:

nf26_11:mov	ax, bx
	shl	eax, 16
nf26_12:mov	ax, bx
	mov	[edi], eax
nf26_13:mov	ax, bx
	shl	eax, 16
nf26_14:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_15:mov	ax, bx
	shl	eax, 16
nf26_16:mov	ax, bx
	mov	[edi], eax
nf26_17:mov	ax, bx
	shl	eax, 16
nf26_18:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_21:mov	ax, bx
	shl	eax, 16
nf26_22:mov	ax, bx
	mov	[edi], eax
nf26_23:mov	ax, bx
	shl	eax, 16
nf26_24:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_25:mov	ax, bx
	shl	eax, 16
nf26_26:mov	ax, bx
	mov	[edi], eax
nf26_27:mov	ax, bx
	shl	eax, 16
nf26_28:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_31:mov	ax, bx
	shl	eax, 16
nf26_32:mov	ax, bx
	mov	[edi], eax
nf26_33:mov	ax, bx
	shl	eax, 16
nf26_34:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_35:mov	ax, bx
	shl	eax, 16
nf26_36:mov	ax, bx
	mov	[edi], eax
nf26_37:mov	ax, bx
	shl	eax, 16
nf26_38:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_41:mov	ax, bx
	shl	eax, 16
nf26_42:mov	ax, bx
	mov	[edi], eax
nf26_43:mov	ax, bx
	shl	eax, 16
nf26_44:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_45:mov	ax, bx
	shl	eax, 16
nf26_46:mov	ax, bx
	mov	[edi], eax
nf26_47:mov	ax, bx
	shl	eax, 16
nf26_48:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

	lea	eax, [esi*8-8]
	sub	edi, eax

	; Load bx,dx,cx,bp with four colors
	mov	esi, [esp]
	Trans16	bx, esi+16
	Trans16	dx, esi+18
	Trans16 cx, esi+20
	Trans16 bp, esi+22
	mov	esi, nf_width

nf26_51:mov	ax, bx
	shl	eax, 16
nf26_52:mov	ax, bx
	mov	[edi], eax
nf26_53:mov	ax, bx
	shl	eax, 16
nf26_54:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_55:mov	ax, bx
	shl	eax, 16
nf26_56:mov	ax, bx
	mov	[edi], eax
nf26_57:mov	ax, bx
	shl	eax, 16
nf26_58:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_61:mov	ax, bx
	shl	eax, 16
nf26_62:mov	ax, bx
	mov	[edi], eax
nf26_63:mov	ax, bx
	shl	eax, 16
nf26_64:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_65:mov	ax, bx
	shl	eax, 16
nf26_66:mov	ax, bx
	mov	[edi], eax
nf26_67:mov	ax, bx
	shl	eax, 16
nf26_68:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_71:mov	ax, bx
	shl	eax, 16
nf26_72:mov	ax, bx
	mov	[edi], eax
nf26_73:mov	ax, bx
	shl	eax, 16
nf26_74:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_75:mov	ax, bx
	shl	eax, 16
nf26_76:mov	ax, bx
	mov	[edi], eax
nf26_77:mov	ax, bx
	shl	eax, 16
nf26_78:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_81:mov	ax, bx
	shl	eax, 16
nf26_82:mov	ax, bx
	mov	[edi], eax
nf26_83:mov	ax, bx
	shl	eax, 16
nf26_84:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf26_85:mov	ax, bx
	shl	eax, 16
nf26_86:mov	ax, bx
	mov	[edi], eax
nf26_87:mov	ax, bx
	shl	eax, 16
nf26_88:mov	ax, bx
	mov	[edi+4], eax

	pop	esi
	pop	ebp
	add	esi, 32
	sub	edi, 8
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf10+32
nf42:	; 1x2 8x4x2 (32 bytes)

if 0 ;debug
	add	esi, 32
	mov	ebx, 0
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfhpk_mov4
	lea	edx, byte ptr ds:nf42_11+2

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_11-nf42_11)], bh
	mov	[edx+(nf42_12-nf42_11)], bl
	shr	ebx, 16
	mov	[edx+(nf42_13-nf42_11)], bh
	mov	[edx+(nf42_14-nf42_11)], bl

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_15-nf42_11)], bh
	mov	[edx+(nf42_16-nf42_11)], bl
	shr	ebx, 16
	mov	[edx+(nf42_17-nf42_11)], bh
	mov	[edx+(nf42_18-nf42_11)], bl


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_21-nf42_11)], bh
	mov	[edx+(nf42_22-nf42_11)], bl
	shr	ebx, 16
	mov	[edx+(nf42_23-nf42_11)], bh
	mov	[edx+(nf42_24-nf42_11)], bl

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_25-nf42_11)], bh
	mov	[edx+(nf42_26-nf42_11)], bl
	shr	ebx, 16
	mov	[edx+(nf42_27-nf42_11)], bh
	mov	[edx+(nf42_28-nf42_11)], bl


	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_31-nf42_11)], bh
	mov	[edx+(nf42_32-nf42_11)], bl
	shr	ebx, 16
	mov	[edx+(nf42_33-nf42_11)], bh
	mov	[edx+(nf42_34-nf42_11)], bl

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_35-nf42_11)], bh
	mov	[edx+(nf42_36-nf42_11)], bl
	shr	ebx, 16
	mov	[edx+(nf42_37-nf42_11)], bh
	mov	[edx+(nf42_38-nf42_11)], bl


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_41-nf42_11)], bh
	mov	[edx+(nf42_42-nf42_11)], bl
	shr	ebx, 16
	mov	[edx+(nf42_43-nf42_11)], bh
	mov	[edx+(nf42_44-nf42_11)], bl

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_45-nf42_11)], bh
	mov	[edx+(nf42_46-nf42_11)], bl
	shr	ebx, 16
	mov	[edx+(nf42_47-nf42_11)], bh
	mov	[edx+(nf42_48-nf42_11)], bl


	lea	edx, [edx+(nf42_51-nf42_11)]

	mov	al, [esi+24]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_51-nf42_51)], bh
	mov	[edx+(nf42_52-nf42_51)], bl
	shr	ebx, 16
	mov	[edx+(nf42_53-nf42_51)], bh
	mov	[edx+(nf42_54-nf42_51)], bl

	mov	al, [esi+25]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_55-nf42_51)], bh
	mov	[edx+(nf42_56-nf42_51)], bl
	shr	ebx, 16
	mov	[edx+(nf42_57-nf42_51)], bh
	mov	[edx+(nf42_58-nf42_51)], bl


	mov	al, [esi+26]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_61-nf42_51)], bh
	mov	[edx+(nf42_62-nf42_51)], bl
	shr	ebx, 16
	mov	[edx+(nf42_63-nf42_51)], bh
	mov	[edx+(nf42_64-nf42_51)], bl

	mov	al, [esi+27]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_65-nf42_51)], bh
	mov	[edx+(nf42_66-nf42_51)], bl
	shr	ebx, 16
	mov	[edx+(nf42_67-nf42_51)], bh
	mov	[edx+(nf42_68-nf42_51)], bl


	mov	al, [esi+28]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_71-nf42_51)], bh
	mov	[edx+(nf42_72-nf42_51)], bl
	shr	ebx, 16
	mov	[edx+(nf42_73-nf42_51)], bh
	mov	[edx+(nf42_74-nf42_51)], bl

	mov	al, [esi+29]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_75-nf42_51)], bh
	mov	[edx+(nf42_76-nf42_51)], bl
	shr	ebx, 16
	mov	[edx+(nf42_77-nf42_51)], bh
	mov	[edx+(nf42_78-nf42_51)], bl


	mov	al, [esi+30]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_81-nf42_51)], bh
	mov	[edx+(nf42_82-nf42_51)], bl
	shr	ebx, 16
	mov	[edx+(nf42_83-nf42_51)], bh
	mov	[edx+(nf42_84-nf42_51)], bl

	mov	al, [esi+31]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_85-nf42_51)], bh
	mov	[edx+(nf42_86-nf42_51)], bl
	shr	ebx, 16
	mov	[edx+(nf42_87-nf42_51)], bh
	mov	[edx+(nf42_88-nf42_51)], bl

	push	ebp
	push	esi
	; Load bx,dx,cx,bp with four colors
	Trans16	bx, esi, 1
	Trans16	dx, esi+2
	Trans16 cx, esi+4
	Trans16 bp, esi+6
	mov	esi, nf_width

	jmp	nf42_0		; flush prefetch
	ALIGN 4
nf42_0:

nf42_11:mov	ax, bx
	shl	eax, 16
nf42_12:mov	ax, bx
	mov	[edi], eax
nf42_13:mov	ax, bx
	shl	eax, 16
nf42_14:mov	ax, bx
	mov	[edi+4], eax
nf42_15:mov	ax, bx
	shl	eax, 16
nf42_16:mov	ax, bx
	mov	[edi+8], eax
nf42_17:mov	ax, bx
	shl	eax, 16
nf42_18:mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf42_21:mov	ax, bx
	shl	eax, 16
nf42_22:mov	ax, bx
	mov	[edi], eax
nf42_23:mov	ax, bx
	shl	eax, 16
nf42_24:mov	ax, bx
	mov	[edi+4], eax
nf42_25:mov	ax, bx
	shl	eax, 16
nf42_26:mov	ax, bx
	mov	[edi+8], eax
nf42_27:mov	ax, bx
	shl	eax, 16
nf42_28:mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf42_31:mov	ax, bx
	shl	eax, 16
nf42_32:mov	ax, bx
	mov	[edi], eax
nf42_33:mov	ax, bx
	shl	eax, 16
nf42_34:mov	ax, bx
	mov	[edi+4], eax
nf42_35:mov	ax, bx
	shl	eax, 16
nf42_36:mov	ax, bx
	mov	[edi+8], eax
nf42_37:mov	ax, bx
	shl	eax, 16
nf42_38:mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf42_41:mov	ax, bx
	shl	eax, 16
nf42_42:mov	ax, bx
	mov	[edi], eax
nf42_43:mov	ax, bx
	shl	eax, 16
nf42_44:mov	ax, bx
	mov	[edi+4], eax
nf42_45:mov	ax, bx
	shl	eax, 16
nf42_46:mov	ax, bx
	mov	[edi+8], eax
nf42_47:mov	ax, bx
	shl	eax, 16
nf42_48:mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

	; Load bx,dx,cx,bp with four colors
	mov	esi, [esp]
	Trans16	bx, esi+16, 1
	Trans16	dx, esi+18
	Trans16 cx, esi+20
	Trans16 bp, esi+22
	mov	esi, nf_width

nf42_51:mov	ax, bx
	shl	eax, 16
nf42_52:mov	ax, bx
	mov	[edi], eax
nf42_53:mov	ax, bx
	shl	eax, 16
nf42_54:mov	ax, bx
	mov	[edi+4], eax
nf42_55:mov	ax, bx
	shl	eax, 16
nf42_56:mov	ax, bx
	mov	[edi+8], eax
nf42_57:mov	ax, bx
	shl	eax, 16
nf42_58:mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf42_61:mov	ax, bx
	shl	eax, 16
nf42_62:mov	ax, bx
	mov	[edi], eax
nf42_63:mov	ax, bx
	shl	eax, 16
nf42_64:mov	ax, bx
	mov	[edi+4], eax
nf42_65:mov	ax, bx
	shl	eax, 16
nf42_66:mov	ax, bx
	mov	[edi+8], eax
nf42_67:mov	ax, bx
	shl	eax, 16
nf42_68:mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf42_71:mov	ax, bx
	shl	eax, 16
nf42_72:mov	ax, bx
	mov	[edi], eax
nf42_73:mov	ax, bx
	shl	eax, 16
nf42_74:mov	ax, bx
	mov	[edi+4], eax
nf42_75:mov	ax, bx
	shl	eax, 16
nf42_76:mov	ax, bx
	mov	[edi+8], eax
nf42_77:mov	ax, bx
	shl	eax, 16
nf42_78:mov	ax, bx
	mov	[edi+12], eax
	add	edi, esi

nf42_81:mov	ax, bx
	shl	eax, 16
nf42_82:mov	ax, bx
	mov	[edi], eax
nf42_83:mov	ax, bx
	shl	eax, 16
nf42_84:mov	ax, bx
	mov	[edi+4], eax
nf42_85:mov	ax, bx
	shl	eax, 16
nf42_86:mov	ax, bx
	mov	[edi+8], eax
nf42_87:mov	ax, bx
	shl	eax, 16
nf42_88:mov	ax, bx
	mov	[edi+12], eax

	pop	esi
	pop	ebp
	add	esi, 32
	sub	edi, nfpk_back_right
	retn

;----------------------------------------
	ALIGN 4
nf11:	;     8x8x16 (128 bytes)
if 0 ;debug
	add	esi, 128
	mov	ebx, 0
	jmp	nf_solid
endif
	mov	edx, nf_width


Trans16Blk	MACRO idx
	Trans16	bx, idx
	mov	[edi], bx
	Trans16	bx, idx+2
	mov	[edi+2], bx
	Trans16 bx, idx+4
	mov	[edi+4], bx
	Trans16 bx, idx+6
	mov	[edi+6], bx
	Trans16 bx, idx+8
	mov	[edi+8], bx
	Trans16 bx, idx+10
	mov	[edi+10], bx
	Trans16 bx, idx+12
	mov	[edi+12], bx
	Trans16 bx, idx+14
	mov	[edi+14], bx
	ENDM

	Trans16Blk	esi	;0
	add	edi, edx
	Trans16Blk	esi+16	;1
	add	edi, edx
	Trans16Blk	esi+32	;2
	add	edi, edx
	Trans16Blk	esi+48	;3
	add	edi, edx
	Trans16Blk	esi+64	;4
	add	edi, edx
	Trans16Blk	esi+80	;5
	add	edi, edx
	Trans16Blk	esi+96	;6
	add	edi, edx
	Trans16Blk	esi+112	;7

	add	esi, 128
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8
	retn

;----------------------------------------
	ALIGN 4
nf12:	; low 4x4x16 (32 bytes)
if 0 ;debug
	add	esi, 32
	mov	ebx, 0
	jmp	nf_solid
endif
	mov	edx, nf_width

	Trans16	bx, esi
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi], eax
	mov	[edi+edx], eax

	Trans16	bx, esi+2
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax

	Trans16	bx, esi+4
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+8], eax
	mov	[edi+edx+8], eax

	Trans16	bx, esi+6
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+12], eax
	mov	[edi+edx+12], eax

	lea	edi, [edi+edx*2]

	Trans16	bx, esi+8
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi], eax
	mov	[edi+edx], eax

	Trans16	bx, esi+10
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax

	Trans16	bx, esi+12
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+8], eax
	mov	[edi+edx+8], eax

	Trans16	bx, esi+14
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+12], eax
	mov	[edi+edx+12], eax

	lea	edi, [edi+edx*2]

	Trans16	bx, esi+16
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi], eax
	mov	[edi+edx], eax

	Trans16	bx, esi+18
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax

	Trans16	bx, esi+20
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+8], eax
	mov	[edi+edx+8], eax

	Trans16	bx, esi+22
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+12], eax
	mov	[edi+edx+12], eax

	lea	edi, [edi+edx*2]

	Trans16	bx, esi+24
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi], eax
	mov	[edi+edx], eax

	Trans16	bx, esi+26
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax

	Trans16	bx, esi+28
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+8], eax
	mov	[edi+edx+8], eax

	Trans16	bx, esi+30
	shrd	eax, ebx, 16
	mov	ax, bx
	mov	[edi+12], eax
	mov	[edi+edx+12], eax

	add	edi, edx

	sub	edi, nfpk_back_right
	add	esi, 32
	retn

;----------------------------------------
	ALIGN 4
nf13:	; 2x2 4x4x0 (8 bytes)
if 0 ;debug
	add	esi, 8
	mov	ebx, 0
	jmp	nf_solid
endif
	mov	edx, nf_width

	Trans16	cx, esi
	shrd	ebx, ecx, 16
	mov	bx, cx

	Trans16	cx, esi+2
	shrd	eax, ecx, 16
	mov	ax, cx
	mov	ecx, eax

	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ecx
	mov	[edi+12], ecx
	mov	[edi+edx], ebx
	mov	[edi+edx+4], ebx
	mov	[edi+edx+8], ecx
	mov	[edi+edx+12], ecx
	lea	edi, [edi+edx*2]
	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ecx
	mov	[edi+12], ecx
	mov	[edi+edx], ebx
	mov	[edi+edx+4], ebx
	mov	[edi+edx+8], ecx
	mov	[edi+edx+12], ecx
	lea	edi, [edi+edx*2]

	Trans16	cx, esi+4
	shrd	ebx, ecx, 16
	mov	bx, cx

	Trans16	cx, esi+6
	shrd	eax, ecx, 16
	mov	ax, cx
	mov	ecx, eax

	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ecx
	mov	[edi+12], ecx
	mov	[edi+edx], ebx
	mov	[edi+edx+4], ebx
	mov	[edi+edx+8], ecx
	mov	[edi+edx+12], ecx
	lea	edi, [edi+edx*2]
	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ecx
	mov	[edi+12], ecx
	mov	[edi+edx], ebx
	mov	[edi+edx+4], ebx
	mov	[edi+edx+8], ecx
	mov	[edi+edx+12], ecx
	add	edi, edx

	sub	edi, nfpk_back_right
	add	esi, 8

	retn

;----------------------------------------
	ALIGN 4
nf14:	;     8x8x0 (2 bytes)
	Trans16	cx, esi
	add	esi, 2
	shrd	ebx, ecx, 16
	mov	bx, cx

nf_solid:
	mov	edx, nf_width

	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ebx
	mov	[edi+12], ebx
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ebx
	mov	[edi+12], ebx
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ebx
	mov	[edi+12], ebx
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ebx
	mov	[edi+12], ebx
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ebx
	mov	[edi+12], ebx
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ebx
	mov	[edi+12], ebx
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ebx
	mov	[edi+12], ebx
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	mov	[edi+8], ebx
	mov	[edi+12], ebx

	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
nf15:	; unused
	retn

nfHPkDecomp ENDP

; Normal version
;
nfPkDecomp PROC USES ESI EDI EBX, \
		ops:PTRBYTE, comp:PTRBYTE, \
		x:DWORD, y:DWORD, w:DWORD, h:DWORD
	LOCAL tbuf: PTRBYTE
	LOCAL new_row:DWORD
	LOCAL DiffBufPtrs:DWORD

	LOCAL nfpk_back_right: DWORD
	LOCAL wcnt:DWORD

	LOG_LABEL "StartPkDecomp"

.data
nfpk_OpTbl label dword
	dword offset nf0	; Prev Same (0)
	dword offset nf1	; No change (and copied to screen) (0)
	dword offset nf2	; Near shift from older part of current buf (1)
	dword offset nf3	; Near shift from newer part of current buf (1)
	dword offset nf4	; Near shift from previous buffer (1)
	dword offset nf5	; Far shift from previous buffer (2)
	dword offset nf6	; Far shift from current buffer (2)
				; [Or if COMPOPS, run of no changes (0)]
	dword offset nf7	;     8x8x1 (10 bytes) or low 4x4x1 (4 bytes)
	dword offset nf8	; 2x2 4x4x1 (16 bytes) or 2x1 4x8x1 (12 bytes) or 1x2 8x4x1 (12 bytes)
	dword offset nf9	;     8x8x2 (20 bytes) or low 4x4x2 (8 bytes) or
				; low 4x8x2 (12 bytes) or low 8x4x2 (12 bytes)
	dword offset nf10	; 2x2 4x4x2 (32 bytes) or 2x1 4x8x2 (24 bytes) or 1x2 4x8x2 (24 bytes)
	dword offset nf11	;     8x8x8 (64 bytes)
	dword offset nf12	; low 4x4x8 (16 bytes)
	dword offset nf13	; 2x2 4x4x0 (ie 2x2x8) (4 bytes)
	dword offset nf14	;     8x8x0 (1 byte)
	dword offset nf15	; mix 8x8x0 (2 bytes)
.code

	NF_DECOMP_INIT	0

	mov	eax, nf_back_right
	sub	eax, SWIDTH
	mov	nfpk_back_right, eax

	mov	esi, comp
	mov	edi, tbuf
nf_StartRow:
	mov	eax, w
	shr	eax, 1
	mov	wcnt,eax
	ALIGN 4
nf_NextPair:
	dec	wcnt
	js	nf_NextRow
	mov	ebx, ops
	mov	al, [ebx]
	inc	ebx
	mov	ops, ebx

	xor	ebx, ebx
	mov	bl, al
	shr	bl, 4
	and	eax, 0Fh
	push	offset nf_NextPair
	push	nfpk_OpTbl[ebx*4]
	jmp	nfpk_OpTbl[eax*4]

nf_NextRow:
	add	edi, new_row
	dec	h
	jnz	nf_StartRow
	LOG_LABEL "EndPkDecomp"

	ret

;----------------------------------------
	ALIGN 4

nf0:	; No change from previous buffer
	mov	eax, DiffBufPtrs
	jmp	nf_shift

; \ Diagonal blend
;0        1
;  00010101	1
;  00001313	2
;  20303101	3
;  02030313	4
;  23203031	5
;  02020333	6
;  23232333	7
;2 22023233	8
;
nf0_d:
				; 3412  (low to high)
				;------
	mov	al, bl		; 0001	(1)
	mov	ah, bh
	shl	eax, 16
	mov	al, bl
	mov	ah, bl
	mov	[edi], eax
	mov	ah, bh		; 0101
	mov	[edi+4], eax
	add	edi, edx

	mov	ah, bl		; 0000	(2)
	rol	eax, 16
	mov	ah, bl
	mov	[edi], eax
	mov	al, bh		; 1313
	mov	ah, ch
	shl	eax, 16
	mov	al, bh
	mov	ah, ch
	mov	[edi+4], eax
	add	edi, edx

	mov	al, ch		; 2030 (3)
	mov	ah, bl
	shl	eax, 16
	mov	al, cl
	mov	ah, bl
	mov	[edi], eax
	mov	al, bl		; 3101
	mov	ah, bh
	shl	eax, 16
	mov	al, ch
	mov	ah, bh
	mov	[edi+4], eax
	add	edi, edx

	mov	al, bl		; 0203 (4)
	mov	ah, ch
	shl	eax, 16
	mov	al, bl
	mov	ah, cl
	mov	[edi], eax
	mov	al, bh		; 0313
	mov	ah, ch
	rol	eax, 16
	mov	[edi+4], eax
	add	edi, edx

	mov	al, cl		; 2320 (5)
	mov	ah, bl
	shl	eax, 16
	mov	al, cl
	mov	ah, ch
	mov	[edi], eax
	mov	al, ch		; 3031
	mov	ah, bh
	shl	eax, 16
	mov	al, ch
	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

	mov	al, bl		; 0202 (6)
	mov	ah, cl
	shl	eax, 16
	mov	al, bl
	mov	ah, cl
	mov	[edi], eax
	mov	ah, ch		; 0333
	shl	eax, 16
	mov	al, ch
	mov	ah, ch
	mov	[edi+4], eax
	add	edi, edx

	mov	al, cl		; 2323 (7)
	rol	eax, 16
	mov	al, cl
	mov	[edi], eax
	mov	al, ch		; 2333
	rol	eax, 16
	mov	[edi+4], eax
	add	edi, edx

	mov	al, bl		; 2202 (8)
	mov	ah, cl
	shl	eax, 16
	mov	al, cl
	mov	ah, cl
	mov	[edi], eax
	mov	al, ch		; 3233
	mov	ah, ch
	shl	eax, 16
	mov	al, ch
	mov	ah, cl
	mov	[edi+4], eax

	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8
	retn


; / RDiagonal blend
;0        1
;  01010111	1
;  20201111	2
;  01021313	3
;  20212131	4
;  02121323	5
;  22213131	6
;  22232323	7
;2 22323133	8
;
nf0_r:
				; 3412  (low to high)
				;------
	mov	al, bl		; 0101	(1)
	mov	ah, bh
	shl	eax, 16
	mov	al, bl
	mov	ah, bh
	mov	[edi], eax
	mov	al, bh		; 0111
	rol	eax, 16
	mov	[edi+4], eax
	add	edi, edx

	mov	al, cl		; 2020 (2)
	mov	ah, bl
	shl	eax, 16
	mov	al, cl
	mov	ah, bl
	mov	[edi], eax
	mov	al, bh		; 1111
	mov	ah, bh
	shl	eax, 16
	mov	al, bh
	mov	ah, bh
	mov	[edi+4], eax
	add	edi, edx

	mov	al, bl		; 0102 (3)
	mov	ah, cl
	rol	eax, 16
	mov	al, bl
	mov	[edi], eax
	mov	al, bh		; 1313
	mov	ah, ch
	shl	eax, 16
	mov	al, bh
	mov	ah, ch
	mov	[edi+4], eax
	add	edi, edx

	mov	al, cl		; 2021 (4)
	mov	ah, bh
	shl	eax, 16
	mov	al, cl
	mov	ah, bl
	mov	[edi], eax
	mov	al, ch		; 2131
	mov	ah, bh
	rol	eax, 16
	mov	[edi+4], eax
	add	edi, edx

	ror	eax, 8		; 0212 (5)
	mov	al, bl
	mov	ah, cl
	mov	[edi], eax
	mov	al, cl		; 1323
	mov	ah, ch
	shl	eax, 16
	mov	al, bh
	mov	ah, ch
	mov	[edi+4], eax
	add	edi, edx

	mov	al, cl		; 2221 (6)
	mov	ah, bh
	shl	eax, 16
	mov	al, cl
	mov	ah, cl
	mov	[edi], eax
	mov	al, ch		; 3131
	mov	ah, bh
	shl	eax, 16
	mov	al, ch
	mov	ah, bh
	mov	[edi+4], eax
	add	edi, edx

	mov	al, cl		; 2223 (7)
	mov	ah, ch
	shl	eax, 16
	mov	al, cl
	mov	ah, cl
	mov	[edi], eax
	mov	ah, ch		; 2323
	rol	eax, 16
	mov	[edi+4], eax
	add	edi, edx

	rol	eax, 8		; 2232 (8)
	mov	al, cl
	mov	[edi], eax
	mov	al, ch		; 3133
	mov	ah, ch
	shl	eax, 16
	mov	al, ch
	mov	ah, bh
	mov	[edi+4], eax

	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8
	retn


;----------------------------------------
	ALIGN 4
nf1:	; No change (and copied to screen)
	add	edi, SWIDTH
	retn

;----------------------------------------
	ALIGN 4
nf2:	; Near shift from older part of current buffer
	xor	eax, eax
	mov	al, [esi]
	inc	esi
	mov	ax, nfpk_ShiftP2[eax*2]
nf_xyc_shift:
	xor	ebx, ebx
	mov	bl, ah
	shl	eax, 24
	sar	eax, 24
	add	eax, nfpk_ShiftY[ebx*4]
	jmp	nf_shift

;----------------------------------------
	ALIGN 4
nf3:	; Near shift from newer part of current buffer
	xor	eax, eax
	mov	al, [esi]
	inc	esi
	mov	ax, nfpk_ShiftP2[eax*2]
	neg	al
	neg	ah
	jmp	nf_xyc_shift

;----------------------------------------
	ALIGN 4
nf4:	; Near shift from previous buffer
	xor	eax, eax
	mov	al, [esi]
	inc	esi
	mov	ax, nfpk_ShiftP1[eax*2]
	jmp	nf_xyp_shift

;----------------------------------------
	ALIGN 4
nf5:	; Far shift from previous buffer
	mov	ax, [esi]
	add	esi, 2
nf_xyp_shift:
	xor	ebx, ebx
	mov	bl, ah
	shl	eax, 24
	sar	eax, 24
	add	eax, nfpk_ShiftY[ebx*4]
	add	eax, DiffBufPtrs
	jmp	nf_shift

;----------------------------------------
	ALIGN 4

nf6:	; Run of no changes (must only appear in first nibble opcodes)
	; Next nibble k specifies 2k+4 squares with no changes
	add	esp, 4			; Next nibble is not an opcode
	add	ebx, 2			; (minimum of 4 squares)
	ALIGN 4
nf6a:	add	edi, SWIDTH*2		; Advance over two squares
	dec	ebx
	jz	nf6z			; Last pair of squares
	dec	wcnt			; Same row?
	jns	nf6a			;  Yes
	add	edi, new_row		; Advance to next row
	dec	h			; Decrement row count (should never become zero here)
	mov	eax, w			; Reset wcnt
	shr	eax ,1
	dec	eax
	mov	wcnt, eax
	jmp	nf6a

nf6z:	retn

;----------------------------------------
	ALIGN 4
nf_shift:
if 0 ;debug
	mov	eax, 0
	mov	ebx, eax
	jmp	nf_solid
endif
	mov	ebx, esi	; save esi
	lea	esi, [edi+eax]
	mov	edx, nf_width

	REPEAT 7
	  mov eax, [esi]
	  mov [edi], eax
	  mov eax, [esi+4]
	  mov [edi+4], eax
	  add	esi, edx
	  add	edi, edx
	ENDM
	mov eax, [esi]
	mov [edi], eax
	mov eax, [esi+4]
	mov [edi+4], eax

	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8
	mov	esi, ebx	; restore esi
	retn

;----------------------------------------
	ALIGN 4
nf7:	;     8x8x1 (10 bytes)

	mov	ax, [esi]
	cmp	al, ah
	ja	nf23

if 0 ;debug
	add	esi, 10
	mov	eax, 0fefefefeH
	mov	ebx, eax
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfpk_mov8
	lea	edx, byte ptr ds:nf7_11+2

	mov	al, [esi+2]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_11-nf7_11)], bl
	mov	[edx+(nf7_12-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_13-nf7_11)], bl
	mov	[edx+(nf7_14-nf7_11)], bh

	mov	al, [esi+3]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_21-nf7_11)], bl
	mov	[edx+(nf7_22-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_23-nf7_11)], bl
	mov	[edx+(nf7_24-nf7_11)], bh

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_31-nf7_11)], bl
	mov	[edx+(nf7_32-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_33-nf7_11)], bl
	mov	[edx+(nf7_34-nf7_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_41-nf7_11)], bl
	mov	[edx+(nf7_42-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_43-nf7_11)], bl
	mov	[edx+(nf7_44-nf7_11)], bh

	lea	edx, [edx+(nf7_51-nf7_11)]

	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_51-nf7_51)], bl
	mov	[edx+(nf7_52-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_53-nf7_51)], bl
	mov	[edx+(nf7_54-nf7_51)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_61-nf7_51)], bl
	mov	[edx+(nf7_62-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_63-nf7_51)], bl
	mov	[edx+(nf7_64-nf7_51)], bh

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_71-nf7_51)], bl
	mov	[edx+(nf7_72-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_73-nf7_51)], bl
	mov	[edx+(nf7_74-nf7_51)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_81-nf7_51)], bl
	mov	[edx+(nf7_82-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_83-nf7_51)], bl
	mov	[edx+(nf7_84-nf7_51)], bh

	push	ebp
	push	esi
	; load bx,dx,cx,bp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	mov	cx, [esi]
	mov	esi,nf_width
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax
	jmp	nf7_0		; flush prefetch
	ALIGN 4
nf7_0:
nf7_11:	mov	ax, bx
	shl	eax, 16
nf7_12:	mov	ax, bx
	mov	[edi], eax
nf7_13:	mov	ax, bx
	shl	eax, 16
nf7_14:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_21:	mov	ax, bx
	shl	eax, 16
nf7_22:	mov	ax, bx
	mov	[edi], eax
nf7_23:	mov	ax, bx
	shl	eax, 16
nf7_24:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_31:	mov	ax, bx
	shl	eax, 16
nf7_32:	mov	ax, bx
	mov	[edi], eax
nf7_33:	mov	ax, bx
	shl	eax, 16
nf7_34:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_41:	mov	ax, bx
	shl	eax, 16
nf7_42:	mov	ax, bx
	mov	[edi], eax
nf7_43:	mov	ax, bx
	shl	eax, 16
nf7_44:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_51:	mov	ax, bx
	shl	eax, 16
nf7_52:	mov	ax, bx
	mov	[edi], eax
nf7_53:	mov	ax, bx
	shl	eax, 16
nf7_54:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_61:	mov	ax, bx
	shl	eax, 16
nf7_62:	mov	ax, bx
	mov	[edi], eax
nf7_63:	mov	ax, bx
	shl	eax, 16
nf7_64:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_71:	mov	ax, bx
	shl	eax, 16
nf7_72:	mov	ax, bx
	mov	[edi], eax
nf7_73:	mov	ax, bx
	shl	eax, 16
nf7_74:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_81:	mov	ax, bx
	shl	eax, 16
nf7_82:	mov	ax, bx
	mov	[edi], eax
nf7_83:	mov	ax, bx
	shl	eax, 16
nf7_84:	mov	ax, bx
	mov	[edi+4], eax

	pop	esi
	pop	ebp
	add	esi, 10
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf7+16
nf23:	; low 4x4x1 (4 bytes)

	xor	eax, eax
	lea	ecx, nfpk_mov4l
	lea	edx, byte ptr ds:nf23_11+2

	mov	al, [esi+2]
	and	al, 0fH
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_11-nf23_11)], bl
	mov	[edx+(nf23_12-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_13-nf23_11)], bl
	mov	[edx+(nf23_14-nf23_11)], bh

	mov	al, [esi+2]
	shr	al, 4
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_31-nf23_11)], bl
	mov	[edx+(nf23_32-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_33-nf23_11)], bl
	mov	[edx+(nf23_34-nf23_11)], bh


	mov	al, [esi+3]
	and	al, 0fH
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_51-nf23_11)], bl
	mov	[edx+(nf23_52-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_53-nf23_11)], bl
	mov	[edx+(nf23_54-nf23_11)], bh

	mov	al, [esi+3]
	shr	al, 4
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_71-nf23_11)], bl
	mov	[edx+(nf23_72-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_73-nf23_11)], bl
	mov	[edx+(nf23_74-nf23_11)], bh

	mov	edx, nf_width

	; load bx,cx with 00,11 color combinations
	mov	bx, [esi]
	mov	cl, bh
	mov	bh, bl
	mov	ch, cl

	jmp	nf23_0		; flush prefetch
	ALIGN 4
nf23_0:

nf23_11:mov	ax, bx
	shl	eax, 16
nf23_12:mov	ax, bx
	mov	[edi], eax
	mov	[edi+edx], eax

nf23_13:mov	ax, bx
	shl	eax, 16
nf23_14:mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	lea	edi, [edi+edx*2]

nf23_31:mov	ax, bx
	shl	eax, 16
nf23_32:mov	ax, bx
	mov	[edi], eax
	mov	[edi+edx], eax

nf23_33:mov	ax, bx
	shl	eax, 16
nf23_34:mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	lea	edi, [edi+edx*2]

nf23_51:mov	ax, bx
	shl	eax, 16
nf23_52:mov	ax, bx
	mov	[edi], eax
	mov	[edi+edx], eax

nf23_53:mov	ax, bx
	shl	eax, 16
nf23_54:mov	ax, bx
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	lea	edi, [edi+edx*2]

nf23_71:mov	ax, bx
	shl	eax, 16
nf23_72:mov	ax, bx
	mov	[edi], eax
	mov	[edi+edx], eax

nf23_73:mov	ax, bx
	shl	eax, 16
nf23_74:mov	ax, bx
	mov	[edi+4], eax
	add	edi, edx
	mov	[edi+4], eax

	sub	edi, nfpk_back_right
	add	esi, 4
	retn

;----------------------------------------
	ALIGN 4
nf8:	; 2x2 4x4x1 (16 bytes)

	mov	ax, [esi]
	cmp	al, ah
	ja	nf24

	xor	eax, eax
	lea	ecx, nfpk_mov8
	lea	edx, byte ptr ds:nf8_11+2	

	mov	al, [esi+2]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_11-nf8_11)], bl
	mov	[edx+(nf8_12-nf8_11)], bh
	shr	ebx, 16
	mov	[edx+(nf8_13-nf8_11)], bl
	mov	[edx+(nf8_14-nf8_11)], bh

	mov	al, [esi+3]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_21-nf8_11)], bl
	mov	[edx+(nf8_22-nf8_11)], bh
	shr	ebx, 16
	mov	[edx+(nf8_23-nf8_11)], bl
	mov	[edx+(nf8_24-nf8_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_31-nf8_11)], bl
	mov	[edx+(nf8_32-nf8_11)], bh
	shr	ebx, 16
	mov	[edx+(nf8_33-nf8_11)], bl
	mov	[edx+(nf8_34-nf8_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_41-nf8_11)], bl
	mov	[edx+(nf8_42-nf8_11)], bh
	shr	ebx, 16
	mov	[edx+(nf8_43-nf8_11)], bl
	mov	[edx+(nf8_44-nf8_11)], bh

	add	edx, nf8_51-nf8_11

	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_51-nf8_51)], bl
	mov	[edx+(nf8_52-nf8_51)], bh
	shr	ebx, 16
	mov	[edx+(nf8_53-nf8_51)], bl
	mov	[edx+(nf8_54-nf8_51)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_61-nf8_51)], bl
	mov	[edx+(nf8_62-nf8_51)], bh
	shr	ebx, 16
	mov	[edx+(nf8_63-nf8_51)], bl
	mov	[edx+(nf8_64-nf8_51)], bh


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_71-nf8_51)], bl
	mov	[edx+(nf8_72-nf8_51)], bh
	shr	ebx, 16
	mov	[edx+(nf8_73-nf8_51)], bl
	mov	[edx+(nf8_74-nf8_51)], bh

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_81-nf8_51)], bl
	mov	[edx+(nf8_82-nf8_51)], bh
	shr	ebx, 16
	mov	[edx+(nf8_83-nf8_51)], bl
	mov	[edx+(nf8_84-nf8_51)], bh


	push	ebp
	push	esi
	; load bx,dx,cx,bp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	mov	cx, [esi]
	mov	esi, nf_width
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

	jmp	nf8_0		; flush prefetch
	ALIGN 4
nf8_0:
nf8_11:	mov	ax, bx
	shl	eax, 16
nf8_12:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf8_13:	mov	ax, bx
	shl	eax, 16
nf8_14:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf8_21:	mov	ax, bx
	shl	eax, 16
nf8_22:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf8_23:	mov	ax, bx
	shl	eax, 16
nf8_24:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

	mov	eax, [esp]
	mov	cx, [eax+4]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf8_31:	mov	ax, bx
	shl	eax, 16
nf8_32:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf8_33:	mov	ax, bx
	shl	eax, 16
nf8_34:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf8_41:	mov	ax, bx
	shl	eax, 16
nf8_42:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf8_43:	mov	ax, bx
	shl	eax, 16
nf8_44:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

	lea	eax, [esi*8-4]
	sub	edi, eax

	mov	eax, [esp]
	mov	cx, [eax+8]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf8_51:	mov	ax, bx
	shl	eax, 16
nf8_52:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf8_53:	mov	ax, bx
	shl	eax, 16
nf8_54:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf8_61:	mov	ax, bx
	shl	eax, 16
nf8_62:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf8_63:	mov	ax, bx
	shl	eax, 16
nf8_64:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

	mov	eax, [esp]
	mov	cx, [eax+12]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf8_71:	mov	ax, bx
	shl	eax, 16
nf8_72:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf8_73:	mov	ax, bx
	shl	eax, 16
nf8_74:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf8_81:	mov	ax, bx
	shl	eax, 16
nf8_82:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf8_83:	mov	ax, bx
	shl	eax, 16
nf8_84:	mov	ax, bx
	mov	[edi], eax

	pop	esi
	pop	ebp
	add	esi, 16
	sub	edi, 4
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf8+16
nf24:	; 2x1 4x8x1 (12 bytes)

	mov	ax, [esi+6]
	cmp	al, ah
	ja	nf40

	xor	eax, eax
	lea	ecx, nfpk_mov8
	lea	edx, byte ptr ds:nf24_11+2	

	mov	al, [esi+2]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_11-nf24_11)], bl
	mov	[edx+(nf24_12-nf24_11)], bh
	shr	ebx, 16
	mov	[edx+(nf24_13-nf24_11)], bl
	mov	[edx+(nf24_14-nf24_11)], bh

	mov	al, [esi+3]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_21-nf24_11)], bl
	mov	[edx+(nf24_22-nf24_11)], bh
	shr	ebx, 16
	mov	[edx+(nf24_23-nf24_11)], bl
	mov	[edx+(nf24_24-nf24_11)], bh


	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_31-nf24_11)], bl
	mov	[edx+(nf24_32-nf24_11)], bh
	shr	ebx, 16
	mov	[edx+(nf24_33-nf24_11)], bl
	mov	[edx+(nf24_34-nf24_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_41-nf24_11)], bl
	mov	[edx+(nf24_42-nf24_11)], bh
	shr	ebx, 16
	mov	[edx+(nf24_43-nf24_11)], bl
	mov	[edx+(nf24_44-nf24_11)], bh

	add	edx, nf24_51-nf24_11

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_51-nf24_51)], bl
	mov	[edx+(nf24_52-nf24_51)], bh
	shr	ebx, 16
	mov	[edx+(nf24_53-nf24_51)], bl
	mov	[edx+(nf24_54-nf24_51)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_61-nf24_51)], bl
	mov	[edx+(nf24_62-nf24_51)], bh
	shr	ebx, 16
	mov	[edx+(nf24_63-nf24_51)], bl
	mov	[edx+(nf24_64-nf24_51)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_71-nf24_51)], bl
	mov	[edx+(nf24_72-nf24_51)], bh
	shr	ebx, 16
	mov	[edx+(nf24_73-nf24_51)], bl
	mov	[edx+(nf24_74-nf24_51)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_81-nf24_51)], bl
	mov	[edx+(nf24_82-nf24_51)], bh
	shr	ebx, 16
	mov	[edx+(nf24_83-nf24_51)], bl
	mov	[edx+(nf24_84-nf24_51)], bh


	push	ebp
	push	esi
	; load bx,dx,cx,bp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	mov	cx, [esi]
	mov	esi, nf_width
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

	jmp	nf24_0		; flush prefetch
	ALIGN 4
nf24_0:
nf24_11:mov	ax, bx
	shl	eax, 16
nf24_12:mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf24_13:mov	ax, bx
	shl	eax, 16
nf24_14:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_21:mov	ax, bx
	shl	eax, 16
nf24_22:mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf24_23:mov	ax, bx
	shl	eax, 16
nf24_24:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_31:mov	ax, bx
	shl	eax, 16
nf24_32:mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf24_33:mov	ax, bx
	shl	eax, 16
nf24_34:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_41:mov	ax, bx
	shl	eax, 16
nf24_42:mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf24_43:mov	ax, bx
	shl	eax, 16
nf24_44:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

	lea	eax, [esi*8-4]
	sub	edi, eax

	mov	eax, [esp]
	mov	cx, [eax+6]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf24_51:mov	ax, bx
	shl	eax, 16
nf24_52:mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf24_53:mov	ax, bx
	shl	eax, 16
nf24_54:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_61:mov	ax, bx
	shl	eax, 16
nf24_62:mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf24_63:mov	ax, bx
	shl	eax, 16
nf24_64:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_71:mov	ax, bx
	shl	eax, 16
nf24_72:mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf24_73:mov	ax, bx
	shl	eax, 16
nf24_74:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_81:mov	ax, bx
	shl	eax, 16
nf24_82:mov	ax, bx
	mov	[edi], eax
	add	edi, esi
nf24_83:mov	ax, bx
	shl	eax, 16
nf24_84:mov	ax, bx
	mov	[edi], eax

	pop	esi
	pop	ebp
	add	esi, 12
	sub	edi, 4
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf8+32
nf40:	; 1x2 8x4x1 (12 bytes)

	xor	eax, eax
	lea	ecx, nfpk_mov8
	lea	edx, byte ptr ds:nf40_11+2	

	mov	al, [esi+2]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_11-nf40_11)], bl
	mov	[edx+(nf40_12-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_13-nf40_11)], bl
	mov	[edx+(nf40_14-nf40_11)], bh

	mov	al, [esi+3]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_21-nf40_11)], bl
	mov	[edx+(nf40_22-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_23-nf40_11)], bl
	mov	[edx+(nf40_24-nf40_11)], bh


	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_31-nf40_11)], bl
	mov	[edx+(nf40_32-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_33-nf40_11)], bl
	mov	[edx+(nf40_34-nf40_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_41-nf40_11)], bl
	mov	[edx+(nf40_42-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_43-nf40_11)], bl
	mov	[edx+(nf40_44-nf40_11)], bh

	add	edx, nf40_51-nf40_11

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_51-nf40_51)], bl
	mov	[edx+(nf40_52-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_53-nf40_51)], bl
	mov	[edx+(nf40_54-nf40_51)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_61-nf40_51)], bl
	mov	[edx+(nf40_62-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_63-nf40_51)], bl
	mov	[edx+(nf40_64-nf40_51)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_71-nf40_51)], bl
	mov	[edx+(nf40_72-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_73-nf40_51)], bl
	mov	[edx+(nf40_74-nf40_51)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_81-nf40_51)], bl
	mov	[edx+(nf40_82-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_83-nf40_51)], bl
	mov	[edx+(nf40_84-nf40_51)], bh


	push	ebp
	push	esi
	; load bx,dx,cx,bp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	mov	cx, [esi]
	mov	esi, nf_width
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

	jmp	nf40_0		; flush prefetch
	ALIGN 4
nf40_0:
nf40_11:mov	ax, bx
	shl	eax, 16
nf40_12:mov	ax, bx
	mov	[edi], eax
nf40_13:mov	ax, bx
	shl	eax, 16
nf40_14:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf40_21:mov	ax, bx
	shl	eax, 16
nf40_22:mov	ax, bx
	mov	[edi], eax
nf40_23:mov	ax, bx
	shl	eax, 16
nf40_24:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf40_31:mov	ax, bx
	shl	eax, 16
nf40_32:mov	ax, bx
	mov	[edi], eax
nf40_33:mov	ax, bx
	shl	eax, 16
nf40_34:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf40_41:mov	ax, bx
	shl	eax, 16
nf40_42:mov	ax, bx
	mov	[edi], eax
nf40_43:mov	ax, bx
	shl	eax, 16
nf40_44:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

	mov	eax, [esp]
	mov	cx, [eax+6]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf40_51:mov	ax, bx
	shl	eax, 16
nf40_52:mov	ax, bx
	mov	[edi], eax
nf40_53:mov	ax, bx
	shl	eax, 16
nf40_54:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf40_61:mov	ax, bx
	shl	eax, 16
nf40_62:mov	ax, bx
	mov	[edi], eax
nf40_63:mov	ax, bx
	shl	eax, 16
nf40_64:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf40_71:mov	ax, bx
	shl	eax, 16
nf40_72:mov	ax, bx
	mov	[edi], eax
nf40_73:mov	ax, bx
	shl	eax, 16
nf40_74:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf40_81:mov	ax, bx
	shl	eax, 16
nf40_82:mov	ax, bx
	mov	[edi], eax
nf40_83:mov	ax, bx
	shl	eax, 16
nf40_84:mov	ax, bx
	mov	[edi+4], eax

	pop	esi
	pop	ebp
	add	esi, 12
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
nf9:	;     8x8x2 (20 bytes)

	mov	eax, [esi]
	cmp	al, ah
	ja	nf41

	shr	eax, 16
	cmp	al, ah
	ja	nf25

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf9_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_11-nf9_11)], bl
	mov	[edx+(nf9_12-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_13-nf9_11)], bl
	mov	[edx+(nf9_14-nf9_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_15-nf9_11)], bl
	mov	[edx+(nf9_16-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_17-nf9_11)], bl
	mov	[edx+(nf9_18-nf9_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_21-nf9_11)], bl
	mov	[edx+(nf9_22-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_23-nf9_11)], bl
	mov	[edx+(nf9_24-nf9_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_25-nf9_11)], bl
	mov	[edx+(nf9_26-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_27-nf9_11)], bl
	mov	[edx+(nf9_28-nf9_11)], bh


	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_31-nf9_11)], bl
	mov	[edx+(nf9_32-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_33-nf9_11)], bl
	mov	[edx+(nf9_34-nf9_11)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_35-nf9_11)], bl
	mov	[edx+(nf9_36-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_37-nf9_11)], bl
	mov	[edx+(nf9_38-nf9_11)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_41-nf9_11)], bl
	mov	[edx+(nf9_42-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_43-nf9_11)], bl
	mov	[edx+(nf9_44-nf9_11)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_45-nf9_11)], bl
	mov	[edx+(nf9_46-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_47-nf9_11)], bl
	mov	[edx+(nf9_48-nf9_11)], bh


	lea	edx, [edx+(nf9_51-nf9_11)]

	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_51-nf9_51)], bl
	mov	[edx+(nf9_52-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_53-nf9_51)], bl
	mov	[edx+(nf9_54-nf9_51)], bh

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_55-nf9_51)], bl
	mov	[edx+(nf9_56-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_57-nf9_51)], bl
	mov	[edx+(nf9_58-nf9_51)], bh


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_61-nf9_51)], bl
	mov	[edx+(nf9_62-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_63-nf9_51)], bl
	mov	[edx+(nf9_64-nf9_51)], bh

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_65-nf9_51)], bl
	mov	[edx+(nf9_66-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_67-nf9_51)], bl
	mov	[edx+(nf9_68-nf9_51)], bh


	mov	al, [esi+16]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_71-nf9_51)], bl
	mov	[edx+(nf9_72-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_73-nf9_51)], bl
	mov	[edx+(nf9_74-nf9_51)], bh

	mov	al, [esi+17]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_75-nf9_51)], bl
	mov	[edx+(nf9_76-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_77-nf9_51)], bl
	mov	[edx+(nf9_78-nf9_51)], bh


	mov	al, [esi+18]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_81-nf9_51)], bl
	mov	[edx+(nf9_82-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_83-nf9_51)], bl
	mov	[edx+(nf9_84-nf9_51)], bh

	mov	al, [esi+19]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_85-nf9_51)], bl
	mov	[edx+(nf9_86-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_87-nf9_51)], bl
	mov	[edx+(nf9_88-nf9_51)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf9_0		; flush prefetch
	ALIGN 4
nf9_0:
nf9_11:	mov	al, bl
nf9_12:	mov	ah, bl
	shl	eax, 16
nf9_13:	mov	al, bl
nf9_14:	mov	ah, bl
	mov	[edi], eax

nf9_15:	mov	al, bl
nf9_16:	mov	ah, bl
	shl	eax, 16
nf9_17:	mov	al, bl
nf9_18:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_21:	mov	al, bl
nf9_22:	mov	ah, bl
	shl	eax, 16
nf9_23:	mov	al, bl
nf9_24:	mov	ah, bl
	mov	[edi], eax

nf9_25:	mov	al, bl
nf9_26:	mov	ah, bl
	shl	eax, 16
nf9_27:	mov	al, bl
nf9_28:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_31:	mov	al, bl
nf9_32:	mov	ah, bl
	shl	eax, 16
nf9_33:	mov	al, bl
nf9_34:	mov	ah, bl
	mov	[edi], eax

nf9_35:	mov	al, bl
nf9_36:	mov	ah, bl
	shl	eax, 16
nf9_37:	mov	al, bl
nf9_38:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_41:	mov	al, bl
nf9_42:	mov	ah, bl
	shl	eax, 16
nf9_43:	mov	al, bl
nf9_44:	mov	ah, bl
	mov	[edi], eax

nf9_45:	mov	al, bl
nf9_46:	mov	ah, bl
	shl	eax, 16
nf9_47:	mov	al, bl
nf9_48:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_51:	mov	al, bl
nf9_52:	mov	ah, bl
	shl	eax, 16
nf9_53:	mov	al, bl
nf9_54:	mov	ah, bl
	mov	[edi], eax

nf9_55:	mov	al, bl
nf9_56:	mov	ah, bl
	shl	eax, 16
nf9_57:	mov	al, bl
nf9_58:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_61:	mov	al, bl
nf9_62:	mov	ah, bl
	shl	eax, 16
nf9_63:	mov	al, bl
nf9_64:	mov	ah, bl
	mov	[edi], eax

nf9_65:	mov	al, bl
nf9_66:	mov	ah, bl
	shl	eax, 16
nf9_67:	mov	al, bl
nf9_68:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_71:	mov	al, bl
nf9_72:	mov	ah, bl
	shl	eax, 16
nf9_73:	mov	al, bl
nf9_74:	mov	ah, bl
	mov	[edi], eax

nf9_75:	mov	al, bl
nf9_76:	mov	ah, bl
	shl	eax, 16
nf9_77:	mov	al, bl
nf9_78:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_81:	mov	al, bl
nf9_82:	mov	ah, bl
	shl	eax, 16
nf9_83:	mov	al, bl
nf9_84:	mov	ah, bl
	mov	[edi], eax

nf9_85:	mov	al, bl
nf9_86:	mov	ah, bl
	shl	eax, 16
nf9_87:	mov	al, bl
nf9_88:	mov	ah, bl
	mov	[edi+4], eax

	add	esi, 20
	sub	edi, nfpk_back_right
	retn

;----------------------------------------
	ALIGN 4
;nf9+16
nf25:	; low 4x4x2 (8 bytes)

if 0 ;debug
	mov	eax, 0
	mov	ebx, 0
	add	esi, 8
	jmp	nf_solid
endif

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf25_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_14-nf25_11)], bl
	mov	[edx+(nf25_13-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_12-nf25_11)], bl
	mov	[edx+(nf25_11-nf25_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_24-nf25_11)], bl
	mov	[edx+(nf25_23-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_22-nf25_11)], bl
	mov	[edx+(nf25_21-nf25_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_34-nf25_11)], bl
	mov	[edx+(nf25_33-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_32-nf25_11)], bl
	mov	[edx+(nf25_31-nf25_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_44-nf25_11)], bl
	mov	[edx+(nf25_43-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_42-nf25_11)], bl
	mov	[edx+(nf25_41-nf25_11)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf25_0		; flush prefetch
	ALIGN 4
nf25_0:
nf25_11:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_12:mov	al, bl
	mov	ah, al
	mov	[edi], eax
	mov	[edi+edx], eax
nf25_13:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_14:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	lea	edi, [edi+edx*2]

nf25_21:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_22:mov	al, bl
	mov	ah, al
	mov	[edi], eax
	mov	[edi+edx], eax
nf25_23:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_24:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	lea	edi, [edi+edx*2]

nf25_31:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_32:mov	al, bl
	mov	ah, al
	mov	[edi], eax
	mov	[edi+edx], eax
nf25_33:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_34:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	lea	edi, [edi+edx*2]

nf25_41:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_42:mov	al, bl
	mov	ah, al
	mov	[edi], eax
	mov	[edi+edx], eax
nf25_43:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_44:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	add	edi, edx

	add	esi, 8
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf9+32
nf41:	; low 4x8x2 (12 bytes)
	shr	eax, 16
	cmp	al, ah
	ja	nf57

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf41_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_14-nf41_11)], bl
	mov	[edx+(nf41_13-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_12-nf41_11)], bl
	mov	[edx+(nf41_11-nf41_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_24-nf41_11)], bl
	mov	[edx+(nf41_23-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_22-nf41_11)], bl
	mov	[edx+(nf41_21-nf41_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_34-nf41_11)], bl
	mov	[edx+(nf41_33-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_32-nf41_11)], bl
	mov	[edx+(nf41_31-nf41_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_44-nf41_11)], bl
	mov	[edx+(nf41_43-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_42-nf41_11)], bl
	mov	[edx+(nf41_41-nf41_11)], bh

	lea	edx, [edx+(nf41_51-nf41_11)]

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_54-nf41_51)], bl
	mov	[edx+(nf41_53-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_52-nf41_51)], bl
	mov	[edx+(nf41_51-nf41_51)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_64-nf41_51)], bl
	mov	[edx+(nf41_63-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_62-nf41_51)], bl
	mov	[edx+(nf41_61-nf41_51)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_74-nf41_51)], bl
	mov	[edx+(nf41_73-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_72-nf41_51)], bl
	mov	[edx+(nf41_71-nf41_51)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_84-nf41_51)], bl
	mov	[edx+(nf41_83-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_82-nf41_51)], bl
	mov	[edx+(nf41_81-nf41_51)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf41_0		; flush prefetch
	ALIGN 4
nf41_0:
nf41_11:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_12:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_13:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_14:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_21:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_22:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_23:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_24:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_31:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_32:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_33:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_34:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_41:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_42:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_43:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_44:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_51:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_52:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_53:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_54:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_61:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_62:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_63:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_64:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_71:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_72:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_73:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_74:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_81:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_82:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_83:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_84:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax

	add	esi, 12
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf9+48
nf57:	; low 8x4x2 (12 bytes)
	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf57_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_11-nf57_11)], bl
	mov	[edx+(nf57_12-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_13-nf57_11)], bl
	mov	[edx+(nf57_14-nf57_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_15-nf57_11)], bl
	mov	[edx+(nf57_16-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_17-nf57_11)], bl
	mov	[edx+(nf57_18-nf57_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_21-nf57_11)], bl
	mov	[edx+(nf57_22-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_23-nf57_11)], bl
	mov	[edx+(nf57_24-nf57_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_25-nf57_11)], bl
	mov	[edx+(nf57_26-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_27-nf57_11)], bl
	mov	[edx+(nf57_28-nf57_11)], bh


	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_31-nf57_11)], bl
	mov	[edx+(nf57_32-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_33-nf57_11)], bl
	mov	[edx+(nf57_34-nf57_11)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_35-nf57_11)], bl
	mov	[edx+(nf57_36-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_37-nf57_11)], bl
	mov	[edx+(nf57_38-nf57_11)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_41-nf57_11)], bl
	mov	[edx+(nf57_42-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_43-nf57_11)], bl
	mov	[edx+(nf57_44-nf57_11)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_45-nf57_11)], bl
	mov	[edx+(nf57_46-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_47-nf57_11)], bl
	mov	[edx+(nf57_48-nf57_11)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf57_0		; flush prefetch
	ALIGN 4
nf57_0:
nf57_11:mov	al, bl
nf57_12:mov	ah, bl
	shl	eax, 16
nf57_13:mov	al, bl
nf57_14:mov	ah, bl
	mov	[edi], eax
	mov	[edi+edx], eax

nf57_15:mov	al, bl
nf57_16:mov	ah, bl
	shl	eax, 16
nf57_17:mov	al, bl
nf57_18:mov	ah, bl
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	lea	edi, [edi+edx*2]

nf57_21:mov	al, bl
nf57_22:mov	ah, bl
	shl	eax, 16
nf57_23:mov	al, bl
nf57_24:mov	ah, bl
	mov	[edi], eax
	mov	[edi+edx], eax

nf57_25:mov	al, bl
nf57_26:mov	ah, bl
	shl	eax, 16
nf57_27:mov	al, bl
nf57_28:mov	ah, bl
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	lea	edi, [edi+edx*2]

nf57_31:mov	al, bl
nf57_32:mov	ah, bl
	shl	eax, 16
nf57_33:mov	al, bl
nf57_34:mov	ah, bl
	mov	[edi], eax
	mov	[edi+edx], eax

nf57_35:mov	al, bl
nf57_36:mov	ah, bl
	shl	eax, 16
nf57_37:mov	al, bl
nf57_38:mov	ah, bl
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	lea	edi, [edi+edx*2]

nf57_41:mov	al, bl
nf57_42:mov	ah, bl
	shl	eax, 16
nf57_43:mov	al, bl
nf57_44:mov	ah, bl
	mov	[edi], eax
	mov	[edi+edx], eax

nf57_45:mov	al, bl
nf57_46:mov	ah, bl
	shl	eax, 16
nf57_47:mov	al, bl
nf57_48:mov	ah, bl
	mov	[edi+4], eax
	mov	[edi+edx+4], eax
	add	edi, edx

	add	esi, 12
	sub	edi, nfpk_back_right
	retn

;----------------------------------------
	ALIGN 4
nf10:	; 2x2 4x4x2 (32 bytes)

	mov	ax, [esi]
	cmp	al, ah
	ja	nf26

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf10_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_11-nf10_11)], bl
	mov	[edx+(nf10_12-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_13-nf10_11)], bl
	mov	[edx+(nf10_14-nf10_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_15-nf10_11)], bl
	mov	[edx+(nf10_16-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_17-nf10_11)], bl
	mov	[edx+(nf10_18-nf10_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_21-nf10_11)], bl
	mov	[edx+(nf10_22-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_23-nf10_11)], bl
	mov	[edx+(nf10_24-nf10_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_25-nf10_11)], bl
	mov	[edx+(nf10_26-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_27-nf10_11)], bl
	mov	[edx+(nf10_28-nf10_11)], bh


	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_31-nf10_11)], bl
	mov	[edx+(nf10_32-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_33-nf10_11)], bl
	mov	[edx+(nf10_34-nf10_11)], bh

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_35-nf10_11)], bl
	mov	[edx+(nf10_36-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_37-nf10_11)], bl
	mov	[edx+(nf10_38-nf10_11)], bh


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_41-nf10_11)], bl
	mov	[edx+(nf10_42-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_43-nf10_11)], bl
	mov	[edx+(nf10_44-nf10_11)], bh

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_45-nf10_11)], bl
	mov	[edx+(nf10_46-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_47-nf10_11)], bl
	mov	[edx+(nf10_48-nf10_11)], bh


	lea	edx, [edx+(nf10_51-nf10_11)]

	mov	al, [esi+20]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_51-nf10_51)], bl
	mov	[edx+(nf10_52-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_53-nf10_51)], bl
	mov	[edx+(nf10_54-nf10_51)], bh

	mov	al, [esi+21]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_55-nf10_51)], bl
	mov	[edx+(nf10_56-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_57-nf10_51)], bl
	mov	[edx+(nf10_58-nf10_51)], bh


	mov	al, [esi+22]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_61-nf10_51)], bl
	mov	[edx+(nf10_62-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_63-nf10_51)], bl
	mov	[edx+(nf10_64-nf10_51)], bh

	mov	al, [esi+23]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_65-nf10_51)], bl
	mov	[edx+(nf10_66-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_67-nf10_51)], bl
	mov	[edx+(nf10_68-nf10_51)], bh


	mov	al, [esi+28]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_71-nf10_51)], bl
	mov	[edx+(nf10_72-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_73-nf10_51)], bl
	mov	[edx+(nf10_74-nf10_51)], bh

	mov	al, [esi+29]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_75-nf10_51)], bl
	mov	[edx+(nf10_76-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_77-nf10_51)], bl
	mov	[edx+(nf10_78-nf10_51)], bh


	mov	al, [esi+30]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_81-nf10_51)], bl
	mov	[edx+(nf10_82-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_83-nf10_51)], bl
	mov	[edx+(nf10_84-nf10_51)], bh

	mov	al, [esi+31]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_85-nf10_51)], bl
	mov	[edx+(nf10_86-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_87-nf10_51)], bl
	mov	[edx+(nf10_88-nf10_51)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf10_0		; flush prefetch
	ALIGN 4
nf10_0:
nf10_11:mov	al, bl
nf10_12:mov	ah, bl
	shl	eax, 16
nf10_13:mov	al, bl
nf10_14:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_15:mov	al, bl
nf10_16:mov	ah, bl
	shl	eax, 16
nf10_17:mov	al, bl
nf10_18:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_21:mov	al, bl
nf10_22:mov	ah, bl
	shl	eax, 16
nf10_23:mov	al, bl
nf10_24:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_25:mov	al, bl
nf10_26:mov	ah, bl
	shl	eax, 16
nf10_27:mov	al, bl
nf10_28:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+8]
	mov	cx, [esi+10]

nf10_31:mov	al, bl
nf10_32:mov	ah, bl
	shl	eax, 16
nf10_33:mov	al, bl
nf10_34:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_35:mov	al, bl
nf10_36:mov	ah, bl
	shl	eax, 16
nf10_37:mov	al, bl
nf10_38:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_41:mov	al, bl
nf10_42:mov	ah, bl
	shl	eax, 16
nf10_43:mov	al, bl
nf10_44:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_45:mov	al, bl
nf10_46:mov	ah, bl
	shl	eax, 16
nf10_47:mov	al, bl
nf10_48:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

	lea	eax, [edx*8-4]
	sub	edi, eax

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+16]
	mov	cx, [esi+18]

nf10_51:mov	al, bl
nf10_52:mov	ah, bl
	shl	eax, 16
nf10_53:mov	al, bl
nf10_54:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_55:mov	al, bl
nf10_56:mov	ah, bl
	shl	eax, 16
nf10_57:mov	al, bl
nf10_58:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_61:mov	al, bl
nf10_62:mov	ah, bl
	shl	eax, 16
nf10_63:mov	al, bl
nf10_64:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_65:mov	al, bl
nf10_66:mov	ah, bl
	shl	eax, 16
nf10_67:mov	al, bl
nf10_68:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+24]
	mov	cx, [esi+26]

nf10_71:mov	al, bl
nf10_72:mov	ah, bl
	shl	eax, 16
nf10_73:mov	al, bl
nf10_74:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_75:mov	al, bl
nf10_76:mov	ah, bl
	shl	eax, 16
nf10_77:mov	al, bl
nf10_78:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_81:mov	al, bl
nf10_82:mov	ah, bl
	shl	eax, 16
nf10_83:mov	al, bl
nf10_84:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_85:mov	al, bl
nf10_86:mov	ah, bl
	shl	eax, 16
nf10_87:mov	al, bl
nf10_88:mov	ah, bl
	mov	[edi], eax

	add	esi, 32
	sub	edi, 4
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf10+16
nf26:	; 2x1 4x8x2 (24 bytes)

	mov	ax, [esi+12]
	cmp	al, ah
	ja	nf42

if 0 ;debug
	mov	eax, 0
	mov	ebx, 0
	add	esi, 24
	jmp	nf_solid
endif

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf26_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_11-nf26_11)], bl
	mov	[edx+(nf26_12-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_13-nf26_11)], bl
	mov	[edx+(nf26_14-nf26_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_15-nf26_11)], bl
	mov	[edx+(nf26_16-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_17-nf26_11)], bl
	mov	[edx+(nf26_18-nf26_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_21-nf26_11)], bl
	mov	[edx+(nf26_22-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_23-nf26_11)], bl
	mov	[edx+(nf26_24-nf26_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_25-nf26_11)], bl
	mov	[edx+(nf26_26-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_27-nf26_11)], bl
	mov	[edx+(nf26_28-nf26_11)], bh

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_31-nf26_11)], bl
	mov	[edx+(nf26_32-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_33-nf26_11)], bl
	mov	[edx+(nf26_34-nf26_11)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_35-nf26_11)], bl
	mov	[edx+(nf26_36-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_37-nf26_11)], bl
	mov	[edx+(nf26_38-nf26_11)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_41-nf26_11)], bl
	mov	[edx+(nf26_42-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_43-nf26_11)], bl
	mov	[edx+(nf26_44-nf26_11)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_45-nf26_11)], bl
	mov	[edx+(nf26_46-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_47-nf26_11)], bl
	mov	[edx+(nf26_48-nf26_11)], bh


	lea	edx, [edx+(nf26_51-nf26_11)]

	mov	al, [esi+16]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_51-nf26_51)], bl
	mov	[edx+(nf26_52-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_53-nf26_51)], bl
	mov	[edx+(nf26_54-nf26_51)], bh

	mov	al, [esi+17]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_55-nf26_51)], bl
	mov	[edx+(nf26_56-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_57-nf26_51)], bl
	mov	[edx+(nf26_58-nf26_51)], bh


	mov	al, [esi+18]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_61-nf26_51)], bl
	mov	[edx+(nf26_62-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_63-nf26_51)], bl
	mov	[edx+(nf26_64-nf26_51)], bh

	mov	al, [esi+19]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_65-nf26_51)], bl
	mov	[edx+(nf26_66-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_67-nf26_51)], bl
	mov	[edx+(nf26_68-nf26_51)], bh


	mov	al, [esi+20]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_71-nf26_51)], bl
	mov	[edx+(nf26_72-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_73-nf26_51)], bl
	mov	[edx+(nf26_74-nf26_51)], bh

	mov	al, [esi+21]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_75-nf26_51)], bl
	mov	[edx+(nf26_76-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_77-nf26_51)], bl
	mov	[edx+(nf26_78-nf26_51)], bh


	mov	al, [esi+22]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_81-nf26_51)], bl
	mov	[edx+(nf26_82-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_83-nf26_51)], bl
	mov	[edx+(nf26_84-nf26_51)], bh

	mov	al, [esi+23]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_85-nf26_51)], bl
	mov	[edx+(nf26_86-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_87-nf26_51)], bl
	mov	[edx+(nf26_88-nf26_51)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf26_0		; flush prefetch
	ALIGN 4
nf26_0:
nf26_11:mov	al, bl
nf26_12:mov	ah, bl
	shl	eax, 16
nf26_13:mov	al, bl
nf26_14:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_15:mov	al, bl
nf26_16:mov	ah, bl
	shl	eax, 16
nf26_17:mov	al, bl
nf26_18:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_21:mov	al, bl
nf26_22:mov	ah, bl
	shl	eax, 16
nf26_23:mov	al, bl
nf26_24:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_25:mov	al, bl
nf26_26:mov	ah, bl
	shl	eax, 16
nf26_27:mov	al, bl
nf26_28:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_31:mov	al, bl
nf26_32:mov	ah, bl
	shl	eax, 16
nf26_33:mov	al, bl
nf26_34:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_35:mov	al, bl
nf26_36:mov	ah, bl
	shl	eax, 16
nf26_37:mov	al, bl
nf26_38:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_41:mov	al, bl
nf26_42:mov	ah, bl
	shl	eax, 16
nf26_43:mov	al, bl
nf26_44:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_45:mov	al, bl
nf26_46:mov	ah, bl
	shl	eax, 16
nf26_47:mov	al, bl
nf26_48:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

	lea	eax, [edx*8-4]
	sub	edi, eax

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+12]
	mov	cx, [esi+14]

nf26_51:mov	al, bl
nf26_52:mov	ah, bl
	shl	eax, 16
nf26_53:mov	al, bl
nf26_54:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_55:mov	al, bl
nf26_56:mov	ah, bl
	shl	eax, 16
nf26_57:mov	al, bl
nf26_58:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_61:mov	al, bl
nf26_62:mov	ah, bl
	shl	eax, 16
nf26_63:mov	al, bl
nf26_64:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_65:mov	al, bl
nf26_66:mov	ah, bl
	shl	eax, 16
nf26_67:mov	al, bl
nf26_68:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_71:mov	al, bl
nf26_72:mov	ah, bl
	shl	eax, 16
nf26_73:mov	al, bl
nf26_74:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_75:mov	al, bl
nf26_76:mov	ah, bl
	shl	eax, 16
nf26_77:mov	al, bl
nf26_78:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_81:mov	al, bl
nf26_82:mov	ah, bl
	shl	eax, 16
nf26_83:mov	al, bl
nf26_84:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_85:mov	al, bl
nf26_86:mov	ah, bl
	shl	eax, 16
nf26_87:mov	al, bl
nf26_88:mov	ah, bl
	mov	[edi], eax

	add	esi, 24
	sub	edi, 4
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf10+32
nf42:	; 1x2 8x4x2 (24 bytes)

if 0 ;debug
	mov	eax, 0
	mov	ebx, 0
	add	esi, 24
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf42_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_11-nf42_11)], bl
	mov	[edx+(nf42_12-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_13-nf42_11)], bl
	mov	[edx+(nf42_14-nf42_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_15-nf42_11)], bl
	mov	[edx+(nf42_16-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_17-nf42_11)], bl
	mov	[edx+(nf42_18-nf42_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_21-nf42_11)], bl
	mov	[edx+(nf42_22-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_23-nf42_11)], bl
	mov	[edx+(nf42_24-nf42_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_25-nf42_11)], bl
	mov	[edx+(nf42_26-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_27-nf42_11)], bl
	mov	[edx+(nf42_28-nf42_11)], bh


	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_31-nf42_11)], bl
	mov	[edx+(nf42_32-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_33-nf42_11)], bl
	mov	[edx+(nf42_34-nf42_11)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_35-nf42_11)], bl
	mov	[edx+(nf42_36-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_37-nf42_11)], bl
	mov	[edx+(nf42_38-nf42_11)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_41-nf42_11)], bl
	mov	[edx+(nf42_42-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_43-nf42_11)], bl
	mov	[edx+(nf42_44-nf42_11)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_45-nf42_11)], bl
	mov	[edx+(nf42_46-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_47-nf42_11)], bl
	mov	[edx+(nf42_48-nf42_11)], bh


	lea	edx, [edx+(nf42_51-nf42_11)]

	mov	al, [esi+16]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_51-nf42_51)], bl
	mov	[edx+(nf42_52-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_53-nf42_51)], bl
	mov	[edx+(nf42_54-nf42_51)], bh

	mov	al, [esi+17]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_55-nf42_51)], bl
	mov	[edx+(nf42_56-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_57-nf42_51)], bl
	mov	[edx+(nf42_58-nf42_51)], bh


	mov	al, [esi+18]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_61-nf42_51)], bl
	mov	[edx+(nf42_62-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_63-nf42_51)], bl
	mov	[edx+(nf42_64-nf42_51)], bh

	mov	al, [esi+19]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_65-nf42_51)], bl
	mov	[edx+(nf42_66-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_67-nf42_51)], bl
	mov	[edx+(nf42_68-nf42_51)], bh


	mov	al, [esi+20]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_71-nf42_51)], bl
	mov	[edx+(nf42_72-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_73-nf42_51)], bl
	mov	[edx+(nf42_74-nf42_51)], bh

	mov	al, [esi+21]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_75-nf42_51)], bl
	mov	[edx+(nf42_76-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_77-nf42_51)], bl
	mov	[edx+(nf42_78-nf42_51)], bh


	mov	al, [esi+22]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_81-nf42_51)], bl
	mov	[edx+(nf42_82-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_83-nf42_51)], bl
	mov	[edx+(nf42_84-nf42_51)], bh

	mov	al, [esi+23]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_85-nf42_51)], bl
	mov	[edx+(nf42_86-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_87-nf42_51)], bl
	mov	[edx+(nf42_88-nf42_51)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf42_0		; flush prefetch
	ALIGN 4
nf42_0:
nf42_11:mov	al, bl
nf42_12:mov	ah, bl
	shl	eax, 16
nf42_13:mov	al, bl
nf42_14:mov	ah, bl
	mov	[edi], eax

nf42_15:mov	al, bl
nf42_16:mov	ah, bl
	shl	eax, 16
nf42_17:mov	al, bl
nf42_18:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf42_21:mov	al, bl
nf42_22:mov	ah, bl
	shl	eax, 16
nf42_23:mov	al, bl
nf42_24:mov	ah, bl
	mov	[edi], eax

nf42_25:mov	al, bl
nf42_26:mov	ah, bl
	shl	eax, 16
nf42_27:mov	al, bl
nf42_28:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf42_31:mov	al, bl
nf42_32:mov	ah, bl
	shl	eax, 16
nf42_33:mov	al, bl
nf42_34:mov	ah, bl
	mov	[edi], eax

nf42_35:mov	al, bl
nf42_36:mov	ah, bl
	shl	eax, 16
nf42_37:mov	al, bl
nf42_38:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf42_41:mov	al, bl
nf42_42:mov	ah, bl
	shl	eax, 16
nf42_43:mov	al, bl
nf42_44:mov	ah, bl
	mov	[edi], eax

nf42_45:mov	al, bl
nf42_46:mov	ah, bl
	shl	eax, 16
nf42_47:mov	al, bl
nf42_48:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+12]
	mov	cx, [esi+14]

nf42_51:mov	al, bl
nf42_52:mov	ah, bl
	shl	eax, 16
nf42_53:mov	al, bl
nf42_54:mov	ah, bl
	mov	[edi], eax

nf42_55:mov	al, bl
nf42_56:mov	ah, bl
	shl	eax, 16
nf42_57:mov	al, bl
nf42_58:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf42_61:mov	al, bl
nf42_62:mov	ah, bl
	shl	eax, 16
nf42_63:mov	al, bl
nf42_64:mov	ah, bl
	mov	[edi], eax

nf42_65:mov	al, bl
nf42_66:mov	ah, bl
	shl	eax, 16
nf42_67:mov	al, bl
nf42_68:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf42_71:mov	al, bl
nf42_72:mov	ah, bl
	shl	eax, 16
nf42_73:mov	al, bl
nf42_74:mov	ah, bl
	mov	[edi], eax

nf42_75:mov	al, bl
nf42_76:mov	ah, bl
	shl	eax, 16
nf42_77:mov	al, bl
nf42_78:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf42_81:mov	al, bl
nf42_82:mov	ah, bl
	shl	eax, 16
nf42_83:mov	al, bl
nf42_84:mov	ah, bl
	mov	[edi], eax

nf42_85:mov	al, bl
nf42_86:mov	ah, bl
	shl	eax, 16
nf42_87:mov	al, bl
nf42_88:mov	ah, bl
	mov	[edi+4], eax

	add	esi, 24
	sub	edi, nfpk_back_right
	retn

;----------------------------------------
	ALIGN 4
nf11:	;     8x8x8 (64 bytes)
if 0 ;debug
	add	esi, 64
	mov	eax, 0fefefefeH
;	mov	ebx, eax
	mov	ebx, 0
	jmp	nf_solid
endif
	mov	edx, nf_width

	mov eax, [esi]		;0
	mov [edi], eax
	mov eax, [esi+4]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+8]	;1
	mov [edi], eax
	mov eax, [esi+12]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+16]	;2
	mov [edi], eax
	mov eax, [esi+20]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+24]	;3
	mov [edi], eax
	mov eax, [esi+28]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+32]	;4
	mov [edi], eax
	mov eax, [esi+36]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+40]	;5
	mov [edi], eax
	mov eax, [esi+44]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+48]	;6
	mov [edi], eax
	mov eax, [esi+52]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+56]	;7
	mov [edi], eax
	mov eax, [esi+60]
	mov [edi+4], eax

	add	esi, 64
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8
	retn

;----------------------------------------
	ALIGN 4
nf12:	; low 4x4x8 (16 bytes)
	mov	edx, nf_width

	mov	eax, [esi]
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi], ebx
	mov	[edi+edx], ebx
	shr	eax, 16
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi+4], ebx
	mov	[edi+edx+4], ebx
	lea	edi, [edi+edx*2]

	mov	eax, [esi+4]
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi], ebx
	mov	[edi+edx], ebx
	shr	eax, 16
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi+4], ebx
	mov	[edi+edx+4], ebx
	lea	edi, [edi+edx*2]

	mov	eax, [esi+8]
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi], ebx
	mov	[edi+edx], ebx
	shr	eax, 16
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi+4], ebx
	mov	[edi+edx+4], ebx
	lea	edi, [edi+edx*2]

	mov	eax, [esi+12]
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi], ebx
	mov	[edi+edx], ebx
	shr	eax, 16
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi+4], ebx
	mov	[edi+edx+4], ebx
	add	edi, edx

	sub	edi, nfpk_back_right
	add	esi, 16
	retn

;----------------------------------------
	ALIGN 4
nf13:	; 2x2 4x4x0 (4 bytes)
	mov	edx, nf_width

	mov	cl, [esi]
	mov	ch, cl
	mov	eax, ecx
	shl	eax, 16
	mov	ax, cx

	mov	cl, [esi+1]
	mov	ch, cl
	mov	ebx, ecx
	shl	ebx, 16
	mov	bx, cx

	mov	[edi], eax
	mov	[edi+4], ebx
	mov	[edi+edx], eax
	mov	[edi+edx+4], ebx
	lea	edi, [edi+edx*2]
	mov	[edi], eax
	mov	[edi+4], ebx
	mov	[edi+edx], eax
	mov	[edi+edx+4], ebx
	lea	edi, [edi+edx*2]

	mov	cl, [esi+2]
	mov	ch, cl
	mov	eax, ecx
	shl	eax, 16
	mov	ax, cx

	mov	cl, [esi+3]
	mov	ch, cl
	mov	ebx, ecx
	shl	ebx, 16
	mov	bx, cx

	mov	[edi], eax
	mov	[edi+4], ebx
	mov	[edi+edx], eax
	mov	[edi+edx+4], ebx
	lea	edi, [edi+edx*2]
	mov	[edi], eax
	mov	[edi+4], ebx
	add	edi, edx
	mov	[edi], eax
	mov	[edi+4], ebx

	sub	edi, nfpk_back_right
	add	esi, 4

	retn

;----------------------------------------
	ALIGN 4
nf14:	;     8x8x0 (1 byte)
if 0 ;debug
	jmp nf0
endif
	mov	bl, [esi]		; Copy color into 8 positions
	inc	esi
	mov	bh, bl
	mov	eax, ebx
	shl	eax, 16
	mov	ax, bx
	mov	ebx, eax
if 0 ;debug
	mov	eax, 080808080h
	mov	ebx, eax
endif
	jmp	nf_solid

	retn

;----------------------------------------
	ALIGN 4
nf15:	; mix 8x8x0 (2 bytes)
if 0 ;debug
	inc	esi
	jmp nf0
endif
	mov	bx, [esi]		; Copy 2 colors into 8 positions
	add	esi, 2			;  in a checkerboard
	mov	ax, bx
	shl	eax, 16
	mov	ax, bx
	mov	ebx, eax
	rol	ebx, 8
if 0 ;debug
	mov	eax, 080808080h
	mov	ebx, eax
endif
nf_solid:
	mov	edx, nf_width

	mov	[edi], eax
	mov	[edi+4], eax
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	add	edi, edx
	mov	[edi], eax
	mov	[edi+4], eax
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	add	edi, edx
	mov	[edi], eax
	mov	[edi+4], eax
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	add	edi, edx
	mov	[edi], eax
	mov	[edi+4], eax
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx

	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

nfPkDecomp ENDP

; Half vertical resolution version (skip odd lines)
;
nfPkDecompH PROC USES ESI EDI EBX, \
		ops:PTRBYTE, comp:PTRBYTE, \
		x:DWORD, y:DWORD, w:DWORD, h:DWORD
	LOCAL tbuf: PTRBYTE
	LOCAL new_row:DWORD
	LOCAL DiffBufPtrs:DWORD

	LOCAL nfpk_back_right: DWORD
	LOCAL wcnt:DWORD

	LOG_LABEL "StartPkDecomp"

.data
nfpk_OpTblH label dword
	dword offset nf0	; Prev Same (0)
	dword offset nf1	; No change (and copied to screen) (0)
	dword offset nf2	; Near shift from older part of current buf (1)
	dword offset nf3	; Near shift from newer part of current buf (1)
	dword offset nf4	; Near shift from previous buffer (1)
	dword offset nf5	; Far shift from previous buffer (2)
	dword offset nf6	; Far shift from current buffer (2)
				; [Or if COMPOPS, run of no changes (0)]
	dword offset nf7	;     8x8x1 (10 bytes) or low 4x4x1 (4 bytes)
	dword offset nf8	; 2x2 4x4x1 (16 bytes) or 2x1 4x8x1 (12 bytes) or 1x2 8x4x1 (12 bytes)
	dword offset nf9	;     8x8x2 (20 bytes) or low 4x4x2 (8 bytes) or
				; low 4x8x2 (12 bytes) or low 8x4x2 (12 bytes)
	dword offset nf10	; 2x2 4x4x2 (32 bytes) or 2x1 4x8x2 (24 bytes) or 1x2 4x8x2 (24 bytes)
	dword offset nf11	;     8x8x8 (64 bytes)
	dword offset nf12	; low 4x4x8 (16 bytes)
	dword offset nf13	; 2x2 4x4x0 (ie 2x2x8) (4 bytes)
	dword offset nf14	;     8x8x0 (1 byte)
	dword offset nf15	; mix 8x8x0 (2 bytes)
.code


	NF_DECOMP_INIT	0

	mov	eax, nf_width
	shl	eax, 2
	sub	eax, nf_new_w
	mov	new_row, eax

	shr	nf_new_h, 1

	mov	eax, nf_width
	lea	eax, [eax*2+eax-SWIDTH]
	mov	nfpk_back_right, eax

	mov	esi, comp
	mov	edi, tbuf
nf_StartRow:
	mov	eax, w
	shr	eax, 1
	mov	wcnt,eax
	ALIGN 4
nf_NextPair:
	dec	wcnt
	js	nf_NextRow
	mov	ebx, ops
	mov	al, [ebx]
	inc	ebx
	mov	ops, ebx

	xor	ebx, ebx
	mov	bl, al
	shr	bl, 4
	and	eax, 0Fh
	push	offset nf_NextPair
	push	nfpk_OpTblH[ebx*4]
	jmp	nfpk_OpTblH[eax*4]

nf_NextRow:
	add	edi, new_row
	dec	h
	jnz	nf_StartRow
	LOG_LABEL "EndPkDecomp"

	ret

;----------------------------------------
	ALIGN 4
nf0:	; No change from previous buffer
	mov	eax, DiffBufPtrs
	jmp	nf_shift

;----------------------------------------
	ALIGN 4
nf1:	; No change (and copied to screen)
	add	edi, SWIDTH
	retn

;----------------------------------------
	ALIGN 4
nf2:	; Near shift from older part of current buffer
	xor	eax, eax
	mov	al, [esi]
	inc	esi
	mov	ax, nfpk_ShiftP2[eax*2]
nf_xyc_shift:
	xor	ebx, ebx
	mov	bl, ah
	shl	eax, 24
	sar	eax, 24
	add	bl, 080h
	adc	bl, 080h
	sar	bl, 1
	add	eax, nfpk_ShiftY[ebx*4]
	jmp	nf_shift

;----------------------------------------
	ALIGN 4
nf3:	; Near shift from newer part of current buffer
	xor	eax, eax
	mov	al, [esi]
	inc	esi
	mov	ax, nfpk_ShiftP2[eax*2]
	neg	al
	neg	ah
	jmp	nf_xyc_shift

;----------------------------------------
	ALIGN 4
nf4:	; Near shift from previous buffer
	xor	eax, eax
	mov	al, [esi]
	inc	esi
	mov	ax, nfpk_ShiftP1[eax*2]
	jmp	nf_xyp_shift

;----------------------------------------
	ALIGN 4
nf5:	; Far shift from previous buffer
	mov	ax, [esi]
	add	esi, 2
nf_xyp_shift:
	xor	ebx, ebx
	mov	bl, ah
	shl	eax, 24
	sar	eax, 24
	add	bl, 080h
	adc	bl, 080h
	sar	bl, 1
	add	eax, nfpk_ShiftY[ebx*4]
	add	eax, DiffBufPtrs
	jmp	nf_shift

;----------------------------------------
	ALIGN 4

nf6:	; Run of no changes (must only appear in first nibble opcodes)
	; Next nibble k specifies 2k+4 squares with no changes
	add	esp, 4			; Next nibble is not an opcode
	add	ebx, 2			; (minimum of 4 squares)
	ALIGN 4
nf6a:	add	edi, SWIDTH*2		; Advance over two squares
	dec	ebx
	jz	nf6z			; Last pair of squares
	dec	wcnt			; Same row?
	jns	nf6a			;  Yes
	add	edi, new_row		; Advance to next row
	dec	h			; Decrement row count (should never become zero here)
	mov	eax, w			; Reset wcnt
	shr	eax ,1
	dec	eax
	mov	wcnt, eax
	jmp	nf6a

nf6z:	retn

;----------------------------------------
	ALIGN 4
nf_shift:
if 0 ;debug
	mov	eax, 0
	mov	ebx, eax
	jmp	nf_solid
endif
	mov	ebx, esi	; save esi
	lea	esi, [edi+eax]
	mov	edx, nf_width

	REPEAT 3
	  mov eax, [esi]
	  mov [edi], eax
	  mov eax, [esi+4]
	  mov [edi+4], eax
	  add	esi, edx
	  add	edi, edx
	ENDM
	mov eax, [esi]
	mov [edi], eax
	mov eax, [esi+4]
	mov [edi+4], eax

	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8
	mov	esi, ebx	; restore esi
	retn

;----------------------------------------
	ALIGN 4
nf7:	;     8x8x1 (10 bytes)

	mov	ax, [esi]
	cmp	al, ah
	ja	nf23

if 0 ;debug
	add	esi, 10
	mov	eax, 0fefefefeH
	mov	ebx, eax
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfpk_mov8
	lea	edx, byte ptr ds:nf7_11+2

	mov	al, [esi+2]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_11-nf7_11)], bl
	mov	[edx+(nf7_12-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_13-nf7_11)], bl
	mov	[edx+(nf7_14-nf7_11)], bh

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_31-nf7_11)], bl
	mov	[edx+(nf7_32-nf7_11)], bh
	shr	ebx, 16
	mov	[edx+(nf7_33-nf7_11)], bl
	mov	[edx+(nf7_34-nf7_11)], bh

	lea	edx, [edx+(nf7_51-nf7_11)]

	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_51-nf7_51)], bl
	mov	[edx+(nf7_52-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_53-nf7_51)], bl
	mov	[edx+(nf7_54-nf7_51)], bh

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf7_71-nf7_51)], bl
	mov	[edx+(nf7_72-nf7_51)], bh
	shr	ebx, 16
	mov	[edx+(nf7_73-nf7_51)], bl
	mov	[edx+(nf7_74-nf7_51)], bh

	push	ebp
	push	esi
	; load bx,dx,cx,bp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	mov	cx, [esi]
	mov	esi,nf_width
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax
	jmp	nf7_0		; flush prefetch
	ALIGN 4
nf7_0:
nf7_11:	mov	ax, bx
	shl	eax, 16
nf7_12:	mov	ax, bx
	mov	[edi], eax
nf7_13:	mov	ax, bx
	shl	eax, 16
nf7_14:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_31:	mov	ax, bx
	shl	eax, 16
nf7_32:	mov	ax, bx
	mov	[edi], eax
nf7_33:	mov	ax, bx
	shl	eax, 16
nf7_34:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_51:	mov	ax, bx
	shl	eax, 16
nf7_52:	mov	ax, bx
	mov	[edi], eax
nf7_53:	mov	ax, bx
	shl	eax, 16
nf7_54:	mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf7_71:	mov	ax, bx
	shl	eax, 16
nf7_72:	mov	ax, bx
	mov	[edi], eax
nf7_73:	mov	ax, bx
	shl	eax, 16
nf7_74:	mov	ax, bx
	mov	[edi+4], eax

	pop	esi
	pop	ebp
	add	esi, 10
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf7+16
nf23:	; low 4x4x1 (4 bytes)

	xor	eax, eax
	lea	ecx, nfpk_mov4l
	lea	edx, byte ptr ds:nf23_11+2

	mov	al, [esi+2]
	and	al, 0fH
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_11-nf23_11)], bl
	mov	[edx+(nf23_12-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_13-nf23_11)], bl
	mov	[edx+(nf23_14-nf23_11)], bh

	mov	al, [esi+2]
	shr	al, 4
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_31-nf23_11)], bl
	mov	[edx+(nf23_32-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_33-nf23_11)], bl
	mov	[edx+(nf23_34-nf23_11)], bh


	mov	al, [esi+3]
	and	al, 0fH
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_51-nf23_11)], bl
	mov	[edx+(nf23_52-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_53-nf23_11)], bl
	mov	[edx+(nf23_54-nf23_11)], bh

	mov	al, [esi+3]
	shr	al, 4
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf23_71-nf23_11)], bl
	mov	[edx+(nf23_72-nf23_11)], bh
	shr	ebx, 16
	mov	[edx+(nf23_73-nf23_11)], bl
	mov	[edx+(nf23_74-nf23_11)], bh

	mov	edx, nf_width

	; load bx,cx with 00,11 color combinations
	mov	bx, [esi]
	mov	cl, bh
	mov	bh, bl
	mov	ch, cl

	jmp	nf23_0		; flush prefetch
	ALIGN 4
nf23_0:

nf23_11:mov	ax, bx
	shl	eax, 16
nf23_12:mov	ax, bx
	mov	[edi], eax

nf23_13:mov	ax, bx
	shl	eax, 16
nf23_14:mov	ax, bx
	mov	[edi+4], eax
	add	edi, edx

nf23_31:mov	ax, bx
	shl	eax, 16
nf23_32:mov	ax, bx
	mov	[edi], eax

nf23_33:mov	ax, bx
	shl	eax, 16
nf23_34:mov	ax, bx
	mov	[edi+4], eax
	add	edi, edx

nf23_51:mov	ax, bx
	shl	eax, 16
nf23_52:mov	ax, bx
	mov	[edi], eax

nf23_53:mov	ax, bx
	shl	eax, 16
nf23_54:mov	ax, bx
	mov	[edi+4], eax
	add	edi, edx

nf23_71:mov	ax, bx
	shl	eax, 16
nf23_72:mov	ax, bx
	mov	[edi], eax

nf23_73:mov	ax, bx
	shl	eax, 16
nf23_74:mov	ax, bx
	mov	[edi+4], eax

	sub	edi, nfpk_back_right
	add	esi, 4
	retn

;----------------------------------------
	ALIGN 4
nf8:	; 2x2 4x4x1 (16 bytes)

	mov	ax, [esi]
	cmp	al, ah
	ja	nf24

	xor	eax, eax
	lea	ecx, nfpk_mov8
	lea	edx, byte ptr ds:nf8_11+2	

	mov	al, [esi+2]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_11-nf8_11)], bl
	mov	[edx+(nf8_12-nf8_11)], bh

	mov	al, [esi+3]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_21-nf8_11)], bl
	mov	[edx+(nf8_22-nf8_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_31-nf8_11)], bl
	mov	[edx+(nf8_32-nf8_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_41-nf8_11)], bl
	mov	[edx+(nf8_42-nf8_11)], bh

	add	edx, nf8_51-nf8_11

	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_51-nf8_51)], bl
	mov	[edx+(nf8_52-nf8_51)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_61-nf8_51)], bl
	mov	[edx+(nf8_62-nf8_51)], bh


	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_71-nf8_51)], bl
	mov	[edx+(nf8_72-nf8_51)], bh

	mov	al, [esi+15]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf8_81-nf8_51)], bl
	mov	[edx+(nf8_82-nf8_51)], bh


	push	ebp
	push	esi
	; load bx,dx,cx,bp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	mov	cx, [esi]
	mov	esi, nf_width
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

	jmp	nf8_0		; flush prefetch
	ALIGN 4
nf8_0:
nf8_11:	mov	ax, bx
	shl	eax, 16
nf8_12:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf8_21:	mov	ax, bx
	shl	eax, 16
nf8_22:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

	mov	eax, [esp]
	mov	cx, [eax+4]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf8_31:	mov	ax, bx
	shl	eax, 16
nf8_32:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf8_41:	mov	ax, bx
	shl	eax, 16
nf8_42:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

	lea	eax, [esi*4-4]
	sub	edi, eax

	mov	eax, [esp]
	mov	cx, [eax+8]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf8_51:	mov	ax, bx
	shl	eax, 16
nf8_52:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf8_61:	mov	ax, bx
	shl	eax, 16
nf8_62:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

	mov	eax, [esp]
	mov	cx, [eax+12]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf8_71:	mov	ax, bx
	shl	eax, 16
nf8_72:	mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf8_81:	mov	ax, bx
	shl	eax, 16
nf8_82:	mov	ax, bx
	mov	[edi], eax

	pop	esi
	pop	ebp
	add	esi, 16
	sub	edi, 4
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf8+16
nf24:	; 2x1 4x8x1 (12 bytes)

	mov	ax, [esi+6]
	cmp	al, ah
	ja	nf40

	xor	eax, eax
	lea	ecx, nfpk_mov8
	lea	edx, byte ptr ds:nf24_11+2	

	mov	al, [esi+2]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_11-nf24_11)], bl
	mov	[edx+(nf24_12-nf24_11)], bh

	mov	al, [esi+3]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_21-nf24_11)], bl
	mov	[edx+(nf24_22-nf24_11)], bh

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_31-nf24_11)], bl
	mov	[edx+(nf24_32-nf24_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_41-nf24_11)], bl
	mov	[edx+(nf24_42-nf24_11)], bh

	add	edx, nf24_51-nf24_11

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_51-nf24_51)], bl
	mov	[edx+(nf24_52-nf24_51)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_61-nf24_51)], bl
	mov	[edx+(nf24_62-nf24_51)], bh

	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_71-nf24_51)], bl
	mov	[edx+(nf24_72-nf24_51)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf24_81-nf24_51)], bl
	mov	[edx+(nf24_82-nf24_51)], bh

	push	ebp
	push	esi
	; load bx,dx,cx,bp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	mov	cx, [esi]
	mov	esi, nf_width
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

	jmp	nf24_0		; flush prefetch
	ALIGN 4
nf24_0:
nf24_11:mov	ax, bx
	shl	eax, 16
nf24_12:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_21:mov	ax, bx
	shl	eax, 16
nf24_22:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_31:mov	ax, bx
	shl	eax, 16
nf24_32:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_41:mov	ax, bx
	shl	eax, 16
nf24_42:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

	lea	eax, [esi*4-4]
	sub	edi, eax

	mov	eax, [esp]
	mov	cx, [eax+6]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf24_51:mov	ax, bx
	shl	eax, 16
nf24_52:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_61:mov	ax, bx
	shl	eax, 16
nf24_62:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_71:mov	ax, bx
	shl	eax, 16
nf24_72:mov	ax, bx
	mov	[edi], eax
	add	edi, esi

nf24_81:mov	ax, bx
	shl	eax, 16
nf24_82:mov	ax, bx
	mov	[edi], eax

	pop	esi
	pop	ebp
	add	esi, 12
	sub	edi, 4
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf8+32
nf40:	; 1x2 8x4x1 (12 bytes)
	xor	eax, eax
	lea	ecx, nfpk_mov8
	lea	edx, byte ptr ds:nf40_11+2	

	mov	al, [esi+2]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_11-nf40_11)], bl
	mov	[edx+(nf40_12-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_13-nf40_11)], bl
	mov	[edx+(nf40_14-nf40_11)], bh

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_31-nf40_11)], bl
	mov	[edx+(nf40_32-nf40_11)], bh
	shr	ebx, 16
	mov	[edx+(nf40_33-nf40_11)], bl
	mov	[edx+(nf40_34-nf40_11)], bh

	add	edx, nf40_51-nf40_11

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_51-nf40_51)], bl
	mov	[edx+(nf40_52-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_53-nf40_51)], bl
	mov	[edx+(nf40_54-nf40_51)], bh

	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf40_71-nf40_51)], bl
	mov	[edx+(nf40_72-nf40_51)], bh
	shr	ebx, 16
	mov	[edx+(nf40_73-nf40_51)], bl
	mov	[edx+(nf40_74-nf40_51)], bh

	push	ebp
	push	esi
	; load bx,dx,cx,bp with 00,01,10,11 color combinations
	; (note that bits are read least significant first).
	mov	cx, [esi]
	mov	esi, nf_width
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

	jmp	nf40_0		; flush prefetch
	ALIGN 4
nf40_0:
nf40_11:mov	ax, bx
	shl	eax, 16
nf40_12:mov	ax, bx
	mov	[edi], eax
nf40_13:mov	ax, bx
	shl	eax, 16
nf40_14:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf40_31:mov	ax, bx
	shl	eax, 16
nf40_32:mov	ax, bx
	mov	[edi], eax
nf40_33:mov	ax, bx
	shl	eax, 16
nf40_34:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

	mov	eax, [esp]
	mov	cx, [eax+6]
	mov	bl,cl
	mov	bh,cl
	mov	dl,ch
	mov	dh,cl
	mov	al,ch
	mov	ah,ch
	mov	ebp,eax

nf40_51:mov	ax, bx
	shl	eax, 16
nf40_52:mov	ax, bx
	mov	[edi], eax
nf40_53:mov	ax, bx
	shl	eax, 16
nf40_54:mov	ax, bx
	mov	[edi+4], eax
	add	edi, esi

nf40_71:mov	ax, bx
	shl	eax, 16
nf40_72:mov	ax, bx
	mov	[edi], eax
nf40_73:mov	ax, bx
	shl	eax, 16
nf40_74:mov	ax, bx
	mov	[edi+4], eax

	pop	esi
	pop	ebp
	add	esi, 12
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
nf9:	;     8x8x2 (20 bytes)

	mov	eax, [esi]
	cmp	al, ah
	ja	nf41

	shr	eax, 16
	cmp	al, ah
	ja	nf25

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf9_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_11-nf9_11)], bl
	mov	[edx+(nf9_12-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_13-nf9_11)], bl
	mov	[edx+(nf9_14-nf9_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_15-nf9_11)], bl
	mov	[edx+(nf9_16-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_17-nf9_11)], bl
	mov	[edx+(nf9_18-nf9_11)], bh


	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_31-nf9_11)], bl
	mov	[edx+(nf9_32-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_33-nf9_11)], bl
	mov	[edx+(nf9_34-nf9_11)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_35-nf9_11)], bl
	mov	[edx+(nf9_36-nf9_11)], bh
	shr	ebx, 16
	mov	[edx+(nf9_37-nf9_11)], bl
	mov	[edx+(nf9_38-nf9_11)], bh

	lea	edx, [edx+(nf9_51-nf9_11)]

	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_51-nf9_51)], bl
	mov	[edx+(nf9_52-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_53-nf9_51)], bl
	mov	[edx+(nf9_54-nf9_51)], bh

	mov	al, [esi+13]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_55-nf9_51)], bl
	mov	[edx+(nf9_56-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_57-nf9_51)], bl
	mov	[edx+(nf9_58-nf9_51)], bh


	mov	al, [esi+16]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_71-nf9_51)], bl
	mov	[edx+(nf9_72-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_73-nf9_51)], bl
	mov	[edx+(nf9_74-nf9_51)], bh

	mov	al, [esi+17]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf9_75-nf9_51)], bl
	mov	[edx+(nf9_76-nf9_51)], bh
	shr	ebx, 16
	mov	[edx+(nf9_77-nf9_51)], bl
	mov	[edx+(nf9_78-nf9_51)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf9_0		; flush prefetch
	ALIGN 4
nf9_0:
nf9_11:	mov	al, bl
nf9_12:	mov	ah, bl
	shl	eax, 16
nf9_13:	mov	al, bl
nf9_14:	mov	ah, bl
	mov	[edi], eax

nf9_15:	mov	al, bl
nf9_16:	mov	ah, bl
	shl	eax, 16
nf9_17:	mov	al, bl
nf9_18:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_31:	mov	al, bl
nf9_32:	mov	ah, bl
	shl	eax, 16
nf9_33:	mov	al, bl
nf9_34:	mov	ah, bl
	mov	[edi], eax

nf9_35:	mov	al, bl
nf9_36:	mov	ah, bl
	shl	eax, 16
nf9_37:	mov	al, bl
nf9_38:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_51:	mov	al, bl
nf9_52:	mov	ah, bl
	shl	eax, 16
nf9_53:	mov	al, bl
nf9_54:	mov	ah, bl
	mov	[edi], eax

nf9_55:	mov	al, bl
nf9_56:	mov	ah, bl
	shl	eax, 16
nf9_57:	mov	al, bl
nf9_58:	mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf9_71:	mov	al, bl
nf9_72:	mov	ah, bl
	shl	eax, 16
nf9_73:	mov	al, bl
nf9_74:	mov	ah, bl
	mov	[edi], eax

nf9_75:	mov	al, bl
nf9_76:	mov	ah, bl
	shl	eax, 16
nf9_77:	mov	al, bl
nf9_78:	mov	ah, bl
	mov	[edi+4], eax

	add	esi, 20
	sub	edi, nfpk_back_right
	retn

;----------------------------------------
	ALIGN 4
;nf9+16
nf25:	; low 4x4x2 (8 bytes)

if 0 ;debug
	mov	eax, 0
	mov	ebx, 0
	add	esi, 8
	jmp	nf_solid
endif

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf25_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_14-nf25_11)], bl
	mov	[edx+(nf25_13-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_12-nf25_11)], bl
	mov	[edx+(nf25_11-nf25_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_24-nf25_11)], bl
	mov	[edx+(nf25_23-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_22-nf25_11)], bl
	mov	[edx+(nf25_21-nf25_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_34-nf25_11)], bl
	mov	[edx+(nf25_33-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_32-nf25_11)], bl
	mov	[edx+(nf25_31-nf25_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf25_44-nf25_11)], bl
	mov	[edx+(nf25_43-nf25_11)], bh
	shr	ebx, 16
	mov	[edx+(nf25_42-nf25_11)], bl
	mov	[edx+(nf25_41-nf25_11)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf25_0		; flush prefetch
	ALIGN 4
nf25_0:
nf25_11:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_12:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf25_13:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_14:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf25_21:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_22:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf25_23:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_24:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf25_31:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_32:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf25_33:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_34:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf25_41:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_42:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf25_43:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf25_44:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax

	add	esi, 8
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf9+32
nf41:	; low 4x8x2 (12 bytes)
	shr	eax, 16
	cmp	al, ah
	ja	nf57

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf41_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_14-nf41_11)], bl
	mov	[edx+(nf41_13-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_12-nf41_11)], bl
	mov	[edx+(nf41_11-nf41_11)], bh

	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_34-nf41_11)], bl
	mov	[edx+(nf41_33-nf41_11)], bh
	shr	ebx, 16
	mov	[edx+(nf41_32-nf41_11)], bl
	mov	[edx+(nf41_31-nf41_11)], bh

	lea	edx, [edx+(nf41_51-nf41_11)]

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_54-nf41_51)], bl
	mov	[edx+(nf41_53-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_52-nf41_51)], bl
	mov	[edx+(nf41_51-nf41_51)], bh

	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf41_74-nf41_51)], bl
	mov	[edx+(nf41_73-nf41_51)], bh
	shr	ebx, 16
	mov	[edx+(nf41_72-nf41_51)], bl
	mov	[edx+(nf41_71-nf41_51)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf41_0		; flush prefetch
	ALIGN 4
nf41_0:
nf41_11:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_12:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_13:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_14:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_31:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_32:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_33:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_34:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_51:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_52:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_53:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_54:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax
	add	edi, edx

nf41_71:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_72:mov	al, bl
	mov	ah, al
	mov	[edi], eax
nf41_73:mov	ah, bl
	mov	al, ah
	shl	eax, 16
nf41_74:mov	al, bl
	mov	ah, al
	mov	[edi+4], eax

	add	esi, 12
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf9+48
nf57:	; low 8x4x2 (12 bytes)
	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf57_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_11-nf57_11)], bl
	mov	[edx+(nf57_12-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_13-nf57_11)], bl
	mov	[edx+(nf57_14-nf57_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_15-nf57_11)], bl
	mov	[edx+(nf57_16-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_17-nf57_11)], bl
	mov	[edx+(nf57_18-nf57_11)], bh


	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_21-nf57_11)], bl
	mov	[edx+(nf57_22-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_23-nf57_11)], bl
	mov	[edx+(nf57_24-nf57_11)], bh

	mov	al, [esi+7]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_25-nf57_11)], bl
	mov	[edx+(nf57_26-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_27-nf57_11)], bl
	mov	[edx+(nf57_28-nf57_11)], bh


	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_31-nf57_11)], bl
	mov	[edx+(nf57_32-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_33-nf57_11)], bl
	mov	[edx+(nf57_34-nf57_11)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_35-nf57_11)], bl
	mov	[edx+(nf57_36-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_37-nf57_11)], bl
	mov	[edx+(nf57_38-nf57_11)], bh


	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_41-nf57_11)], bl
	mov	[edx+(nf57_42-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_43-nf57_11)], bl
	mov	[edx+(nf57_44-nf57_11)], bh

	mov	al, [esi+11]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf57_45-nf57_11)], bl
	mov	[edx+(nf57_46-nf57_11)], bh
	shr	ebx, 16
	mov	[edx+(nf57_47-nf57_11)], bl
	mov	[edx+(nf57_48-nf57_11)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf57_0		; flush prefetch
	ALIGN 4
nf57_0:
nf57_11:mov	al, bl
nf57_12:mov	ah, bl
	shl	eax, 16
nf57_13:mov	al, bl
nf57_14:mov	ah, bl
	mov	[edi], eax

nf57_15:mov	al, bl
nf57_16:mov	ah, bl
	shl	eax, 16
nf57_17:mov	al, bl
nf57_18:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf57_21:mov	al, bl
nf57_22:mov	ah, bl
	shl	eax, 16
nf57_23:mov	al, bl
nf57_24:mov	ah, bl
	mov	[edi], eax

nf57_25:mov	al, bl
nf57_26:mov	ah, bl
	shl	eax, 16
nf57_27:mov	al, bl
nf57_28:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf57_31:mov	al, bl
nf57_32:mov	ah, bl
	shl	eax, 16
nf57_33:mov	al, bl
nf57_34:mov	ah, bl
	mov	[edi], eax

nf57_35:mov	al, bl
nf57_36:mov	ah, bl
	shl	eax, 16
nf57_37:mov	al, bl
nf57_38:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf57_41:mov	al, bl
nf57_42:mov	ah, bl
	shl	eax, 16
nf57_43:mov	al, bl
nf57_44:mov	ah, bl
	mov	[edi], eax

nf57_45:mov	al, bl
nf57_46:mov	ah, bl
	shl	eax, 16
nf57_47:mov	al, bl
nf57_48:mov	ah, bl
	mov	[edi+4], eax

	add	esi, 12
	sub	edi, nfpk_back_right
	retn

;----------------------------------------
	ALIGN 4
nf10:	; 2x2 4x4x2 (32 bytes)

	mov	ax, [esi]
	cmp	al, ah
	ja	nf26

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf10_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_11-nf10_11)], bl
	mov	[edx+(nf10_12-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_13-nf10_11)], bl
	mov	[edx+(nf10_14-nf10_11)], bh

	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_21-nf10_11)], bl
	mov	[edx+(nf10_22-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_23-nf10_11)], bl
	mov	[edx+(nf10_24-nf10_11)], bh

	mov	al, [esi+12]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_31-nf10_11)], bl
	mov	[edx+(nf10_32-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_33-nf10_11)], bl
	mov	[edx+(nf10_34-nf10_11)], bh

	mov	al, [esi+14]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_41-nf10_11)], bl
	mov	[edx+(nf10_42-nf10_11)], bh
	shr	ebx, 16
	mov	[edx+(nf10_43-nf10_11)], bl
	mov	[edx+(nf10_44-nf10_11)], bh

	lea	edx, [edx+(nf10_51-nf10_11)]

	mov	al, [esi+20]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_51-nf10_51)], bl
	mov	[edx+(nf10_52-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_53-nf10_51)], bl
	mov	[edx+(nf10_54-nf10_51)], bh

	mov	al, [esi+22]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_61-nf10_51)], bl
	mov	[edx+(nf10_62-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_63-nf10_51)], bl
	mov	[edx+(nf10_64-nf10_51)], bh

	mov	al, [esi+28]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_71-nf10_51)], bl
	mov	[edx+(nf10_72-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_73-nf10_51)], bl
	mov	[edx+(nf10_74-nf10_51)], bh

	mov	al, [esi+30]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf10_81-nf10_51)], bl
	mov	[edx+(nf10_82-nf10_51)], bh
	shr	ebx, 16
	mov	[edx+(nf10_83-nf10_51)], bl
	mov	[edx+(nf10_84-nf10_51)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf10_0		; flush prefetch
	ALIGN 4
nf10_0:
nf10_11:mov	al, bl
nf10_12:mov	ah, bl
	shl	eax, 16
nf10_13:mov	al, bl
nf10_14:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_21:mov	al, bl
nf10_22:mov	ah, bl
	shl	eax, 16
nf10_23:mov	al, bl
nf10_24:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+8]
	mov	cx, [esi+10]

nf10_31:mov	al, bl
nf10_32:mov	ah, bl
	shl	eax, 16
nf10_33:mov	al, bl
nf10_34:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_41:mov	al, bl
nf10_42:mov	ah, bl
	shl	eax, 16
nf10_43:mov	al, bl
nf10_44:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

	lea	eax, [edx*4-4]
	sub	edi, eax

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+16]
	mov	cx, [esi+18]

nf10_51:mov	al, bl
nf10_52:mov	ah, bl
	shl	eax, 16
nf10_53:mov	al, bl
nf10_54:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_61:mov	al, bl
nf10_62:mov	ah, bl
	shl	eax, 16
nf10_63:mov	al, bl
nf10_64:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+24]
	mov	cx, [esi+26]

nf10_71:mov	al, bl
nf10_72:mov	ah, bl
	shl	eax, 16
nf10_73:mov	al, bl
nf10_74:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf10_81:mov	al, bl
nf10_82:mov	ah, bl
	shl	eax, 16
nf10_83:mov	al, bl
nf10_84:mov	ah, bl
	mov	[edi], eax

	add	esi, 32
	sub	edi, 4
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf10+16
nf26:	; 2x1 4x8x2 (24 bytes)

	mov	ax, [esi+12]
	cmp	al, ah
	ja	nf42

if 0 ;debug
	mov	eax, 0
	mov	ebx, 0
	add	esi, 24
	jmp	nf_solid
endif

	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf26_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_11-nf26_11)], bl
	mov	[edx+(nf26_12-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_13-nf26_11)], bl
	mov	[edx+(nf26_14-nf26_11)], bh

	mov	al, [esi+6]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_21-nf26_11)], bl
	mov	[edx+(nf26_22-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_23-nf26_11)], bl
	mov	[edx+(nf26_24-nf26_11)], bh

	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_31-nf26_11)], bl
	mov	[edx+(nf26_32-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_33-nf26_11)], bl
	mov	[edx+(nf26_34-nf26_11)], bh

	mov	al, [esi+10]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_41-nf26_11)], bl
	mov	[edx+(nf26_42-nf26_11)], bh
	shr	ebx, 16
	mov	[edx+(nf26_43-nf26_11)], bl
	mov	[edx+(nf26_44-nf26_11)], bh

	lea	edx, [edx+(nf26_51-nf26_11)]

	mov	al, [esi+16]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_51-nf26_51)], bl
	mov	[edx+(nf26_52-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_53-nf26_51)], bl
	mov	[edx+(nf26_54-nf26_51)], bh

	mov	al, [esi+18]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_61-nf26_51)], bl
	mov	[edx+(nf26_62-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_63-nf26_51)], bl
	mov	[edx+(nf26_64-nf26_51)], bh

	mov	al, [esi+20]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_71-nf26_51)], bl
	mov	[edx+(nf26_72-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_73-nf26_51)], bl
	mov	[edx+(nf26_74-nf26_51)], bh

	mov	al, [esi+22]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf26_81-nf26_51)], bl
	mov	[edx+(nf26_82-nf26_51)], bh
	shr	ebx, 16
	mov	[edx+(nf26_83-nf26_51)], bl
	mov	[edx+(nf26_84-nf26_51)], bh

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf26_0		; flush prefetch
	ALIGN 4
nf26_0:
nf26_11:mov	al, bl
nf26_12:mov	ah, bl
	shl	eax, 16
nf26_13:mov	al, bl
nf26_14:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_21:mov	al, bl
nf26_22:mov	ah, bl
	shl	eax, 16
nf26_23:mov	al, bl
nf26_24:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_31:mov	al, bl
nf26_32:mov	ah, bl
	shl	eax, 16
nf26_33:mov	al, bl
nf26_34:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_41:mov	al, bl
nf26_42:mov	ah, bl
	shl	eax, 16
nf26_43:mov	al, bl
nf26_44:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

	lea	eax, [edx*4-4]
	sub	edi, eax

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+12]
	mov	cx, [esi+14]

nf26_51:mov	al, bl
nf26_52:mov	ah, bl
	shl	eax, 16
nf26_53:mov	al, bl
nf26_54:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_61:mov	al, bl
nf26_62:mov	ah, bl
	shl	eax, 16
nf26_63:mov	al, bl
nf26_64:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_71:mov	al, bl
nf26_72:mov	ah, bl
	shl	eax, 16
nf26_73:mov	al, bl
nf26_74:mov	ah, bl
	mov	[edi], eax
	add	edi, edx

nf26_81:mov	al, bl
nf26_82:mov	ah, bl
	shl	eax, 16
nf26_83:mov	al, bl
nf26_84:mov	ah, bl
	mov	[edi], eax

	add	esi, 24
	sub	edi, 4
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf10+32
nf42:	; 1x2 8x4x2 (24 bytes)

if 0 ;debug
	mov	eax, 0
	mov	ebx, 0
	add	esi, 24
	jmp	nf_solid
endif
	xor	eax, eax
	lea	ecx, nfpk_mov4
	lea	edx, byte ptr ds:nf42_11+1

	mov	al, [esi+4]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_11-nf42_11)], bl
	mov	[edx+(nf42_12-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_13-nf42_11)], bl
	mov	[edx+(nf42_14-nf42_11)], bh

	mov	al, [esi+5]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_15-nf42_11)], bl
	mov	[edx+(nf42_16-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_17-nf42_11)], bl
	mov	[edx+(nf42_18-nf42_11)], bh


	mov	al, [esi+8]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_31-nf42_11)], bl
	mov	[edx+(nf42_32-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_33-nf42_11)], bl
	mov	[edx+(nf42_34-nf42_11)], bh

	mov	al, [esi+9]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_35-nf42_11)], bl
	mov	[edx+(nf42_36-nf42_11)], bh
	shr	ebx, 16
	mov	[edx+(nf42_37-nf42_11)], bl
	mov	[edx+(nf42_38-nf42_11)], bh

	lea	edx, [edx+(nf42_51-nf42_11)]

	mov	al, [esi+16]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_51-nf42_51)], bl
	mov	[edx+(nf42_52-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_53-nf42_51)], bl
	mov	[edx+(nf42_54-nf42_51)], bh

	mov	al, [esi+17]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_55-nf42_51)], bl
	mov	[edx+(nf42_56-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_57-nf42_51)], bl
	mov	[edx+(nf42_58-nf42_51)], bh


	mov	al, [esi+20]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_71-nf42_51)], bl
	mov	[edx+(nf42_72-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_73-nf42_51)], bl
	mov	[edx+(nf42_74-nf42_51)], bh

	mov	al, [esi+21]
	mov	ebx, [ecx+eax*4]
	mov	[edx+(nf42_75-nf42_51)], bl
	mov	[edx+(nf42_76-nf42_51)], bh
	shr	ebx, 16
	mov	[edx+(nf42_77-nf42_51)], bl
	mov	[edx+(nf42_78-nf42_51)], bh


	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi]
	mov	cx, [esi+2]

	mov	edx, nf_width
	jmp	nf42_0		; flush prefetch
	ALIGN 4
nf42_0:
nf42_11:mov	al, bl
nf42_12:mov	ah, bl
	shl	eax, 16
nf42_13:mov	al, bl
nf42_14:mov	ah, bl
	mov	[edi], eax

nf42_15:mov	al, bl
nf42_16:mov	ah, bl
	shl	eax, 16
nf42_17:mov	al, bl
nf42_18:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf42_31:mov	al, bl
nf42_32:mov	ah, bl
	shl	eax, 16
nf42_33:mov	al, bl
nf42_34:mov	ah, bl
	mov	[edi], eax

nf42_35:mov	al, bl
nf42_36:mov	ah, bl
	shl	eax, 16
nf42_37:mov	al, bl
nf42_38:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

	; Load bl,bh,cl,ch with four colors
	mov	bx, [esi+12]
	mov	cx, [esi+14]

nf42_51:mov	al, bl
nf42_52:mov	ah, bl
	shl	eax, 16
nf42_53:mov	al, bl
nf42_54:mov	ah, bl
	mov	[edi], eax

nf42_55:mov	al, bl
nf42_56:mov	ah, bl
	shl	eax, 16
nf42_57:mov	al, bl
nf42_58:mov	ah, bl
	mov	[edi+4], eax
	add	edi, edx

nf42_71:mov	al, bl
nf42_72:mov	ah, bl
	shl	eax, 16
nf42_73:mov	al, bl
nf42_74:mov	ah, bl
	mov	[edi], eax

nf42_75:mov	al, bl
nf42_76:mov	ah, bl
	shl	eax, 16
nf42_77:mov	al, bl
nf42_78:mov	ah, bl
	mov	[edi+4], eax

	add	esi, 24
	sub	edi, nfpk_back_right
	retn

;----------------------------------------
	ALIGN 4
nf11:	;     8x8x8 (64 bytes)
if 0 ;debug
	add	esi, 64
	mov	eax, 0fefefefeH
;	mov	ebx, eax
	mov	ebx, 0
	jmp	nf_solid
endif
	mov	edx, nf_width

	mov eax, [esi]		;0
	mov [edi], eax
	mov eax, [esi+4]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+16]	;2
	mov [edi], eax
	mov eax, [esi+20]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+32]	;4
	mov [edi], eax
	mov eax, [esi+36]
	mov [edi+4], eax
	add	edi, edx
	mov eax, [esi+48]	;6
	mov [edi], eax
	mov eax, [esi+52]
	mov [edi+4], eax

	add	esi, 64
	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8
	retn

;----------------------------------------
	ALIGN 4
nf12:	; low 4x4x8 (16 bytes)
	mov	edx, nf_width

	mov	eax, [esi]
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi], ebx
	shr	eax, 16
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi+4], ebx
	add	edi, edx

	mov	eax, [esi+4]
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi], ebx
	shr	eax, 16
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi+4], ebx
	add	edi, edx

	mov	eax, [esi+8]
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi], ebx
	shr	eax, 16
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi+4], ebx
	add	edi, edx

	mov	eax, [esi+12]
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi], ebx
	shr	eax, 16
	mov	bl, ah
	mov	bh, ah
	shl	ebx, 16
	mov	bl, al
	mov	bh, al
	mov	[edi+4], ebx

	sub	edi, nfpk_back_right
	add	esi, 16
	retn

;----------------------------------------
	ALIGN 4
nf13:	; 2x2 4x4x0 (4 bytes)
	mov	edx, nf_width

	mov	cl, [esi]
	mov	ch, cl
	mov	eax, ecx
	shl	eax, 16
	mov	ax, cx

	mov	cl, [esi+1]
	mov	ch, cl
	mov	ebx, ecx
	shl	ebx, 16
	mov	bx, cx

	mov	[edi], eax
	mov	[edi+4], ebx
	mov	[edi+edx], eax
	mov	[edi+edx+4], ebx
	lea	edi, [edi+edx*2]

	mov	cl, [esi+2]
	mov	ch, cl
	mov	eax, ecx
	shl	eax, 16
	mov	ax, cx

	mov	cl, [esi+3]
	mov	ch, cl
	mov	ebx, ecx
	shl	ebx, 16
	mov	bx, cx

	mov	[edi], eax
	mov	[edi+4], ebx
	add	edi, edx
	mov	[edi], eax
	mov	[edi+4], ebx

	sub	edi, nfpk_back_right
	add	esi, 4

	retn

;----------------------------------------
	ALIGN 4
nf14:	;     8x8x0 (1 byte)
	mov	bl, [esi]		; Copy color into 8 positions
	inc	esi
	mov	bh, bl
	mov	eax, ebx
	shl	eax, 16
	mov	ax, bx
	mov	ebx, eax
if 0 ;debug
	mov	eax, 080808080h
	mov	ebx, eax
endif
	jmp	nf_solid

	retn

;----------------------------------------
	ALIGN 4
nf15:	; mix 8x8x0 (2 bytes)
	mov	bx, [esi]		; Copy 2 colors into 8 positions
	add	esi, 2			;  in a checkerboard
	mov	ax, bx
	shl	eax, 16
	mov	ax, bx
	mov	ebx, eax
	rol	ebx, 8
if 0 ;debug
	mov	eax, 080808080h
	mov	ebx, eax
endif
nf_solid:
	mov	edx, nf_width

	mov	[edi], eax
	mov	[edi+4], eax
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx
	add	edi, edx
	mov	[edi], eax
	mov	[edi+4], eax
	add	edi, edx
	mov	[edi], ebx
	mov	[edi+4], ebx

	sub	edi, nfpk_back_right	; (SHEIGHT-1)*width+8

	retn

nfPkDecompH ENDP


; If at least 11 palette entries aren't changed, this is more compact
;  than uncompressed 256 entry palette.
;
;static void palLoadCompPalette(unsigned char *buf)
;
palLoadCompPalette PROC USES ESI EDI, \
			buf: PTRBYTE
	mov	ax, ds			; Insure es==ds for symantec flat mode
	mov	es, ax

	mov	cx, 32
	mov	esi, buf
	mov	edi, offset pal_tbl
next:	lodsb
	or	al, al
	jnz	chk0
	add	edi, 24
	loop	next
	jmp	done

chk0:	test	al, 1
	jz	not0
	movsw
	movsb
	test	al, 2
	jz	not1
cpy1:	movsw
	movsb
	test	al, 4
	jz	not2
cpy2:	movsw
	movsb
	test	al, 8
	jz	not3
cpy3:	movsw
	movsb
	test	al, 16
	jz	not4
cpy4:	movsw
	movsb
	test	al, 32
	jz	not5
cpy5:	movsw
	movsb
	test	al, 64
	jz	not6
cpy6:	movsw
	movsb
	or	al, al
	jns	not7
cpy7:	movsw
	movsb
	loop	next
	jmp	done

not0:	add	edi, 3
	test	al, 2
	jnz	cpy1
not1:	add	edi, 3
	test	al, 4
	jnz	cpy2
not2:	add	edi, 3
	test	al, 8
	jnz	cpy3
not3:	add	edi, 3
	test	al, 16
	jnz	cpy4
not4:	add	edi, 3
	test	al, 32
	jnz	cpy5
not5:	add	edi, 3
	test	al, 64
	jnz	cpy6
not6:	add	edi, 3
	or	al, al
	js	cpy7
not7:	add	edi, 3
	loop	next

done:	ret

palLoadCompPalette ENDP

EXTERN snd_8to16: WORD		; short snd_8to16[256];

;unsigned sndDecompM16(unsigned short *dst, unsigned char *src,
;			unsigned len, unsigned prev);
;
;Decompresses a mono stream containing len samples
;(src is len bytes, dst is len*2 bytes)
;prev is the previous decompression state or zero.
;Returns new decompression state.
;
sndDecompM16 PROC USES ESI EDI EBX, \
		dst:PTRWORD, src:PTRBYTE, len:DWORD, prev:DWORD
	mov	eax, prev

	mov	ecx, len
	jecxz	done

	mov	esi, src
	mov	edi, dst

	xor	ebx, ebx

lp:	mov	bl, byte ptr [esi]
	add	esi, 1
	add	ax, word ptr snd_8to16[ebx*2]
	mov	word ptr [edi], ax
	add	edi, 2
	dec	ecx
	jnz	lp

done:	ret
sndDecompM16 ENDP
