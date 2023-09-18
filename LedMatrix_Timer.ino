// Including the required Arduino libraries
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <EEPROM.h>

// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

// Defining size, and output pins
#define MAX_DEVICES 4
#define CS_PIN 3
#define MAX_ZONES 2

//D 11
//CS 3
//CLK 13

#define START_TIME 10

#define trigPin 6
#define echoPin 7

#define b_up 8
#define b_down 9

#define buzzerPin 2

// Create a new instance of the MD_Parola class with hardware SPI connection
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

int TIME_A = 30;

double wait = millis();
int timeA = TIME_A;
int timeB = 60 - TIME_A;
int reps = 1;
int stop = 0;

int fase = 0;   //stato della macchina a stati

long durata, cm;

int nota = 600;
int durata_nota = 220;

void setup() {
  delay(1000);
	// Intialize the object
	myDisplay.begin(MAX_ZONES);
  myDisplay.setZone(0, 0, 1);
  myDisplay.setZone(1, 2, 3);

  myDisplay.setIntensity(0,0);

	// Set the intensity (brightness) of the display (0-15)
	myDisplay.setIntensity(1,4);

	// Clear the display
	myDisplay.displayClear();

  for(int i =0;i<START_TIME;i++)
    inizio(START_TIME - i);

  // sensore sonico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(b_up, INPUT_PULLUP);
  pinMode(b_down, INPUT_PULLUP);

  pinMode(buzzerPin, INPUT);

  TIME_A = EEPROM.read(0);
  if(TIME_A == 0)
    TIME_A = 30;
  timeA = TIME_A;
  timeB = 60 - TIME_A;
}



void inizio(int i){
  char t_str[8];
  itoa( i, t_str, 10 );
  myDisplay.displayZoneText(0, t_str, PA_CENTER, 100, 0, PA_NO_EFFECT, PA_NO_EFFECT);

  

  if (myDisplay.displayAnimate())
  {    
      
    
    for (uint8_t i=0; i<MAX_ZONES; i++)
    {
      if (myDisplay.getZoneStatus(i))
      {
        // do something with the parameters for the animation then reset it
        myDisplay.displayReset(i);
      }
    }
  }
  if(i <= 3){
    pinMode(buzzerPin, OUTPUT);
    tone(buzzerPin, nota, durata_nota);
    delay(durata_nota);
    noTone(buzzerPin);
    delay(1000-durata_nota);
  }
  else if(i == 0){
    delay(100);
    tone(buzzerPin, nota, durata_nota);
    delay(durata_nota);
    noTone(buzzerPin);
    delay(900-durata_nota);
  }
  else{
    delay(1000);
  }

}

void allena(){

  if(millis() - wait >= 1000){
    wait = millis();
    
    String reps_s = String(reps)+"s";

    myDisplay.displayZoneText(1, reps_s.c_str(), PA_CENTER, 100, 0, PA_NO_EFFECT, PA_NO_EFFECT); 

    char t_str[8];
    itoa( timeA, t_str, 10 );

    myDisplay.displayZoneText(0, t_str, PA_CENTER, 100, 0, PA_NO_EFFECT, PA_NO_EFFECT);

    if (myDisplay.displayAnimate())
    {    
       
      
      for (uint8_t i=0; i<MAX_ZONES; i++)
      {
        if (myDisplay.getZoneStatus(i))
        {
          // do something with the parameters for the animation then reset it
          myDisplay.displayReset(i);
        }
      }
    }

    if(timeA == TIME_A){
      suona2();    
    }else{
      suona();
    }

    
    pinMode(buzzerPin, INPUT);  
    
    timeA--;

    if(timeA <= 0){
      timeA = TIME_A;
      timeB = 60-TIME_A;
      fase = 1;
    }
  }
  
}

void riposa(){
  

  

  if(millis() - wait >= 1000){
    wait = millis();
    
    String reps_s = String(reps)+"r";

    myDisplay.displayZoneText(1, reps_s.c_str(), PA_CENTER, 100, 0, PA_NO_EFFECT, PA_NO_EFFECT); 

    char t_str[8];
    itoa( timeB, t_str, 10 );

    myDisplay.displayZoneText(0, t_str, PA_CENTER, 100, 0, PA_NO_EFFECT, PA_NO_EFFECT);

    if (myDisplay.displayAnimate())
    {    
       
      
      for (uint8_t i=0; i<MAX_ZONES; i++)
      {
        if (myDisplay.getZoneStatus(i))
        {
          // do something with the parameters for the animation then reset it
          myDisplay.displayReset(i);
        }
      }
    }
    if(timeB == 60-TIME_A){
      suona2();    
    }else{
      suona();      
    }

    
    pinMode(buzzerPin, INPUT);

    timeB--;

    if(timeB <= 0){
      timeA = TIME_A;
      timeB = 60-TIME_A;
      fase = 0;

      reps += 1;
    }
  }
}

void controllo_stop(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  durata = pulseIn(echoPin, HIGH, 100000);
  cm = durata / 58; // per i pollici la formula è durata / 148;
  if(cm > 5 && cm < 20){
    stop = stop *-1 +1;
    delay(1000);
  }
}

void controllo_pulsanti(){
  int timeout = 0;
  while(timeout < 25){
    if(digitalRead(b_up) == LOW && TIME_A < 60){
      TIME_A += 1;
      timeout = 0;
    }
    if(digitalRead(b_down) == LOW && TIME_A > 0){
      TIME_A -= 1;
      timeout = 0;
    }    
    delay(200);
    timeout += 1;

    char str[8];
    itoa( TIME_A, str, 10 );

    myDisplay.displayZoneText(0, str, PA_CENTER, 100, 0, PA_NO_EFFECT, PA_NO_EFFECT);

    char t_str[8];
    itoa( (5-(timeout / 5)), t_str, 10 );
    myDisplay.displayZoneText(1, t_str, PA_CENTER, 100, 0, PA_NO_EFFECT, PA_NO_EFFECT); 

    if (myDisplay.displayAnimate())
    {    
       
      
      for (uint8_t i=0; i<MAX_ZONES; i++)
      {
        if (myDisplay.getZoneStatus(i))
        {
          // do something with the parameters for the animation then reset it
          myDisplay.displayReset(i);
        }
      }
    }
  }
  EEPROM.update(0, TIME_A);
}



void suona(){
  noTone(buzzerPin);
  if(timeA <= 3 || timeB <= 3){
    pinMode(buzzerPin, OUTPUT);
    tone(buzzerPin, nota, durata_nota);
    delay(durata_nota);
    noTone(buzzerPin);
  }
}

void suona2(){
  noTone(buzzerPin);
  pinMode(buzzerPin, OUTPUT);
  tone(buzzerPin, nota, durata_nota);
  delay(durata_nota);
  noTone(buzzerPin);
  delay(100);
  noTone(buzzerPin);
  pinMode(buzzerPin, OUTPUT);
  tone(buzzerPin, nota, durata_nota);
  delay(durata_nota);
  noTone(buzzerPin);
}

void loop() {

  if(fase == 0 && stop == 0){
    allena();
  }

  if(fase == 1 && stop == 0){
    riposa();
  }

  controllo_stop();
  
  if(digitalRead(b_up) == LOW || digitalRead(b_down) == LOW)
    controllo_pulsanti();
    
  delay(10);
}
