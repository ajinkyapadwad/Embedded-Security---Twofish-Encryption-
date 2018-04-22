/*
 * File  : Main C file for Twofish encryption application
 * Author: Ajinkya Padwad
 * Date  : April 04th 2018
 */

#include <xparameters.h> // contains all the device address declarations
#include "platform.h"    // declarations for the ARM Cortex M9 platform
#include "xgpio.h"		 // declarations for functions on GPIO ports
#include <stdio.h>       // for printf
#include <stdlib.h>		 // for exit()
#define switchPin XPAR_AXI_GPIO_1_DEVICE_ID			// address for switch
#define twofishIP XPAR_TWOFISHIP_0_DEVICE_ID		// device address for the twofish IP
#define twofish_baseaddr XPAR_TWOFISHIP_0_S00_AXI_BASEADDR	// values address from the IP

#define input 0xF	// pin mode input
#define output 0x0  // pin mode output
#define BUFFER_SIZE 129

int main()
{
	init_platform();

	// enable switch : optionally set from the board.
    XGpio enableSwitch;									// create XGPIO instance for the switches
    XGpio_Initialize(&enableSwitch, switchPin);			// initialise the port - mapping of pin
    XGpio_SetDataDirection(&enableSwitch, 1, input);	// set the direction of the pin
    u32 switchStatus = 0;								// set initial switch status to be OFF
    u32 writeEnable = 1;								// value to be written to enable the encryption

    switchStatus = XGpio_DiscreteRead(&enableSwitch, 1);// read the current status from the board switch
    writeEnable = switchStatus & 0x1;					// mask the value from switch to check if enable set

    Xil_Out32(twofish_baseaddr, writeEnable);			// write the enable command to the twofish IP

    u32 enableReady = 0;								// variable to store value from the IP
    enableReady = Xil_In32(twofish_baseaddr+36);		// check if the IP is ready and enabled

    xil_printf("Enable is : %x\r\n", enableReady);		// print the current value of Enable

    // 128 bits of plaintext starting at  0x80000000 :
	unsigned int plaintext3 = 0x80000000;
	unsigned int plaintext2 = 0x00000000;
	unsigned int plaintext1 = 0x00000000;
	unsigned int plaintext0 = 0x00000000;

	// variables to store the 128 bits of encrypted cipher
    int cipher3 = 0;
    int cipher2 = 0;
    int cipher1 = 0;
    int cipher0 = 0;

    char write_buffer[BUFFER_SIZE+1] __attribute__ ((aligned(32)));

//    FILE *outFile = fopen("output.txt", "w");
//    if (outFile == NULL)
//    {
//        printf("Error opening file!\n");
////        exit(1);
//    }

    do
    {
		Xil_Out32(twofish_baseaddr + 4, plaintext3);
		Xil_Out32(twofish_baseaddr + 8, plaintext2);
		Xil_Out32(twofish_baseaddr + 12, plaintext1);
		Xil_Out32(twofish_baseaddr + 16, plaintext0);

		cipher3 = Xil_In32(twofish_baseaddr+40);
		cipher2 = Xil_In32(twofish_baseaddr+44);
		cipher1 = Xil_In32(twofish_baseaddr+48);
		cipher0 = Xil_In32(twofish_baseaddr+52);

    	sprintf(write_buffer, "PT=%08X%08X%08X%08X\nCT=%08X%08X%08X%08X\r\n", plaintext3,plaintext2,plaintext1,plaintext0,cipher3,cipher2,cipher1,cipher0);

    	printf("%s", write_buffer);

//    	fprintf(outFile, "%s", write_buffer);


		if(plaintext3 == 0x00000001)
		{
			plaintext3 = plaintext3 >> 1;
			plaintext2 = 0x10000000;
		}
		else
		{
			plaintext3	= plaintext3 >> 1;
		}

		if(plaintext2 == 0x00000001)
		{
			plaintext2 = plaintext2 >> 1;
			plaintext1 = 0x10000000;
		}
		else
		{
			plaintext2	= plaintext2 >> 1;
		}

		if(plaintext1 == 0x00000001)
		{
			plaintext1 = plaintext1 >> 1;
			plaintext0 = 0x10000000;
		}
		else
		{
			plaintext1	= plaintext1 >> 1;
		}

		plaintext0 = plaintext0 >> 1;

    }while (plaintext0 != 0x00000001);

//    fclose(outFile);

    cleanup_platform();
    return XST_SUCCESS;
}

