#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

//====== Local variables ======//
#define SCREENSIZE	240 * 320
#define FILESIZE	(SCREENSIZE) * sizeof(short) //Size in bytes of the frame buffer

int lcdDriver;
unsigned short* frameBuffer;
struct fb_copyarea shape;

//====== Local functions ======//
void initScreen();
void pushToScreen(unsigned short val, int x, int y);
void cleanScreen();

int main(int argc, char *argv[])
{
    initScreen();
    cleanScreen();
    pushToScreen("ZZ", 5, 5);
    printf("Test passed!\n");
 	exit(EXIT_SUCCESS);
}


//====== LCD screen functions section ======//

void initScreen()
{
	lcdDriver = open("/dev/fb0", 2);
	frameBuffer = (unsigned short*) mmap(NULL, FILESIZE, PROT_WRITE | PROT_READ, MAP_SHARED, lcdDriver, 0);
	
	// Default rectangle size
	shape.width = 16;
	shape.height = 16;
}

void pushToScreen(unsigned short val, int x, int y)
{
	int a;
	int b;
	
	for (a = y * 16; a < (y*16 + 16); a++) {
		for (b = x * 16; b < (x*16 + 16); b++) {
			frameBuffer[a * 320 + b] = val;
		}
	}

	shape.dx = x * 16;
	shape.dy = y * 16;
	
	ioctl(lcdDriver, 0x4680, &shape);
}

void cleanScreen()
{
	shape.dx = 0;
	shape.dy = 0;
	shape.width = 320;
	shape.height = 240;
	
	int i;
	for (i = 0; i < SCREENSIZE; i++) {
		frameBuffer[i] = 0x0000;
	}
	
	ioctl(lcdDriver, 0x4680, &shape);
	shape.width = 16;
	shape.height = 16;
}
