#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>
#include <Adafruit_PWMServoDriver.h>

MPU6050 mpu;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 50
#define NUM_SERVOS 2  // Define number of servos
#define MIN 110
#define MAX 530

bool dmpReady = false;
uint8_t mpuIntStatus;    
uint8_t devStatus;       
uint16_t packetSize;    
uint8_t fifoBuffer[64];  
Quaternion q;           
VectorFloat gravity;    
float ypr[3];           

volatile bool mpuInterrupt = false;
void dmpDataReady() {
    mpuInterrupt = true;
}

uint16_t angleToPulse(int servo, float angle) {
    return map(angle, 0, 180, MIN, MAX);
}

void setup() {
    Serial.begin(9600);
    Wire.begin();
    
    Serial.println("Initializing MPU6050...");
    mpu.initialize();
    
    pwm.begin();
    pwm.setPWMFreq(SERVO_FREQ);

    // Initialize servos to center position
    pwm.setPWM(6, 0, angleToPulse(6, 90));
    pwm.setPWM(7, 0, angleToPulse(7, 90));
    

    // Initialize DMP
    devStatus = mpu.dmpInitialize();


    
    // Set gyro offsets here if needed
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788);
    
    mpu.CalibrateAccel(10);
    mpu.CalibrateGyro(10);

    Serial.println(devStatus);
    if (devStatus == 0) {
        mpu.setDMPEnabled(true);
        attachInterrupt(digitalPinToInterrupt(2), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        Serial.print("DMP Initialization failed (code ");
        Serial.print(devStatus);
        Serial.println(")");
        while (1);
    }
}

void loop() {
    if (!dmpReady) return;

    // Wait for MPU interrupt or extra packet(s) available
    if (!mpuInterrupt && mpu.getFIFOCount() < packetSize) return;

    // Reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = mpu.getIntStatus();

    // Get current FIFO count
    uint16_t fifoCount = mpu.getFIFOCount();

    // Check for overflow (this should never happen unless our code is too inefficient)
    if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        mpu.resetFIFO();
        Serial.println("FIFO overflow!");
        return;
    }

    // Wait for correct available data length
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    
    // Read a packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

    // Convert Yaw & Roll from radians to degrees
    float yaw = ypr[0] * 180.0 / M_PI;
    float roll = -ypr[2] * 180.0 / M_PI;  // Negated roll value to reverse direction

    // Map Yaw & Roll to Servo Angles (0° - 180°)
    int rollAngle = map(roll, -90, 90, 0, 180);
    int yawAngle = map(yaw, -90, 90, 0, 180);
    
    // Constrain angles to prevent servo overtravel
    rollAngle = constrain(rollAngle, 0, 180);
    yawAngle = constrain(yawAngle, 0, 180);

    // Move servos (swapped servo numbers)
    pwm.setPWM(6, 0, angleToPulse(6, yawAngle));    // Yaw now controls servo 6
    pwm.setPWM(7, 0, angleToPulse(7, rollAngle));   // Roll now controls servo 7

    // Debug Output
    Serial.print("Yaw: "); Serial.print(yaw);
    Serial.print(" | Roll: "); Serial.print(roll);   // Changed from Pitch to Roll
    Serial.print(" | Servo1 (Yaw): "); Serial.print(yawAngle);    // Updated debug labels
    Serial.print(" | Servo2 (Roll): "); Serial.println(rollAngle);

    delay(1); // Small delay for smooth movement
}