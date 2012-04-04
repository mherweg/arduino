// Ladyada's logger modified by Bill Greiman to use the Fat16 library 

uint8_t sensorCount = 3; //number of analog pins to log

#include <Fat16.h>
#include <Fat16util.h>

// macros to use PSTR
#define putstring(str) SerialPrint_P(PSTR(str))
#define putstring_nl(str) SerialPrintln_P(PSTR(str))

SdCard card;
Fat16 f;

#define led1Pin 4                // LED1 connected to digital pin 4
#define led2Pin 3                // LED2 connected to digital pin 3


// set the  RX_BUFFER_SIZE to 32!
#define BUFFSIZE 40         
char buffer[BUFFSIZE];      


uint8_t bufferidx = 0;
uint32_t tmp;
uint8_t i = 0;

// blink out an error code
void error(uint8_t errno) {
  
   while(1) {
     for (i=0; i<errno; i++) {    
       digitalWrite(led1Pin, HIGH);
       digitalWrite(led2Pin, HIGH);
       delay(100);
       digitalWrite(led1Pin, LOW);   
       digitalWrite(led2Pin, LOW);   
       delay(100);
     }
     for (; i<10; i++) {
       delay(200);
     }      
   } 
}

void setup()                    // run once, when the sketch starts
{
  Serial.begin(2400);
//  putstring_nl("GPSlogger");
  pinMode(led1Pin, OUTPUT);      // sets the digital pin as output
  pinMode(led2Pin, OUTPUT);      // sets the digital pin as output
 
  if (!card.init()) {
  //  putstring_nl("Card init. failed!"); 
    error(1);
  }
  if (!Fat16::init(&card)) {
  //  putstring_nl("No partition!"); 
    error(2);
  }
  strcpy(buffer, "GPSLOG00.CSV");
  for (i = 0; i < 100; i++) {
    buffer[6] = '0' + i/10;
    buffer[7] = '0' + i%10;
    // create if does not exist, do not open existing, write
    if (f.open(buffer, O_CREAT | O_EXCL | O_WRITE)) break;
  }
  
  if(!f.isOpen()) {
  //  putstring("couldnt create "); Serial.println(buffer);
    error(3);
  }
 // putstring("writing to "); Serial.println(buffer);
 // putstring_nl("ready!");

  // write header
  if (sensorCount > 6) sensorCount = 6;
  strncpy_P(buffer, PSTR("reboot"), 24 + 6*sensorCount);
  Serial.println(buffer);
  // clear write error
  f.writeError = false;
  f.println(buffer);
  if (f.writeError || !f.sync()) {
   // putstring_nl("can't write header!");
    error(4);
  }
  
 // delay(1000);

 //  putstring("\r\n");

}

void loop()                     // run over and over again
{
  char c;
 
  // read one 'line'
  if (Serial.available()) {
    c = Serial.read();
    
    buffer[bufferidx] = c;
    
      buffer[bufferidx+1] = 0; // terminate it
      
    //  Serial.print(buffer);
      
      // clear write error
      f.writeError = false;      
     
    bufferidx++;
  }
  
    if (bufferidx == BUFFSIZE-1) {
       Serial.print('!', BYTE);
       
         f.print(buffer);
         if (f.writeError || !f.sync()) {
        // putstring_nl("can't write to card");
           error(4);
         }
      // putstring_nl(" ok ");
       bufferidx = 0;
    }
  
}

