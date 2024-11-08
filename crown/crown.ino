#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 3
#define MAX 255
#define MIN 50
#define RATE 40
#define CONTROLLERS 5

double rel_brightness[CONTROLLERS*3] = {1,1,0.9727472238,0.3311311215,0.2466039337,0.2398832919,0.201372425,0.1923091729,0.1570362804,0.1541700453,0.1472312502,0.1330454418,0.1294195841,0.1235947433,0.1235947433};

Adafruit_NeoPixel stars = Adafruit_NeoPixel(CONTROLLERS, PIN, NEO_GRB + NEO_KHZ800);
int starlight[CONTROLLERS][3];

void setup() {
  //Serial.begin(9600);
  //Serial.println("setup");
  randomSeed(analogRead(0));
  for(int i = 0; i < CONTROLLERS; i++){
    for(int j = 0; j < 3; j++){
      starlight[i][j] = 0;
    }
  }
  stars.begin();
  stars.show(); // Initialize all pixels to 'off'
  fadeIn();
}

void loop() {
  fadeToRandom();  
}

void sequentialStars(){
  for(int i = 0; i < CONTROLLERS*3; i++){
    //Serial.print("Star #: ");
    //Serial.println(i);
    setStar(i, MAX*rel_brightness[i]);
    stars.show();
    delay(500);
  }  
}

void fadeToRandom(){
  int target[CONTROLLERS*3];
  int numSteps = 50;
  for(int s = 0; s < CONTROLLERS*3; s++){
    target[s]= random(MIN*rel_brightness[s],MAX*rel_brightness[s]);
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
    stars.show();
    delay(RATE);
  }
  
}

void fadeIn(){
  int target[CONTROLLERS*3];
  int numSteps = 60;
  for(int s = 0; s < CONTROLLERS*3; s++){
    target[s]= random(MIN*rel_brightness[s],MAX*rel_brightness[s]);
  }
  for(int steps = 1; steps <= numSteps; steps++){
    for(int star = 0; star < CONTROLLERS*3; star++){
      setStar(star, target[star]/numSteps*steps);
    }
    stars.show();
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
  stars.setPixelColor(driver, starlight[driver][0], starlight[driver][1], starlight[driver][2]);
}