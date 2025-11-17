// ------------------- Inclusión de librerías -------------------
#include <Arduino.h>
#include <DHT22.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

// ------------------- Definiciones -------------------
#define Sens_HUM1 36
#define Sens_HUM2 39
#define Sens_HUM3 34
#define INDI_LED 23 
#define TEMP 0
#define RELE_PIN 12

// Configuración PWM
const int PWM_CHANNEL = 0;     
const int PWM_FREQ = 50;      
const int PWM_RESOLUTION = 16;  
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);
const int LED_OUTPUT_PIN = 18;   // Usamos pin 18 para PWM (separado del LED indicador)
const int DELAY_MS = 4;          // Delay para fade

// WiFi
const char* ssid       = "Electronica_7moB";
const char* password   = "proyecto7B!";

// NTP
const long gmtOffset_sec = -10800;  
const int daylightOffset_sec = 0;
const char* ntpServer = "pool.ntp.org";

// Variables globales
uint16_t HUM_1 = 0;
uint16_t HUM_2 = 0;
uint16_t HUM_3 = 0;
float t = 0;
bool franja_OK = 0;
uint16_t PWM_value = 1700;   // Valor inicial PWM

LiquidCrystal_I2C lcd(0x27, 16, 2); 
DHT22 dht22(TEMP); 
WebServer server(80);

// ------------------- Setup -------------------
void setup() {
  Serial.begin(115200);
  pinMode(TEMP, INPUT);
  pinMode(INDI_LED, OUTPUT);
  pinMode(RELE_PIN, OUTPUT);

  // Conexión WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi");
  Serial.println("IP local: " + WiFi.localIP().toString());

  // Configuración NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Servidor web
  server.on("/", handleRoot);
  server.on("/datos", handleDatos);
  server.begin();

  // Configuración PWM
//  ledcAttachChannel(LED_OUTPUT_PIN, PWM_FREQ, PWM_RESOLUTION, PWM_CHANNEL);
}

// ------------------- Loop -------------------
void loop() {
  server.handleClient();

  // Ejemplo de variación PWM (fade simple)
/*  digitalWrite(RELE_PIN, LOW);
 // ledcWriteChannel(PWM_CHANNEL, 1700);
  delay(1000);
  digitalWrite(RELE_PIN, HIGH);
  //ledcWriteChannel(PWM_CHANNEL, 8250);
  delay(1000);*/
}

// ------------------- Funciones auxiliares -------------------
int getSoil_moisture(int num){
  uint16_t hum = analogRead(num);
  int S_HumD = (-0.0367 * hum) + 150.2865;
  return constrain(S_HumD, 0, 100);
}

void get_data(void){
  t = dht22.getTemperature();
  HUM_1 = getSoil_moisture(Sens_HUM1);
  HUM_2 = getSoil_moisture(Sens_HUM2);
  HUM_3 = getSoil_moisture(Sens_HUM3);  
  if (dht22.getLastError() != dht22.OK) {
    Serial.print("Error DHT22: ");
    Serial.println(dht22.getLastError()); 
  }
}

void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html><html><head><meta charset='UTF-8'><title>Datos SRI</title>
    <script>
      function actualizarDatos() {
        fetch('/datos')
          .then(response => response.json())
          .then(data => {
            document.getElementById('temp').innerText = data.temperatura + ' °C';
            document.getElementById('hum1').innerText = data.hum1 + ' %';
            document.getElementById('hum2').innerText = data.hum2 + ' %';
            document.getElementById('hum3').innerText = data.hum3 + ' %';
            document.getElementById('franja').innerText = data.franja ? 'ACTIVA' : 'INACTIVA';
          });
      }
      setInterval(actualizarDatos, 2000);
    </script></head><body>
    <h2>Proyecto SRI - Datos en tiempo real</h2>
    <p>Temperatura: <span id='temp'>--</span></p>
    <p>Humedad 1: <span id='hum1'>--</span></p>
    <p>Humedad 2: <span id='hum2'>--</span></p>
    <p>Humedad 3: <span id='hum3'>--</span></p>
    <p>Franja horaria: <span id='franja'>--</span></p>
    </body></html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleDatos() {
  get_data(); 

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    int hora = timeinfo.tm_hour;
    int minuto = timeinfo.tm_min;
    franja_OK = ((hora == 5 && minuto >= 0) || (hora >= 6 && hora <= 9) || 
                 (hora == 14 && minuto >= 41) || (hora >= 22 && hora <= 24));
    digitalWrite(INDI_LED, franja_OK ? HIGH : LOW);
  }

  String json = "{";
  json += "\"temperatura\":" + String(t) + ",";
  json += "\"hum1\":" + String(HUM_1) + ",";
  json += "\"hum2\":" + String(HUM_2) + ",";
  json += "\"hum3\":" + String(HUM_3) + ",";
  json += "\"franja\":" + String(franja_OK ? "true" : "false");
  json += "}";

  server.send(200, "application/json", json);
}