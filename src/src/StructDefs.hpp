
struct GPSdata  {
    float lat = 0;
    float lon = 0;
    float altitude = 0;
    float speed = 0;
    float angle = 0;
    float sat_num = 0;
    uint32_t t;
};

struct ACCELdata {
    float x;
    float y;
    float z;
    uint32_t t;
};

struct IMUdata {
    double orient_euler[3] = {0,0,0};
    double gyro_fused[3] = {0,0,0};
    double accel_fused[3] = {0,0,0};
    double accel_raw[3] = {0,0,0};
    double gyro_raw[3] = {0,0,0};
    double magnetometer[3] = {0,0,0};
    double orient_quat[4] = {0,0,0,0}; // {w,x,y,z}
    uint32_t t = 0;
};

struct BAROMdata {
    float pressure = 0;
    float altitude = 0;
    float temperature = 0;
    uint32_t t = 0;
};