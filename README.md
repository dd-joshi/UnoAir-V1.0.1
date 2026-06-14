# UnoAir V1.0.1 - Smart Air Quality Controller

Arduino-based automatic fan controller with manual override for air purification systems.

## Overview

UnoAir is an intelligent fan control system that automatically adjusts fan speed based on real-time air quality measurements from a PM2.5 sensor, while allowing manual override control via a potentiometer.

## Features

- **Automatic Air Quality Monitoring**: Reads PWM duty cycle from PM2.5 sensor to determine air quality
- **Smart Fan Speed Control**: Automatically adjusts fan speed across 4 air quality levels
- **Manual Override**: Potentiometer acts as a ceiling control to limit maximum fan speed
- **Visual Status Indicators**: 
  - Red LED: Poor/hazardous air quality
  - Green LED: Good air quality
  - Both LEDs: Moderate air quality
- **25kHz PWM Output**: Optimized frequency for fan operation (quieter, more efficient)
- **Sensor Failsafe**: Falls back to manual control if sensor signal is lost
- **Debug Serial Output**: Real-time monitoring of sensor readings and fan speeds

## Demo & Resources

- **📺 [Video Demonstration](https://youtu.be/Vonsc9QrG_Y)** - See UnoAir in action
- **🔧 [3D Mount/Adapter](https://www.thingiverse.com/thing:3521586)** - 3D printable enclosure and mounting solution

## Hardware Requirements

### Components
- Arduino Uno microcontroller
- PM2.5 Air Quality Sensor (with PWM output)
- 12V DC Fan with PWM speed control
- Potentiometer (10kΩ recommended)
- 2x LED indicators (Red and Green)
- 2x 220Ω resistors (for LED current limiting)
- Power supply (12V for fan, 5V regulated for Arduino)

### Pin Configuration

| Pin | Function | Connection |
|-----|----------|-----------|
| A0 | POT_PIN | Potentiometer (manual control) |
| 5 | RED_LED | Red LED indicator |
| 6 | GREEN_LED | Green LED indicator |
| 7 | SENSOR_PIN | PM2.5 sensor PWM input |
| 10 | FAN_PWM | Fan PWM control (Timer1 OCR1B) |

## Air Quality Levels & Fan Response

The system uses PM2.5 sensor duty cycle to determine air quality:

| Duty Cycle | Air Quality | Fan Speed | LEDs |
|------------|-------------|-----------|------|
| 0-4% | **Good** | 40-160 (6-25%) | Green |
| 4-8% | **Moderate** | 160-320 (25-50%) | Red + Green |
| 8-12% | **Poor** | 320-520 (50-81%) | Red |
| >12% | **Hazardous** | 639 (100%) | Red |

## Firmware Operation

### Setup Phase
1. Initializes serial communication (115200 baud)
2. Configures Timer1 PWM at 25kHz frequency
3. Displays "Air Purifier Ready" message

### Main Loop (300ms cycle)
1. **Read Potentiometer**: Gets manual ceiling control value
2. **Read Air Quality Sensor**: Acquires PM2.5 duty cycle
3. **Calculate Sensor Speed**: Maps duty cycle to fan speed based on air quality thresholds
4. **Apply Override Logic**: Final speed = minimum of (sensor speed, manual limit)
5. **Minimum Speed Enforcement**: Prevents fan stalling (enforces ~15% minimum when ON)
6. **Output PWM**: Applies final speed to fan
7. **Debug Output**: Prints current readings to serial monitor

### Sensor Failsafe
If the PM2.5 sensor signal is lost (timeout after 1.1 seconds), the system:
- Logs "Sensor read error - using manual control" to serial
- Falls back to manual potentiometer control
- Ensures fan operation isn't interrupted

## PWM Configuration

The Arduino Uno Timer1 is configured for:
- **Frequency**: 25kHz (16MHz ÷ 640)
- **Mode**: Fast PWM with TOP = ICR1
- **Resolution**: 0-639 (10-bit equivalent)
- **Output Pin**: D10 (OCR1B)

This frequency is chosen to:
- Reduce fan noise (beyond human hearing range ~20kHz)
- Improve fan motor efficiency
- Provide smooth speed control

## Serial Debug Output Format

```
Duty: X.X% | Sensor: XXX | Pot: XXX | Final: XXX
```

- **Duty**: PM2.5 sensor duty cycle percentage
- **Sensor**: Calculated fan speed from sensor reading
- **Pot**: Manual control speed value
- **Final**: Actual speed applied to fan

## Installation & Upload

1. Clone or download this repository
2. Open `Uno-Air/Uno-Air.ino` in Arduino IDE
3. Select Board: Arduino Uno
4. Select the correct COM port
5. Click Upload
6. Open Serial Monitor (115200 baud) to verify operation

## Circuit Notes

### Power Supply Considerations
- Arduino Uno requires 5V regulated supply
- 12V fan requires dedicated 12V power (typically 1-2A)
- Use a common ground between Arduino and fan power circuits
- Consider adding a 1000µF capacitor on 12V rail for stability

### PWM Output Protection
- Arduino D10 outputs 5V PWM logic signal
- Use a MOSFET driver or relay module to switch 12V fan
- Don't connect fan directly to Arduino (will damage the chip)

### Sensor Connection
- PM2.5 sensor PWM output (typically 3.3V) connects to D7
- Arduino input pins are 5V tolerant
- Ensure sensor ground is connected to Arduino ground

## Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| Fan not responding | FAN_PWM pin (D10) not connected | Verify PWM driver/MOSFET connection |
| Sensor timeout errors | Weak/missing sensor signal | Check sensor power, PWM signal voltage |
| LED not lighting | LED polarity reversed | Verify + to resistor, - to GND |
| Inconsistent fan speed | Potentiometer dirty | Turn pot back and forth several times |
| Arduino not uploading | Wrong COM port selected | Check Device Manager or Arduino IDE board selection |

## Future Improvements

- [ ] Add OLED display for real-time status
- [ ] Implement data logging to SD card
- [ ] Add WiFi connectivity for remote monitoring
- [ ] Multiple speed presets via buttons
- [ ] Automatic shutdown on high temperatures
- [ ] I2C-based sensors for more accurate readings

## License

This project is open source. Please refer to the LICENSE file for details.

## Support & Contributions

For issues, feature requests, or contributions, please use the GitHub Issues and Pull Requests sections.

---

**Version**: 1.0.1  
**Last Updated**: April 2026  
**Platform**: Arduino Uno  
**Language**: C++  
**Required Libraries**: Arduino core only (no external dependencies)
