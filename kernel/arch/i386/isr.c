#include "kernel/isr.h"
#include "stdio.h"
#include "stdlib.h"

const char *exception_messages[] =
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

// This gets called from our ASM interrupt handler stub.
void isr_handler(registers_t *regs)
{
	if(regs->int_no < 32)
	{
		printf("%s", exception_messages[regs->int_no]);
		printf(" Exception. System halted.\nStarting register dump.\n\n");

		char edi[20];
		char esi[20];
		char ebp[20];
		char esp[30];
		char ebx[20];
		char edx[20];
		char ecx[20];
		char eax[20];
		char eip[20];
		char cs[20];
		char ss[20];

		printf("EDI: 0x%s\n", itoa(regs->edi, edi, 16));
		printf("ESI: 0x%s\n", itoa(regs->esi, esi, 16));
		printf("EIP: 0x%s\n", itoa(regs->eip, eip, 16));
		printf("EBP: 0x%s\t", itoa(regs->ebp, ebp, 16));
		printf("ESP: 0x%s\n", itoa(regs->esp, esp, 16));

		printf("CS: 0x%s\t", itoa(regs->cs, cs, 16));
		printf("SS: 0x%s\n", itoa(regs->ss, ss, 16));

		printf("EAX: 0x%s\t", itoa(regs->eax, eax, 16));
		printf("EBX: 0x%s\t", itoa(regs->ebx, ebx, 16));
		printf("ECX: 0x%s\t", itoa(regs->ecx, ecx, 16));
		printf("EDX: 0x%s\n", itoa(regs->edx, edx, 16));

		for(;;);
	}
}
