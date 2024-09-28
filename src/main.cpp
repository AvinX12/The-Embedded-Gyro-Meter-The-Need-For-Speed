// Header files
#include <mbed.h>
#include "math.h"

// Custom header files interfacing with gyroscope, LCD on the STM32F429I Discovery board
#include "gyroscope.h"
#include "configuration.h"
#include "./drivers/LCD_DISCO_F429ZI.h"

// LCD object
LCD_DISCO_F429ZI lcd;
static BufferedSerial serial_port(USBTX, USBRX);

// Define onboard LED
DigitalOut led1(LED1);
DigitalOut led2(LED2);

// Sampling rate: every 0.5 seconds for 20 seconds
// 1 data sample every 0.5 sec: 40 data samples altogether

// Values are in angular velocity (dps)
float angVelData[40][3];
// Values after converted to linear velocity
float linVelData[40][3];

// To know number of intervals - TRACKER
int Index = 0;

// Total distance covered by the user in 20 seconds
double totalDistanceCovered;

// Initialize Gyroscope function
int Gyro_Init();

// Getting X, Y, Z axis values from the gyroscope
void Gyro_Get_XYZ(float xyz[]);

void setupGyro() {
    
    // Master Out Slave In for SPI  : PF_9
    // Master In Slave Out for SPI  : PF_8
    // Clock for SPI                : PF_7
    // Chip select for SPI          : PC_1
    // All the above data, and similar data are set in configuration.h file

    // Transfer 8 bits, mode 3 - read starting at rising edge (default is 0)
    spi.format(8,3);
    // 1 MHz
    spi.frequency(1'000'000);

    // Initializing the gyroscope and storing it's ID
    int Gyro_ID = Gyro_Init();

    printf("Gyro_ID: %d\n", Gyro_ID);

}

void processGyroData() {

    // To store 3 bytes X, Y, Z
    float GyroXYZ[3]; 
    // To store linear velocity for a interval
    float linVel[3];

    Gyro_Get_XYZ(GyroXYZ);

    // Avoid un-intentional irregualr data
    if(GyroXYZ[0] > MAX_GYRO || GyroXYZ[0] < MIN_GYRO)
        GyroXYZ[0] = 0;
    if(GyroXYZ[1] > MAX_GYRO || GyroXYZ[1] < MIN_GYRO)
        GyroXYZ[1] = 0;
    if(GyroXYZ[2] > MAX_GYRO || GyroXYZ[2] < MIN_GYRO)
        GyroXYZ[2] = 0;
    
    // Linear Velocity: X, Y, Z (0.5s interval)
    // Initially considering it as '0'
    if(Index == 0) {
        linVel[0] = 0;
        linVel[1] = 0;
        linVel[2] = 0;
     } 
    else { // Calculating linear velocity
        // Linear Velocity = Angular Velocity * Radius
        linVel[0] = (angVelData[Index - 1][0] - GyroXYZ[0]) * (RADIUS_X * 0.001);
        linVel[1] = (angVelData[Index - 1][1] - GyroXYZ[1]) * (RADIUS_Y * 0.001);
        linVel[2] = (angVelData[Index - 1][2] - GyroXYZ[2]) * (RADIUS_Z * 0.001);
     }
            
    if(Index < 40) {
        // Accumulate data till 20s
        angVelData[Index][0] = GyroXYZ[0];
        angVelData[Index][1] = GyroXYZ[1];
        angVelData[Index][2] = GyroXYZ[2];

        linVelData[Index][0] = linVel[0];
        linVelData[Index][1] = linVel[1];
        linVelData[Index][2] = linVel[2];
    }

}

// Function to initialize the LCD on-board
void initializeLCD() {
    
    lcd.Clear(LCD_COLOR_WHITE);
    
    BSP_LCD_SetFont(&Font16);
    lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"The Need For Speed", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(2), (uint8_t *)"Embedded Challenge", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"Fall 2023 - Group 29", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"--------------------", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)"ka3210, tx701", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(8), (uint8_t *)"dk4852, hs5472", CENTER_MODE);
    
    BSP_LCD_SetFont(&Font24);
    lcd.DisplayStringAt(0, LINE(8), (uint8_t *)"TIME COUNT", CENTER_MODE);
    wait_us(1000000);
    lcd.DisplayStringAt(0, LINE(10), (uint8_t *)"0 s", CENTER_MODE);

}

// Function to display real-time time count on LCD
void displayTimeOnLCD() {

    char time_display[10] = {0};
    int time_int = (int)(Index * 0.5);
    sprintf(time_display, "%d s", time_int);
    lcd.ClearStringLine(16);
    lcd.DisplayStringAt(0, LINE(10), (uint8_t *)time_display, CENTER_MODE);

}

// Function to display the distance travelled on LCD
void displayDistanceOnLCD(double totalDistanceCovered) {

    char distance_display[25] = {0};
    int distance_int = (int)totalDistanceCovered * 0.9; // scaling for better accuracy 
    
    lcd.Clear(LCD_COLOR_YELLOW);
    
    BSP_LCD_SetFont(&Font16);
    lcd.DisplayStringAt(0, LINE(1), (uint8_t *)"Press RESET button", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(2), (uint8_t *)"to record again", CENTER_MODE);
    
    BSP_LCD_SetFont(&Font24);
    lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"DISTANCE", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"TRAVELLED", CENTER_MODE);
    sprintf(distance_display, "%d m", distance_int);
    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)distance_display, CENTER_MODE);
    
    BSP_LCD_SetFont(&Font20);
    int time_int = (int)(Index * 0.5);
    char time_display[14] = {0};
    sprintf(time_display, "In %d seconds", time_int);
    lcd.DisplayStringAt(0, LINE(14), (uint8_t *)time_display, CENTER_MODE);

}

// Function to calculate the linear velocity and totol distance
void calculateDistance() {
    
    for(int i = 1; i < Index; i++) {
        float x_dist, y_dist, z_dist;
        
        // Distance = Linear Velocity * Time Frequency
        x_dist = linVelData[i][0] * 0.5;
        y_dist = linVelData[i][1] * 0.5;
        z_dist = linVelData[i][2] * 0.5;
        
        //calculate distance travelled in 0.5s interval
        double dist = sqrt((x_dist * x_dist) + (y_dist * y_dist) + (z_dist * z_dist));

        totalDistanceCovered += dist;
    }

    // Calibration
    totalDistanceCovered -= 0.035; // offset
            
    if(totalDistanceCovered < 0)
        totalDistanceCovered = 0;
            
    totalDistanceCovered /= 0.165; // scaling
            
    printf("Total Distance Covered = %f\n", totalDistanceCovered);

    // Display it on LCD
    displayDistanceOnLCD(totalDistanceCovered);

}

int main() {

    // Initializing the Gyroscope
    setupGyro();

    // Initializing the LCD
    initializeLCD();

    while(1) {

        // Process the data from Gyroscope    
        processGyroData();

        // Display time elapsed on LCD screen
        if(Index < 40) {
            displayTimeOnLCD();
            led1 = !led1;  // Blick on-board LED1 while recording distance
            led2 = 0;  // Keeping the LED2 turned OFF
        }

        // Calculating the distance after 20 sec
        if (Index == 40) {
            calculateDistance();
            led1 = 1;  // Keeping the LED1 turned ON after 20 sec
            led2 = 1;  // Keeping the LED2 turned ON after 20 sec
        }

        Index++;

        // Waiting 0.5 sec to sample data
        wait_us(500000);

    }

    return 0;

}