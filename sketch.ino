#include <Keypad.h>
#include <Servo.h>

// Define keypad layout
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {2, 3, 4, 5}; // Rows to pins 2-5
byte colPins[COLS] = {6, 7, 8, 9}; // Columns to pins 6-9

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Servo, LED, and Buzzer pins
Servo doorLock;
const int servoPin = 10; // Servo on pin 10
const int redLedPin = 11; // Red LED (locked/wrong) on pin 11
const int greenLedPin = 12; // Green LED (unlocked) on pin 12
const int buzzerPin = 13; // Buzzer on pin 13

// Password settings
const String correctPassword = "1234"; // Default password
String enteredPassword = "";
const int maxAttempts = 3;
int attempts = 0;

void setup() {
  Serial.begin(9600); // Start serial for debugging
  doorLock.attach(servoPin); // Attach servo
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT); // Set buzzer as output
  digitalWrite(buzzerPin, LOW); // Buzzer off initially
  
  // Initial state: locked
  doorLock.write(0); // Lock position (0 degrees)
  digitalWrite(redLedPin, HIGH); // Red LED ON
  digitalWrite(greenLedPin, LOW); // Green LED OFF
  Serial.println("Smart Door Lock Ready! Enter 4-digit password.");
}

void loop() {
  char key = keypad.getKey();
  
  if (key) {
    Serial.print("*"); // Mask input
    enteredPassword += key;
    
    // Check if 4 digits entered
    if (enteredPassword.length() == 4) {
      if (enteredPassword == correctPassword) {
        // Correct password
        Serial.println("\nAccess Granted! Door Unlocked.");
        doorLock.write(90); // Unlock position (90 degrees)
        digitalWrite(greenLedPin, HIGH); // Green LED ON
        digitalWrite(redLedPin, LOW); // Red LED OFF
        delay(5000); // Keep unlocked for 5 seconds
        // Auto-lock after 5 seconds
        Serial.println("Door Locked.");
        doorLock.write(0); // Lock position
        digitalWrite(greenLedPin, LOW); // Green LED OFF
        digitalWrite(redLedPin, HIGH); // Red LED ON
        attempts = 0; // Reset attempts
      } else {
        // Wrong password
        Serial.println("\nAccess Denied! Wrong Password.");
        digitalWrite(redLedPin, HIGH); // Red LED ON
        digitalWrite(greenLedPin, LOW); // Green LED OFF
        // Activate buzzer: 3 short beeps at 1kHz
        for (int i = 0; i < 3; i++) {
          tone(buzzerPin, 1000); // 1kHz tone
          delay(200);
          noTone(buzzerPin);
          delay(200);
        }
        attempts++;
        if (attempts >= maxAttempts) {
          Serial.println("Too many attempts! System locked for 10 seconds.");
          digitalWrite(redLedPin, HIGH); // Red LED ON
          delay(10000); // Lockout for 10 seconds
          attempts = 0; // Reset attempts
        }
      }
      enteredPassword = ""; // Reset password
    }
  }
}