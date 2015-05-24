global gdt_flush 

gdt_flush:
	mov eax, [esp+4]  
	lgdt [eax]     

	mov ax, 0x10     
	mov ds, ax      
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush 
.flush:
   ret

global idt_flush    ; Allows the C code to call idt_flush().

idt_flush:
   mov eax, [esp+4]  ; Get the pointer to the IDT, passed as a parameter. 
   lidt [eax]        ; Load the IDT pointer.
   ret
