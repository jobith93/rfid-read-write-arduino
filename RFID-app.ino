/******************************************
  PURPOSE:  RFID Reader/Writer Module for Kristu Jayanti College, Bangalore
  Created by Jobith M Basheer
  DATE:   6/2017
  Modified On: 
*******************************************/

/* PIN Description
   NANO - Arduino
   RST  D9
   MISO D12
   MOSI D11
   SCK  D13
   SDA  D8  ss

  PIN Description
   Mega - Arduino
   RST  D49
   MISO D50
   MOSI D51
   SCK  D52
   SDA  D53  ss

  PIN Description
   Uno - Arduino
   RST  D9
   MISO D12
   MOSI D11
   SCK  D13
   SDA  D9  ss
*/

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 8                          //slave select pin
#define RST_PIN 9                         //reset pin
MFRC522 mfrc522(SS_PIN, RST_PIN);         //instatiate a MFRC522 reader object.
MFRC522::MIFARE_Key key;                  //create a MIFARE_Key struct named 'key', which will hold the card information

#define ON LOW
#define OFF HIGH

const int G = 4;
const int Y = 5;
const int R = 6; 

const int beep_dur = 50;

const int buzzer = 10;
int block = 4;

byte readbackblock[18];

int cmd;
String txt;

void setup() {


  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.write("Device Ready! Scan a MiFare Card!\r\n");

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);
  
  pinMode(G, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(R, OUTPUT);

  ledG(OFF);
  ledY(OFF);
  ledR(OFF);
  
  self_test();

  ledG(OFF);
  ledY(OFF);
  ledR(ON);
  
}



void loop() {
  mfrc522.PCD_Init();
  byte blockcontent[16] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

  if (Serial.available() > 0) {

    txt = Serial.readString();
//    Serial.write("Serial Read: ");
//    Serial.print(txt);

    if(txt[0]=='$'){
      for (int j = 0 ; txt[j] ; j++)
      {
        blockcontent[j] = (int) txt[j+1];
      }
    
      Serial.write("Place the card to write:");
      Serial.print(txt);
  
      while (! mfrc522.PICC_IsNewCardPresent()) {
        //nothing;
         ledY(ON);
         delay(50);
         ledY(OFF);
      }
  
      if ( ! mfrc522.PICC_ReadCardSerial()) {
        return;
      }
  
      writeBlock(block, blockcontent);
    }

  }

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }


  readBlock(block, readbackblock);
  for (int j = 0 ; j < 16 ; j++)
  {
    Serial.write(readbackblock[j]);
  }

  delay(500);
  Serial.flush();

}


int writeBlock(int blockNumber, byte arrayAddress[])
{

  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3;
  if (blockNumber > 2 && (blockNumber + 1) % 4 == 0) {
    return 2;
  }


  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  if (status != MFRC522::STATUS_OK) {
    return 3;
  }

  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);

  if (status != MFRC522::STATUS_OK) {
    return 4;
  }
  Serial.write("WRITE DONE!!!!");
  beep(beep_dur);
}


int readBlock(int blockNumber, byte arrayAddress[])
{
  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3;


  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    return 3;
  }

  byte buffersize = 18;
  status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);
  if (status != MFRC522::STATUS_OK) {
    return 4;
  }
  ledG(ON);
  beep(beep_dur);
  ledG(OFF);

}


void beep(int len) {
  digitalWrite(buzzer, LOW);
//  tone(buzzer, 900);
  delay(len);
//  noTone(buzzer);
  digitalWrite(buzzer, HIGH);
}

void ledY(bool trigger) {
  digitalWrite(Y, trigger);
}

void ledR(bool trigger) {
  digitalWrite(R, trigger);
}

void ledG(bool trigger) {
  digitalWrite(G, trigger);
}



/////////////////////////////////// 
#define melodyPin 10

int melody[] = {  2637, 2637, 0, 2637 };
int tempo[] = { 12, 12, 12, 12 };

int song = 0;
 
void sing(int s) {
  // iterate over the notes of the melody:
  song = s;
  
 
    Serial.println(" 'Mario Theme'");
    int size = sizeof(melody) / sizeof(int);
    for (int thisNote = 0; thisNote < size; thisNote++) {
 
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / tempo[thisNote];
 
      buzz(melodyPin, melody[thisNote], noteDuration);
 
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
 
      // stop the tone playing:
      buzz(melodyPin, 0, noteDuration);
 
    }
  
}

 
void buzz(int targetPin, long frequency, long length) {
  digitalWrite(13, HIGH);
  
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
  digitalWrite(13, LOW);
}


void self_test(){
  
  sing(1);
  light_test();
  
  digitalWrite(buzzer, HIGH);

}

void light_test(){
  ledG(ON);
  delay(50);
  ledG(OFF);
  delay(50);
  ledY(ON);
  delay(50);
  ledY(OFF);
  delay(50);
  ledR(ON);
  delay(50);
  ledR(OFF);

  ledR(ON);
  delay(50);
  ledR(OFF);
  delay(50);
  ledY(ON);
  delay(50);
  ledY(OFF);
  delay(50);
  ledG(ON);
  delay(50);
  ledG(OFF);
}

