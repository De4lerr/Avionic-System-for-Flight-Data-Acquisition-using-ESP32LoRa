/***************************************************************************************************/

/* Funções usadas para controle do acelerômetro */

/***************************************************************************************************/

/* Chamada: config_MPU(); 
 * Funcao: Configura o acelerômetro; 
 * Parametros: nenhum; 
 * Retorno: nenhum; 
*/
void config_MPU(void)
{
    Wire.begin();                      // Initialize comunication
    Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
    Wire.write(0x6B);                  // Talk to the register 6B
    Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
    Wire.endTransmission(true);        //end the transmission    
  
    // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
    Wire.beginTransmission(MPU);
    Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
    Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
    Wire.endTransmission(true);
     
    // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
    Wire.beginTransmission(MPU);
    Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
    Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
    Wire.endTransmission(true);
    delay(20);
}

/***************************************************************************************************/

/* Chamada: calculate_IMU_error(); 
 * Funcao: Verifrica o erro relativo às leituras do acelerômetro e giroscópio; 
 * Parametros: nenhum; 
 * Retorno: array no fomato {AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ}; 
*/
float * calculate_IMU_error(void) 
{
  int MPUadd=MPU;
  float AccX, AccY, AccZ;
  float GyroX, GyroY, GyroZ;
  float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
  int   c = 0;
  
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPUadd);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPUadd, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 2048.0;
    AccY = (Wire.read() << 8 | Wire.read()) / 2048.0;
    AccZ = (Wire.read() << 8 | Wire.read()) / 2048.0;

    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPUadd);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPUadd, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 32.8);
    GyroErrorY = GyroErrorY + (GyroY / 32.8);
    GyroErrorZ = GyroErrorZ + (GyroZ / 32.8);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;

  float dados_Erro[5]={AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ};

  return dados_Erro;
}

/***************************************************************************************************/

/* Chamada: read_MPU(float argv[]); 
 * Funcao: lê os dados do acelerômetro e giroscópio; 
 * Parametros: array com os erros {AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ}; 
 * Retorno: string no fomato "ACELERAÇÃO X, ACELERAÇÃO Y, ACELERAÇÃO Z, GIRO X, GIRO Y, GIRO Z"; 
*/
char * read_MPU(float erro[])
{
  int MPUadd=MPU;
  float AccX, AccY, AccZ = 0.0;
  float GyroX, GyroY, GyroZ = 0.0;
  float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ = 0.0;
  float roll, pitch, yaw = 0.0;
  float elapsedTime, currentTime, previousTime;
  char *AccX_S, *AccY_S, *AccZ_S, *roll_S, *pitch_S, *yaw_S = "0.0";

  //Lendo os dados do acelerômetro
  Wire.beginTransmission(MPUadd);
  Wire.write(0x3B);                 //Começa com o registrador 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPUadd, 6, true);   //Lê 6 registradores no total, cada valor de cada eixo é guardado em 2 registradores

  //Para medições de +-16g, precisamos dividir o valor lido por 2048, de acordo com o datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 2048.0;
  AccY = (Wire.read() << 8 | Wire.read()) / 2048.0;
  AccZ = (Wire.read() << 8 | Wire.read()) / 2048.0;

  //Calculado Roll e Pitch
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) + erro[1];      
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + erro[2]; 

  //Lendo dados do giroscópio
  previousTime = currentTime;                        //Tempo passado anterior é armazenado antes que o novo comece
  currentTime = millis();                            //Leitura do tempo atual
  elapsedTime = (currentTime - previousTime) / 1000; // Dividindo tempo por 1000 para ser dado em segundos
  Wire.beginTransmission(MPUadd);
  Wire.write(0x43);                                  //Endereço do primeiro registrador de giros é 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPUadd, 6, true);                    // Lê 4 registradores no total, cada valor de cada eixo é guardado em 2 registradores

  //Para medições de 1000 deg/s, precisamos dividir o valor lido por 32.8, de acordo com o datasheet
  GyroX = (Wire.read() << 8 | Wire.read()) / 32.8; 
  GyroY = (Wire.read() << 8 | Wire.read()) / 32.8;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 32.8;

  // Correct the outputs with the calculated error values
  GyroX = GyroX + erro[3];
  GyroY = GyroY + erro[4];
  GyroZ = GyroZ + erro[5];

  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  
  //Passa os dados para strings em um buffer de tamanho específico
  dtostrf(AccX,3,5,AccX_S);
  dtostrf(AccY,3,5,AccY_S);
  dtostrf(AccZ,3,5,AccZ_S);
  dtostrf(roll,4,5,roll_S);
  dtostrf(pitch,4,5,pitch_S);
  dtostrf(yaw,4,5,yaw_S);

  char *dados_MPU;
  sprintf(dados_MPU,"%s,%s,%s,%s,%s\n",AccX_S,AccY_S,AccZ_S,roll_S,pitch_S,yaw_S);
  return dados_MPU;
}