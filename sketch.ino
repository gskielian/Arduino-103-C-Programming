#include <Servo.h>
#include <stdint.h>

// define LED Pin
#define LED 13

// define control pins
#define PUL 9
#define DIR 10
#define ENA 11
#define OPTO 2

//PORTB TRANSLATIONS
//PUL 0  is PORTB 1
//DIR 10 is PORTB 2
//ENA 11 is PORTB 3
#define PB_PUL 1
#define PB_DIR 2
#define PB_ENA 3


//x is used for the pulse width
int x;
float rpm;

//This is used for the recording movements
Servo motor;
unsigned long duration = 0; //duration of blank signal for recording stage
int count_dots; //current count for recording stage

//these are variables which handle the incoming data from tornado
int msgByte= -1;         // incoming byte
const int msgSize = 50;  // max message size
char msgArray[msgSize];  // array for incoming data
int msgPos = 0;          // current position
String str;              // a string for simplifying response message code
char response_array[50]; // a character array for printing out messages
int response_number;

//for the setup, we create as fast communication with the raspberry pi as possible
void setup() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT); // this is our indicator light
    pinMode(ENA, OUTPUT); // this turns on and off the motor current
    pinMode(DIR, OUTPUT); // low is CCW, high is CW
    pinMode(OPTO, INPUT); // sets up the for pulseIn on pin 2
  motor.attach(PUL);
  //gotofirst well
  motor.writeMicroseconds(2000);
  delay(10);
}

void loop() {
    handleSerial();
}

void handleSerial() {
  if (Serial.available() > 0) {
    msgByte = Serial.read();

    if (msgByte != '\n') {
      // add incoming byte to array
      msgArray[msgPos] = msgByte;
      msgPos++;
    } else {
      // reached end of line
      msgArray[msgPos] = 0;

      // here the message is processed
      // for now just send it back over serial

      //shake sequence
        if ( msgArray[0] == '1' ) {
          enable_dir(1,0); //enable and ccw
          shake_sequence(4);
          enable_dir(0,0); //disable
          motor.detach();
          response_number=1;
        }
      //ramp_sequence
        else if (msgArray[0] == '2') {
          enable_dir(1,0); //enable and ccw
          demo_centrifuge_stage();
          enable_dir(0,0); //enable and ccw
          motor.detach();
          response_number=2;
        }
      //recording_sequence
        else if (msgArray[0] == '3') {
          enable_dir(1,1); //enable and ccw
          find_first_well();
          enable_dir(0,1); //enable and ccw
          motor.detach();
          response_number=3;
        }
      //full_sequence
        else if (msgArray[0] == '4') {
          enable_dir(1,0); //enable and ccw
          shake_sequence(4);
          demo_centrifuge_stage();
          enable_dir(1,1); //enable and cw
          find_first_well();
          enable_dir(0,1); //disable and ccw
          motor.detach();
          response_number=3;
        }
      //LED ON
        else if (msgArray[0] == '5') {
          digitalWrite(LED,HIGH);
          response_number=5;
        }
      //LED OFF
        else if (msgArray[0] == '6') {
          digitalWrite(LED,LOW);
          response_number=6;
        } else{
          motor.detach();
          response_number=0;
        }
        Serial.println(response_number);
        //Serial.println(msgArray);
        // reset byte array
        for (int c = 0; c < msgSize; c++) {
          msgArray[c] = ' ';
          response_array[c] = ' ';
        }
        msgPos = 0;
    }
  }
}

void enable_dir( int enc, int dirc )//enable choice, direction choice
{
  //PORTB |= 1<<PB_ENA;
  digitalWrite(ENA, enc);
  delayMicroseconds(10);

  //PORTB &= (1 << PB_DIR)^0xFF; //turns PB_DIR LOW
  digitalWrite(DIR,dirc); // low is CCW high is CW
  delayMicroseconds(10);
}

void find_first_well()
{
  motor.attach(PUL);
  //gotofirst well
  long open_space;
do {
  motor.writeMicroseconds(2000);
  open_space = pulseIn(OPTO,LOW,500000);
  //Serial.println("not open");
} while (open_space != 0);

  //Serial.println(" open");
  motor.writeMicroseconds(2000);
  duration = pulseIn(OPTO,LOW,5000000); //5 seconds

  //stop and increment count_dots
  motor.detach();
  count_dots=1; // equiv to count_dots = 0; count_dots++;
  delay(1000);
 // Serial.println(count_dots);
 // Serial.println("foundfirstwell");
 // Serial.println(duration);

  gotonext(100000);// begin counting dots
}


void gotonext(long inter_dot_duration)
{

    int last_dot=0;
    int numdots=0;

    while (last_dot==0 && numdots<11) {
      motor.attach(PUL);
      digitalWrite(DIR,1);
      motor.writeMicroseconds(2000);
      duration = pulseIn(OPTO,LOW,2*inter_dot_duration); //5 seconds

      if(duration == 0) {
        //strategy: make it try to find the next dot if can't find and count_dots > 1 then we go back
        //Serial.println(count_dots); // this is the dot number
        //Serial.println("backtracking");

        motor.attach(9);
        //here we will need to send it backwards, first setting the direction pin to opposite
        digitalWrite(DIR,0);

        duration = pulseIn(2,LOW,4*inter_dot_duration);
        delay(500);
        //Serial.println("photo taking goes here");
        count_dots=0;
        last_dot = 1;
        //Serial.println(duration);
      } else {
        count_dots++;
        //Serial.println(count_dots);
        //Serial.println(duration);
      }
      motor.detach();
      delay(1000);
      numdots++;
    }
    count_dots=0;

      switch (numdots) {
        case 1:
          Serial.println(7);
          break;
        case 2:
          Serial.println(8);
          break;
        case 3:
          Serial.println(9);
          break;
        case 4:
          Serial.println(10);
          break;
        default:
          break;
      }
      digitalWrite(LED,HIGH);
      delay(2000); // wait for camera to take photo
      digitalWrite(LED,LOW);
      motor.attach(PUL);
      digitalWrite(DIR,1);
      motor.writeMicroseconds(2000);
      delay(1000);

  //this will set up hardware interrupts, and stop the rotation if detected.
  //come to think of it, if we get the pwm library up, then we might be
  //able to use that asynchronously here
  //of course we would be restricted to the set speed of the PWM (whatever that may be)

}


/*
void linear_acceleration( int start_speed, int target_speed)  {

  uint16_t i;
  // math
  // 2x thedelay * 400 pulse/rev = time / pulse * 400 pulse / revolution = 400*time (s) /revolution 
  // we want a constant increase in the revolutions per second, the reciprocal of the previous.
  // 400 * (2 * the delay) = seconds/rev
  // 1/(2*thedelay * 400 ) = revolutions per second
  // 926
  //go throuhh these for a certain amount of mW each

int arduino_array = {
  915,
  904,
  893,
  882,
  872,
  862,
  852,
  843,
  833,
  824,
  815,
  806,
  798,
  789,
  781,
  773,
  765,
  758,
  750,
  743,
  735,
  728,
  721,
  714,
  708,
  701,
  694,
  688,
  682,
  676,
  670,
  664,
  658,
  652,
  647,
  641,
  636,
  630,
  625,
  620,
  615,
  610,
  605,
  600,
  595,
  591,
  586,
  581,
  577,
  573,
  568,
  564,
  560,
  556,
  551,
  547,
  543,
  540,
  536,
  532,
  528,
  524,
  521,
  517,
  514,
  510,
  507,
  503,
  500};

  for( i = 0 ; i < x; i++) {
    //digitalWrite(PUL, HIGH);
    PORTB = 0b00001110; // PB_PUL is portb 1
    delayMicroseconds(thedelay); // 100,000 pulses per second, 100 us is 10,000 pulses per second
    //digitalWrite(PUL, LOW);
    PORTB = 0b00001100; // PB_PUL is portb 1
    delayMicroseconds(thedelay); //180 works for 6400 ppr, however it would seem that we are limited somehow on our output line from going any lower, furthermore it seems that 3200 will not actuate at 180
  }

}
*/

//maybe the problem was that there was not enough time to ramp up
void ramp_to_speed( uint16_t x, int thedelay) {

  uint16_t i;
  for( i = 0 ; i < x; i++) {
    //digitalWrite(PUL, HIGH);
    PORTB = 0b00001110; // PB_PUL is portb 1
    delayMicroseconds(thedelay); // 100,000 pulses per second, 100 us is 10,000 pulses per second
    //digitalWrite(PUL, LOW);
    PORTB = 0b00001100; // PB_PUL is portb 1
    delayMicroseconds(thedelay); //180 works for 6400 ppr, however it would seem that we are limited somehow on our output line from going any lower, furthermore it seems that 3200 will not actuate at 180
  }
}


void ramp_to_speed2( int x, int thedelay, int direction) {

int i;
if ( direction == 1) {
  uint16_t i;
  for( i = 0 ; i < x; i++) {
    //digitalWrite(PUL, HIGH);
    PORTB = 0b00001110; // PB_PUL is portb 1
    delayMicroseconds(thedelay); // 100,000 pulses per second, 100 us is 10,000 pulses per second
    //digitalWrite(PUL, LOW);
    PORTB = 0b00001100; // PB_PUL is portb 1
    delayMicroseconds(thedelay); //180 works for 6400 ppr, however it would seem that we are limited somehow on our output line from going any lower, furthermore it seems that 3200 will not actuate at 180
  }
  /*
    digitalWrite(DIR,HIGH);
    for( i = 0 ; i < x; i++) {
      digitalWrite(PUL, HIGH);
      //PORTB = 0b00001110; // PB_PUL is portb 1
      delayMicroseconds(thedelay); // 100,000 pulses per second, 100 us is 10,000 pulses per second
      digitalWrite(PUL, LOW);
      //PORTB = 0b00001100; // PB_PUL is portb 1
      delayMicroseconds(thedelay); //180 works for 6400 ppr, however it would seem that we are limited somehow on our output line from going any lower, furthermore it seems that 3200 will not actuate at 180
    }
    */
  }
  else {
    uint16_t i;
    for( i = 0 ; i < x; i++) {
      //digitalWrite(PUL, HIGH);
      PORTB = 0b00001010; // PB_PUL is portb 1
      delayMicroseconds(thedelay); // 100,000 pulses per second, 100 us is 10,000 pulses per second
      //digitalWrite(PUL, LOW);
      PORTB = 0b00001000; // PB_PUL is portb 1
      delayMicroseconds(thedelay); //180 works for 6400 ppr, however it would seem that we are limited somehow on our output line from going any lower, furthermore it seems that 3200 will not actuate at 180
    }
  /*
    digitalWrite(DIR,LOW);
    for( i = 0 ; i < x; i++) {
      digitalWrite(PUL, HIGH);
      //PORTB = 0b00001010; // PB_PUL is portb 1
      delayMicroseconds(thedelay); // 100,000 pulses per second, 100 us is 10,000 pulses per second
      digitalWrite(PUL, LOW);
      //PORTB = 0b00001000; // PB_PUL is portb 1
      delayMicroseconds(thedelay); //180 works for 6400 ppr, however it would seem that we are limited somehow on our output line from going any lower, furthermore it seems that 3200 will not actuate at 180
    }
    */
  }


}

void shake_sequence( int number ) {
  int i, x;
  //Serial.print("shaking "); Serial.print(number); Serial.println(" times");  // LOW is CCW, HIGH is CW for ( i = 0; i < number ; i++ ) { //Serial.println(i);  // LOW is CCW, HIGH is CW digitalWrite(DIR, LOW);
  for (i = 0; i < number; i++ )
  {

        digitalWrite(ENA, HIGH);
        digitalWrite(DIR, HIGH);
        motor.attach(PUL);
        //gotofirst well
        motor.writeMicroseconds(2000);
        delay(10);
        motor.detach();

        ramp_to_speed2(100,3000,1);
        for ( x = 3000; x > 1000 ; x -= 100) {
          ramp_to_speed2(20,x,1);
        }
        for ( x = 1000; x > 3000 ; x += 100) {
          ramp_to_speed2(20,x,1);
        }
        //digitalWrite(ENA, LOW);
        digitalWrite(ENA, HIGH);
        delay(800);

        digitalWrite(DIR, LOW);
        motor.attach(PUL);
        //gotofirst well
        motor.writeMicroseconds(2000);
        delay(10);
        motor.detach();

        ramp_to_speed2(100,3000,0);
        for ( x = 3000; x > 1000 ; x -= 100) {
          ramp_to_speed2(20,x,0);
        }
        for ( x = 1000; x > 3000 ; x += 100) {
          ramp_to_speed2(20,x,0);
        }
        //digitalWrite(ENA, LOW);
        digitalWrite(ENA, HIGH);
        delay(800);
          //PORTB |= 0b00000100; // PB_PUL is portb 1
  }
}


   /**
   * Divides a given PWM pin frequency by a divisor.
   * 
   * The resulting frequency is equal to the base frequency divided by
   * the given divisor:
   *   - Base frequencies:
   *      o The base frequency for pins 3, 9, 10, and 11 is 31250 Hz.
   *      o The base frequency for pins 5 and 6 is 62500 Hz.
   *   - Divisors:
   *      o The divisors available on pins 5, 6, 9 and 10 are: 1, 8, 64,
   *        256, and 1024.
   *      o The divisors available on pins 3 and 11 are: 1, 8, 32, 64,
   *        128, 256, and 1024.
 * 
 * PWM frequencies are tied together in pairs of pins. If one in a
 * pair is changed, the other is also changed to match:
 *   - Pins 5 and 6 are paired on timer0
 *   - Pins 9 and 10 are paired on timer1
 *   - Pins 3 and 11 are paired on timer2
 * 
 * Note that this function will have side effects on anything else
 * that uses timers:
 *   - Changes on pins 3, 5, 6, or 11 may cause the delay() and
 *     millis() functions to stop working. Other timing-related
 *     functions may also be affected.
 *   - Changes on pins 9 or 10 will cause the Servo library to function
 *     incorrectly.
 * 
 * Thanks to macegr of the Arduino forums for his documentation of the
 * PWM frequency divisors. His post can be viewed at:
 *   http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1235060559/0#4

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

*/

// this function will only need to go to 1000rpm
void demo_centrifuge_stage() {

  ramp_to_speed(200,3000);

  for ( x = 3000; x > 1000 ; x -= 200) {
    ramp_to_speed(10,x);
  }

  for ( x = 1000; x > 500 ; x--) {
    ramp_to_speed(5,x);
  }

  Serial.println("125 rpm reached");

  for ( x = 500; x > 100 ; x--) {
    ramp_to_speed(10,x);
  }

  digitalWrite(ENA, LOW);
  long safety;
  do{
    safety = pulseIn(OPTO, LOW, 1000000);
  } while (safety != 0);

  Serial.println("it is now safe to remove disk");
}
  /*
  Serial.println("625 rpm reached");
  for ( x = 100; x >=75; x--) {
    ramp_to_speed(2000,x);
  }
  //holding speed
    ramp_to_speed(20000,75);
    
  Serial.println("1000 rpm reached");
  Serial.println("Sequence Complete, spinning down");
  */

void centrifuge_stage()
{

  ramp_to_speed(200,3000);

  for ( x = 3000; x > 1000 ; x -= 200) {
    ramp_to_speed(10,x);
  }
  for ( x = 1000; x > 500 ; x--) {
    ramp_to_speed(20,x);
  }
  Serial.println("125 rpm reached");
  for ( x = 500; x > 100 ; x--) {
    ramp_to_speed(10,x);
  }

  Serial.println("625 rpm reached");
  for ( x = 100; x >=75; x--) {
    ramp_to_speed(2000,x);
  }
  Serial.println("1000 rpm reached");

  for ( x = 75; x >=60; x--) {
    ramp_to_speed(5000,x);
  }

  for ( x = 60; x >=50; x--) {
    ramp_to_speed(10000,x);
  }
  for ( x = 50; x >=45; x--) {
    ramp_to_speed(15000,x);
  }
  for ( x = 45; x >=41; x--) {
    ramp_to_speed(40000,x);
  }
  for ( x = 41; x >=38; x--) {
    ramp_to_speed(60000,x);
  }

  Serial.println("2000 rpm reached");
  ramp_to_speed(60000,x);

  Serial.println("Sequence Complete, spinning down");

  digitalWrite(ENA, LOW);
  delay(5000);
  Serial.println("it is now safe to remove disk");
}
