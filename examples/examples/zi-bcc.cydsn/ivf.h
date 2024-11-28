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
    
typedef enum{
    change_none = 0,    //0x00 :알림 없음/알림 반영 완료
    change_eo,          //0x01 : 영상융합처리기 -> 조준경 EO 변경 요청 알림
    change_ir_white,    //0x02 : 영상융합처리기 -> 조준경 IR 백상 변경 요청 알림
    change_ir_black     //0x03 : 영상융합처리기 -> 조준경 IR 흑상 변경 요청 알림
}ScopeCamera;

typedef struct {
    ScopeCamera scopeCamera;
} IvfCom;

IvfCom ivfCom;
    
#endif

/* [] END OF FILE */
