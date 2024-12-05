#include <ZFrame.h>

static ZxxKind zxxKind = Unknown;

ZxxKind detectZxx(const char *buf)
{
    if(memcmp(buf,"ZED ",4)==0) return ZED;
    if(memcmp(buf,"ZCH ",4)==0) return ZCH;
    if(memcmp(buf,"ZCD ",4)==0) return ZCD;
    return Unknown;
}

ZxxKind inspect(const char *buf)
{
    int kind = (buf[3]<<24)| (buf[2]<<16) | (buf[1]<<8) | buf[0];
    if(kind==ZED) return ZED;
    if(kind==ZCH) return ZCH;
    if(kind==ZCD) return ZCD;
    return Unknown;
}

int parse(ZxxKind k, void *data, const char *buf)
{
    int rtn = 0;
    
    switch(k) {
        case ZED:
            {
                ZED_FRAME *z = (ZED_FRAME*)data;
                if(ZED_NUM == sscanf(buf, "ZED USB:%d BND:%c PPID:0x%X DeviceID:0x%X TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d",
                                    &z->usb,&z->bnd,&z->ppid,&z->devid,&z->trt,&z->ack,&z->ppc,&z->txid,&z->rxid,&z->run,&z->cnt)) {
                                        z->kind = ZED;
                                        rtn = 1;
                                    }
            }
            break;
        case ZCH:
            {
                ZCH_FRAME *z = (ZCH_FRAME*)data;
                if(ZCH_NUM == sscanf(buf, "ZCH USB:%d VND:0x%X PRD:0x%X BND:%c PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d",
                                    &z->usb,&z->vnd,&z->prd,&z->bnd,&z->ppid,&z->devid,&z->fmt,&z->idx,&z->trt,&z->ack,&z->ppc,&z->txid,&z->rxid,&z->run,&z->cnt)) {
                                        z->kind = ZCH;
                                        rtn=1;
                                    }
            }
            break;
        case ZCD:
            {
                ZCD_FRAME *z = (ZCD_FRAME*)data;
                if(ZCD_NUM == sscanf(buf, "ZCD USB:%d PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d FPS:0x%X TRT:%c ACK:%c PPC:%c RUN:%c ITF:%c TXID:0x%X RXID:0x%X DestID_ERR_CNT:%d(%d) PHY_RX_FRAME_CNT:%d(%d) MFIR:%d/%d CNT:%d",
                                    &z->usb,&z->ppid,&z->devid,&z->fmt,&z->idx,&z->fps,&z->trt,&z->ack,&z->ppc,&z->run,&z->itf,&z->txid,&z->rxid,&z->destIdErrCnt,&z->destIdDiff,&z->phyRxFrameCnt,&z->frameDiff,&z->destIdDiff,&z->frameDiff,&z->cnt)) {
                                        z->kind = ZCD;
                                        rtn=1;
                                    }
            }
            break;
        default:
            break;
    }
    return rtn;
}

void *getFrame(ZED_FRAME *zed, ZCH_FRAME *zch)
{
    if(zxxKind==ZED) return zed;
    if(zxxKind==ZCH) return zch;
    return 0;
}

uint16 getFrameSize()
{
    if(zxxKind==ZED) return sizeof(ZED_FRAME);
    if(zxxKind==ZCH) return sizeof(ZCH_FRAME);
    if(zxxKind==ZCD) return sizeof(ZCD_FRAME);
    return 0;   //never reach here
}

ZxxKind* getZxxKind()
{
    return &zxxKind;
}

void setZcdBuffer(uint8_t *buf,ZCD_FRAME *z)
{
    uint8_t *ptr = buf; // Pointer to traverse the buffer

    // Write COMMON_FIELDS
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

    // Write ZCD_ADDITIONAL_FIELDS
    memcpy(ptr, &z->fps, sizeof(z->fps));                       ptr += sizeof(z->fps);
    memcpy(ptr, &z->itf, sizeof(z->itf));                       ptr += sizeof(z->itf);
    memcpy(ptr, &z->destIdErrCnt, sizeof(z->destIdErrCnt));     ptr += sizeof(z->destIdErrCnt);
    memcpy(ptr, &z->destIdDiff, sizeof(z->destIdDiff));         ptr += sizeof(z->destIdDiff);
    memcpy(ptr, &z->phyRxFrameCnt, sizeof(z->phyRxFrameCnt));   ptr += sizeof(z->phyRxFrameCnt);
    memcpy(ptr, &z->frameDiff, sizeof(z->frameDiff));           ptr += sizeof(z->frameDiff);
}

void setZedImuBuffer(uint8_t *buf,ZED_FRAME *z)
{
    uint8_t *ptr = buf; // Pointer to traverse the buffer
    
	memcpy(ptr, &z->imu1, sizeof(z->imu1));                     ptr += sizeof(z->imu1);
	memcpy(ptr, &z->imu2, sizeof(z->imu2));                     ptr += sizeof(z->imu2);
	memcpy(ptr, &z->imu3, sizeof(z->imu3));                     ptr += sizeof(z->imu3);
	memcpy(ptr, &z->imu4, sizeof(z->imu4));                     ptr += sizeof(z->imu4);
	memcpy(ptr, &z->imu5, sizeof(z->imu5));                     ptr += sizeof(z->imu5);
	memcpy(ptr, &z->imuChecksum, sizeof(z->imuChecksum));       ptr += sizeof(z->imuChecksum);
}

void setZchImuBuffer(uint8_t *buf,ZCH_FRAME *z)
{
    uint8_t *ptr = buf; // Pointer to traverse the buffer
    
	memcpy(ptr, &z->imu1, sizeof(z->imu1));                     ptr += sizeof(z->imu1);
	memcpy(ptr, &z->imu2, sizeof(z->imu2));                     ptr += sizeof(z->imu2);
	memcpy(ptr, &z->imu3, sizeof(z->imu3));                     ptr += sizeof(z->imu3);
	memcpy(ptr, &z->imu4, sizeof(z->imu4));                     ptr += sizeof(z->imu4);
	memcpy(ptr, &z->imu5, sizeof(z->imu5));                     ptr += sizeof(z->imu5);
	memcpy(ptr, &z->imuChecksum, sizeof(z->imuChecksum));       ptr += sizeof(z->imuChecksum);
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

    //IMU FIELDS
	memcpy(ptr, &z->imu1, sizeof(z->imu1));                     ptr += sizeof(z->imu1);
	memcpy(ptr, &z->imu2, sizeof(z->imu2));                     ptr += sizeof(z->imu2);
	memcpy(ptr, &z->imu3, sizeof(z->imu3));                     ptr += sizeof(z->imu3);
	memcpy(ptr, &z->imu4, sizeof(z->imu4));                     ptr += sizeof(z->imu4);
	memcpy(ptr, &z->imu5, sizeof(z->imu5));                     ptr += sizeof(z->imu5);
	memcpy(ptr, &z->imuChecksum, sizeof(z->imuChecksum));       ptr += sizeof(z->imuChecksum);
}

void setZchBuffer(uint8_t *buf,ZCH_FRAME *z)
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
    
    //IMU FIELDS
	memcpy(ptr, &z->imu1, sizeof(z->imu1));                     ptr += sizeof(z->imu1);
	memcpy(ptr, &z->imu2, sizeof(z->imu2));                     ptr += sizeof(z->imu2);
	memcpy(ptr, &z->imu3, sizeof(z->imu3));                     ptr += sizeof(z->imu3);
	memcpy(ptr, &z->imu4, sizeof(z->imu4));                     ptr += sizeof(z->imu4);
	memcpy(ptr, &z->imu5, sizeof(z->imu5));                     ptr += sizeof(z->imu5);
	memcpy(ptr, &z->imuChecksum, sizeof(z->imuChecksum));       ptr += sizeof(z->imuChecksum);
    
    // Write ZCD_ADDITIONAL_FIELDS
    memcpy(ptr, &z->vnd, sizeof(z->vnd));                       ptr += sizeof(z->vnd);
    memcpy(ptr, &z->prd, sizeof(z->prd));                       ptr += sizeof(z->prd);
}

bool isNoZingCb(uint32 loopCount,uint32 period,uint32 *zingCount)
{
    bool noZingCb = false;
    
    if( (loopCount%period==0) ){
        if(*zingCount==0) noZingCb = true;
        *zingCount = 0;
    }
    return noZingCb;
}