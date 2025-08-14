// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"S
#include "x86.h"





static void consputc(int);

static int panicked = 0;

static struct {
	struct spinlock lock;
	int locking;
} cons;

static void
printint(int xx, int base, int sign)
{
	static char digits[] = "0123456789abcdef";
	char buf[16];
	int i;
	uint x;

	if(sign && (sign = xx < 0))
		x = -xx;
	else
		x = xx;

	i = 0;
	do{
		buf[i++] = digits[x % base];
	}while((x /= base) != 0);

	if(sign)
		buf[i++] = '-';

	while(--i >= 0)
		consputc(buf[i]);
}

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
	int i, c, locking;
	uint *argp;
	char *s;

	locking = cons.locking;
	if(locking)
		acquire(&cons.lock);

	if (fmt == 0)
		panic("null fmt");

	argp = (uint*)(void*)(&fmt + 1);
	for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
		if(c != '%'){
			consputc(c);
			continue;
		}
		c = fmt[++i] & 0xff;
		if(c == 0)
			break;
		switch(c){
		case 'd':
			printint(*argp++, 10, 1);
			break;
		case 'x':
		case 'p':
			printint(*argp++, 16, 0);
			break;
		case 's':
			if((s = (char*)*argp++) == 0)
				s = "(null)";
			for(; *s; s++)
				consputc(*s);
			break;
		case '%':
			consputc('%');
			break;
		default:
			// Print unknown % sequence to draw attention.
			consputc('%');
			consputc(c);
			break;
		}
	}

	if(locking)
		release(&cons.lock);
}

void
panic(char *s)
{
	int i;
	uint pcs[10];

	cli();
	cons.locking = 0;
	// use lapiccpunum so that we can call panic from mycpu()
	cprintf("lapicid %d: panic: ", lapicid());
	cprintf(s);
	cprintf("\n");
	getcallerpcs(&s, pcs);
	for(i=0; i<10; i++)
		cprintf(" %p", pcs[i]);
	panicked = 1; // freeze other CPU
	for(;;)
		;
}

#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

static ushort screen[9][9]; //buffer za karaktere koji budu pokriveni menijem
static char boje[][255] = {":WHT BLK:", ":PUR WHT:", ":RED AQU:", ":WHT YEL:"};
static char crtice[9] = {"_________"};

static int menu_open = 0;
static int selected = 0;
static int flag = 0;
void printMenu(int pos){
	int globalPos = 0;
	if(pos%80 > 70)
		globalPos = pos - 10*80-9;
	else globalPos = pos - 10*80+1;
	int tmp = globalPos;
	int brojac = 0;
	int x = 0;
	int counter = -1;

	for(int i = 0; i<9; i++){
		globalPos +=80;
		tmp = globalPos;
		
		for(int j = 0; j<9; j++){
			if(menu_open == 0)
				screen[i][j] = crt[tmp];

			if(brojac == 0){
				crt[tmp++] = (crtice[j]&0xff) | 0x7000;	
			}
			else if(brojac != 0 && counter != selected)
				crt[tmp++] = boje[x][j] | 0x7000;

			else if(counter == selected){
				if(selected == 3){
					crt[tmp++] = boje[x][j] | 0x2000;
				}
				if(selected == 2){
					crt[tmp++] = boje[x][j] | 0x2000;
				}
				if(selected == 1){
					crt[tmp++] = boje[x][j] | 0x2000;
				}
				if(selected == 0){
					crt[tmp++] = boje[x][j] | 0x2000;
				}
			}
			
		}
		
		brojac +=1;
		brojac %= 2;
		if(brojac == 1) counter+=1;
		if(brojac == 0)
			x++;	// x predstavlja koji red stampam za boje samo, izmedju crtica	
			
	}
	menu_open = 1;

	
}

void printScreen(int pos){

	int globalPos = 0;
	if(pos%80 > 70)
		globalPos = pos - 10*80-9;
	else globalPos = pos - 10*80+1;

	int tmp = globalPos;
	for(int i = 0; i<9; i++){
		globalPos +=80;
		tmp = globalPos;
		for(int j = 0; j<9; j++){
			crt[tmp++] = screen[i][j];		
		}
	}
	menu_open = 0;

}

static void
cgaputc(int c) // ovde treba da stampamo meni 
{
	int pos;

	// Cursor position: col + 80*row.
	// ove dve adrese 0x0E i 0x0F zajedno cine poziciju kursora, koja se cuva u dva bajta
	outb(CRTPORT, 14); // 14 = 0x0E 
	pos = inb(CRTPORT+1) << 8;
	outb(CRTPORT, 15); // 15 = 0x0F
	pos |= inb(CRTPORT+1);

	if(c == '\n' && menu_open == 0)
		pos += 80 - pos%80;
	else if(c == BACKSPACE && menu_open == 0){
		if(pos > 0) --pos;
	}else if(c == 260){
		if(menu_open == 0)
			printMenu(pos);	
		else printScreen(pos);
	}else if(c == 'w' && menu_open == 1){
			selected--;
			if(selected < 0)
				selected = 3;
			printMenu(pos);
			
	}else if(c == 's' && menu_open == 1){
			selected++;
			if(selected > 3)
				selected = 0;
			printMenu(pos);
	}
	else if(menu_open == 0){
			if(selected == 0){
				crt[pos++] = (c&0xff) | 0x0700; // WHT BLK
			}
			if(selected == 1){
				crt[pos++] = (c&0xff) | 0x7500;	//PUR WHT		
			}	
			if(selected == 2){
				crt[pos++] = (c&0xff) | 0x3400;	// RED AQUA		
			}
			if(selected == 3){
				crt[pos++] = (c&0xff) | 0x6700;	// WHT YELLOW		
			}
	}
	if(pos < 0 || pos > 25*80)
		panic("pos under/overflow");

	if((pos/80) >= 24){  // Scroll up.
		memmove(crt, crt+80, sizeof(crt[0])*23*80);
		pos -= 80;
		memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
	}

	outb(CRTPORT, 14);
	outb(CRTPORT+1, pos>>8);
	outb(CRTPORT, 15);
	outb(CRTPORT+1, pos);
	crt[pos] = ' ' | 0x0700;
}

void
consputc(int c)
{
	if(panicked){
		cli();
		for(;;)
			;
	}
	if(c == BACKSPACE){
		uartputc('\b'); uartputc(' '); uartputc('\b');
	} else
		uartputc(c);
	cgaputc(c);
}

#define INPUT_BUF 128
struct {
	char buf[INPUT_BUF];
	uint r;  // Read index
	uint w;  // Write index
	uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void))
{
	int c, doprocdump = 0;

	acquire(&cons.lock);
	while((c = getc()) >= 0){
		switch(c){
		case C('P'):  // Process listing.
			// procdump() locks cons.lock indirectly; invoke later
			doprocdump = 1;
			break;
		case C('U'):  // Kill line.
			while(input.e != input.w &&
			      input.buf[(input.e-1) % INPUT_BUF] != '\n'){
				input.e--;
				consputc(BACKSPACE);
			}
			break;
		case C('H'): case '\x7f':  // Backspace
			if(input.e != input.w){
				input.e--;
				consputc(BACKSPACE);
			}
			break;
		case 260: 
			cgaputc(c);
			break;
		case 'w':
            if(menu_open){
                cgaputc(c);
                break;
            }
        case 's':
            if(menu_open){
                cgaputc(c);
                break;
            }
		default:
			if(menu_open == 1) break;
			if(c != 0 && input.e-input.r < INPUT_BUF){
				c = (c == '\r') ? '\n' : c;
				input.buf[input.e++ % INPUT_BUF] = c;				
					consputc(c);
				if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
					input.w = input.e;
					wakeup(&input.r);
				}
			}
			break;
		}
	}
	release(&cons.lock);
	if(doprocdump) {
		procdump();  // now call procdump() wo. cons.lock held
	}
}

int
consoleread(struct inode *ip, char *dst, int n)
{
	uint target;
	int c;

	iunlock(ip);
	target = n;
	acquire(&cons.lock);
	while(n > 0){
		while(input.r == input.w){
			if(myproc()->killed){
				release(&cons.lock);
				ilock(ip);
				return -1;
			}
			sleep(&input.r, &cons.lock);
		}
		c = input.buf[input.r++ % INPUT_BUF];
		if(c == C('D')){  // EOF
			if(n < target){
				// Save ^D for next time, to make sure
				// caller gets a 0-byte result.
				input.r--;
			}
			break;
		}
		*dst++ = c;
		--n;
		if(c == '\n')
			break;
	}
	release(&cons.lock);
	ilock(ip);

	return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
	int i;

	iunlock(ip);
	acquire(&cons.lock);
	for(i = 0; i < n; i++)
		consputc(buf[i] & 0xff);
	release(&cons.lock);
	ilock(ip);

	return n;
}

void
consoleinit(void)
{
	initlock(&cons.lock, "console");

	devsw[CONSOLE].write = consolewrite;
	devsw[CONSOLE].read = consoleread;
	cons.locking = 1;

	ioapicenable(IRQ_KBD, 0);
}

