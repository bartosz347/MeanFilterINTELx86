
; mean_filter arguments
%define     SRC_IMG 	ebp+20
%define     OUT_IMG     ebp+24
%define     WIDTH       ebp+8
%define     HEIGHT      ebp+12
%define     WINDOW      ebp+16

; local variables
%define     kernelX     ebp-4
%define     kernelY     ebp-8
%define     cursorX     ebp-12
%define     cursorY     ebp-16
%define     window_size ebp-20 ; window/2
%define     window_area ebp-24 ; window*window


;; todo
;; cleanup
;; window_size, window_area are used as bytes, not dwords
;; eliminate jmp ?
;; window can't be greater than 15, (limits: 255*k is saved on 16bits, window*window  is saved on 8 bits

section	    .text
global      _mean_filter

_mean_filter:
    push    ebp
	mov	    ebp, esp

    mov     dword [kernelX], 0          ; kernel.x = 0
    mov     dword [kernelY], 0          ; kernel.y = 0
    mov     dword [cursorX], 0
    mov     dword [cursorY], 0
    mov     dword [window_size], 0
    mov     dword [window_area], 0
    mov     ax, word [WINDOW]           ; ax = WINDOW
    mov     dl,2
    div     dl                          ; al = ax / dl
    mov     ah,0
    mov     byte [window_size], al      ; window_size = window/2
    mov     ax, word [WINDOW]           ; ax = WINDOW
    mov     ebx, dword [WINDOW]         ; ax = WINDOW
    mul     bl
    mov     byte[window_area], al


start_new_kernel:
    PSLLDQ  xmm2,64
    mov     esi, dword [kernelX]        ; esi = kernel.x
    mov     edi, dword [kernelY]
    mov     eax, 0
    mov     al, byte [window_size]      ; al = window_size
    mov     dword [cursorX], esi        ; cursor.x = kernel.x
    mov     dword [cursorY], edi
    sub     dword [cursorX], eax        ; cursor.x = cursor.x - window_size
    sub     dword [cursorY], eax


process_next_column_in_row:

    cmp     dword [cursorX], 0
    jge     cursor_x_gt_zero
    mov     dword [cursorX], 0
cursor_x_gt_zero:
    cmp     dword [cursorY], 0
    jge     cursor_y_gt_zero
    mov     dword [cursorY], 0
cursor_y_gt_zero:

    mov     eax,0
    mov     al, byte [window_size]      ; al = window_size
    mov     esi, dword [kernelX]        ; esi = kernel.x
    add     esi, eax                    ; esi = kernel.x + window_size
    mov     ecx, 0                      ; index (c.x + c.y * width) will be stored in ecx, ecx = 0
    mov     eax, dword [cursorY]        ; eax = cursor.y
    mul     dword [WIDTH]               ; edx:eax = cursor.y * width
    add     eax, dword [cursorX]        ; index = index + cursor.x
    mov     ecx, [SRC_IMG]              ; adding to index, address of src_img

    cmp     dword [cursorX], esi
    jge     load_single_rgba_to_xmm

load_double_rgba_to_xmm:
    PMOVZXBW xmm1, [4*eax+ecx]          ; loading 2xRGBA to xmm1
    PADDQ   xmm2,xmm1                   ; xmm2 += xmm1

    add     dword [cursorX], 2          ; cursor.x += 2
    mov     edx, dword [cursorX]        ; edx = cursor.x
    cmp     edx, dword [WIDTH]
    jge     move_cursor_to_next_row_in_kernel

    mov     eax,0
    mov     al, byte [window_size]      ; al = window_size
    mov     esi, dword [kernelX]        ; esi = kernel.x
    add     esi, eax                    ; esi = kernel.x + window_size
    cmp     dword [cursorX],esi
    jle     process_next_column_in_row
    jmp     move_cursor_to_next_row_in_kernel


load_single_rgba_to_xmm:
    PSLLDQ  xmm1,64
    PINSRB  xmm1, [4*eax+ecx], 0        ; Insert a byte integer R
    PINSRB  xmm1, [4*eax+ecx+1], 2      ; Insert a byte integer G
    PINSRB  xmm1, [4*eax+ecx+2], 4      ; Insert a byte integer B
    PINSRB  xmm1, [4*eax+ecx+3], 6      ; Insert a byte integer A

    PADDQ   xmm2,xmm1                   ; xmm2 += xmm1

move_cursor_to_next_row_in_kernel:
    inc     dword [cursorY]             ; cursor.y +=1
    mov     eax,0
    mov     al, byte [window_size]      ; al = window_size
    mov     esi, dword [kernelX]        ; esi = kernel.x
    mov     dword [cursorX], esi        ; cursor.x = kernel.x
    sub     dword [cursorX], eax        ; cursor.x = cursor.x - window_size

    mov     edi, dword [kernelY]        ; esi = kernel.y, probably loading not needed todo
    add     edi, eax
    cmp     [cursorY], edi
    jg      proc
    mov     edi, [cursorY]
    cmp     edi, [HEIGHT]
    jl      process_next_column_in_row
proc:

    ;; xmm2 = xmm2/n*n
    MOVDQ2Q mm0, xmm2                   ; mm0 = low xmm2
    PSRLDQ  xmm2,8                      ; xmm2 shift right
    MOVDQ2Q mm1, xmm2                   ; mm1 = low xmm2

    paddq   mm0,mm1                     ; mm0 += mm1



    ;; setup index
    mov     ebx, 0                      ; index (k.x + k.y * width) will be stored in ecx, ecx = 0
    mov     eax, dword [kernelY]        ; eax = kernel.y
    mul     dword [WIDTH]               ; edx:eax = kernel.y * width
    add     eax, dword [kernelX]        ; index = index + kernel.x
    mov     esi, [OUT_IMG]              ; adding to index, address of out_img
    mov     ebx, eax
    ;;

    ;; saving mm0 to array
    mov     eax,0
    mov     edx,0
    mov     dl, byte[window_area]
    movd    ecx,mm0                     ;ecx = LOW mm0

    mov     ah,ch
    mov     al,cl
    div     dl
    mov     byte[esi+4*ebx],al          ; R

    sar     ecx, 16

    mov     ah,ch
    mov     al,cl
    div     dl
    mov     byte[esi+4*ebx+1],al        ; G

    psrlq   mm0,32
    movd    ecx,mm0

    mov     ah,ch
    mov     al,cl
    div     dl
    mov     byte[esi+4*ebx+2],al        ; B

    sar     ecx, 16

;    mov     ah,ch
;    mov     al,cl
;    div     dl
;    mov     byte[esi+4*ebx+3],al
    mov      byte[esi+4*ebx+3],255             ; A, always 255


    inc     dword [kernelX]
    mov     edi, [kernelX]
    cmp     edi, [WIDTH]
    jl      skip1
    mov     dword [kernelX],0
    inc     dword [kernelY]

skip1:
    mov     edi, [kernelY]
    cmp     edi,[HEIGHT]
    jl      start_new_kernel



	mov     eax,0     ;return 0
	leave
	ret