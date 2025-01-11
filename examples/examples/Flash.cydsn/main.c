/*******************************************************************************
* File Name: main.c
*
* Version: 1.00
*
* Description:
*  This is the source code for the Flash example project of the cy_boot
*  component.
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <project.h>
#include <CyFlash.h>

/* Defines second ROW from the last ROW */
#define CY_TEST_FLASH_ROW       (CY_FLASH_NUMBER_ROWS - 2u)

/* Defines absolut address of ROW */
#define CY_TEST_FLASH_ADDR      (CY_TEST_FLASH_ROW * CY_FLASH_SIZEOF_ROW)

#if(CY_PSOC4)
    #define LIGHT_OFF           (1u)
    #define LIGHT_ON            (0u)
#else
    #define LIGHT_OFF           (0u)
    #define LIGHT_ON            (1u)
    
    /* Defines ID of the array to write */
    #define ARRAY_ID            ((uint8)(CY_TEST_FLASH_ROW / CY_FLASH_SIZEOF_ROW))

    /* Defines second ROW from the last ROW in the defined array */
    #define ARRAY_ID_ROW_ADDR   ((uint16)(CY_FLASH_SIZEOF_ROW - 2u))
#endif /* (CY_PSOC4) */
    
uint8 rowPattern[CY_FLASH_SIZEOF_ROW];


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  At the beginning of the main function, the LEDs turns off, generates the
*  flash row data pattern, in the cycle erases a row of Flash and programs it
*  with the row data pattern, compares a row from flash with the row data
*  pattern (if the error during write to the Flash or compare data happens,
*  the LED_Error lights on and processor halts). If error occurs during data
*  comparison, the LED_Error blinks with approximately 1 second intervals to
*  shows error happens. If no error occurs during data comparison, the LED_Ok
*  blinks with approximately 1 second intervals.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
int main()
{
    uint16 i;
    uint8 result = 0u;
    cystatus returnValue = CYRET_SUCCESS;
    volatile uint8 source;
    volatile uint8 destination;

    /* LEDs Light off */
    LED_Error_Write(LIGHT_OFF);
    LED_Ok_Write(LIGHT_OFF);

    /* Generate flash row pattern */
    for(i = 0u; i < CY_FLASH_SIZEOF_ROW; i++)
    {
        rowPattern[i] = CY_FLASH_SIZEOF_ROW - i;
    }

    for(;;)
    {
        /* Erases a row of Flash and programs it with the rowPattern */
        #if(CY_PSOC4)
            returnValue = CySysFlashWriteRow(CY_TEST_FLASH_ROW, rowPattern);

            /* Check if operration is successful */
            if (returnValue != CY_SYS_FLASH_SUCCESS)
            {
                LED_Error_Write(LIGHT_ON);
                CyHalt(0x00u);
            }
        #else
            CySetTemp();
            returnValue = CyWriteRowData(ARRAY_ID, ARRAY_ID_ROW_ADDR, rowPattern);

            /* Check if operration is successful */
            if (returnValue != CYRET_SUCCESS)
            {
                LED_Error_Write(LIGHT_ON);
                CyHalt(0x00u);
            }
        #endif /* (CY_PSOC4) */

        /* Compare a row from flash with the rowPattern */
        for (i = 0u; i < CY_FLASH_SIZEOF_ROW; i++)
        {
            /* Define source value */
            #if(CY_PSOC3)
                source = CY_GET_XTND_REG8((void CYFAR *) (CYDEV_FLS_BASE + CY_TEST_FLASH_ADDR + i));
            #elif(CY_PSOC5)
                source = (*((uint8 *) (CYDEV_FLS_BASE + CY_TEST_FLASH_ADDR + i)));
            #else
                source = (*((uint8 *) (CY_TEST_FLASH_ADDR + i)));
            #endif /* (CY_PSOC3) */
            
            /* Define destination value */
            destination = rowPattern[i];

            /* Check if source and destination values are equal */
            if ( source != destination)
            {
                result++;
            }
        }

        /* Check if source and destination have not equal values */
        if (result != 0u)
        {
            returnValue = CYRET_BAD_DATA;

            for(i = result; i > 0u; i--)
            {
                LED_Error_Write(LIGHT_ON);
                CyDelay(1000u);
                LED_Error_Write(LIGHT_OFF);
                CyDelay(1000u);
            }
        }
        else
        {
            LED_Ok_Write(LIGHT_ON);
            CyDelay(1000u);
            LED_Ok_Write(LIGHT_OFF);
            CyDelay(1000u);
        }

        /* Check if compare operration is successful */
        if (returnValue != CYRET_SUCCESS)
        {
            LED_Error_Write(LIGHT_ON);
            CyHalt(0x00u);
        }
     }
}

/* [] END OF FILE */
