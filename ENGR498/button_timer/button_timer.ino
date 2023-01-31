/*
  Button timer
  time between button presses
  - needs debounce
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 7;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

unsigned long startTime;
unsigned long endTime;
unsigned long duration;

int state = 0;
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  // begin console
  // - switch to display
  Serial.begin(9600);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // S0 Begin timer when button first pressed and no timerRunning yet
  if (state == 0 && buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    Serial.print ("Timer Started!\n");
    startTime = millis();
    state = 1;
  } 
  // button unpressed, waits for next press to stop
  if (state == 1 && buttonState == LOW) {
    state = 2;
  }
  // button pressed again, stops timer and displays
  if (state == 2 && buttonState == HIGH) {
    endTime = millis();
    digitalWrite(ledPin, LOW);
    printTimeElapsed();
    state = 3;
  }
  // unpress and wait for next button press. ie. restarts timer
  if (state == 3 && buttonState == LOW) {
    state = 0;
  }
//  Serial.print ("state: ");
//  Serial.println (state);
 
}

void printTimeElapsed(){
  endTime = millis();
  duration = endTime - startTime;
  Serial.print ("Seconds: ");
  Serial.println (duration / 1000);
}
