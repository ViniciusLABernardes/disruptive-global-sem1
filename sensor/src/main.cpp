#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Credenciais Wi-Fi
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// API do ThingSpeak
const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "3QX1ZDAA06RMTWTL";

// Pinos dos sensores (todos usando potenciômetro)
#define PIN_UMIDADE     32
#define PIN_VIBRACAO    33
#define PIN_INCLINACAO  34

unsigned long lastUpdate = 0;
const int intervalo = 2000; // 15 segundos

int leituraSuave(int pin, int n=10) {
  long soma = 0;
  for (int i=0; i<n; i++) {
    soma += analogRead(pin);
    delay(5);
  }
  return soma / n;
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando-se ao Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi conectado!");
}

void loop() {
  if (millis() - lastUpdate >= intervalo) {
    lastUpdate = millis();
 int leituraUmidade = leituraSuave(PIN_UMIDADE);
int leituraVibracao = leituraSuave(PIN_VIBRACAO);
int leituraInclinacao = leituraSuave(PIN_INCLINACAO);

  float umidade = leituraUmidade * 100.0 / 4095.0;              // 0-100%
  float vibracaoRichter = leituraVibracao * 9.0 / 4095.0;        // 0.0 a 9.0 escala Richter
  int inclinacao = leituraInclinacao * 180.0 / 4095.0;           // 0-180°

  Serial.printf("Umidade: %.2f %% | Vibração: %.2f Richter | Inclinação: %d°\n",
                umidade, vibracaoRichter, inclinacao);


    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = String(server) + "?api_key=" + apiKey +
                   "&field1=" + String(umidade, 2) +
                   "&field2=" + String(vibracaoRichter, 2) +
                   "&field3=" + String(inclinacao);

      http.begin(url);
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.println("Enviado ao ThingSpeak com sucesso!");
      } else {
        Serial.printf("Erro ao enviar: %d\n", httpResponseCode);
      }

      http.end();
    }
  }
}