#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 50
#define NUM_SERVOS 2
#define MIN 110
#define MAX 530

String inputString = "";
boolean stringComplete = false;
int xAngle = 90;  // Default center position
int yAngle = 90;  // Default center position

uint16_t angleToPulse(int servo, float angle) {
    return map(angle, 0, 180, MIN, MAX);
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    pwm.begin();
    pwm.setPWMFreq(SERVO_FREQ);

    // Initialize servos to center position
    pwm.setPWM(6, 0, angleToPulse(6, 90));
    pwm.setPWM(7, 0, angleToPulse(7, 90));
}

void loop() {
    // Read serial input
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        if (inChar == '\n') {
            stringComplete = true;
        } else {
            inputString += inChar;
        }
    }

    // Process the received angles
    if (stringComplete) {
        // Parse the comma-separated values
        int commaIndex = inputString.indexOf(',');
        if (commaIndex != -1) {
            xAngle = inputString.substring(0, commaIndex).toInt();
            yAngle = inputString.substring(commaIndex + 1).toInt();
            
            // Constrain angles to prevent servo overtravel
            xAngle = constrain(xAngle, 0, 180);
            yAngle = constrain(yAngle, 0, 180);

            // Move servos
            pwm.setPWM(6, 0, angleToPulse(6, xAngle));
            pwm.setPWM(7, 0, angleToPulse(7, yAngle));

            // Debug output
            Serial.print("X Angle: "); Serial.print(xAngle);
            Serial.print(" | Y Angle: "); Serial.println(yAngle);
        }

        // Clear the string for next input
        inputString = "";
        stringComplete = false;
    }

    delay(1); // Small delay for smooth movement
}