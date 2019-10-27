#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

#define FILESIZE 240 * 640 * sizeof(char) //Size in bytes of the frame buffer

void lcd_test1();	//Function to test first lcd writting method
void lcd_test2();	//Function to test second lcd writting method

int main(int argc, char *argv[])
{
    lcd_test2();
 	exit(EXIT_SUCCESS);
}

void lcd_test1() {
    char *x = "ZZZZZZZZZZZZZZZZZZZZZZZZZZZ"; 	//Line to draw on the lcd
    char fb_path[20] = "/dev/fb0";				//Path of the lcd driver
    int result; 								//Error checking variable
	
	//Open the file
	int fb = open(fb_path,O_WRONLY);
	
	//Move the place you want to draw
	lseek(fb,640,SEEK_CUR);
	
	//Write to the screen
	result = write(fb,&x,200);
	if (result < 0) {
		printf("Writing error!\n");
	}

	//Close the file
	close(fb);	
}

void lcd_test2() {
	int i;
    int j;
    int fd;							//File descriptor
    int result; 					//Error checking variable
	char fb_path[20] = "/dev/fb0";	//Path of the lcd driver
	struct fb_copyarea rect;		//Struct to refresh the lcd
	
	//Open the driver file
    fd = open(fb_path, O_RDWR | O_CREAT | O_TRUNC,(mode_t)0600);
    if (fd == -1) {
		perror("Error opening file for writing");
		exit(EXIT_FAILURE);
    }
    
    //Stretch the file size to the size of the (mmapped) array
    result = lseek(fd, FILESIZE-2, SEEK_SET);
    if (result == -1) {
		close(fd);
		perror("Error calling lseek() to 'stretch' the file");
		exit(EXIT_FAILURE);
    }
    
    //Write a value to the end of the file
    result = write(fd, "", 2);
    if (result != 2) {
		close(fd);
		perror("Error writing last byte of the file");
		exit(EXIT_FAILURE);
    }

    //Map the file to an array
    char *map = mmap(0, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) {
		close(fd);
		perror("Error mmapping the file");
		exit(EXIT_FAILURE);
    }
    
    /*Draw something in the array:
    	-x coordinates go from 0 to 638
    	-y coordinates go from 0 to 239
    	-Each pixel is two byes long*/
    //Draw a square in the array
    for (i = 0; i <=239; i++) {
    	for (j = 0; j <=500; j+=2) {
			map[i*640 + j] = *"ZA"; 
    	}
    }
 
 	/*Draw desired area into the lcd
 		-x coordinates go from 0 to 319
    	-y coordinates go from 0 to 239*/
    rect.dx=0;
	rect.dy=0;
    rect.width=319;
    rect.height=239;
    ioctl(fd,0x4680,&rect);

    //Unmap memory
    if (munmap(map, FILESIZE) == -1) {
		perror("Error un-mmapping the file");
    }
}

