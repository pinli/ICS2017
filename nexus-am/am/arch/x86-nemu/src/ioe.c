#include <am.h>
#include <x86.h>
#include <stdio.h>

#define RTC_PORT 0x48   // Note that this is not standard
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  unsigned long timetemp=inl(RTC_PORT);
  timetemp-=boot_time;
  return timetemp;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
//	printf("draw_rect test x is %d, y is %d, w is %d, h is %d\n", x, y, w, h);
    int i;
    for(i=0; i<h; i++)
	{
//		for(j=0;j<w;j++)
//		fb[(y+i)*_screen.width+x+j]=pixels[i+j];
		memcpy(fb + (y + i) * _screen.width + x, pixels + i * w, w * 4);
	}
}

void _draw_sync() {
}
//static int flag_key=0;
//#define I8042_DATA_PORT 0x60 
//#define I8042_STATUS_PORT 0x64
int _read_key() {
/*  int keytemp=0;
  if(inb(0x64)!=0x0)
  {
	 if(flag_key==0)
	 {
	    keytemp=inb(0x60);
		flag_key=1;
		return keytemp+0x8000;
	 }
	  else if (flag_key==1)
	 {
		keytemp=inb(0x60);
		flag_key=0;
		return keytemp;
	 }
   }
  return 0;*/
	uint8_t haskey = inb(0x64);
	if (haskey) {
		return inl(0x60);
	} else {
		return _KEY_NONE;
	}
}
