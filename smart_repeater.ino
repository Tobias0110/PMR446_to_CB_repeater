//Tobias Ecker OE3TEC 2019

#include <Wire.h>
#include <EEPROM.h>

#define SPEED 200
#define POWERUPCH 10
//5000 pro min
#define TXTIME 10000

uint8_t input, new_symbol, cmd_en, code[2], exe, i, lock_tx = 0, last_tx = 0;
const uint8_t cb_pll[40] {0xEF, 0xE1, 0xE9, 0xEA, 0xF8, 0xF0, 0xEC, 0xE0, 0xE8, 0xC4, 0xCF, 0xC1, 0xC9, 0xCA, 0xD8, 0xD0, 0xCC, 0xC0, 0xC8, 0xA4, 0xAF, 0xA1, 0xA9, 0xAA, 0xB8, 0xB0, 0xAC, 0xA0, 0xA8, 0x84, 0x8F, 0x81, 0x89, 0x8A, 0x98, 0x90, 0x8C, 0x80, 0x88, 0x44};
uint32_t duration = 0;

void pmr_tx_on() {
  pinMode(17, OUTPUT); //PTT
  digitalWrite(17, LOW);
}

void pmr_tx_off() {
  pinMode(17, INPUT); //PTT
}

void cb_tx_on() {
  digitalWrite(7, HIGH);
}

void cb_tx_off() {
  digitalWrite(7, LOW);
}

void pmr_ok() {
  //Inform the PMR user about the success
  delay(5000);
  pmr_tx_on();
  tone(3, 1000, 1000);
  delay(1000);
  pmr_tx_off();
}

void setup() {
  //Set to high Z
  pinMode(17, INPUT); //pmr_tx
  pinMode(7, OUTPUT); //cb_tx
  digitalWrite(7, LOW);
  
  pinMode(2, INPUT); //pmr_rx
  pinMode(5, INPUT_PULLUP); //cb_rx

  //CM8870
  pinMode(12, INPUT); //Q1
  pinMode(11, INPUT); //Q2
  pinMode(10, INPUT); //Q3
  pinMode(9, INPUT); //Q4
  pinMode(8, INPUT); //StD

  pinMode(3, OUTPUT); //Tone

  Serial.begin(9600);
  Wire.begin();

  Wire.beginTransmission(0x27);
  Wire.write(cb_pll[POWERUPCH-1]);
  Wire.endTransmission();

  //Start PMR
  pinMode(16, OUTPUT); //Power
  digitalWrite(16, LOW);
  delay(6000);
  pinMode(16, INPUT);

  //Check if TX is disabled
  lock_tx = EEPROM.read(0);
}

void loop() {
  // put your main code here, to run repeatedly:

if((digitalRead(2) == 1) && (lock_tx == 0) && (duration < TXTIME))
{
  //TX on CB
  cb_tx_on();
  //no TX on PMR
  pmr_tx_off();
  Serial.println("CBTX");
  //reset the duration counter if the repeater switches from TXPMR to TXCB without a brake
  if(last_tx == 2)
  {
    duration = 0;
  }
  //CB TX happened
  last_tx = 1;

  duration++;
}
else if ((digitalRead(5) == 0) && (lock_tx == 0) && (duration < TXTIME))
{
  //TX on PMR
  pmr_tx_on();
  //no TX on CB
  cb_tx_off();
  Serial.println("PMRTX");
  //reset the duration counter if the repeater switches from TXCB to TXPMR without a brake
  if(last_tx == 1)
  {
    duration = 0;
  }
  //PMR TX happened
  last_tx = 2;

  duration++;
}
else
{
  pmr_tx_off();
  cb_tx_off();
  //reset duration counter only if nothing is recived on PMR or CB
  if(((digitalRead(2) == 1) || (digitalRead(5) == 0)) == 0)
  {
    duration = 0;
  }
  //no tx happened
  last_tx = 0;
  Serial.println("NOTX");
}

Serial.println(duration);

new_symbol = digitalRead(8);
if(new_symbol == 1)
{
  input = 0;
  input |= digitalRead(12);
  input |= digitalRead(11) << 1;
  input |= digitalRead(10) << 2;
  input |= digitalRead(9) << 3;

  //# starts and ends a new code word
  if((input == 12) && (cmd_en == 0))
  {
    cmd_en = 1;
    code[0] = 0;
    code[1] = 0;
    i = 0;
  }
  else if (cmd_en == 1)
  {
    //recive two digits
    if(input == 10) input = 0; //See datasheet of CM8870 (10 equals 0)
    code[i] = input;
    i++;
  }

  if ((i == 2) && (cmd_en == 1))
  {
    cmd_en = 0;
    //create one number
    exe = code[0] * 10 + code[1];
    //Check if the recived information is valid
    Serial.println(exe);
    //Channel No. OK
    if((exe > 0) && (exe < 41))
    {
      Serial.println(cb_pll[exe-1]);

      //Set CB-Channel
      Wire.beginTransmission(0x27);
      Wire.write(cb_pll[exe-1]);
      Wire.endTransmission();

      //Inform the PMR user about the success
      pmr_ok();
    }
    else if (exe == 00)
    {
      delay(3000);
      pmr_tx_on();
      delay(500);
      tone(3, 147, SPEED);
      delay(SPEED*1.5);
      tone(3, 185, SPEED);
      delay(SPEED*1.5);
      tone(3, 220, SPEED);
      delay(SPEED*1.5);
      tone(3, 220, SPEED*2.5);
      delay(SPEED*3);
      
      tone(3, 440, SPEED);
      delay(SPEED*1.5);
      tone(3, 440, SPEED);
      delay(SPEED*3);
      
      tone(3, 370, SPEED);
      delay(SPEED*1.5);
      tone(3, 370, SPEED);
      delay(SPEED*3);

      tone(3, 147, SPEED);
      delay(SPEED*1.5);
      tone(3, 147, SPEED);
      delay(SPEED*1.5);
      tone(3, 185, SPEED);
      delay(SPEED*1.5);
      tone(3, 220, SPEED);
      delay(SPEED*1.5);
      tone(3, 220, SPEED*2.5);
      delay(SPEED*3);

      tone(3, 494, SPEED);
      delay(SPEED*1.5);
      tone(3, 494, SPEED);
      delay(SPEED*3);
      
      tone(3, 392, SPEED);
      delay(SPEED*1.5);
      tone(3, 392, SPEED);
      delay(SPEED*3);

      tone(3, 139, SPEED);
      delay(SPEED*1.5);
      tone(3, 139, SPEED);
      delay(SPEED*1.5);
      tone(3, 165, SPEED);
      delay(SPEED*1.5);
      tone(3, 247, SPEED);
      delay(SPEED*1.5);
      tone(3, 247, SPEED*2);
      delay(SPEED*2.5);

      tone(3, 494, SPEED);
      delay(SPEED*1.5);
      tone(3, 494, SPEED);
      delay(SPEED*3);
      
      tone(3, 370, SPEED);
      delay(SPEED*1.5);
      tone(3, 370, SPEED);
      delay(SPEED*3);
      
      pmr_tx_off();
    }
    else if (exe == 50)
    {
      lock_tx = 1;
      EEPROM.write(0, 1);
      pmr_ok();
    }
    else if (exe == 51)
    {
      lock_tx = 0;
      EEPROM.write(0, 0);
      pmr_ok();
    }
    else
    {
      //Inform the PMR user about the error
      delay(5000);
      pmr_tx_on();
      tone(3, 1000, 1000);
      delay(2000);
      tone(3, 1000, 1000);
      delay(1000);
      pmr_tx_off();
    }
  }
  
  //wait until the decoder is ready for the next symbol
  while(digitalRead(8) == 1);
}

  //delay(100);
}
