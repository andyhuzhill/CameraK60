/*
 * main implementation: use this 'C' sample to create your own application
 *
 */

#include "derivative.h" /* include peripheral declarations */


uint8 img_bin_buff[CAMERA_SIZE];

int 
main(void)
{	
    printf("hello world!\n");
    
    Ov7725_Init(img_bin_buff);
    LCD_Init(RED);
	while(1)
	{
	    ov7725_get_img();
	    
	}
}
