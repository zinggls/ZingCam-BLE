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
        if (imu_cnt < num_bytes)
        {
            imu_status[imu_cnt] = ch;
            checksum = checksum + ch;
        }
        else
        {
            if (imu_cnt == num_bytes)
            {
                checksum_upper = ch;
            }
            else if (imu_cnt == num_bytes + 1)
            {
                checksum_lower = ch;
                
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
    
    calibrate = 1;
    sprintf(msg, "<cmo>");
    UART_IMU_UartPutString(msg);
}

void IMU_set_motion_offset_x(void)
{
    char msg[6];
    
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
