#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // RS, E, D4, D5, D6, D7

void setup() {
  pinMode(7, INPUT_PULLUP);   // Button A
  pinMode(8, INPUT_PULLUP);   // Button B
  pinMode(13, OUTPUT);        // LED
  lcd.begin(16, 2);
  lcd.print("Button test");
}

void loop() {
  bool a = digitalRead(7) == LOW;
  bool b = digitalRead(8) == LOW;
  digitalWrite(13, a || b);
  lcd.setCursor(0, 1);
  lcd.print("A:"); lcd.print(a ? 1 : 0);
  lcd.print("  B:"); lcd.print(b ? 1 : 0);
  delay(50);
}
