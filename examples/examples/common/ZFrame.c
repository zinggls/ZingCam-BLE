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