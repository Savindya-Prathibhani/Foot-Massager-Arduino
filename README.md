# Foot-Massager-Arduino
Arduino-based foot massager with temperature and pulse sensors

This project is an Arduino-based foot massager that uses various sensors to provide a relaxing experience. The project includes the following features:
- Temperature sensing
- Pulse rate monitoring
- Water level detection
- IR sensor for leg detection
- Multiple control relays for fan, motor, and spray

## Components Used
- Arduino mega board
- LiquidCrystal_I2C_Hangul library for LCD display
- Keypad library for user input
- OneWire and DallasTemperature libraries for temperature sensor
- Various sensors and relays

## How to Use
1. Connect the components as per the circuit diagram.
2. Upload the provided Arduino code to your Arduino board.
3. Power on the system and follow the instructions on the LCD screen.

## Code Explanation
### Libraries Included
- `Wire.h`: For I2C communication
- `LiquidCrystal_I2C_Hangul.h`: For LCD display
- `Keypad.h`: For keypad input
- `OneWire.h` and `DallasTemperature.h`: For temperature sensor

### Pin Configuration
- Water Level Sensor: A0
- IR Sensor: 29
- Temperature Sensor: 27
- Various relays: 31, 33, 35, 37
- Buzzer: 7
- Pulse Sensor: A1
- Keypad: Pins 39, 41, 43, 45 and 47, 49, 51, 53

### Functions
- `getTemperature()`: Reads temperature from the sensor
- `displayTemperature(int temperature)`: Displays temperature on the LCD
- `readPulse()`: Reads and displays the pulse rate
- `readWaterLevelSensor()`: Reads the water level
- `startMassage()`, `stopMassage()`: Controls the massage functionality
- `buzAlert()`: Activates the buzzer
- `functionalKeyA()`, `functionalKeyB()`, `functionalKeyC()`: Handles keypad input for various functions

## Contribution
Feel free to fork this repository, make improvements, and submit pull requests.

