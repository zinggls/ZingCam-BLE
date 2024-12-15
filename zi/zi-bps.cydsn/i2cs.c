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
#include "Peripheral.h"

static uint8 i2cReadBuffer [I2C_RD_BUFFER_SIZE] = {0};
static uint8 i2cWriteBuffer[I2C_WR_BUFFER_SIZE] = {0};
static uint32 notifyCustom = 0;

void i2cs_start(void)
{
    I2C_I2CSlaveInitReadBuf (i2cReadBuffer,  I2C_RD_BUFFER_SIZE);
    I2C_I2CSlaveInitWriteBuf(i2cWriteBuffer, I2C_WR_BUFFER_SIZE);
    I2C_Start();
}

void i2c_command_to_bcc()
{
    CYBLE_GATTS_HANDLE_VALUE_NTF_T myDataHandle;
    myDataHandle.attrHandle = CYBLE_CUSTOM_SERVICE_ZXX_CHAR_HANDLE;
    myDataHandle.value.val = (uint8_t *)&peripheral;
    myDataHandle.value.len = sizeof(PERIPHERAL);
    CyBle_GattsWriteAttributeValue( &myDataHandle, 0, &cyBle_connHandle, 0 );
    if(CyBle_GattsNotification(cyBle_connHandle,&myDataHandle)==CYBLE_ERROR_OK) notifyCustom++;
}

void i2cs_process()
{
    /* Write complete: parse the command packet */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_CMPLT))
    {
        /* Check the packet length */
        if (PACKET_SIZE == I2C_I2CSlaveGetWriteBufSize())
        {
            peripheral.scope.scopeKindChangeNotify = i2cWriteBuffer[0];
            peripheral.scope.scopeOutChangeNotify = i2cWriteBuffer[1];
            peripheral.scope.scopeOperMode = i2cWriteBuffer[2];
            peripheral.scope.scopeStateKind = i2cWriteBuffer[3];
            peripheral.scope.scopeStateOut = i2cWriteBuffer[4];
            peripheral.scope.scopeStateBattery = i2cWriteBuffer[5];
            peripheral.scope.scopeStateIR = i2cWriteBuffer[6];
            peripheral.scope.scopeStateEO = i2cWriteBuffer[7];
            
            i2c_command_to_bcc();
        }
        
        /* Clear the slave write buffer and status */
        I2C_I2CSlaveClearWriteBuf();
        (void) I2C_I2CSlaveClearWriteStatus();
        
        /*  Update the read buffer */
        
        /*  
            화기조준경 영상 종류 변경 요청 알림

            화기조준경 -> 무선영상 송신기 화기조준경 영상 종류 변경 알림
            0x00 :알림 없음/알림 확인 완료
            0x04 : 조준경 -> 영상융합처리기 EO 변경 알림
            0x05 : 조준경 -> 영상융합처리기 IR 백상 변경 알림
            0x06 : 조준경 -> 영상융합처리기 IR 흑상 변경 알림
        
            무선영상 송신기 -> 화기조준경 화기조준경 영상 종류 변경 요청 알림
            0x00 :알림 없음/알림 반영 완료
            0x01 : 영상융합처리기 -> 조준경 EO 변경 요청 알림
            0x02 : 영상융합처리기 -> 조준경 IR 백상 변경 요청 알림
            0x03 : 영상융합처리기 -> 조준경 IR 흑상 변경 요청 알림
        */
        switch(peripheral.scope.scopeKindChangeNotify){
            case 0x0:
                i2cReadBuffer[0] = 0x0;     //0x00 :알림 없음/알림 반영 완료
                break;
            case 0x4:
                i2cReadBuffer[0] = 0x1;     //0x01 : 영상융합처리기 -> 조준경 EO 변경 요청 알림
                break;
            case 0x5:
                i2cReadBuffer[0] = 0x2;     //0x02 : 영상융합처리기 -> 조준경 IR 백상 변경 요청 알림
                break;
            case 0x6:
                i2cReadBuffer[0] = 0x3;     //0x03 : 영상융합처리기 -> 조준경 IR 흑상 변경 요청 알림
                break;
            default:
                //정의 안되어 있음
                break;
        }
        
        /*  
            화기조준경 영상 출력 종류 변경 요청 알림

            화기조준경 -> 무선영상 송신기 화기조준경 영상 출력 종류 변경 알림
            0x00 :알림 없음/알림 확인 완료
            0x03 : 조준경 -> 영상융합처리기 영상 출력 변경 알림
            0x04 : 조준경 -> 영상융합처리기 영상 미출력 변경 알림
        
            무선영상 송신기 -> 화기조준경 화기조준경 영상 출력 종류 변경 요청 알림
            0x00 :알림 없음/알림 반영 완료
            0x01 : 영상융합처리기 -> 조준경 영상 출력으로 변경 요청 알림
            0x02 : 영상융합처리기 -> 조준경 영상 미출력으로 변경 요청 알림
        */
        switch(peripheral.scope.scopeOutChangeNotify){
            case 0x0:
                i2cReadBuffer[1] = 0x0;     //0x00 :알림 없음/알림 반영 완료
                break;
            case 0x3:
                i2cReadBuffer[1] = 0x1;     //0x01 : 영상융합처리기 -> 조준경 영상 출력으로 변경 요청 알림
                break;
            case 0x4:
                i2cReadBuffer[1] = 0x2;     //0x02 : 영상융합처리기 -> 조준경 영상 미출력으로 변경 요청 알림
                break;
            default:
                //정의 안되어 있음
                break;
        }
        i2cReadBuffer[2] = peripheral.scope.scopeOperMode;
    }

    /* Read complete: expose buffer to master */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_CMPLT))
    {
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
