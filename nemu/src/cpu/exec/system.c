#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
//  Log("id_dest->reg is %x, id_dest->value is %x", id_dest->reg, id_dest->val);
//  uint32_t size_idtr=id_dest->reg;
//  Log("the data[0] is %x", vaddr_read(id_dest->reg, 2));
  cpu.idtr.limit=vaddr_read(id_dest->reg, 2);
  uint32_t temp=vaddr_read(id_dest->reg+4, 2);
  temp<<=16;
  temp+=vaddr_read(id_dest->reg+2, 2);
  cpu.idtr.base=temp;
//  Log("cpu.IDTR.Limit is %x, cpu.IDTR.Base is %x", cpu.IDTR.Limit, cpu.IDTR.Base);

//  Log("vaddr_read(id_dest->reg, 2) is %x, vaddr_read(id_dest->reg+2, 2) is %x, vaddr_read(id_dest->reg+4, 2) is %x", vaddr_read(id_dest->reg+4, 2), vaddr read(id_dest->reg+4, 2), vaddr_read(id_dest->reg+4, 2));
//  uint16_t * addr_idtr=(uint16_t *) size_idtr;
//  cpu.IDTR.Limit=addr_idtr[0];
//  Log("addr_idtr[0] is %x, addr_idtr[1] is %x, addr_idtr[2] is %x", addr_idtr[0], addr_idtr[1], addr_idtr[2]);
/*  uint32_t temp=addr_idtr[1];
  temp<<=16;
  temp+=addr_idtr[2];
  cpu.IDTR.Base=temp;*/
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

extern void raise_intr(uint8_t NO, vaddr_t ret_addr);

make_EHelper(int) {
  uint8_t the_NO=id_dest->val;
  raise_intr(the_NO, decoding.seq_eip);
  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  rtl_pop(&decoding.jmp_eip);
  rtl_pop((rtlreg_t *)&cpu.cs);
  rtl_pop((rtlreg_t *)&cpu.eflags_ini);
  
  decoding.is_jmp = 1;

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
//  Log("pio_read(cpu.gpr[R_AL]._8[0], 1) is %x", pio_read(cpu.gpr[R_AL]._8[0], 1));
//  cpu.gpr[R_AL]._8[0]= pio_read(cpu.gpr[R_DX]._16, 1);
 t0 = pio_read(id_src->val, id_src->width);
   operand_write(id_dest, &t0);
  
	print_asm_template2(in);
#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
//  Log("start cpu.gpr[R_DX]._16 is %x, cpu.gpr[R_AL]._8[0] is %x", cpu.gpr[R_DX]._16, cpu.gpr[R_AL]._8[0]);
//  pio_write(cpu.gpr[R_DX]._16, 1, cpu.gpr[R_AL]._8[0]);
//  Log("end cpu.gpr[R_DX]._16 is %x, cpu.gpr[R_AL]._8[0] is %x", cpu.gpr[R_DX]._16, cpu.gpr[R_AL]._8[0]);

pio_write(id_dest->val, id_src->width, id_src->val);  
	print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(inl) {
//  Log("pio_read(cpu.gpr[R_AL]._8[0], 1) is %x", pio_read(cpu.gpr[R_AL]._8[0], 1));
  cpu.gpr[R_AL]._32=pio_read(cpu.gpr[R_DX]._16, 4);
//  Log("cpu.gpr[R_DX]._16 is %x, cpu.gpr[R_AL]._8[0] is %x", cpu.gpr[R_DX]._16, cpu.gpr[R_AL]._8[0]);
  print_asm_template2(inl);
#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(outl) {
//  Log("cpu.gpr[R_DX]._16 is %x, cpu.gpr[R_AL]._8[0] is %x", cpu.gpr[R_DX]._16, cpu.gpr[R_AL]._8[0]);
  pio_write(cpu.gpr[R_DX]._16, 1, cpu.gpr[R_AL]._16);
  print_asm_template2(outl);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
