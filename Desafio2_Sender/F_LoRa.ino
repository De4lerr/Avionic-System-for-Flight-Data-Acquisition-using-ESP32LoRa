/***************************************************************************************************/

/* Funções usadas para controle do rádio LoRa */

/***************************************************************************************************/

/* Chamada: init_comunicacao_lora(); 
 * Função: inicia comunicação com chip LoRa; 
 * Parametros: nenhum; 
 * Retorno: true: comunicacao ok
 *          false: falha na comunicacao
*/

bool init_comunicacao_lora(void)
{
    bool status_init = false;
    Serial.println("[LoRa Sender] Tentando iniciar comunicacao com o radio LoRa...");
    SPI.begin(SCK_LORA, MISO_LORA, MOSI_LORA, SS_PIN_LORA);
    LoRa.setPins(SS_PIN_LORA, RESET_PIN_LORA, LORA_DEFAULT_DIO0_PIN);
     
    if (!LoRa.begin(BAND)) 
    {
        Serial.println("[LoRa Sender] Comunicacao com o radio LoRa falhou. Nova tentativa em 1 segundo...");        
        delay(1000);
        status_init = false;
    }
    else
    {
        /* Configura o ganho do receptor LoRa para 20dBm, o maior ganho possível (visando maior alcance possível) */
        LoRa.setTxPower(HIGH_GAIN_LORA); 
        Serial.println("[LoRa Sender] Comunicacao com o radio LoRa ok");
        status_init = true;
    }
 
    return status_init;
}