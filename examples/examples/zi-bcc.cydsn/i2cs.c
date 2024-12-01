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
#include "ivf.h"

static uint8 i2cReadBuffer [I2C_RD_BUFFER_SIZE] = {0};
static uint8 i2cWriteBuffer[I2C_WR_BUFFER_SIZE] = {0};

uint8 changeScope(ScopeCamera cam)
{
    uint8 result;

    /* Execute the received command */
    switch (cam)
    {
        case change_none:
            LED_RED_Write  (LED_OFF);
            LED_GREEN_Write(LED_OFF);
            LED_BLUE_Write (LED_OFF);
            result = change_none;
            break;

        case change_eo:
            LED_RED_Write  (LED_ON);
            LED_GREEN_Write(LED_OFF);
            LED_BLUE_Write (LED_OFF);
            result = change_eo;
            break;

        case change_ir_white:
            LED_RED_Write  (LED_OFF);
            LED_GREEN_Write(LED_ON);
            LED_BLUE_Write (LED_OFF);
            result = change_ir_white;
            break;

        case change_ir_black:
            LED_RED_Write  (LED_OFF);
            LED_GREEN_Write(LED_OFF);
            LED_BLUE_Write (LED_ON);
            result = change_ir_black;
            break;

        default:
            result = change_none;
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
    
    setZcdBuffer(i2cReadBuffer+99,zcd);    //ZCD offset ICD(46) + ZCH(53) = 99
    
    /* Read complete: expose buffer to master */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_CMPLT))
    {
        /* Clear the slave read buffer and status */
        I2C_I2CSlaveClearReadBuf();
        (void) I2C_I2CSlaveClearReadStatus();
    }
}

void setZcdBuffer(uint8_t *buf,ZCD_FRAME *zcd)
{
    uint8_t *ptr = buf; // Pointer to traverse the buffer

    // Write COMMON_FIELDS
    memcpy(ptr, &zcd->kind, sizeof(zcd->kind));                     ptr += sizeof(zcd->kind);
    memcpy(ptr, &zcd->usb, sizeof(zcd->usb));                       ptr += sizeof(zcd->usb);
    memcpy(ptr, &zcd->bnd, sizeof(zcd->bnd));                       ptr += sizeof(zcd->bnd);
    memcpy(ptr, &zcd->ppid, sizeof(zcd->ppid));                     ptr += sizeof(zcd->ppid);
    memcpy(ptr, &zcd->devid, sizeof(zcd->devid));                   ptr += sizeof(zcd->devid);
    memcpy(ptr, &zcd->fmt, sizeof(zcd->fmt));                       ptr += sizeof(zcd->fmt);
    memcpy(ptr, &zcd->idx, sizeof(zcd->idx));                       ptr += sizeof(zcd->idx);
    memcpy(ptr, &zcd->trt, sizeof(zcd->trt));                       ptr += sizeof(zcd->trt);
    memcpy(ptr, &zcd->ack, sizeof(zcd->ack));                       ptr += sizeof(zcd->ack);
    memcpy(ptr, &zcd->ppc, sizeof(zcd->ppc));                       ptr += sizeof(zcd->ppc);
    memcpy(ptr, &zcd->run, sizeof(zcd->run));                       ptr += sizeof(zcd->run);
    memcpy(ptr, &zcd->txid, sizeof(zcd->txid));                     ptr += sizeof(zcd->txid);
    memcpy(ptr, &zcd->rxid, sizeof(zcd->rxid));                     ptr += sizeof(zcd->rxid);
    memcpy(ptr, &zcd->cnt, sizeof(zcd->cnt));                       ptr += sizeof(zcd->cnt);
    memcpy(ptr, &zcd->pos, sizeof(zcd->pos));                       ptr += sizeof(zcd->pos);

    // Write ZCD_ADDITIONAL_FIELDS
    memcpy(ptr, &zcd->fps, sizeof(zcd->fps));                       ptr += sizeof(zcd->fps);
    memcpy(ptr, &zcd->itf, sizeof(zcd->itf));                       ptr += sizeof(zcd->itf);
    memcpy(ptr, &zcd->destIdErrCnt, sizeof(zcd->destIdErrCnt));     ptr += sizeof(zcd->destIdErrCnt);
    memcpy(ptr, &zcd->destIdDiff, sizeof(zcd->destIdDiff));         ptr += sizeof(zcd->destIdDiff);
    memcpy(ptr, &zcd->phyRxFrameCnt, sizeof(zcd->phyRxFrameCnt));   ptr += sizeof(zcd->phyRxFrameCnt);
    memcpy(ptr, &zcd->frameDiff, sizeof(zcd->frameDiff));           ptr += sizeof(zcd->frameDiff);
}

void setZedBuffer(uint8_t *buf,ZED_FRAME *z)
{
    uint8_t *ptr = buf; // Pointer to traverse the buffer

    // Write COMMON_FIELDS (45Bytes)
    memcpy(ptr, &z->kind, sizeof(z->kind));                     ptr += sizeof(z->kind);
    memcpy(ptr, &z->usb, sizeof(z->usb));                       ptr += sizeof(z->usb);
    memcpy(ptr, &z->bnd, sizeof(z->bnd));                       ptr += sizeof(z->bnd);
    memcpy(ptr, &z->ppid, sizeof(z->ppid));                     ptr += sizeof(z->ppid);
    memcpy(ptr, &z->devid, sizeof(z->devid));                   ptr += sizeof(z->devid);
    memcpy(ptr, &z->fmt, sizeof(z->fmt));                       ptr += sizeof(z->fmt);
    memcpy(ptr, &z->idx, sizeof(z->idx));                       ptr += sizeof(z->idx);
    memcpy(ptr, &z->trt, sizeof(z->trt));                       ptr += sizeof(z->trt);
    memcpy(ptr, &z->ack, sizeof(z->ack));                       ptr += sizeof(z->ack);
    memcpy(ptr, &z->ppc, sizeof(z->ppc));                       ptr += sizeof(z->ppc);
    memcpy(ptr, &z->run, sizeof(z->run));                       ptr += sizeof(z->run);
    memcpy(ptr, &z->txid, sizeof(z->txid));                     ptr += sizeof(z->txid);
    memcpy(ptr, &z->rxid, sizeof(z->rxid));                     ptr += sizeof(z->rxid);
    memcpy(ptr, &z->cnt, sizeof(z->cnt));                       ptr += sizeof(z->cnt);
    memcpy(ptr, &z->pos, sizeof(z->pos));                       ptr += sizeof(z->pos);
}

uint8_t* getI2CReadBuffer()
{
    return i2cReadBuffer;
}

/* [] END OF FILE */
