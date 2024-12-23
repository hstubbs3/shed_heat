#define setupDelayTime 100
#define WATTS_MILLIS 150

int max_watts_spike=1300;
int max_watts_minute=1200;

float watts_factor = 4200.0;
int light_level = -1; 
char cBuffer[256];

struct struct_temp{
  char n[15]; //name
  byte t; // current setting
  struct_temp (char *_n, byte _t ) : t(_t)  { memcpy(n,_n,14); n[14]=0; }
} ;

typedef struct_temp Temperature;

Temperature daTemps[] = {
  { "ComfyTemp", 60 },
  { "ComfyFoot", 90 },
  { "ELEM_MAX", 120 },
  { "Defrost",  0 },
};

enum TEMP: byte {
  COMFYTEMP=0,
  COMFYFOOT=1,
  ELEM_MAX=2,
  DEFROST=3,
  NO_TEMP=255,
};

struct struct_TEMP_TIME_EVENT {
  TEMP t; // temperature affected
  byte h; // 24 hour time to start
  byte m; // minute to start at
  byte v; // value to change the temp to...
  struct_TEMP_TIME_EVENT( TEMP _t, byte _h, byte _m, byte _v) : t(_t), h(_h), m(_m), v(_v) {}
} ; 

typedef struct_TEMP_TIME_EVENT TEMP_TIME_EVENT;

TEMP_TIME_EVENT tempChanges[] = {
  { COMFYTEMP,  0,  0,  45 }, //0
  { COMFYTEMP,  6,  0,  69 }, //1
  { COMFYTEMP, 18,  0,  55 }, //2  
  { COMFYFOOT,  0,  0,  60 }, //3
  { COMFYFOOT,  4,  0, 100 }, //4
  { COMFYFOOT, 18,  0,  80 }, //5
  { DEFROST,   8,  0,  45 }, //6
  { DEFROST,  15,  0,   0 }, //7
};

unsigned long int rough_timer = 0 ;

byte rtcTime[7]; //second, minute, hour, dayOfWeek, dayOfMonth, month, year;
int counterLow=1; //setting up a ghetto counter to be like counterHigh is seconds
int counterMed=0; // tries to be 1/100th second...
int counterHigh=1; // ticks once a second..
unsigned long int keypress_counter=0; // for reading key presses... 
char customKey = 0; 
int mode=3 ; // for switching between menus and screens...
int oldmode=-1; 

int secondLoops=1000; //actually 1/10th of second...
byte mode0_display_second=99;

char dateTimeStamp[64]; // ie -  "2024/10/12 10:24:00 - HH M LLLL/TTTT | " but up to 64 length just in case... 

void makeDateTimeStamp(){
  sprintf(dateTimeStamp,"20%02d/%02d/%02d %02d:%02d:%02d - %02d %d %04d/%04d ::",rtcTime[6],rtcTime[5],rtcTime[4],rtcTime[2],rtcTime[1],rtcTime[0],counterHigh,counterMed,counterLow,secondLoops);
}

void do_temp_events(int h, int m){
//  Serial.println("do_temp_events");
  byte oldTemps[4];
  for ( int i=0; i<4; i++) {
    oldTemps[i]=daTemps[i].t;
  }
  byte lastChanged[4]={255,255,255,255};
  
  for (int i=0; i<8; i++) {
    if(tempChanges[i].t!=NO_TEMP){
      //byte rtcTime[7]; //second, minute, hour, dayOfWeek, dayOfMonth, month, year;
//      sprintf(cBuffer,"%02d:%02d  tempChanges[%d] : %s to %d at %02d:%02d",h,m,i,daTemps[tempChanges[i].t].n,tempChanges[i].v,tempChanges[i].h,tempChanges[i].m);
//      Serial.print(cBuffer);
      if( h > tempChanges[i].h || ( tempChanges[i].h==h && m >= tempChanges[i].m)){
        daTemps[tempChanges[i].t].t = tempChanges[i].v ; lastChanged[tempChanges[i].t]=i;        
//        Serial.println(" CHANGED");
      } else {
//        Serial.println("");
      }
    }
  }
  for (int i=0; i<4; i++) {
    if ( daTemps[i].t != oldTemps[i] ) { // so the temp was actually changed...
      Serial.print(dateTimeStamp); Serial.print(daTemps[i].n); Serial.print(" updated according to tempChanges[");Serial.print(lastChanged[i]);Serial.print(" from "); Serial.print(oldTemps[i]);Serial.print(" to ");Serial.println(daTemps[i].t);
    } else {
      Serial.print(dateTimeStamp); Serial.print(daTemps[i].n); Serial.print(" still at ");Serial.println(oldTemps[i]);
    }
  }
}




//temp sensors
#include <DHT.h>

#define numHT 10
char shed[] = "SHED";
char ambient[] = "AMBIENT";
char resevoir[] = "RESERVOIR";
char sand[] = "SANDA";
char heatsink[] = "HEATERA";
char roof[] = "ROOF";
char foot[] = "FOOTWARM";
char missingno[] = "NOT HERE";
char sand2[] = "SANDB";
char heater[] = "HEATERB" ;
char floortemp[] = "FLOOR";

//this ordering is to reflect how the heat from the roof panel should flow to the shed...
enum eHT: byte {
  HT_SHED=0,
  HT_SANDA=1,
  HT_SANDB=2,
  HT_RESERVOIR=3,
  HT_ROOF=4,
  HT_AMBIENT=5,
  HT_FOOT=6,
  HT_HEATERA=7,
  HT_HEATERB=8,
  HT_FLOOR=9,
  } ;

char *HT_names[] = {(char*)&shed,(char*)&sand,(char*)&sand2,(char*)&resevoir,(char*)&roof,(char*)&ambient,(char*)&foot,(char*)&heatsink,(char*)&heater,(char*)floortemp,(char*)&missingno};
byte HT_Pins[] = {24,25,26,27,28,29,30,31,32,33} ;

DHT dht[] ={
  {24,DHT11},
  {25,DHT11},
  {26,DHT11},
  {27,DHT11},
  {28,DHT11},
  {29,DHT11},  
  {30,DHT11},
  {31,DHT11},
  {32,DHT11},
  {33,DHT11},
};

struct dht_readout { 
  char strF[8];
  char strH[8];
  float fF;
  float fH;
  int iF;
  int iH;
};

struct dht_readout dht_reads[numHT];

bool checkDHT(int i) {
  dht_reads[i].fF=dht[i].readTemperature(true);
  dht_reads[i].fH=dht[i].readHumidity();
  if (isnan(dht_reads[i].fF) || isnan(dht_reads[i].fH)) {
    dht_reads[i].fF=255.0f;
    dht_reads[i].fH=255.0f;
    dht_reads[i].iF=255;
    dht_reads[i].iH=255;
    sprintf(dht_reads[i].strF,"ERR");
    sprintf(dht_reads[i].strH,"ERR");
    return false;
  }
  dtostrf(dht_reads[i].fF,3,2,dht_reads[i].strF);
  dtostrf(dht_reads[i].fH,3,2,dht_reads[i].strH);
  
  dht_reads[i].iF=int(trunc(dht_reads[i].fF));
  dht_reads[i].iH=int(trunc(dht_reads[i].fH));
  return true;
}



void check_all_DHT(){
  Serial.println("CHECK ALL DHT");
  for (int i=0; i<numHT; i++){
    checkDHT(i);
    Serial.print((char*)HT_names[i]);
    Serial.print(" : ");
    Serial.print(dht_reads[i].strF);
    Serial.print(" F ");
    Serial.print(dht_reads[i].strH);
    Serial.println(" % Humidity");
  }
  Serial.println("DONE CHECK ALL DHT");
}

//relays
#define numRELAY 9
enum relays: byte {R_LOOP=0,R_XFERA=1,R_XFERB=2,R_HEAT_A_FAN=3,R_HEAT_B_FAN=4,R_HEAT_A=5,R_HEAT_B=6,R_FOOT=7,R_XFERA_HEAT=8}; 
char heatloop[]="LOOP_PUMP";  
char xfer1[]="XFERA_PUMP";
char xfer2[]="XFERB_PUMP";
char heatfan[]="HEATER_A_FAN";
char heatelem[]="HEATER_A_ELEMENTS";
char footwarm[]="FOOT_WARMER";
char heat1fans[] = "HEATER_B_FANS";
char heatelem1[] = "HEATER_B_ELEMENTS";
char xferAheat[] = "XFERA_HEAT";

char *r_names[]={(char *)&heatloop,(char *)&xfer1,(char *)&xfer2,(char *)&heatfan,(char *)&heat1fans,(char *)&heatelem,(char *)&heatelem1,(char *)&footwarm,(char *)&xferAheat};
bool run_relays[numRELAY];
byte r_pins[]={12,11,10,9,7,8,6,5,4};
int r_watts[]={
  75, // loop pump
  25, // xfer 1
  25, // xfer 2
  50, // heat 1 fans
  25, // heat 2 fan
  250, // heater 1
  200,// heater 2
   50, // foot warmer
  100, // xferA heat
  };

relays r_shutdown_list[numRELAY]={
  R_HEAT_A_FAN,
  R_HEAT_B_FAN,
  R_FOOT,
  R_HEAT_B,
  R_XFERB,
  R_HEAT_A,
  R_XFERA_HEAT,
  R_XFERA,
  R_LOOP,
};


void check_watts_ok(int current_watts, int last_min_watts){
  if ( last_min_watts > max_watts_minute ) { // we check the long plan first to avoid situation where something hits current watts over and over getting turned off and so want to just keep stuff off... ?
    sprintf(cBuffer,"%s check_watts_ok:: last_minute MAX: %d but current usage is %d",dateTimeStamp,max_watts_minute,last_min_watts);Serial.println(cBuffer);
    for (int i=0; i<numRELAY; i++){
      int relayIndex = r_shutdown_list[i];
      if (run_relays[relayIndex]){
        run_relays[relayIndex]=false;
        last_min_watts -= r_watts[relayIndex];
        sprintf(cBuffer,"%s check_watts_ok:: disabling relay %d %s trying to save %d watts to get maybe to %d",dateTimeStamp,relayIndex,r_names[relayIndex],r_watts[relayIndex],last_min_watts);Serial.println(cBuffer);
        if ( last_min_watts < max_watts_spike ) { break; }
      }
    }
  } else if ( current_watts > max_watts_spike ) {
    sprintf(cBuffer,"%s check_watts_ok:: spike MAX: %d but current usage is %d",dateTimeStamp,max_watts_spike,current_watts);Serial.println(cBuffer);
    for (int i=0; i<numRELAY; i++){
      int relayIndex = r_shutdown_list[i];
      if (run_relays[relayIndex]){
        run_relays[relayIndex]=false;
        current_watts -= r_watts[relayIndex];
        sprintf(cBuffer,"%s check_watts_ok:: disabling relay %d %s trying to save %d watts to get maybe to %d",dateTimeStamp,relayIndex,r_names[relayIndex],r_watts[relayIndex],current_watts);Serial.println(cBuffer);
        if ( current_watts < max_watts_spike ) { break; }
      }
    }
  }
}

enum ruleTypes: byte { ruleEND_LIST,ruleDHT_TARGET_TEMP_ON_HIGH, ruleDHT_TARGET_TEMP_ON_LOW, ruleDHT_LT_DHT_ON, ruleDHT_LT_DHT_OFF, ruleDHT_GT_DHT_ON, ruleDHT_GT_DHT_OFF, ruleDHT_LT_TARGET_DHT, ruleDHT_GT_TEMP_OFF, ruleDHT_LT_TEMP_ON };

struct tempRule {
  ruleTypes t;
  byte a;
  byte b;
  byte r;
  int v;
  char d[16]; // so can always be displayed on the 16x2 ? 
  
  tempRule(char *_d, ruleTypes _t, byte _a, byte _b, byte _r, int _v ) : t(_t), a(_a), b(_b), r(_r), v(_v) { memcpy(d,_d,15); d[15]=0; }
};

bool evaluateRule( struct tempRule* rule, bool verbose=true){
/* handled by emergency rules..
  if ( dht_reads[rule->a].iF == 255 && run_relays[rule->r]) {
    digitalWrite(r_pins[rule->r],HIGH);
    run_relays[rule->r]=false;
    sprintf(cBuffer,"EvaluateRule : disabling relay %d %s due to error with sensor %d %s at pin %d",rule->r,r_names[rule->r],rule->a,HT_names[rule->a],HT_Pins[rule->a]);
    Serial.println(cBuffer);
    return;
  }
  
*/
  switch(rule->t){
    case ruleEND_LIST:
        return false;
        break;
    case ruleDHT_LT_TARGET_DHT:
        if ( dht_reads[rule->a].iF + rule->v < dht_reads[rule->b].iF && !run_relays[rule->r]) {
           sprintf(cBuffer, "EvaluateRule : %s - DHT_LT_TARGET_DHT - %s : %s F + %d < %s : %s F - switching on %d %s",rule->d, HT_names[rule->a], dht_reads[rule->a].strF,HT_names[rule->b], dht_reads[rule->b].strF, rule->v, rule->r,r_names[rule->r]);
           Serial.println(cBuffer);
           run_relays[rule->r]=true; // relay will switch on loop... 
        } else if ( dht_reads[rule->a].iF + rule->v >= dht_reads[rule->b].iF  && run_relays[rule->r]) {
           sprintf(cBuffer, "EvaluateRule : %s - DHT_LT_TARGET_DHT - %s : %s F + %d >= %s : %s F - switching off %d %s",rule->d, HT_names[rule->a], dht_reads[rule->a].strF,HT_names[rule->b], dht_reads[rule->b].strF, rule->v, rule->r,r_names[rule->r]);
           Serial.println(cBuffer);
           Serial.println(cBuffer);
           run_relays[rule->r]=false; // relay will switch on loop... 
          
        } else if (verbose) {
          sprintf(cBuffer,"EvaluateRule : %s - DHT_LT_TARGET_DHT - %s : %s + %d F ? %s : %s F - nothing to change %d %s",rule->d, HT_names[rule->a], dht_reads[rule->a].strF,rule->v,HT_names[rule->b], dht_reads[rule->b].strF,rule->r,r_names[rule->r]);
          Serial.println(cBuffer);
          
        }
        break;
    case ruleDHT_TARGET_TEMP_ON_LOW:
        if ( dht_reads[rule->a].iF + rule->v < daTemps[rule->b].t && !run_relays[rule->r]) {
           sprintf(cBuffer, "EvaluateRule : %s - TARGET_TEMP_ON_LOW - DHT %d %s - %s + %d < %d F - switching on %d %s",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,daTemps[rule->b].t,rule->v,rule->r,r_names[rule->r]);
           Serial.println(cBuffer);
           run_relays[rule->r]=true; // relay will switch on loop... 
        } else if ( dht_reads[rule->a].iF + rule->v >= daTemps[rule->b].t && run_relays[rule->r]) {
           sprintf(cBuffer, "EvaluateRule : %s - TARGET_TEMP_ON_LOW - DHT %d %s - %s +%d >= %d F - switching off %d %s",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,daTemps[rule->b].t,rule->v,rule->r,r_names[rule->r]);
           Serial.println(cBuffer);
           run_relays[rule->r]=false; // relay will switch on loop... 
          
        } else if (verbose) {
          sprintf(cBuffer,"EvaluateRule : %s - TARGET_TEMP_ON_LOW - DHT %d %s - %s ? %d F - nothing to change",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,daTemps[rule->b].t);
          Serial.println(cBuffer);
        }
        break;

    case ruleDHT_GT_TEMP_OFF:
        if ( dht_reads[rule->a].iF > daTemps[rule->b].t && run_relays[rule->r]) {
           sprintf(cBuffer, "EvaluateRule : %s - ruleDHT_GT_TEMP_OFF - DHT %d %s - %s > %d F - switching off %d %s",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,daTemps[rule->b].t,rule->r,r_names[rule->r]);
           Serial.println(cBuffer);
           run_relays[rule->r]=false; // relay will switch off loop... 
          
        } else if (verbose) {
          sprintf(cBuffer,"EvaluateRule : %s - ruleDHT_GT_TEMP_OFF - DHT %d %s - %s ? %d F - nothing to change",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,daTemps[rule->b].t);
          Serial.println(cBuffer);
        }
        break;
    case ruleDHT_LT_TEMP_ON:
        if ( dht_reads[rule->a].iF < daTemps[rule->b].t - rule->v && !run_relays[rule->r]) {
           sprintf(cBuffer, "EvaluateRule : %s - ruleDHT_LT_TEMP_ON - DHT %d %s - %s < %d - %d F - switching on %d %s",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,daTemps[rule->b].t,rule->v,rule->r,r_names[rule->r]);
           Serial.println(cBuffer);
           run_relays[rule->r]=true; // relay will switch on loop... 
          
        } else if (verbose) {
          sprintf(cBuffer,"EvaluateRule : %s - ruleDHT_LT_TEMP_ON - DHT %d %s - %s ? %d - %d F - nothing to change",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,daTemps[rule->b].t,rule->v);
          Serial.println(cBuffer);
        }
        break;
    
    default:
        sprintf(cBuffer,"evaluateRule: invalid type... %d %d %d %s",rule->t,rule->a,rule->b,rule->r,rule->d);
        Serial.println(cBuffer);
  }
  return true;
}

/*
char ruleA[]= "Main loop if Roof > Reservoir";
char ruleB[]="Xfer loop if Resevoir > Sand";
char ruleC[]= "SHED TEMP UNDER COMFY - activate heat";
char ruleD[]="SHED TEMP UNDER COMFY - activate fan";
char ruleE[]="HEATSINK IN EXCESS OF MAXTEMP";
char ruleF[]="end of list";
*/
//  tempRule(char *_d;, ruleTypes _t, byte _a, byte _b, byte _r ) : t(_t), a(_a), b(_b), r(_r) { memcpy(d,_d,16); d[16]=0; }

/*
enum TEMP: byte {
  COMFYTEMP=0,
  COMFYFOOT=1,
  ELEM_MAX=2,
  NOFREEZE=3,
  NO_TEMP=255;
};
*/
struct tempRule daRules[] = {
//  { "0123456789ABCDE",  rule type, a, b, r, v }, 
    { "main: roof>res", ruleDHT_LT_TARGET_DHT, HT_RESERVOIR, HT_ROOF, R_LOOP, 0 }, //0
    { "xfer1:res>sandA", ruleDHT_LT_TARGET_DHT, HT_SANDA, HT_RESERVOIR, R_XFERA, 0 }, //1
    { "xfer2:res>sandB", ruleDHT_LT_TARGET_DHT, HT_SANDB, HT_RESERVOIR, R_XFERB, 0 }, //1
    { "hA: shed<comfy", ruleDHT_TARGET_TEMP_ON_LOW, HT_SHED, COMFYTEMP, R_HEAT_A, 2 }, //2
    { "f0: shed<heatA", ruleDHT_LT_TARGET_DHT, HT_SHED, HT_HEATERA, R_HEAT_A_FAN, 60 }, //3
    { "hB: shed<comfy", ruleDHT_TARGET_TEMP_ON_LOW, HT_SHED, COMFYTEMP, R_HEAT_B, 0 }, //2
    { "f0: shed<heatB", ruleDHT_LT_TARGET_DHT, HT_SHED, HT_HEATERB, R_HEAT_B_FAN, 40 }, //3
    { "foot warmer", ruleDHT_TARGET_TEMP_ON_LOW, HT_FOOT, COMFYFOOT, R_FOOT, 0 }, //4
    { "m: Defrost", ruleDHT_LT_TEMP_ON, HT_ROOF, DEFROST, R_LOOP, 0}, //5  so the system doesn't freeze... using 30 to account for salt water...
    { "xA: Defrost", ruleDHT_LT_TEMP_ON, HT_ROOF, DEFROST, R_XFERA, 1}, //6 ... xferA is looped with heater A / sand A by the door , inline with HeaterA which will serve to keep loop from freezing..
    { "xB: Defrost", ruleDHT_LT_TEMP_ON, HT_ROOF, DEFROST, R_XFERB, 2}, // 7
    { "xAh: Defrost", ruleDHT_TARGET_TEMP_ON_LOW, HT_ROOF, DEFROST, R_XFERA_HEAT, 1}, // 8
    { "hA: Defrost", ruleDHT_LT_TEMP_ON, HT_ROOF, DEFROST, R_HEAT_A, 3 }, //9
    { "hB: Defrost", ruleDHT_LT_TEMP_ON, HT_ROOF, DEFROST, R_HEAT_B, 4 }, //15 <- so up to 16 rules... 
    { "h0 too hot!!!!", ruleDHT_GT_TEMP_OFF, HT_HEATERA, ELEM_MAX, R_HEAT_A, 0 }, //10
    { "h1 too hot!!!!", ruleDHT_GT_TEMP_OFF, HT_HEATERB, ELEM_MAX, R_HEAT_B, 0 }, //11
    { "hot foot!!!!", ruleDHT_GT_TEMP_OFF, HT_FOOT, ELEM_MAX, R_FOOT, 0 }, //12
    { "End Of List", ruleEND_LIST,0,0,0, 0 }, //15 <- so up to 16 rules... 
    { "End Of List", ruleEND_LIST,0,0,0, 0 }, //15 <- so up to 16 rules... 
};

 
//4x4 matrix keypad stuffs here...
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {46,47,48,49}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {50,51,52,53}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


//16x2 LCD display -
#include <LiquidCrystal.h>
//rw grounded...
//LiquidCrystal object_name(rs,en,d4,d5,d6,d7)
LiquidCrystal lcd(40,41,42,43,44,45);

char lcdLine0[]="0123456789ABCDEF";
char lcdLine1[]="0123456789ABCDEF";

//all the stuffs to do the 128x64 display...
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     23 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include "shed_digits.h"
#include "shed_font.h"

static const unsigned char PROGMEM start_screen[] ={
  0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111,  
  0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  

  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  

  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  

  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101, 0b01010101,  
  };
/*
void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}*/

char textScreen[8][21]={
  "0123456789ABCDEF0123",//0
  "--------------------",//1
  "This is just a small",//2
  "test, trying to put ",//3
  "text to the OLED.   ",//4
  "                    ",//5
  "This is ONLY A TEST!",//6
  "....................",//7
};

byte textStartLine=0;
byte textCursorLine=0;

//byte textNumCols=1;   how to do cols? 
//byte textCursorCol=0;

void drawTextScreen(){
  display.clearDisplay();
  display.setRotation(0);
  display.setFont();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  for (byte i=textStartLine; i<8; i++){
    Serial.print("drawTextScreen : line ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(textScreen[i]);
    display.println(textScreen[i]);
  }
  for (byte i=0; i<textStartLine; i++){
    Serial.print("drawTextScreen : line ");
    Serial.print(i);
    Serial.print(" - ");
    Serial.println(textScreen[i]);
    display.println(textScreen[i]);
    }
 display.display();
}

void printTextLine(char *src){
  if (textStartLine == textCursorLine){
    if (textStartLine < 7){
      textStartLine++;
    } else {
      textStartLine=0;
    }
  }
  memcpy(textScreen[textCursorLine],src,20);
  if(textCursorLine<7){
    textCursorLine++;
  } else {
    textCursorLine=0;
  }
}


//real time clock stuff
#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){
  return( (val/16*10) + (val%16) );
}

void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year){
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x00); // select register
 // Wire.write(0b00011100); // write register bitmap, bit 7 is /EOS
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void strTimeStamp(char *output){
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 3);
  // request three bytes of data from DS3231 starting from register 00h
  byte second = bcdToDec(Wire.read() & 0x7f);
  byte minute = bcdToDec(Wire.read());
  byte hour = bcdToDec(Wire.read() & 0x3f);
//  while ( hour > 23 ) {setDS3231time(0,0,0,0,0,0,0,0);  }
  sprintf(output,"%02d:%02d:%02d ",hour,minute,second);
  Serial.print("strTimeStamp: ");
  Serial.println(output);
}

//power monitoring stuffs..

#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ADS;

int16_t offset = 0 ;
float last_min_secs[60];
float run_average = 0;
int run_counter = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println(__FILE__);


  
  Wire.begin();

  
  strTimeStamp(cBuffer);
//  displayTime();  
  ADS.begin();
  ADS.setGain(GAIN_TWO);
  for (int i=0; i<60; i++) { last_min_secs[i]=0.0; }

  lcd.begin(16,2);
  lcd.print("Hello!! ");
  lcd.print(cBuffer);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    lcd.setCursor(0,1);
    lcd.print("gfx failed");
    for(;;); // Don't proceed, loop forever
  }

  for (int i=0; i<numRELAY; i++){
    sprintf(cBuffer,"init relay %d : %d %s ",i,r_pins[i]);
    Serial.print(cBuffer);    
    Serial.println((char*)r_names[i]);
    lcd.clear();
    lcd.print("relay ");
    lcd.print(i);
    lcd.print(" pin ");
    lcd.println(r_pins[i]);
    lcd.setCursor(0,1);
    lcd.print((char*)r_names[i]);
  
    printTextLine(cBuffer);
    printTextLine(r_names[i]);
    drawTextScreen();

    pinMode(r_pins[i],OUTPUT);
    delay(100);
    digitalWrite(r_pins[i],HIGH);
    run_relays[i]=false;
    delay(setupDelayTime);
//  while (!customKeypad.getKey()) {delay(1);}
  }
  Serial.println("Done relays..");

  printTextLine("Done relays...");
  drawTextScreen();
//  while (!customKeypad.getKey()) {delay(1);}
  delay(setupDelayTime);

  for (int i=0; i<numHT; i++) {
    sprintf(cBuffer,"init temp %d : %d ",i,HT_Pins[i]);
    Serial.print(cBuffer);    
    Serial.println((char*)HT_names[i]);
    dht[i].begin();
    delay(setupDelayTime);
/*

    printTextLine(cBuffer);
    printTextLine((char*)HT_names[i]);
    drawTextScreen();

    lcd.clear();
    lcd.print(i);
    lcd.print(" ");
    lcd.print(HT_Pins[i]);
    lcd.setCursor(0,1);
    lcd.print((char*)HT_names[i]);

    if(!checkDHT(i)){
      Serial.println("ERROR!");
      }
    Serial.println(dht_reads[i].strF);
    lcd.setCursor(11,0);
    lcd.print(dht_reads[i].strF);
    printTextLine(dht_reads[i].strF);
    drawTextScreen();
  */  
  }
  printTextLine(" ");
  printTextLine("Done DHTs...");
  printTextLine(" ");

//  printTextLine("press any key... ");
  drawTextScreen();
//  while (!customKeypad.getKey()) {delay(1);}
//  delay(setupDelayTime);
  Serial.println("SYSTEM INIT DONE!");
  readDS3231time(&rtcTime[0], &rtcTime[1],&rtcTime[2],&rtcTime[3],&rtcTime[4],&rtcTime[5],&rtcTime[6]);
  rough_timer = millis();
}


byte setTime[8]; //for setting time using mode 2..

char dayOfWeek[8][4]= { "ERR","Tue","Wed","Thu","Fri","Sat","Sun","Mon" } ;

char setTimeString[13][17] = {
// "0123456789ABCDEF",
   "Day of Week     ",
   "YEAR    - tens  ",
   "YEAR    - ones  ",
   "MONTH   - tens  ",
   "MONTH   - ones  ",
   "DAY     - tens  ",
   "DAY     - ones  ",
   "HOUR    - tens  ",
   "HOUR    - ones  ",
   "MINUTES - tens  ",
   "MINUTES - ones  ",
   "SECONDS - tens  ",
   "SECONDS - ones  "
};



byte last_csv_minute = -1;

char str_max_min[16];
char str_run_average[16];
int int_max_min = 0; 


void loop() {
/* enum eHT: byte {
  HT_SHED=0,
  HT_HEATSINK=1,
  HT_SAND=2,
  HT_RESERVOIR=3,
  HT_ROOF=4,
  HT_AMBIENT=5
  } ;
  */
  //enum relays: byte {R_LOOP=0,R_XFER=1,R_FAN=2,R_HEAT=3}; 
/*
  if ( run_relays[R_HEAT] ) {
    if ( dht_reads[HT_HEATSINK].iF > HEAT_ELEMENT_MAX_TEMP || dht_reads[HT_SAND] > StoreTemp) {
      digitalWrite(r_pins[R_HEAT],HIGH);
      run_relays[R_HEAT]=false;
      sprintf(cBuffer,"HEATSINK temp %d\xA7F > %d\xA7F , disabling heat element");
      Serial.println(cBuffer);
    }
  }
 */
  // put your main code here, to run repeatedly:

  if (keypress_counter==0) {
    customKey = customKeypad.getKey();  
    if (customKey){
     display.drawBitmap(120,0,start_screen,8,8,0);
     Serial.print(dateTimeStamp);
     Serial.println(customKey);
     display.setCursor(120, 0);
     display.setTextSize(1); // Draw 1X-scale text
     display.println(customKey);
     keypress_counter=499 + millis();
     }
  } else if ( millis() > keypress_counter ){ 
    customKey = 0;
    keypress_counter = 0;
  }
  //process current mode
  switch(mode){
    case 0:
      if (customKey=='A'){ mode=1;}
      else if (customKey=='B') { mode=2; }
      else if (customKey=='C') { mode=3; }
      else if (mode0_display_second != rtcTime[0]){
        mode0_display_second = rtcTime[0];
        display.clearDisplay();
        display.setTextSize(1); // Draw 1X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.print(F("HFS3001     "));
        if(rtcTime[2]<10){ display.print("0");}
        display.print(rtcTime[2]);
        display.print(":");
        if(rtcTime[1]<10){ display.print("0");}
        display.print(rtcTime[1]);
        display.print(":");
        if(rtcTime[0]<10){ display.print("0");}
        display.println(rtcTime[0]);

        for (int i=0 ; i<numRELAY; i++){
          if (run_relays[i]) { display.print("X "); }
          else { display.print("_ "); }
        }
        display.println(" ");
        for (int i=0 ; i<numHT; i++){
          sprintf(cBuffer,"%13s %-5sF",HT_names[i],dht_reads[i].strF);
          display.println(cBuffer);
        }
      }
      break;
    case 1:
      if (customKey=='B') { mode=0; }
      break;
    case 2:
      if (customKey=='A') { mode=0; }
      else if (customKey=='B'){
        sprintf(cBuffer,"Setting the RTC to values : %s 20%02d/%2d/%2d %2d:%02d:%02d",dayOfWeek[setTime[3]],setTime[6],setTime[5],setTime[4],setTime[2],setTime[1],setTime[0]); 
        Serial.println(cBuffer);
        // void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
        setDS3231time(setTime[0],setTime[1],setTime[2],setTime[3],setTime[4],setTime[5],setTime[6]);
        mode=0;
       }
      if (counterLow==0){
        
      display.clearDisplay();
      display.setTextSize(1); // Draw 1X-scale text
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0);

      display.println("Set RTC time");
      display.println("A: exit w/o saving");
      display.println("B: confirm setting");
      sprintf(cBuffer,"%s 20%02d/%02d/%02d",dayOfWeek[setTime[3]],setTime[6],setTime[5],setTime[4]);
      display.println(cBuffer);
      switch (setTime[7]){
          default:
              display.println("---   -- -- --");
              break;
          case 0:
              display.println("XXX   -- -- --");
              break;
          case 1:
              display.println("---   X- -- --");
              break;
          case 2:
              display.println("---   -X -- --");
              break;
          case 3:
              display.println("---   -- X- --");
              break;
          case 4:
              display.println("---   -- -X --");
              break;
          case 5:
              display.println("---   -- -- X-");
              break;
          case 6:
              display.println("---   -- -- -X");
              break;
           }
      sprintf(cBuffer,"        %02d:%02d:%02d",setTime[2],setTime[1],setTime[0]);
      display.println(cBuffer);
      switch (setTime[7]){
          default:
              display.println("        -- -- --");
              break;
          case 7:
              display.println("        X- -- --");
              break;
          case 8:
              display.println("        -X -- --");
              break;
          case 9:
              display.println("        -- X- --");
              break;
          case 10:
              display.println("        -- -X --");
              break;
          case 11:
              display.println("        -- -- X-");
              break;
          case 12:
              display.println("        -- -- -X");
              break;
           }
       }

      if (customKey){ 
        sprintf(cBuffer,"Setting RTC time - set %d, key %c",setTime[7],customKey);
        Serial.println(cBuffer);
      switch (customKey) {
            case 'C': // backward to last value
                setTime[7]-=2;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                //whatever the number...
                switch(setTime[7]){
                  default:
                      break;
                  case 0: // day of week
                      setTime[3]=customKey-'0';
                      break;
                  case 1: // year, tens digit
                      setTime[6] = (setTime[6] % 10) + ((customKey - '0') * 10 );
                      break;
                  case 2: // year, ones digit
                      setTime[6] = setTime[6] - (setTime[6] % 10) + (customKey - '0') ;
                      break;
                  case 3: // month, tens digit
                      setTime[5] = (setTime[5] % 10) + ((customKey - '0') * 10 );
                      break;
                  case 4: // month, ones digit
                      setTime[5] = setTime[5] - (setTime[5] % 10) + (customKey - '0') ;
                      break;
                  case 5: // date, tens digit
                      setTime[4] = (setTime[4] % 10) + ((customKey - '0') * 10 );
                      break;
                  case 6: // date, ones digit
                      setTime[4] = setTime[4] - (setTime[4] % 10) + (customKey - '0') ;
                      break;
                  case 7: // hour, tens digit
                      setTime[2] = (setTime[2] % 10) + ((customKey - '0') * 10 );
                      break;
                  case 8: // hour, ones digit
                      setTime[2] = setTime[2] - (setTime[2] % 10) + (customKey - '0') ;
                      break;
                  case 9: // minutes, tens digit
                      setTime[1] = (setTime[1] % 10) + ((customKey - '0') * 10 );
                      break;
                  case 10: // minutes, ones digit
                      setTime[1] = setTime[1] - (setTime[1] % 10) + (customKey - '0') ;
                      break;
                  case 11: // seconds, tens digit
                      setTime[0] = (setTime[0] % 10) + ((customKey - '0') * 10 );
                      break;
                  case 12: // seconds, ones digit
                      setTime[0] = setTime[0] - (setTime[0] % 10) + (customKey - '0') ;
                      break;
                  
                }
            case 'D': // forward to next value
                setTime[7]+=1;
            default:
                if (setTime[7] < 0 || setTime[7] > 12) { setTime[7]=0;}
                break;
      }
        sprintf(cBuffer,"setTime values : setTime[7]:%d - %s 20%02d/%2d/%2d %2d:%02d:%02d",setTime[7],dayOfWeek[setTime[3]],setTime[6],setTime[5],setTime[4],setTime[2],setTime[1],setTime[0]); 
        Serial.println(cBuffer);
        }
      if (counterLow==0){
        sprintf(cBuffer,"setTime values : setTime[7]:%d - %s 20%02d/%2d/%2d %2d:%02d:%02d",setTime[7],dayOfWeek[setTime[3]],setTime[6],setTime[5],setTime[4],setTime[2],setTime[1],setTime[0]); 
        Serial.println(cBuffer);
        strcpy(lcdLine1,setTimeString[setTime[7]]);        
      }
      break;
    case 3:
      if (customKey=='A') { mode=0; }
      else if (mode0_display_second != rtcTime[0]){
        mode0_display_second = rtcTime[0];
        display.clearDisplay();
        display.setFont(&shed_digits);
        display.setTextSize(1); // Draw 1X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,13);
        for (int i=0 ; i<numHT; i++){
          strcpy(cBuffer,"---------");
          for(int j=0; j<9; j++){
            if(HT_names[i][j] > 0) { cBuffer[j]=HT_names[i][j]; }
            else { break; }
          }
          display.print(cBuffer);
          if ( dht_reads[i].iF > 200) { display.println(",ERROR"); }
          else {
          if ( dht_reads[i].iF >= 100 ){ 
            display.print("/"); 
            if (dht_reads[i].iF < 110) {display.print("0"); }
            display.print(dht_reads[i].iF-100);
          }
          else { 
            display.print(","); 
            if ( dht_reads[i].iF < 10 ) { display.print(" "); }
            display.print(dht_reads[i].iF); 
          }
          display.println("!");
          } 
        }
        display.setFont(&shed_font_4x5_mono);
        display.setCursor(0,122);
        for (int i=0 ; i<numRELAY; i++){
          if (run_relays[i]) { display.print("X"); }
          else { display.print("_"); }
       }
       display.println("");
        display.print(int_max_min); display.print(" W "); display.print(light_level);
      }
      break;
  }

  //process switching to other mode...
  if (oldmode != mode){
    sprintf(cBuffer,"%s switching mode from %d to %d",dateTimeStamp,oldmode,mode);
    Serial.println(cBuffer);
    switch(mode){
      default:
              mode=0;
      case 0:
        display.setRotation(0);
        display.setFont();
        mode0_display_second=69;
        break;
      case 1:
        display.clearDisplay();
        display.setFont();
        display.setRotation(0);
        display.setTextSize(2); // Draw 1X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(16,24);
        display.println("WHAT UP?");
        break;
      case 2:
        display.setRotation(0);
        display.setFont();
        strcpy(lcdLine1,"SetTime!    ");
        for (int i=0; i<7; i++) {
          setTime[i]=rtcTime[i];          
        }
        setTime[7]=0;        
        break;
      case 3:
        display.setRotation(1);
        mode0_display_second=69;
        break; 
    }
  oldmode=mode ; 
  lcd.setCursor(0,1);
  lcd.print(lcdLine1);
  }

  
  if (counterLow==0) {
    if (counterMed==0) {
      // stuff to do every second.. well, try to.. this is done rather than polling the RTC directly every loop...  
    byte last_second = rtcTime[0];
    readDS3231time(&rtcTime[0], &rtcTime[1],&rtcTime[2],&rtcTime[3],&rtcTime[4],&rtcTime[5],&rtcTime[6]);
    if ( rtcTime[0] > 59 || rtcTime[1] > 59 || rtcTime[2] > 23 ) {
      if( rough_timer < millis() ) {
        Serial.println("rough_timer too low, adjusting... "); rough_timer = millis() + 1000;
      }
      Serial.print(dateTimeStamp);Serial.println("ERROR! Disabling all relays and rechecking RTC!");
      makeDateTimeStamp();
      for (int i=0; i<numRELAY; i++ ){ digitalWrite(r_pins[i],HIGH); }
      while ( rough_timer > millis() && ( rtcTime[0] > 59 || rtcTime[1] > 59 || rtcTime[2] > 23 ) ){
        readDS3231time(&rtcTime[0], &rtcTime[1],&rtcTime[2],&rtcTime[3],&rtcTime[4],&rtcTime[5],&rtcTime[6]);      
        }
      if (rtcTime[0] > 59 ) {
        Serial.println("ERROR! RTC still not reading OK, switching to rough_timer");
        if (last_second == 59 ) { 
          rtcTime[0] = 0;
          if (rtcTime[1] > 59 ) {
            if (last_csv_minute == 59) { rtcTime[1]=0; } else { rtcTime[1]=last_csv_minute + 1; } 
          } 
        } else {
          rtcTime[0] = last_second + 1; 
          rtcTime[1] = last_csv_minute ; 
        }
        rough_timer += 1000 ; // setting timer value to use next time
      }
    } 
 
    if(rtcTime[0] == last_second) {
        while(rtcTime[0] == last_second) { // this is to stall until next second.. 
          secondLoops +=2;
         // Serial.print("Secondloop too short, increasing by 2 to ");
         // Serial.println(secondLoops);
          if (secondLoops > 32000) { secondLoops = 32000; }
          delay(2);
          readDS3231time(&rtcTime[0], &rtcTime[1],&rtcTime[2],&rtcTime[3],&rtcTime[4],&rtcTime[5],&rtcTime[6]);
        }
    }
    else {
      if ( last_second > rtcTime[0] ) { last_second -= 60 ; }
      if (rtcTime[0] - last_second > 1 ) {
        if( secondLoops > 1) { 
          secondLoops -=1; if ( secondLoops < 0 ) secondLoops = 0 ; }
       // Serial.print("Secondloop too long, reducing by 1 to ");
      //  Serial.println(secondLoops);
      } else {
      //  Serial.print("Secondloop on target - ");
       //   Serial.println(secondLoops);        
     }
    }
      if (rtcTime[3] < 0 || rtcTime[3] > 7 ) { 
        Serial.print("Got bad value for dayOfWeek: ");
        Serial.println(rtcTime[3]);
        rtcTime[3]=0; 
      }
      makeDateTimeStamp();

      counterMed=10;
     int16_t maxi = -32000;
     int16_t mini = 32000;
     int16_t sample = 0; 
     
     unsigned long int stop_at = millis() + WATTS_MILLIS;
     while (millis() < stop_at ) {
         sample = ADS.readADC_Differential_0_1();
        if ( sample < mini ) { mini = sample; }
        else if (sample > maxi) { maxi = sample; }
     }
     int16_t old_offset = offset; 
    offset = (maxi + mini)/2; 
    maxi -= offset;
    mini -= offset;
    float max_min ; 
    if (mini < 0) { max_min=watts_factor*ADS.computeVolts((maxi/2 - mini/2)); } 
    else { max_min=watts_factor*ADS.computeVolts(maxi); }
    dtostrf(max_min,3,2,str_max_min);
    int_max_min = int(trunc(max_min));
    run_average -= last_min_secs[run_counter];
    last_min_secs[run_counter]=max_min/60.0;
    run_average += last_min_secs[run_counter] ;
    dtostrf(run_average,3,2,str_run_average);

    run_counter += 1;
    if (run_counter >=60) { run_counter=0; }
  
      makeDateTimeStamp();
 //     sprintf(cBuffer,"%s %2d / %2d - %s (%d) - secondLoops: %d - old_offset: %d offset: %d maxi: %d - mini: %d - Watts: %s - last_min_watts: %s",dateTimeStamp,last_second,rtcTime[0],dayOfWeek[rtcTime[3]],rtcTime[3],secondLoops,maxi,mini,str_max_min,str_run_average);
      sprintf(cBuffer,"%s %2d / %2d - %s (%d) - secondLoops: %d",dateTimeStamp,last_second,rtcTime[0],dayOfWeek[rtcTime[3]],rtcTime[3],secondLoops);
      Serial.print(cBuffer);
//      sprintf(cBuffer," - old_offset: %d offset: %d maxi: %d - mini: %d - Watts: %s - last_min_watts: %s",old_offset,offset,maxi,mini,str_max_min,str_run_average);
      sprintf(cBuffer," - maxi: %d mini: %d - Watts: %s - last_min_watts: %s",maxi,mini,str_max_min,str_run_average);
      Serial.println(cBuffer);
      sprintf(lcdLine0,"%2d/%2d   %2d:%02d:%02d",rtcTime[5],rtcTime[4],rtcTime[2],rtcTime[1],rtcTime[0]);

    check_watts_ok(int_max_min,int(trunc(run_average))); // enforce power limits

     if (counterHigh==0){ //stuff to do every 10s
        
         // strcpy(lcdLine1,"________________");
        //sprintf(cBuffer,"LCD display : %s -- %s",lcdLine0,lcdLine1);
        //Serial.println(cBuffer);
        counterHigh=9; //do stuff every 10s..
        do_temp_events(rtcTime[2],rtcTime[1]);
        light_level=analogRead(A0);
        Serial.print("Light Level: "); Serial.print(light_level); Serial.println("");
        check_all_DHT();
        int i=0; //struct tempRule* rule
        for ( tempRule r : daRules ) { 
          if (!evaluateRule(&r,false)) break ;
          i++ ; 
        }

       
        sprintf(cBuffer,"%s %d DONE RULES",dateTimeStamp,last_second,rtcTime[0],dayOfWeek[rtcTime[3]],secondLoops);
        Serial.println(cBuffer);
        Serial.print(i);
        Serial.println(" done rules...");

        int watts = 5;
        Serial.print("Relays: ");
        for (int i=0; i < numRELAY; i++) {
          Serial.print((char*)r_names[i]);
          if ( run_relays[i]) { 
            Serial.print(": ON "); 
            watts += r_watts[i];
            }
          else { Serial.print(": OFF "); }
        }
        Serial.print(" :: ~");
        Serial.print(watts);
        Serial.println(" watts");
    } else {
      counterHigh--;

        if ( last_csv_minute != rtcTime[1] ){
          last_csv_minute = rtcTime[1];
          Serial.print("CSV_HEADERS,time,");
          for (int i=0; i<numHT; i++){
            Serial.print(HT_names[i]);
            Serial.print(",");
          }
          for (int i=0; i<numRELAY; i++){
            Serial.print((char*)r_names[i]);
            Serial.print(",");
          }
          Serial.print("Watts,");
          for (int i=0; i<numHT; i++) {
            Serial.print(HT_names[i]); Serial.print("_HUM,");
          }
          Serial.println("light, ");
          sprintf(cBuffer,"CSV_VALUES, 20%02d/%02d/%02d %02d:%02d,",rtcTime[6],rtcTime[5],rtcTime[4],rtcTime[2],rtcTime[1]);
          Serial.print(cBuffer);
          for (int i=0; i<numHT; i++){
            if (dht_reads[i].iF != 255) { Serial.print(dht_reads[i].strF); }
            Serial.print(",");
          }
          for (int i=0; i<numRELAY; i++){
            Serial.print(run_relays[i]);
            Serial.print(",");
          }
          Serial.print(str_run_average); Serial.print(",");
          for (int i=0; i<numHT; i++){            
            if (dht_reads[i].iH != 255) { Serial.print(dht_reads[i].strH); }
            Serial.print(",");
          }
          Serial.print(light_level); Serial.println(",");
        }

     }
    
 
    }
    counterLow=secondLoops;
    counterMed--;
    // this stuff runs every 1/10 second...

    for (int i=0; i < numRELAY; i++) {
       if (run_relays[i]) { digitalWrite(r_pins[i],LOW); }
       else { digitalWrite(r_pins[i],HIGH); }
    }

    lcd.setCursor(0,0);
    lcd.print(lcdLine0);
    lcd.setCursor(0,1);
    lcd.print(lcdLine1);  
    display.display();
//  Serial.print(counterHigh);  Serial.print(" ");  Serial.println(counterLow);

  } else {
    counterLow--;
  }

//  sprintf(cBuffer,"counters - %d %d %d",counterHigh,counterMed,counterLow);
//  Serial.println(cBuffer);
}

void displayTime(){
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231
  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
  Serial.print(hour, DEC);
  // convert the byte variable to a decimal number when displayed
  Serial.print(":");
  if (minute<10){
    Serial.print("0");
  }
  Serial.print(minute, DEC);
  Serial.print(":");
  if (second<10){
    Serial.print("0");
  }
  Serial.print(second, DEC);
  Serial.print(" ");
  Serial.print(dayOfMonth, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(year, DEC);
  Serial.print(" Day of week: ");
  switch(dayOfWeek){
  case 1:
    Serial.println("Sunday");
    break;
  case 2:
    Serial.println("Monday");
    break;
  case 3:
    Serial.println("Tuesday");
    break;
  case 4:
    Serial.println("Wednesday");
    break;
  case 5:
    Serial.println("Thursday");
    break;
  case 6:
    Serial.println("Friday");
    break;
  case 7:
    Serial.println("Saturday");
    break;
  default:
    Serial.println("Blataday");
    break;
  }
}
