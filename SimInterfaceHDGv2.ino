//Sample using LiquidCrystal library
//#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <ClickEncoder.h>
//#include <Encoder.h>
/*******************************************************

This program will test the LCD panel and the buttons
Mark Bramwell, July 2010

********************************************************/
// select the pins used on the LCD panel
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
//Encoder enc(A0,A1);
ClickEncoder *encoder;

bool led_state=HIGH;

//int pins[3][4]={{0,1,2,3},{4,5,6,7},{8,9,10,11}};
int pins[4]={2,3,4,5};
int bit_id[3]={6,7,8};
float dis_value=888;
const int led_pin=9;
const int btn=10;
const int en_btn=11;
const int bi=12;
const int lt=13;
float vs=0;
int hdg_sel=0;
int alt_sel=0;
int spd_sel=0;
 int counter = 0; 
 int currentStateCLK;
 int previousStateCLK; 

int16_t last, value;

void timerIsr() {
  encoder->service();
}

void getDataFromFS2020(void);

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
  Serial.begin(9600);
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

long oldPosition  = -999;
int dis_value_old=0;
void loop()
{   
  value = encoder->getValue();
//  long newPosition = enc.read();
//  if (newPosition != oldPosition) {
//    long poserr=newPosition-oldPosition;
//    oldPosition = newPosition;
    dis_value+=float(value)/2;
//    Serial.println(dis_value);
//    Serial.println(newPosition);
//  }
   
//  getDataFromFS2020();

  if(digitalRead(lt)==LOW)
  {
    set_led();
  }
  else
  {
    set_led_state();
  }

  if(digitalRead(en_btn)==LOW)
  {
    dis_value=0;
  }
  if(digitalRead(btn)==LOW)
  {
    toggle_led();
    delay(200);
  }  
  dis_value=max(min(dis_value,999),0);
  if(dis_value != dis_value_old)
  {
    set_dis(int(dis_value));
  }
  dis_value_old=dis_value;
}

void getDataFromFS2020() {
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

        if (input.indexOf("@86/") != -1) { //Check for Vertical Speed ID
      hdg_sel = Serial.readStringUntil('$').toInt();  //Saves the value as int to "value" Variable for later use
        
      //Do something with the values...
      //like displaying or sending something
      //Or include a function like:
      //void refreshStepperMotorPosition(int value)

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
