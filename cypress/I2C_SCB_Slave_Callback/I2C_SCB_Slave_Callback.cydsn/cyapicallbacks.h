/******************************************************************************
* File Name: main.c
*
* Version 1.30
*
* Description: 
*  This code example demonstrates basic usage of PSoC 4 Serial Communication 
*  Block (SCB) Component in I2C Slave mode. This project implements the 
*  I2C Slave device, which receives and executes commands from I2C Master and 
*  controls the RGB LED color. Result of command execution can be read by 
*  I2C Master. 
*
* Related Document: 
*  CE195364_I2C_Slave_using_a_Serial_Communication_Block(SCB)with_PSoC4.pdf
*
* Hardware Dependency: 
*  See CE195364_I2C_Slave_using_a_Serial_Communication_Block(SCB)with_PSoC4.pdf
*
*******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress 
* reserves the right to make changes to the Software without notice. Cypress 
* does not assume any liability arising out of the application or use of the 
* Software or any product or circuit described in the Software. Cypress does 
* not authorize its products for use in any products where a malfunction or 
* failure of the Cypress product may reasonably be expected to result in 
* significant property damage, injury or death ("High Risk Product"). By 
* including Cypress’s product in a High Risk Product, the manufacturer of such 
* system or application assumes all risk of such use and in doing so agrees to 
* indemnify Cypress against all liability. 
*******************************************************************************/
#ifndef CYAPICALLBACKS_H
#define CYAPICALLBACKS_H
    
    /* Enable the EXIT_CALLBACK function in the generated *
     * source file I2CS_I2C_INT.c                         */
    #define I2CS_I2C_ISR_EXIT_CALLBACK
    void I2CS_I2C_ISR_ExitCallback(void);
    
#endif /* CYAPICALLBACKS_H */   
/* [] */
