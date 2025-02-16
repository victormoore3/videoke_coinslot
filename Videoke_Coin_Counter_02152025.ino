#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define COIN_ACCEPTOR_PIN 2  // Coin acceptor signal pin
#define VIDEOKE_PULSE_PIN 3  // Pulse output to videoke machine

LiquidCrystal_I2C lcd(0x27, 20, 4);  // LCD 20x4

volatile int pulseCount = 0;  // Pulse counter
unsigned long lastPulseTime = 0;  // Debounce timer

void coinInterrupt() {
    unsigned long currentTime = millis();
    if (currentTime - lastPulseTime > 50) {  // 50ms debounce
        pulseCount++;
        lastPulseTime = currentTime;
    }
}

void setup() {
    lcd.begin();
    lcd.backlight();

    pinMode(COIN_ACCEPTOR_PIN, INPUT_PULLUP);
    pinMode(VIDEOKE_PULSE_PIN, OUTPUT);
    digitalWrite(VIDEOKE_PULSE_PIN, HIGH);  // Default HIGH

    attachInterrupt(digitalPinToInterrupt(COIN_ACCEPTOR_PIN), coinInterrupt, FALLING);

    pulseCount = 0;

    // Initial display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Welcome to Immuki");
    lcd.setCursor(0, 1);
    lcd.print("Beach Hut Rental");
    lcd.setCursor(0, 2);
    lcd.print("P10 per song");
    lcd.setCursor(0, 3);
    lcd.print("Coin/s inserted: 0");
}

void sendPulseToVideoke() {
    digitalWrite(VIDEOKE_PULSE_PIN, LOW);
    delay(500);  // Increased pulse duration (was 100ms)
    digitalWrite(VIDEOKE_PULSE_PIN, HIGH);
}

void updateDisplay() {
    static int lastPulseCount = -1;

    if (pulseCount != lastPulseCount) {
        lastPulseCount = pulseCount;

        // Display Coin/s Inserted
        lcd.setCursor(16, 3);
        lcd.print("    ");  // Clear old number
        lcd.setCursor(16, 3);
        lcd.print(pulseCount);

        // Display "Coins needed"
        lcd.setCursor(0, 2);
        lcd.print("                    "); // Clear row
        lcd.setCursor(0, 2);
        if (pulseCount < 10) {
            lcd.print("Coins needed: ");
            lcd.print(10 - pulseCount);
        } else {
            lcd.print("P10 per song");
        }

        // **Send Pulse to Videoke Machine ONLY ONCE per P10 inserted**
        if (pulseCount >= 10) { 
            sendPulseToVideoke();  // Send a single pulse
            pulseCount -= 10;  // Deduct exactly 10, ensuring no extra pulses
        }
    }
}

void loop() {
    updateDisplay();
}
