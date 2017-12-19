#include <SPI.h>
#include <MFRC522.h>
#include <RH_ASK.h>

#define TX_PIN 7
#define RX_PIN 6
#define GREEN_PIN 5
#define RED_PIN 4
#define BLUE_PIN 3
#define BUTTON_PIN 2

#define SS_PIN 10
#define RST_PIN 0
MFRC522 mfrc522(SS_PIN, RST_PIN);

RH_ASK driver(2000, RX_PIN, TX_PIN, 10, false);
String omkar = "00 00 00 00"; // Put your ID's here!
String ruchir = "00 00 00 00";
String aivant = "00 00 00 00";
String keyring = "00 00 00 00";

struct data{
  bool demoModeToggle;
  bool authorized;
} authData;

byte tx_buf[sizeof(authData)] = {0};

void setup() {

  pinMode(BUTTON_PIN, INPUT);

  
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.begin(9600);    // Debugging only
  if (!driver.init()){
    Serial.println("init failed");
    setColor(255, 0, 0);
  }
         
}

void loop() {
  if(!mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  if(!mfrc522.PICC_ReadCardSerial()){
    return;
  }

  String content = "";
  byte letter;
  for(byte i = 0; i < mfrc522.uid.size; i++){
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  String id = content.substring(1);
  Serial.println("Found Id: " + id);
  if(id == omkar){
    authorize(true);
  }else if(id == ruchir){
    authorize(true);
  }else if(id == aivant){
    authorize(true);
  }else if(id == keyring){
    authorize(true);
  }else{
    authorize(false);
  }


}

void authorize(bool authorized){
  Serial.println("Authorized: " + String(authorized));
  if(authorized){
    setColor(0, 255, 0);
  }else{
    setColor(255, 0, 0);
  }
  authData.authorized = authorized;
  authData.demoModeToggle = digitalRead(BUTTON_PIN) == HIGH and authorized;
  Serial.println(authData.demoModeToggle);
  memcpy(tx_buf, &authData, sizeof(authData) );
  byte zize=sizeof(authData);
  driver.send((uint8_t *)tx_buf, zize);
  driver.waitPacketSent();
  setColor(0, 0, 0);
//  if(authData.demoModeToggle){
//    flashLED(0, 0, 255, 2);
//  }
}

void flashLED(int red, int green, int blue, int numTimes){
  for(int i = 0; i < numTimes*2; i++){
      if(i%2 == 0){
        setColor(red, green, blue);
      }else{
        setColor(0, 0, 0);
      }
      delay(200);
    }
}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);  
}

