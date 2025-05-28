# ESP32 Arduino Chess Timer

A feature-rich **chess clock** built using an **ESP32**, **LCD screen (I2C)**, and **two buttons**. This timer supports both **timed matches** with countdowns and a **freeplay mode** for casual games. Ideal for DIY enthusiasts and chess lovers alike.

---

## 🧠 Features

- **Two Game Modes**:
  - **Timed Mode**:
    - Selectable presets: **1, 3, or 10 minutes** per player.
    - Time decrements only on the active player’s turn.
    - Pressing the active player's button switches turns.
  - **Freeplay Mode**:
    - Unlimited time.
    - Press to switch turns.
    - Inactive player can turn off the display to simulate idle state.

- **Mode Switching**:
  - Press and hold **both buttons for 3 seconds** to toggle between Timed and Freeplay modes at any time.

- **Power Saving**:
  - In **Freeplay Mode**, the display turns off when the inactive player presses their button.
  - Display reactivates on any button press.

- **Simple LCD Interface**:
  - Uses a 16x2 LCD over I2C to show mode, timer, turns, and messages like **"TIME UP!"** or **"WHITE/BLACK LOST"**.

---

## 🧰 Hardware Requirements

- **ESP32 Dev Board** (e.g. Freenove ESP32-Wrover with GPIO extension board)
- **16x2 LCD with I2C interface** (Address: `0x27`)
- **2 Momentary Push Buttons**
- Breadboard or PCB, jumper wires

### Pin Configuration

| Component      | ESP32 Pin |
|----------------|-----------|
| White Button   | GPIO 33   |
| Black Button   | GPIO 32   |
| LCD (I2C)      | SDA - GPIO 21, SCL - GPIO 22 (default ESP32 I2C pins) |

---

## 🚀 Getting Started

1. **Install Required Libraries**:
   - `LiquidCrystal_I2C` (available through the Arduino Library Manager)

2. **Upload the Sketch**:
   - Open the project in the Arduino IDE.
   - Select the appropriate ESP32 board.
   - Upload the sketch to your ESP32 via USB.

3. **Operation**:
   - On boot, the LCD shows the current mode.
   - In **Timed Mode**:
     - Press **White** to cycle through time presets (1, 3, 10 minutes).
     - Press **Black** to start the game.
     - Press your button on your turn to switch turns.
     - Timer ends the game and displays a loss if time runs out.
   - In **Freeplay Mode**:
     - Press to alternate turns.
     - Inactive side can turn off the display to simulate a pause.

---

## 📷 Optional Enhancements

- Add a buzzer for turn or timeout alerts.
- Use LEDs to indicate turn.
- Add a settings menu for custom time entry.

---

## 📄 License

This project is licensed under the [MIT License](https://opensource.org/licenses/MIT).  
Feel free to use, modify, and share.

---

## ✍️ Author

Made by **[Your Name Here]**  
Enjoy the game and keep playing fair!

