#include "common.h"
#include "syscall.h"

extern ssize_t fs_write(int fd, const void *buf, size_t len);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern int fs_open(const char *pathname, int flags, int mode);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern int fs_close(int fd);
extern void _halt(int code);
extern void _putc(char ch);
extern char _end;
extern intptr_t p_brk;

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  uintptr_t result=0;


  switch (a[0]) {
	case SYS_none:
		result=1;
		break;
	case SYS_write:
//		Log("test %d %d %d %d", a[0], a[1], a[2], a[3]);
/*		if(a[1]==1||a[1]==2)
		{
			int i;
			for(i=0;i<a[3];i++)
			{
				_putc(((char *)a[2])[i]);
			}
		}
		result=(uintptr_t)a[3];*/
		result = fs_write(a[1], (void *)a[2], a[3]);
		break;
	case SYS_open:
		result=fs_open((char *)a[1], 0, 0);
		break;
	case SYS_close:
		result = fs_close(a[1]);
		break;
	case SYS_read:
//		Log("the buf is %d", a[1]);  
		result = fs_read(a[1], (void *)a[2], a[3]);
		break;
	case SYS_lseek:
		result=fs_lseek(a[1], a[2], a[3]);
		break;
	case SYS_brk:
//		_end=(char)a[1];
		result = 0;
		break;
	case SYS_exit: 
		_halt(a[1]); 
		break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  SYSCALL_ARG1(r)=result;
  return r;
}
