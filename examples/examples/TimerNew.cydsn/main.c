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

CY_ISR(Timer_ISR)
{
    // Clear the interrupt
    Timer_ClearInterrupt(Timer_INTR_MASK_TC);

    // Add your custom code here
    LED_RED_Write(!LED_RED_Read());
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    Timer_Start();
    Timer_ISR_StartEx(Timer_ISR);

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
