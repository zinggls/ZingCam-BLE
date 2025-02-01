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

#include "i2cs.h"
#include "project.h"
#include "icd.h"
#include "led.h"

static uint8 i2cReadBuffer [I2C_RD_BUFFER_SIZE] = {0};
static uint8 i2cWriteBuffer[I2C_WR_BUFFER_SIZE] = {0};
static uint32 writeReqCount = 0;

uint8 changeScope(uint8_t cam)
{
    uint8 result;

    /* Execute the received command */
    switch (cam)
    {
        case 0x00:          //0x00 :알림 없음/알림 반영 완료
            setRGB(LED_OFF,LED_OFF,LED_OFF);
            result = 0x00;
            break;

        case 0x01:          //0x01 : 영상융합처리기 -> 조준경 EO 변경 요청 알림
            setRGB(LED_ON,LED_OFF,LED_OFF);
            result = 0x01;
            break;

        case 0x02:          //0x02 : 영상융합처리기 -> 조준경 IR 백상 변경 요청 알
            setRGB(LED_OFF,LED_ON,LED_OFF);
            result = 0X02;
            break;

        case 0x03:          //0x03 : 영상융합처리기 -> 조준경 IR 흑상 변경 요청 알림
            setRGB(LED_OFF,LED_OFF,LED_ON);
            result = 0x03;
            break;

        default:
            result = 0x00;
            break;
    }

    return result;
}

void i2cs_start(void)
{
    I2C_I2CSlaveInitReadBuf (i2cReadBuffer,  I2C_RD_BUFFER_SIZE);
    I2C_I2CSlaveInitWriteBuf(i2cWriteBuffer, I2C_WR_BUFFER_SIZE);
    I2C_Start();
}

void i2c_command_to_bps() {
    // Use the characteristic index to obtain the attribute handle    
    CYBLE_GATT_DB_ATTR_HANDLE_T attrHandle = cyBle_customCServ[CYBLE_CUSTOMC_CUSTOM_SERVICE_SERVICE_INDEX]
                                                .customServChar[CYBLE_CUSTOMC_CUSTOM_SERVICE_ZXX_CHAR_INDEX]
                                                .customServCharHandle;

    CYBLE_GATTC_WRITE_REQ_T writeReq;
    writeReq.attrHandle = attrHandle;
    writeReq.value.val = (uint8_t*)&ivfCom;
    writeReq.value.len = sizeof(IvfCom);

    if(CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeReq)==CYBLE_ERROR_OK) writeReqCount++;
}

void i2cs_process(ZCD_FRAME *zcd)
{
    /* Write complete: parse the command packet */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_CMPLT))
    {
        /* Check the packet length */
        if (PACKET_SIZE == I2C_I2CSlaveGetWriteBufSize())
        {
            ivfCom.scopeCamera = changeScope(i2cWriteBuffer[0]);
            ivfCom.scopeOutput = i2cWriteBuffer[1];
            ivfCom.wirelessVideoChannelMode = i2cWriteBuffer[2];
            ivfCom.wirelessVideoChannelInformation = i2cWriteBuffer[3];
            ivfCom.scopeOperationMode = i2cWriteBuffer[4];
            ivfCom.wirelessVideoTransmitterOperationModeStatus = i2cWriteBuffer[5];
            ivfCom.wirelessVideoReceiverOperationModeStatus = i2cWriteBuffer[6];
            ivfCom.wirelessVideoTransmitterImuOutputType = i2cWriteBuffer[7];
            ivfCom.wirelessVideoTransmitterImuCalibrate = i2cWriteBuffer[8];
            ivfCom.wirelssVideoReceiverImuOutputType = i2cWriteBuffer[9];
            ivfCom.wirelessVideoReceiverImuCalibrate = i2cWriteBuffer[10];
            
            i2c_command_to_bps();
        }
        
        /* Clear the slave write buffer and status */
        I2C_I2CSlaveClearWriteBuf();
        (void) I2C_I2CSlaveClearWriteStatus();
        
        /* Update the read buffer */
        setReadBuffer_ScopeCamera(&i2cReadBuffer[0],ivfCom.scopeCamera);
        setReadBuffer_ScopeOutput(&i2cReadBuffer[1],ivfCom.scopeOutput);
        i2cReadBuffer[2] = ivfCom.wirelessVideoChannelMode;
        i2cReadBuffer[3] = ivfCom.wirelessVideoChannelInformation;
        i2cReadBuffer[4] = ivfCom.scopeOperationMode;
        i2cReadBuffer[5] = ivfCom.wirelessVideoTransmitterOperationModeStatus;
        i2cReadBuffer[6] = ivfCom.wirelessVideoReceiverOperationModeStatus;
        i2cReadBuffer[7] = ivfCom.wirelessVideoTransmitterImuOutputType;
        i2cReadBuffer[8] = ivfCom.wirelessVideoTransmitterImuCalibrate;
        i2cReadBuffer[9] = ivfCom.wirelssVideoReceiverImuOutputType;
        i2cReadBuffer[10] = ivfCom.wirelessVideoReceiverImuCalibrate;
    }
    
    setZcdBuffer(i2cReadBuffer+ZING_ZCD_OFFSET,zcd);    //ZCD offset ICD(47) + ZXX(53) = 100
    
    /* Read complete: expose buffer to master */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_CMPLT))
    {
        //i2cReadBuffer를 모두 초기화하면 i2c로 읽어가는 모든 값들이 0으로 초기화된다
        //값이 전달되지 않는 것을 표시하는 것은 타이머를 사용하여 구현하도록 변경할 계획이므로 메모리 초기화 코드는 삭제
        
        /* Clear the slave read buffer and status */
        I2C_I2CSlaveClearReadBuf();
        (void) I2C_I2CSlaveClearReadStatus();
    }
}

uint8_t* getI2CReadBuffer()
{
    return i2cReadBuffer;
}

/* [] END OF FILE */
