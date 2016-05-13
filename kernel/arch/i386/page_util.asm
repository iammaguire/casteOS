global paging_load_PDBR
paging_load_PDBR:
	mov eax, [esp + 4]
	mov cr3, eax
	ret

global paging_get_PDBR
paging_get_PDBR:
	mov eax, cr3
	ret

global flush_TLB
flush_TLB:
	mov eax, [esp + 4]
	invlpg [eax]
	ret

global paging_enable
paging_enable:
	mov eax, cr0
	cmp dword [esp + 4], 1
	je enable
	jne disable
enable:
	or eax, 0x80000000
	mov cr0, eax
	jmp done
disable:
	and eax, 0x7FFFFFFF
	mov cr0, eax
done:
	ret