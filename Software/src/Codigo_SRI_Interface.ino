#include <Arduino.h>
#include <DHT22.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

// ------------------- Definiciones -------------------
#define Sens_HUM1 36
#define Sens_HUM2 39
#define Sens_HUM3 34
#define SERVO_1 14
#define SERVO_2 27
#define SERVO_3 26
#define INDI_LED 23 
#define TEMP 0
#define BOMBA 19

// Configuración PWM
const int PWM_CHANNEL_SERVO_1 = 0;
const int PWM_CHANNEL_SERVO_2 = 1;
const int PWM_CHANNEL_SERVO_3 = 2;

const int PWM_FREQ = 50;      
const int PWM_RESOLUTION = 16;  
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);

const int LED_OUTPUT_PIN_1 = SERVO_1;   // Usamos pin 18 para PWM (separado del LED indicador)
const int LED_OUTPUT_PIN_2 = SERVO_2;   // Usamos pin 18 para PWM (separado del LED indicador)
const int LED_OUTPUT_PIN_3 = SERVO_3;   // Usamos pin 18 para PWM (separado del LED indicador)

const int DELAY_MS = 4;          // Delay para fade9

// WiFi
const char* ssid       = "BERNAT-2.4G-52tW"; //"Electronica_7moB";
const char* password   =  "Valentina_2022*"; //"proyecto7B!";

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

uint16_t PWM_value_1 = 8000;   // Valor inicial PWM
uint16_t PWM_value_2 = 4875;   // Valor inicial PWM
uint16_t PWM_value_3 = 1750;   // Valor inicial PWM

bool riego1_activo = false;
bool riego2_activo = false;
bool riego3_activo = false; 

DHT22 dht22(TEMP); 
WebServer server(80);

// Funciones Riego
void riego_Sector1(void);
void riego_Sector2(void);
void riego_Sector3(void);

// ------------------- Setup -------------------
void setup() {
  Serial.begin(9600);
  pinMode(TEMP, INPUT);
  pinMode(INDI_LED, OUTPUT);
  pinMode(BOMBA, OUTPUT);

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
  ledcSetup(PWM_CHANNEL_SERVO_1, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_OUTPUT_PIN_1, PWM_CHANNEL_SERVO_1);

  ledcSetup(PWM_CHANNEL_SERVO_2, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_OUTPUT_PIN_2, PWM_CHANNEL_SERVO_2);

  ledcSetup(PWM_CHANNEL_SERVO_3, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_OUTPUT_PIN_3, PWM_CHANNEL_SERVO_3);
}

// ------------------- Loop -------------------
void loop() {
  server.handleClient();
if(t >= 10 && t <= 25 && franja_OK == true){
    riego_Sector1();
    riego_Sector2();
    riego_Sector3();
  }
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
void riego_Sector1(void){
  // Código para riego del Sector 1
  if(HUM_1 < 70 ){
    ledcWrite(PWM_CHANNEL_SERVO_1, PWM_value_1);
    ledcWrite(PWM_CHANNEL_SERVO_2, PWM_value_1);
    ledcWrite(PWM_CHANNEL_SERVO_3, PWM_value_1);
    digitalWrite(BOMBA, HIGH); // Activar riego

    riego1_activo = true;

    while (HUM_1 < 50)
    {
      server.handleClient();
      get_data();
    }
    riego1_activo = false;
    digitalWrite(BOMBA, LOW); // Desactivar riego
    
  }
}
void riego_Sector2(void){
  // Código para riego del Sector 2
  if(HUM_2 < 70 ){
    ledcWrite(PWM_CHANNEL_SERVO_1, PWM_value_2);
    ledcWrite(PWM_CHANNEL_SERVO_2, PWM_value_2);
    ledcWrite(PWM_CHANNEL_SERVO_3, PWM_value_2);
    digitalWrite(BOMBA, HIGH); // Activar riego

    riego2_activo = true;

    while (HUM_2 < 50)
    {
      server.handleClient();
      get_data();
    }
    riego2_activo = false;
    digitalWrite(BOMBA, LOW); // Desactivar riego
    
  }
}
void riego_Sector3(void){
  // Código para riego del Sector 3
  if(HUM_3 < 70 ){
    ledcWrite(PWM_CHANNEL_SERVO_1, PWM_value_3);
    ledcWrite(PWM_CHANNEL_SERVO_2, PWM_value_3);
    ledcWrite(PWM_CHANNEL_SERVO_3, PWM_value_3);
    digitalWrite(BOMBA, HIGH); // Activar riego

    riego3_activo = true;

    while (HUM_3 < 50)
    {
      server.handleClient();
      get_data();
    }
    riego3_activo = false;
    digitalWrite(BOMBA, LOW); // Desactivar riego
    
  }
}