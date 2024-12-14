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

#include "ivf.h"

void setReadBuffer_ScopeCamera(uint8_t* rdBuf,uint8_t sc)
{
    /*
    화기조준경 영상 종류 변경 요청 알림
    0x00 :알림 없음/알림 반영 완료
    0x01 : 영상융합처리기 -> 조준경 EO 변경 요청 알림
    0x02 : 영상융합처리기 -> 조준경 IR 백상 변경 요청 알림
    0x03 : 영상융합처리기 -> 조준경 IR 흑상 변경 요청 알림
    
    화기조준경 영상 종류 변경 알림
    0x00 :알림 없음/알림 확인 완료
    0x04 : 조준경 -> 영상융합처리기 EO 변경 알림
    0x05 : 조준경 -> 영상융합처리기 IR 백상 변경 알림
    0x06 : 조준경 -> 영상융합처리기 IR 흑상 변경 알림
    */    
    
    if(sc==0x0) {
        *rdBuf = 0;     //0x00 :알림 없음/알림 확인 완료
        return;
    }
    *rdBuf = sc + 3;
}

void setReadBuffer_ScopeOutput(uint8_t* rdBuf,uint8_t so)
{
    if(so==0) {
        *rdBuf = 0;
        return;
    }
    *rdBuf = so + 2;
}

/* [] END OF FILE */
