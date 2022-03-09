#include <TimerOne.h>
#include <ClickEncoder.h>

//Encoder enc(A0,A1);
ClickEncoder *encoder;

float dis_value=888;
bool led_state=HIGH;

int pins[4]={2,3,4,5};
int bit_id[3]={6,7,8};

const int led_pin=9;
const int btn=10;
const int en_btn=11;
const int bi=12;
const int lt=13;
float vs=0;
uint16_t hdg_sel=0;
uint8_t hdg_state=0;
uint16_t alt_sel=0;
uint16_t spd_sel=0;
int counter = 0; 
int currentStateCLK;
int previousStateCLK; 


void getDataFromFS2020(void);
void heading_bug_inc(void);
void heading_bug_dec(void);
void heading_bug_set(uint16_t _hdg_set_value);
void ap_panel_heading_hold(void);

int16_t last=0, value=0;

void timerIsr() {
  encoder->service();
}

void toggle_led(void)
{
  if(led_state==LOW)
  {
    led_state=HIGH;
    digitalWrite(led_pin, led_state);
  }
  else
  {
    led_state=LOW;
    analogWrite(led_pin, 100);
  } 
}
void set_led(void)
{
  analogWrite(led_pin, 100);
}
void reset_led(void)
{
  analogWrite(led_pin, 0);
}
void set_led_state(void)
{
  digitalWrite(led_pin, led_state);
}
void set_dis_single(int _id, int _value)
{
  digitalWrite(bit_id[_id], LOW);
  for(int i=0;i<4;i++)
  {
    bool state=bitRead(_value, i);  
    digitalWrite(pins[i],state);
  }
  delay(20);
  digitalWrite(bit_id[_id], HIGH);
}
void set_dis(int _value)
{
  int value1[3]={0,0,0};
   value1[2]=_value/100;
   value1[1]=(_value-value1[2]*100)/10;
   value1[0]=_value-value1[2]*100-value1[1]*10;
  for(int i=0;i<3;i++)
  {
    set_dis_single(i,value1[i]);
  }
}

void setup()
{
  Serial.begin(115200);
//  Serial.setTimeout(50);
  encoder = new ClickEncoder(A0, A1, 11);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 
  
  last = -1;
  
  pinMode(led_pin, OUTPUT);
   pinMode(bi, OUTPUT);
  for(int i=2;i<10;i++)
  {
    pinMode(i, OUTPUT);
  }
  pinMode(btn, INPUT_PULLUP);
  pinMode(en_btn, INPUT_PULLUP);

  pinMode(lt, INPUT);
//  set_dis(dis_value);
  digitalWrite(led_pin, led_state);
  digitalWrite(bi, HIGH);
}


int dis_value_old=0;
bool flag_minus=LOW;
bool flag_plus=LOW;
float newPosition = 0;
float oldPosition  = 0;
void loop()
{   
      if(digitalRead(lt)==LOW)
  {
    set_led();
  }
  else{
    

  value = encoder->getValue();
  newPosition+=float(value)/2;
//  Serial.println(newPosition);
float poserr=newPosition-oldPosition;
  if (abs(poserr)>0.9) 
  {
    if (poserr>0) {
      heading_bug_inc();
    }
    else if(poserr<0){
      heading_bug_dec();
    }
    oldPosition=newPosition;
  }
   
  getDataFromFS2020();
  //light self check

  
  if(hdg_sel==000)
  {
    hdg_sel=360;
  }
  if(hdg_sel!=dis_value)
  {
    dis_value=hdg_sel;
  //  dis_value=max(min(dis_value,999),0);
  int int_dis_value=int(dis_value);
  set_dis(int_dis_value);
  }
  
  
  bool btn_e=digitalRead(en_btn);
  if(btn_e==LOW)
  {
    ap_panel_heading_set(!hdg_state);
    delay(200);
  }
  bool btn_t=digitalRead(btn);
  if(btn_t==LOW)
  {
//    ap_panel_heading_hold();
    ap_panel_heading_set(!hdg_state);
    delay(200);
  }  

  digitalWrite(led_pin,!hdg_state);
    }
}

void getDataFromFS2020(void) {
  //Declaration of variables to store the incoming data
  String input = "@"; //General input buffer
  int value;  //Value memory
  int index;  //Index memory
 if (Serial.available() > 0) {
    Serial.readStringUntil('@'); //Look out for first Indicator
    input += Serial.readStringUntil('/') + "/"; //read the ID
    index = Serial.readStringUntil('=').toInt();    
    
    if (input.indexOf("@763/") != -1) { //Check for Vertical Speed ID
      vs = Serial.readStringUntil('$').toInt();  //Saves the value as int to "value" Variable for later use
        
      //Do something with the values...
      //like displaying or sending something
      //Or include a function like:
      //void refreshStepperMotorPosition(int value)

    } 

    if (input.indexOf("@86/") != -1) { //Check for hdg ID
      float f_hdg_sel = Serial.readStringUntil('$').toFloat();  //Saves the value as int to "value" Variable for later use
      hdg_sel = (int)round(f_hdg_sel);
    } 
    if (input.indexOf("@87/") != -1) { //Check for hdg_status ID
      hdg_state = Serial.readStringUntil('$').toInt();  //Saves the value as int to "value" Variable for later use
    } 
//    if (input.indexOf("@90/") != -1) { //Check for AP Master ID -> Gives Bool Value
//      value = Serial.readStringUntil('$').toInt();  //Saves the value as int to "value" Variable for later use
//
//      if (value == 1) {
//        //do something
//        Serial.print("@465/1$"); //Turns landing lights on when AP is on        
//      } else {
//        //do something else
//        Serial.print("@465/0$"); //Turns landing lights on when AP is off        
//      }
//    }
  }
}
void heading_bug_inc(void)
{
  Serial.print("@429/1$"); 
}
void heading_bug_dec(void)
{
  Serial.print("@428/1$"); 
}
void heading_bug_set(uint16_t _hdg_set_value)
{
  Serial.print("@431/"); 
  Serial.print(_hdg_set_value); 
  Serial.print("$"); 
}
void ap_panel_heading_set(int _state)
{
  Serial.print("@106/");
  Serial.print(_state);
  Serial.print("$");
}
void ap_panel_heading_hold(void)
{
  Serial.print("@10046/$"); //j
  delay(200);
}
