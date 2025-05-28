#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// === CONFIG ===
LiquidCrystal_I2C lcd(0x27, 16, 2);
const int whiteBtnPin = 33;
const int blackBtnPin = 32;

const unsigned long debounceDelay   = 50;
const unsigned long bothLongPressMs = 3000;

const unsigned long presetDurations[] = { 1UL*60*1000, 3UL*60*1000, 10UL*60*1000 };
const int numPresets = sizeof(presetDurations) / sizeof(presetDurations[0]);

// === STATE ===
enum Mode { TIMED, FREEPLAY };
Mode mode = TIMED;

bool gameStarted  = false;
bool whiteTurn    = true;
bool displayOff   = false;

// Timed-mode state
int  presetIndex       = 0;
unsigned long whiteTimeLeft, blackTimeLeft;
unsigned long lastMillis = 0;

// Button-state tracking
bool lastWhiteBtnState = HIGH, lastBlackBtnState = HIGH;
unsigned long lastWhiteDebounce = 0, lastBlackDebounce = 0;
unsigned long bothPressedSince  = 0;
bool bothLongActive = false;

void setup() {
  pinMode(whiteBtnPin, INPUT_PULLUP);
  pinMode(blackBtnPin, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  showMode();
}

void loop() {
  unsigned long now = millis();
  readButtons(now);
  checkBothLongPress(now);

  // Sleep/wake based on displayOff
  if (displayOff) {
    if (buttonPressed(whiteBtnPin) || buttonPressed(blackBtnPin)) {
      displayOff = false;
      lcd.backlight();
      lastMillis = now;
    } else {
      return;  // remain sleeping
    }
  }

  if (mode == TIMED) {
    if (!gameStarted) {
      // Pre-game: choose time preset
      lcd.setCursor(0,0);
      lcd.print("Set time:        ");
      lcd.setCursor(0,1);
      lcd.print("   ");
      lcd.print(presetDurations[presetIndex] / 60000);
      lcd.print(" min           ");
      if (buttonPressed(whiteBtnPin)) {
        presetIndex = (presetIndex + 1) % numPresets;
      }
      if (buttonPressed(blackBtnPin)) startTimedGame(now);

    } else {
      // Running timed game
      unsigned long dt = now - lastMillis;
      lastMillis = now;
      if (whiteTurn) whiteTimeLeft = whiteTimeLeft > dt ? whiteTimeLeft - dt : 0;
      else           blackTimeLeft = blackTimeLeft > dt ? blackTimeLeft - dt : 0;

      // Check for timeout
      if (whiteTimeLeft == 0 || blackTimeLeft == 0) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("  TIME UP!       ");
        lcd.setCursor(0,1);
        lcd.print(whiteTimeLeft == 0 ? "  WHITE LOST    " : "  BLACK LOST    ");
        while (true);
      }

      // Turn-swap: only active button changes turn
      if (whiteTurn) {
        if (buttonPressed(whiteBtnPin)) {
          whiteTurn = false;
        }
      } else {
        if (buttonPressed(blackBtnPin)) {
          whiteTurn = true;
        }
      }

      // Display clocks with W/B labels and arrow
      lcd.setCursor(0,0);
      printTime(whiteTimeLeft);
      lcd.print(" W"); lcd.print(whiteTurn ? "<" : " ");
      lcd.setCursor(0,1);
      printTime(blackTimeLeft);
      lcd.print(" B"); lcd.print(!whiteTurn ? "<" : " ");
    }

  } else {
    // FREEPLAY
    static bool firstFP = true;
    static bool lastTurn;

    if (!gameStarted) {
      gameStarted = true;
      whiteTurn   = true;
      firstFP     = true;
    }

    // Redraw only on first entry or turn change
    if (firstFP || whiteTurn != lastTurn) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(whiteTurn ? "White's turn    " : "Black's turn    ");
      firstFP = false;
      lastTurn = whiteTurn;
    }

    // Active-side swaps turn; inactive-side sleeps
    if (whiteTurn) {
      if (buttonPressed(whiteBtnPin)) {
        whiteTurn = false;
      } else if (buttonPressed(blackBtnPin)) {
        displayOff = true;
        lcd.noBacklight();
      }
    } else {
      if (buttonPressed(blackBtnPin)) {
        whiteTurn = true;
      } else if (buttonPressed(whiteBtnPin)) {
        displayOff = true;
        lcd.noBacklight();
      }
    }

    delay(50);
  }
}

void startTimedGame(unsigned long now) {
  gameStarted   = true;
  whiteTurn     = true;
  whiteTimeLeft = blackTimeLeft = presetDurations[presetIndex];
  lastMillis    = now;
  lcd.clear();
}

void showMode() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(mode == TIMED ? "MODE: TIMED     " : "MODE: FREEPLAY  ");
  delay(1000);
  lcd.clear();
}

void toggleMode() {
  mode = (mode == TIMED ? FREEPLAY : TIMED);
  gameStarted = false;
  displayOff  = false;
  bothLongActive = false;
  lcd.backlight();
  showMode();
}

void readButtons(unsigned long now) {
  int rW = digitalRead(whiteBtnPin), rB = digitalRead(blackBtnPin);
  if (rW != lastWhiteBtnState && now - lastWhiteDebounce > debounceDelay) {
    lastWhiteDebounce = now;
    lastWhiteBtnState = rW;
  }
  if (rB != lastBlackBtnState && now - lastBlackDebounce > debounceDelay) {
    lastBlackDebounce = now;
    lastBlackBtnState = rB;
  }
  if (lastWhiteBtnState == LOW && lastBlackBtnState == LOW) {
    if (!bothPressedSince) bothPressedSince = now;
  } else {
    bothPressedSince = 0;
    bothLongActive  = false;
  }
}

bool buttonPressed(int pin) {
  static bool consW = false, consB = false;
  if (pin == whiteBtnPin) {
    if (lastWhiteBtnState == LOW && !consW) { consW = true; return true; }
    if (lastWhiteBtnState == HIGH) consW = false;
  }
  if (pin == blackBtnPin) {
    if (lastBlackBtnState == LOW && !consB) { consB = true; return true; }
    if (lastBlackBtnState == HIGH) consB = false;
  }
  return false;
}

void checkBothLongPress(unsigned long now) {
  if (bothPressedSince && !bothLongActive && now - bothPressedSince >= bothLongPressMs) {
    bothLongActive = true;
    toggleMode();
  }
}

void printTime(unsigned long t) {
  unsigned int s = (t / 1000) % 60;
  unsigned int m = t / 60000;
  if (m < 10) lcd.print('0'); lcd.print(m);
  lcd.print(':');
  if (s < 10) lcd.print('0'); lcd.print(s);
}