#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
/*	vaddr_t int_intr=cpu.IDTR.Base+8*NO;
	push(cpu.EFLAGS);
	push(cpu.cs);
	push(ret_addr);
	cpu.eip=int_intr;*/

	rtlreg_t temp_flag=cpu.eflags_ini;
	rtl_push((rtlreg_t *)&temp_flag);
	cpu.eflags.IF=0;
	rtlreg_t temp_cs=cpu.cs;
	rtl_push((rtlreg_t *)&temp_cs);
	rtl_push((rtlreg_t *)&ret_addr);
	uint32_t IDT_start=cpu.idtr.base;
	uint32_t DEST_low=vaddr_read(IDT_start+NO*8+0,2);
	uint32_t DEST_high=vaddr_read(IDT_start+NO*8+6,2);
	uint32_t DEST_res=(DEST_high<<16)+DEST_low;
	decoding.jmp_eip=DEST_res;
	decoding.is_jmp=true;

}

void dev_raise_intr() {
}
