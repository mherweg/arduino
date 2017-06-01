
// Hardware panel for Flightgear
// test programm for display, rotary encoders, key matrix
// ATMEGA32


#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include "util.h"
#include <stdlib.h>

// shiftout & display: lower half of PORTC

// BCD out to 7442: bits 4,5,6 of PORTC

// PORTB: input rotary 0-3

// PORTA: 7 return lines from key matrix


#define HC595_PORT   PORTC
#define HC595_DDR    DDRC

#define HC595_DS_POS PC0      //Data pin (DS) pin location
#define HC595_SH_CP_POS PC2      //Shift Clock (SH_CP) pin location 
#define HC595_ST_CP_POS PC1      //Store Clock (ST_CP) pin location
#define WR PC3

#define CE 7   // bit 7 in shift-register 0

#define BIT_S(var,b) ((var&(1<<b))?1:0)

// 8 x CE  auf shift-register 3
uint8_t c_array[16]={
  0x81,9,5,0x5c,3,0x4a,0x46,0x4e,
  0x80,8,4,0x3c,2,0x2a,0x26,0x2e
};
// das 9. CE Bit  -  bit 7 in shift-register 0
uint8_t ce_array[16]={
  0,0,0,1,0,1,1,1,
  1,1,1,1,1,1,1,1
};

uint8_t outword[4]={  0,0,0,0  };

char text[]="Hello World! 1234567890 abcdefghijklmnopqrstuvwxyz.,;<>!$%&/()=?";

//rotary state machine
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6
#define R_START 0x0
#define DIR_NONE 0x0
// Clockwise step.
#define DIR_CW 0x10
// Anti-clockwise step.
#define DIR_CCW 0x20

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};

unsigned char state [6] ={ R_START,R_START,R_START,R_START,R_START,R_START};

uint8_t keybyte[7]    ={0,0,0,0,0,0,0};
uint8_t keybyte_old[7]={0,0,0,0,0,0,0};


void ScanKeyMatrix(){
   uint8_t row=0;
  for (row=0;row<4;row++){
         
    PORTC = (PORTC & 0x0F) + (row << 4); 
    wait(1);  // wichtig!! aber zu lang
    keybyte[row] = ~PINA;
    //led_put_number(keybyte[row], row*4+16);
    //wait(200);
    
       }
   if (keybyte[0] != keybyte_old[0]){    
	led_put_number(keybyte[0], 16);
	keybyte_old[0] = keybyte[0];
   }
   if (keybyte[1] != keybyte_old[1]){    
	led_put_number(keybyte[1], 20);
	keybyte_old[1] = keybyte[1];
   }
   if (keybyte[2] != keybyte_old[2]){    
	led_put_number(keybyte[2], 24);
	keybyte_old[2] = keybyte[2];
   }
   if (keybyte[3] != keybyte_old[3]){    
	led_put_number(keybyte[3], 28);
	keybyte_old[3] = keybyte[3];
   }
   
}


unsigned char RotaryProcess( uint8_t knob) {
  // Grab state of input pins.
  uint8_t map[4]={
  0x03,
  0x0c,
  0x30,
  0xc0,};
  
  unsigned char pinstate;
  
  pinstate = PINB & map[knob] ;
  if (knob==1){
  pinstate >>= 2;
  }
  //= (digitalRead(pin2) << 1) | digitalRead(pin1);
  // Determine new state from the pins and state table.
  state[knob] = ttable[state[knob] & 0xf][pinstate];
  //state2 = ttable[state & 0xf][pinstate2];
  // Return emit bits, ie the generated event.
  //return state & 0x30;

  
  return state[knob];
  
}


//Sends a clock pulse on SH_CP line
void HC595Pulse()
{
  //Pulse the Shift Clock
  HC595_PORT|=(1<<HC595_SH_CP_POS);//HIGH
  //wait(1);
  HC595_PORT&=(~(1<<HC595_SH_CP_POS));//LOW
}

//Sends a clock pulse on ST_CP line
void HC595Latch()
{
  //Pulse the Store Clock
  HC595_PORT|=(1<<HC595_ST_CP_POS);//HIGH
  //wait(1);
  HC595_PORT&=(~(1<<HC595_ST_CP_POS));//LOW
  //wait(1);
}

void HC595Write(uint8_t data)
{
  //Send each 8 bits serially
  uint8_t i;
  
  //Order is MSB first
  for(i=0;i<8;i++)
  {
    //Output the data on DS line according to the
    //Value of MSB
    if(data & 0b10000000)
    {
      //MSB is 1 so output high
      HC595_PORT|=(1<<HC595_DS_POS);
    }
    else
    {
      HC595_PORT&=(~(1<<HC595_DS_POS));
      //MSB is 0 so output high
    }
    //MH
    //wait(1);
    HC595Pulse();  //Pulse the Clock line
    data=data<<1;  //Now bring next bit at MSB position
    
  }
  
  //Now all 8 bits have been transferred to shift register
  //Move them to output latch at one
  // HC595Latch();
}


void led_putc(uint8_t letter, uint8_t col)
{
  uint8_t i,b,ce,digit;
  b= col/4;
  digit = col % 4;      //auswahl von einer der 4 Stellen in einem DLO3416 Modul per A0 ind A1 
  
  if (digit == 0){
    digit=3;      
  }
  else{  
    if (digit == 3){
      digit=0;      
    }
  }
  
  outword[3] = c_array[b];
  outword[0] = letter;
  // TODO: nur 2 bits ändern!!!
  outword[2] = digit;
  
  ce=ce_array[b];
  if (ce == 0){
    outword[0] &= ~(1 << CE);      // CE Bit low
  }
  else
  {
    outword[0] |= (1 << CE);
  }
  
  HC595Write(outword[3])  ; // 8 x ce
  HC595Write(outword[2])  ;// LEDs + a0, a1
  HC595Write(outword[1])  ;//  LEDs
  HC595Write(outword[0])  ;// D0..6  + CE9
  HC595Latch();
  //wait(1);
  PORTC &= ~(1 << WR);  // WR auf Low
  //wait(1);
  PORTC |= (1 << WR); 
}


void led_put_number(int number, uint8_t col) 
{
  char Buffer[6];
  int elev,c,j;
  char letter;
  
    // int to char test: 
    //for (elev=0;elev<25000;elev+=100)
    //{
      itoa( number, Buffer, 10 );
      j = strlen(Buffer);    
      for (c=0;c<j;c++){
	letter = Buffer[c];
	led_putc(letter,  col+c-j);
	//wait(500);
      }
    //}
}

void led_clear(uint8_t pos,uint8_t len)
{
 uint8_t c; 
 for (c=pos;c<(pos+len);c++){
      led_putc(' ',  c);
      //wait(100);
    }
}

int main()
{
  
  //     0b10000001,                      
  uint8_t c,i,j, knob;
  int elev,elev_old,hdg=180,hdg_old,spd=250,spd_old;
  uint8_t letter = 0;
  unsigned char ret;
  
  DDRC = 0xFF;  //Port C all outputs
  DDRB = 0x00;   // rotary inputs
  PORTB = 0xFF; // enable pullup
  
  DDRA = 0x00;   // 7 keyboard matrix input lines
  PORTA = 0xFF; // enable pullup
  
  
 
    
 
     // gibt den Text aus
      for (c=0;c<sizeof(text)-1;c++){
          letter = text[c];
          led_putc(letter,  c);
          //wait(500);
        }
        wait(1000);
	
led_clear(0,64);	
	
	
  while(1)
  {
//    ret = RotaryProcess(0);
//     //led_putc(ret + '0',  1);
//     if (ret == 0x10)
//     {
//      elev -=100 ;
//     }
//     
//     if (ret == 0x20)
//     {
//       elev += 100;
//     }
//     
//     if (elev != elev_old)
//     {
//     led_clear(10-5,5);
//     led_put_number(elev,10);
//     elev_old = elev;
//     }

  ScanKeyMatrix();
    
    
   ret = RotaryProcess(0);
    //led_putc(ret + '0',  1);
    if (ret == 0x10)
    {
     spd -=10;
    }
    
    if (ret == 0x20)
    {
      spd += 10;
    }
    
    if (spd != spd_old)
    {
    led_clear(3-3,3);
    led_put_number(spd,3);
    spd_old = spd;
    }    
    
    
    
    
    ret = RotaryProcess(1);
    //led_putc(ret + '0',  1);
    if (ret == 0x20)
    {
      if (hdg == 0){
	hdg=359;
      }
      else{
	hdg -=1 ;
      }
    }
    
    if (ret == 0x10)
    {
      if (hdg == 359){
	hdg=0;
      }
      else{
      hdg += 1;
      }
    }
    if (hdg != hdg_old)
    {
    led_clear(7-3,3);
    led_put_number(hdg,7);
    hdg_old = hdg;
    }
    
  }
  
} //main
