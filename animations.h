/*
 * Advanced Eye Expressions for ESP32 Gemini Assistant
 * Contains sophisticated eye animation functions using U8g2 library
 */

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <U8g2lib.h>

// External variables (defined in main code)
extern U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;
extern bool happy_state;
extern int skip;
extern int q;

// Animation frame counts for different states
int getAnimationFrameCount(int animationState) {
  switch(animationState) {
    case ANIM_IDLE:
      return 1;
    case ANIM_BLINK:
      return 3;
    case ANIM_HAPPY:
      return 4;
    case ANIM_HEART:
      return 4;
    case ANIM_LOOK_LEFT:
      return 2;
    case ANIM_LOOK_RIGHT:
      return 2;
    case ANIM_SAD:
      return 6;
    case ANIM_ANGRY:
      return 6;
    case ANIM_CUTE:
      return 3;
    case ANIM_WONDER:
      return 8;
    default:
      return 1;
  }
}

// Eye close expression
void close(){
    u8g2.clearBuffer();
        
  u8g2.drawRBox(5,19,55,18, 6);         
  u8g2.drawRBox(67,19,55,18,6);
    u8g2.setDrawColor(0);
    
    u8g2.drawBox(5,1,55,18);         
   u8g2.drawBox(67,1,55,18);
    
    u8g2.setDrawColor(1);
    u8g2.sendBuffer();
}

// Normal expression 
void normal(){
    u8g2.clearBuffer();    
  u8g2.drawRBox(8,12,50,35, 9);         
  u8g2.drawRBox(70,12,50,35,9);
    u8g2.sendBuffer();
    happy_state=false;
}

void blink(){
  static unsigned long PrevTime1 = 0;
  static unsigned long PrevTime2 = 0;
  unsigned long CurrentTime1 = millis();
  unsigned long CurrentTime2 = millis();
  
  if((CurrentTime1 - PrevTime1)>150){
    normal();     
    PrevTime1=CurrentTime1;  
  }  
      
  if((CurrentTime2 - PrevTime2)>1900){
    close();        
    delay(50);
    PrevTime2=CurrentTime2;           
  }
}

void sad(){   
  for(int i=0;i<=15;i+=3){
    u8g2.clearBuffer();
        
    u8g2.drawRBox(8,18,50,29, 9);         
    u8g2.drawRBox(70,18,50,29,9);    
      u8g2.setDrawColor(0);    
   //                    14             25
    u8g2.drawTriangle(3, 14, 64, 14, 3, 21+i);    
    u8g2.drawTriangle(68, 14, 124, 21+i, 124, 14);  
  //                               25        
      u8g2.setDrawColor(1);
             
    u8g2.sendBuffer();    
  }
}

// Upset expression 
void upset(){
    if(!happy_state){
   u8g2.clearBuffer();
    
  u8g2.drawRBox(8,12,50,35, 9);         
  u8g2.drawRBox(70,12,50,35,9);    
   u8g2.setDrawColor(0); 
     
  u8g2.drawBox(8,q,50,35);         
  u8g2.drawBox(70,q,50,35); 
    u8g2.setDrawColor(1); 
    
    u8g2.sendBuffer();
// to make the smooth animation
    if(q<=-7){  q+=3;  }
   }
}

// Wonder expression with smooth animation
void wonder(){   
    u8g2.drawRBox(8,12,50,35, 9);         
  u8g2.drawRBox(70,12,50,35,9);
        
    for(int i=1;i<=10*skip;i+=4){
      u8g2.clearBuffer();
  u8g2.drawRBox(8,12+i,50,35-i, 9);         
  u8g2.drawRBox(70,12,50,35,9);
    u8g2.sendBuffer(); 
       }       
    delay(1600*skip);
    
        
   for(int i=1;i<=10*skip;i+=4){
     u8g2.clearBuffer();
  u8g2.drawRBox(8,22-i,50,25+i, 9);         
  u8g2.drawRBox(70,12+i,50,35-i,9);    
    u8g2.sendBuffer();
    }
     delay(1600*skip);
}

// Happy expression with smooth animation 
void happy(){
  for(int i=62*skip;i>58;i-=3){
    u8g2.clearBuffer();
    
  u8g2.drawRBox(8,12,50,35, 11);
  u8g2.drawRBox(70,12,50,35,11);
   u8g2.setDrawColor(0); 
      
    u8g2.drawDisc(33, i, 38, U8G2_DRAW_ALL);
//                33  62 38   draw all    
    u8g2.drawDisc(95, i, 38, U8G2_DRAW_ALL);     
    
  u8g2.setDrawColor(1);
  
    u8g2.sendBuffer();
   }
    happy_state=true;
}

void cute(){
for(int i=0*skip;i<=2;i+=2){
    u8g2.clearBuffer();    
  u8g2.drawRBox(8,12,50,35, 12);
  u8g2.drawRBox(70,12,50,35,12);
   u8g2.setDrawColor(0);       
    u8g2.drawDisc(30, 66-i, 40, U8G2_DRAW_ALL);
//                33  62 38   draw all    
    u8g2.drawDisc(98, 66-i, 40, U8G2_DRAW_ALL);     
 //               95   62 38   
  u8g2.setDrawColor(1);  
    u8g2.sendBuffer();
   }

    happy_state=true;
}

// Angry expression
void angry(){
    for(int i=0;i<=15;i+=3){
    u8g2.clearBuffer();
  u8g2.drawRBox(8,18,50,29, 9);         
  u8g2.drawRBox(70,18,50,29,9);
    u8g2.setDrawColor(0);    
    u8g2.drawTriangle(3, 14, 64, 18+i, 124, 14);
    u8g2.setDrawColor(1);     
    u8g2.sendBuffer(); 
        }   
}

// String expression
void iam(){    
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(5, 15, "My name is Akno");
    u8g2.drawStr(5, 30, "Nice to meet you :)");
    
    u8g2.sendBuffer();
}

// String expression
void youmademe(){
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_helvB08_tf); 
    u8g2.drawStr(1, 15, "The person who made me");
    u8g2.drawStr(1, 30, "Is Abdulsalam :) ");
    u8g2.drawStr(1, 45, "He is such a great person");
    
    u8g2.sendBuffer();
}

void suspicious(){
    for(int i=0;i<=10;i+=3){
    u8g2.clearBuffer();
    u8g2.drawRBox(8,12,50,20, 9);         
    u8g2.drawRBox(70,12,50,20,9);    
    u8g2.setDrawColor(0);    
    u8g2.drawTriangle(12, 12, 64, 12+i, 107, 12);          
    u8g2.setDrawColor(1);     
    u8g2.sendBuffer();     
        }   
}

void downeye(){
   close();
    delay(120);
  for(int i=0;i<=12;i+=4){
    u8g2.clearBuffer();    
  u8g2.drawRBox(8,22+i,50,21, 9);         
  u8g2.drawRBox(70,22+12,50,21,9);
    u8g2.sendBuffer();
       } 
}

void righteye(){
        close();
    delay(120);
  for(int i=0;i<=8;i+=4){
    u8g2.clearBuffer();    
  u8g2.drawRBox(8-i,17,50,27, 9);         
  u8g2.drawRBox(70-i,17,50,27,9);
    u8g2.sendBuffer();
       }
}

void lefteye(){
  close();
    delay(120);
  for(int i=0;i<=8;i+=4){
    u8g2.clearBuffer();    
  u8g2.drawRBox(8+i,17,50,27, 9);         
  u8g2.drawRBox(70+i,17,50,27,9);
    u8g2.sendBuffer();
     }  
}
   
void upeye(){
   close();
    delay(120);
  for(int i=0;i<=12;i+=4){
    u8g2.clearBuffer();    
  u8g2.drawRBox(8,12-i,50,25, 9);         
  u8g2.drawRBox(70,12-12,50,25,9);
    u8g2.sendBuffer();
       }
}

// Function to get animation frame data (compatibility function)
const unsigned char* getAnimationFrame(int animationState, int frameIndex) {
  // This function is kept for compatibility but the actual animations
  // are now handled by the direct function calls above
  return nullptr;
}

#endif // ANIMATIONS_H