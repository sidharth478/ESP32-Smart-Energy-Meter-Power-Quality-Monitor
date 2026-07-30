# ⚡ ESP32 Smart Energy Meter & Power Quality Monitor

![Simulation Status](https://img.shields.io/badge/Simulation-Wokwi%20Passed-brightgreen)
![Platform](https://img.shields.io/badge/Platform-ESP32-blue)
![Domain](https://img.shields.io/badge/Domain-Electrical%20%26%20Electronics%20Engineering-orange)

An **Embedded C++ Power Quality Monitoring & Smart Energy Meter System** simulated on the ESP32 platform. This project samples AC voltage and load current waveforms to calculate real-time electrical parameters, computes active/apparent power, tracks cumulative kWh energy consumption, and provides dynamic fault detection for over-voltage, over-current, and low power factor conditions.

---

## 📊 Demo & Circuit Simulation

![Simulation Demo](https://github.com/sidharth478/ESP32-Smart-Energy-Meter-Power-Quality-Monitor/blob/main/SMPM_simulation_demo2026-07-30%20200253.png)

> 🔗 **Live Interactive Wokwi Simulation:**  
> (https://wokwi.com/projects/470980459237821441))*

---

## ⚙️ Features & Electrical Calculations

The system samples high-speed ADC inputs and evaluates core power equations in real time:

* **RMS Voltage ($V_{\text{rms}}$) & RMS Current ($I_{\text{rms}}$):** Scaled sampling from analog sensors (ZMPT101B & SCT-013).
* **Active Power ($P$):** Calculated as $P = V_{\text{rms}} \times I_{\text{rms}} \times \text{PF}$ (Watts).
* **Apparent Power ($S$):** Calculated as $S = V_{\text{rms}} \times I_{\text{rms}}$ (VA).
* **Power Factor ($\text{PF}$):** Evaluated dynamically to identify inductive drop under heavy loading.
* **Energy Accumulation ($\text{kWh}$):** $E = \int P(t) \, dt$ integrated across sample time intervals.
* **Fault Alerts:** 
  * 🔴 **Red Alert LED:** Triggers during Over-Voltage ($>245\text{V}$) or Over-Current ($>20\text{A}$) events.
  * 🔊 **Audio Buzzer Alarm:** Triggers when Power Factor drops below minimum threshold ($\text{PF} < 0.85$).

---

## 🛠️ System Architecture & Wiring

| Component | Wokwi Model / Real World Equiv. | ESP32 Connection | Description |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | ESP32 DevKit v1 | — | Core Processing & Dual ADC |
| **Voltage Sensor** | Potentiometer 1 (ZMPT101B) | `GPIO 34` (ADC) | Simulates $0 - 260\text{V}$ AC RMS |
| **Current Sensor** | Potentiometer 2 (SCT-013) | `GPIO 35` (ADC) | Simulates $0 - 30\text{A}$ AC Load Current |
| **Display** | 0.96" SSD1306 OLED | `GPIO 21` (SDA), `GPIO 22` (SCL) | Real-time parameter dashboard |
| **Alert LED** | Red LED + $220\,\Omega$ Resistor | `GPIO 15` | Over-voltage / over-current trip indicator |
| **Buzzer** | Active Buzzer | `GPIO 4` | Low power factor audio alarm |

---

## 📁 Repository Structure

```text
smart-energy-meter/
├── firmware/
│   ├── sketch.ino            # Main C++ source code
│   └── libraries.txt         # Required OLED display drivers
├── simulation/
│   └── diagram.json          # Wokwi circuit layout export
├── docs/
│   └── simulation_demo.png   # Screenshot of live simulation
├── .gitignore
├── LICENSE                   # MIT License
└── README.md                 # Project Manual
