// Ladyada's logger modified by Bill Greiman to use the Fat16 library 

uint8_t sensorCount = 3; //number of analog pins to log

#include <Fat16.h>
#include <Fat16util.h>
#include <LiquidCrystal.h>

// macros to use PSTR
#define putstring(str) SerialPrint_P(PSTR(str))
#define putstring_nl(str) SerialPrintln_P(PSTR(str))

SdCard card;
Fat16 f;

#define led1Pin 4                // LED1 connected to digital pin 4
#define led2Pin 3                // LED2 connected to digital pin 3


// set the  RX_BUFFER_SIZE to 32!
#define BUFFSIZE 32         
char buffer[BUFFSIZE];      


uint8_t bufferidx = 0;
uint32_t tmp,blockcount = 0;
uint8_t i = 0;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

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

  lcd.begin(16, 2);
  // Print a message to the LCD.
  //lcd.print("hello laserman!");

  pinMode(led1Pin, OUTPUT);      // sets the digital pin as output
  pinMode(led2Pin, OUTPUT);      // sets the digital pin as output
 
  if (!card.init()) {
     lcd.setCursor(0, 1);
     lcd.print("Card init failed"); 
    error(1);
  }
  if (!Fat16::init(&card)) {
     lcd.setCursor(0, 1);
     lcd.print("No partition!"); 
    error(2);
  }
  strcpy(buffer, "GPSLOG00.TXT");
  for (i = 0; i < 100; i++) {
    buffer[6] = '0' + i/10;
    buffer[7] = '0' + i%10;
    // create if does not exist, do not open existing, write
    if (f.open(buffer, O_CREAT | O_EXCL | O_WRITE)) break;
  }
  
  if(!f.isOpen()) {
     lcd.setCursor(0, 1);
     lcd.print("couldnt create"); 
  //Serial.println(buffer);
    error(3);
  }
 // putstring("writing to "); Serial.println(buffer);
   lcd.setCursor(0, 0);
   lcd.print("ready!");

  // write header
 
  strncpy_P(buffer, PSTR("reboot"), 24 + 6*sensorCount);
  Serial.println(buffer);
  // clear write error
  f.writeError = false;
  f.println(buffer);
  if (f.writeError || !f.sync()) {
    lcd.setCursor(0, 1);
   lcd.print("can't write header!");
    error(4);
  }
  
  lcd.autoscroll();
  lcd.setCursor(16,1);
 // delay(1000);

 //  putstring("\r\n");

}

void loop()                     // run over and over again
{
  char c;
 
  // read one 'line'
  if (Serial.available()) {
    c = Serial.read();
    //lcd.setCursor(15, 0);
    lcd.write(c);
   
    buffer[bufferidx] = c;
    
      buffer[bufferidx+1] = 0; // terminate it
      
    //  Serial.print(buffer);
      
      // clear write error
      f.writeError = false;      
     
    bufferidx++;
  }
  
    if (bufferidx == BUFFSIZE-1) {
              
         f.print(buffer);
         if (f.writeError || !f.sync()) {
           lcd.setCursor(0, 1);
           lcd.print("can't write to card");
           error(4);
         }
       lcd.setCursor(0, 0);
       Serial.print(blockcount,DEC);
       
       blockcount++;
       bufferidx = 0;
       lcd.clear();
    }
  
}

