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
    
typedef enum{
    change_none = 0,    //0x00 :알림 없음/알림 반영 완료
    change_eo,          //0x01 : 영상융합처리기 -> 조준경 EO 변경 요청 알림
    change_ir_white,    //0x02 : 영상융합처리기 -> 조준경 IR 백상 변경 요청 알림
    change_ir_black     //0x03 : 영상융합처리기 -> 조준경 IR 흑상 변경 요청 알림
}ScopeCamera;

typedef struct {
    ScopeCamera scopeCamera;
    uint8_t scopeOutput;
    uint8_t wirelessVideoChannelMode;
    uint8_t wirelessVideoChannelInformation;
} IvfCom;

IvfCom ivfCom;
    
void setReadBuffer_ScopeCamera(uint8_t* rdBuf,ScopeCamera sc);
void setReadBuffer_ScopeOutput(uint8_t* rdBuf,uint8_t so);

#endif

/* [] END OF FILE */
