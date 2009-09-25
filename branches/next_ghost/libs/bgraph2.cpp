/*
 *  This file is part of Skeldal project
 * 
 *  Skeldal is free software: you can redistribute 
 *  it and/or modify it under the terms of the GNU General Public 
 *  License as published by the Free Software Foundation, either 
 *  version 3 of the License, or (at your option) any later version.
 *
 *  OpenSkeldal is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Skeldal.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  --------------------
 *
 *  Project home: https://sourceforge.net/projects/skeldal/
 *  
 *  Last commit made by: $Id$
 */
#include <inttypes.h>
#include <cstdio>
#include <cstdlib>
#include "libs/bgraph.h"
#include "libs/memman.h"

//uint16_t *screen;
//uint16_t curcolor,charcolors[7] = {0x0000,RGB555(0,31,0),RGB555(0,28,0),RGB555(0,24,0),RGB555(0,20,0),0x0000,0x0000};
uint16_t curcolor, charcolors[7];
//long scr_linelen;
//long scr_linelen2;
//long dx_linelen;
uint16_t *curfont,*writepos,writeposx;
uint8_t fontdsize=0;
uint8_t *palmem=NULL,*xlatmem=NULL;
void (*showview)(uint16_t,uint16_t,uint16_t,uint16_t);
char line480=0;
long screen_buffer_size=0;
char banking=0;
char screenstate=0;
char __skip_change_line_test=0;
char no_restore_mode=0;

uint16_t *mscursor,*mssavebuffer=NULL;
int16_t mscuroldx=0,mscuroldy=0;
int16_t mscuroldxs=1,mscuroldys=1;
char write_window=0;
long pictlen; // Tato promenna je pouze pouzita v BGRAPH1.ASM

void text_mode();

void wait_retrace();

void Bgraph2_Init(void) {
	charcolors[0] = 0;
	charcolors[1] = RGB555(0,31,0);
	charcolors[2] = RGB555(0,28,0);
	charcolors[3] = RGB555(0,24,0);
	charcolors[4] = RGB555(0,20,0);
	charcolors[5] = 0;
	charcolors[6] = 0;
}


void line32(uint16_t x1,uint16_t y1, uint16_t x2, uint16_t y2)
  {
  line_32(x1,y1,(x2-x1),(y2-y1));
  }

void position(uint16_t x,uint16_t y)
  {
  writeposx=x;
  writepos=getadr32(x,y);
  }

void rel_position_x(uint16_t x)
  {
  writeposx+=x;
  writepos+=x;
  }


void outtext(const char *text) {
	int pos;

	if (fontdsize) {
		while (*text) {
			char2_32(writepos, curfont, *text);
			pos = (charsize(curfont, *text) & 0xff) << 1;
			writepos += pos;
			writeposx += pos;
			text++;
		}
	} else {
		while (*text) {
			char_32(writepos, curfont, *text);
			pos = charsize(curfont, *text) & 0xff;
			writepos += pos;
			writeposx += pos;
			text++;
		}
	}
}

/*MODEinfo vesadata[3];
SVGAinfo svgadata[3];
int lastbank=0;
int granuality=0;
int gran_mask=0;
uint16_t gr_page_end=0;
int gr_end_screen=0;

uint16_t *mapvesaadr(uint16_t *a);
#pragma aux mapvesaadr parm [edi] value [edi]

void write_vesa_info(int mode)
  {
  char c[20];

  getsvgainfo(&svgadata);
  printf("VIDEO mem   %5dKb\n"
         "Oem:        %s\n\n",
         svgadata[0].memory*64,
         svgadata[0].oemstr);
  getmodeinfo(&vesadata,mode);
  if (vesadata[0].modeattr & MA_SUPP)
     {
     if (vesadata[0].modeattr & MA_LINEARFBUF) sprintf(c,"%8Xh",(long)vesadata[0].linearbuffer); else strcpy(c,"None");
    printf("Mode:        %04X \n"
           "WinA           %02X\n"
           "WinB           %02X\n"
           "Granuality: %5dKB\n"
           "WinSize:    %5dKB\n"
           "Xres:       %5d\n"
           "Yres:       %5d\n"
           "Bppix:      %5d\n"
           "Lbuffer:    %s\n\n",
           mode,
           vesadata[0].winaattr,
           vesadata[0].winbattr,
           vesadata[0].wingran,
           vesadata[0].winsize,
           vesadata[0].xres,
           vesadata[0].yres,
           vesadata[0].bppix,
           c);
    }
  else printf("Mode %04X not currently supported!!!\n\n");
//  printf("--- Hit ENTER if values are correct or press CTRL+Break ---\n");
//  getche();
  delay(300);
  }
*/
static void showview_dx(uint16_t x,uint16_t y,uint16_t xs,uint16_t ys);
//void showview64b(uint16_t x,uint16_t y,uint16_t xs,uint16_t ys);
/*void showview32b(uint16_t x,uint16_t y,uint16_t xs,uint16_t ys)
  {
  register longint a,b;

  if (x>640 || y>480) return;
  if (xs==0) xs=640;
  if (ys==0) ys=480;
  xs++;ys++;
  x&=~3;
  xs=(xs & ~3)+4;
  if (x+xs>640) xs=640-x;
  if (y+ys>480) ys=480-y;
  if (xs>550 && ys>400)
     {
     redraw32bb(screen,NULL,NULL);
     return;
     }
  a=(x<<1)+linelen*y;
  b=y*1280+x*2;
  redrawbox32bb(xs,ys,(void *)((char *)screen+a),(void *)b);
  }
*/
/*
void set_scan_line(int newline);
#pragma aux set_scan_line=\
           "mov  eax,4f06h"\
           "xor  ebx,ebx"\
           "int 10h"\
   parm [ecx] modify [eax ebx];

int get_scan_line();
#pragma aux get_scan_line=\
           "mov  eax,4f06h"\
           "mov  bh,01"\
           "int  10h"\
   modify[eax ebx] value[ecx];

void *create_hixlat()
  {
  uint16_t *s;
  uint16_t i;

  s=NewArr(uint16_t,32768);
  for (i=0;i<32768;i++) s[i]=((i & ~0x1f)<<1) | (i & 0x1f);
  return (uint8_t *)s;
  }

int initmode64b(void *paletefile)
  {
  int i;

  getmodeinfo(&vesadata,0x111);
  if (!(vesadata[0].modeattr & MA_SUPP)) return -1;
  //write_vesa_info(0x110);
  if (vesadata[0].winaattr & (WA_SUPP | WA_WRITE)) write_window=0;
  else if (vesadata[0].winbattr & (WA_SUPP | WA_WRITE)) write_window=1;
  else return -1;
  i=vesadata[0].wingran*1024;
  granuality=0;lastbank=0;
  while (i>>=1) granuality++;
  gran_mask=(1<<granuality)-1;
  gr_end_screen=0xa0000+gran_mask+1;
  gr_page_end=gran_mask+1;
  setvesamode(0x111,-1);
  lbuffer=(uint16_t *)0xa0000;
  screen=lbuffer;
  linelen=640*2;
  showview=showview64b;
  screen=(void *)malloc(screen_buffer_size);
  banking=1;
  screenstate=1;
  xlatmem=paletefile;
  return 0;

  }

int initmode32b()
  {
  int i;
  getmodeinfo(&vesadata,0x110);
  if (!(vesadata[0].modeattr & MA_SUPP)) return -1;
  //write_vesa_info(0x110);
  if (vesadata[0].winaattr & (WA_SUPP | WA_WRITE)) write_window=0;
  else if (vesadata[0].winbattr & (WA_SUPP | WA_WRITE)) write_window=1;
  else return -1;
  i=vesadata[0].wingran*1024;
  granuality=0;lastbank=0;
  while (i>>=1) granuality++;
  gran_mask=(1<<granuality)-1;
  gr_end_screen=0xa0000+gran_mask+1;
  gr_page_end=gran_mask+1;
  setvesamode(0x110,-1);
  lbuffer=(uint16_t *)0xa0000;
  screen=lbuffer;
  linelen=640*2;
  showview=showview32b;
  screen=(void *)malloc(screen_buffer_size);
  banking=1;
  screenstate=1;
  return 0;
  }



int initmode32bb()
  {
  int i;
  getmodeinfo(&vesadata,0x110);
  if (!(vesadata[0].modeattr & MA_SUPP)) return -1;
  //write_vesa_info(0x110);
  if (vesadata[0].winaattr & (WA_SUPP | WA_WRITE)) write_window=0;
  else if (vesadata[0].winbattr & (WA_SUPP | WA_WRITE)) write_window=1;
  else return -1;
  i=vesadata[0].wingran*1024;
  granuality=0;lastbank=0;
  while (i>>=1) granuality++;
  setvesamode(0x110,-1);
  set_scan_line(1024);
  if (get_scan_line()!=1024 && !__skip_change_line_test)
     {
     text_mode();
     return -10;
     }
  lbuffer=(uint16_t *)0xa0000;
  screen=lbuffer;
  linelen=640*2;
  showview=showview32b;
  screen=(void *)malloc(screen_buffer_size);
  banking=1;
  screenstate=1;
  return 0;
  }



uint16_t *mapvesaadr1(uint16_t *a)
  {
  uint16_t bank;

  bank=(long)a>>16;
  if (bank!=lastbank)
     {
     lastbank=bank;
     bank=bank;
          {
           union REGS regs;
           regs.w.ax = 0x4f05;
           regs.w.bx = write_window;
           regs.w.dx = bank;
           int386 (0x10,&regs,&regs); // window A
          }
     }
 return (uint16_t *)(((long)a & 0xffff)+0xa0000);
}

void switchvesabank(uint16_t bank)
#pragma aux switchvesabank parm [eax]
  {
           union REGS regs;
           regs.w.ax = 0x4f05;
           regs.w.bx = 0;
           regs.w.dx = bank;
           int386 (0x10,&regs,&regs); // window A
  }

*/
int initmode_dx(char inwindow, char zoom, char monitor, int refresh)
  {
//  if (!DXInit64(inwindow,zoom,monitor,refresh)) return -1;
  if (!Screen_Init(inwindow,zoom,monitor,refresh)) return -1;
  showview=showview_dx;
  screenstate=1;
//  scr_linelen2=scr_linelen/2;
  return 0;
  }


  /*
int initmode256(void *paletefile)
  {
  MODEinfo data;

  getmodeinfo(&data,0x100+line480);
  if (!(data.modeattr & MA_SUPP)) return initmode256b(paletefile);
  if (!(data.modeattr & MA_LINEARFBUF)) return initmode256b(paletefile);
  //write_vesa_info(0x101);
  setvesamode(0x4101,-1);
  if (lbuffer==NULL)lbuffer=(uint16_t *)physicalalloc((long)data.linearbuffer,screen_buffer_size>>1);
  screen=lbuffer;
  linelen=640*2;
  palmem=(char *)paletefile;
  xlatmem=palmem+768;
  setpal((void *)palmem);
  showview=showview256;
  screen=(void *)malloc(screen_buffer_size);
  screenstate=1;
  banking=0;
  return 0;
  }


void showview256b(uint16_t x,uint16_t y,uint16_t xs,uint16_t ys)
  {
  register longint a,b;

  if (x>640 || y>480) return;
  if (xs==0) xs=640;
  if (ys==0) ys=480;
  xs++;ys++;
  x&=~3;
  xs=(xs & ~3)+4;
  y&=~1;
  ys=(ys & ~1)+2;
  if (x+xs>640) xs=640-x;
  if (y+ys>480) ys=480-y;
  if (xs>550 && ys>400)
     {
     redraw256b(screen,0,xlatmem);
     return;
     }
  a=(x<<1)+linelen*y;
  b=y*640+x;
  redrawbox256b(xs,ys,(void *)((char *)screen+a),(void *)b,xlatmem);
  }


int initmode256b(void *paletefile)
  {
  int i;
  getmodeinfo(&vesadata,0x100);
  if (!(vesadata[0].modeattr & MA_SUPP)) return -1;
  //write_vesa_info(0x101);
  i=vesadata[0].wingran*1024;
  if (vesadata[0].winaattr & (WA_SUPP | WA_WRITE)) write_window=0;
  else if (vesadata[0].winbattr & (WA_SUPP | WA_WRITE)) write_window=1;
  else return -1;
  granuality=0;lastbank=0;
  while (i>>=1) granuality++;
  gran_mask=(1<<granuality)-1;
  gr_end_screen=0xa0000+gran_mask+1;
  gr_page_end=gran_mask+1;
  setvesamode(0x101,-1);
  lbuffer=(uint16_t *)0xa0000;
  screen=lbuffer;
  palmem=(char *)paletefile;
  xlatmem=palmem+768;
  setpal((void *)palmem);
  linelen=640*2;
  showview=showview256b;
  screen=(void *)malloc(screen_buffer_size);
  banking=1;
  screenstate=1;
  return 0;
  }


void init_lo();
#pragma aux init_lo modify[eax ebx ecx edx esi edi]

int initmode_lo(void *paletefile)
  {
  init_lo();
  palmem=(char *)paletefile;
  xlatmem=palmem+768;
  setpal((void *)palmem);
  linelen=640*2;
  lbuffer=0;
  showview=showview_lo;
  screen=(void *)malloc(screen_buffer_size);
  screenstate=1;
  banking=1;
  return 0;
  }
*/

void closemode()
  {
  if (screenstate)
     {
     palmem=NULL;
     Sys_Shutdown();
//     DXCloseMode();
     }
  screenstate=0;

  }

static void showview_dx(uint16_t x,uint16_t y,uint16_t xs,uint16_t ys)
  {
//  register longint a;

  if (x>Screen_GetXSize() || y>Screen_GetYSize()) return;
  if (xs==0) xs=Screen_GetXSize();
  if (ys==0) ys=Screen_GetYSize();
  xs+=2;ys+=2;
  if (x+xs>Screen_GetXSize()) xs=Screen_GetXSize()-x;
  if (y+ys>Screen_GetYSize()) ys=Screen_GetYSize()-y;
  Screen_DrawRect(x,y,xs,ys);  
  }
/*
static void showview64b(uint16_t x,uint16_t y,uint16_t xs,uint16_t ys)
  {
  register longint a;

  if (x>640 || y>480) return;
  if (xs==0) xs=640;
  if (ys==0) ys=480;
  xs+=2;ys+=2;
  if (x+xs>640) xs=640-x;
  if (y+ys>480) ys=480-y;
  if (xs>550 && ys>400)
     {
     redraw64b(screen,NULL,xlatmem);
     return;
     }
  a=(x<<1)+linelen*y;
  redrawbox64b(xs,ys,(void *)((char *)screen+a),(void *)((char *)a),xlatmem);
  }


void showview256(uint16_t x,uint16_t y,uint16_t xs,uint16_t ys)
  {
  register longint a;

  if (xs==0) xs=640;
  if (ys==0) ys=480;
  x&=0xfffe;y&=0xfffe;xs+=2;ys+=2;
  if (x>640 || y>480) return;
  if (x+xs>640) xs=640-x;
  if (y+ys>480) ys=480-y;
  if (xs>550 && ys>400)
     {
     redraw256(screen,lbuffer,xlatmem);
     return;
     }
  a=(x<<1)+linelen*y;
  redrawbox256(xs,ys,(void *)((char *)screen+a),(void *)((char *)lbuffer+(a>>1)),xlatmem);
  }

void showview_lo(uint16_t x,uint16_t y,uint16_t xs,uint16_t ys)
  {
  register longint a,b;

  if (xs==0) xs=640;
  if (ys==0) ys=480;
  if (ys==0) ys=480;
  x&=0xfffe;y&=0xfffe;xs+=2;ys+=2;
  if (x+xs>640) xs=640-x;
  if (y+ys>480) ys=480-y;
  if (xs>550 && ys>400)
     {
     redraw_lo(screen,lbuffer,xlatmem);
     return;
     }
  a=(x<<1)+linelen*y;
  b=x+640*y;
  redrawbox_lo(xs,ys,(void *)((char *)screen+a),(void *)((char *)lbuffer+b),xlatmem);
  }



*/
void show_ms_cursor(int16_t x,int16_t y)
  {
  int16_t xs,ys;

  int mx =  Screen_GetXSize() - 1;
  int my =  Screen_GetYSize() - 1;

  if (x<0) x=0;
  if (x>mx) x=mx;
  if (y<0) y=0;
  if (y>my) y=my;
  xs=*(int16_t *)mscursor;
  ys=*((int16_t *)mscursor+1);
  get_picture(x,y,xs,ys,mssavebuffer);
  put_picture(x,y,mscursor);
  mscuroldx=x;
  mscuroldy=y;
  }

void hide_ms_cursor()
  {
  put_picture(mscuroldx,mscuroldy,mssavebuffer);
  }

void *register_ms_cursor(uint16_t *cursor)
  {
  int16_t xs,ys;

  mscursor=cursor;
  xs=*(int16_t *)mscursor;
  ys=*((int16_t *)mscursor+1);
  if (mssavebuffer!=NULL) free(mssavebuffer);
  mssavebuffer = (uint16_t*)malloc(xs*ys*2+10);//5 bajtu pro strejcka prihodu
  return mssavebuffer;
  }

void move_ms_cursor(int16_t newx,int16_t newy,char nodraw)
  {
  int16_t xs,ys;
  int mx =  Screen_GetXSize() - 1;
  int my =  Screen_GetYSize() - 1;
  static int16_t msshowx=0,msshowy=0;

  xs=*(int16_t *)mscursor;
  ys=*((int16_t *)mscursor+1);
  if (nodraw)
     {
     showview(msshowx,msshowy,xs,ys);
     msshowx=mscuroldx;
     msshowy=mscuroldy;
     return;
     }
  if (newx<0) newx=0;
  if (newy<0) newy=0;
  if (newx>mx) newx=mx;
  if (newy>my) newy=my;
  put_picture(mscuroldx,mscuroldy,mssavebuffer);
  show_ms_cursor(newx,newy);
  mscuroldx=newx;mscuroldy=newy;
  showview(msshowx,msshowy,mscuroldxs,mscuroldys);
  if (mscuroldx!=msshowx || mscuroldy!=msshowy)showview(mscuroldx,mscuroldy,mscuroldxs,mscuroldys);
  msshowx=newx;msshowy=newy;
  showview(msshowx,msshowy,xs,ys);
  mscuroldxs=xs;
  mscuroldys=ys;
  }

int text_height(const char *text)
  {
  char max=0,cur;

  while (*text)
     if ((cur=charsize(curfont,*text++)>>8)>max) max=cur;
  return max<<fontdsize;
  }

int text_width(const char *text)
  {
  int suma=0;

  while (*text)
     suma+=charsize(curfont,*text++) & 0xff;
  return suma<<fontdsize;
  }


void set_aligned_position(int x,int y,char alignx,char aligny,const char *text)
  {
  switch (alignx)
     {
     case 1:x-=text_width(text)>>1;break;
     case 2:x-=text_width(text);break;
     }
  switch (aligny)
     {
     case 1:y-=text_height(text)>>1;break;
     case 2:y-=text_height(text);break;
     }
  position(x,y);
  }

/*void pal_optimize()
  {
  long *stattable;
  uint16_t *c;
  char *d;
  int i;
  long maxr,maxg,maxb,max;
  int j;

  if (palmem==NULL) return;
  stattable=(long *)getmem(32768*sizeof(long));
  memset(stattable,0,32768*sizeof(long));
  c=screen;
  for(i=0;i<screen_buffer_size;i++,c++)
     stattable[*c & 0x7fff]++;
  for(j=0;j<256;j++)
     {
     max=0;
     for (i=0;i<32768;i++)
        if (stattable[i]>max)
           {
           *((uint16_t *)xlatmem+j)=i;
           max=stattable[i];
           }
     stattable[*((uint16_t *)xlatmem+j)]=-1;
     }
  d=palmem;
  c=(uint16_t *)xlatmem;
  for(i=0;i<256;i++)
     {
     j=*c++;
     *d++=((j>>9)& 0x3e);
     *d++=((j>>4)& 0x3e);
     *d++=(j & 0x1f)<<1;
     }
  setpal((void *)palmem);
  memset(xlatmem,0,65536);
  for(j=0;j<32768;j++)
     {
     int r1,g1,b1;
     int r2,g2,b2,dif;
     char *c;
     maxr=maxg=maxb=999999999;
     r1=(j>>9)& 0x3e;g1=(j>>4)& 0x3e;b1=(j & 0x1f)<<1;
     c=palmem;
        for(i=0;i<256;i++)
        {
        r2=abs(r1-*c++);
        g2=abs(g1-*c++);
        b2=abs(b1-*c++);
        dif=r2+b2+g2;
        if (dif<=maxb)
           {
           if (dif<maxb) xlatmem[j*2]=i;
           else xlatmem[j*2]=xlatmem[j*2+1];
           xlatmem[j*2+1]=i;
           maxb=dif;
           }
        }
     }
  showview(0,0,0,0);
  free(stattable);
  }
*/
void rectangle(int x1,int y1,int x2,int y2,int color)
  {
  curcolor=color;
  hor_line(x1,y1,x2);
  hor_line(x1,y2,x2);
  ver_line(x1,y1,y2);
  ver_line(x2,y1,y2);
  }

void *create_special_palette()
  {
  char *c;
  int i,j,k;
  void *z;

  z = c = (char*)getmem(3*256+2*32768);
  for(i=0;i<6;i++)
     for(j=0;j<7;j++)
        for(k=0;k<6;k++)
     {*c++=12*i;*c++=10*j;*c++=12*k;}
  c = (char*)z;
  c+=768;
  for(i=0;i<32;i++)
     for(j=0;j<32;j++)
        for(k=0;k<32;k++)
           {
           *c++=((i+3)/6)*42+((j+3)/5)*6+((k+3)/6);
           *c++=(i*2/12)*42+(j/5)*6+(k*2/12);
           }
  return z;
  }
void *create_special_palette2()
  {
  char *c;
  int i,j,k;
  void *z;

  z = c = (char*)getmem(3*256+2*32768);
  for(i=0;i<64;i++)
     {
     *c++=0;*c++=i;*c++=0;
     }
  for(j=0;j<24;j++)
     for(k=0;k<8;k++)
     {*c++=j*64/24;*c++=0;*c++=k*8;}
  c = (char*)z;
  c+=768;
  for(i=0;i<32;i++)
     for(j=0;j<32;j++)
        for(k=0;k<32;k++)
           {
           *c++=64+(i*24/32)*8+k/4;
           *c++=j*2;
           }
  return z;
  }


void *create_blw_palette16()
  {
  char *c;
  int i,j,k;
  void *z;
  char pal_colors[]={0,1,2,3,4,5,20,7,56,57,58,59,60,61,62,63};
  char carnat[]={0,1,3,2,4,5,7,6,12,13,15,14,8,9,11,10};

  z = c = (char*)getmem(3*256+2*32768);
  for(i=0;i<16;i++)
     {
     j=pal_colors[carnat[i]]*3;k=i*4+3;
     c[j]=k;c[j+1]=k;c[j+2]=k;
     }
  c+=768;
  for(i=0;i<32;i++)
     for(j=0;j<32;j++)
        for(k=0;k<32;k++)
           {
           int u=(i*3+j*5+k*2)/10;
           c[0]=carnat[u/2];
           c[1]=carnat[(u+1)/2];
           c+=2;
           }
  return z;
  }
/*
void showview16(uint16_t x,uint16_t y,uint16_t xs,uint16_t ys)
  {
  int x1,x2;
  if (x>640 || y>480) return;
  if (xs==0) xs=640;
  if (ys==0) ys=480;
  if (x+xs>640) xs=640-x;
  if (y+ys>480) ys=480-y;
  if (xs>550 && ys>400)
     {
     redraw16(screen,lbuffer,xlatmem);
     return;
     }
  x1=x & ~0x7;
  x2=(x+xs+7) & ~0x7;
  redrawbox16((x2-x1)/8,ys,screen+x1+640*y,(char *)lbuffer+x1/8+80*y,xlatmem);
  }

void init16colors();
#pragma aux init16colors modify [eax]=\
  "mov  eax,12h"\
  "int  10h"\

int initmode16(void *palette)
  {
  palette;
  init16colors();
  lbuffer=(uint16_t *)0xa0000;
  screen=lbuffer;
  linelen=640*2;
  showview=showview16;
  screen=(void *)malloc(screen_buffer_size);
  palmem=(char *)palette;
  xlatmem=palmem+768;
  setpal((void *)palmem);
  banking=0;
  screenstate=1;
  return 0;
  }

void empty_show_view(int x,int y,int xs,int ys)
  {
  x,y,xs,ys;
  }


int init_empty_mode()
  {
  screen=(void *)malloc(screen_buffer_size);
  showview=empty_show_view;
  banking=1;
  lbuffer=NULL;
  screenstate=1;
  return 0;
  }
*/