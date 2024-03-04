#include "imu.h"
#include "icd.h"

static uint8_t calibrate = 0;

void IMU_init(void)
{
    UART_IMU_Start();
}

uint8_t IMU_get(uint16_t* values)
{
    uint8_t upper;
    uint8_t lower;
    uint16_t SOP;
    uint16_t CHK;
    uint16_t DATA;
    uint8_t cnt;
    uint8_t total_cnt;
    //char message[128];
    
#if HBLE
    if (calibrate == 1)
    {
        if (UART_IMU_UartGetChar() == '<') // <ok> or <er>
        {
            calibrate = 0;
            return 1;
        }
        else
        {
            return 0;
        }
    }
#endif
#if DBLE
    if (AIAS_ICD_get(WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE) == 0x01 || // GYRO
        AIAS_ICD_get(WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE) == 0x02 || // ACCELERO
        AIAS_ICD_get(WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE) == 0x03)   // MAGNETO
    {
        if (calibrate == 0)
        {
            switch (AIAS_ICD_get(WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE))
            {
                case 0x01:
                    IMU_calibration_gyro();
                    break;
                case 0x02:
                    IMU_calibration_accelero_free();
                    break;
                case 0x03:
                    IMU_calibration_magneto_free();
                    break;
            }
        }
        
        if (UART_IMU_UartGetChar() == '<') // <ok> or <er>
        {
            AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE, 0x80);
            calibrate = 0;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE, 0x00);
#endif
    UART_IMU_UartPutChar('*');
    CyDelay(10);
    
    upper = UART_IMU_UartGetChar();
    lower = UART_IMU_UartGetChar();
    SOP = (upper << 8) | lower;
    CHK = 0;
    total_cnt = NUM_TOTAL_IMU_VALUES;
    
    if (SOP == 0x5555)
    {
        cnt = 0;
        
        do
        {
            CHK = CHK + upper + lower;
            upper = UART_IMU_UartGetChar();
            lower = UART_IMU_UartGetChar();
            DATA = upper << 8 | lower;
            //sprintf(message, "%d. Receive %X (%X)\r\n", cnt, DATA, CHK);
            //UART_DBG_UartPutString(message);
#if DBLE
            if (AIAS_ICD_get(WIRELESS_VIDEO_RECEIVER_IMU_OUTPUT_TYPE) == IMU_EULER)
            {
                total_cnt = NUM_TOTAL_IMU_VALUES;
            }
            else if (AIAS_ICD_get(WIRELESS_VIDEO_RECEIVER_IMU_OUTPUT_TYPE) == IMU_QUATERNION)
            {
                total_cnt = NUM_TOTAL_IMU_VALUES + 1;
            }
#endif

            if (cnt <= total_cnt)
            {
                values[cnt++] = DATA;
            }
            else
            {
                return 0;
            }
        } while (DATA != CHK);
                
        return 1;
    }
    else
    {
        return 0;
    }
}

void IMU_set_output_format(uint8_t sof)
{
    char msg[6];
    
    if (sof == IMU_EULER)
    {
        sprintf(msg, "<sof1>");
#if DBLE
        AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_OUTPUT_TYPE, 0x00);
#endif
    }
    else if (sof == IMU_QUATERNION)
    {
        sprintf(msg, "<sof2>");
#if DBLE
        AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_OUTPUT_TYPE, 0x01);
#endif
    }
    UART_IMU_UartPutString(msg);
}

void IMU_set_output_gyro(uint8_t sog)
{
    char msg[6];
    
    sprintf(msg, "<sog%d>", sog);
    UART_IMU_UartPutString(msg);
}

void IMU_set_output_accelero(uint8_t soa)
{
    char msg[6];
    
    sprintf(msg, "<soa%d>", soa);
    UART_IMU_UartPutString(msg);
}

void IMU_set_output_mageneto(uint8_t som)
{
    char msg[6];
    
    sprintf(msg, "<som%d>", som);
    UART_IMU_UartPutString(msg);
}

void IMU_set_output_temperature(uint8_t sot)
{
    char msg[6];
    
    sprintf(msg, "<sot%d>", sot);
    UART_IMU_UartPutString(msg);
}

void IMU_set_output_timestamp(uint8_t sots)
{
    char msg[7];
    
    sprintf(msg, "<sots%d>", sots);
    UART_IMU_UartPutString(msg);
}

void IMU_calibration_gyro(void)
{
    char msg[4];
    
    calibrate = 1;
    sprintf(msg, "<cg>");
    UART_IMU_UartPutString(msg);
}

void IMU_calibration_accelero_free(void)
{
    char msg[5];
    
    calibrate = 1;
    sprintf(msg, "<caf>");
    UART_IMU_UartPutString(msg);
}

void IMU_calibration_accelero_simple(void)
{
    char msg[5];
    
    calibrate = 1;
    sprintf(msg, "<cas>");
    UART_IMU_UartPutString(msg);
}

void IMU_calibration_magneto_free(void)
{
    char msg[5];
    
    calibrate = 1;
    sprintf(msg, "<cmf>");
    UART_IMU_UartPutString(msg);
}

void IMU_calibration_magneto_xy(void)
{
    char msg[6];
    
    calibrate = 1;
    sprintf(msg, "<cnxy>");
    UART_IMU_UartPutString(msg);
}

void IMU_calibration_magneto_pxy(void)
{
    char msg[7];
    
    calibrate = 1;
    sprintf(msg, "<+cnxy>");
    UART_IMU_UartPutString(msg);
}

void IMU_calibration_magneto_z(void)
{
    char msg[5];
    
    calibrate = 1;
    sprintf(msg, "<cnz>");
    UART_IMU_UartPutString(msg);
}

void IMU_calibration_magneto_pz(void)
{
    char msg[6];
    
    calibrate = 1;
    sprintf(msg, "<+cnz>");
    UART_IMU_UartPutString(msg);
}

void IMU_set_motion_offset(void)
{
    char msg[5];
    
    sprintf(msg, "<cmo>");
    UART_IMU_UartPutString(msg);
}

void IMU_set_motion_offset_x(void)
{
    char msg[6];
    
    calibrate = 1;
    sprintf(msg, "<cmox>");
    UART_IMU_UartPutString(msg);
}

void IMU_set_motion_offset_y(void)
{
    char msg[6];
    
    calibrate = 1;
    sprintf(msg, "<cmoy>");
    UART_IMU_UartPutString(msg);
}

void IMU_set_motion_offset_z(void)
{
    char msg[6];
    
    calibrate = 1;
    sprintf(msg, "<cmoz>");
    UART_IMU_UartPutString(msg);
}

void IMU_set_motion_offset_xy(void)
{
    char msg[7];
    
    calibrate = 1;
    sprintf(msg, "<cmoxy>");
    UART_IMU_UartPutString(msg);
}

void IMU_clear_motion_offset(void)
{
    char msg[6];
    
    calibrate = 1;
    sprintf(msg, "<cmco>");
    UART_IMU_UartPutString(msg);
}
