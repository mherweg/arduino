/*
  til311
  
  This example code is in the public domain.

 */

void setup() {                
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(7, OUTPUT);    //latch strobe input
  pinMode(8, OUTPUT);    // latch data input A
  pinMode(9, OUTPUT);    // latch data input B
  pinMode(10, OUTPUT);   // latch data input C
  pinMode(11, OUTPUT);   // latch data input D
  pinMode(13, OUTPUT);   // arduino LED
  digitalWrite(7, LOW); //blanking input
}

void loop() {


 for(uint8_t i=0;i<=15;i++)
 {
     PORTB = i;
     delay(1000);
   } 
  
  digitalWrite(13, HIGH);   // set the LED on
  digitalWrite(7, HIGH);   // blank display
  
  delay(1000);              // wait for a second
  digitalWrite(13, LOW);    // set the LED off
  delay(1000);              // wait for a second
  digitalWrite(7, LOW);    // set the LED off

}

