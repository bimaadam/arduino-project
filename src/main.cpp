#include <WiFi.h>
#include <WebServer.h>

// ========== Konfigurasi ========== //
const char* ssid = "IoT-Dev By Bima";      // Nama Access Point 
const int ledPin = 2;               // GPIO 2 (LED bawaan ESP32)

// ========== Variabel Global ========== //
WebServer server(80);
bool isBlinking = false;
unsigned long previousMillis = 0;
const long blinkInterval = 200;     // Interval blink (ms)

// ========== HTML Template ========== //
const String htmlTemplate = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    button { padding: 10px 20px; font-size: 16px; }
    .on { background-color: #4CAF50; color: white; }
    .off { background-color: #f44336; color: white; }
    .blink { background-color: #FFA500; color: black; }
  </style>
</head>
<body>
  <h1>Kontrol LED ESP32</h1>
  <p>Status LED: <b>%STATUS%</b></p>
  <p>
    <a href="/on"><button class="on">NYALAKAN</button></a>
    <a href="/off"><button class="off">MATIKAN</button></a>
    <a href="/blink"><button class="blink">BLINK</button></a>
  </p>
</body>
</html>
)rawliteral";

// ========== Fungsi Generate HTML ========== //
String generateHTML() {
  String statusText;
  if (isBlinking) {
    statusText = "BLINKING";
  } else {
    statusText = digitalRead(ledPin) ? "ON" : "OFF";
  }
  String html = htmlTemplate;
  html.replace("%STATUS%", statusText);
  return html;
}

// ========== Handler Web Server ========== //
void handleRoot() {
  server.send(200, "text/html", generateHTML());
}

void handleOn() {
  isBlinking = false;
  digitalWrite(ledPin, HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  isBlinking = false;
  digitalWrite(ledPin, LOW);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleBlink() {
  isBlinking = true;
  server.sendHeader("Location", "/");
  server.send(303);
}

// ========== Setup & Loop ========== //
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Buat Access Point
  WiFi.softAP(ssid);
  Serial.println("\nAccess Point dibuat!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Setup Web Server
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/blink", handleBlink);
  server.begin();
  Serial.println("Web server aktif!");
}

void loop() {
  server.handleClient();

  // Blink otomatis jika mode aktif
  if (isBlinking) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= blinkInterval) {
      previousMillis = currentMillis;
      digitalWrite(ledPin, !digitalRead(ledPin));
    }
  }
}