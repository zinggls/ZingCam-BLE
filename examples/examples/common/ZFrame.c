#include <ZFrame.h>

static const char ZED_SSCANF_FORMAT[] = "ZED USB:%d BND:%c PPID:0x%X DeviceID:0x%X TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d";
static const char ZCH_SSCANF_FORMAT[] = "ZCH USB:%d VND:0x%X PRD:0x%X BND:%c PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d";
static const char ZCD_SSCANF_FORMAT[] = "ZCD USB:%d PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d FPS:0x%X TRT:%c ACK:%c PPC:%c RUN:%c ITF:%c TXID:0x%X RXID:0x%X DestID_ERR_CNT:%d(%d) PHY_RX_FRAME_CNT:%d(%d) MFIR:%d/%d CNT:%d";

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

static int zedSscanf(const char *buf,ZXX_FRAME *z)
{
    return sscanf(buf, ZED_SSCANF_FORMAT,
        &z->usb,&z->bnd,&z->ppid,&z->devid,&z->trt,&z->ack,&z->ppc,&z->txid,&z->rxid,&z->run,&z->cnt);
}

static int zchSscanf(const char *buf,ZXX_FRAME *z)
{
    return sscanf(buf, ZCH_SSCANF_FORMAT,
        &z->usb,&z->vnd,&z->prd,&z->bnd,&z->ppid,&z->devid,&z->fmt,&z->idx,&z->trt,&z->ack,&z->ppc,&z->txid,&z->rxid,&z->run,&z->cnt);
}

static int zcdSscanf(const char *buf,ZCD_FRAME *z)
{
    return sscanf(buf, ZCD_SSCANF_FORMAT,
                    &z->usb,&z->ppid,&z->devid,&z->fmt,&z->idx,&z->fps,&z->trt,&z->ack,&z->ppc,&z->run,&z->itf,&z->txid,&z->rxid,&z->destIdErrCnt,&z->destIdDiff,&z->phyRxFrameCnt,&z->frameDiff,&z->destIdDiff,&z->frameDiff,&z->cnt);
}

int parse(void *data, const char *buf)
{
    int rtn = 0;
    
    ZxxKind k = detectZxx(buf);
    switch(k) {
        case ZED:
            {
                ZXX_FRAME *z = (ZXX_FRAME*)data;
                rtn = zedSscanf(buf,z);
                if(ZED_NUM == rtn) z->kind = ZED;
            }
            break;
        case ZCH:
            {
                ZXX_FRAME *z = (ZXX_FRAME*)data;
                rtn = zchSscanf(buf,z);
                if(ZCH_NUM == rtn) z->kind = ZCH;
            }
            break;
        case ZCD:
            {
                ZCD_FRAME *z = (ZCD_FRAME*)data;
                rtn = zcdSscanf(buf,z);
                if(ZCD_NUM == rtn) z->kind = ZCD;
            }
            break;
        default:
            break;
    }
    return rtn;
}

uint16 getFrameSize()
{
    if(zxxKind==ZED) return sizeof(ZXX_FRAME);
    if(zxxKind==ZCH) return sizeof(ZXX_FRAME);
    if(zxxKind==ZCD) return sizeof(ZCD_FRAME);
    return 0;   //never reach here
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

void setImuBuffer(uint8_t *buf,ZXX_FRAME *z)
{
    uint8_t *ptr = buf; // Pointer to traverse the buffer
    
	memcpy(ptr, &z->imu1, sizeof(z->imu1));                     ptr += sizeof(z->imu1);
	memcpy(ptr, &z->imu2, sizeof(z->imu2));                     ptr += sizeof(z->imu2);
	memcpy(ptr, &z->imu3, sizeof(z->imu3));                     ptr += sizeof(z->imu3);
	memcpy(ptr, &z->imu4, sizeof(z->imu4));                     ptr += sizeof(z->imu4);
	memcpy(ptr, &z->imu5, sizeof(z->imu5));                     ptr += sizeof(z->imu5);
	memcpy(ptr, &z->imuChecksum, sizeof(z->imuChecksum));       ptr += sizeof(z->imuChecksum);
}

void setZedBuffer(uint8_t *buf,ZXX_FRAME *z)
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

void setZchBuffer(uint8_t *buf,ZXX_FRAME *z)
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