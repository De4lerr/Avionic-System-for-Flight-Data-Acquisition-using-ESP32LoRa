/* Definição de Bibliotecas */
#include <LoRa.h>
#include <SPI.h>
#include <Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_ipc.h>

/* Definicoes para comunicação com radio LoRa */
#define SCK_LORA           5
#define MISO_LORA          19
#define MOSI_LORA          27
#define RESET_PIN_LORA     14
#define SS_PIN_LORA        18
 
#define HIGH_GAIN_LORA     20  /* dBm */
#define BAND               915E6  /* 915MHz de frequencia */

/* Definições para módulo GPS */
#define RX_GPS             2
#define TX_GPS             3
#define BAUD_GPS           9600

/* Definições para acelerômetro */
#define MPU               0x68 // MPU6050 I2C address
#define BAUD_MPU          9600

/* Definicoes gerais */
#define DEBUG_SERIAL_BAUDRATE    9600 //115200 por defaut no LoRa

bool sem;
float * erro_MPU;
char * informacao_a_ser_enviada;
char * dados_GPS;
char * dados_MPU;
char * tempo;

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RX_GPS, TX_GPS);

/* Protótipos de Funções */
  /* LoRa */
    bool init_comunicacao_lora(void);

  /* GPS */
    char * read_GPS(void);

  /* MPU */
    void config_MPU(void);
    float * calculate_IMU_error(void);
    char * read_MPU(float erro[]);

/***************************************************************************************************/

/* setup */
void setup() 
{   
  /* Inicia a comunicação da ESP com demais sensores */
  gpsSerial.begin(BAUD_GPS); //GPS
  Serial.begin(DEBUG_SERIAL_BAUDRATE); //LoRa e MPU
  config_MPU(); //Configura a MPU
  while (!Serial);

  /* Obtem os erros de leitura da MPU */
  erro_MPU=calculate_IMU_error();

  /* Tenta, até obter sucesso, comunicacao com o chip LoRa */
  while(init_comunicacao_lora() == false);
}

/***************************************************************************************************/

/* Programa principal */
  /* Programa na CPU_PRO */
  void LoopOnProCpu(void *arg) 
  {
    (void)arg;
    sem = 1;
    strcpy(dados_MPU,read_MPU(erro_MPU)); //Copia a leitura da MPU
    sem = 0;
  }

  /* Programa na CPU_APP */
  void loop() 
  {
    dtostrf(millis(),10,0,tempo);                    //Passa o tempo atual p/ string
    esp_ipc_call(PRO_CPU_NUM, LoopOnProCpu, NULL);   //Copia a leitura da MPU em outro processo
    strcpy(dados_GPS,read_GPS());                    //Copia a leitura do GPS

    while(sem);                                      //Espera a execução da tarefa da CPU_PRO

    sprintf(informacao_a_ser_enviada,"EPTAP,%s,%s,%s,\n",dados_GPS,dados_MPU,tempo); //Une as informações obtidas numa mesma String

    //Envia a informação e espera 0,1 segundo
    LoRa.beginPacket();
    LoRa.write((unsigned char *)informacao_a_ser_enviada, sizeof(informacao_a_ser_enviada));
    LoRa.endPacket();
  }