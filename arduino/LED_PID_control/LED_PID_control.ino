// C++ code
unsigned long last_time = 0;  // in milliseconds
int Kp = 15;
int Ki = 10;
int Kd = 0;
int LED = 11;
//set the target, increase/decrease setpoint by 1
int Add = A1;
int Minus = A2;
int last_Add_reading = HIGH;     // last raw reading of the pin (HIGH = unpressed, due to INPUT_PULLUP)
int last_Minus_reading = HIGH;
unsigned long last_Add_debounce_time = 0;    // when we last saw the Add pin change
unsigned long last_Minus_debounce_time = 0;
unsigned long debounce_delay = 50;           // ms to ignore further changes after one is seen
bool Add_pressed_handled = false;
bool Minus_pressed_handled = false;

float integral,previous_error,current_value,drive_signal,setpoint;
void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(Add,INPUT_PULLUP);
  pinMode(Minus,INPUT_PULLUP);
  Serial.begin(9600);
}

void loop()
{
  int current_state1 = digitalRead(Add);
  if (current_state1 != last_Add_reading){
    last_Add_debounce_time = millis();
    last_Add_reading = current_state1;
  }
  if(((millis() - last_Add_debounce_time) > debounce_delay) && (digitalRead(Add)== LOW) && (Add_pressed_handled == false)){
    setpoint= setpoint +20;
    Add_pressed_handled = true;
  }
  if (digitalRead(Add) == HIGH){
    Add_pressed_handled = false;
  }

  int current_state2 = digitalRead(Minus);
  if (current_state2 != last_Minus_reading){
    last_Minus_debounce_time = millis();
    last_Minus_reading = current_state2;
  }
  if(((millis() - last_Minus_debounce_time) > debounce_delay) && (digitalRead(Minus)== LOW) && (Minus_pressed_handled == false)){
    setpoint=setpoint -20;
    Minus_pressed_handled = true;
  }
  if (digitalRead(Minus) == HIGH){
    Minus_pressed_handled = false;
  }
  float error,derivative,dt,k;
  k = 0.1; //plant's lag constant
  unsigned long now = millis();
  dt = (now - last_time) / 1000.0;
  last_time = now;
  if (dt <= 0) {
  dt = 0.001;  // treat as a tiny nonzero timestep instead of dividing by zero
  }
  error = setpoint - current_value;
  integral = integral + (error * dt);
  derivative = (error - previous_error) / dt ;
  drive_signal = (Kp * error) + (Ki * integral) + (Kd * derivative);
  current_value += k * (drive_signal - current_value);
  previous_error = error;
  analogWrite(LED,constrain((int)current_value, 0, 255)); //LED displays the current_value number, brighter=higher
  static unsigned long last_print = 0;
  if (millis() - last_print >= 100) {
  last_print = millis();
  Serial.print(setpoint);
  Serial.print(",");
  Serial.print(current_value);
  Serial.print(",");
  Serial.println(drive_signal);   // println (not print) on the last one adds the newline
  }
 }
