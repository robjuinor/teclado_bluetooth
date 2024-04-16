#include <BleKeyboard.h>  // Biblioteca responsavel pela emulacao de teclado bluetooth

BleKeyboard teclado_ble;  // Objeto da classe BleKeyboard

#define NUMERO_TECLAS 10  // Numero de botoes B
#define NUMERO_CONFIG 2  // Numero de alavancas de configuracao (necessario rever o codigo se for adicionar mais alavancas)

// Associa botoes a portas do microcontrolador
#define B1 13  // O numero 13 representa a GPIO 13 e assim por diante
#define B2 12
#define B3 14
#define B4 26
#define B5 33
#define B6 15
#define B7 4
#define B8 16
#define B9 17
#define B10 5

// Associa alavancas a portas do microcontrolador
#define A1 18
#define A2 19

// Inicializa estados dos botoes e alavancas
bool estados_botoes[NUMERO_TECLAS + NUMERO_CONFIG] = {false, false, false, false, false, false, false, false, false, false, false, false};
// Lista com os pinos de cada botao e alavanca
int pinos_botoes[NUMERO_TECLAS + NUMERO_CONFIG] = {B1, B2, B3, B4, B5, B6, B7, B8, B9, B10, A1, A2};

// Inicializa as 4 configuracoes possiveis para o teclado (alguns valores sao digitados aqui diferentes do valor real para um notebook comum em pt-BR. Por exemplo, '/' corresponde ao valor real digitado ';')
uint8_t valores_teclas0[NUMERO_TECLAS] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
uint8_t valores_teclas1[NUMERO_TECLAS] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};
uint8_t valores_teclas2[NUMERO_TECLAS] = {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';'};
uint8_t valores_teclas3[NUMERO_TECLAS] = {'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/'};

// Inicializa a configuracao vigente do teclado
uint8_t valores_teclas[NUMERO_TECLAS] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

// Ativa o pull up das portas do microcontrolador para que sejam ativadas com nivel logico 0
void prepara_entradas(){
  for(int contador = 0; contador < NUMERO_TECLAS + NUMERO_CONFIG; contador ++){
    pinMode(pinos_botoes[contador], INPUT_PULLUP);
  }
}

void setup() {
  Serial.begin(115200);  // Baud rate da visualizacao serial
  Serial.println("Inicializando");
  prepara_entradas(); // Ativa o pull up das portas do microcontrolador
  teclado_ble.begin();  // Ativa o teclado bluetooth
}

// Variavel para visualizacao serial, util em debugging
bool notificacaoConexaoEnviada = false;

// Funcao que cuida do aperto de botoes
void interacao_botao(int indice){
  if(!digitalRead(pinos_botoes[indice])){  // Se leu que o botao foi apertado
    if(!estados_botoes[indice]){  // Se o estado anterior era falso
      estados_botoes[indice] = true;  // Atualiza estado para verdadeiro
      teclado_ble.press(valores_teclas[indice]);  // Digita valor do botao no computador
      delay(50);  // Delay 50ms para reduzir efeito de bouncing mecanico com os botoes usados
    }
  }
  else{
    if(estados_botoes[indice]){  // Se o estado anterior era verdadeiro
      estados_botoes[indice] = false;  // Atualiza estado para falso
      teclado_ble.release(valores_teclas[indice]); // Para de digitar valor do botao no computador
    }
  }
}

// Funcao que configura o teclado a partir do nivel logico das alavancas (duas alavancas formando um numero binario de dois digitos, ou seja 4 valores = 4 configuracoes possiveis)
void configurar_teclado(){
  // Confere estado logico das alavancas A1 e A2 e atualiza valor da alavanca A1, alem de escolher o valor dos botoes baseado nos estados logicos de A1 e A2
  if(!digitalRead(pinos_botoes[NUMERO_TECLAS])){  // Leitura da alavanca A1
    estados_botoes[NUMERO_TECLAS] = false;  // Atualiza estado logico alavanca A1
    if(estados_botoes[NUMERO_TECLAS + 1]){  // Confere estado logico alavanca A2
      memcpy(valores_teclas, valores_teclas2, NUMERO_TECLAS);  // Utiliza a configuracao 2 como a vigente
    }
    else{
      memcpy(valores_teclas, valores_teclas3, NUMERO_TECLAS);  // Utiliza a configuracao 3 como a vigente
    }
  }
  else{
    estados_botoes[NUMERO_TECLAS] = true;  // Atualiza estado logico alavanca A1
    if(estados_botoes[NUMERO_TECLAS + 1]){  // Confere estado logico alavanca A2
      memcpy(valores_teclas, valores_teclas0, NUMERO_TECLAS);  // Utiliza a configuracao 0 como a vigente
    }
    else{
      memcpy(valores_teclas, valores_teclas1, NUMERO_TECLAS);  // Utiliza a configuracao 1 como a vigente
    }
  }
  
  // Atualiza estado logico da alavanca A2
  if(!digitalRead(pinos_botoes[NUMERO_TECLAS + 1])){  // Leitura da alavanca A2
    estados_botoes[NUMERO_TECLAS + 1] = false;  // Atualiza estado logico alavanca A2
  }
  else{
    estados_botoes[NUMERO_TECLAS + 1] = true;  // Atualiza estado logico alavanca A2
  }
}

// Loop principal
void loop() {
  int contador;  // Inicializa contagem de iteracao dos botoes
  if(teclado_ble.isConnected()){
    //Notificacao serial de conexao
    if(!notificacaoConexaoEnviada){
      Serial.println("Conectado");  // Visualizacao serial, util em debugging
      notificacaoConexaoEnviada = true;
    }

    // Atualiza configuracao do teclado com base em A1 e A2
    configurar_teclado();

    // Itera as teclas para determinar seu nivel logico e digitar valor no computador
    for(contador = 0; contador < NUMERO_TECLAS; contador ++){
      interacao_botao(contador);
    }
  }
}
