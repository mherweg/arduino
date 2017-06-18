    


#include <Adafruit_GFX.h>



byte image[]  = {
    
   0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
    
     };    
    
    
    
#define TAKT 5
#define HIDE 4
#define DATA 0

#define BIT_S(var,b) ((var&(1<<b))?1:0)

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

uint8_t myrandom(){
  static uint16_t muh = 0xAA;
  uint8_t x;
  for (x=0; x<8; x++) {
    muh = (muh<<1) ^ BIT_S(muh,1) ^ BIT_S(muh,8) ^ BIT_S(muh,9) ^ BIT_S(muh,13) ^ BIT_S(muh,15);
  }
  return (uint8_t) muh;
}




#define NUMPLANE 1
#define NUM_ROWS 8
#define LINEBYTES 8


void shiftbyte(byte dat)
{
  unsigned char p;
  int i;
  p=0x01;

 
  for(i=0;i<8;i++)
  {
  if (dat & p)
       digitalWrite(DATA, 1);   
  else
      digitalWrite(DATA, 0);
    
  digitalWrite(TAKT, 0);
  digitalWrite(TAKT, 1);
  p<<=1;
  }
 
}

void show_frame()
{
  int line,j;
  int offset=0;
  byte val;  
   digitalWrite(HIDE, 1)  ;   //hide 
  //for (line=0;line<8;line++)
  for (line=8;line>=0;line--)
  {
    //for(j=7;j>=0;j--)
    for(j=0;j<8;j++)
    { 
     //val=image[offset]-1; //show inverted
     offset=line*8+j;
      val=image[offset];
     shiftbyte(val);
    }
  }
   digitalWrite(HIDE, 0);    //show
}



void setup(){
  Serial.begin(9600); 
  //while (!Serial) {
   // ; // wait for serial port to connect. Needed for Leonardo only
  //}
 pinMode(DATA, OUTPUT);
 pinMode(HIDE, OUTPUT);
 pinMode(TAKT, OUTPUT);
// delay(1000);
 //allon();
 //fillpattern(0xFF);
// delay(1000);
// fillpattern(0x00);
// delay(1000);
 //fadeout();
}
 int zeit=0;
int offset = 0;




void loop() {  
   
  		//for ever
  
    show_frame();
   delay(10);
    zeit++;
    image[7]=zeit;
    
     
  
  } // for ever
    
//main
