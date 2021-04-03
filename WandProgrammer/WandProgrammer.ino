/*
* An IR LED must be connected to Arduino pin 13.
* An IR detector/demodulator must be connected to pin 11
* Version 001
*/

#include <IRremote.h>
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden


#define D_0 0
#define D_1 1
#define RX D_1
#define TX D_0
#define D_2 2   
#define D_3 3   
#define D_4 4   
#define D_5 5   // IRremote
#define D_6 6  
#define D_7 7 
#define D_8 8  
#define D_9 9  
#define D_10 10
#define D_11 11 // button 1
#define D_12 12 // button 2
#define D_13 13 

#define A_1 A1 
#define A_2 A2
#define A_3 A3
#define A_4 A4 // i2cDisplay (SDA)
#define A_5 A5 // i2cDisplay (SCL)


// IR commands from AEG hob2hood device
const long SPEED_1 = 0xE3C01BE2;
const long SPEED_2 = 0xD051C301;
const long SPEED_3 = 0xC22FFFD7;
const long SPEED_4 = 0xB9121B29;
const long SPEED_OFF = 0x55303A3;
const long LIGHT_ON = 0xE208293C;
const long LIGHT_OFF = 0x24ACF947;

// IR commands for DAH
const long DAH_SPEED_1 = 0x23C01B31;
const long DAH_SPEED_2 = 0x6751C385;
const long DAH_SPEED_3 = 0x462FFF63;
const long DAH_SPEED_4 = 0x46121B93;
const long DAH_SPEED_OFF = 0x6430345;
const long DAH_LIGHT_ON = 0x64082965;
const long DAH_LIGHT_OFF = 0x62ACF932;


int BUTTON1_PIN = D_11;
int BUTTON_SEND = D_12;

int IRpin = D_5; // Pin fuer den IR Empfaenger
int LED = D_3;  // LED Pin
IRrecv irrecv(IRpin);
IRsend irsend;
decode_results results; // Die Ergebnisse decodieren

LiquidCrystal_I2C lcd(0x27, 20, 4); //Hier wird festgelegt um was für einen Display es sich handelt. 
//LiquidCrystal_I2C lcd(0x27, 16, 2); //Hier wird festgelegt um was für einen Display es sich handelt. 

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


int lastButtonState;
int messageIDx=0;


void setup()
{
  Serial.begin(9600);// Serielle Ausgabe
  Serial.println(F("STARTING " __FILE__ " from " __DATE__ "\r\nUsing library version Version 2.2.3 " ));

  irrecv.enableIRIn(); // IR Empfaenger starten
  pinMode(LED, OUTPUT);

  lcd.init(); //Im Setup wird der LCD gestartet 
  lcd.backlight(); //Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 
  // lcd.noBacklight();

  pinMode(BUTTON1_PIN, INPUT);
  messageIDx=0;
  doLcd(messageIDx);

}


void loop() {
    delay(40);
  
    int buttonState = digitalRead(BUTTON1_PIN);
    int buttonSendState = digitalRead(BUTTON_SEND);

    if (lastButtonState == HIGH && buttonState == LOW) {
        Serial.println("Press start");
        if(++messageIDx > 25){
          messageIDx=0;
        }

        doLcd(messageIDx);
    }
    if (lastButtonState == LOW && buttonState == HIGH) {
        Serial.println("-- Released");
    }
    
    lastButtonState = buttonState;
   

    if (buttonSendState == LOW) {
        Serial.println("-- Pressing");
        delay(40);
        doSend(); 
    } 

}


void doLcd(int messageIDx) { 

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
  lcd.print(codes[messageIDx], HEX); 
  Serial.println(codes[messageIDx], HEX); 
  
} 

void doSend() { 
   irsend.sendSony(codes[messageIDx], 20);
   delay(100);
} 
