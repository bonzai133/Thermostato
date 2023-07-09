# Thermostato
Connected thermostat based on ESP12F Relay + Alim card.

- ESP 12F Relay and Alim card
- MCP 9808 (I2C): temperature sensor
- Oled 1.3": 128x64 (I2C)

# GPIO mapping
- GPIO -> 12F Relay
  - 1 GND -> GND
  - 2 GPIO 1 (TX) -> TX (LED)
  - 3 GPIO 2 (TX)
  - 4 CH_PD
  - 5 GPIO 0 (SPI_CS2) -> Relay 1
  - 6 TTY_RST
  - 7 GPIO 3 (RX) -> RX
  - 8 VCC -> 5V
  - 16 _ -> Led_i 2

- GPIO -> MCP 9808
  - GPIO 0 -> SDA
  - GPIO 2 -> SCL

- GPIO -> Oled
  - GPIO 0 -> SDA
  - GPIO 2 -> SCK

# Setup
## Wifi
- secrets.h : will contain wifi credentials. Must never be commited in Git.
'''
const char* ssid = "";
const char* password = "";
'''

## AsyncElegantOTA
- Allow to update new version Over The Air
  - Can also support login/pwd. Check project documentation.
  - Access to http://192.168.x.y/update
    - Upload firmware or filesystem


## Little FS
- Contain Settings and webpage
- Access to config file (debug)
  - http://192.168.x.y/config/thermostato.cfg
- TODO
  - Remove /data/config/thermostato.cfg when building update to avoid erasing values set by user
  - When /data/config is empty in little.fs, thermostato.cfg is deleted. How to keep it ?
  - Use json file for simple modification (see arduinoJson example) ?

# Display
- Date and time
- Temperature
- Heating mode
- IP Address
- High and low temperature setpoint
- TODO
  - Replace Sun icon by Pump icon

# Web page
- Home
  - TODO: Replace mode from API by label
- Config
- Advanced config: 
  - TODO
    - Persist values
    - Display timezone + date
- About
  - Better css

# REST API
- /api/config
- TODO
  - Check PUT method

# Relay control
- TODO
  - Define and implement heating algorithm
  - Configuration of heating periods

# Monitoring
  - Expose /metrics for Prometheus
  - Temperature
  - TODO
    - isHeating
    - ESP.getFreeHeap();
