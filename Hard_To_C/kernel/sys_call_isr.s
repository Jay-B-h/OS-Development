bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call			; The C function that sys_call_isr will call
sys_call_isr:
	;push esp
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	push eax
	push ss
	push gs
	push fs
	push es
	push ds
	push esp
	call sys_call
	mov esp, eax
	pop ds
	pop es
	pop fs
	pop gs
	pop ss
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
;	pop esp
	iret
