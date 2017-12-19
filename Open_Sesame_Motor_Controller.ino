#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <Servo.h>

const int PIEZO_PIN = A0; // Piezo output
#define SERVO_PIN 9
#define RED_PIN 10
#define GREEN_PIN 11
#define YELLOW_PIN 13
#define BUTTON_PIN 12
#define maxKnocks 20

Servo doorServo;
RH_ASK driver(2000, 6, 7, 10, false);

struct data{
  bool demoModeToggle;
  bool authorized;
} authData;

bool demoModeActive = false;

int knockEndCount = 0;
bool activeSpike = false;

float knockThreshold = 0.06;
float knockEndThreshold = 1000; // How many checks in a row must it be below the threshold to count as the "end of a knock"

float averageErrorThreshold = 100;

// Records
bool recording = false;
bool buttonPressed = false;

int knockTimeout = 2500;

float demoSaved[maxKnocks]; // Demo Pattern
float saved[maxKnocks]; // Currently saved pattern
float current[maxKnocks]; // Active knock pattern attempt
float tempSaved[maxKnocks]; // Active saved pattern attempt

int numKnocks = -1; // Number of knocks in saved pattern
int numDemoKnocks = -1;
int knockCount = 0; // Active track of how many knocks have been done
unsigned long timeLastKnock = 0;
unsigned long timeLastSpike = 0;
unsigned long lastButtonPress = 0;
unsigned long timeLastMessage = 0;


void setup() 
{
  // Set up and turn off all the LED Pins and Button Pins
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(YELLOW_PIN, LOW);

  pinMode(BUTTON_PIN, INPUT);
  doorServo.attach(SERVO_PIN);
  doorServo.write(0);
  
  Serial.begin(9600);
  if (!driver.init()){
    Serial.println("init failed");
    digitalWrite(RED_PIN, HIGH);
  }
}

void loop() 
{

  checkReciever();
  digitalWrite(RED_PIN, demoModeActive ? HIGH : LOW);

  // Read Piezo ADC value in, and convert it to a voltage
  int piezoADC = analogRead(PIEZO_PIN);
  float piezoV = piezoADC / 1023.0 * 5.0;

  checkButtonPress();
  checkSpike(piezoV);

  if(!recording and knockCount > 0 and millis() - timeLastKnock > knockTimeout){
    knockCount = 0;
    Serial.println("Pattern attempt timed out");
    flashLED(RED_PIN, 2);
  }

}

void checkReciever(){
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);

  if (driver.recv(buf, &buflen)){ // Non-blocking    
    // Message with a good checksum received, dump it.
    memcpy(&authData, buf, sizeof(authData));
    if(millis() - timeLastMessage < 5000){
      return;
    }
    timeLastMessage = millis();

    Serial.println("\n---Recieved Message---");
    Serial.println("Demo Mode Toggle: " + String(authData.demoModeToggle));
    Serial.println("Authorized: " + String(authData.authorized) + "\n");
    if(authData.demoModeToggle){
      demoModeActive = !demoModeActive;
    }else{
      if(authData.authorized){
        digitalWrite(GREEN_PIN, HIGH);
        open();
        digitalWrite(GREEN_PIN, LOW);
      }else{
        digitalWrite(RED_PIN, HIGH);
        delay(1000);
        digitalWrite(RED_PIN, LOW);
      }
    }
    
  }
}

void open() {
  Serial.println("\n---OPENING DOOR---\n");
  doorServo.write(180);
  delay(3000);
  doorServo.write(0);
}


void checkButtonPress(){
  if(digitalRead(BUTTON_PIN) == HIGH){
    buttonPressed = true;
  }else if(buttonPressed){
    buttonPressed = false;
    lastButtonPress = millis();
    toggleRecording();
  }
}

void toggleRecording(){
  recording = !recording;
  digitalWrite(YELLOW_PIN, recording ? HIGH : LOW);
  

  
  if(recording){
    Serial.println("Recording started");
  }else{
    Serial.println("Attempting to save pattern");
    validateSavedPattern();
  }
  
  knockCount = 0;
  timeLastKnock = 0;
}

void validateSavedPattern(){
  if(knockCount > 1){
    Serial.println("Pattern of "  + String(knockCount) + " saved");
    if(demoModeActive){
      numDemoKnocks = knockCount;
    }else{
      numKnocks = knockCount;
    }
    memcpy(demoModeActive? demoSaved : saved, tempSaved, maxKnocks*sizeof(float));
  }else{
    Serial.println("Not Enough Knocks");
  }
}

void checkSpike(float voltage){
  if(activeSpike){ // Currently in a spike
    if(voltage <= knockThreshold){ // Potential end of spike
      knockEndCount += 1;
    }else{ // Spike continues
      //Serial.println(knockEndCount);
      knockEndCount = 0;
      Serial.println("Spiking at " + String(voltage));
    }
    if(knockEndCount >= knockEndThreshold){ //Definitely end of a spike
      knockEndCount = 0;
      activeSpike = false;
      
      unsigned long timeSinceButtonPress = millis() - lastButtonPress; 
      if(timeSinceButtonPress < 500){
        Serial.println("Ignoring button press spike");
        return;
      }
      if(millis() - timeLastKnock < 50){
        Serial.println("Noise Knock");
        return;
      }
      timeLastSpike = millis();
      knock();
    }
  }else{ // Not in a spike
    if(voltage > knockThreshold){
      Serial.println("Active Spike: " + String(voltage));
      activeSpike = true;
    }
  }
}

void knock(){
  digitalWrite(GREEN_PIN, HIGH);
  Serial.println("Knock");
  digitalWrite(GREEN_PIN, LOW);

  if(!recording){
    if(demoModeActive){
      if(numDemoKnocks < 1){
        Serial.println("No Saved Demo Pattern. Exiting");
        return;
      }
    }else{
      if(numKnocks < 1){
        Serial.println("No Saved Pattern. Exiting");
        return;
      }
    }
  }
  
  if(knockCount == 0){
    Serial.println("Adding First Knock");
    knockCount = 1;
  }else{
    float timeSinceLastKnock = millis() - timeLastKnock;
    String nameString = "Attempt";
    if(recording){
      nameString = demoModeActive ? "Demo Saved" : "Saved";
    }
    Serial.println("Adding " + nameString + " Knock with time: " + String(timeSinceLastKnock));
    
    if(recording){
      tempSaved[knockCount - 1] = timeSinceLastKnock;
    }else{
      current[knockCount - 1] = timeSinceLastKnock;
    }
    
    if(knockCount <= maxKnocks){
      knockCount += 1;
    }
  }
  
  if(!recording){
    if(demoModeActive){
      if(numDemoKnocks == knockCount){
        Serial.println("End of demo pattern attempt");
        checkEquality(demoSaved, numDemoKnocks);
        knockCount = 0;
      }
    }else{
      if(numKnocks == knockCount){
        Serial.println("End of pattern attempt");
        checkEquality(saved, numKnocks);
        knockCount = 0;
      }
    }
  }

  timeLastKnock = millis();

  
}
  

void checkEquality(float check[maxKnocks], int num){
  Serial.println("Checking Equality");
  float totalDiff = 0;
  for(int i = 0; i < num - 1; i++){
    float currentRecord = current[i];
    float savedRecord = check[i];
    Serial.println("Current: " + String(currentRecord) + " Saved: " + String(savedRecord));
    totalDiff += abs(currentRecord - savedRecord); 
  }

  float average = totalDiff/(num-1);
  Serial.println("Average Difference: " + String(average));

  if(average < averageErrorThreshold){
    digitalWrite(GREEN_PIN, HIGH);
    open();
    digitalWrite(GREEN_PIN, LOW);
  }else{
    digitalWrite(RED_PIN, HIGH);
    delay(1000);
    digitalWrite(RED_PIN, LOW);
  }
}

void flashLED(int pin, int numTimes){
  for(int i = 0; i < numTimes*2; i++){
      digitalWrite(pin, i%2 == 0 ? HIGH : LOW);
      delay(100);
    }
}
