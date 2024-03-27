#include <BleKeyboard.h>  // Biblioteca responsavel pela emulacao de teclado bluetooth

BleKeyboard bleKeyboard;  // Objeto da classe BleKeyboard

#define NUMERO_TECLAS 10
#define NUMERO_CONFIG 2

// Associa botoes a portas do microcontrolador
#define GP_0 13
#define GP_1 12
#define GP_2 14
#define GP_3 26
#define GP_4 33
#define GP_5 15
#define GP_6 4
#define GP_7 16
#define GP_8 17
#define GP_9 5

// Associa alavancas a portas do microcontrolador
#define CF_0 18
#define CF_1 19

// Inicializa estados dos botoes e alavancas e uma lista com os pinos de cada botao e alavanca
bool keyStates[NUMERO_TECLAS + NUMERO_CONFIG] = {false, false, false, false, false, false, false, false, false, false, false, false};
int keyPins[NUMERO_TECLAS + NUMERO_CONFIG] = {GP_0, GP_1, GP_2, GP_3, GP_4, GP_5, GP_6, GP_7, GP_8, GP_9, CF_0, CF_1};

// Inicializa as 4 configuracoes possiveis para o teclado
uint8_t keyCodes0[NUMERO_TECLAS] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
uint8_t keyCodes1[NUMERO_TECLAS] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};
uint8_t keyCodes2[NUMERO_TECLAS] = {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';'};
uint8_t keyCodes3[NUMERO_TECLAS] = {'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'};

// Inicializa a configuracao vigente do teclado
uint8_t keyCodes[NUMERO_TECLAS] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

// Ativa o pull up das portas do microcontrolador
void setInputs(){
  for(int counter = 0; counter < NUMERO_TECLAS + NUMERO_CONFIG; counter ++){
    pinMode(keyPins[counter], INPUT_PULLUP);
  }
}

void setup() {
  Serial.begin(115200);  // Baud rate da visualizacao serial
  Serial.println("Starting BLE work!");
  setInputs(); // Ativa o pull up das portas do microcontrolador
  bleKeyboard.begin();  // Ativa o teclado bluetooth
}

// Variavel para visualizacao serial, util em debugging
bool connectNotificationSent = false;

// Funcao que cuida do aperto de botoes
void handleButton(int keyIndex){
  if(!digitalRead(keyPins[keyIndex])){
    if(!keyStates[keyIndex]){
      keyStates[keyIndex] = true;
      bleKeyboard.press(keyCodes[keyIndex]);
      delay(50);
    }
  }
  else{
    if(keyStates[keyIndex]){
      keyStates[keyIndex] = false;
      bleKeyboard.release(keyCodes[keyIndex]);
    }
  }
}

// Funcao que configura o teclado a partir do nivel logico das alavancas (duas alavancas formando um numero binario de dois digitos, ou seja 4 valores = 4 configuracoes possiveis)
void configurar_teclado(){
  if(!digitalRead(keyPins[NUMERO_TECLAS])){
    keyStates[NUMERO_TECLAS] = false;
    if(keyStates[NUMERO_TECLAS + 1]){
      memcpy(keyCodes, keyCodes2, NUMERO_TECLAS);
    }
    else{
      memcpy(keyCodes, keyCodes3, NUMERO_TECLAS);
    }
  }
  else{
    keyStates[NUMERO_TECLAS] = true;
    if(keyStates[NUMERO_TECLAS + 1]){
      memcpy(keyCodes, keyCodes0, NUMERO_TECLAS);
    }
    else{
      memcpy(keyCodes, keyCodes1, NUMERO_TECLAS);
    }
  }
  

  if(!digitalRead(keyPins[NUMERO_TECLAS + 1])){
    keyStates[NUMERO_TECLAS + 1] = false;
  }
  else{
    keyStates[NUMERO_TECLAS + 1] = true;
  }
}

// Loop principal
void loop() {
  int counter;  // Inicializa contagem de iteracao dos botoes
  if(bleKeyboard.isConnected()){
    if(!connectNotificationSent){
      Serial.println("Code connected...");  // Visualizacao serial, util em debugging
      connectNotificationSent = true;
    }

    configurar_teclado();

    // Itera as teclas para determinar seu nivel logico
    for(counter = 0; counter < NUMERO_TECLAS; counter ++){
      handleButton(counter);
    }
  }
}