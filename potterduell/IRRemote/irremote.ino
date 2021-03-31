#include <IRremote.h>

int RECV_PIN = 5;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()

{
    // https://funduino.de/nr-11-fernbedienung
  
    Serial.begin(9600);

    Serial.println(F("STARTING " __FILE__ " from " __DATE__ "\r\nUsing library version Version 2.2.3 " ));

    
    // pinMode (13, OUTPUT);   //An Pin 13 wird eine LED angeschlossen.
    // digitalWrite(13, LOW);    //Diese soll zunächst aus sein

    irrecv.enableIRIn();

}

void loop() {

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
    }
    
    else{
       Serial.println(results.value, HEX);    
    }
  irrecv.resume();
  }
}
