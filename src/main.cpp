#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

// Definições de pinos
#define BTN_P1 32
#define BTN_P2 35
#define BTN_P3 34
#define SERVO_PIN 2

// Definições de ângulos do servo para cada posição
#define ANGULO_P1 0
#define ANGULO_P2 90
#define ANGULO_P3 180

// Configurações do LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C comum para LCD1602

// Servo motor
Servo meuServo;

// Estado atual
volatile bool movimentoSolicitado = false;
volatile int destinoSolicitado = 0;
int posicaoAtual = 1; // Inicialmente na posição 1

// Protótipo da função
void moverPara(int destino);

// Funções de interrupção
void IRAM_ATTR tratarBotaoP1() {
  if (!movimentoSolicitado) {
    destinoSolicitado = 1;
    movimentoSolicitado = true;
  }
}

void IRAM_ATTR tratarBotaoP2() {
  if (!movimentoSolicitado) {
    destinoSolicitado = 2;
    movimentoSolicitado = true;
  }
}

void IRAM_ATTR tratarBotaoP3() {
  if (!movimentoSolicitado) {
    destinoSolicitado = 3;
    movimentoSolicitado = true;
  }
}

void setup() {
  // Inicializar comunicação serial
  Serial.begin(115200);

  // Configurar pinos dos botões
  pinMode(BTN_P1, INPUT_PULLUP);
  pinMode(BTN_P2, INPUT_PULLUP);
  pinMode(BTN_P3, INPUT_PULLUP);

  // Anexar interrupções
  attachInterrupt(digitalPinToInterrupt(BTN_P1), tratarBotaoP1, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_P2), tratarBotaoP2, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_P3), tratarBotaoP3, FALLING);

  // Inicializar o servo
  meuServo.attach(SERVO_PIN);

  // Inicializar o LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Sistema Pronto");

  // Inicializar servo na posição 1
  meuServo.write(ANGULO_P1);
}

void loop() {
  if (movimentoSolicitado) {
    // Desabilitar interrupções temporariamente
    noInterrupts();
    int destino = destinoSolicitado;
    movimentoSolicitado = false;
    interrupts();

    // Validar se é movimento permitido
    if (destino != posicaoAtual) {
      moverPara(destino);
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ja em P");
      lcd.print(destino);
      delay(2000);
    }
  }
}

void moverPara(int destino) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Movendo para P");
  lcd.print(destino);
  Serial.printf("Movendo para P%d...\n", destino);

  // Simular tempo de deslocamento
  delay(2000);

  // Definir ângulo correspondente
  switch (destino) {
    case 1:
      meuServo.write(ANGULO_P1);
      break;
    case 2:
      meuServo.write(ANGULO_P2);
      break;
    case 3:
      meuServo.write(ANGULO_P3);
      break;
    default:
      break;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Chegou em P");
  lcd.print(destino);
  Serial.printf("Chegou em P%d\n", destino);

  posicaoAtual = destino;

  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Em repouso...");
}
