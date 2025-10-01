#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------------------------
// Pins (avoid conflicts with board macros)
const int MY_LED_RED = 9;
const int MY_LED_BLUE = 5;
const int MY_LED_GREEN = 6;
const int MY_LED_YELLOW = 10;

const int BTN_RED = 2;
const int BTN_BLUE = 3;
const int BTN_GREEN = 4;
const int BTN_YELLOW = 7;

// ---------------------------
// LCD
#define LCD_ADDRESS 0x27   // Change to 0x3F if needed
#define LCD_COLS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// ---------------------------
// Game Setup
const int SEQ_LEN = 4;
int sequence[SEQ_LEN];

// Arrays for LEDs and buttons
int ledPins[4] = {MY_LED_RED, MY_LED_BLUE, MY_LED_GREEN, MY_LED_YELLOW};
int buttonPins[4] = {BTN_RED, BTN_BLUE, BTN_GREEN, BTN_YELLOW};
const char* colorNames[4] = {"RED", "BLUE", "GREEN", "YELLOW"};

// Forward declarations
void flashSequence();
bool getUserInput();

// ---------------------------
// Setup
void setup() {
  Serial.begin(115200);

  // Initialize LEDs
  for (int i = 0; i < 4; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // Initialize buttons
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);  // LOW when pressed
  }

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press buttons in");
  lcd.setCursor(0,1);
  lcd.print("the sequence!");

  // Seed random
  randomSeed(analogRead(A0));

  // Generate random sequence
  for (int i = 0; i < SEQ_LEN; i++) {
    sequence[i] = random(0, 4);  // 0-3 = red, blue, green, yellow
  }

  delay(1000);
  flashSequence();
}

// ---------------------------
// Main Loop
void loop() {
  if (getUserInput()) {
    // Correct sequence
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Correct!");
    for (int i = 0; i < 4; i++) {
      digitalWrite(ledPins[i], HIGH); // turn all LEDs on
    }
    while(1); // Stop game
  } else {
    // Incorrect sequence
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Incorrect!");
    lcd.setCursor(0,1);
    lcd.print("Try again...");
    delay(1000);
    flashSequence();
  }
}

// ---------------------------
// Flash LED sequence
// ---------------------------
void flashSequence() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Watch the LEDs");

  for (int i = 0; i < SEQ_LEN; i++) {
    int led = sequence[i];
    digitalWrite(ledPins[led], HIGH);
    lcd.setCursor(0,1);
    lcd.print(colorNames[led]);
    delay(500);
    digitalWrite(ledPins[led], LOW);
    delay(250);
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Your turn!");
}

// ---------------------------
// Get user input and check sequence
// ---------------------------
bool getUserInput() {
  for (int i = 0; i < SEQ_LEN; i++) {
    bool pressed = false;
    while(!pressed) {
      for (int j = 0; j < 4; j++) {
        if (digitalRead(buttonPins[j]) == LOW) { // button pressed
          pressed = true;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("You pressed:");
          lcd.setCursor(0,1);
          lcd.print(colorNames[j]);
          delay(300);  // debounce and feedback
          if (sequence[i] != j) return false; // wrong button
        }
      }
    }
  }
  return true; // all correct
}
