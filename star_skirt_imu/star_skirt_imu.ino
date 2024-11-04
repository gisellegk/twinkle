#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <SimpleIMU.h>
SimpleIMU mpu(0x68);

int MAX = 0;
int RATE = 20;

#define DEFAULT_RATE 20
#define DEFAULT_MAX 100

#define PIN1 8
#define CONTROLLERS1 20

#define PIN2 16
#define CONTROLLERS2 20

#define CONTROLLERS (CONTROLLERS1 + CONTROLLERS2)

Adafruit_NeoPixel stars1 = Adafruit_NeoPixel(CONTROLLERS1, PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stars2 = Adafruit_NeoPixel(CONTROLLERS2, PIN2, NEO_GRB + NEO_KHZ800);
int starlight[CONTROLLERS][3];

GyroData gyro;
int shimmer = 0;
int twinkle = 0;
#define SHIMMER_LEN 20
#define TWINKLE_LEN 400

void setup() {
  Serial.begin(9600);
  Serial.println("setup");

  // Serial.println(CONTROLLERS);
  while (!mpu.init())
	{
		Serial.println("MPU initialization failed. Please check your wiring.");
		delay(1000);
	}
	Serial.println("MPU initialized successfully!");
  randomSeed(analogRead(0));
  for(int i = 0; i < CONTROLLERS; i++){
    for(int j = 0; j < 3; j++){
      starlight[i][j] = 0;
    }
  }
  stars1.begin();
  stars2.begin();
  stars1.show(); // Initialize all pixels to 'off'
  stars2.show(); // Initialize all pixels to 'off'

}

void loop() {
  if(MAX > 0) {
    fadeToRandom();
    if(shimmer) {
      shimmer--;
      Serial.println("shimmer");
    }
    else if(RATE < DEFAULT_RATE) RATE++;
    else if(twinkle) twinkle--;
    else MAX-=5;
    Serial.print("shimmer: ");
    Serial.print(shimmer);
    Serial.print("MAX: ");
    Serial.print(MAX);
    Serial.print(", RATE: ");
    Serial.println(RATE);

  } else {
    delay(100);
    fadeToRandom();
    Serial.println("idle");
  }
  mpu.readGyro(&gyro);
  Serial.print(abs(gyro.x));
  Serial.print(" ");
  Serial.println(abs(gyro.y));
  Serial.println();
  if(((abs(gyro.x) + abs(gyro.y)) > 200) && MAX <= 0) {
    Serial.print(abs(gyro.x + gyro.y));
    Serial.println("initializing...");
    if(MAX == 0) fadeIn();
    MAX = DEFAULT_MAX;
    RATE = 0;
    

    shimmer = SHIMMER_LEN;
    twinkle = TWINKLE_LEN;
    
  }
}

void sequentialStars(){
  for(int i = 0; i < CONTROLLERS*3; i++){
    //Serial.print("Star #: ");
    //Serial.println(i);
    setStar(i, MAX);
    stars1.show();
    stars2.show();
    delay(500);
  }  
}

void fadeToRandom(){
  int target[CONTROLLERS*3];
  int numSteps = 50;
  for(int s = 0; s < CONTROLLERS*3; s++){
    target[s]= random(0,MAX);
    //if(s==8){Serial.print("set target: "); Serial.println(target[s]);}
  }

  for(int steps = 1; steps <= numSteps; steps++){
    for(int star = 0; star < CONTROLLERS*3; star++){
      int t = target[star];
      int a = starlight[getStarDriver(star)][getStarIndex(star)];

      if(t > a){
        int diff = t - a;
        setStar(star, a + diff/numSteps*steps +1);
        /*if(star==8){
          Serial.print("up ");
          Serial.println(a + diff/numSteps*steps +1);
        }*/
      } else if (t < a){
        int diff = a - t; 
        setStar(star, a - diff/numSteps*steps -1);
        /*if(star==8){
          Serial.print("down ");
          Serial.println(a - diff/numSteps*steps -1);
        }*/
      } else {
        //idk do nothing?
        //if(star==8)Serial.print("?");
      }

    }
    stars1.show();
    stars2.show();
    delay(RATE);
  }
  
}

void fadeIn(){
  int target[CONTROLLERS*3];
  int numSteps = 60;
  for(int s = 0; s < CONTROLLERS*3; s++){
    target[s]= random(0,MAX);
  }

  for(int steps = 1; steps <= numSteps; steps++){
    for(int star = 0; star < CONTROLLERS*3; star++){
      // Serial.print("setStar: ");
      // Serial.print(star, " ");
      // Serial.println(steps);
      setStar(star, target[star]/numSteps*steps);
    }
    stars1.show();
    stars2.show();
    delay(RATE);
  }
  
}

int getStarDriver(int num){
  return (num/3)%CONTROLLERS;
}
int getStarIndex(int num){
  return num%3;
}

void setStar(int num, int val) {
  int driver = getStarDriver(num); // 0-5 
  int index = getStarIndex(num); // 0-2
  starlight[driver][index] = val;
  // Serial.println(driver);
  if(driver < CONTROLLERS1) {
    // Serial.println("stars1");
    stars1.setPixelColor(driver, starlight[driver][0], starlight[driver][1], starlight[driver][2]);
  }
  else {
    driver = driver-CONTROLLERS1;
    stars2.setPixelColor(driver, starlight[driver][0], starlight[driver][1], starlight[driver][2]);
    // Serial.println("stars2");
  }
}