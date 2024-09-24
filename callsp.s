	.import	popax, popa
	.export	_callsp

_callsp:
	sta	param
	stx	param+1
	jsr	popa
	sta	cmd
	jsr	popax
	sta	spjsr+1
	stx	spjsr+2

spjsr:	jsr	$0000
cmd:	.byte	$00
param:	.addr	$0000
	ldx	#$00
	rts
