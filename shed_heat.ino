#define ComfyTemp 69
#define HEAT_ELEMENT_MAX_TEMP 120
#define setupDelayTime 100

char cBuffer[128];


//temp sensors
#include <DHT.h>

#define numHT 6
char shed[] = "SHED";
char ambient[] = "AMBIENT";
char resevoir[] = "RESERVOIR";
char sand[] = "SAND";
char heatsink[] = "HEATSINK";
char roof[] = "ROOF";

//this ordering is to reflect how the heat from the roof panel should flow to the shed...
enum eHT: byte {
  HT_SHED=0,
  HT_HEATSINK=1,
  HT_SAND=2,
  HT_RESERVOIR=3,
  HT_ROOF=4,
  HT_AMBIENT=5
  } ;

void *HT_names[] = {&shed,&heatsink,&sand,&resevoir,&roof,&ambient};
byte HT_Pins[] = {24,25,26,27,28,29} ;

DHT dht[] ={
  {24,DHT11},
  {25,DHT11},
  {26,DHT11},
  {27,DHT11},
  {28,DHT11},
  {29,DHT11},  
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
    Serial.println("*F ");
  }
}

//relays
#define numRELAY 4
enum relays: byte {R_LOOP=0,R_XFER=1,R_FAN=2,R_HEAT=3}; 
char heatloop[]="LOOP PUMP";
char xfer[]="XFER PUMP";
char heatfan[]="HEAT FAN";
char heatelem[]="12V DC HEAT";
void *r_names[]={&heatloop,&xfer,&heatfan,&heatelem};
bool run_relays[numRELAY];
byte r_pins[]={12,11,10,9};


enum ruleTypes: byte { ruleEND_LIST,ruleDHT_TARGET_TEMP_ON_HIGH, ruleDHT_TARGET_TEMP_ON_LOW, ruleDHT_LT_DHT_ON, ruleDHT_LT_DHT_OFF, ruleDHT_GT_DHT_ON, ruleDHT_GT_DHT_OFF, ruleDHT_LT_TARGET_DHT, ruleDHT_GT_TEMP_OFF };

struct tempRule {
  ruleTypes t;
  byte a;
  byte b;
  byte r;
  void *d;
  
  tempRule(ruleTypes _t, byte _a, byte _b, byte _r, char *_d) : t(_t), a(_a), b(_b), r(_r), d(_d) {}
};

void evaluateRule( struct tempRule* rule, bool verbose=true){
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
    case ruleDHT_TARGET_TEMP_ON_LOW:
        if ( dht_reads[rule->a].iF < rule->b && !run_relays[rule->r]) {
           sprintf(cBuffer, "EvaluateRule : %s - TARGET_TEMP_ON_LOW - DHT %d %s - %s > %d F - switching on %d %s",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,rule->b,rule->r,r_names[rule->r]);
           Serial.println(cBuffer);
           run_relays[rule->r]=true; // relay will switch on loop... 
        } else if ( dht_reads[rule->a].iF > rule->b && run_relays[rule->r]) {
           sprintf(cBuffer, "EvaluateRule : %s - TARGET_TEMP_ON_LOW - DHT %d %s - %s < %d F - switching off %d %s",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,rule->b,rule->r,r_names[rule->r]);
           Serial.println(cBuffer);
           run_relays[rule->r]=false; // relay will switch on loop... 
          
        } else if (verbose) {
          sprintf(cBuffer,"EvaluateRule : %s - TARGET_TEMP_ON_LOW - DHT %d %s - %s ? %d F - nothing to change",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,rule->b);
          Serial.println(cBuffer);
        }
        break;

    case ruleDHT_GT_TEMP_OFF:
        if ( dht_reads[rule->a].iF > rule->b && run_relays[rule->r]) {
           sprintf(cBuffer, "EvaluateRule : %s - TARGET_TEMP_ON_HIGH - DHT %d %s - %s > %d F - switching off %d %s",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,rule->b,rule->r,r_names[rule->r]);
           Serial.println(cBuffer);
           run_relays[rule->r]=false; // relay will switch on loop... 
          
        } else if (verbose) {
          sprintf(cBuffer,"EvaluateRule : %s - TARGET_TEMP_ON_HIGH - DHT %d %s - %s ? %d F - nothing to change",rule->d,rule->a, HT_names[rule->a], dht_reads[rule->a].strF,rule->b);
          Serial.println(cBuffer);
        }
        break;

    default:
        sprintf(cBuffer,"evaluateRule: invalid type... %d %d %d %s",rule->t,rule->a,rule->b,rule->r,rule->d);
        Serial.println(cBuffer);
  }
}


char ruleA[]= "Main loop if Roof > Reservoir";
char ruleB[]="Xfer loop if Resevoir > Sand";
char ruleC[]= "SHED TEMP UNDER COMFY - activate heat";
char ruleD[]="SHED TEMP UNDER COMFY - activate fan";
char ruleE[]="HEATSINK IN EXCESS OF MAXTEMP";
char ruleF[]="end of list";

//  tempRule(ruleTypes _t, byte _a, byte _b, byte _r, char *_d) : t(_t), a(_a), b(_b), r(_r), d(_d) {}

struct tempRule daRules[] = {
  { ruleTypes::ruleDHT_LT_TARGET_DHT, HT_RESERVOIR, HT_ROOF, R_LOOP, (void*)&ruleA } ,
  { ruleTypes::ruleDHT_LT_TARGET_DHT, HT_SAND, HT_RESERVOIR, R_XFER, (void*)&ruleB },
  { ruleTypes::ruleDHT_TARGET_TEMP_ON_LOW, HT_SHED, ComfyTemp, R_HEAT, (void*)&ruleC },
  { ruleTypes::ruleDHT_TARGET_TEMP_ON_LOW, HT_SHED, ComfyTemp, R_FAN, (void*)&ruleD },
  { ruleTypes::ruleDHT_GT_TEMP_OFF, HT_HEATSINK, HEAT_ELEMENT_MAX_TEMP, R_HEAT, (void*)&ruleE },
  { ruleTypes::ruleEND_LIST,0,0,0, (void*)&ruleF }
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  strTimeStamp(cBuffer);
//  displayTime();  
  Serial.println("init...");
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

  // Show initial display buffer contents on the screen --
  display.clearDisplay();
  display.drawBitmap(0,0,start_screen,128,64,1);
  display.display();
  Serial.println("splash screen - hit any button");
  lcd.setCursor(0,1);
  lcd.print("splash!");
//  while (!customKeypad.getKey()) {delay(1);}

  delay(setupDelayTime);

  drawTextScreen();

  Serial.println("text screen test - hit any button");
  lcd.setCursor(0,1);
  lcd.print("TEXT SCREEN!!");
//  while (!customKeypad.getKey()) {delay(1);}
  delay(setupDelayTime);


  
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
    sprintf(cBuffer,"init temp %d : %d ",i,r_pins[i]);
    Serial.print(cBuffer);    
    Serial.println((char*)HT_names[i]);
    printTextLine(cBuffer);
    printTextLine((char*)HT_names[i]);
    drawTextScreen();

    lcd.clear();
    lcd.print(i);
    lcd.print(" ");
    lcd.print(HT_Pins[i]);
    lcd.setCursor(0,1);
    lcd.print((char*)HT_names[i]);
    dht[i].begin();

    if(!checkDHT(i)){
      Serial.println("ERROR!");
/*
      printTextLine("ERROR!");
      printTextLine((char*)HT_names[i]);
      drawTextScreen();
      char leKey=customKeypad.getKey();
      while (leKey==0) {
        leKey=customKeypad.getKey();
        delay(1);
        }
      Serial.println(leKey);
      if (leKey=='D') { break; }
      */    
      }
    Serial.println(dht_reads[i].strF);
    lcd.setCursor(11,0);
    lcd.print(dht_reads[i].strF);
    printTextLine(dht_reads[i].strF);
    drawTextScreen();
    delay(setupDelayTime);
    
  }
  printTextLine(" ");
  printTextLine("Done DHTs...");
  printTextLine(" ");
//  printTextLine("press any key... ");
  drawTextScreen();
//  while (!customKeypad.getKey()) {delay(1);}
  delay(setupDelayTime);
  Serial.println("SYSTEM INIT DONE!");
}

int counterLow=0; //setting up a ghetto counter to be like counterHigh is seconds
int counterMed=10; // tries to be 1/th second...
int counterHigh=0; // ticks once a second..
int keypress_counter=0; // for reading key presses... 
char customKey = 0; 
int mode=0 ; // for switching between menus and screens...
int oldmode=-1; 

byte rtcTime[7]; //second, minute, hour, dayOfWeek, dayOfMonth, month, year;

int secondLoops=3500; //actually 1/100th of second...
byte mode0_display_second=99;

byte setTime[8]; //for setting time using mode 2..

char dayOfWeek[8][4]= { "Mon","Tue","Wed","Thu","Fri","Sat","Sun","ERR" } ;

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
     Serial.println(customKey);
     display.setCursor(120, 0);
     display.setTextSize(1); // Draw 1X-scale text
     display.println(customKey);
     keypress_counter=499;
     }
  } else {
    keypress_counter--;
    customKey = 0;
  }
  //process current mode
  switch(mode){
    case 0:
      if (customKey=='A'){ mode=1;}
      else if (customKey=='B') { mode=2; }
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
  }

  //process switching to other mode...
  if (oldmode != mode){
    sprintf(cBuffer,"switching mode from %d to %d",oldmode,mode);
    Serial.println(cBuffer);
    switch(mode){
      default:
              mode=0;
      case 0:
        mode0_display_second=69;
        break;
      case 1:
        display.clearDisplay();
        display.setTextSize(2); // Draw 1X-scale text
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(16,24);
        display.println("WHAT UP?");
        break;
      case 2:
        strcpy(lcdLine1,"SetTime!    ");
        for (int i=0; i<7; i++) {
          setTime[i]=rtcTime[i];          
        }
        setTime[7]=0;
        
        break;
    }
  oldmode=mode ; 
  lcd.setCursor(0,1);
  lcd.print(lcdLine1);
  }

  
  if (counterLow==0) {
    if (counterMed==0) {
      counterMed=10;
      // stuff to do every second..
  for (int i=0; i < numRELAY; i++) {
    if (run_relays[i]) { digitalWrite(r_pins[i],LOW); }
    else { digitalWrite(r_pins[i],HIGH); }
  }
  
       byte last_second = rtcTime[0];
    readDS3231time(&rtcTime[0], &rtcTime[1],&rtcTime[2],&rtcTime[3],&rtcTime[4],&rtcTime[5],&rtcTime[6]);
    if (rtcTime[3] > 6 || rtcTime[3] < 0 ) { rtcTime[3]=7; }
    sprintf(cBuffer,"counterHigh: %2d - seconds %2d / %2d - %s 20%02d/%02d/%02d %2d:%02d:%02d",counterHigh,last_second,rtcTime[0],dayOfWeek[rtcTime[3]],rtcTime[6],rtcTime[5],rtcTime[4],rtcTime[2],rtcTime[1],rtcTime[0]);
    Serial.println(cBuffer);

    sprintf(lcdLine0,"%2d/%2d   %2d:%02d:%02d",rtcTime[5],rtcTime[4],rtcTime[2],rtcTime[1],rtcTime[0]);

  //  if (rtcTime[0] % 2 == 0 ) { // check only every 2s 
      if ( last_second > rtcTime[0] ) { last_second -= 60 ; }
      
    if (rtcTime[0] > last_second) {
      if (rtcTime[0] - last_second > 1 ) {
        secondLoops -=10;
        Serial.print("Secondloop too long, reducing by 10 to ");
        Serial.println(secondLoops);
      } 
    }else if(rtcTime[0] == last_second) {
        secondLoops +=1;
        Serial.print("Secondloop too short, increasing by 1 to ");
        Serial.println(secondLoops);        
      }
     else {
      Serial.print("Secondloop on target - ");
        Serial.println(secondLoops);        
      
     }
    //}
      if (counterHigh==0){ //stuff to do every 10s
        
         // strcpy(lcdLine1,"________________");
        sprintf(cBuffer,"LCD display : %s -- %s",lcdLine0,lcdLine1);
        Serial.println(cBuffer);
        counterHigh=9; //do stuff every 10s..
        check_all_DHT();

 for (int i=0; i<255; i++){
  if ( daRules[i].t == ruleEND_LIST ) {
    Serial.print(i);
    Serial.println(" done rules...");
    break;
  }
  evaluateRule(&daRules[i],false);
 }


    } else {
      counterHigh--;
     }
    }
    
 
        
    counterLow=secondLoops;
    counterMed--;
//    if(counterMed==0){counterHigh--;}

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
