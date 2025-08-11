#include <WiFi.h>
#include <esp_now.h>

// MAC da placa ESP8266 que envia o comando do botão
uint8_t peerAddress[] = { 0xEC, 0xFA, 0xBC, 0xD4, 0xE6, 0xA7 };

// Estrutura dos dados recebidos
typedef struct struct_message {
  uint8_t buttonState;  // Estado do botão (0 = solto, 1 = pressionado)
} struct_message;

struct_message incomingData;

const int LED_PIN = 4; // Pino do LED na ESP32-C3 (GPIO4)

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  Serial.println("[RECEPCAO] Pacote recebido!");

  // Mostrar o endereço MAC de quem enviou
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           info->src_addr[0], info->src_addr[1], info->src_addr[2],
           info->src_addr[3], info->src_addr[4], info->src_addr[5]);
  Serial.print("[RECEPCAO] De: ");
  Serial.println(macStr);

  // Copia os dados recebidos para a estrutura
  memcpy(&incomingData, data, sizeof(incomingData));

  Serial.print("[RECEPCAO] Estado do botão: ");
  Serial.println(incomingData.buttonState);

  // Aciona ou desliga o LED conforme estado do botão
  if (incomingData.buttonState == 1) {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("[LED] LIGADO");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("[LED] DESLIGADO");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("=== ESP32-C3 Super Mini - Receptor (LED) ===");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  WiFi.mode(WIFI_STA); // Modo estação (sem AP)
  Serial.print("MAC Local (ESP32-C3): ");
  Serial.println(WiFi.macAddress());

  // Inicializa ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("[ERRO] Falha ao iniciar ESP-NOW");
    return;
  }

  // Registra callback de recepção de dados
  esp_now_register_recv_cb(OnDataRecv);

  // Configura o peer (ESP8266) para receber os dados dele
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 1;    // Canal Wi-Fi (geralmente 1)
  peerInfo.encrypt = false; // Sem criptografia

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("[ERRO] Falha ao adicionar peer");
    return;
  }
  Serial.println("[OK] Peer adicionado com sucesso");
}

void loop() {
  // Nada a fazer no loop, tudo ocorre no callback
}
