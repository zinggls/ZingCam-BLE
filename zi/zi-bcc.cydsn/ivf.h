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

#ifndef IVF_H
#define IVF_H
    
#include <stdint.h>

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

#endif

/* [] END OF FILE */
