#include "cpu/exec.h"

make_EHelper(test) {
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;
  rtlreg_t res=id_dest->val&id_src->val;
  rtl_update_ZFSF(&res,id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;
  rtlreg_t res=id_dest->val&id_src->val;
  operand_write(id_dest,&res);
  rtl_update_ZFSF(&res, id_dest->width);
  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t2,&(id_dest->val),&(id_src->val));
  operand_write(id_dest,&t2);
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;
  print_asm_template2(xor);
}

make_EHelper(xchg) {
  uint8_t temp=decoding.opcode&0x7;
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  if(id_dest->width==2)
  {
	  rtlreg_t temp_r=cpu.gpr[temp]._16;
	  cpu.gpr[temp]._16=cpu.gpr[0]._16;
	  cpu.gpr[0]._16=temp_r;
  }
  else if(id_dest->width==4)
  {
	  rtlreg_t temp_r=cpu.gpr[temp]._32;
	  cpu.gpr[temp]._32=cpu.gpr[0]._32;
	  cpu.gpr[0]._32=temp_r;
  }

  print_asm_template2(xchg);
}

make_EHelper(or) {
  rtl_or(&t2, &id_dest->val, &id_src->val);
  cpu.eflags.CF=0;
  cpu.eflags.OF=0;
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2,id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
  
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
//Log("subcode is %x, t2 is %x",subcode, t2);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  t2=id_dest->val;
  rtl_not(&t2);
  operand_write(id_dest, &t2);
  print_asm_template1(not);
}

make_EHelper(rol) {
  int temp=id_src->val;
  int tempcf=0;
  t2=id_dest->val;
  unsigned int r_width=id_dest->width;
  r_width*=8;
  unsigned int r_left=0x1;
  r_left<<=(r_width-1);
  while(temp!=0)
  {
	  if((t2&r_left)==0x0)
	  {
		  tempcf=0;
	  }
	  else
	  {
		  tempcf=1;
	  }
	  t2=t2*2+tempcf;
	  temp--;
	  if(temp==0)
	  {
		  cpu.eflags.CF=tempcf;
	  }
  }
  operand_write(id_dest, &t2);
  print_asm_template1(rol_cl);
}
