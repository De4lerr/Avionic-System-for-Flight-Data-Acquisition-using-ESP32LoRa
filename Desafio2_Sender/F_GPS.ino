/***************************************************************************************************/

/* Funções usadas para controle do GPS */

/***************************************************************************************************/

/* Chamada: read_GPS(); 
 * Funcao: Lê os dados do GPS; 
 * Parametros: nenhum; 
 * Retorno: string no fomato "HORA,MINUTO,SEGUNDO,DIA,MÊS,ANO,LATIDUTE,LONGITUDE"
*/
char * read_GPS(void)
{
  int dia, mes, ano = 0;
  int hora, min, seg = 0;
  char *lat, *lng = "0.0";
  char *hora_S, *min_S, *seg_S, *dia_S, *mes_S, *ano_S = "00";
  
  //Localização
  if (gps.location.isValid()){
    dtostrf(gps.location.lat(),3,6,lat);
    dtostrf(gps.location.lng(),3,6,lng);
  }

  //Data
  if (gps.date.isValid())
  {
    dia=gps.date.day();
    mes=gps.date.month();
    ano=gps.date.year();
  }

  //Tempo
  if (gps.time.isValid())
  {
    hora=gps.time.hour()-3; //Ajuste de fuso-horário
    min=gps.time.minute();
    seg=gps.time.second();
  }
  
  //Torna os valores medidos strings em buffers de tamanho específico
  itoa(hora,hora_S,2);
  itoa(min,min_S,2);
  itoa(seg,seg_S,2);
  itoa(dia,dia_S,2);
  itoa(mes,mes_S,2);
  itoa(ano,ano_S,4);

  char *dados_GPS;
  sprintf(dados_GPS,"%s,%s,%s,%s,%s,%s,%s,%s",hora_S,min_S,seg_S,dia_S,mes_S,ano_S,lat,lng);
  return dados_GPS;
}
