#include "main.h"

static uint32_t AIAS_systick;

static void AIAS_systick_isr(void);

int main(void)
{
    CyGlobalIntEnable;
    CySysTickStart();
    
    char* zing_status;
    char** status_values;
    AIAS_BLE_FRAME ble_frame;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
         
    ZING_init();
    DBLE_init();
    Debug_init();
    
    zing_status = (char*)malloc(sizeof(char) * MAX_ZING_STATUS_LENGTH);
    status_values = (char**)malloc(sizeof(char*) * NUM_ZCD_STATUS);
    for (uint8_t i = 0; i < NUM_ZCD_STATUS; i++)
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
        
    while (1)
    {
        CyBle_ProcessEvents();
        
        memset(zing_status, 0, MAX_ZING_STATUS_LENGTH);
        if (ZING_get_status(zing_status) == 1)
        {
            ble_frame = DBLE_set_frame(zing_status, status_values);
            
            notification.attrHandle = 0x0001;
            notification.value.val = (uint8_t*)&ble_frame;
            notification.value.len = sizeof(AIAS_BLE_FRAME);
            CyBle_GattsNotification(cyBle_connHandle, &notification);
        }
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