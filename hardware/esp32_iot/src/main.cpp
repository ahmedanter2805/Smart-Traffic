#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- WIFI CONFIG ---
const char* ssid = "AHMED ANTER 2.4G";
const char* password = "19712805Aa#";

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

    // Initial OLED setup - Try both common addresses
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
            Serial.println("OLED failed on both 0x3C and 0x3D");
        }
    }
    
    // Test Buzzer on startup
    digitalWrite(BUZZER, HIGH); delay(100); digitalWrite(BUZZER, LOW);
    tone(BUZZER, 1000, 200); 

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

unsigned long lastSirenTime = 0;
int sirenState = 0;

void handleSirens(int type) {
    if (type == 0) { noTone(BUZZER); return; }
    
    unsigned long now = millis();
    if (type == 1) { // Ambulance - Piercing High-Low
        if (now - lastSirenTime > 250) {
            sirenState = !sirenState;
            tone(BUZZER, sirenState ? 2500 : 3500); // Higher frequencies for more volume
            lastSirenTime = now;
        }
    } else if (type == 2) { // Police - Rapid Yelp
        if (now - lastSirenTime > 150) {
            sirenState = !sirenState;
            tone(BUZZER, sirenState ? 2000 : 3000);
            lastSirenTime = now;
        }
    }
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        char incomingPacket[255];
        int len = udp.read(incomingPacket, 255);
        if (len > 0) incomingPacket[len] = 0;
        String data = String(incomingPacket);

        // Instant LED Update
        if (data.indexOf("L:G") > -1) { digitalWrite(GREEN_LED, 1); digitalWrite(RED_LED, 0); digitalWrite(YELLOW_LED, 0); }
        else if (data.indexOf("L:Y") > -1) { digitalWrite(YELLOW_LED, 1); digitalWrite(RED_LED, 0); digitalWrite(GREEN_LED, 0); }
        else if (data.indexOf("L:R") > -1) { digitalWrite(RED_LED, 1); digitalWrite(GREEN_LED, 0); digitalWrite(YELLOW_LED, 0); }

        // OLED Update
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("SMART TRAFFIC IoT");
        display.drawLine(0, 10, 128, 10, WHITE);
        display.setCursor(0, 20);
        
        if (data.indexOf("C:") > -1) {
            int start = data.indexOf("C:") + 2;
            int end = data.indexOf("|", start);
            display.println("Cars: " + data.substring(start, (end == -1 ? data.length() : end)));
        }
        
        if (data.indexOf("V:") > -1) {
            int vStart = data.indexOf("V:") + 2;
            String vID = data.substring(vStart);
            if (vID.toInt() != -1) {
                display.fillRect(0, 30, 128, 20, WHITE);
                display.setTextColor(BLACK);
                display.setCursor(5, 35);
                display.println("!! VIOLATION: #" + vID);
                display.setTextColor(WHITE);
                tone(BUZZER, 4000, 500); // Sharp alarm for violation
            }
        }
        
        if (data.indexOf("E:1") > -1) { display.setCursor(0, 55); display.println("EMERGENCY: AMB"); handleSirens(1); }
        else if (data.indexOf("E:2") > -1) { display.setCursor(0, 55); display.println("EMERGENCY: POL"); handleSirens(2); }
        else { handleSirens(0); }

        display.display();
    }
}
