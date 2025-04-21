#include <WiFi.h>
#include <HTTPClient.h>

// WiFi
const char* ssid = "GCUnet_Leni";
const char* password = "cocacola123";

// API Node.js
const char* serverName = "http://127.0.0.1:3000/api/ph";  // Altere se necessário

// Sensor de pH
const int ph_pin = 34;
const float neutralVoltage = 2.5;
const float acidFactor = 0.18;

int contagem = 0;
const int maxEnvios = 1;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Conecta no Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Conectado ao Wi-Fi!");
}

void loop() {
  if (contagem >= maxEnvios) return;

  int raw = analogRead(ph_pin);
  float voltage = (3.3 / 4095.0) * raw;
  float ph = 7 + ((neutralVoltage - voltage) / acidFactor);

  Serial.print("📤 Enviando leitura: pH = ");
  Serial.print(ph, 2);
  Serial.print(" | Raw = ");
  Serial.println(raw);

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    String json = "{\"ph\":" + String(ph, 2) + ",\"raw\":" + String(raw) + "}";
    int response = http.POST(json);

    if (response > 0) {
      Serial.println("✅ Enviado com sucesso!");
    } else {
      Serial.print("❌ Falha ao enviar. Código: ");
      Serial.println(response);
    }

    http.end();
  } else {
    Serial.println("⚠️ Wi-Fi desconectado.");
  }

  contagem++;
  delay(5000);  // Aguarda 5s entre cada envio
}
