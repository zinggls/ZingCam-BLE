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

#define I2C_WR_BUFFER_SIZE  (10u)
#define I2C_RD_BUFFER_SIZE  (3u)

/* Buffer and packet size */
#define PACKET_SIZE         (I2C_WR_BUFFER_SIZE)

void i2cs_start(void);
void i2cs_process();
uint8_t* getI2CReadBuffer();
uint8 scopeWorkingState;

#endif //I2CS_H
/* [] END OF FILE */
