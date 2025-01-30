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

#define ON  0x0
#define OFF 0x1

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    TX_Run_Write(OFF);
    SPDT_Write(OFF);
    CH_LED_Write(0x0);

    for(;;)
    {
        /* Place your application code here. */
        TX_Run_Write(~TX_Run_Read());
        SPDT_Write(~SPDT_Read());
        CyDelay(1000);
        
        CH_LED_Write(0x1);
        CyDelay(1000);
        CH_LED_Write(0x2);
        CyDelay(1000);
        CH_LED_Write(0x3);
        CyDelay(1000);
    }
}

/* [] END OF FILE */
