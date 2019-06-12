#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
extern WP* new_wp();
extern void free_wp(int NO);
extern void all_watchpoint();
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_w(char *args) {
	WP *p=new_wp();
	sscanf(args,"%s",p->watch_str);
	return 0;
}

static int cmd_n(char *args){
	cpu_exec(1);
	return 0;
}

static int cmd_d(char *args){
	int watch_NO;
	sscanf(args,"%d",&watch_NO);
	free_wp(watch_NO);
	return 0;
}

static int cmd_si(char *args){
	char *num=strtok(args," ");
	int si_num;
	sscanf(num,"%d",&si_num);
	cpu_exec(si_num);
	return 0;
}

static int cmd_info(char *args){
    char *info_flag=strtok(args," ");
    if(strcmp(info_flag,"r")==0)
    {
			printf("eax      %#10x    %10d\n",cpu.eax,cpu.eax);
			printf("ecx      %#10x    %10d\n",cpu.ecx,cpu.ecx);
			printf("edx      %#10x    %10d\n",cpu.edx,cpu.edx);
			printf("ebx      %#10x    %10d\n",cpu.ebx,cpu.ebx);
            printf("esp      %#10x    %10d\n",cpu.esp,cpu.esp);
            printf("ebp      %#10x    %10d\n",cpu.ebp,cpu.ebp);
            printf("esi      %#10x    %10d\n",cpu.esi,cpu.esi);
            printf("edi      %#10x    %10d\n",cpu.edi,cpu.edi);
            printf("the CF is %d,the ZF is %d\n",cpu.eflags.CF,cpu.eflags.ZF);
            printf("the SF is %d,the IF is %d\n",cpu.eflags.SF,cpu.eflags.IF);
            printf("the OF is %d\n",cpu.eflags.OF);
            printf("the EIP is %x\n",cpu.eip);
     }
	else if(strcmp(info_flag,"w")==0)
    {
          all_watchpoint();
	}
	return 0;
}

static int cmd_expr(char *args){
	char *num=strtok(args," ");
	bool *success=false;
	printf("the result is %d\n",expr(num,success));
	return 0;
}

static int cmd_x(char *args){
	    char *arg=strtok(args," ");
		uint32_t num;
        sscanf(arg,"%u",&num);
 		arg=strtok(NULL," ");
 		uint32_t addr;
 		sscanf(arg,"%x",&addr);
 		uint32_t plusone=0x0;

 		int i=num/4,j=num%4;
 		int sum=0;
 		int k;
 		for(k=0;k<i;k++)
        {
                printf("%#x:    ",addr+plusone);
	            plusone+=0x00000010;
		        int kin;
	            for(kin=0;kin<4;kin++)
 				{
			            printf("%0#10x    ",vaddr_read(addr+sum*4,4));
						sum++;
	            }
                printf("\n");
	    }			
	    if(j>0)
		{
			    printf("%#x:    ",addr+plusone);
                for(k=0;k<j;k++)
				{
					printf("%0#10x    ",vaddr_read(addr+sum*4,4));
					sum++;
				}
				printf("\n");
		}

		return 0;

}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "x","Scan memory", cmd_x},
  { "n", "Next Step", cmd_n},
  { "d","Delete WatchPoint", cmd_d},
  { "info","Reg inof", cmd_info},
  { "w","Set WatchPoint", cmd_w},
  { "si","Several Step", cmd_si},
  { "p","Calculate expression", cmd_expr},

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}