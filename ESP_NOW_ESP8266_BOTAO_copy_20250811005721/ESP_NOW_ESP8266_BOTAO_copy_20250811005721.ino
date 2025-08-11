#include <ESP8266WiFi.h>
#include <espnow.h>

// MAC da placa ESP32-C3 (destino)
//uint8_t broadcastAddress[] = { 0x40, 0x4C, 0xCA, 0xF5, 0x75, 0x88 };
//MAC Address: 84:FC:E6:00:2B:10
uint8_t broadcastAddress[] = { 0x84, 0xFC, 0xE6, 0x00, 0x2B, 0x10 };

// Estrutura dos dados a serem enviados
typedef struct struct_message {
  uint8_t buttonState;  // Estado do botão (0 = solto, 1 = pressionado)
} struct_message;

struct_message myData;

const int BUTTON_PIN = 5; // GPIO5 (D1) no ESP8266

// Callback para verificar status do envio
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("[ENVIO] Status: ");
  Serial.println(sendStatus == 0 ? "SUCESSO" : "FALHA");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== ESP8266 - Transmissor (Botão) ===");

  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Botão com pull-up interno

  WiFi.mode(WIFI_STA);
  Serial.print("MAC Local (ESP8266): ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != 0) {
    Serial.println("[ERRO] Falha ao iniciar ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER); // Define como controlador
  esp_now_register_send_cb(OnDataSent);

  // Adiciona peer (destino)
  if (esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0) != 0) {
    Serial.println("[ERRO] Falha ao adicionar peer");
    return;
  }
  Serial.println("[OK] Peer adicionado com sucesso");
}

void loop() {
  // Lê estado do botão (LOW = pressionado por pull-up)
  int btnState = digitalRead(BUTTON_PIN);
  myData.buttonState = (btnState == LOW) ? 1 : 0;

  // Envia estado do botão para ESP32-C3
  esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  // Debug no monitor serial
  Serial.print("[BOTÃO] Estado: ");
  Serial.println(myData.buttonState);

  delay(50); // Evita flood no envio
}
