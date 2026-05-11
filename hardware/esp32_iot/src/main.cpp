#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- WIFI CONFIG ---
const char* ssid = "AHMED ANTER 2.4G";
const char* password = "19712805Aa#";

// --- LED PINS (12 LEDs) ---
const int L_N[] = {25, 26, 27}; 
const int L_S[] = {14, 12, 13};
const int L_W[] = {2, 4, 5};
const int L_E[] = {18, 19, 23};

const int BUZZER_SIREN = 33;
const int BUZZER_ALARM = 32;

Adafruit_SSD1306 display(128, 64, &Wire, -1);
WiFiUDP udp;

void setRoadLights(const int pins[], char state) {
    digitalWrite(pins[0], state == 'R');
    digitalWrite(pins[1], state == 'Y');
    digitalWrite(pins[2], state == 'G');
}

unsigned long lastSirenTime = 0;
int sirenState = 0;

void handleSirens(int type) {
    if (type == 0) { noTone(BUZZER_SIREN); return; }
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
    int allPins[] = {25,26,27,14,12,13,2,4,5,18,19,23,33,32};
    for(int p : allPins) pinMode(p, OUTPUT);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        display.begin(SSD1306_SWITCHCAPVCC, 0x3D);
    }
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Connecting to WiFi...");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) { delay(500); }
    udp.begin(4210);
    
    display.clearDisplay();
    display.println("MEGA CITY IoT READY");
    display.println("IP: " + WiFi.localIP().toString());
    display.display();
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        char buf[255]; int len = udp.read(buf, 255); if (len > 0) buf[len] = 0;
        String data = String(buf);

        if (data.indexOf("L:") > -1) {
            int s = data.indexOf("L:") + 2;
            setRoadLights(L_N, data[s]); setRoadLights(L_S, data[s+1]);
            setRoadLights(L_W, data[s+2]); setRoadLights(L_E, data[s+3]);
        }

        display.clearDisplay();
        display.setCursor(0,0);
        display.println("CITY CONTROL CENTER");
        display.drawLine(0, 8, 128, 8, WHITE);
        
        display.setCursor(0, 10);
        int lIdx = data.indexOf("L:") + 2;
        display.printf("N:%c S:%c W:%c E:%c\n", data[lIdx], data[lIdx+1], data[lIdx+2], data[lIdx+3]);
        
        if (data.indexOf("C:") > -1) {
            int s = data.indexOf("C:")+2; int e = data.indexOf("|", s);
            display.println("Vehicles: " + data.substring(s, e));
        }

        if (data.indexOf("V:") > -1) {
            int vStart = data.indexOf("V:") + 2;
            String vID = data.substring(vStart);
            if (vID.toInt() != -1) {
                display.fillRect(0, 32, 128, 12, WHITE);
                display.setTextColor(BLACK); display.setCursor(2, 34);
                display.println("!! TICKET CAR #" + vID);
                display.setTextColor(WHITE);
                tone(BUZZER_ALARM, 3000, 200); 
            }
        }

        if (data.indexOf("E:1") > -1) { display.setCursor(0, 50); display.println("ALRT: AMBULANCE"); handleSirens(1); }
        else if (data.indexOf("E:2") > -1) { display.setCursor(0, 50); display.println("ALRT: POLICE"); handleSirens(2); }
        else if (data.indexOf("E:3") > -1) { display.setCursor(0, 50); display.println("ALRT: FIRE TRUCK"); handleSirens(3); }
        else { handleSirens(0); }

        display.display();
    }
}
