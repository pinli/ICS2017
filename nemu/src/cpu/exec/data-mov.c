#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

/*make_EHelper(call) {
  rtl_push(&cpu.eip);
  decoding.jmp_eip =cpu.eip+vaddr_read(cpu.eip, 4);  
  decoding.is_jmp=1;

}*/

/*make_EHelper(push) {
  rtl_push(&cpu.gpr[decoding.opcode&0x7]._32);
  print_asm_template1(push);
}*/

make_EHelper(pop) {
  rtl_pop(&cpu.gpr[decoding.opcode&0x7]._32);
  print_asm_template1(pop);
}

/*make_EHelper(pushm) {
  rtl_push(&(id_dest->val));
//  Log("%x", id_dest->val);

  print_asm("pushm");
}

make_EHelper(pushIb) {
  rtl_push(&(id_dest->val));
//  Log("%x", id_dest->val);
  print_asm("pushIb");
}*/

make_EHelper(push) {
  if (id_dest->width == 1) {
	  rtl_sext(&t0, &id_dest->val, 1);
	  rtl_push(&t0);
	} else {
	  rtl_push(&id_dest -> val);
	}
	print_asm_template1(push);
}


make_EHelper(pusha) {
  rtlreg_t temp=0;
  temp=cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&temp);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);
  print_asm("pusha");
}

make_EHelper(popa) {
  rtlreg_t temp=0;
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&temp);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
  print_asm("popa");
}

make_EHelper(leave) {
  if (decoding.is_operand_size_16) {
	  cpu.gpr[R_ESP]._16=cpu.gpr[R_EBP]._16;
      rtlreg_t temp=0;
      rtl_pop(&temp);
      cpu.gpr[R_EBP]._16=temp;
  }
  else{
	  cpu.gpr[R_ESP]._32=cpu.gpr[R_EBP]._32;
      rtlreg_t temp=0;
      rtl_pop(&temp);
      cpu.gpr[R_EBP]._32=temp;
  }
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
	rtlreg_t temp=cpu.gpr[0]._16;
	rtlreg_t res;
	rtl_msb(&res, &temp, 2);
	if(res==1)
		cpu.gpr[2]._16=0xFFFF;
	else
		cpu.gpr[2]._16=0x0;
  }
  else {
	rtlreg_t temp=cpu.gpr[0]._32;
	rtlreg_t res;
	rtl_msb(&res, &temp, 4);
	if(res==1)
		cpu.gpr[2]._32=0xFFFFFFFF;
	else
		cpu.gpr[2]._32=0x0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
	  int16_t temp = (int16_t) (int8_t) (uint8_t) reg_b(R_AL);
	  reg_w(R_AX) = temp;
  }
  else {
	  int32_t temp = (int32_t) (int16_t) (uint16_t) reg_w(R_AX);
	  reg_l(R_EAX) = temp;
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
//  Log("id_src->val is %x, id_dest->val is %x", id_src->val, id_dest->val);
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
//  Log("id_src->val is %x, id_dest->val is %x", id_src->val, id_dest->val);
//  Log("t2 is %x", t2);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
