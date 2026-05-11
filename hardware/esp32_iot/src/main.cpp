#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- WIFI CONFIG ---
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

// --- UDP CONFIG ---
WiFiUDP udp;
const int udpPort = 4210;

// --- HARDWARE PINS ---
const int RED_LED = 25;
const int YELLOW_LED = 26;
const int GREEN_LED = 27;
const int BUZZER = 33;

// --- OLED DISPLAY ---
Adafruit_SSD1306 display(128, 64, &Wire, -1);

void playSiren(int type) {
    if (type == 1) { // Ambulance - Wailing
        for(int hz = 600; hz < 1200; hz += 5) { tone(BUZZER, hz); delay(5); }
        for(int hz = 1200; hz > 600; hz -= 5) { tone(BUZZER, hz); delay(5); }
    } else if (type == 2) { // Police - Yelping
        for(int i=0; i<3; i++) {
            tone(BUZZER, 800); delay(100); 
            tone(BUZZER, 1200); delay(100);
        }
    }
    noTone(BUZZER);
}

void setup() {
    Serial.begin(115200);
    
    pinMode(RED_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    // Initial OLED setup
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED failed");
        for(;;);
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 20);
    display.println("Connecting to WiFi...");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    udp.begin(udpPort);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("TRAFFIC IOT NODE");
    display.drawLine(0, 10, 128, 10, WHITE);
    display.setCursor(0, 20);
    display.println("IP: " + WiFi.localIP().toString());
    display.println("Status: READY");
    display.display();
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        char incomingPacket[255];
        int len = udp.read(incomingPacket, 255);
        if (len > 0) incomingPacket[len] = 0;
        
        String data = String(incomingPacket);
        Serial.println("Received: " + data);

        // Update Display
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("TRAFFIC IOT NODE");
        display.drawLine(0, 10, 128, 10, WHITE);
        
        // Protocol: L:[G|Y|R]|C:[count]|E:[0|1|2]
        // Example: L:G|C:15|E:1
        
        String light = "RED";
        if (data.indexOf("L:G") > -1) {
            digitalWrite(GREEN_LED, HIGH); digitalWrite(RED_LED, LOW); digitalWrite(YELLOW_LED, LOW);
            light = "GREEN";
        } else if (data.indexOf("L:Y") > -1) {
            digitalWrite(YELLOW_LED, HIGH); digitalWrite(RED_LED, LOW); digitalWrite(GREEN_LED, LOW);
            light = "YELLOW";
            tone(BUZZER, 1000, 50); // Warning beep
        } else if (data.indexOf("L:R") > -1) {
            digitalWrite(RED_LED, HIGH); digitalWrite(GREEN_LED, LOW); digitalWrite(YELLOW_LED, LOW);
            light = "RED";
        }

        display.setCursor(0, 20);
        display.setTextSize(1);
        display.println("Light: " + light);
        
        if (data.indexOf("C:") > -1) {
            int start = data.indexOf("C:") + 2;
            int end = data.indexOf("|", start);
            if (end == -1) end = data.length();
            display.println("Active Cars: " + data.substring(start, end));
        }

        display.display();

        // Sirens
        if (data.indexOf("E:1") > -1) playSiren(1); // Ambulance
        else if (data.indexOf("E:2") > -1) playSiren(2); // Police
    }
}
