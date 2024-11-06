#include <CAN.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definição dos pinos para comunicação CAN
#define TX_GPIO_NUM   5     // Pino de transmissão (TX) do barramento CAN
#define RX_GPIO_NUM   4     // Pino de recepção (RX) do barramento CAN

// Configuração do display OLED
#define SCREEN_WIDTH 128    // Largura do display OLED em pixels
#define SCREEN_HEIGHT 64    // Altura do display OLED em pixels
#define OLED_RESET    -1    // Pino de reset do OLED
#define SCREEN_ADDRESS 0x3C // Endereço I2C padrão para o display OLED

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Inicializa o display OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Falha na inicialização do display SSD1306"));
    for(;;); // Loop infinito se o display falhar ao iniciar
  }
  
  display.clearDisplay();         // Limpa o buffer do display
  display.setTextSize(1);         // Define o tamanho do texto como 1 (padrão)
  display.setTextColor(WHITE);    // Define a cor do texto como branca
  
  // Mensagem inicial de configuração no display OLED
  display.setCursor(0, 0);
  display.println("Inicializando CAN...");
  display.display();

  // Configuração do barramento CAN
  CAN.setPins(RX_GPIO_NUM, TX_GPIO_NUM);
  if (!CAN.begin(500E3)) {  // Inicializa o CAN a 500 kbps
    Serial.println("Erro ao iniciar o barramento CAN!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Erro no CAN!");
    display.display();
    while (1);
  }
  
  // Exibe uma mensagem de sucesso no display OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Receptor CAN OK");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Tenta analisar o pacote recebido
  int packetSize = CAN.parsePacket();

  if (packetSize) {  // Se um pacote foi recebido
    Serial.print("Pacote recebido: ");

    // Verifica se o pacote é estendido
    if (CAN.packetExtended()) {
      Serial.print("estendido ");
    }

    // Exibe o ID do pacote
    Serial.print("com ID 0x");
    Serial.print(CAN.packetId(), HEX);
    Serial.print(" e tamanho ");
    Serial.println(packetSize);

    // Inicializa uma string para armazenar a leitura recebida
    String leituraRecebida = "";

    // Lê os dados do pacote
    while (CAN.available()) {
      char c = CAN.read(); // Lê cada byte do pacote
      leituraRecebida += c; // Adiciona à string
    }

    // Converte a leitura para um número de ponto flutuante
    float leituraKPa = leituraRecebida.toFloat();
    // Converte para psi
    float leituraPsi = leituraKPa * 0.145038; // kPa para psi

    // Exibe a leitura recebida no monitor serial
    Serial.print("Leitura recebida: ");
    Serial.print(leituraKPa);
    Serial.print(" kPa, ");
    Serial.print(leituraPsi);
    Serial.println(" psi");
    Serial.println(); // Nova linha para melhor legibilidade

    // Atualiza o display OLED com as leituras
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Leitura de Pressao");
    display.setTextSize(2);
    display.setCursor(0, 16);
    display.print(leituraKPa);
    display.print(" kPa");
    display.setCursor(0, 40);
    display.print(leituraPsi);
    display.print(" psi");
    display.display();
  }
}
