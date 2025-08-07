# HomeSpanStrips

A HomeSpan-based ESP32 project for controlling addressable LED strips through Apple HomeKit. This project allows you to control RGB LED strips directly from your iOS device using the Apple Home app.

A key feature is the ability of the ESP32 to measure the power it is receiving via two 1k resistors from power GPIO1 and ground. As power increases, we check if supply is dropping too low to prevent brown out.

## Features

- 🌈 **Rainbow LED Strip**: Animated rainbow effects with brightness control
- 🎨 **RGB Light Strip**: Full color control with individual RGB values
- 🏠 **Apple HomeKit Integration**: Native control through iOS Home app
- 📱 **Automatic Discovery**: Shows up automatically in HomeKit
- 💡 **Dual Strip Support**: Two independent light accessories
- ⚡ **Power Management**: Automatic power switching between strips

## Hardware Requirements

- ESP32-C3 DevKit M-1 (or compatible ESP32 board)
- WS2812B/WS2815 addressable LED strip
- 5V/12V power supply (depending on LED strip)
- Connection wires and resistors as needed

## WiFi Setup

### Method 1: Edit Credentials File (Recommended)

1. Create `include/Credentials.h` in your code editor
2. Add the WiFi credentials:
   ```cpp
   #define WIFI_SSID "YourWiFiNetworkName"
   #define WIFI_PASSWORD "YourWiFiPassword"
   ```
3. Save the file and upload the firmware

### Method 2: Runtime Configuration

If you prefer to set WiFi credentials at runtime:
1. Comment out the `#define` lines in `Credentials.h`
2. Use the HomeSpan WiFi setup mode (see below)

## HomeSpan Setup for Apple Home

### Initial Configuration

1. **Build and Upload**: 
   - Compile and upload the firmware to your ESP32
   - Open the Serial Monitor (115200 baud) to see status messages

2. **Device Configuration**:
   - Each device needs a unique bridge name and pairing code
   - Edit `include/Globals.h` and uncomment/modify the configuration for your device:
   ```cpp
   const char* SERIAL_NO   = "JRM.008.XX";        // Unique serial number
   const char* BRIDGE_NAME = "Your Bridge Name";  // Unique bridge name
   const char* PARING_CODE = "88881011";          // 8-digit pairing code
   #define PIXEL_COUNT 300                        // Number of LEDs in your strip
   #define MAX_BRIGHTNESS 255                     // Maximum brightness level
   //#define IS_W2815_12V // Uncomment if using WS2815 12V strip
   ```

3. **Hardware Pins** (adjust in `Globals.h` if needed):
   - NeoPixel Data Pin: GPIO 6
   - Strip Power Pin: GPIO 5  
   - Status LED Pin: GPIO 10
   - Control Switch Pin: GPIO 9

### Adding to Apple Home

1. **Put Device in Pairing Mode**:
   - Power on your ESP32 device
   - Wait for the status LED to indicate ready state
   - Alternatively, press and hold the control button (GPIO 9) to enter pairing mode

2. **Add to iOS Home App**:
   - Open the **Home** app on your iPhone/iPad
   - Tap the **+** button in the top right
   - Select **Add Accessory**
   - Tap **More options** if you don't see your device immediately
   - Look for your bridge name (e.g., "Master Curtain 1")
   - Tap on your device

3. **Enter Pairing Code**:
   - When prompted, enter your 8-digit pairing code
   - Default code is `88881011` (you should change this for security)

4. **Complete Setup**:
   - Choose which room to add the lights to
   - You'll see two light accessories:
     - "Rainbow LED" - Animated rainbow strip with brightness control
     - "Light strip" - Full RGB color control

### WiFi Setup via HomeSpan (Alternative Method)

If you didn't pre-configure WiFi credentials:

1. **Access Setup Mode**:
   - Connect to the ESP32's serial port
   - Send `W` command to enter WiFi setup
   - Or press the control button during startup

2. **Configure Network**:
   - Follow the prompts to enter your WiFi SSID and password
   - The device will save credentials and restart

### Troubleshooting

**Device Not Appearing in Home App:**
- Ensure both iPhone and ESP32 are on the same WiFi network
- Check serial monitor for connection status
- Verify the pairing code is correct
- Try restarting the ESP32 device

**WiFi Connection Issues:**
- Verify WiFi credentials in `Credentials.h`
- Check your router supports 2.4GHz (ESP32-C3 doesn't support 5GHz)
- Ensure network allows device-to-device communication

**LED Strip Not Working:**
- Check power supply voltage matches your LED strip (5V or 12V)
- Verify data pin connection (GPIO 6)
- Check `PIXEL_COUNT` matches your actual LED count
- For WS2815 12V strips, uncomment `#define IS_W2815_12V` in code

**Multiple Devices:**
- Each device must have a unique bridge name and pairing code
- Change the configuration in `Globals.h` before uploading to each device
- Don't use the same pairing code on your network

### Device Controls

- **Rainbow LED**: 
  - On/Off control
  - Brightness adjustment (creates different rainbow effects)
  
- **Light Strip**:
  - On/Off control
  - Full RGB color selection
  - Brightness control

Note: Only one strip can be active at a time. Turning on one will automatically turn off the other.

## Technical Details

- **Platform**: ESP32-C3 using Arduino framework
- **HomeKit Library**: HomeSpan 2.1.0
- **LED Library**: Adafruit NeoPixel 1.12.4
- **Communication**: WiFi 802.11 b/g/n (2.4GHz only)
- **Protocol**: Apple HomeKit Accessory Protocol (HAP)

