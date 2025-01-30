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

void TxRun_Test();
void SPDT_Test();
void CH_LED_Test();
void Batt_state_LED_Test();
void HGATE_Con1_Test();
void HGATE_Con2_Test();

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
        for(int i=0;i<3;i++)
        {
            SW_LED_LED_Write(~SW_LED_LED_Read());
            CyDelay(100);
        }
        
        TxRun_Test();
        SPDT_Test();
        CH_LED_Test();
        Batt_state_LED_Test();
        HGATE_Con1_Test();
        HGATE_Con2_Test();
    }
}

void TxRun_Test()
{
    TX_Run_Write(~TX_Run_Read());
    CyDelay(1000);
}

void SPDT_Test()
{
    SPDT_Write(~SPDT_Read());
    CyDelay(1000);
}

void CH_LED_Test()
{
    CH_LED_Write(0x1);
    CyDelay(1000);
    CH_LED_Write(0x2);
    CyDelay(1000);
    CH_LED_Write(0x3);
    CyDelay(1000);
}

void Batt_state_LED_Test()
{
    Batt_state_LED_Write(0x7);
    CyDelay(1000);
    Batt_state_LED_Write(0x6);
    CyDelay(1000);
    Batt_state_LED_Write(0x5);
    CyDelay(1000);
    Batt_state_LED_Write(0x4);
    CyDelay(1000);
    Batt_state_LED_Write(0x3);
    CyDelay(1000);
    Batt_state_LED_Write(0x2);
    CyDelay(1000);
    Batt_state_LED_Write(0x1);
    CyDelay(1000);
    Batt_state_LED_Write(0x0);
    CyDelay(1000);
}

void HGATE_Con1_Test()
{
    HGATE_Con1_Write(0x0f);
    CyDelay(1000);
    HGATE_Con1_Write(0x07);
    CyDelay(1000);
    HGATE_Con1_Write(0x03);
    CyDelay(1000);
    HGATE_Con1_Write(0x01);
    CyDelay(1000);
    HGATE_Con1_Write(0x00);
}

void HGATE_Con2_Test()
{
    HGATE_Con2_1_Write(0x03);
    HGATE_Con2_2_Write(0x03);
    CyDelay(1000);
    HGATE_Con2_1_Write(0x03);
    HGATE_Con2_2_Write(0x01);
    CyDelay(1000);
    HGATE_Con2_1_Write(0x03);
    HGATE_Con2_2_Write(0x00);
    CyDelay(1000);
    HGATE_Con2_1_Write(0x01);
    HGATE_Con2_2_Write(0x00);
    CyDelay(1000);
    HGATE_Con2_1_Write(0x00);
    HGATE_Con2_2_Write(0x00); 
}

/* [] END OF FILE */
