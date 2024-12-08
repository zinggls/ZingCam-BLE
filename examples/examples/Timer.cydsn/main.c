/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

uint16 ms_count = 0;

CY_ISR(MY_ISR) {
	ms_count++;
	
	if(ms_count == 1000) { // 1 second
		LED_Write(!LED_Read()); // Toggle LED
		ms_count = 0; // reset ms counter
	}
}

int main(void)
{
    Timer_1_Start(); // Configure and enable timer
	isr_1_StartEx(MY_ISR); // Point to MY_ISR to carry out the interrupt sub-routine
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
