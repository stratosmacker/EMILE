/*
 *
 * (c) 2004 Laurent Vivier <LaurentVivier@wanadoo.fr>
 *
 */

	.global enter_kernel040
enter_kernel040:
	link.w  %fp,#0

	/* get physical start address of kernel */

	move.l	8(%fp), %a0

	/* size of image to relocate */

	move.l	12(%fp), %d0

	/* destination addresse */

	move.l	16(%fp), %a2

	/* disable MMU */

	moveq.l	#0, %d1
	movec	%d1, %tc
	movec	%d1, %itt0
	movec	%d1, %dtt0
	movec	%d1, %itt1
	movec	%d1, %dtt1

	/* copy kernel to destination address */

	move.l	%a2, %a1
	
copy_loop:
	move.b	(%a0)+, (%a2)+
	subq.l	#1, %d0
	bge.s	copy_loop

	/* jump into kernel */

	jmp	%a1@

	/* never comes here... I hope */

	unlk    %fp
	rts
	.global end_enter_kernel040
end_enter_kernel040: