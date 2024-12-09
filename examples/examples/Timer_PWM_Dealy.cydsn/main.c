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

#define BRIGHTNESS_CHANGE         (200u)

/******************************************************************************
* Function Name: Isr_TC_Handler
*******************************************************************************
*
* Summary:
*  Handles the Interrupt Service Routine for the PWM Component.
*
******************************************************************************/
CY_ISR(Isr_TC_Handler)
{   
	/* Clear the TC Interrupt */
   	PWM_ClearInterrupt(PWM_INTR_MASK_TC);
    
    LED_Green_Write(~LED_Green_Read());
    CyDelay(1000);
}


/******************************************************************************
* Function Name: main
*******************************************************************************
*
* Summary:
*  This function:
*  1. Sets up and enables the PWM interrupt.
*  2. Starts the PWM Component.
*
******************************************************************************/
int main()
{   
    /* Enable the global interrupt */
    CyGlobalIntEnable;
    
    /* Enable the Interrupt component connected to the PWM interrupt output */
    Isr_TC_StartEx(Isr_TC_Handler);
    
	/* Start the PWM Component */
    PWM_Start();
    
    for(;;)
    {
        
    }
}

/* [] END OF FILE */
