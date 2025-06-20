#ifndef ICD_H
#define ICD_H

#define ICD_SIZE                            22
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
#define ICD_SCOPE_BATTERY_OFFSET            13
#define ICD_TX_BATTERY_OFFSET               14
#define ICD_SCOPE_IR_STATE_OFFSET           15
#define ICD_SCOPE_EO_STATE_OFFSET           16
#define ICD_TX_MODEM_STATE_OFFSET           17
#define ICD_RX_MODEM_STATE_OFFSET           18
#define ICD_TX_IMU_STATE_OFFSET             19
#define ICD_RX_IMU_STATE_OFFSET             20
#define ICD_BLE_OFFSET                      21
#define IMU_TX_SIZE                         12
#define IMU_TX_OFFSET                       22
#define IMU_RX_SIZE                         12
#define IMU_RX_OFFSET                       34
#define ICD_IVF_SIZE                        46                  //ICD_SIZE + IMU_TX_SIZE + IMU_RX_SIZE = 22 + 12 + 12 = 46
    
#define ZING_ZXX_SIZE                       73                  //sizeof(ZXX_FRAME)
#define ZING_ZCD_SIZE                       66                  //sizeof(ZCD_FRAME)
#define ZING_ZXX_OFFSET                     ICD_IVF_SIZE
#define ZING_ZCD_OFFSET                     119                 //ZING_ZXX_OFFSET + ZING_ZXX_SIZE = 46 + 73 = 119

#define I2C_IVF_READ_BUFFER_SIZE            185                 //ICD_IVF_SIZE + ZING_ZXX_SIZE + ZING_ZCD_SIZE = 46 + 73 + 66 = 185
    
#endif //ICD_H