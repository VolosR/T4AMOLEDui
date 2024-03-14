#define LILYGO_TDISPLAY_AMOLED_SERIES
#include "LilyGo_AMOLED.h"      //To use LilyGo AMOLED series screens, please include <LilyGo_AMOLED.h>
#include <TFT_eSPI.h> 
#include <ESP32Time.h>  
#include "poz.h"
#include "bigFont.h"
#include "middleFont.h"
#include "smallFont.h"
#include "fatFont.h"
#include "hugeFatFont.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);
LilyGo_Class amoled;
ESP32Time rtc(0); 

//colors
unsigned short grays[24];
unsigned short mainColor=TFT_MAGENTA;
unsigned short blue=0x0250;
unsigned short lightblue=0x3D3F;
unsigned short yell=0xD7B0;
unsigned short green=0x87BB;
unsigned short purple=0x684D;
unsigned short darkblue=0x09ED;
unsigned short temp=0;

unsigned long st,en;  //starte of cycle and end of cycle, nedded for caluclating fps
float fps;
int deb=0; // touch debounce

int selBright=5;  //selected brightnes level
int brightness[12]={30,50,70,90,110,130,150,170,190,210,230,250};  // all levels

int daysData[7][3]; //in, ex, in+ex
int chosenDay=0;
int totalIn=0;  // total week income
int totalOut=0; // total week expences
int profit=0; // total profit=totalIn-totalOut
String dataLbl[3]={"Income","Expences","Todal:"};
unsigned short dataCol[3]={mainColor, lightblue,yell};  //colors of lines
String leftStrings[4]={"LilyGo","T-Display","T4 AMOLED","ESP32 S3"}; //strings on leftside of screen
String days[7]={"MON","TUE","WED","THU","FRI","SAT","SUN"}; // days in week

double rad=0.01745;
int sx=505;  //seconds diagram x
int sy=172;  // second diagram y
int r=46;    //radius
float x[19];
float y[19];
float x2[19];
float y2[19];
int seg=0;

void setup()
{
    rtc.setTime(10, 4, 12, 1, 1, 2023,0); //set rtc

    amoled.begin();
    amoled.setBrightness(brightness[selBright-1]);
    spr.createSprite(600, 450);
    spr.setSwapBytes(1);

    //define level of grays or greys
     int co=240;
     for(int i=0;i<24;i++)
     {grays[i]=tft.color565(co, co, co);
     co=co-10;}
     

     //define coordinates of seconds graph
    int temp=180;
      for(int i=0;i<19;i++)
    {
       x[i]=((r)*cos(rad*(temp)))+sx;
       y[i]=((r)*sin(rad*(temp)))+sy;
       x2[i]=((r-14)*cos(rad*(temp)))+sx;
       y2[i]=((r-14)*sin(rad*(temp)))+sy;
       
       temp=temp+10;
       if(temp>=360)
       temp=0;
    }

     initDraw();
}

void initDraw()
{
  
  for(int i=0;i<7;i++)
  {
  for(int j=0;j<2;j++)
  if(i==0)daysData[i][j]=random(600,6000); else daysData[i][j]=random(200,5000);
  daysData[i][2]=daysData[i][1]+daysData[i][0];
  totalIn=totalIn+daysData[i][0];
  totalOut=totalOut+daysData[i][1];
  
  }
  
  profit=totalIn-totalOut;

  spr.fillSprite(blue);
  spr.fillSmoothRoundRect(10,10,580,430,14,TFT_BLACK,blue);
  spr.fillSmoothRoundRect(125,20,454,410,14,grays[20],TFT_BLACK);

  spr.drawWedgeLine(26, 60, 36, 42, 2, 2,mainColor);
  spr.drawWedgeLine(34, 60, 44, 42, 2, 2,mainColor);
  spr.drawWedgeLine(42, 60, 52, 42, 2, 2,mainColor);
  spr.drawWedgeLine(28,100,106,100,7,7,grays[21],TFT_BLACK);

  spr.fillSmoothCircle(380,66,9,grays[16],grays[20]);
  spr.fillSmoothCircle(380,66,7,grays[20],grays[16]);
  spr.fillSmoothCircle(418,66,9,grays[16],grays[20]);
  spr.fillSmoothCircle(418,66,7,grays[20],grays[16]);
  spr.drawFastVLine(396, 58, 18, grays[16]);
  spr.drawFastVLine(402, 58, 18, grays[16]);

  spr.fillSmoothRoundRect(445,40,120,140,8,grays[22],grays[20]);
  spr.fillSmoothRoundRect(445,195,120,222,8,grays[22],grays[20]);
  spr.fillSmoothRoundRect(142,178,288,130,8,grays[22],grays[20]);
  spr.fillSmoothRoundRect(142,322,288,94,8,grays[22],grays[20]);

  for(int i=0;i<3;i++){
  spr.fillSmoothRoundRect(142+(i*100),90,88,74,8,grays[22],grays[20]);
  spr.setTextColor(grays[13],grays[22]);
  spr.loadFont(smallFont);
  spr.drawString(dataLbl[i],154+(i*100),96);
  spr.unloadFont();}

  spr.setTextColor(grays[4],grays[20]);
  spr.loadFont(bigFont);
  spr.drawString("Business Overview",142,32);
  spr.unloadFont();

  spr.setTextColor(grays[8],grays[22]);
  spr.loadFont(middleFont);
  spr.drawString("Last seven days",150,184);
  spr.drawString("Total Values",150,328);
  spr.drawString("Runtime:",450,46);
  spr.drawString("Device",450,199);
   spr.setTextColor(mainColor,TFT_BLACK);
  spr.drawString("VOLOS",60,42);
  spr.unloadFont();
   spr.setTextColor(0xC8F6,TFT_BLACK);
  spr.drawString("YOUTUBE CHANNEL",22,70);

  spr.drawWedgeLine(547,44,545,52,2,1,lightblue,grays[22]);
  spr.drawWedgeLine(552,44,552,52,2,1,lightblue,grays[22]);

  spr.setTextColor(yell,TFT_BLACK);
  spr.drawString("MARCH 2024",30,414);

  spr.setTextColor(grays[13],grays[20]);
  spr.loadFont(smallFont);
  spr.drawString("I am fat and i dont care!",142,62);
  spr.setTextColor(grays[16],TFT_BLACK);
  for(int i=0;i<4;i++)
  {spr.drawString(leftStrings[i],28,120+(i*26));}
  spr.unloadFont();


spr.loadFont(smallFont);
spr.setTextColor(grays[16],TFT_BLACK);
spr.drawString("INCOME",160,358);
spr.drawString("OUTCOME",260,358);
spr.drawString("PROFIT",362,358);
spr.unloadFont();


spr.loadFont(middleFont);
spr.setTextColor(grays[8],TFT_BLACK);
spr.drawString(String(totalIn),160,384);
spr.drawString(String(totalOut),260,384);
spr.drawString(String(profit),362,384);
spr.unloadFont();
  
}
void draw()
{ 
  spr.setTextDatum(0);

  //##################################### three numbers region
  for(int i=0;i<3;i++){
  spr.fillSmoothRoundRect(146+(i*100),120,76,42,8,grays[22],grays[20]);
  spr.setTextColor(grays[6],grays[22]);
  spr.loadFont(middleFont);
  spr.drawString(String(daysData[chosenDay][i]),154+(i*100),124);
  spr.unloadFont();
  spr.fillRect(154+(i*100),156,68,2,dataCol[i]);
  }

  //############################################ graph region
  spr.fillSmoothRoundRect(150,212,273,88,8,grays[21],grays[20]);
  spr.fillRect(384,186,36,20,grays[22]);
  spr.setTextColor(grays[11],grays[22]);
  spr.loadFont(smallFont);
  spr.drawString(days[chosenDay],388,188);
  spr.unloadFont();
  
  spr.setTextDatum(4);
  spr.setTextColor(grays[13],grays[21]);
  spr.drawFastVLine(180,215,80,grays[7]);
  spr.drawFastHLine(180,295,231,grays[7]);
  for(int i=1;i<4;i++){
  spr.drawFastHLine(184,295-(i*20),227,grays[16]);
  spr.drawString(String(i*25),165,295-(i*20));
  }

  for(int i=1;i<8;i++)
  {
    if(i-1==chosenDay) temp=mainColor; else temp=purple;
    spr.drawWedgeLine(180+(i*30)-4,290,180+(i*30)-4,290-map(daysData[i-1][0],200,6200,0,80),3,3,temp,grays[21]);
    if(i-1==chosenDay) temp=lightblue; else temp=darkblue;
    spr.drawWedgeLine(180+(i*30)+4,290,180+(i*30)+4,290-map(daysData[i-1][1],200,6200,0,80),3,3,temp,grays[21]);
  }

  //#########################################device region
  spr.fillSmoothRoundRect(450,229,110,182,8,grays[21],grays[20]);
  spr.fillSmoothCircle(505,334,26,mainColor,grays[21]);
  for(int i=0;i<12;i++)
  spr.drawSmoothArc(505, 334, 45, 36, i*30, ((i+1)*30)-6, grays[18], grays[21]);

  for(int i=0;i<selBright;i++)
  spr.drawSmoothArc(505, 334, 45, 32, i*30, ((i+1)*30)-6, mainColor, grays[21]);

  spr.setTextColor(grays[13],grays[21]);
  spr.loadFont(smallFont);
  spr.setTextDatum(0);
  spr.drawString("Battery:",452,232);
  spr.drawString("Brightness:",452,258);
  spr.setTextColor(grays[8],grays[21]);
  spr.drawString("FPS:",452,390);
  spr.drawString(String(fps),524,390);
  spr.unloadFont();

  // batery
  spr.drawRect(512,234,34,18,green);
  for(int i=0;i<5;i++)
  spr.fillRect(515+(i*6),236,4,14,green);
  spr.fillRect(546,238,3,10,green);

  spr.setTextDatum(4);
  spr.setTextColor(TFT_BLACK,mainColor);
  spr.loadFont(fatFont);
  spr.drawString(String(brightness[selBright-1]),504,336);
  spr.unloadFont();
  spr.setTextDatum(0);

  //#######################################runtime region
   spr.fillSmoothRoundRect(450,74,110,101,8,grays[21],grays[20]);
   spr.loadFont(hugeFatFont);
   spr.setTextDatum(4);
   spr.setTextColor(grays[3],TFT_BLACK);
   spr.drawString(rtc.getTime().substring(0,5),505,102);
   spr.unloadFont();

  spr.loadFont(fatFont);
  spr.drawString(String(rtc.getSecond()),sx,sy-6);
  spr.unloadFont();
  spr.setTextDatum(0);

for(int i=0;i<seg;i++)
spr.drawWedgeLine(x[i],y[i],x2[i],y2[i],1,1,lightblue,TFT_BLACK);



   
amoled.pushColors(0, 0, 600, 450, (uint16_t *)spr.getPointer());
}

void loop()
{
   st=millis();
   static int16_t x, y;
   bool touched = amoled.getPoint(&x, &y);


   if(touched){
   if(deb==0)
   {
    deb=1;
        if(x>445 && x<590 && y>10 && y<190) 
        {rtc.setTime(0, 0, 0, 1, 1, 2023,0);}

        if(x>445 && x<590 && y>195 && y<440) 
        {selBright++; if(selBright==13) selBright=1; amoled.setBrightness(brightness[selBright-1]);}

         if(x>140 && x<435 && y>10 && y<440) 
        {chosenDay++; if(chosenDay==7) chosenDay=0;}

          if(x>10 && x<120 && y>10 && y<440) 
        {initDraw();}
    }
   }else deb=0;

   seg++;
   if(seg==19) seg=0;
   
   draw();
   en=millis();
   fps=1000.00/(en-st);
}




