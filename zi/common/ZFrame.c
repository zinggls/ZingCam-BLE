#include <ZFrame.h>
#include "icd.h"
#include "versionInfo.h"
#include "i2cs.h"
#include "Peripheral.h"

static const char ZED_SSCANF_FORMAT[] = "ZED USB:%d BND:%c PPID:0x%X DeviceID:0x%X TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d";
static const char ZCH_SSCANF_FORMAT[] = "ZCH USB:%d VND:0x%X PRD:0x%X BND:%c PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d";
static const char ZCD_SSCANF_FORMAT[] = "ZCD USB:%d PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d FPS:0x%X TRT:%c ACK:%c PPC:%c RUN:%c ITF:%c TXID:0x%X RXID:0x%X DestID_ERR_CNT:%d DestID_DIFF:%d PHY_RX_FRAME_CNT:%d PHY_RX_DIFF:%d CNT:%d";
static char git_describe[128];
static char git_info[128];

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
                    &z->usb,&z->ppid,&z->devid,&z->fmt,&z->idx,&z->fps,&z->trt,&z->ack,&z->ppc,&z->run,&z->itf,&z->txid,&z->rxid,&z->destIdErrCnt,&z->destIdDiff,&z->phyRxFrameCnt,&z->frameDiff,&z->cnt);
}

static int parse_log(const char *log, char *git_describe, char *git_info)
{
    // Initialize buffers to empty strings
    git_describe[0] = '\0';
    git_info[0] = '\0';

    // Use sscanf to extract the required fields and return its result
    return sscanf(log, "GIT DESCRIBE:%[^ ] GIT_INFO:Version Information=%s", git_describe, git_info);
}

static ZxxKind Kind = Unknown;

int parse(void *data, const char *buf)
{
    int rtn = 0;
    
    ZxxKind k = detectZxx(buf);
    switch(k) {
        case ZED:
            {
                Kind = ZED;
                ZXX_FRAME *z = (ZXX_FRAME*)data;
                rtn = zedSscanf(buf,z);
                if(ZED_NUM == rtn) { z->kind = ZED; }else{ z->kind = -1; }
            }
            break;
        case ZCH:
            {
                Kind = ZCH;
                ZXX_FRAME *z = (ZXX_FRAME*)data;
                rtn = zchSscanf(buf,z);
                if(ZCH_NUM == rtn) { z->kind = ZCH; }else{ z->kind = -1; }
            }
            break;
        case ZCD:
            {
                Kind = ZCD;
                ZCD_FRAME *z = (ZCD_FRAME*)data;
                rtn = zcdSscanf(buf,z);
                if(ZCD_NUM == rtn) { z->kind = ZCD; }else{ z->kind = -1; }
            }
            break;
        default:
            {
                rtn = parse_log(buf,git_describe,git_info);
                if(GIT_DES == rtn){
                    Version ver;
                    memset(&ver,0,sizeof(Version));
                    strcpy(ver.info,git_describe);
                    
                    if(Kind==ZCD) {
                        memcpy(getI2CReadBuffer()+I2C_IVF_READ_BUFFER_SIZE+VERSION_SIZE,ver.info,VERSION_SIZE);
                    }else if(Kind==ZED || Kind==ZCH) {
                        memcpy(peripheral.zxxVer.info,git_describe,VERSION_SIZE);
                    }
                }
            }
            break;
    }
    return rtn;
}

uint8_t *setZcdBuffer(uint8_t *buf,ZCD_FRAME *z)
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
    
    return ptr;
}

uint8_t *setZxxBuffer(uint8_t *buf,ZXX_FRAME *z)
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

    //USB_VND_PRD_FIELDS
    memcpy(ptr, &z->vnd, sizeof(z->vnd));                       ptr += sizeof(z->vnd);
    memcpy(ptr, &z->prd, sizeof(z->prd));                       ptr += sizeof(z->prd);

    return ptr;
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