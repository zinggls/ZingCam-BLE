#ifndef ICD_H
#define ICD_H

#include <stdint.h>

#define ICD_SIZE                            23
#define ICD_SCOPE_VIDEO_KIND_NOTIFY_OFFSET   0
#define ICD_SCOPE_OUTPUT_NOTIFY_OFFSET       1
#define ICD_WIRELESS_CHANNEL_MODE_OFFSET     2
#define ICD_WIRELESS_CHANNEL_INFO_OFFSET     3
#define ICD_SCOPE_OPER_MODE_OFFSET           4
#define ICD_TX_OPER_MODE_OFFSET              5
#define ICD_RX_OPER_MODE_OFFSET              6
#define ICD_TX_IMU_TYPE_OFFSET               7
#define ICD_TX_IMU_CALIB_OFFSET              8
#define ICD_RX_IMU_TYPE_OFFSET               9
#define ICD_RX_IMU_CALIB_OFFSET             10
#define ICD_SCOPE_VIDEO_KIND_OFFSET         11
#define ICD_SCOPE_OUTPUT_OFFSET             12
#define ICD_SCOPE_DETECT_OFFSET             13
#define ICD_SCOPE_BATTERY_OFFSET            14
#define ICD_TX_BATTERY_OFFSET               15
#define ICD_SCOPE_IR_STATE_OFFSET           16
#define ICD_SCOPE_EO_STATE_OFFSET           17
#define ICD_TX_MODEM_STATE_OFFSET           18
#define ICD_RX_MODEM_STATE_OFFSET           19
#define ICD_TX_IMU_STATE_OFFSET             20
#define ICD_RX_IMU_STATE_OFFSET             21
#define ICD_BLE_OFFSET                      22
    
#define IMU_TX_SIZE                         12
#define IMU_TX_OFFSET                       23
#define IMU_RX_SIZE                         12
#define IMU_RX_OFFSET                       35
#define ICD_IVF_SIZE                        47                  //ICD_SIZE + IMU_TX_SIZE + IMU_RX_SIZE = 23 + 12 + 12 = 47
    
#define ZING_ZXX_SIZE                       53                  //sizeof(ZXX_FRAME)
#define ZING_ZCD_SIZE                       66                  //sizeof(ZCD_FRAME)
#define ZING_ZXX_OFFSET                     ICD_IVF_SIZE
#define ZING_ZCD_OFFSET                     100                 //ZING_ZXX_OFFSET + ZING_ZXX_SIZE = 47 + 53 = 100

#define I2C_IVF_READ_BUFFER_SIZE            166                 //ICD_IVF_SIZE + ZING_ZXX_SIZE + ZING_ZCD_SIZE = 47 + 53 + 66 = 166
    
typedef struct {
    uint8_t scopeCamera;
    uint8_t scopeOutput;
    uint8_t wirelessVideoChannelMode;
    uint8_t wirelessVideoChannelInformation;
    uint8_t scopeOperationMode;    
    uint8_t wirelessVideoTransmitterOperationModeStatus;
    uint8_t wirelessVideoReceiverOperationModeStatus;
    uint8_t wirelessVideoTransmitterImuOutputType;
    uint8_t wirelessVideoTransmitterImuCalibrate;
    uint8_t wirelssVideoReceiverImuOutputType;
    uint8_t wirelessVideoReceiverImuCalibrate;
} IvfCom;

IvfCom ivfCom;
    
void setReadBuffer_ScopeCamera(uint8_t* rdBuf,uint8_t sc);
void setReadBuffer_ScopeOutput(uint8_t* rdBuf,uint8_t so);

#endif //ICD_H