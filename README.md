# WifiBlinds
This is a project I made to automate the blinds in my apartment.
The hardware platform is based on a Wemos D1 mini (which is based on the esp8266).

The following components are used (all items can be bought from ebay):
- Wemos D1 Mini
- 38KHz TSOP IR receiver
- H-bridge
- High torque geared mini DC motor
- 3d printed motor mount (not from Ebay)


Features of the system:
- Scheduling (to make blinds close/open on user selected times)
- Synchronization with NTP server.
- Controlled with Samsung remote.
- Webinterface (includes: open/close, firmware update, monitoring div. variables)


Things that I want to add/improve:
- Store user values on flash (the board has no eeprom)
- Implement MQTT protocol (this will allow it to talk with a broad range of smart-home products)
- AP functionality on startup, in case of no known wifi's discovered.
- PWM on the GPIO to the H-brdige.
- Encoder feedback from motor.
- Change to a ESP32 dev board.
- Impose architecture, and use design patterns between modules.
