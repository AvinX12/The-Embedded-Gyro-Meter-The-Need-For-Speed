#include <mbed.h>
#include "configuration.h"
SPI spi(GYRO_MOSI, GYRO_MISO, GYRO_SCK);
DigitalOut GyroCs(GYRO_CS);

// ZERO-level, after calibration of gyroscope values
int16_t zero_X = 13;
int16_t zero_Y = 22;
int16_t zero_Z = 3; 

int Gyro_Init()
{
    int ID;

    GyroCs = 0;
    spi.write(ID_REG_ADDRESS | READ_CMD);
    ID = spi.write(DUMMY_BYTE);
    wait_us(3);
    GyroCs = 1;

    GyroCs = 0;
    spi.write(REG_1_ADDRESS); // REG_1 address
    spi.write(REG_1_CONFIG);
    wait_us(3);
    GyroCs = 1;

    GyroCs = 0;
    spi.write(REG_2_ADDRESS); // REG_2 address
    spi.write(REG_2_CONFIG);
    wait_us(3);
    GyroCs = 1;

    GyroCs = 0;
    spi.write(REG_4_ADDRESS); // REG_4 address
    spi.write(REG_4_CONFIG);
    wait_us(3);
    GyroCs = 1;

    GyroCs = 0;
    spi.write(REG_5_ADDRESS); // REG_5 address
    spi.write(REG_5_CONFIG);
    wait_us(3);
    GyroCs = 1;

    return ID;
}

void Gyro_Get_XYZ(float xyz[])
{
    int low = 0;
    int high = 0;
    int16_t x, y, z;

    // Read data for Xaxis
    GyroCs = 0;
    spi.write(X_REG_ADDRESS | READ_CMD | MULTIPLEBYTE_CMD);
    wait_us(3);
    low = spi.write(DUMMY_BYTE);
    high = spi.write(DUMMY_BYTE);
    wait_us(3);
    GyroCs = 1;
    x = (high << 8) | low;

    // Read data for Yaxis
    GyroCs = 0;
    spi.write(Y_REG_ADDRESS | READ_CMD | MULTIPLEBYTE_CMD);
    wait_us(3);
    low = spi.write(DUMMY_BYTE);
    high = spi.write(DUMMY_BYTE);
    wait_us(3);
    GyroCs = 1;
    y = (high << 8) | low;

    // Read data for Zaxis
    GyroCs = 0;
    spi.write(Z_REG_ADDRESS | READ_CMD | MULTIPLEBYTE_CMD);
    wait_us(3);
    low = spi.write(DUMMY_BYTE);
    high = spi.write(DUMMY_BYTE);
    wait_us(3);
    GyroCs = 1;
    z = (high << 8) | low;

    xyz[0] = (x - zero_X) * FS_500_SENSITIVITY;
    xyz[1] = (y - zero_Y) * FS_500_SENSITIVITY;
    xyz[2] = (z - zero_Z) * FS_500_SENSITIVITY;

#if CALIBRATION
    if (cal_count < 100)
    {
        X_cal[cal_count] = x;
        Y_cal[cal_count] = y;
        Z_cal[cal_count] = z;
        cal_count++;
    }
    else if (cal_count == 100)
    {
        printf("x;y;z;\n");
        for (int i = 0; i < 100; i++)
        {
            zero_X += X_cal[i] / 100;
            zero_Y += Y_cal[i] / 100;
            zero_Z += Z_cal[i] / 100;
            printf("%d;%d;%d;\n", X_cal[i], Y_cal[i], Z_cal[i]);
        }
        cal_count++;
    }
#endif
}