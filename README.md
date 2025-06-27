# Pneumatic Rail Position Control

**Final Project:** Automotive Greenhouse > Picking Fruit > Pneumatic-Rail-Position-Control

This subsystem implements a closed-loop control system for a pneumatic linear rail, allowing a robotic arm to move precisely along the X-axis within an automated greenhouse environment. The carriage moves the arm to desired picking positions, and the arm itself operates in the Y-Z plane to harvest fruit.

---

## 🧐 Project Objective

Traditional pneumatic rails stop only at mechanical endpoints. To achieve fruit-picking precision, the carriage must stop at **any commanded position** along the 24-inch rail, with an error tolerance within **0.25 inches**. This is achieved using **ultrasonic sensors**, solenoid valves, and position feedback control logic developed on **Arduino Mega**.

---

## ⚙️ System Architecture

| Component        | Specification                                            |
| ---------------- | -------------------------------------------------------- |
| **MCU**          | Arduino Mega                                             |
| **Rail Length**  | 24 inches                                                |
| **Sensors**      | 2 × HC-SR04 Ultrasonic Modules                           |
| **Actuators**    | 2 × Festo 24V DC 3/2 Single Solenoid Valves              |
| **Relays**       | 2 × HLS-4078-DC5V Relays                                 |
| **Power Supply** | 24V external source                                      |
| **Mounting**     | Sensors on carriage sides, reflectors fixed at rail ends |

### Solenoid Logic

- `Left valve ON, Right OFF` → Move left
- `Left OFF, Right ON` → Move right
- `Both ON` → Stop (balanced pressure)

---

## 🔄 Control Strategy

### ➔ Sensing & Filtering

- **Dual ultrasonic sensors** measure distances to rail ends
- **5-sample median filtering** per sensor
- Check:
  ```
  if |L + R - 24.0| > 1.0 → discard as invalid
  ```
- Position estimation:
  ```
  position = (L + (24 - R)) / 2
  ```

### ➔ Motion Execution

- Movement via **pulse-based actuation**
- Test pulse duration: **1000 ms**
- After pulse: pause **300 ms** before checking position
- If position error ≤ 0.25 in → stop
- Else → repeat pulse

---

## 📊 Experimental Analysis

Two test runs:

- **Left → Right** required 12 steps
- **Right → Left** required 15 steps
- Observed **non-linear friction** and **directional asymmetry**

#### ➔ Key Findings

- Carriage motion is **not linear**
- Friction varies across positions and directions
- Control as a stepper-like system is **unreliable**
- **PID control is necessary**, and likely **direction-dependent**

---

## 📌 Current Features

- ✅ Solenoid relay control from Arduino
- ✅ Dual-sensor filtering & position estimation
- ✅ Manual `left/right` commands via serial
- ✅ Target-based closed-loop positioning
- ✅ Consistency checks to prevent false measurements

---

## 🚧 Work in Progress

- ♻️ PID position control (dual-direction tuning)
- 📡 Bluetooth integration (remote control + wiring simplification)
- 📈 Additional stability tuning & overshoot damping
- 💻 Optional UI for position entry and monitoring

---

## 🧪 How to Use

1. Connect:
   - Arduino Mega
   - 24V power to solenoids via relays
   - HC-SR04s on pins 9/10 (left), 6/7 (right)
2. Upload the code from `/code/` using Arduino IDE
3. Open Serial Monitor @ 9600 baud
4. Send one of the following:
   - `"left"` or `"right"` → Manual jog
   - A number like `12.5` → Target position (inches from left end)

Serial output will show:

- Position from both sensors
- Target vs actual
- Error
- Movement status

---

## 📁 Repository Structure

```
/code             → Arduino code (main.ino, sensor modules)
/hardware         → Schematic, pin layout, system photos
/analysis         → Step response logs, graphs, test notes
/README.md        → Project description
```

---

## 🗖️ Timeline Snapshot

| Milestone                     | Status         |
| ----------------------------- | -------------- |
| Hardware + Wiring             | ✅ Complete    |
| Sensor Mounting + Calibration | ✅ Complete    |
| Basic Open-Loop Step Testing  | ✅ Complete    |
| Data Logging & Analysis       | ✅ Complete    |
| PID Framework Implementation  | ♻️ In progress |
| Bluetooth Remote Control      | ♻️ Planned     |
| Final Integration & Demo      | ♻️ Upcoming    |

---

## 🧑‍💻 Author

This subsystem is fully developed, tested, and documented by **[Your Name]** as part of the final team project for the Smart Greenhouse Automation system.

---

## 📅 License

This project is open-source under the [MIT License](LICENSE).

---

## 🔗 Acknowledgments

This project is part of the **Smart Greenhouse Automation Final Project**, under the course/module: _[Course Name, Institution]_.
