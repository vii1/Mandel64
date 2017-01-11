
	.section basic

basicstub
	.block
	.word (+), 0		; puntero a siguiente linea y nº de línea
	.null $9e, ^main	; comando SYS
+	.word (+), 1
	.null $8f, " mandelbrot by vii"	; REM
+	.word (+), 2
	.null $8f, " 2017"
+	.word 0			; fin
	.bend

	.send