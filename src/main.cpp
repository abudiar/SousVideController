#pragma region Includes
// Must Include
#include <Arduino.h>
// Filesystem
#include <FS.h>
#include <LittleFS.h>
// WiFi, Server, Time, and OTA
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESPAsyncTCP.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h> //https://github.com/tzapu/WiFiManager
// SPI and Display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Temperature
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>
#include <Ticker.h>
// Settings
#include <ArduinoJson.h>
#include <splash.h>
// Math
#include <math.h>
#pragma endregion

#pragma region Pins
#define heaterPin D5  // Heater Relay pin
#define builtinLed D4 // Built in LED
#define tempBus D7    // Temperature Probes
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#pragma endregion

#pragma region Settings
//FS* filesystem = &SPIFFS;
FS *fileSystem = &LittleFS;
// Display Settings (Start X, Start Y, Font Size)
const int logDisp[] = {0, 0, 1}; // Log Display Settings
// Display Settings (Start Y, Font Size)
const int loadDisp[] = {25, 2};  // Loading Display Settings
const int ipDisp[] = {0, 1};     // IP Display Settings
const int rTempDisp[] = {20, 3}; // Temperature Display Settings
const int rPctDisp[] = {45, 2};  // Percentage Display Settings
const int rTuneDisp[] = {9, 1};  // Tuning Display Settings
const int stateDisp[] = {25, 2}; // State Machine Display Settings
// WiFi Settings
const char *wifiSSID = "Sous Vide"; // WiFi Manager SSID
const char *wifiPass = "password";  // WiFi Manager Password
bool resetWifi = false;             // Set to true to reset Wifi Settings on run
#define SERVER_PORT 80              // WebServer Port
unsigned int localPort = 1337;      // local port to listen for UDP packets
// Time
static const char ntpServerName[] = "pool.ntp.org"; // Random NTP Pool
const int timeZone = 7;                             // Jakarta Time
// Serial
const int baudRate = 9600;         // Serial Baud Rate
int logInterval = 20 * 1000; // log every 60 secs
// Config
const char *cfgPath = "/config.json"; // Config file
const char *logPath = "/log.csv";     // Log file
#pragma endregion

#pragma region Local Variables
bool loadAnim = true;               // Oscicillates on loading animation
bool newLog = false;                 // New log file?
int numberOfProbes;                 // Number of temperature probes found
DeviceAddress tempDeviceAddress;    // Store Probe Address
long lastRunTime = 0;               // Last run time
long lastLogTime = 0;               // Last log time
volatile unsigned long next;        // Next interrupt
bool start = false;                 // Has machine started?
bool useCelcius;                    // Using C or F?
Ticker timerTicker;                 // PIDTicker
String dispTemp[4] = {""};          // Holds input temp for display purposes {"C", "F", " °C", " °F"}
String setTemp[4] = {""};           // Holds setpoint temp for display purposes {"C", "F", " °C", " °F"}
bool isPreheated = false;           // Has sous vide preheated?
time_t startTime = 0;               // Sous vide start time
time_t timeSinceStart = 0;          // Time since sous vide start
WiFiUDP Udp;                        // UDP
const int NTP_PACKET_SIZE = 48;     // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; // NTP buffer to hold incoming & outgoing packets
// State Machine
enum operatingState
{
  OFF = 0,
  RUN,
  AUTO
};
operatingState opState = OFF;
// PID Connection
double Setpoint;
double Input;
double Output;
double Power;
volatile long onTime = 0;
// PID Tuning
double Kp;
double Ki;
double Kd;
// 10 second Time Proportional Output window
int WindowSize = 10000;
unsigned long windowStartTime;
// PID AutoTune
byte ATuneModeRemember = 2;
int aTuneControlType = 1; // 0 = PI, 1 = PID
double aTuneStartValue = WindowSize / 2;
double aTuneStep = aTuneStartValue;
double aTuneNoise = 0.2;
unsigned int aTuneLookBack = 90;
boolean tuning = false;
#pragma endregion

#pragma region Library Declarations
// Create AsyncWebServer object on port 80
AsyncWebServer server(SERVER_PORT);
DNSServer dns;
// OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// Temperature Probes
OneWire oneWire(tempBus); //Setup OneWire bus
DallasTemperature sensors(&oneWire);
// PID
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);
PID_ATune aTune(&Input, &Output);
// Time
time_t getNtpTime();
void sendNTPpacket(IPAddress &address);
#pragma endregion

#pragma region Functions
#pragma region Time
// Returns a string of digits in format XX
String getTimeDigits(int digits)
{
  String d = "";
  if (digits < 10)
  {
    d = "0" + (String)digits;
  }
  else
  {
    d = (String)digits;
  }
  return d;
}

String formatTime(time_t time)
{
  String t = "";
  // t += hour(time);
  // t += ":";
  // t += getTimeDigits(minute(time));
  // t += ":";
  // t += getTimeDigits(second(time));
  // t += hour(time)*60*60+minute(time)*60+second(time);
  t += hour(time) > 0 ? (String)hour(time) : "";
  t += hour(time) > 0 ? "h" : "";
  t += minute(time) > 0 ? getTimeDigits(minute(time)) : "";
  t += minute(time) > 0 ? "m" : "";
  t += getTimeDigits(second(time));
  t += "s";
  return t;
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0)
    ; // discard any previously received packets
  Serial.println("Transmitting NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500)
  {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE)
    {
      Udp.read(packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      Serial.print("Recieved NTP Response -> Time: ");
      Serial.println(secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR);
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}
#pragma endregion

#pragma region Device and GPIO
// Print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}
// Initializes a GPIO pin as output (default: LOW)
void initOutPin(uint8_t pin, uint8_t val = LOW)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, val);
}
#pragma endregion

#pragma region Display
// Gets substring of temperature
String getDec(double temperature, int afterDec)
{
  double rnd = round(temperature * 10) / 10;
  String temp = (String)rnd;
  return temp.substring(0, temp.indexOf('.') + 1 + afterDec);
}
// Saves in format of {"C", "F", " °C", " °F"}
void saveTempArray(double temperature, int afterDec, String tempArray[])
{
  tempArray[0] = getDec(temperature, afterDec) + "C";
  tempArray[1] = getDec(DallasTemperature::toFahrenheit(temperature), afterDec) + "F";
  tempArray[2] = getDec(temperature, afterDec);
  tempArray[3] = getDec(DallasTemperature::toFahrenheit(temperature), afterDec);
}
// Sets text on center
void printCenterX(String text, const int disp[])
{
  // For Text Manipulation
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextSize(disp[1]);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int x = (SCREEN_WIDTH - w) / 2;
  display.setCursor(x, disp[0]);
  display.println(text);
}
// Prints for Temp
void printTemp(String tempArray[], const int disp[])
{
  // For Text Manipulation
  // String text = temp;
  // if (temp.length() > temp.indexOf('.'))
  String text = tempArray[0];
  if (!useCelcius)
  {
    text = tempArray[1];
  }
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextSize(disp[1]);
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  int x = (SCREEN_WIDTH - w) / 2;
  display.setCursor(x, disp[0]);
  display.println(text);
}
// Sets cursor and font size
void setCursorSize(const int disp[], String text = "", boolean print = false)
{
  display.setTextSize(disp[2]);
  display.setCursor(disp[0], disp[1]);
  if (text != "")
    display.println(text);
  if (print)
    display.display();
}
// Print on new line and dsiplay
void printDisplay(String message)
{
  display.println(message);
  display.display();
}
#pragma endregion

#pragma region Config
// Loads the default parameters
void loadDefaultPars()
{
  Setpoint = 60;
  Kp = 850;
  Ki = 0.5;
  Kd = 0.1;
  useCelcius = true;
  saveTempArray(Setpoint, 1, setTemp);
}

// Save any parameter changes to File System
bool savePars()
{
  StaticJsonDocument<200> cfgObj;
  String cfgString = "";

  Serial.print("Saving Setpoint: ");
  Serial.print(Setpoint);
  Serial.print(" & PID: Kp, ");
  Serial.print(Kp);
  Serial.print(" Ki, ");
  Serial.print(Ki);
  Serial.print(" Kd, ");
  Serial.print(Kd);
  Serial.print(" & useCelcius: ");
  Serial.println(useCelcius);
  // Save to JSON
  cfgObj["Sp"] = Setpoint;
  cfgObj["Kp"] = Kp;
  cfgObj["Ki"] = Ki;
  cfgObj["Kd"] = Kd;
  cfgObj["ScC"] = useCelcius;

  File cfgFile = fileSystem->open(cfgPath, "w");
  if (!cfgFile)
  {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  if (serializeJson(cfgObj, cfgFile) > 0)
  {
    Serial.println("Saved config file");
  }
  else
  {
    Serial.println("Unable to write config file");
    cfgFile.close();
    return false;
  }
  cfgFile.close();
  return true;
}

// Load parameters from File System
bool loadPars()
{
  File cfg = fileSystem->open(cfgPath, "r");
  if (!cfg)
  {
    Serial.println("Failed to open config file, using default configuration");
    loadDefaultPars();
    return false;
  }
  size_t size = cfg.size();
  if (size > 1024)
  {
    Serial.println("Config file size is too large, using default configuration");
    loadDefaultPars();
    return false;
  }
  std::unique_ptr<char[]> buf(new char[size]);
  cfg.readBytes(buf.get(), size);
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error)
  {
    Serial.println(F("Failed to read file, using default configuration"));
    loadDefaultPars();
    return false;
  }
  // Load from File System
  Setpoint = doc["Sp"];
  Kp = doc["Kp"];
  Ki = doc["Ki"];
  Kd = doc["Kd"];
  useCelcius = doc["ScC"];

  Serial.print("Loaded Setpoint: ");
  Serial.println(Setpoint);
  Serial.print("Loaded PID: Kp, ");
  Serial.print(Kp);
  Serial.print(" Ki, ");
  Serial.print(Ki);
  Serial.print(" Kd, ");
  Serial.println(Kd);
  Serial.print("Loaded useCelcius: ");
  Serial.println(useCelcius);

  cfg.close();
  saveTempArray(Setpoint, 1, setTemp);
  return true;
}

// Logs to File System in csv format
void csvLog()
{
  if (newLog)
  {
    Serial.println("New log -> Deleting log file");
    fileSystem->remove(logPath);
    newLog = false;
  }
  bool newFile = false;
  if (!fileSystem->exists(logPath))
  {
    Serial.println("Log file does not exist -> Creating...");
    newFile = true;
  }
  File logFile = fileSystem->open(logPath, "a");
  if (!logFile)
  {
    Serial.println("Unable to open log file!");
    return;
  }
  if (newFile)
  {
    // Time, Temperature, Output
    Serial.print("Logging: ");
    Serial.print("Time");
    Serial.print(",");
    Serial.print("Temperature");
    Serial.print(",");
    Serial.print("Power");
    Serial.print(",");
    Serial.println("Target");

    logFile.print("Time");
    logFile.print(",");
    logFile.print("Temperature");
    logFile.print(",");
    logFile.print("Power");
    logFile.print(",");
    logFile.println("Target");
  }
  time_t timeSinceStart = now() - startTime;
  Serial.print("Logging: ");
  Serial.print(formatTime(timeSinceStart));
  Serial.print(",");
  Serial.print(Input);
  Serial.print(",");
  Serial.print(Power);
  Serial.print(",");
  Serial.println(Setpoint);

  logFile.print(formatTime(timeSinceStart));
  logFile.print(",");
  logFile.print(Input);
  logFile.print(",");
  logFile.print(Power);
  logFile.print(",");
  logFile.println(Setpoint);

  logFile.close();
}
#pragma endregion

#pragma region ISR
// Called by ISR every 15ms to drive the output
void DriveOutput()
{
  long now = millis();
  // Set the output
  // "on time" is proportional to the PID output
  if (now - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if ((onTime > 100) && (onTime > (now - windowStartTime)))
  {
    digitalWrite(heaterPin, HIGH);
  }
  else
  {
    digitalWrite(heaterPin, LOW);
  }
}

// Timer Interrupt Handler
void timerTick()
{
  next = ESP.getCycleCount() + 80000 * 15; // run in the next 15 ms
  timer1_write(next);
  if (opState == OFF)
  {
    digitalWrite(heaterPin, LOW); // make sure relay is off
  }
  else
  {
    DriveOutput();
  }
}
#pragma endregion

#pragma region PID
// Start auto-tune cycle
void StartAutoTune()
{
  // REmember the mode we were in
  ATuneModeRemember = myPID.GetMode();

  // set up the auto-tune parameters
  Output=aTuneStartValue;
  aTune.SetControlType(aTuneControlType);
  aTune.SetNoiseBand(aTuneNoise);
  aTune.SetOutputStep(aTuneStep);
  aTune.SetLookbackSec((int)aTuneLookBack);
  tuning = true;
}
// Return to normal control
void FinishAutoTune()
{
  tuning = false;

  // Extract the auto-tune calculated parameters
  Kp = aTune.GetKp();
  Ki = aTune.GetKi();
  Kd = aTune.GetKd();

  // Re-tune the PID and revert to normal control mode
  myPID.SetTunings(Kp, Ki, Kd);
  myPID.SetMode(ATuneModeRemember);

  // Persist any changed parameters to File System
  savePars();
}
// Execute Control Loop
void DoControl()
{
  // Read the input:
  if (sensors.isConversionComplete())
  {
    Input = sensors.getTempC(tempDeviceAddress);
    saveTempArray(Input, 1, dispTemp);
    if (!isPreheated && Input >= Setpoint - 0.5)
    {
      isPreheated = true;
    }
    sensors.requestTemperatures(); // prime the pump for the next one - but don't wait
  }

  if (tuning) // run the auto-tuner
  {
    if (aTune.Runtime()) // returns 'true' when done
    {
      FinishAutoTune();
    }
  }
  else // Execute control algorithm
  {
    myPID.Compute();
  }

  // Time Proportional relay state is updated regularly via timer interrupt.
  onTime = Output;
}
#pragma endregion
#pragma endregion

#pragma region LibSetups
void setupTime()
{
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
}

void setupDisplay()
{

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  Serial.println("SSD1306");

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  // Clear the buffer
  // display.clearDisplay();
  // display.setCursor(0, 0);
  // printCenterX("Loading..", loadDisp);
  display.clearDisplay();
  // display.drawBitmap(0, 0, SousVideSplash, SousVideSplash_w, SousVideSplash_h, 1);
  display.drawBitmap((SCREEN_WIDTH - splash1_width) / 2, (SCREEN_HEIGHT - splash1_height) / 2,
                     splash1_data, splash1_width, splash1_height, 1);
  display.display();
}

void setupWifiManager()
{
  if (resetWifi)
  {
    WiFi.disconnect(true);
    delay(2000);
    ESP.reset();
  }
  AsyncWiFiManager wifiManager(&server, &dns);
  IPAddress ip(192, 168, 0, 196);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  wifiManager.setSTAStaticIPConfig(ip, gateway, subnet);
  String temp = "Connect on ";
  temp += wifiSSID;
  temp += " at 192.168.4.1";
  printDisplay(temp);
  if (!wifiManager.autoConnect(wifiSSID, wifiPass))
  {
    printDisplay("ERR: WiFi Mgr!");
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  printDisplay("SUC: WiFi Mgr!");
}

void setupWifiOTA()
{

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
    {
      type = "sketch";
    }
    else
    { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using fileSystem->end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
    {
      Serial.println("Auth Failed");
    }
    else if (error == OTA_BEGIN_ERROR)
    {
      Serial.println("Begin Failed");
    }
    else if (error == OTA_CONNECT_ERROR)
    {
      Serial.println("Connect Failed");
    }
    else if (error == OTA_RECEIVE_ERROR)
    {
      Serial.println("Receive Failed");
    }
    else if (error == OTA_END_ERROR)
    {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void setupmDNS()
{

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin("esp8266"))
  {
    printDisplay("ERR: mDNS Rspndr!");
    Serial.println("Error setting up mDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  printDisplay("SUC: mDNS Rspndr!");

  //  // Start TCP (HTTP) server
  //  server.begin();
  //  Serial.println("TCP server started");
  //
  //  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
}
String formatBytes(size_t bytes)
{
  if (bytes < 1024)
  {
    return String(bytes) + "B";
  }
  else if (bytes < (1024 * 1024))
  {
    return String(bytes / 1024.0) + "KB";
  }
  else if (bytes < (1024 * 1024 * 1024))
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
  else
  {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}
void setupFileSystem()
{
  // Initialize File System
  if (!fileSystem->begin())
  {
    printDisplay("ERR: Mnt File System!");
    Serial.println("An Error has occurred while mounting File System");
    return;
  }
  else
  {
    Dir dir = fileSystem->openDir("/");
    while (dir.next())
    {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Serial.printf("\n");
    Serial.println("Filesystem");
    printDisplay("SUC: Mnt File System!");
  }
}

void setupTempProbes()
{
  // Start up the library
  sensors.begin();
  if (!sensors.getAddress(tempDeviceAddress, 0))
  {
    printDisplay("ERR: Temp Prb");
  }
  // Grab a count of devices on the wire
  numberOfProbes = sensors.getDeviceCount();

  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(numberOfProbes, DEC);
  Serial.println(" devices.");

  // Loop through each device, print out address
  for (int i = 0; i < numberOfProbes; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    }
    else
    {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
  sensors.setResolution(tempDeviceAddress, 12);
  sensors.setWaitForConversion(false);
  printDisplay("SUC: Temp Prb!");
}

void setupPID()
{
  // Initialize the PID and related variables
  loadPars();
  myPID.SetTunings(Kp, Ki, Kd);

  myPID.SetSampleTime(1000);
  myPID.SetOutputLimits(0, WindowSize);
}
void setupInterrupts()
{
  timerTicker.attach(0.015, timerTick);
}
#pragma endregion

#pragma region Server Functions

// Convert Boolean to String (for check Success?)
inline const char *const BoolToString(bool b)
{
  return b ? "true" : "false";
}

// Prints server request
void printServerRequest(AsyncWebServerRequest *request)
{
  Serial.printf("Server Request: ");
  if (request->method() == HTTP_GET)
    Serial.printf("GET");
  else if (request->method() == HTTP_POST)
    Serial.printf("POST");
  else if (request->method() == HTTP_DELETE)
    Serial.printf("DELETE");
  else if (request->method() == HTTP_PUT)
    Serial.printf("PUT");
  else if (request->method() == HTTP_PATCH)
    Serial.printf("PATCH");
  else if (request->method() == HTTP_HEAD)
    Serial.printf("HEAD");
  else if (request->method() == HTTP_OPTIONS)
    Serial.printf("OPTIONS");
  else
    Serial.printf("UNKNOWN");
  Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

  // if (request->contentLength())
  // {
  //   Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
  //   Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
  // }

  // int headers = request->headers();
  // int i;
  // for (i = 0; i < headers; i++)
  // {
  //   AsyncWebHeader *h = request->getHeader(i);
  //   Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
  // }

  // int params = request->params();
  // for (i = 0; i < params; i++)
  // {
  //   AsyncWebParameter *p = request->getParam(i);
  //   if (p->isFile())
  //   {
  //     Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
  //   }
  //   else if (p->isPost())
  //   {
  //     Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
  //   }
  //   else
  //   {
  //     Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
  //   }
  // }
}

// Get Temperature
String getTempScale()
{
  if (useCelcius)
    return "C";
  else
    return "F";
}
String getState()
{
  if (opState == OFF)
  {
    if (tuning)
      return "Tuning(Off)";
    return "Off";
  }
  else if (opState == RUN)
  {
    if (tuning)
      return "Tuning(Run)";
    if (!isPreheated)
      return "Preheat";
    return "Ready";
  }
  return "";
}

// Process settings from json of webpage
String processSettings(StaticJsonDocument<256> data)
{
  String result = "Updated ";
  if (data.containsKey("scale") == 1) {
    if (data["scale"].as<String>() == "C")
      useCelcius = true;
    else
      useCelcius = false;
    result += "| useCelcius ";
  }
  if (data.containsKey("target") == 1) {
    Setpoint = data["target"].as<double>();
    result += "| Setpoint ";
  }
  if (data.containsKey("kp") == 1) {
      Kp = data["kp"].as<double>();
    result += "| Kp ";
  }
  if (data.containsKey("ki") == 1) {
      Ki = data["ki"].as<double>();
    result += "| Ki ";
  }
  if (data.containsKey("kd") == 1) {
      Kd = data["kd"].as<double>();
    result += "| Kd ";
  }
  if (data.containsKey("logInterval") == 1) {
      logInterval = data["logInterval"].as<int>();
    result += "| logInterval ";
  }
  savePars();
  DoControl();
  
  String stringData = "";
  serializeJson(data, stringData);
  // Serial.println(stringData);
  return stringData;
  // return result;
  // if (data.containsKey("pageNum") == 1)
  // {
  //   int pageNum = data["pageNum"].as<int>();
  //   if (pageNum == 0 && data.containsKey("scale") == 1 && data.containsKey("target") == 1)
  //   {
  //     savePars();
  //     DoControl();
  //     return "Settings on Home Page Saved";
  //   }
  //   else if (pageNum == 1 && data.containsKey("kp") == 1 && data.containsKey("ki") == 1 && data.containsKey("kd") == 1)
  //   {
  //     Kp = data["kp"].as<double>();
  //     Ki = data["ki"].as<double>();
  //     Kd = data["kd"].as<double>();
  //     savePars();
  //     DoControl();
  //     return "Settings on Settings Page Saved";
  //   }
  // }
}

String packSettings()
{
  String stringData = "";
  StaticJsonDocument<256> dataObj;
  dataObj["state"] = getState();
  dataObj["temp"] = (String)Input;
  dataObj["scale"] = getTempScale();
  dataObj["target"] = (String)Setpoint;
  dataObj["kp"] = (String)Kp;
  dataObj["ki"] = (String)Ki;
  dataObj["kd"] = (String)Kd;
  dataObj["output"] = (String)Output;
  dataObj["input"] = (String)Input;
  serializeJson(dataObj, stringData);
  // Serial.println(stringData);
  return stringData;
}
// Process command from webpage
String processCommand(String cmd)
{
  if (cmd == "start")
  {
    Serial.println("Starting Sous Vide");
    start = true;
    startTime = now();
    return packSettings();
  }
  else if (cmd == "stop")
  {
    Serial.println("Stopping Sous Vide");
    start = false;
    opState = OFF;
    aTune.Cancel();
    return packSettings();
  }
  else if (cmd == "autotune")
  {
    Serial.println("Starting Autotune");
    StartAutoTune();
    start = true;
    startTime = now();
    return packSettings();
  }
  return "Invalid command";
}

String getContentType(AsyncWebServerRequest *request, String filename)
{
  if (request->hasArg("download"))
  {
    return "application/octet-stream";
  }
  else if (filename.endsWith(".htm"))
  {
    return "text/html";
  }
  else if (filename.endsWith(".html"))
  {
    return "text/html";
  }
  else if (filename.endsWith(".css"))
  {
    return "text/css";
  }
  else if (filename.endsWith(".js"))
  {
    return "application/javascript";
  }
  else if (filename.endsWith(".json"))
  {
    return "application/json";
  }
  else if (filename.endsWith(".csv"))
  {
    return "text/csv";
  }
  else if (filename.endsWith(".png"))
  {
    return "image/png";
  }
  else if (filename.endsWith(".gif"))
  {
    return "image/gif";
  }
  else if (filename.endsWith(".jpg"))
  {
    return "image/jpeg";
  }
  else if (filename.endsWith(".ico"))
  {
    return "image/x-icon";
  }
  else if (filename.endsWith(".xml"))
  {
    return "text/xml";
  }
  else if (filename.endsWith(".pdf"))
  {
    return "application/x-pdf";
  }
  else if (filename.endsWith(".zip"))
  {
    return "application/x-zip";
  }
  else if (filename.endsWith(".gz"))
  {
    return "application/x-gzip";
  }
  return "text/plain";
}
bool handleFileRead(AsyncWebServerRequest *request, String path)
{
  if (path.endsWith("/"))
  {
    path += "index.html";
  }
  String contentType = getContentType(request, path);
  String pathWithGz = path + ".gz";
  if (fileSystem->exists(path))
  {
    request->send(LittleFS, path, contentType);
    return true;
  }
  else if (fileSystem->exists(pathWithGz))
  {
    path += ".gz";
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, path, contentType);
    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
    return true;
  }
  return false;
}
#pragma endregion

#pragma region Server
void setupServer()
{
  server.on("/data", HTTP_POST, [](AsyncWebServerRequest *request) {
    printServerRequest(request);
    StaticJsonDocument<256> data;
    if (request->hasParam("data"))
    {
      auto error = deserializeJson(data, request->getParam("data")->value());
      if (error)
      {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
        request->send_P(200, "text/plain", "deserializeJson() failed");
        return;
      }
    }
    request->send_P(200, "text/plain", processSettings(data).c_str());
  });

  server.on("/command", HTTP_POST, [](AsyncWebServerRequest *request) {
    printServerRequest(request);
    bool isSuccess = false;
    String cmd = "";
    if (request->hasParam("command"))
    {
      cmd = (request->getParam("command")->value());
      request->send_P(200, "text/plain", processCommand(cmd).c_str());
    }
    else
      request->send_P(200, "text/plain", BoolToString(false));
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    printServerRequest(request);
    request->send_P(200, "text/plain", packSettings().c_str());
  });

  server.onNotFound([](AsyncWebServerRequest *request) {
    if (!handleFileRead(request, request->url()))
    {
      request->send(404, "text/plain", "FileNotFound");
    }
  });

  // Start server
  server.begin();

  Serial.println("Ready");
  Serial.print("AP SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  printDisplay("SUC: ServerPrt 80!");
}
#pragma endregion

#pragma region Setup
void setupIO()
{
  initOutPin(builtinLed, HIGH);
  initOutPin(heaterPin);
}

void startDisplay()
{
  display.clearDisplay();
  display.setTextColor(WHITE);

  printCenterX(WiFi.localIP().toString(), ipDisp);

  display.display();
}

void setup()
{
  Serial.begin(baudRate);
  Serial.println("Booting");
  setupDisplay();

  setCursorSize(logDisp);

  setupFileSystem();
  setupWifiManager();
  setupmDNS();
  setupWifiOTA();
  setupServer();
  setupTempProbes();
  setupPID();
  setupInterrupts();
  setupTime();

  setupIO();

  startDisplay();
}
#pragma endregion

#pragma region States
void Off()
{
  myPID.SetMode(MANUAL);
  digitalWrite(heaterPin, LOW); // make sure it is off
  // uint8_t buttons = 0;
  printCenterX(WiFi.localIP().toString(), ipDisp);
  double roundTemp = round(Input * 10) / 10;
  String tempText = (String)roundTemp + "C";
  printCenterX(tempText, rTempDisp);
  printCenterX("OFF", rPctDisp);
  // Read the input:
  if (sensors.isConversionComplete())
  {
    Input = sensors.getTempC(tempDeviceAddress);
    saveTempArray(Input, 1, dispTemp);
    sensors.requestTemperatures(); // prime the pump for the next one - but don't wait
  }

  if (start)
  {
    newLog = true;
    sensors.requestTemperatures(); // Start an asynchronous temperature reading

    //turn the PID on
    myPID.SetMode(AUTOMATIC);
    windowStartTime = millis();
    opState = RUN; // start control
  }
}
void Run()
{ //TODO: Add second temp check (Set to mode manual in the even 100C)

  if (millis() - lastRunTime > 100)
  {
    myPID.SetTunings(Kp, Ki, Kd);

    if (!start)
    {
      opState = OFF;
    }

    DoControl();

    printCenterX(WiFi.localIP().toString(), ipDisp);

    double roundTemp = round(Input * 10) / 10;
    String tempText = "";
    if (useCelcius)
    {
      tempText = (String)roundTemp;
      tempText.remove(strlen(tempText.c_str()) - 1);
      tempText += "C";
    }
    else
    {
      tempText = (String)DallasTemperature::toFahrenheit(Input);
      tempText.remove(strlen(tempText.c_str()) - 1);
      tempText += "F";
    }
    printCenterX(tempText, rTempDisp);

    float outputPct = map(Output, 0, WindowSize, 0, 100);
    double roundOutputPct = round(outputPct * 10) / 10;
    Power = roundOutputPct / 100;
    String pctText = (String)roundOutputPct;
    pctText.remove(strlen(pctText.c_str()) - 3);
    pctText += "%";
    printCenterX(pctText, rPctDisp);

    if (tuning)
    {
      printCenterX("Tuning", rTuneDisp);
    }

    // periodically log to serial port in csv format
    if (millis() - lastLogTime > logInterval)
    {
      lastLogTime = millis();
      csvLog();
    }
  }
}
#pragma endregion

void loop()
{
  ArduinoOTA.handle();
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  switch (opState)
  {
  case OFF:
    Off();
    break;
  case RUN:
    Run();
    break;
  }
  display.display();
}