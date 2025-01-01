/******************************************************************************
* File Name: main.c
*
* Version: 1.00
*
* Description: This example demonstrates the use of the PSoC 4 watchdog timer
*              (WDT) to do periodic interrupts and WDT reset. On each interrupt,
*              the current count of one of the WDT timers is printed to indicate
*              when the WDT will intiate reset. The cause of reset is also printed
*              when main() starts after a system reset.
*
* Related Document: CE224703.pdf 
*
* Hardware Dependency: CY8CKIT-042 PSoC 4 PIONEER KIT
*
******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation.
******************************************************************************
* This software, including source code, documentation and related materials
* ("Software") is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and 
* foreign), United States copyright laws and international treaty provisions. 
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the 
* Cypress source code and derivative works for the sole purpose of creating 
* custom software in support of licensee product, such licensee product to be
* used only in conjunction with Cypress's integrated circuit as specified in the
* applicable agreement. Any reproduction, modification, translation, compilation,
* or representation of this Software except as specified above is prohibited 
* without the express written permission of Cypress.
* 
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes to the Software without notice. 
* Cypress does not assume any liability arising out of the application or use
* of Software or any product or circuit described in the Software. Cypress does
* not authorize its products for use as critical components in any products 
* where a malfunction or failure may reasonably be expected to result in 
* significant injury or death ("ACTIVE Risk Product"). By including Cypress's 
* product in a ACTIVE Risk Product, the manufacturer of such system or application
* assumes all risk of such use and in doing so indemnifies Cypress against all
* liability. Use of this Software may be limited by and subject to the applicable
* Cypress software license agreement.
*****************************************************************************/

#include <project.h>
#include <stdio.h>

/*******************************************************************************
*            Constants
*******************************************************************************/
#define interruptNumWDT 9

/*******************************************************************************
*            Global variables
*******************************************************************************/
char snum[32]; // Used for sprintf conversion
uint8 currentStatus;

/***************************************************************************//**
* Function Name: ISR_WatchDog
********************************************************************************
*
* Summary:
* This function sets a flag to the background process, and clears the interrupt
* source so that the interrupt can occur again.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
CY_ISR(ISR_WatchDog)
{
    /* Sets flag for printing the current count status of the Watchdog Timer */
    currentStatus = 1u;
    
    /* Clear interrupts state and pending interrupts */
    CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
    CyIntClearPending(interruptNumWDT);
}

/*******************************************************************************
* Function Name: main
********************************************************************************
*
*  The main function performs the following actions:
*   1. Displays the cause/source of reset: either WDT or power up / reset button.
*   2. Interrupt number and handler are initialized.
*   3. WDT counter 0 and 1 are set up to generate interrupts and resets properly.
*   4. Both WDT counters are enabled.
*   5. Count is kept track through the interrupt and displayed through UART.
*
*******************************************************************************/
int main(void)
{
    /* Cause of reset flag */
    uint8 resetSource = *(reg32 *)CYREG_RES_CAUSE;

    /* Start UART operation */
    UART_Start();
    
    /* On reset, the source is printed */
    UART_UartPutString("\r\nCause of reset: ");
    
    if (0u == resetSource) /* Source is initial power or reset */
    {
        UART_UartPutString("Power up or Reset button pressed\r\n");
    }
    else /* Source is Watchdog Timer */
    {   
        UART_UartPutString("Watchdog Timer");
    }
    
    /* Sets interrupt handler for WDT and enables interrupt number */
    CyIntSetVector(interruptNumWDT, ISR_WatchDog);
    CyIntEnable(interruptNumWDT);
    
    /* Enabling global interrupts */
    CyGlobalIntEnable;
    
    /* Set the first WDT counter to generate interrupt on match */
    CySysWdtWriteMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_INT);
    CySysWdtWriteMatch(CY_SYS_WDT_COUNTER0, 0x5000);
    
    /* Counter 0 is cleared when there is match */
    CySysWdtWriteClearOnMatch(CY_SYS_WDT_COUNTER0, 1u);
    
    /* Enable WDT conters 0 and 1 cascade */
    CySysWdtWriteCascade(CY_SYS_WDT_CASCADE_01);
    
    /* Set the second WDT counter to generate reset on match */
    CySysWdtWriteMode(CY_SYS_WDT_COUNTER1, CY_SYS_WDT_MODE_RESET);
    CySysWdtWriteMatch(CY_SYS_WDT_COUNTER1, 0x0009);
    
     /* Counter 1 is cleared when there is match */
    CySysWdtWriteClearOnMatch(CY_SYS_WDT_COUNTER1, 1u);
    
    /* Counter 0 and Counter 1 are enabled */
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK | CY_SYS_WDT_COUNTER1_MASK);
    
    UART_UartPutString("\r\nTimer 1 Count: ");
    
    /* Initialize current status flag */
    currentStatus = 0u;
    
    for(;;)
    {
        /* Print current count status of the Watchdog Timer if flag is set to 1 */
        if(currentStatus != 0)
        {
            sprintf(snum, "%lu" ,CySysWdtReadCount(1u));
            UART_UartPutString(snum);
            currentStatus = 0u;
        }
    }
}

/* [] END OF FILE */
