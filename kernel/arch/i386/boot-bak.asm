MBALIGN     equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
GFXMODE    equ  1<<2
FLAGS       equ  MBALIGN | MEMINFO | GFXMODE      ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot

MODE_TYPE equ 1
WIDTH equ 1920
HEIGHT equ 1080
DEPTH equ 32
FILLER_VAR equ 0

section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	dd FILLER_VAR
	dd FILLER_VAR
	dd FILLER_VAR
	dd FILLER_VAR
	dd FILLER_VAR
	dd MODE_TYPE
	dd WIDTH
	dd HEIGHT
	dd DEPTH
align 4
stack_bottom:
times 16384 db 0
stack_top:
 
section .text
global _start

_start:
	mov esp, stack_top

	extern kernel_early
	push ebx
	call kernel_early

	extern _init
	call _init

	extern kernel_main
	call kernel_main

	cli
.hang:
	hlt
	jmp .hang
