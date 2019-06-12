#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(jmp_b) {
  char temp=id_dest->val;
  decoding.jmp_eip =decoding.seq_eip+temp;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  rtl_push(&decoding.seq_eip); 
  decoding.jmp_eip =decoding.seq_eip+id_dest->val;  
//Log("Build time: %x,jmp_eip is %x,seq_eip is %x", vaddr_read(cpu.eip+1, 4),decoding.jmp_eip,decoding.seq_eip);
  decoding.is_jmp=1;    
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(call_ind) {
  // the target address is calculated at the decode stage
  rtl_push(&decoding.seq_eip); 
  decoding.jmp_eip =id_dest->val;  
//Log("Build time: %x,jmp_eip is %x,seq_eip is %x", vaddr_read(cpu.eip+1, 4),decoding.jmp_eip,decoding.seq_eip);
  decoding.is_jmp=1;    
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
//  rtl_pop(&decoding.seq_eip);
  if(decoding.is_operand_size_16) {
		rtl_pop(&decoding.jmp_eip);
		decoding.jmp_eip &= 0x0000ffff;
		} else {
		rtl_pop(&decoding.jmp_eip);
		}
    decoding.is_jmp = 1;
  print_asm("ret");
}

make_EHelper(call_rm) {
  rtl_push(&decoding.seq_eip);
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1; 
  print_asm("call *%s", id_dest->str);
}