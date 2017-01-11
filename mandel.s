; ----------
; MANDELBROT
; ----------

	.include "c64.inc"
	.include "float.inc"

	.include "sections.inc"
	.include "basicstub.inc"

; ---

ptr1    =       $FB      ; word ptr
tmp     =       $FD      ; word ptr

bmp     =       $2000
screen  =       $400
color   =       $D800

max_iter = 3

; ---

movw    .macro  dst, src
	lda \src
	sta \dst
	lda \src+1
	sta \dst+1
        .endm

shl2or  .macro  n
	asl a
	asl a
	ora \n
        .endm

	.section code

main    .proc
	lda #11
	sta VIC_BORDERCOLOR       ; color de borde: gris 1
	lda #0
	sta VIC_BG_COLOR0       ; color de fondo: negro
	
	lda #$18
	sta VIC_VIDEO_ADR       ; la memoria de bitmap comienza en $2000
	lda #$30
	sta VIC_CTRL1       ; modo bit-map
	lda #$10
	sta VIC_CTRL2       ; multicolor

clrcol	ldx #0          ; memoria de color por defecto:
_loop	lda #$98        ; 0=negro, 1=marrón, 2=naranja, 3=amarillo
	sta screen,x
	sta screen + $100,x
	sta screen + $200,x
	sta screen + $300,x
	lda #7
	sta color,x
	sta color + $100,x
	sta color + $200,x
	sta color + $300,x
	inx
	bne _loop

clrfb                  ; limpiamos framebuffer
	stx ptr1        ; ptr1 = bmp
	ldx #>bmp
	stx ptr1+1
	ldy #0
	lda #0
	
_loop1	sta (ptr1),y    ; memset( bmp, 0, 0x2000 )
	iny
	bne _loop1
	inx
	cpx #>bmp+$20
	beq _end
	stx ptr1+1
	bne _loop1
_end

domandel
	#movw ptr1, bmp
	lda #25
	lda #40
	pha
	jsr mandel
	sta tmp
	#shl2or tmp
	#shl2or tmp
	#shl2or tmp
	sta ptr1
	inc ptr1        ; *(ptr1++) = pixel
	#ldfac1 x0
	#fadd xstep
	#stfac1 x0       ; x0 += xstep
	pla
	tax
	dex
	bne _cont
	#fmov x0, kx0    ; x0 = kx0
	#ldfac1 y0
	#fadd ystep
	#stfac1 y0       ; y0 += ystep
_cont
	rts
        .pend

; ============
; Mandel
; In: x0, y0
; Out: A (n. iteraciones)
; ============
mandel  .proc

	lda #0          ; num iters = 0
	pha
	jsr FMULA
	#stfac1 cx       ; cx = 0
	#stfac1 cy       ; cy = 0
_loop
	#ldfac1 cx
	#tfac12
	#fmul
	#stfac1 cx2      ; cx2 = cx * cx
	#ldfac1 cy
	#tfac12
	#fmul
	#stfac1 cy2      ; cy2 = cy * cy
	
	#fneg
	#fadd cx2       
	#fadd x0        
	#stfac1 xtemp    ; xtemp = cx2 - cy2 + x0
	
	#ldfac1 cx
	#ldfac2 cy
	#fmul
	#fmul #2
	#fadd y0
	#stfac1 cy       ; cy = 2*cx*cy + y0

	#fmov cx, xtemp  ; cx = xtemp
	
	pla
	tax
	inx             ; num iters ++
	cpx #max_iter
	bpl _end        ; if num iters >= max_iter
	txa
	pha
	
	#ldfac1 cx2
	#fadd cy2
	#fcmp k4f
	gmi _loop       ; if cx2 + cy2 < 4
_end
	pla
	rts

        .pend

        .send code

; === Mandel data ===

	.section data
		.dfloat 11879546.0
x0      .dfloat -2.5
y0      .dfloat -1
cx      .dfloat 0
cy      .dfloat 0
cx2     .dfloat 0          ; cx ^ 2
cy2     .dfloat 0          ; cy ^ 2
xtemp   .dfloat 0

	;.rodata

kx0     .dfloat -2.5
k4f     .dfloat 4
xstep   .dfloat 0.0109375  ; (1 - (-2.5)) / 320
ystep   .dfloat 0.01       ; (1 - (-1)) / 200
	
        .send data
        