#ifndef _REGISTER_HPP_
#define _REGISTER_HPP_

//-------------------------------------------------------
//                   BNO055 Registers
//-------------------------------------------------------

                                          // bit 7 || bit 6 || bit 5 || bit 4 || bit 3 || bit 2 || bit 1 || bit 0
#define BNO055_MAG_RADIUS_MSB        0x6A // Magnetometer Radius
#define BNO055_MAG_RADIUS_LSB        0x69 // Magnetometer Radius
#define BNO055_ACC_RADIUS_MSB        0x68 // Accelerometer Radius
#define BNO055_ACC_RADIUS_LSB        0x67 // Accelerometer Radius
#define BNO055_GYR_OFFSET_Z_MSB      0x66 // Gyroscope Offset Z <15:8>
#define BNO055_GYR_OFFSET_Z_LSB      0x65 // Gyroscope Offset Z <7:0>
#define BNO055_GYR_OFFSET_Y_MSB      0x64 // Gyroscope Offset Y <15:8>
#define BNO055_GYR_OFFSET_Y_LSB      0x63 // Gyroscope Offset Y <7:0>
#define BNO055_GYR_OFFSET_X_MSB      0x62 // Gyroscope Offset X <15:8>
#define BNO055_GYR_OFFSET_X_LSB      0x61 // Gyroscope Offset X <7:0>
#define BNO055_MAG_OFFSET_Z_MSB      0x60 // Magnetometer Offset Z <15:8>
#define BNO055_MAG_OFFSET_Z_LSB      0x5F // Magnetometer Offset Z <7:0>
#define BNO055_MAG_OFFSET_Y_MSB      0x5E // Magnetometer Offset Y <15:8>
#define BNO055_MAG_OFFSET_Y_LSB      0x5D // Magnetometer Offset Y <7:0>
#define BNO055_MAG_OFFSET_X_MSB      0x5C // Magnetometer Offset X <15:8>
#define BNO055_MAG_OFFSET_X_LSB      0x5B // Magnetometer Offset X <7:0>
#define BNO055_ACC_OFFSET_Z_MSB      0x5A // Accelerometer Offset Z <15:8>
#define BNO055_ACC_OFFSET_Z_LSB      0x59 // Accelerometer Offset Z <7:0>
#define BNO055_ACC_OFFSET_Y_MSB      0x58 // Accelerometer Offset Y <15:8>
#define BNO055_ACC_OFFSET_Y_LSB      0x57 // Accelerometer Offset Y <7:0>
#define BNO055_ACC_OFFSET_X_MSB      0x56 // Accelerometer Offset X <15:8>
#define BNO055_ACC_OFFSET_X_LSB      0x55 // Accelerometer Offset X <7:0>
// WIP FINISH COMMENTS based on the BNO055 Datasheet
                                          // bit 7      ||      bit 6       ||      bit 5       ||      bit 4       ||      bit 3       ||      bit 2       ||      bit 1       ||      bit 0

#define BNO055_AXIS_MAP_SIGN         0x42 //                                                                                            ||Remapped X-Ax Sign||Remapped Y-Ax Sign||Remapped Z-Ax Sign
#define BNO055_AXIS_MAP_CONFIG       0x41 //                                ||        Remapped Z axis value         ||         Remapped Y axis value        ||        Remapped X axis value
#define BNO055_TEMP_SOURCE           0x40 //                                                                                                                ||          TEMP_Source <1:0>
#define BNO055_SYS_TRIGGER           0x3F // CLK_SEL    ||     RST_INT      ||      RST_SYS     ||                                                                               ||    Self_Test
#define BNO055_PWR_MODE              0x3E // 
#define BNO055_OPR_MODE              0x3D // 
#define BNO055_Reserved              0x3C // 
#define BNO055_UNIT_SEL              0x3B // 
#define BNO055_SYS_ERR               0x3A // 
#define BNO055_SYS_STATUS            0x39 // 
#define BNO055_SYS_CLK_STA           0x38 // 
#define BNO055_INT_STA               0x37 // 
#define BNO055_ST_RESULT             0x36 // 
#define BNO055_CALIB_STAT            0x35 // 
#define BNO055_TEMP                  0x34 // 
#define BNO055_GRV_Data_Z_M          0x33 // 
#define BNO055_GRV_Data_Z_L          0x32 // 
#define BNO055_GRV_Data_Y_M          0x31 // 
#define BNO055_GRV_Data_Y_L          0x30 // 
#define BNO055_GRV_Data_X_M          0x2F // 
#define BNO055_GRV_Data_X_L          0x2E // 
#define BNO055_LIA_Data_Z_MB         0x2D // 
#define BNO055_LIA_Data_Z_LS         0x2C // 
#define BNO055_LIA_Data_Y_MB         0x2B // 
#define BNO055_LIA_Data_Y_LS         0x2A // 
#define BNO055_LI A_Data_X_MB        0x29 // 
#define BNO055_LIA_Data_X_LS         0x28 // 
#define BNO055_QUA_Data_z_M          0x27 // 
#define BNO055_QUA_Data_z_LS         0x26 // 
#define BNO055_QUA_Data_y_M          0x25 // 
#define BNO055_QUA_Data_y_LS         0x24 // 
#define BNO055_QUA_Data_x_M          0x23 // 
#define BNO055_QUA_Data_x_LS         0x22 // 
#define BNO055_QUA_Data_w_M          0x21 // 
#define BNO055_QUA_Data_w_L          0x20 // 
#define BNO055_EUL_Pitch_MSB         0x1F // 
#define BNO055_EUL_Pitch_LSB         0x1E // 

#define BNO055_EUL_Roll_MSB          0x1D // Roll Data <15:8>
#define BNO055_EUL_Roll_LSB          0x1C // Roll Data <7:0>
#define BNO055_EUL_Heading_MSB       0x1B // Heading Data <15:8>
#define BNO055_EUL_Heading_LSB       0x1A // Heading Data <7:0>
#define BNO055_GYR_DATA_Z_MSB        0x19 // Gyroscope Data Z <15:8>
#define BNO055_GYR_DATA_Z_LSB        0x18 // Gyroscope Data Z <7:0>
#define BNO055_GYR_DATA_Y_MSB        0x17 // Gyroscope Data Y <15:8>
#define BNO055_GYR_DATA_Y_LSB        0x16 // Gyroscope Data Y <7:0>
#define BNO055_GYR_DATA_X_MSB        0x15 // Gyroscope Data X <15:8>
#define BNO055_GYR_DATA_X_LSB        0x14 // Gyroscope Data X <7:0>
#define BNO055_MAG_DATA_Z_MSB        0x13 // Magnetometer Data Z <15:8>
#define BNO055_MAG_DATA_Z_LSB        0x12 // Magnetometer Data Z <7:0>
#define BNO055_MAG_DATA_Y_MSB        0x11 // Magnetometer Data Y <15:8>
#define BNO055_MAG_DATA_Y_LSB        0x10 // Magnetometer Data Y <7:0>
#define BNO055_MAG_DATA_X_MSB        0x0F // Magnetometer Data X <15:8>
#define BNO055_MAG_DATA_X_LSB        0x0E // Magnetometer Data X <7:0>
#define BNO055_ACC_DATA_Z_MSB        0x0D // Acceleration Data Z <15:8>
#define BNO055_ACC_DATA_Z_LSB        0x0C // Acceleration Data Z <7:0>
#define BNO055_ACC_DATA_Y_MSB        0x0B // Acceleration Data Y <15:8>
#define BNO055_ACC_DATA_Y_LSB        0x0A // Acceleration Data Y <7:0>
#define BNO055_ACC_DATA_X_MSB        0x09 // Acceleration Data X <15:8>
#define BNO055_PWR_ACC_DATA_X_LSB    0x08 // Acceleration Data X <7:0>
#define BNO055_Page_ID               0x07 // Page ID
#define BNO055_BL_Rev_ID             0x06 // Boot loader Version
#define BNO055_SW_REV_ID_MSB         0x05 // SW Revision ID <15:8>
#define BNO055_SW_REV_ID_LSB         0x04 // SW Revision ID <7:0>
#define BNO055_GYR_ID                0x03 // GYRO chip ID
#define BNO055_MAG_ID                0x02 // MAG chip ID
#define BNO055_ACC_ID                0x01 // ACC chip ID
#define BNO055_CHIP_ID               0x00 // BNO055 CHIP ID




//-------------------------------------------------------
//                  MPL3115A2 Registers
//-------------------------------------------------------

#define MPL3115_I2C_ADDR             0x60 // I2C address
#define MPL3115_STATUS               0x00 // Sensor Status Register
#define MPL3115_OUT_P_MSB            0x01 // Pressure Data Out MSB
#define MPL3115_OUT_P_CSB            0x02 // Pressure Data Out CSB
#define MPL3115_OUT_P_LSB            0x03 // Pressure Data Out LSB
#define MPL3115_OUT_T_MSB            0x04 // Temperature Data Out MSB
#define MPL3115_OUT_T_LSB            0x05 // Temperature Data Out LSB
#define MPL3115_DR_STATUS            0x06 // Sensor Status Register
#define MPL3115_OUT_P_DELTA MSB      0x07 // Pressure Data Out Delta MSB
#define MPL3115_OUT_P_DELTA_CSB      0x08 // Pressure Data Out Delta CSB
#define MPL3115_OUT_P_DELTA_LSB      0x09 // Pressure Data Out Delta LSB
#define MPL3115_OUT_T_DELTA_MSB      0x0A // Temperature Data Out Delta MSB
#define MPL3115_OUT_T_DELTA_LSB      0x0B // Temperature Data Out Delta LSB
#define MPL3115_WHO_AM_I             0x0C // Device Identification Register
#define MPL3115_F_STATUS             0x0D // FIFO Status Register
#define MPL3115_F_DATA               0X0E // FIFO 8-bit Data Access
#define MPL3115_F_SETUP              0x0F // FIFO Setup Register
#define MPL3115_TIME_DLY             0x10 // Time Delay
#define MPL3115_SYSMOD               0x11 // System Mode Register
#define MPL3115_INT_SOURCE           0x12 // Interrupt Source Register
#define MPL3115_PT_DATA_CFG          0x13 // PT Data Configuration Register
#define MPL3115_BAR_IN_MSB           0x14 // BAR Input in MSB
#define MPL3115_BAR_IN_LSB           0x15 // BAR Input in LSB
#define MPL3115_P_TGT_MSB            0x16 // Pressure Target MSB
#define MPL3115_P_TGT_LSB            0x17 // Pressure Target LSB
#define MPL3115_T_TGT                0x18 // Temperature Target
#define MPL3115_P_WND_MSB            0x19 // Pressure/Altitude Window MSB
#define MPL3115_P_WND_LSB            0x1A // Pressure/Altitude Window LSB
#define MPL3115_T_WND                0X1B // Temperature Window
#define MPL3115_P_MIN_MSB            0x1C // Minimum Pressure Data Out MSB
#define MPL3115_P_MIN_CSB            0x1D // Minimum Pressure Data Out CSB
#define MPL3115_P_MIN_LSB            0x1E // Minimum Pressure Data Out LSB
#define MPL3115_T_MIN_MSB            0x1F // Minimum Temperature Data Out MSB
#define MPL3115_T_MIN_LSB            0x20 // Minimum Temperature Data Out LSB
#define MPL3115_P_MAX_MSB            0x21 // Maximum Pressure Data Out MSB
#define MPL3115_P_MAX_CSB            0x22 // Maximum Pressure Data Out CSB
#define MPL3115_P_MAX_LSB            0x23 // Maximum Pressure Data Out LSB
#define MPL3115_T_MAX_MSB            0X24 // Maximum Temperature Data Out MSB
#define MPL3115_T_MAX_LSB            0x25 // Maximum Temperature Data Out LSB
#define MPL3115_CTRL_REG1            0x26 // Control Register 1
#define MPL3115_CTRL_REG2            0x27 // Control Register 2
#define MPL3115_CTRL_REG3            0x28 // Control Register 3
#define MPL3115_CTRL_REG4            0x29 // Control Register 4
#define MPL3115_CTRL_REG5            0x2A // Control Register 5
#define MPL3115_OFF_P                0x2B // Pressure Data User Offset
#define MPL3115_OFF_T                0x2C // Temperature Data User Offset
#define MPL3115_OFF_H                0x2D // Altitude Data User Offset Register

#endif