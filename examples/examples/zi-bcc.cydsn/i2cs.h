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

#define I2C_WR_BUFFER_SIZE  (3u)
#define I2C_RD_BUFFER_SIZE  (3u)

/* LED constants */
#define LED_ON              (0u)
#define LED_OFF             (1u)

/* Buffer and packet size */
#define PACKET_SIZE         (I2C_WR_BUFFER_SIZE)

/* Packet positions */
#define PACKET_SOP_POS      (0u)
#define PACKET_CMD_POS      (1u)
#define PACKET_STS_POS      (PACKET_CMD_POS)
#define PACKET_EOP_POS      (2u)

/* Start and end of packet markers */
#define PACKET_SOP          (0x01u)
#define PACKET_EOP          (0x17u)

/* Command statuses */
#define STS_CMD_DONE        (0x00u)
#define STS_CMD_FAIL        (0xFFu)

/* List of supported commands */
#define CMD_SET_OFF         (0u)
#define CMD_SET_RED         (1u)
#define CMD_SET_GREEN       (2u)
#define CMD_SET_BLUE        (3u)

typedef enum{
    change_none = 0,    //0x00 :알림 없음/알림 반영 완료
    change_eo,          //0x01 : 영상융합처리기 -> 조준경 EO 변경 요청 알림
    change_ir_white,    //0x02 : 영상융합처리기 -> 조준경 IR 백상 변경 요청 알림
    change_ir_black     //0x03 : 영상융합처리기 -> 조준경 IR 흑상 변경 요청 알림
}ScopeCamera;

typedef struct {
    ScopeCamera sc;
} IvfCom;

IvfCom ivfCom;

void i2cs_start(void);
void i2cs_process(void);

/* [] END OF FILE */
