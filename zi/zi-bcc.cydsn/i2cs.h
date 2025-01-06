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

#ifndef I2CS_H
#define I2CS_H

#include <ZFrame.h>

#define I2C_WR_BUFFER_SIZE  (11u)
#define I2C_RD_BUFFER_SIZE  (265u)  //ICD(46)+ZXX(53)+ZCD(66)+VERSION(4*25=100) = 265

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

void i2cs_start(void);
void i2cs_process(ZCD_FRAME *zcd);
uint8_t* getI2CReadBuffer();

#endif //I2CS_H
/* [] END OF FILE */
