# ZingCam-BLE

<pre>
typedef struct
{
    uint16_t imu_values[NUM_TOTAL_IMU_VALUES];
    ZING_status status;
    ICD_parameters icd_params;
} ZCBLE_frame;
    
typedef struct
{
    uint8_t usb;
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t channel;
    uint16_t ppid;
    uint16_t device_id;
    uint8_t format;
    uint8_t index;
    uint32_t fps;
    uint8_t trt;
    uint8_t ack;
    uint8_t ppc;
    uint8_t run;
    uint8_t itf;
    uint32_t txid;
    uint32_t rxid;
    uint32_t destid_err_cnt;
    uint32_t phy_rx_frame_cnt;
    uint32_t mfir;
    uint32_t cnt;
} ZING_status;

    
typedef struct
{
    ZCBLE_scope scope;
    ZCBLE_wireless_channel w_c;
    ZCBLE_opmode opmode;
    ZCBLE_transmitter_imu tx_imu;
    ZCBLE_battery_level battey;
    ZCBLE_modules_status modules;
    ZCBLE_modem modem;
} ICD_parameters;

typedef struct
{
    uint8_t camera:4;
    uint8_t output:4;
} ZCBLE_scope;

typedef struct
{
    uint8_t mode:4;
    uint8_t info:4;
} ZCBLE_wireless_channel;

typedef struct
{
    uint8_t scope:4;
    uint8_t transmitter:4;
} ZCBLE_opmode;

typedef struct
{
    uint8_t status;
    uint8_t type:4;
    uint8_t calibrate:4;
} ZCBLE_transmitter_imu;

typedef struct
{
    uint8_t scope;
    uint8_t transmitter;
} ZCBLE_battery_level;

typedef struct
{
    uint8_t ir:4;
    uint8_t eo:4;
} ZCBLE_modules_status;

typedef struct
{
    uint8_t transmitter:4;
    uint8_t reserved:4;
} ZCBLE_modem;

</pre>
