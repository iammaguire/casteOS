[global read_cr0]
read_cr0:
	mov eax, cr0
	ret

[global read_cr2]
read_cr2:
	mov eax, cr2
	ret

[global read_cr3]
read_cr3:
	mov eax, cr3
	ret

[global read_cr4]
read_cr4:
	mov eax, cr4
	ret

[global write_cr0]
write_cr0:
	mov eax, [esp+4]
	mov cr0, eax
	ret

[global write_cr3]
write_cr3:
	mov eax, [esp+4]
	mov cr3, eax
	ret

[global write_cr4]
write_cr4:
	mov eax, [esp+4]
	mov cr4, eax
	ret

[global invalidate_tlb]
invalidate_tlb:
	mov eax, [esp+4]
	and eax, 0xFFFFF000
	invlpg [eax]
	ret

[global enable_paging]
enable_paging:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret
