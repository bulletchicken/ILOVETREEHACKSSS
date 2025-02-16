#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // Typically needed if using PCA9685

#define SERVOMIN  150  // 'minimum' pulse length count (example)
#define SERVOMAX  600  // 'maximum' pulse length count (example)

// If using Adafruit PCA9685:
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Custom write function
void write(uint8_t servo, int angle) {
  // Map [0..180] degrees to [SERVOMIN..SERVOMAX]
  pwm.setPWM(servo, 0, map(angle, 0, 180, SERVOMIN, SERVOMAX));
}

//----------------------------------------------------------
// TIMING
//----------------------------------------------------------
unsigned long lastUpdate = 0; 
const int updateInterval = 20;  // ~50 Hz servo update

//----------------------------------------------------------
// SETUP
//----------------------------------------------------------
void setup() {
  Serial.begin(9600);
  // If using Adafruit PCA9685:
  // Wire.begin();
pwm.begin();
  pwm.setPWMFreq(50); // 50Hz for standard servos

  write(0, 20);
  write(15, 160);
  write(2, 20);
  write(13, 160);
  
}

//----------------------------------------------------------
// LOOP
//----------------------------------------------------------
void loop() {
  Serial.println("Type to begin"); // Changed print to println to make message visible
  
  // Wait for input before proceeding
  while (!Serial.available()) {
    delay(100); // Small delay to prevent overwhelming the processor
  }

  char command = Serial.read();
  Serial.println(command); // Debug print to see what command is received
  
  if (command == '1') {
    Serial.println("Standing up..."); // Debug message
    
    delay(2000);
    
    // Return to neutral walking position
    write(2, 90);  // Left Back Shoulder
    write(3, 90);  // Left Back Knee
    write(12, 90); // Right Back Shoulder
    write(13, 90); // Right Back Knee
    write(0, 90);  // Left Front Shoulder
    write(1, 90);  // Left Front Knee
    write(14, 90); // Right Front Shoulder
    write(15, 90); // Right Front Knee

    delay(1000);

    unsigned long startTime = millis();
    while (millis() - startTime < 9000) { // Run for 9 seconds
      unsigned long currentTime = millis();
      if (currentTime - lastUpdate >= updateInterval) {
        lastUpdate = currentTime;
        catWalk();
      }
    }

    write(2, 90);  // Left Back Shoulder
    write(3, 90);  // Left Back Knee
    write(12, 90); // Right Back Shoulder
    write(13, 90); // Right Back Knee
    write(0, 90);  // Left Front Shoulder
    write(1, 90);  // Left Front Knee
    write(14, 90); // Right Front Shoulder
    write(15, 90); // Right Front Knee

    lookAround();
  }

  

  if (command == '2') {
    lookAround();
    Serial.println("Wagging tail slowly..."); // Debug message
    // Wag tail back and forth several times with slower movement
    for(int i = 0; i < 4; i++) {
      write(8, 90); // Tail right
      delay(500);    // Longer delay for slower movement
      write(8, 50);  // Tail left 
      delay(500);    // Longer delay for slower movement
    }
    write(8, 90);    // Return to center

    
  }
  
  // Clear any remaining characters in the serial buffer
  while(Serial.available() > 0) {
    Serial.read();
  }


  // }
}

//----------------------------------------------------------
// catWalk(): Four-beat walk cycle
//----------------------------------------------------------

void strech() {
    for(int angle = 90; angle <= 110; angle += 1) {
        write(0, angle);
        write(1, angle);
        write(2, angle);
        write(3, angle);
        write(12, angle);
        write(13, angle);
        write(14, angle);
        write(15, angle);
        delay(200);
    }

    for(int angle = 110; angle >= 90; angle -= 1) {
        write(0, angle);
        write(1, angle);
        write(2, angle);
        write(3, angle);
        write(12, angle);
        write(13, angle);
        write(14, angle);
        write(15, angle);
        delay(50);
    }
}

void lookAround() {
        for(int angle = 90; angle <= 120; angle += 2) {
        write(6, angle);
        delay(50);
    }
    
    // Slowly turn head right 
    for(int angle = 120; angle >= 50; angle -= 2) {
        write(6, angle);
        delay(50);
    }
    
    // Return to center
    for(int angle = 50; angle <= 70; angle += 2) {
        write(6, angle);
        delay(50);
    }
}

void catWalk() {
  float t = millis() / 1000.0;

  // Frequency of the step cycle in Hz
  float freq = 0.7;       // Slower frequency for demonstration
  // Amplitude of the shoulder joint swing
  float ampShoulder = 30; // Try ~15..30 for smaller/bigger steps
  // Amplitude of the knee bend
  float ampKnee     = 30; // Try smaller so it lifts foot gently

  // For a 4-beat walk, each leg typically is out of phase by 90 degrees (π/2).
  // We'll define the order as:
  //   1) Left Back (LB)  ->  phase 0
  //   2) Left Front (LF) ->  phase π/2
  //   3) Right Back (RB) ->  phase π
  //   4) Right Front (RF)->  phase 3π/2
  //
  // You can re-order if you prefer a different stepping sequence.
  float LB_phase = 0.0;          
  float LF_phase = 3.14159 * 0.5; 
  float RB_phase = 3.14159;       
  float RF_phase = 3.14159 * 1.5; 

  // Calculate the sinusoidal values
  float cycleLB = sin(2.0 * 3.14159 * freq * t + LB_phase);
  float cycleLF = sin(2.0 * 3.14159 * freq * t + LF_phase);
  float cycleRB = sin(2.0 * 3.14159 * freq * t + RB_phase);
  float cycleRF = sin(2.0 * 3.14159 * freq * t + RF_phase);

  //--------------------------------------------------
  // LEFT BACK LEG (channels 2=shoulder, 3=knee)
  //--------------------------------------------------
  // Shoulder: 90 +/- amplitude
  float LB_shoulder = 90 + ampShoulder * cycleLB;
  // Knee offset ~ + π/2, so it flexes the knee “behind” the shoulder motion
  float LB_knee     = 90 + ampKnee * sin(2.0 * 3.14159 * freq * t + LB_phase + 3.14159/2);

  //--------------------------------------------------
  // LEFT FRONT LEG (channels 0=shoulder, 1=knee)
  //--------------------------------------------------
  float LF_shoulder = 90 + ampShoulder * cycleLF;
  float LF_knee     = 90 + ampKnee * sin(2.0 * 3.14159 * freq * t + LF_phase + 3.14159/2);

  //--------------------------------------------------
  // RIGHT BACK LEG (channels 12=shoulder, 13=knee)
  //--------------------------------------------------
  // According to your sign notes: " - angle = cc "
  // If you need to invert, you can do 90 - amplitude * ...
  // But let's just keep symmetrical for now. If it's reversed, invert them.
  float RB_shoulder = 90 + ampShoulder * cycleRB;
  float RB_knee     = 90 + ampKnee * sin(2.0 * 3.14159 * freq * t + RB_phase + 3.14159/2);

  //--------------------------------------------------
  // RIGHT FRONT LEG (channels 14=shoulder, 15=knee)
  //--------------------------------------------------
  float RF_shoulder = 90 + ampShoulder * cycleRF;
  float RF_knee     = 90 + ampKnee * sin(2.0 * 3.14159 * freq * t + RF_phase + 3.14159/2);

  //--------------------------------------------------
  // WRITE THE ANGLES
  //--------------------------------------------------
  write(2,  LB_shoulder); 
  write(3,  LB_knee);

  write(0,  LF_shoulder);
  write(1,  LF_knee);

  write(12, RB_shoulder);
  write(13, RB_knee);

  write(14, RF_shoulder);
  write(15, RF_knee);
}