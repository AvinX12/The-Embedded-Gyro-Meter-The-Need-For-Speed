#define CALIBRATION 0  // 0: Normal mode; 1: Calculates the zero level values for gyro
#define PLOT 0         // 0: Normal mode; 1: Print values for plot

// from the datasheet L3GD20
#define RADIUS_X 2     
#define RADIUS_Y 2
#define RADIUS_Z 0.55

#define GYRO_MOSI PF_9   // Master Out Slave In for SPI
#define GYRO_MISO PF_8   // Master In Slave Out for SPI
#define GYRO_SCK  PF_7   // Clock for SPI
#define GYRO_CS   PC_1   // Chip select for SPI

#define REG_1_CONFIG 0x3f   // datarate = 100 Hz, Cutoff = 25, Enable = 1, Xaxis = 1, Yaxis = 1, Zaxis = 1 
#define REG_2_CONFIG 0x00   // Highpass filter mode: Normal, Highpass cutoff: 8Hz
#define REG_4_CONFIG 0x10   // Endianess = DataLSB, Fullscale = 500, SelfTest = Normal, SPI = 4 wiremode
#define REG_5_CONFIG 0x10   // Boot: Normal mode, FIFO_EN: disabled, HPen: HPF enabled, No interrupt

#define REG_1_ADDRESS  0x20
#define REG_2_ADDRESS  0x21
#define REG_4_ADDRESS  0x23
#define REG_5_ADDRESS  0x24
#define ID_REG_ADDRESS 0x0F
#define X_REG_ADDRESS  0x28
#define Y_REG_ADDRESS  0x2A
#define Z_REG_ADDRESS  0x2C

#define READ_CMD         0x80
#define MULTIPLEBYTE_CMD 0x40
#define DUMMY_BYTE       0x00

#define FS_500_SENSITIVITY 0.0175 // sensitivity from the datasheet

#define MAX_GYRO 500
#define MIN_GYRO -500