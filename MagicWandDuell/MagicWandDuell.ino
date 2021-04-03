#include <DFRobotDFPlayerMini.h>

#include <IRremote.h>
#include "LedControl.h"
#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden

#define D_0 0
#define D_1 1
#define RX D_1
#define TX D_0
#define D_2 2   // LedControl 0 - 7Segment (CS)
#define D_3 3   // LedControl 0 - 7Segment (CLK)
#define D_4 4   // LedControl 0 - 7Segment (DIN)
#define D_5 5   // IRremote
#define D_6 6   // LedControl 1 (CS)
#define D_7 7   // LedControl 1 (CLK)
#define D_8 8   // LedControl 1 (DIN)
#define D_9 9   // LedControl 2 (CS)
#define D_10 10 // LedControl 2 (CLK)
#define D_11 11 // LedControl 2 (DIN)
#define D_12 12 // GGF DFPlayerMini
#define D_13 13 // GGF DFPlayerMini

#define A_1 A1 
#define A_2 A2
#define A_3 A3
#define A_4 A4 // i2cDisplay (SDA)
#define A_5 A5 // i2cDisplay (SCL)

int RECV_PIN = D_5;
IRrecv irrecv(RECV_PIN);
decode_results results;

LedControl lc0=LedControl(4,3,2,3);
LedControl lc1=LedControl(D_8,D_7,D_6,3);
LedControl lc2=LedControl(D_11,D_10,D_9,3);

byte figure0[8]={
  B00111100,
  B01000010,
  B10011001,
  B10111101,
  B10111101,
  B10011001,
  B01000010,
  B00111100};
  
byte figure1[8]={
  B11111111,
  B11111111,
  B11111111,
  B10111101,
  B10111101,
  B10011001,
  B01000010,
  B00111100};

byte figure_water[8]={
  B00111100,
  B01000010,
  B10011001,
  B10110001,
  B10001101,
  B10011001,
  B01000010,
  B00111100};


byte figure_fire_s[8]={
  B00000000,
  B11000000,
  B11110010,
  B11111101,
  B11111010,
  B11100001,
  B11000000,
  B00000000};
byte figure_water_s[8]={
  B00100000,
  B00010000,
  B00100000,
  B00010000,
  B00100000,
  B00010000,
  B00100000,
  B00010000};
byte figure_air_s[8]={
  B00000000,
  B00001100,
  B00000110,
  B00101100,
  B01010110,
  B00101100,
  B01000110,
  B10100000};
byte figure_ground_s[8]={
  B00000000,
  B00001100,
  B11111100,
  B00001100,
  B00000000,
  B00000000,
  B00000000,
  B00000000};


SoftwareSerial mySoftwareSerial(D_12, D_13); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

#define s_welcome1  15
#define s_welcome2  2
#define s_welcome3  3
#define s_readSet   4
#define s_p1wins    5
#define s_p2wind    6
#define s_welcome   7



LiquidCrystal_I2C lcd(0x27, 20, 4); //Hier wird festgelegt um was für einen Display es sich handelt. 
//LiquidCrystal_I2C lcd(0x27, 16, 2); //Hier wird festgelegt um was für einen Display es sich handelt. 

unsigned long delaytime=500;


int p1Counter=0;
int p2Counter=0;


void setup(){
    Serial.begin(9600);
    Serial.println(F("STARTING " __FILE__ " from " __DATE__ "\r\nUsing library version Version 2.2.3 " ));

    initDFPlayer();
    
    // pinMode (13, OUTPUT);     //An Pin 13 wird eine LED angeschlossen.
    // digitalWrite(13, LOW);    //Diese soll zunächst aus sein

    // https://funduino.de/nr-11-fernbedienung
    irrecv.enableIRIn();
    //irrecv.blink13(true);

    lcd.init(); //Im Setup wird der LCD gestartet 
    lcd.backlight(); //Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 
    // lcd.noBacklight();
    
    /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
    lc1.shutdown(0,false);
    lc2.shutdown(0,false);
    /* Set the brightness to a medium values */
    lc1.setIntensity(0,8);
    lc2.setIntensity(0,8);
    /* and clear the display */
    lc1.clearDisplay(0);
    lc2.clearDisplay(0);
    //we have already set the number of devices when we created the LedControl
    int devices=lc0.getDeviceCount();
    //we have to init all devices in a loop
    for(int address=0;address<devices;address++) {
      /*The MAX72XX is in power-saving mode on startup*/
      lc0.shutdown(address,false);
      /* Set the brightness to a medium values */
      lc0.setIntensity(address,8);
      /* and clear the display */
      lc0.clearDisplay(address);
    }
        
    myDFPlayer.volume(20);  //Set volume value. From 0 to 30

    doLedControl0(figure_water_s);
    doLedControl1(figure_ground_s);
    //hello();
    lc0.setDigit(0,0,p1Counter,false);
    lc0.setDigit(0,4,p2Counter,false);

    doLcd();

    myDFPlayer.play(s_welcome1); 
}

void incP1Counter(){
   lc0.setDigit(0,0,++p1Counter,false);
}
void incP2Counter(){
   lc0.setDigit(0,4,++p2Counter,false);
}

void initDFPlayer(){
    mySoftwareSerial.begin(9600);
    Serial.println();
    Serial.println(F("DFRobot DFPlayer Mini Demo"));
    Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)")); 
    if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      while(true){
        delay(0); // Code to compatible with ESP8266 watch dog.
      }
    }
    Serial.println(F("DFPlayer Mini online."));    
}

void loop() {
  doIRRec();
  //doDFPlayer();
}

void doLcd(){ 
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print("Hallo"); 
  lcd.setCursor(0, 1);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
  lcd.print(""); 
}

int songIdx=0;

int idx;
int p1Idx=0;
boolean p1Set=false;
int p2Idx=0;
boolean p2Set=false;

char *element[] = {
  "Fire                ",
  "Water               ",
  "Ground",
  "Air                 "
 };
 
 char *charms[] = {
  "Spongify            ", 
  "Expelliarmus        ",
  "Duro                ",
  "Impedimenta         ",
  "Protego             ",
  "Reducio             ",
  "Stupefy             ",
  "Stupefy Duo         ",
  "Oppugno             ",
  "Rictusempra         ",
  "Lumos               ",
  "Diffindo            ",
  "Ridiculus           "
 };
 char *moves[] = {
  "rotate anticlockwise", 
  "rotate clockwise    ",
  "flick upwards       ",
  "flick downwards     ",
  "flick left          ",
  "flick right         ",
  "tab on top          ",
  "tab on side         ",
  "big switch          ",
  "push forward        ",
  "double tap on top   ",
  "double tap on side  ",
  "pull back           "
};
char *ticks[] = {
  "(1) ", 
  "(2) ",
  "(3) ",
  "(4) ",
  "(5) ",
  "(6) ",
  "(7) ",
  "(8) ",
  "(9) ",
  "(10)",
  "(11)",
  "(12)",
  "(13)"};

long codes[]={
  0x68B90,  
  0x68B91,  
  0x68B92,  
  0x68B93,  
  0x68B94,  
  0x68B95,  
  0x68B96,  
  0x68B97,
  0x68B98,
  0x68B99,
  0x68BA0,
  0x68BA1,
  0x68BA2,
  0x68C90,  
  0x68C91,  
  0x68C92,  
  0x68C93,  
  0x68C94,  
  0x68C95,  
  0x68C96,  
  0x68C97,
  0x68C98,
  0x68C99,
  0x68CA0,
  0x68CA1,
  0x68CA2
};
 
void doLcd(long messageIDx){ 

  int charmsIdx=messageIDx>12?messageIDx-13:messageIDx;
  Serial.println(messageIDx, DEC);
        
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  lcd.print(charms[charmsIdx]); 
  Serial.println(charms[charmsIdx]); 
  lcd.setCursor(0, 1);
  if(messageIDx<13){
    lcd.print("Wand 1");
  }else{
    lcd.print("Wand 2");
  } 
  if(charmsIdx<3){
    lcd.print(" Fire  ");
  }else if(charmsIdx<6){
    lcd.print(" Water ");  
  }else if(charmsIdx<9){
    lcd.print(" Ground");
  }else {
    lcd.print(" Air   ");
  }


  lcd.setCursor(0, 2);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
  lcd.print(moves[charmsIdx]); 
  Serial.println(moves[charmsIdx]); 
  lcd.setCursor(0, 3);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
  lcd.print(ticks[charmsIdx]); 
  Serial.print(ticks[charmsIdx]); 
/*  lcd.print(codes[messageIDx], HEX); 
  Serial.println(codes[messageIDx], HEX); 
//*/
  
  //lcd.print(messageIDx); 
  
  //lcd.setCursor(0, 1);// In diesem Fall bedeutet (0,1) das erste Zeichen in der zweiten Zeile. 
  //lcd.print(code, HEX);

  //myDFPlayer.next();
  
  //  myDFPlayer.playFolder(1, 1); 
 // myDFPlayer.playFolder(1,charmsIdx+1);  //Play the first mp3
 // myDFPlayer.play(3,charmsIdx);  //Play the first mp3
        myDFPlayer.play(charmsIdx+1); 
  Serial.println(charmsIdx, DEC); 
  //myDFPlayer.start();
  //if (myDFPlayer.available()) {
  //  printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  //}
    
} 


void doLedControl0(byte figure[]) {
  for (int i=0; i<8; i++){
    lc1.setRow(0,i,figure[i]);
  }
  delay(20);
}
void doLedControl1(byte figure[]) {
  for (int i=0; i<8; i++){
    lc2.setRow(0,i,figure[i]);
  }
  delay(20);
}

void doDFPlayer() {
  static unsigned long timer = millis();
  if (millis() - timer > 9000) {
    timer = millis();
     myDFPlayer.next();  //Play next mp3 every 3 second.
    // myDFPlayer.play(1); 
    //  delay(9000);
  }
 // if (myDFPlayer.available()) {
 //   printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
 // }
}


int check(int p1Idx, int p2Idx){
   int p2IdxToCompare=p2Idx-13;
   if(p2IdxToCompare==p1Idx){
      return 0;
   } else if( p2IdxToCompare<p1Idx){
      return -1;
   }else{
      return 1;
   }
}
 

void doIRRec() {
  if (irrecv.decode(&results)) {
    
    if (results.decode_type == NEC) {
      Serial.print("NEC: ");
    } else if (results.decode_type == SONY) {
      Serial.print("SONY: ");
    } else if (results.decode_type == RC5) {
      Serial.print("RC5: ");
    } else if (results.decode_type == RC6) {
      Serial.print("RC6: ");
    } else if (results.decode_type == UNKNOWN) {
      Serial.print("UNKNOWN: ");
    }
    
    Serial.println(results.value, HEX);
    
    if (results.value == 0x68B90){
      idx=0;
      p1Idx=idx;
    } else if (results.value == 0x68B91){
      idx=1;
      p1Idx=idx;
    } else  if (results.value == 0x68B92){
      idx=2;
      p1Idx=idx;
    } else  if (results.value == 0x68B93){
      idx=3;
      p1Idx=idx;
    } else  if (results.value == 0x68B94){
      idx=4;
      p1Idx=idx;
    } else  if (results.value == 0x68B95){
      idx=5;
      p1Idx=idx;
    } else  if (results.value == 0x68B96){
      idx=6;
      p1Idx=idx;
    } else  if (results.value == 0x68B97){
      idx=7;
      p1Idx=idx;
    } else  if (results.value == 0x68B98){
      idx=8;
      p1Idx=idx;
    } else  if (results.value == 0x68B99){
      idx=9;
      p1Idx=idx;
    } else  if (results.value == 0x68BA0){
      idx=10;
      p1Idx=idx;
    } else  if (results.value == 0x68BA1){
      idx=11;
      p1Idx=idx;
    } else  if (results.value == 0x68BA2){
      idx=12;
      p1Idx=idx;
    } else  if (results.value == 0x68C90){
      idx=13;
      p2Idx=idx;
    } else  if (results.value == 0x68C91){
      idx=14;
      p2Idx=idx;
    } else  if (results.value == 0x68C92){
      idx=15;
      p2Idx=idx;
    } else  if (results.value == 0x68C93){
      idx=16;
      p2Idx=idx;
    } else  if (results.value == 0x68C94){
      idx=17;
      p2Idx=idx;
    } else  if (results.value == 0x68C95){
      idx=18;
      p2Idx=idx;
    } else  if (results.value == 0x68C96){
      idx=19;
      p2Idx=idx;
    } else  if (results.value == 0x68C97){
      idx=20;
      p2Idx=idx;
    } else  if (results.value == 0x68C98){
      idx=21;
      p2Idx=idx;
    } else  if (results.value == 0x68C99){
      idx=22;
      p2Idx=idx;
    } else  if (results.value == 0x68CA0){
      idx=23;
      p2Idx=idx;
    } else  if (results.value == 0x68CA1){
      idx=24;
      p2Idx=idx;
    } else  if (results.value == 0x68CA2){
      idx=25;
      p2Idx=idx;
    } 
    if(p1Idx>0){
      if(p1Idx<3){
        doLedControl0(figure_fire_s);
      } else if(p1Idx<6){
        doLedControl0(figure_water_s);
      } else if(p1Idx<9){
        doLedControl0(figure_ground_s);
      } else {
        doLedControl0(figure_air_s);
      }     
    }

    if(p2Idx>0){
      int p2CharmIdx=p2Idx-13;
      if(p2CharmIdx<3){
        doLedControl1(figure_fire_s);
      } else if(p2CharmIdx<6){
        doLedControl1(figure_water_s);
      } else if(p2CharmIdx<9){
        doLedControl1(figure_ground_s);
      } else {
        doLedControl1(figure_air_s);
      }     
    }

    if(p1Idx >0 && p1Set==false){
        myDFPlayer.play(p1Idx); 
        p1Set=true; 
    }
    if(p2Idx >0 && p2Set==false){
        myDFPlayer.play(p2Idx);  
        p2Set=true;
    }
    
    if( p1Idx >0 && p2Idx >0){
      int result = check(p1Idx,p2Idx);
  
      if(result==1){
        incP1Counter();
        myDFPlayer.playFolder(1, 1); 
      }else if(result== -1){
        myDFPlayer.playFolder(1, 2); 
        incP2Counter(); 
      }else{
         myDFPlayer.playFolder(1, 2);
      }
      p1Idx=0;
      p2Idx=0;
      p1Set=false;
      p2Set=false;
    }
     
    doLcd(idx);
    
    irrecv.resume(); // Receive the next value
  }
  
/*
  if (irrecv.decode(&results)) {
    if (results.value == 0x6182021B)  {
      // digitalWrite (13, HIGH);
          Serial.println("rot");
    } else if (results.value == 0x3D9AE3F7){
      Serial.println("gruen");
      //digitalWrite (13, LOW);
      //soll die LED aus gehen.
    } else if (results.value == 0x3D9AE3F7){
      Serial.println("gruen");
     
    } else if (results.value == 0x9716BE3F){
      Serial.println("blau");
    } else if (results.value == 0x8C22657B){
      Serial.println("gelbblau");
    } else if (results.value == 0x488F3CBB){
      Serial.println("rotgruen");
    } else if (results.value == 0x449E79F){
      Serial.println("rotblau");
    } else{
      
       Serial.println(results.value, HEX);    
    }
    doLcd(results.value);

    if(results.decode_type == SONY){
            Serial.println("it's a sony");
    }
    irrecv.resume();
  }
  //*/
}


/*

void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}(/*/
