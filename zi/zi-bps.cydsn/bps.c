#include "bps.h"
#include <Log.h>
#include <ZFrame.h>
#include "led.h"

static uint16 writereqCustom = 0;

uint16 getWritereqCustom()
{
    return writereqCustom;
}

void BleCallBack(uint32 event, void* eventParam)
{
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;

    switch(event)
    {
        /* if there is a disconnect or the stack just turned on from a reset then start the advertising and turn on the LED blinking */
        case CYBLE_EVT_STACK_ON:
        L("CYBLE_EVT_STACK_ON\r\n");
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            L("Advertising fast\r\n");
            setRGB(LED_OFF,LED_OFF,LED_OFF);
            break;
            
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            L("CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP\r\n");
            setRGB(LED_ON,LED_ON,LED_ON);
            break;
        
        /* when a connection is made, update the LED and Capsense states in the GATT database and stop blinking the LED */    
        case CYBLE_EVT_GATT_CONNECT_IND:
            L("CYBLE_EVT_GATT_CONNECT_IND\r\n");
            setRGB(LED_OFF,LED_ON,LED_ON);
		    break;
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            L("CYBLE_EVT_GAP_DEVICE_CONNECTED\r\n");
            setRGB(LED_OFF,LED_ON,LED_OFF);
            break;

        /* handle a write request */
        case CYBLE_EVT_GATTS_WRITE_REQ:
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            
            if (wrReqParam->handleValPair.attrHandle == CYBLE_CUSTOM_SERVICE_ZXX_CHAR_HANDLE) {
                if (wrReqParam->handleValPair.value.len == sizeof(ZXX_FRAME)) {
                    CyBle_GattsWriteRsp(cyBle_connHandle);
                
                    writereqCustom++;
                }
            }
			break;  
            
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            L("CYBLE_EVT_GATT_DISCONNECT_IND\r\n");
            setRGB(LED_ON,LED_OFF,LED_OFF);
            break;
            
        case CYBLE_EVT_STACK_BUSY_STATUS:
            break;
            
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            {
                // Respond to MTU exchange request from Central
                CYBLE_GATT_XCHG_MTU_PARAM_T mtuExchgParam = *(CYBLE_GATT_XCHG_MTU_PARAM_T*)eventParam;
                CYBLE_API_RESULT_T res = CyBle_GattsExchangeMtuRsp(cyBle_connHandle, mtuExchgParam.mtu);
                L("MTU Exchange Request Handled, MTU size: %d, CyBle_GattsExchangeMtuRsp=0x%x\r\n", mtuExchgParam.mtu,res);
            }
            break;
            
        case CYBLE_EVT_GAP_DATA_LENGTH_CHANGE:
            // Data length parameters changed
            {
                CYBLE_GAP_DATA_LENGTH_T* dataLength = (CYBLE_GAP_DATA_LENGTH_T*)eventParam;

                // Log the updated parameters
                L("Data Length Change Event:\n");
                L("  Max TX Octets: %d\n", dataLength->maxTxOctets);
                L("  Max RX Octets: %d\n", dataLength->maxRxOctets);
                L("  Max TX Time: %d us\n", dataLength->maxTxTime);
                L("  Max RX Time: %d us\n", dataLength->maxRxTime);
            }
            break;
            
        default:
            setRGB(LED_ON,LED_OFF,LED_ON);
            L("unhandled BLE event = 0x%lx\r\n",event);
            break;
    }
} 