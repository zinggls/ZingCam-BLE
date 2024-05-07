#include "main.h"

static uint32_t AIAS_systick;

static void AIAS_systick_isr(void);

int main(void)
{
    CyGlobalIntEnable;
    CySysTickStart();
    
    char* imu;
    char** imu_values;
    char* zing_status;
    char** status_values;
    AIAS_BLE_FRAME ble_frame;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
         
    IMU_init();
    ZING_init();
    HBLE_init();
    Debug_init();
    
    imu = (char*)malloc(sizeof(char) * MAX_IMU_RECV_LENGTH);
    imu_values = (char**)malloc(sizeof(char*) * NUM_IMU);
    for (uint8_t i = 0; i < NUM_IMU; i++)
    {
        imu_values[i] = (char*)malloc(sizeof(char) * MAX_IMU_VALUE_LENGTH);
    }
    
    zing_status = (char*)malloc(sizeof(char) * MAX_ZING_STATUS_LENGTH);
    status_values = (char**)malloc(sizeof(char*) * NUM_ZCH_STATUS);
    for (uint8_t i = 0; i < NUM_ZCH_STATUS; i++)
    {
        status_values[i] = (char*)malloc(sizeof(char) * MAX_STATUS_VALUE_LENGTH);
    }
    
    AIAS_systick = 0;
    
    for (uint8_t i = 0; i < CY_SYS_SYST_NUM_OF_CALLBACKS; ++i)
    {
        if (CySysTickGetCallback(i) == NULL)
        {
            CySysTickSetCallback(i, AIAS_systick_isr);
            break;
        }
    }
        
    UART_IMU_UartPutString("<soc1>");
    
    while (1)
    {
        /*
        CyBle_ProcessEvents();
        
        if (cyBle_state == CYBLE_STATE_CONNECTED)
        {
            if (CyBle_GattGetBusStatus() == CYBLE_STACK_STATE_FREE)
            {
                memset(zing_status, 0, MAX_ZING_STATUS_LENGTH);
                if (ZING_get_status(zing_status) == 1)
                {
                    ble_frame.zing_frame = HBLE_set_zing_frame(zing_status, status_values);
                }
                
                memset(imu, 0, MAX_IMU_RECV_LENGTH);
                if (IMU_recv(imu) == 1)
                {
                    ble_frame.imu_frame = HBLE_set_imu_frame(imu, imu_values, 0);
                } 
                notification.attrHandle = 0x0001;
                notification.value.val = (uint8_t*)&ble_frame;
                notification.value.len = sizeof(AIAS_BLE_FRAME);
                CyBle_GattsNotification(cyBle_connHandle, &notification);
                
                LED_RED_Write(!(LED_RED_Read()));
            }
        }
        */
        UART_IMU_UartPutChar('*');
        UART_DBG_UartPutChar(UART_IMU_UartGetChar());
    }
}

static void AIAS_systick_isr(void)
{
    AIAS_systick = AIAS_systick + 1;
}

int AIAS_get_systick(void)
{
    return AIAS_systick;
}