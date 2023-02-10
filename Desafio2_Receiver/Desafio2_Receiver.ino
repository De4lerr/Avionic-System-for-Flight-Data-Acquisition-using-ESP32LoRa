#include <LoRa.h>
#include <SPI.h>
 
/* Definicoes para comunicação com radio LoRa */
#define SCK_LORA           5
#define MISO_LORA          19
#define MOSI_LORA          27
#define RESET_PIN_LORA     14
#define SS_PIN_LORA        18
 
#define HIGH_GAIN_LORA     20  /* dBm */
#define BAND               915E6  /* 915MHz de frequencia */
 
/* Definicoes gerais */
#define DEBUG_SERIAL_BAUDRATE    9600 //115200 por defaut no LoRa

String e="E";
String p="P";
String t="T";
String a="A";
const char * dados[] = {"HORA","MINUTO","SEGUNDO","DIA","MÊS","ANO","LATIDUTE","LONGITUDE","ACELERAÇÃO X","ACELERAÇÃO Y","ACELERAÇÃO Z","GIRO X","GIRO Y","GIRO Z","TEMPO EM EXECUÇÃO"};

/* Funcao de setup */
void setup() 
{
    Serial.begin(DEBUG_SERIAL_BAUDRATE);
    while (!Serial);
 
    /* Tenta, até obter sucesso, comunicacao com o chip LoRa */
    while(init_comunicacao_lora() == false);       
}
 
/* Programa principal */
void loop() 
{
  String informacaoRecebida, informacaoPrint; 
  int packetSize = LoRa.parsePacket();
  int i, d = 0;
  
  if (packetSize) 
  {
    // received a packet
    Serial.print("\n\nPacote recebido!!\n");

      // read packet
      while (LoRa.available())
      {
        informacaoRecebida = LoRa.readString();
        if(informacaoRecebida[0]==e[0] && informacaoRecebida[1]==p[0] && informacaoRecebida[2]==t[0] && informacaoRecebida[3]==a[0] && informacaoRecebida[4]==p[0]) //Verifica se são dados que queremos
        { 
          informacaoRecebida.remove(0,5); //Remove a verificação EPTAP
          
          for(i=0;i<=packetSize;i++)
          {
            if (informacaoRecebida[i]==',')
            {
              Serial.print(dados[d]);
              Serial.print(": ");
              Serial.print(informacaoPrint);
              Serial.print("\n");
              informacaoPrint="";
              d++;
            }else{
              informacaoPrint=informacaoPrint+informacaoRecebida[i];
            }
          }

          // print da força do sinal do pacote recebido
          Serial.print("' Força do Sinal: ");
          Serial.println(LoRa.packetRssi());
        }
      }
  }
}
