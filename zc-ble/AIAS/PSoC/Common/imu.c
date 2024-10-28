#include "imu.h"
#include "icd.h"

static uint8_t type = IMU_EULER;
static uint8_t calibrate = 0;
static uint8_t imu_status[NUM_IMU_QUATERNION_DATA_BYTES + 1];
static uint8_t imu_cnt = 0;
static uint16_t checksum = 0;
static uint8_t SOF = 0;
static uint8_t checksum_upper = 0;
static uint8_t checksum_lower = 0;
static uint8_t check_ok = 0;

void UART_IMU_RX_INTERRUPT(void)
{
    char ch;
    uint8_t num_bytes;
    
    ch = UART_IMU_UartGetByte();
    num_bytes = (type == 0) ? NUM_IMU_EULER_DATA_BYTES : NUM_IMU_QUATERNION_DATA_BYTES;
    
    if (ch == 0x55)
    {
        if (SOF == 0)
        {
            imu_cnt = 0;
            imu_status[NUM_IMU_SOF_UPPER] = ch;
            checksum = ch;
            SOF = 1;
        }
        else if (SOF == 1)
        {
            imu_status[NUM_IMU_SOF_LOWER] = ch;
            checksum = checksum + ch;
            SOF = 2;
        }
        else
        {
            imu_status[imu_cnt] = ch;
            checksum = checksum + ch;
        }
    }
    else
    {
        if (imu_cnt < num_bytes - 2)
        {
            imu_status[imu_cnt] = ch;
            checksum = checksum + ch;
        }
        else
        {
            if (imu_cnt == num_bytes - 2)
            {
                imu_status[imu_cnt] = checksum_upper = ch;
            }
            else if (imu_cnt == num_bytes - 2 + 1)
            {
                imu_status[imu_cnt] = checksum_lower = ch;
                
                imu_cnt = 0;
                SOF = 0;
                
                if (checksum == ((checksum_upper << 8) | checksum_lower))
                {
                    // IMU SUCCESS
                    imu_status[num_bytes] = 1;
                }
                else
                {
                    // IMU ERROR
                    imu_status[num_bytes] = 0;
                }
                checksum = 0;
            }
            else
            {
                imu_cnt = 0;
                checksum = 0;
                SOF = 0;
                
                // IMU ERROR
                imu_status[num_bytes] = 0;
            }
        }
    }
    imu_cnt = imu_cnt + 1;
}

void IMU_init(void)
{
    UART_IMU_Start();
    UART_IMU_SetCustomInterruptHandler(UART_IMU_RX_INTERRUPT);
    
#if DBLE
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE, 0);
#endif
}

uint8_t IMU_get(uint16_t* values)
{
    uint8_t upper;
    uint8_t lower;
    uint8_t num_bytes;

    num_bytes = (type == 0) ? NUM_IMU_EULER_DATA_BYTES : NUM_IMU_QUATERNION_DATA_BYTES;
    
    if (imu_status[num_bytes] == 0) // checksum failed
    {
        return 0; // this is not imu's problem
    }
    
    for (uint8_t i = 2; i < num_bytes; i = i + 2)
    {
        upper = imu_status[i];
        lower = imu_status[i + 1];
        
        values[(i / 2) - 1] = ((upper << 8) | lower);
    }
    
    return 1;
}

uint8_t IMU_get_type(void)
{
    return type;
}

uint8_t IMU_get_calibrate(void)
{
    return calibrate;
}

void IMU_set_output_format(uint8_t sof)
{
    char msg[6];
    
    if (sof == IMU_EULER)
    {
        type = IMU_EULER;
        sprintf(msg, "<sof1>");
    }
    else if (sof == IMU_QUATERNION)
    {
        type = IMU_QUATERNION;
        sprintf(msg, "<sof2>");
    }
    UART_IMU_UartPutString(msg);
}

void IMU_calibration_gyro(void)
{
    char msg[4];
    
    calibrate = 1;
    sprintf(msg, "<cg>");
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