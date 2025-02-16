#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>  // if using PCA9685 driver

#define SERVOMIN  150  
#define SERVOMAX  600  

// If using Adafruit PCA9685:
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void write(uint8_t servo, int angle) {
  // Map [0..180] -> [SERVOMIN..SERVOMAX]
  pwm.setPWM(servo, 0, map(angle, 0, 180, SERVOMIN, SERVOMAX));
}

//------------------------------------
// Timing
//------------------------------------
unsigned long lastUpdate = 0; 
const int updateInterval = 20;  // ~50 Hz

//------------------------------------
// Setup
//------------------------------------
void setup() {
  // If using PCA9685:
  // Wire.begin();
  pwm.begin();
  pwm.setPWMFreq(50); 

  // Initialize legs to a neutral-ish stand
  // You can also immediately set them to the offset centers below
  write(2,  90);
  write(3,  90);
  write(12, 90);
  write(13, 90);
  write(0,  90);
  write(1,  90);
  write(14, 90);
  write(15, 90);

  delay(1000);
}

//------------------------------------
// Loop
//------------------------------------
void loop() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpdate >= updateInterval) {
    lastUpdate = currentTime;
    catWalk();
  }
}

//------------------------------------
// catWalk()
//------------------------------------
void catWalk() {
  float t = millis() / 1000.0;

  // Increase the frequency for a faster walk
  float freq = 1.0;     // 1 Hz = 1 cycle per second; try ~1.0..1.5
  // Shoulder amplitude
  float ampShoulder = 30; 
  // Knee amplitude
  float ampKnee = 30;   

  //
  // OFFSET CENTERS:
  // We'll shift the front shoulders forward (~80°)
  // and the back shoulders backward (~100°).
  // Knees can remain around 90°, but feel free to tweak.
  //
  float frontShoulderCenter = 80;  
  float frontKneeCenter     = 90;  
  float backShoulderCenter  = 100; 
  float backKneeCenter      = 90;  

  //
  // PHASES FOR A 4-BEAT WALK (each leg 90° out of phase):
  //   LB -> phase = 0
  //   LF -> phase = π/2
  //   RB -> phase = π
  //   RF -> phase = 3π/2
  //
  float LB_phase = 0.0;              
  float LF_phase = 3.14159 * 0.5;    
  float RB_phase = 3.14159;          
  float RF_phase = 3.14159 * 1.5;    

  // Sine cycles
  float cycleLB = sin(2.0 * 3.14159 * freq * t + LB_phase);
  float cycleLF = sin(2.0 * 3.14159 * freq * t + LF_phase);
  float cycleRB = sin(2.0 * 3.14159 * freq * t + RB_phase);
  float cycleRF = sin(2.0 * 3.14159 * freq * t + RF_phase);

  // Optional: offset knees by +π/2 so they bend at a different time
  float kneeOffset = 3.14159 / 2;

  //------------------------------------
  // LEFT BACK LEG (2 = shoulder, 3 = knee)
  //------------------------------------
  // " + angle = cc" => may need to invert if your servo is reversed
  float LB_shoulder = backShoulderCenter + ampShoulder * cycleLB;
  float LB_knee     = backKneeCenter     + ampKnee     * sin(2.0 * 3.14159 * freq * t + LB_phase + kneeOffset);

  //------------------------------------
  // LEFT FRONT LEG (0 = shoulder, 1 = knee)
  //------------------------------------
  float LF_shoulder = frontShoulderCenter + ampShoulder * cycleLF;
  float LF_knee     = frontKneeCenter     + ampKnee     * sin(2.0 * 3.14159 * freq * t + LF_phase + kneeOffset);

  //------------------------------------
  // RIGHT BACK LEG (12 = shoulder, 13 = knee)
  //------------------------------------
  // If your notes say " - angle = cc" for the right side, 
  // then you might do 'backShoulderCenter - ampShoulder * cycleRB'.
  // For now let's keep symmetrical and see if it yields forward motion.
  float RB_shoulder = backShoulderCenter + ampShoulder * cycleRB;
  float RB_knee     = backKneeCenter     + ampKnee     * sin(2.0 * 3.14159 * freq * t + RB_phase + kneeOffset);

  //------------------------------------
  // RIGHT FRONT LEG (14 = shoulder, 15 = knee)
  //------------------------------------
  float RF_shoulder = frontShoulderCenter + ampShoulder * cycleRF;
  float RF_knee     = frontKneeCenter     + ampKnee     * sin(2.0 * 3.14159 * freq * t + RF_phase + kneeOffset);

  //------------------------------------
  // WRITE THE ANGLES
  //------------------------------------
  write(2,  LB_shoulder);
  write(3,  LB_knee);

  write(0,  LF_shoulder);
  write(1,  LF_knee);

  write(12, RB_shoulder);
  write(13, RB_knee);

  write(14, RF_shoulder);
  write(15, RF_knee);
}