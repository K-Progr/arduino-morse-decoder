/*
  Two-Button Morse Decoder
  Board : Arduino Uno / Nano
  Screen: LCD1602 (parallel, 4-bit mode, no I2C backpack)
  Extras: 1 LED

  Button A (pin 7)  = Morse key
    - short press            -> dot   (LED stays lit)
    - long press (>=250 ms)  -> dash  (LED goes dark once it counts as a dash)
  Button B (pin 8)  = Next letter
    - tap                    -> commit letter (or a SPACE if nothing typed)
    - hold 5 s              -> backspace (repeats while held;
                                deletes last symbol first, then last letter)
  A + B together             -> clear everything

  LCD line 1: your text (last 15 chars, cursor after)
  LCD line 2: symbols being typed + live preview, e.g. ".-          =A"
*/

#include <LiquidCrystal.h> //library import

//Pins

const byte PIN_KEY  = 7;    // Button A -> GND
const byte PIN_NEXT = 8;    // Button B -> GND
const byte PIN_LED  = 13;   // LED (+ on-board LED)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);   // RS, E, D4, D5, D6, D7

//Timing (ms)-tuneable

const unsigned long DEBOUNCE_MS  = 25;
const unsigned long DASH_MS      = 250;    // press >= this = dash
const unsigned long BACKSPACE_MS = 5000;  // hold B = backspace
const unsigned long REPEAT_MS    = 350;    // backspace repeat rate

//Morse binary tree

//Start at 1. Dot -> node*2, dash -> node*2+1. '*' = no character.

const char TREE[] = "**ETIANMSURWDKGOHVF*L*PJBXCYZQ**54*3***2*******16*******7***8*90";
const byte MAX_SYMBOLS = 5;

byte node = 1;
char symbols[MAX_SYMBOLS + 1] = "";
byte symCount = 0;

//Text buffer

const byte TEXT_MAX = 64;
char text[TEXT_MAX + 1] = "";
byte textLen = 0;

//Buttons

enum Event { EV_NONE, EV_PRESSED, EV_RELEASED };

struct Button {
  byte pin;
  bool stable;              // debounced: true = pressed
  bool lastRead;
  unsigned long lastChange;
  unsigned long pressedAt;
  bool consumed;            // this press already used (A+B clear)
  bool fired;               // B: backspace already fired during this press
  unsigned long lastFire;   // B: time of the last backspace
};

Button keyBtn  = { PIN_KEY,  false, false, 0, 0, false, false, 0 };
Button nextBtn = { PIN_NEXT, false, false, 0, 0, false, false, 0 };

//'now' is passed in so every time check in one loop pass uses the same value

Event updateButton(Button &b, unsigned long now) {
  bool reading = (digitalRead(b.pin) == LOW);   // INPUT_PULLUP: LOW = pressed
  if (reading != b.lastRead) {
    b.lastRead = reading;
    b.lastChange = now;
  }
  if (now - b.lastChange >= DEBOUNCE_MS && reading != b.stable) {
    b.stable = reading;
    if (b.stable) {
      b.pressedAt = now;
      b.consumed = false;
      b.fired = false;
      return EV_PRESSED;
    }
    return EV_RELEASED;
  }
  return EV_NONE;
}

//LED (only writes the pin when the state changes)

bool ledOn = false;
void setLed(bool on) {
  if (on != ledOn) {
    ledOn = on;
    digitalWrite(PIN_LED, on ? HIGH : LOW);
  }
}

//Display (each line redraws only when it changes)

byte cursorCol = 0;

void drawText() {
  //Line 1: last 15 characters, cursor sits after them
  lcd.setCursor(0, 0);
  byte start = (textLen > 15) ? textLen - 15 : 0;
  byte col = 0;
  for (byte i = start; i < textLen; i++, col++) lcd.print(text[i]);
  cursorCol = col;
  for (; col < 16; col++) lcd.print(' ');
}

void drawLine2(const char *status) {
  //Line 2: status message OR symbols + preview
  lcd.setCursor(0, 1);
  byte col;
  if (status != NULL) {
    for (col = 0; status[col] != '\0' && col < 16; col++) lcd.print(status[col]);
    for (; col < 16; col++) lcd.print(' ');
  } else {
    lcd.print(symbols);
    for (col = symCount; col < 13; col++) lcd.print(' ');
    if (symCount > 0) {
      char c = TREE[node];
      lcd.print('=');
      lcd.print(c == '*' ? '?' : c);
      lcd.print(' ');
    } else {
      lcd.print("   ");
    }
  }
  lcd.setCursor(cursorCol, 0);   // park the blinking cursor after the text
}

void drawAll(const char *status) {
  drawText();
  drawLine2(status);
}

//Editing

void resetLetter() {
  node = 1;
  symCount = 0;
  symbols[0] = '\0';
}

void addChar(char c) {
  if (textLen == TEXT_MAX) {                       // full: drop oldest
    memmove(text, text + 1, TEXT_MAX - 1);
    textLen--;
  }
  text[textLen++] = c;
  text[textLen] = '\0';
}

void addSymbol(bool dash) {
  if (symCount >= MAX_SYMBOLS) {
    drawLine2("max 5 symbols");
    return;
  }
  node = node * 2 + (dash ? 1 : 0);
  symbols[symCount++] = dash ? '-' : '.';
  symbols[symCount] = '\0';
  drawLine2(NULL);                // line 1 hasn't changed
}

void commitLetter() {
  if (symCount == 0) {            // nothing typed -> space
    addChar(' ');
    drawAll(NULL);
    return;
  }
  char c = TREE[node];
  resetLetter();
  if (c == '*') {
    drawLine2("unknown code");
    return;
  }
  addChar(c);
  drawAll(NULL);
}

void doBackspace() {
  if (symCount > 0) {             // undo last dot/dash
    symbols[--symCount] = '\0';
    node /= 2;
    drawLine2(NULL);
  } else if (textLen > 0) {       // undo last letter
    text[--textLen] = '\0';
    drawAll(NULL);
  } else {
    drawLine2(NULL);              // nothing to delete; just clear any message
  }
}

//Setup

void setup() {
  pinMode(PIN_KEY, INPUT_PULLUP);
  pinMode(PIN_NEXT, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  lcd.begin(16, 2);
  lcd.print("Morse ready");
  lcd.setCursor(0, 1);
  lcd.print("A=key  B=next");
  delay(1500);
  lcd.clear();
  lcd.cursor();
  drawAll(NULL);
} //Setup

void loop() {
  unsigned long now = millis();
  Event ea = updateButton(keyBtn, now);
  Event eb = updateButton(nextBtn, now);

  //A + B held together -> clear everything
  if (keyBtn.stable && nextBtn.stable && !(keyBtn.consumed && nextBtn.consumed)) {
    keyBtn.consumed = true;
    nextBtn.consumed = true;
    textLen = 0;
    text[0] = '\0';
    resetLetter();
    drawAll("cleared");
  }

  //Button A: key
  if (keyBtn.stable && !keyBtn.consumed) {
    setLed(now - keyBtn.pressedAt < DASH_MS);   //lit = dot, dark = dash
  }
  else {setLed(false);}

  if (ea == EV_RELEASED && !keyBtn.consumed) { addSymbol(now - keyBtn.pressedAt >= DASH_MS);}

  //Button B: next letter / backspace
  if (nextBtn.stable && !nextBtn.consumed) {
    bool due = nextBtn.fired ? (now - nextBtn.lastFire >= REPEAT_MS)
                             : (now - nextBtn.pressedAt >= BACKSPACE_MS);
    if (due) {
      doBackspace();
      nextBtn.fired = true;
      nextBtn.lastFire = now;
    }
  }

  if (eb == EV_RELEASED && !nextBtn.consumed && !nextBtn.fired) { commitLetter();}
} //Loop