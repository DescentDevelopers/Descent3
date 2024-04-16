;  mvelibwa.c
;
; Interplay Movie (MVE) File Player Library (32-Bit Win95 Version)
; Assembly Language Components
; Written by Paul Allen Edelstein
;
; (c) 1997 Interplay Productions.  All Rights Reserved.
; This file is confidential and consists of proprietary information
; of Interplay Productions.  This file and associated libraries
; may not, in whole or in part, be disclosed to third parties,
; incorporated into any software product which is not being created
; for Interplay Productions, copied or duplicated in any form,
; without the prior written permission of Interplay Productions.
; Further, you may not reverse engineer, decompile or otherwise
; attempt to derive source code of this material.
;

;;--- Options ---

ONLYNEW equ 0			; For debug, disables motion comp
LOGGING equ 0			; Log timing statistics
PARTIAL equ 1			; Support for partial updates
PKDATA equ 1			; Support for packed data
HICOLOR equ 1			; Support for HiColor
INTERP equ 0			; Interpolated squares
				; 0:none (4x4x8), 1:generic dither,
				; 2:direction dither, 3:blend
COMPOPS equ 1			; Compressed opcode table
SCALING equ 1			; Scaling support
DECOMPD equ 0			; Support for dithered half vert res
TRANS16 equ 1			; Support for translating 16-bit rgb format

;;--- Constants ---

; Width and height of sections in pixels.
SWIDTH equ 8
SHEIGHT equ 8

LOG2_SWIDTH equ 3
LOG2_SHEIGHT equ 3

;;---

EXTERN pal_tbl   ;:BYTE		; unsigned char pal_tbl[3*256];
EXTERN pal15_tbl  ;:WORD	; unsigned short pal15_tbl[256];
EXTERN nf_trans16_lo
EXTERN nf_trans16_hi
EXTERN snd_8to16 ;: WORD		; short snd_8to16[256];

EXTERN nf_buf_cur ;: PTRBYTE	; unsigned char* nf_buf_cur;
EXTERN nf_buf_prv ;: PTRBYTE	; unsigned char* nf_buf_prv;

;; NextFrame parameters
EXTERN nf_wqty ;: BYTE		;unsigned char nf_wqty;	 // (width/SWIDTH)
EXTERN nf_hqty ;: BYTE		;unsigned char nf_hqty;	 // (height/SHEIGHT)
EXTERN nf_fqty ;: BYTE		;unsigned char nf_fqty;	 // Number of fields
EXTERN nf_hicolor ;: DWORD	;unsigned nf_hicolor;	 // HiColor (0:none,1:normal,2:swapped)

;;  <derived quantities>
EXTERN nf_width ;: DWORD		;unsigned nf_width;	 // wqty * SWIDTH
EXTERN nf_height ;: DWORD		;unsigned nf_height;	 // hqty * SHEIGHT;
EXTERN nf_new_line ;: DWORD	;unsigned nf_new_line;	 // width - SWIDTH
EXTERN nf_new_row0 ;: DWORD	;unsigned nf_new_row0;	 // SHEIGHT*width*2-width
EXTERN nf_back_right ;: DWORD	;unsigned nf_back_right; // (SHEIGHT-1)*width

;; Frame parameters
;;  Portion of current frame which has been updated
;;  and needs to be sent to screen.
;;
EXTERN nf_new_x ;: DWORD		;unsigned nf_new_x;
EXTERN nf_new_y ;: DWORD		;unsigned nf_new_y;
EXTERN nf_new_w ;: DWORD		;unsigned nf_new_w;
EXTERN nf_new_h ;: DWORD		;unsigned nf_new_h;

; These are all of our global parameter-passing variables - AH
extern sndDecompM16_dst
extern sndDecompM16_src
extern sndDecompM16_len
extern sndDecompM16_prev
extern sndDecompM16_return
extern sndDecompS16_dst
extern sndDecompS16_src
extern sndDecompS16_len
extern sndDecompS16_prev
extern sndDecompS16_return
extern nfHPkDecomp_ops
extern nfHPkDecomp_comp
extern nfHPkDecomp_x
extern nfHPkDecomp_y
extern nfHPkDecomp_w
extern nfHPkDecomp_h

; This is the global array of pointers to memory locations that
; need to be self-modified - AH
extern global_unlock_memory_pointers

; These are our functions that the C stubs call - AH
global _asm_sndDecompM16
global _asm_sndDecompS16
global _asm_nfPkConfig
global _asm_nfHPkDecomp

; This is our memory "unlock" function for the self-mofiying asm - AH
global _asm_selfModify

SECTION	.data

	db "(c) 1997 Interplay Productions.  All Rights Reserved.\n"
	db "This file is confidential and consists of proprietary information\n"
	db "of Interplay Productions.  This file and associated libraries\n"
	db "may not, in whole or in part, be disclosed to third parties,\n"
	db "incorporated into any software product which is not being created\n"
	db "for Interplay Productions, copied or duplicated in any form,\n"
	db "without the prior written permission of Interplay Productions.\n"
	db "Further, you may not reverse engineer, decompile or otherwise\n"
	db "attempt to derive source code of this material.\n",0

	tbuf		dd	0	;LOCAL tbuf	: PTRBYTE
	new_row		dd	0	;LOCAL new_row	:DWORD
	DiffBufPtrs	dd	0	;LOCAL DiffBufPtrs	:DWORD

	nfpk_back_right	dd	0	;LOCAL nfpk_back_right	: DWORD
	wcnt		dd	0	;LOCAL wcnt	:DWORD
	bcomp		dd	0	;LOCAL bcomp	:PTRBYTE
	
nfhpk_OpTbl: ;label dword
	dd nf0	;dword offset nf0	; Prev Same (0)
	dd nf1	;dword offset nf1	; No change (and copied to screen) (0)
	dd nf2	;dword offset nf2	; Near shift from older part of current buf (1)
	dd nf3	;dword offset nf3	; Near shift from newer part of current buf (1)
	dd nf4	;dword offset nf4	; Near shift from previous buffer (1)
	dd nf5	;dword offset nf5	; Far shift from previous buffer (2)
	dd nf6	;dword offset nf6	; Far shift from current buffer (2)
				; [Or if COMPOPS, run of no changes (0)]
	dd nf7	;dword offset nf7	;     8x8x1 (10 bytes) or low 4x4x1 (4 bytes)
	dd nf8	;dword offset nf8	; 2x2 4x4x1 (16 bytes) or 2x1 4x8x1 (12 bytes) or 1x2 8x4x1 (12 bytes)
	dd nf9	;dword offset nf9	;     8x8x2 (20 bytes) or low 4x4x2 (8 bytes) or
				; low 4x8x2 (12 bytes) or low 8x4x2 (12 bytes)
	dd nf10	;dword offset nf10	; 2x2 4x4x2 (32 bytes) or 2x1 4x8x2 (24 bytes) or 1x2 4x8x2 (24 bytes)
	dd nf11	;dword offset nf11	;     8x8x8 (64 bytes)
	dd nf12	;dword offset nf12	; low 4x4x8 (16 bytes)
	dd nf13	;dword offset nf13	; 2x2 4x4x0 (ie 2x2x8) (4 bytes)
	dd nf14	;dword offset nf14	;     8x8x0 (1 byte)
	dd nf15	;dword offset nf15	; mix 8x8x0 (2 bytes)

; signed 8-bit y * nf_width
nfpk_ShiftY	times 256 dd 0

; Constant tables

; 8-bit -8:7 x nf_width +  -8:7
nfpk_ShiftP1

%assign y -8
%rep 16 ;16
  %assign x -8
  %rep 16 ;16		
		db x,y
  %assign x x+1
  %endrep
%assign y y+1
%endrep

; 8-bit to right and below in roughly 0:14*nf_width + -14:14 (-3 cases)
; negative is
;  8-bit to left and above in roughly -14:0*nf_width + -14:14 (-3 cases)	
nfpk_ShiftP2

%assign y 0
%rep 8 ;8
  %assign x 8
  %rep 7 ;7		
		db x,y
  %assign x x+1
  %endrep
%assign y y+1
%endrep

%assign y 8
%rep 6 ;6

  %assign x -14
  %rep 14 ;14		
		db x,y
  %assign x x+1
  %endrep

  %assign x 0
  %rep 15 ;15		
		db x,y
  %assign x x+1
  %endrep

%assign y y+1
%endrep

%assign x -14
%rep 14 ;14		
		db x,14
%assign x x+1
%endrep

%assign x 0
%rep 12 ;12		
		db x,14
%assign x x+1
%endrep

; Constant tables
nfhpk_mov4l	;LABEL DWORD
; low 4x1 in 8x1 (patch +1)
;		mov eax, ebx/ecx
db 0c0h+3, 0c0h+3, 0c0h+3, 0c0h+3
db 0c0h+1, 0c0h+3, 0c0h+3, 0c0h+3
db 0c0h+3, 0c0h+1, 0c0h+3, 0c0h+3
db 0c0h+1, 0c0h+1, 0c0h+3, 0c0h+3
db 0c0h+3, 0c0h+3, 0c0h+1, 0c0h+3
db 0c0h+1, 0c0h+3, 0c0h+1, 0c0h+3
db 0c0h+3, 0c0h+1, 0c0h+1, 0c0h+3
db 0c0h+1, 0c0h+1, 0c0h+1, 0c0h+3
db 0c0h+3, 0c0h+3, 0c0h+3, 0c0h+1
db 0c0h+1, 0c0h+3, 0c0h+3, 0c0h+1
db 0c0h+3, 0c0h+1, 0c0h+3, 0c0h+1
db 0c0h+1, 0c0h+1, 0c0h+3, 0c0h+1
db 0c0h+3, 0c0h+3, 0c0h+1, 0c0h+1
db 0c0h+1, 0c0h+3, 0c0h+1, 0c0h+1
db 0c0h+3, 0c0h+1, 0c0h+1, 0c0h+1
db 0c0h+1, 0c0h+1, 0c0h+1, 0c0h+1

nfhpk_mov8	;LABEL DWORD	
; 8x1			(each two bits select a pair of colors in a reg)
; low 4x2 in 8x2	(each two bits select a duplicated color in reg)
; (patch +1)
;		mov ds:[edi+0/4/8/12], ebx/edx/ecx/ebp
; Note: Patched code specifies mov [ebp+0]... instead
;  of mov [edi+0]... to insure that 8-bit offsets are
;  used by the assembler even for offset of zero.

%assign m4 24
%rep 4
  %assign m3 24
  %rep 4
    %assign m2 24
    %rep 4
      %assign m1 24
      %rep 4
        db m1+047h,m2+047h,m3+047h,m4+047h
        %if (m1 == 24)
          %assign m1 16
        %elif (m1 == 16)
          %assign m1 8
        %elif (m1 == 8)
          %assign m1 40
        %endif
      %endrep
      %if (m2 == 24)
        %assign m2 16
      %elif (m2 == 16)
        %assign m2 8
      %elif (m2 == 8)
        %assign m2 40
      %endif
    %endrep
    %if (m3 == 24)
      %assign m3 16
    %elif (m3 == 16)
      %assign m3 8
    %elif (m3 == 8)
      %assign m3 40
    %endif
  %endrep
  %if (m4 == 24)
    %assign m4 16
  %elif (m4 == 16)
    %assign m4 8
  %elif (m4 == 8)
    %assign m4 40
  %endif
%endrep

nfhpk_mov4

; 4x2 (patch +2)
;		mov ax, bx/dx/cx/bp
; low 4x2 in 8x2 (patch +1)
;		mov eax, ebx/edx/ecx/ebp
%assign m4 0c3h
%rep 4
  %assign m3 0c3h
  %rep 4
    %assign m2 0c3h
    %rep 4
      %assign m1 0c3h
      %rep 4
        db m1,m2,m3,m4
        %if (m1 == 0c3h)
          %assign m1 0c2h
        %elif (m1 == 0c2h)
          %assign m1 0c1h
        %elif (m1 == 0c1h)
          %assign m1 0c5h
        %endif
      %endrep
      %if (m2 == 0c3h)
        %assign m2 0c2h
      %elif (m2 == 0c2h)
        %assign m2 0c1h
      %elif (m2 == 0c1h)
        %assign m2 0c5h
      %endif
    %endrep
    %if (m3 == 0c3h)
      %assign m3 0c2h
    %elif (m3 == 0c2h)
      %assign m3 0c1h
    %elif (m3 == 0c1h)
      %assign m3 0c5h
    %endif
  %endrep
  %if (m4 == 0c3h)
    %assign m4 0c2h
  %elif (m4 == 0c2h)
    %assign m4 0c1h
  %elif (m4 == 0c1h)
    %assign m4 0c5h
  %endif
%endrep

	
SEGMENT .text

_asm_selfModify:
        pushf
        ; Move the pointer to the start of the pointer array into eax
        mov eax, global_unlock_memory_pointers

        ; Load unlock addresses from _asm_nfHPkDecomp
        mov dword [eax], nf7_0
        mov dword [eax + 4], nf8_0
        mov dword [eax + 8], nf9_0
        mov dword [eax + 12], nf10_0
        mov dword [eax + 16], nf23_0
        mov dword [eax + 20], nf24_0
        mov dword [eax + 24], nf25_0
        mov dword [eax + 28], nf26_0
        mov dword [eax + 32], nf42_0

        popf
        ret

;--------------------------------------------------------------------
; Sound Management
;--------------------

;unsigned sndDecompM16(unsigned short *dst, unsigned char *src,
;			unsigned len, unsigned prev);
;
;Decompresses a mono stream containing len samples
;(src is len bytes, dst is len*2 bytes)
;prev is the previous decompression state or zero.
;Returns new decompression state.
;
_asm_sndDecompM16:  ; PROC USES ESI EDI EBX, \
;		dst:PTRWORD, src:PTRBYTE, len:DWORD, prev:DWORD
	mov	eax, [sndDecompM16_prev]

	mov	ecx, [sndDecompM16_len]
	jecxz	done

	mov	esi, [sndDecompM16_src]
	mov	edi, [sndDecompM16_dst]

	xor	ebx, ebx

lp:	mov	bl, byte [esi]
	add	esi, 1
	add	ax, word [snd_8to16 + ebx*2]
	mov	word [edi], ax
	add	edi, 2
	dec	ecx
	jnz	lp

done:	
	; Store our return value - AH
	mov	dword [sndDecompM16_return], eax
	
	ret
;sndDecompM16 ENDP

;unsigned sndDecompS16(unsigned short *dst, unsigned char *src,
;			unsigned len, unsigned prev);
;
;Decompresses a stereo stream containing len samples
;(src is len*2 bytes, dst is len*4 bytes)
;prev is the previous decompression state or zero
; (It encodes the 16-bit states of the two stereo channels
;  in its low and high order 16-bit halves.)
;Returns new decompression state.
;
_asm_sndDecompS16: ; PROC USES ESI EDI EBX, \
;		dst:PTRWORD, src:PTRBYTE, len:DWORD, prev:DWORD
	movzx	eax, word [sndDecompS16_prev]
	movzx	edx, word [sndDecompS16_prev+2]

	mov	ecx, [sndDecompS16_len]
	jecxz	Sdone

	mov	esi, [sndDecompS16_src]
	mov	edi, [sndDecompS16_dst]

	xor	ebx, ebx

Slp:	mov	bl, byte [esi]
	add	esi, 1
	add	ax, word [snd_8to16 + ebx*2]
	mov	word [edi], ax
	add	edi, 2

	mov	bl, byte [esi]
	add	esi, 1
	add	dx, word [snd_8to16 + ebx*2]
	mov	word [edi], dx
	add	edi, 2

	dec	ecx
	jnz	Slp

Sdone:	shl	edx, 16
	or	eax, edx
	
	; Store our return value - AH
	mov	dword [sndDecompS16_return], eax
	ret
	
;sndDecompS16 ENDP

;--------------------------------------------------------------------
; NextFrame (Video Decompression)
;----------------------------------

%macro NF_DECOMP_INIT 1 ;HI_COLOR_FLAG: REQ

	mov	eax, [nf_buf_prv]	;br		; DiffBufPtrs = nf_buf_prv - nf_buf_cur
	sub	eax, [nf_buf_cur]       ;br
	mov	[DiffBufPtrs], eax

	xor	ebx, ebx		; ebx = nf_fqty (convert to 32-bits)
	mov	bl, [nf_fqty]

	mov	eax, [nfHPkDecomp_x]	;br		; nf_new_x = x*SWIDTH*2^HI_COLOR_FLAG;
	shl	eax, LOG2_SWIDTH+%1 	;HI_COLOR_FLAG
	mov	[nf_new_x], eax

	mov	eax, [nfHPkDecomp_w]	;br			; nf_new_w = w*SWIDTH*2^HI_COLOR_FLAG;
	shl	eax, LOG2_SWIDTH+%1	;HI_COLOR_FLAG
	mov	[nf_new_w], eax

	mov	eax, [nfHPkDecomp_y]	;br			; nf_new_y = y*nf_fqty*SHEIGHT;
	shl	eax, LOG2_SHEIGHT
	mul	ebx	;nf_fqty
	mov	[nf_new_y], eax

	mov	eax, [nfHPkDecomp_h]	;br			; nf_new_h = h*nf_fqty*SHEIGHT;
	shl	eax, LOG2_SHEIGHT
	mul	ebx	;nf_fqty
	mov	[nf_new_h], eax

	mov	eax, [nf_new_row0]	;br	; new_row = nf_new_row0 - nf_new_w;
	sub	eax, [nf_new_w]		;br
	mov	[new_row], eax

	;; Move to correct place in current buffer
	mov	eax, [nf_buf_cur]	;br		; tbuf = nf_buf_cur
	mov	[tbuf], eax
;	%if (nfHPkDecomp_x || nfHPkDecomp_y)			; if (x||y)	
	; cmp, jnz, jz and labels are mine - AH
	cmp	dword [nfHPkDecomp_x], 0
	jne	before
	cmp	dword [nfHPkDecomp_y], 0
	je	after
before:	
	  mov	eax, [nf_new_y]		;br		;  tbuf += nf_new_y*nf_width + nf_new_x;
	  mul	dword [nf_width]	; Added dword - AH
	  add	eax, [nf_new_x]        	;br
	  add	[tbuf], eax
after:
;	%endif

%endmacro	; DECOMP_INIT

;----------------------------------------------------------------------

; nfPkConfig initializes tables used by nfPkDecomp
;  which are dependent on screen size.
_asm_nfPkConfig: ; PROC USES ESI EDI EBX

	; Build ShiftY table
	;
	lea	edi, [nfpk_ShiftY]
	mov	ebx, [nf_width]

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
;nfPkConfig ENDP
	
%macro Trans16_3 3  ; dst:req, idx:req, mask
	xor	eax, eax
	mov	al, [%2]
	mov	%1, [nf_trans16_lo + eax*2]
	xor	eax, eax
	mov	al, [%2+1]
	or	%1, [nf_trans16_hi + eax*2]
%endmacro

%macro Trans16 2 ; dst:req, idx:req
	xor	eax, eax
	mov	al, [%2]
	mov	%1, [nf_trans16_lo + eax*2]
	xor	eax, eax
	mov	al, [%2+1]
	or	%1, [nf_trans16_hi + eax*2]
%endmacro

_asm_nfHPkDecomp: ; PROC USES ESI EDI EBX, \
;		ops:PTRBYTE, comp:PTRBYTE, \
;		x:DWORD, y:DWORD, w:DWORD, h:DWORD
	
	NF_DECOMP_INIT 1

	mov	eax, [nf_back_right]	;br
	sub	eax, SWIDTH*2
	mov	[nfpk_back_right], eax

	mov	esi, [nfHPkDecomp_comp]	;br
	mov	edi, [tbuf]

	xor	eax, eax
	mov	ax, [esi]
	add	eax, esi
	mov	[bcomp], eax
	add	esi, 2

nf_StartRow:
	mov	eax, [nfHPkDecomp_w]	;br
	shr	eax, 1
	mov	[wcnt],eax
	ALIGN 4
nf_NextPair:
	dec	dword [wcnt] 	; Added dword - AH
	js	nf_NextRow
	mov	ebx, [nfHPkDecomp_ops] ; br
	mov	al, [ebx]
	inc	ebx
	mov	[nfHPkDecomp_ops], ebx

	xor	ebx, ebx
	mov	bl, al
	shr	bl, 4
	and	eax, 0Fh
	push	dword nf_NextPair ; Added dword on these two - AH
	push	dword [nfhpk_OpTbl + ebx*4]
	jmp	[nfhpk_OpTbl + eax*4]

nf_NextRow:
	add	edi, [new_row]		;br
	dec	dword [nfHPkDecomp_h]	; Added dword - AH
	jnz	nf_StartRow

	ret

;----------------------------------------
	ALIGN 4
nf0:	; No change from previous buffer
	mov	eax, [DiffBufPtrs]	; br
	jmp	nf_shift

;----------------------------------------
	ALIGN 4
nf1:	; No change (and copied to screen)

	add	edi, SWIDTH*2
	retn

;----------------------------------------
	ALIGN 4
nf2:	; Near shift from older part of current buffer
	xor	eax, eax
	mov	ebx, [bcomp]	; br
	inc	dword [bcomp]	; Added dword - AH
	mov	al, [ebx]
	mov	ax, [nfpk_ShiftP2 + eax*2]
nf_xyc_shift:
	xor	ebx, ebx
	mov	bl, ah
	shl	eax, 24
	sar	eax, 24-1
	add	eax, [nfpk_ShiftY + ebx*4]
	jmp	nf_shift

;----------------------------------------
	ALIGN 4
nf3:	; Near shift from newer part of current buffer
	xor	eax, eax
	mov	ebx, [bcomp]	; br
	inc	dword [bcomp]	; Added dword - AH
	mov	al, [ebx]
	mov	ax, [nfpk_ShiftP2 + eax*2]
	neg	al
	neg	ah
	jmp	nf_xyc_shift

;----------------------------------------
	ALIGN 4
nf4:	; Near shift from previous buffer
	xor	eax, eax
	mov	ebx, [bcomp]	; br
	inc	dword [bcomp]	; Added dword - AH
	mov	al, [ebx]
	mov	ax, [nfpk_ShiftP1 + eax*2]
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
	add	eax, [nfpk_ShiftY + ebx*4]
	add	eax, [DiffBufPtrs]	; br
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

	mov	ebx, esi	; save esi
	lea	esi, [edi+eax]
	mov	edx, [nf_width]

	%rep 7
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
	%endrep
	mov eax, [esi]
	mov [edi], eax
	mov eax, [esi+4]
	mov [edi+4], eax
	mov eax, [esi+8]
	mov [edi+8], eax
	mov eax, [esi+12]
	mov [edi+12], eax

	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8
	mov	esi, ebx	; restore esi
	
	retn

;----------------------------------------
	ALIGN 4
nf7:	;     8x8x1 (12 bytes)
	test	word [esi], 08000h
	jnz	near nf23

	xor	eax, eax

	lea	ecx, [nfhpk_mov8]
	lea	edx, [nf7_11+1] ; Removed byte ds:- AH

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

	mov	esi,[nf_width]
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
	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf7+16
nf23:	; low 4x4x1 (6 bytes)

	xor	eax, eax
	lea	ecx, [nfhpk_mov4l]
	lea	edx, [nf23_11+1] ; Removed byte ds: - AH

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

	mov	edx, [nf_width]

	; load ebx,ecx with 00,11 color combinations

	Trans16_3	cx, esi, 1
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

	sub	edi, [nfpk_back_right]
	add	esi, 6
	retn

;----------------------------------------
	ALIGN 4
nf8:	; 2x2 4x4x1 (24 bytes)
	test	word [esi], 08000h
	jnz	near nf24

	xor	eax, eax
	
	lea	ecx, [nfhpk_mov8]
	lea	edx, [nf8_11+1] ; Removed byte ds: - AH

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

	mov	esi,[nf_width]
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
	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf8+16
nf24:	; 2x1 4x8x1 (16 bytes)

	test	word [esi+8], 08000h
	jnz	near nf40

	xor	eax, eax

	lea	ecx, [nfhpk_mov8]
	lea	edx, [nf24_11+1] ; Removed byte ds: - AH

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
	Trans16_3 cx, esi, 1

	mov	esi,[nf_width]
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
	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf8+32
nf40:	; 1x2 8x4x1 (16 bytes)

	xor	eax, eax

	lea	ecx, [nfhpk_mov8]
	lea	edx, [nf40_11+1] ; Removed byte ds: - AH

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
	Trans16_3 cx, esi+8, 1
	push	ecx

	Trans16	cx, esi+2
	shl	ecx, 16
	Trans16_3 cx, esi, 1

	mov	esi,[nf_width]
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
	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
nf9:	;     8x8x2 (24 bytes)

	test	word [esi], 08000h
	jnz	near nf41

	test	word [esi+4], 08000h
	jnz	near nf25

	xor	eax, eax

	lea	ecx, [nfhpk_mov4]
	lea	edx, [nf9_11+2] ; Removed byte ds: - AH

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

	mov	esi, [nf_width]

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
	sub	edi, [nfpk_back_right]	; br
	retn

;----------------------------------------
	ALIGN 4
;nf9+16
nf25:	; low 4x4x2 (12 bytes)

	xor	eax, eax
	
	lea	ecx, [nfhpk_mov4]
	lea	edx, [nf25_11+1] ; Removed byte ds: - AH

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
	Trans16_3	cx, esi+4, 1
	shrd	eax, ecx, 16
	mov	ax, cx
	push	eax
	Trans16	cx, esi+6
	shrd	ebp, ecx, 16
	mov	bp, cx
	pop	ecx

	mov	esi, [nf_width]

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
	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf9+32
nf41:	; low 4x8x2 (16 bytes)
	test	word [esi+4], 08000h
	jnz	near nf57

	xor	eax, eax
	
	lea	ecx, [nfhpk_mov8]
	lea	edx, [nf41_11+1] ; Removed byte ds: - AH

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

	Trans16_3	cx, esi, 1
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

	mov	esi, [nf_width]

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
	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf9+48
nf57:	; low 8x4x2 (16 bytes)

	xor	eax, eax

	lea	ecx, [nfhpk_mov4]
	lea	edx, [nf57_11+2] ; Removed byte ds: - AH

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

	Trans16_3	bx, esi, 1
	Trans16	dx, esi+2
	Trans16_3 cx, esi+4, 1
	Trans16 bp, esi+6

	mov	esi, [nf_width]

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
	sub	edi, [nfpk_back_right]	;br
	retn

;----------------------------------------
	ALIGN 4
nf10:	; 2x2 4x4x2 (48 bytes)

	test	word [esi], 08000h
	jnz	near nf26

	xor	eax, eax

	lea	ecx, [nfhpk_mov4]
	lea	edx, [nf10_11+2] ; Remove byte ds: - AH

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

	mov	esi, [nf_width]

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
	mov	esi, [nf_width]

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
	mov	esi, [nf_width]
	
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
	mov	esi, [nf_width]

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
	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf10+16
nf26:	; 2x1 4x8x2 (32 bytes)

	test	word [esi+16], 08000h
	jnz	near nf42

	xor	eax, eax
	
	lea	ecx, [nfhpk_mov4]
	lea	edx, [nf26_11+2] ; Removed byte ds: - AH

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

	Trans16_3	bx, esi, 1
	Trans16	dx, esi+2
	Trans16 cx, esi+4
	Trans16 bp, esi+6

	mov	esi, [nf_width]

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
	mov	esi, [nf_width]

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
	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
;nf10+32
nf42:	; 1x2 8x4x2 (32 bytes)

	xor	eax, eax

	lea	ecx, [nfhpk_mov4]
	lea	edx, [nf42_11+2] ; removed byte ds: - AH

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

	Trans16_3	bx, esi, 1
	Trans16	dx, esi+2
	Trans16 cx, esi+4
	Trans16 bp, esi+6

	mov	esi, [nf_width]

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
	Trans16_3	bx, esi+16, 1
	Trans16	dx, esi+18
	Trans16 cx, esi+20
	Trans16 bp, esi+22
	mov	esi, [nf_width]

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
	sub	edi, [nfpk_back_right]	;br
	retn

;----------------------------------------
	ALIGN 4
nf11:	;     8x8x16 (128 bytes)
	mov	edx, [nf_width]

%macro Trans16Blk 1 ;	MACRO idx
	Trans16	bx, %1 ;idx
	mov	[edi], bx
	Trans16	bx, (%1 + 2) ;idx+2
	mov	[edi+2], bx
	Trans16 bx, (%1 + 4) ;idx+4
	mov	[edi+4], bx
	Trans16 bx, (%1 + 6) ;idx+6
	mov	[edi+6], bx
	Trans16 bx, (%1 + 8) ;idx+8
	mov	[edi+8], bx
	Trans16 bx, (%1 + 10) ;idx+10
	mov	[edi+10], bx
	Trans16 bx, (%1 + 12) ;idx+12
	mov	[edi+12], bx
	Trans16 bx, (%1 + 14) ;idx+14
	mov	[edi+14], bx
%endmacro

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
	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8
	retn

;----------------------------------------
	ALIGN 4
nf12:	; low 4x4x16 (32 bytes)

	mov	edx, [nf_width]

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

	sub	edi, [nfpk_back_right]	;br
	add	esi, 32
	retn

;----------------------------------------
	ALIGN 4
nf13:	; 2x2 4x4x0 (8 bytes)
	mov	edx, [nf_width]

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

	sub	edi, [nfpk_back_right]	; br
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
	mov	edx, [nf_width]

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

	sub	edi, [nfpk_back_right]	;br	; (SHEIGHT-1)*width+8

	retn

;----------------------------------------
	ALIGN 4
nf15:	; unused
	retn

;nfHPkDecomp ENDP


