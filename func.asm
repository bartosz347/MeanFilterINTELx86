

section	.text
global  _func

_func:
	push	ebp
	mov	ebp, esp
	sub	esp,4		;int c
	mov	eax,[ebp+12]    ;c=a+b
	add	eax,[ebp+8]
	mov	[ebp-4],eax
	mov	eax,[ebp-4]     ;return c
	leave
	ret
