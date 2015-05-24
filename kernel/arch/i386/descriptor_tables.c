#include <stdint.h>
#include "kernel/descriptor_tables.h"
#include "kernel/isr.h"
#include "string.h"
#include "stdio.h"

extern void gdt_flush(uint32_t);
extern void idt_flush(uint32_t);

static void init_gdt();
static void gdt_set_gate(int32_t, uint32_t, uint32_t, uint8_t, uint8_t);
static void init_idt();
static void idt_set_gate(uint8_t, uint32_t, uint16_t, uint8_t);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

void init_descriptor_tables()
{
	init_gdt();
	init_idt();
}

void init_gdt() 
{
	gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
	gdt_ptr.base  = (uint32_t)&gdt_entries;

	gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

	gdt_flush((uint32_t)&gdt_ptr);
}

static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_entries[num].base_low    = (base & 0xFFFF);
	gdt_entries[num].base_middle = (base >> 16) & 0xFF;
	gdt_entries[num].base_high   = (base >> 24) & 0xFF;

	gdt_entries[num].limit_low   = (limit & 0xFFFF);
	gdt_entries[num].granularity = (limit >> 16) & 0x0F;

	gdt_entries[num].granularity |= gran & 0xF0;
	gdt_entries[num].access      = access;
}

static void init_idt()
{
	idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
	idt_ptr.base  = (uint32_t)&idt_entries;

	memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

	idt_set_gate( 0, (uint32_t)isr0 , 0x08, 0x8E);
	idt_set_gate( 1, (uint32_t)isr1 , 0x08, 0x8E);
	idt_set_gate( 2, (uint32_t)isr2 , 0x08, 0x8E);	
	idt_set_gate( 3, (uint32_t)isr3 , 0x08, 0x8E);	
	idt_set_gate( 4, (uint32_t)isr4 , 0x08, 0x8E);	
	idt_set_gate( 5, (uint32_t)isr5 , 0x08, 0x8E);	
	idt_set_gate( 6, (uint32_t)isr6 , 0x08, 0x8E);	
	idt_set_gate( 7, (uint32_t)isr7 , 0x08, 0x8E);	
	idt_set_gate( 8, (uint32_t)isr8 , 0x08, 0x8E);	
	idt_set_gate( 9, (uint32_t)isr9 , 0x08, 0x8E);	
	idt_set_gate( 10, (uint32_t)isr10 , 0x08, 0x8E);	
	idt_set_gate( 11, (uint32_t)isr11 , 0x08, 0x8E);	
	idt_set_gate( 12, (uint32_t)isr12 , 0x08, 0x8E);	
	idt_set_gate( 13, (uint32_t)isr13 , 0x08, 0x8E);	
	idt_set_gate( 14, (uint32_t)isr14 , 0x08, 0x8E);	
	idt_set_gate( 15, (uint32_t)isr15 , 0x08, 0x8E);	
	idt_set_gate( 16, (uint32_t)isr16 , 0x08, 0x8E);	
	idt_set_gate( 17, (uint32_t)isr17 , 0x08, 0x8E);	
	idt_set_gate( 18, (uint32_t)isr18 , 0x08, 0x8E);	
	idt_set_gate( 19, (uint32_t)isr19 , 0x08, 0x8E);	
	idt_set_gate( 20, (uint32_t)isr20 , 0x08, 0x8E);	
	idt_set_gate( 21, (uint32_t)isr21 , 0x08, 0x8E);	
	idt_set_gate( 22, (uint32_t)isr22 , 0x08, 0x8E);	
	idt_set_gate( 23, (uint32_t)isr23 , 0x08, 0x8E);	
	idt_set_gate( 24, (uint32_t)isr24 , 0x08, 0x8E);	
	idt_set_gate( 25, (uint32_t)isr25 , 0x08, 0x8E);	
	idt_set_gate( 26, (uint32_t)isr26 , 0x08, 0x8E);	
	idt_set_gate( 27, (uint32_t)isr27 , 0x08, 0x8E);	
	idt_set_gate( 28, (uint32_t)isr28 , 0x08, 0x8E);	
	idt_set_gate( 29, (uint32_t)isr29 , 0x08, 0x8E);	
	idt_set_gate( 30, (uint32_t)isr30 , 0x08, 0x8E);	
	idt_set_gate( 31, (uint32_t)isr31 , 0x08, 0x8E);

	idt_flush((uint32_t)&idt_ptr);
}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
	idt_entries[num].base_lo = base & 0xFFFF;
	idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

	idt_entries[num].sel     = sel;
	idt_entries[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
	idt_entries[num].flags   = flags /*| 0x60 */;
}

void *irq_routines[16] = 
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler)(registers_t *r))
{
	irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq)
{
	irq_routines[irq] = 0;
}

void irq_remap(void)
{
	outportb(0x20, 0x11);
	outportb(0xA0, 0x11);
	outportb(0x21, 0x20);
	outportb(0xA1, 0x28);
	outportb(0x21, 0x04);
	outportb(0xA1, 0x02);
	outportb(0x21, 0x01);
	outportb(0xA1, 0x01);
	outportb(0x21, 0x0);
	outportb(0xA1, 0x0);
}

void irq_install()
{
	irq_remap();
	idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
	idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
	idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
	idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
	idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
	idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
	idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
	idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
	idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
	idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
	idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
	idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
	idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
	idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
	idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
	idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
}

void irq_handler(registers_t *regs)
{
	void (*handler)(registers_t *regs);
	handler = irq_routines[regs->int_no - 32];

	if(handler)
	{
		handler(regs);
	}
	
	if(regs->int_no >= 40)
	{
		outportb(0xA0, 0x20);
	}

	outportb(0x20, 0x20);
}
