MBALIGN     equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
FLAGS       equ  MBALIGN | MEMINFO      ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum of above, to prove we are multiboot

WIDTH equ 800
HEIGHT equ 600
DEPTH equ 32
 
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
 
align 4
stack_bottom:
times 16384 db 0
stack_top:
 
section .text
global _start

_start:
	mov esp, stack_top

	extern kernel_early
	call kernel_early

	extern _init
	call _init

	extern kernel_main
	call kernel_main

	cli
.hang:
	hlt
	jmp .hang
