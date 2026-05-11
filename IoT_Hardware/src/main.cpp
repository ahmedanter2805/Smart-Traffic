#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- WIFI CONFIG ---
const char* ssid = "AHMED";
const char* password = "AHMEDAHMED10";

// --- LED PINS (Back to 3 LEDs as requested) ---
const int RED_EMERGENCY = 25; // High Priority Alert
const int YELLOW_TICKET = 26; // Violation Alert
const int GREEN_TRAFFIC = 27; // Traffic Status

const int BUZZER_SIREN = 33;
const int BUZZER_ALARM = 32;

Adafruit_SSD1306 display(128, 64, &Wire, -1);
WiFiUDP udp;

unsigned long lastSirenTime = 0;
int sirenState = 0;

void handleSirens(int type) {
    if (type == 0) { noTone(BUZZER_SIREN); digitalWrite(RED_EMERGENCY, LOW); return; }
    digitalWrite(RED_EMERGENCY, HIGH); // Red LED ON during any Emergency
    unsigned long now = millis();
    if (type == 1 && now - lastSirenTime > 200) { 
        sirenState = !sirenState; tone(BUZZER_SIREN, sirenState ? 1500 : 2500); lastSirenTime = now;
    } else if (type == 2 && now - lastSirenTime > 100) { 
        sirenState = !sirenState; tone(BUZZER_SIREN, sirenState ? 1000 : 3000); lastSirenTime = now;
    } else if (type == 3 && now - lastSirenTime > 400) { 
        sirenState = !sirenState; tone(BUZZER_SIREN, sirenState ? 600 : 1800); lastSirenTime = now;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(RED_EMERGENCY, OUTPUT);
    pinMode(YELLOW_TICKET, OUTPUT);
    pinMode(GREEN_TRAFFIC, OUTPUT);
    pinMode(BUZZER_SIREN, OUTPUT);
    pinMode(BUZZER_ALARM, OUTPUT);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Connecting WiFi...");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); }
    udp.begin(4210);
    
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("WIFI: CONNECTED");
    display.println("IP: " + WiFi.localIP().toString());
    display.display();
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        char buf[255]; int len = udp.read(buf, 255); if (len > 0) buf[len] = 0;
        String data = String(buf);

        // Green LED reflects the Main Road (North)
        int lIdx = data.indexOf("L:") + 2;
        if (lIdx > 1) {
            digitalWrite(GREEN_TRAFFIC, data[lIdx] == 'G'); 
        }

        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("SMART CITY MONITOR");
        display.drawLine(0, 12, 128, 12, WHITE);
        
        display.setCursor(0, 16);
        if (lIdx > 1) {
            display.printf("N:%c S:%c W:%c E:%c\n", data[lIdx], data[lIdx+1], data[lIdx+2], data[lIdx+3]);
        }
        
        if (data.indexOf("C:") > -1) {
            int s = data.indexOf("C:")+2; int e = data.indexOf("|", s);
            display.setCursor(0, 30);
            display.println("Total Cars: " + data.substring(s, e));
        }

        // Yellow LED Flashes for violations
        if (data.indexOf("V:") > -1) {
            int vStart = data.indexOf("V:") + 2;
            String vID = data.substring(vStart);
            if (vID.toInt() != -1) {
                digitalWrite(YELLOW_TICKET, HIGH);
                display.fillRect(0, 42, 128, 11, WHITE);
                display.setTextColor(BLACK); display.setCursor(2, 44);
                display.println("TICKET: CAR #" + vID);
                display.setTextColor(WHITE);
                tone(BUZZER_ALARM, 3500, 200); 
            } else {
                digitalWrite(YELLOW_TICKET, LOW);
            }
        }

        if (data.indexOf("E:1") > -1) { display.setCursor(0, 55); display.println("ALRT: AMBULANCE"); handleSirens(1); }
        else if (data.indexOf("E:2") > -1) { display.setCursor(0, 55); display.println("ALRT: POLICE"); handleSirens(2); }
        else if (data.indexOf("E:3") > -1) { display.setCursor(0, 55); display.println("ALRT: FIRE TRUCK"); handleSirens(3); }
        else { handleSirens(0); }

        display.display();
    }
}
