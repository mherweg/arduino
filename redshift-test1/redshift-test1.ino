    


#include <Adafruit_GFX.h>


byte image[]  = {
    
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    
     };    
    
    
    
#define TAKT 5
#define HIDE 4
#define DATA 0

#define BIT_S(var,b) ((var&(1<<b))?1:0)

// clear bit
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

// set bit
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


//Adafruit_GFX matrix = Adafruit_GFX(64,8);

uint8_t myrandom(){
  static uint16_t muh = 0xAA;
  uint8_t x;
  for (x=0; x<8; x++) {
    muh = (muh<<1) ^ BIT_S(muh,1) ^ BIT_S(muh,8) ^ BIT_S(muh,9) ^ BIT_S(muh,13) ^ BIT_S(muh,15);
  }
  return (uint8_t) muh;
}


void set_pixel(int x, int y,int color ){

byte b_index , mybit;


b_index = x/8 + y*8;
mybit = x % 8;

if (color > 0)
  sbi(image[b_index], mybit);
else
  cbi(image[b_index], mybit);
 

//Serial.println(b_index);
//Serial.println(mybit);
//Serial.println(image[b_index]);
//Serial.println("-----");

}





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
    // 8 bytes on each line
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

}

int zeit=0,x,y;
void loop() {  
   
  	//for ever

  
 for (y=0;y<8;y++){
  //fill from left to right   
  for(x=0;x<64;x++){
    set_pixel(x,y,255);

     show_frame();
    delay(10);
  }
 }
 
 for(x=0;x<64;x++){
   //clear from top to bottom
   for (y=0;y<8;y++){
 
    set_pixel(x,y,0);

     show_frame();
    delay(10);
  }
 }

 for (y=0;y<8;y++){
  //fill from left to right   
  for(x=0;x<64;x++){
    set_pixel(x,y,255);

     show_frame();
   
  }
 }
  delay(1000);
 for(x=0;x<64;x++){
   //clear from top to bottom
   for (y=0;y<8;y++){
 
    set_pixel(x,y,0);

     show_frame();
   
  }
 }
  delay(1000);
  
    zeit++;
    //image[7]=zeit;

  } // for ever
    
//main
