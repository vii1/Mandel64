; Rutinas del BASIC
LDFAC1 = $BBA2  ; Load FAC#1 from memory (A:L, Y:H)
LDFAC2 = $BA8C  ; Load FAC#2 from memory (A:L, Y:H)
STFAC1 = $BBD4  ; Store FAC#1 in memory (X:L, Y:H)
TFAC12 = $BC0C  ; Copy FAC#1 into FAC#2
TFAC21 = $BBFC  ; Copy FAC#2 into FAC#1
FMUL   = $BA28    ; Multiply
FMULA  = $BA59   ; Multiply by A
FADDM  = $B867   ; Add FAC#1 + MEM (A:L, Y:H)
FADD   = $B86A    ; Add FAC#1 + FAC#2
FCMP   = $BC5B    ; Compare FAC#1 with memory (A:L, Y:H) -> A (0, 1, -1)
FNEG   = $BFB4    ; Negate FAC#1 if nonzero

float_inc	.block

ldaddr	.macro	addr
	lda #<\addr
	ldy #>\addr
	.endm

ldaddr_st .macro	addr
	ldx #<\addr
	ldy #>\addr
	.endm

	.bend

float_t	.struct	exp = 0, sgn = 0, man = 0
	.byte	\exp
	.byte	((bool(\sgn)?1:0) << 7) | ((\man >> 24) & $7f)	; La mantisa se guarda en big-endian
	.byte	(\man >> 16) & $ff
	.byte	(\man >> 8) & $ff
	.byte	\man & $ff 
	.ends

dfloat	.macro	val
	.if type(\val) == gap
		.byte ?,?,?,?,?
	.else
	f := float(\val)
	.if f == 0.0
		.dstruct float_t, 0,0,0
	.else
		; extraemos el signo
		sgn = sign(f) < 0 ? 1 : 0
		; lo forzamos a positivo
		f := abs(f)
		; Calculamos exponente y mantisa:
		; La mantisa debe estar normalizada entre [0.5..1).
		; Si f ya está en este rango, el exponente será 0.
		; si f < 0.5, el exponente será negativo.
		; Si f >= 1, será positivo.
		ex := 0
		.if f < 0.5
			; multiplicamos por 2 hasta que f >= 0.5
			.for ex = 0, f < 0.5, ex = ex - 1
				f *= 2
			.next
		.elsif f >= 1
			; dividimos entre 2 hasta que f < 1
			.for ex = 0, f >= 1.0, ex = ex + 1
				f /= 2
			.next
		.endif
		; Sumamos 128 al exponente para convertirlo en un valor positivo de 8 bits.
		.cerror ex < -127 || ex > 127, "Float constant out of bounds: " .. repr(\val) 
		ex += 128
		; Lo que queda de f es la mantisa normalizada.
		; Para convertirla a un valor de 32 bits, multiplicamos por $100000000 y truncamos.
		; El bit más significativo, que siempre será 1, lo sustituiremos por el bit de signo.
		m = int( trunc( f * $100000000 ) )
		; ¡Hecho!
		.dstruct float_t, ex, sgn, m
		;.warn format("%f -> %02x%08x", \val, ex, (sgn<<31) | (m & $7fffffff))
	.endif
	.endif
	.endm

ldfac1	.macro	addr
	#float_inc.ldaddr \addr
	jsr LDFAC1
	.endm

ldfac2	.macro	addr
	#float_inc.ldaddr \addr
	jsr LDFAC2
	.endm

stfac1	.macro	addr
	#float_inc.ldaddr_st \addr
	jsr STFAC1
	.endm

tfac12	.macro
	jsr TFAC12
	.endm

tfac21	.macro
	js_ TFAC21
	.endm

fmul	.macro    byte = ?
	.if type(\byte) == gap
		jsr FMUL
	.else
		lda \byte
		jsr FMULA
	.endif
	.endm

fadd	.macro    addr = ?
	.if type(\addr) == gap
		jsr FADD
	.else
		#float_inc.ldaddr \addr
		jsr FADDM
	.endif
	.endm

fcmp	.macro    addr
	#float_inc.ldaddr \addr
	jsr FCMP
	.endm

fneg	.macro
	jsr FNEG
	.endm

fmov	.macro    dst, src
	ldy #4
_loop   lda \src,y
	sta \dst,y
	dey
	bpl _loop
	.endm