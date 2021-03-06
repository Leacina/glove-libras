
// Include Libraries
#include "Arduino.h"
#include "Flex.h"
#include "MPU6050.h"
#include "Wire.h"
#include "I2Cdev.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Pin Definitions
#define FLEX_PIN_POLEGAR   12
#define FLEX_PIN_INDICADOR 13
#define FLEX_PIN_MEIO      14
#define FLEX_PIN_ANELAR    25
#define FLEX_PIN_MENOR     26

// Global variables and defines
int16_t mpu6050Ax, mpu6050Ay, mpu6050Az;
int16_t mpu6050Gx, mpu6050Gy, mpu6050Gz;

//POLEGAR
int sensorMinPolegar = 500;
int sensorMaxPolegar = 0;

//INDICADOR
int sensorMinIndicador = 1450; 
int sensorMaxIndicador = 0;

//MEIO
int sensorMinMeio = 1450; 
int sensorMaxMeio = 0;

//ANELAR
int sensorMinAnelar = 1450; 
int sensorMaxAnelar = 0;

//MENOR
int sensorMinMenor = 500; 
int sensorMaxMenor = 0;

// object initialization
Flex FLEX_POLEGAR(FLEX_PIN_POLEGAR);
Flex FLEX_INDICADOR(FLEX_PIN_INDICADOR);
Flex FLEX_MEIO(FLEX_PIN_MEIO);
Flex FLEX_ANELAR(FLEX_PIN_ANELAR);
Flex FLEX_MENOR(FLEX_PIN_MENOR);

Adafruit_MPU6050 mpu;

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;

//Variaveis para identificar as letras
int count = 0;
char letraAnt = ' ';

String command = "";

bool isRecognizeLetter = true;

//CONFIGURAÇÔES INICIAL
void setup() 
{
    // Setup Serial which is useful for debugging
    // Use the Serial Monitor to view printed messages
    Serial.begin(9600);
    while (!Serial) ; // wait for serial port to connect. Needed for native USB
    Serial.println("start");

    BLEDevice::init("GloveLibra");
    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                                           CHARACTERISTIC_UUID,
                                           BLECharacteristic::PROPERTY_READ |
                                           BLECharacteristic::PROPERTY_WRITE |
                                           BLECharacteristic::PROPERTY_NOTIFY
                                         );
                                         
    pCharacteristic->setValue("Iniciando aplicação!");
    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pCharacteristic->notify();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
  
    if (!mpu.begin()) {
       Serial.println("Failed to find MPU6050 chip");
       while (1) {
         delay(10);
       }
    }
    
    Serial.println("MPU6050 Found!");
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

    float sensorPolegar = FLEX_POLEGAR.read();
    float sensorIndicador = FLEX_INDICADOR.read();
    float sensorMeio = FLEX_MEIO.read();
    float sensorAnelar = FLEX_ANELAR.read();
    float sensorMenor = FLEX_MENOR.read();
    
    while(millis()<15000)
    {
    
      if(analogRead(26) > 0)
      {
        //CALIBRA OS DADOS CONFORME O SENSOR
        //POLEGAR
        if(sensorPolegar<sensorMinPolegar)
        {
          sensorMinPolegar=sensorPolegar;
        }
        if(sensorPolegar>sensorMaxPolegar)
        {
          sensorMaxPolegar=sensorPolegar;
        }
        //INDICADOR
        if(sensorIndicador<sensorMinIndicador)
        {
          sensorMinIndicador=sensorIndicador;
        }
        if(sensorIndicador>sensorMaxIndicador)
        {
          sensorMaxIndicador=sensorIndicador;
        }
        //MEIO
        if(sensorMeio<sensorMinMeio)
        {
          sensorMinMeio=sensorMeio;
        }
        if(sensorMeio>sensorMaxMeio)
        {
          sensorMaxMeio=sensorMeio;
        }
        //ANELAR
        if(sensorAnelar<sensorMinAnelar)
        {
          sensorMinAnelar=sensorAnelar;
        }
        if(sensorAnelar>sensorMaxAnelar)
        {
          sensorMaxAnelar=sensorAnelar;
        }
        //MENOR
        if(sensorMenor<sensorMinMenor)
        {
          sensorMinMenor=sensorMenor;
        }
        if(sensorMenor>sensorMaxMenor)
        {
          sensorMaxMenor=sensorMenor;
        }
      }
    }
    
}

//Verifica se tem alguma chamada para identificar a letra
void loop() 
{    
    //Sensor dedo polegar
    float sensorPolegar = FLEX_POLEGAR.read();
    sensorPolegar = constrain(sensorPolegar,sensorMinPolegar, sensorMaxPolegar);
    float anguloPolegar= map(sensorPolegar, sensorMinPolegar, sensorMaxPolegar, 0, 90);

    //Sensor dedo indicador   
    float sensorIndicador = FLEX_INDICADOR.read();
    sensorIndicador = constrain(sensorIndicador,sensorMinIndicador, sensorMaxIndicador);
    float anguloIndicador= map(sensorIndicador, sensorMinIndicador, sensorMaxIndicador, 0, 90);

    //Sensor dedo meio   
    float sensorMeio = FLEX_MEIO.read();
    sensorMeio = constrain(sensorMeio,sensorMinMeio, sensorMaxMeio);
    float anguloMeio= map(sensorMeio, sensorMinMeio, sensorMaxMeio, 0, 90);

    //Sensor dedo anelar   
    float sensorAnelar = FLEX_ANELAR.read();
    sensorAnelar= constrain(sensorAnelar,sensorMinAnelar, sensorMaxAnelar);
    float anguloAnelar= map(sensorAnelar, sensorMinAnelar, sensorMaxAnelar, 0, 90);

    //Sensor dedo menor   
    float sensorMenor = FLEX_MENOR.read();
    sensorMenor = constrain(sensorMenor,sensorMinMenor, sensorMaxMenor);
    float anguloMenor= map(sensorMenor, sensorMinMenor, sensorMaxMenor, 0, 90);

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);  
    
    std::string valueApp = pCharacteristic->getValue();
  
    if(valueApp == "recognizeLetter"){
      isRecognizeLetter= true;
      Serial.println("Entrou");
    }
    
    if(valueApp == "recognizeNumber"){
      isRecognizeLetter= false;
      Serial.println("Entrssou");
    }

    char recognizeGestual = ' ';

    //Se for reconhecimento de letras
    if(isRecognizeLetter){
      recognizeGestual = getLetter(anguloPolegar, anguloIndicador, anguloMeio, anguloAnelar, anguloMenor, a, touchRead(T0), touchRead(T3));
      //Imprime os valores
      Serial.print(F("LETTER: ")); Serial.println(String(anguloPolegar) + "  " + String(anguloIndicador) + "  " + 
                                                         String(anguloMeio) + "  " + String(anguloAnelar) + "  " + String(anguloMenor) + "  " + a.acceleration.y + "  " + "  " + a.acceleration.x + "  " +
                                                         (recognizeGestual));
    } else {
      recognizeGestual = getNumber(anguloPolegar, anguloIndicador, anguloMeio, anguloAnelar, anguloMenor, a, touchRead(T0), touchRead(T3));
      //Imprime os valores
      Serial.print(F("NUMBER: ")); Serial.println(String(anguloPolegar) + "  " + String(anguloIndicador) + "  " + 
                                                        String(anguloMeio) + "  " + String(anguloAnelar) + "  " + String(anguloMenor) + "  " + a.acceleration.y + "  " +
                                                        (recognizeGestual));
    }

    //SE NÂO ESTIVER RECONHECENDO NADA IGNORA
    if(recognizeGestual != ' '){
      if(recognizeGestual == letraAnt){
        count++;
      }else{
        count = 0;
      };
  
      //SE O GESTO ESTA SENDO COMPREENDIDO NUMA SEQUENCIA OU SE FOR UMA LETRA DE MOVIMENTO, NOTIFICA O APLICATIVO
      if((count == 4) || (recognizeGestual == 'J') || ((recognizeGestual == 'K')) || (recognizeGestual == 'Z') || ((recognizeGestual == 'R') && (count == 2))){
        Serial.println(recognizeGestual);
        count = 0;
         
        std::string stringValue((char*)&recognizeGestual, 2);
        pCharacteristic->setValue(stringValue);
        pCharacteristic->notify();
        //Notifica e não envia mais a letra
        pCharacteristic->setValue(" ");
      }
  
      letraAnt = recognizeGestual;
    }
                                         
    delay(200);    
}

/**
 * Verifica os valores dos angulos para capturar as letras
 */
char getLetter(float POLEGAR, float INDICADOR, float MEIO, float ANELAR, float MENOR, sensors_event_t MPU, float toqueIndPol, float toqueIndMeio) {

  if((POLEGAR >= 85) && (INDICADOR >= 50 && INDICADOR <= 65) && (MEIO >= 60 && MEIO <= 70) && (ANELAR >= 60 && ANELAR <= 70) && (MENOR >= 65 && MENOR <= 80)){
    return 'A';
  }else if(((POLEGAR <= 80 && POLEGAR >= 65) && (INDICADOR >= 88) && (MEIO >= 88) && (ANELAR >= 88) && (MENOR >= 88))){
    return 'B';
  }else if(((POLEGAR >= 85) && (INDICADOR >= 75 && INDICADOR <= 85) && (MEIO >= 75 && MEIO <= 85) && (ANELAR >= 75 && ANELAR <= 85) && (MENOR >= 80 && MENOR <= 88))){
    return 'C';
  }else if(((POLEGAR >= 80 && POLEGAR <= 85) && (INDICADOR >= 88 && INDICADOR <= 90) && (MEIO >= 65 && MEIO <= 75) && (ANELAR >= 65 && ANELAR <= 75) && (MENOR >= 75 && MENOR <= 85))){
    return 'D';
  }else if(((POLEGAR >= 70 && POLEGAR <= 80) && (INDICADOR >= 50 && INDICADOR <= 64) && (MEIO >= 60 && MEIO <= 70) && (ANELAR >= 60 && ANELAR <= 70) && (MENOR >= 70 && MENOR <= 80))){
    return 'E';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 83 && INDICADOR <= 88) && (MEIO >= 88) && (ANELAR >= 88) && (MENOR >= 88) && (toqueIndPol <= 30))){
    return 'F';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 88) && (MEIO >= 55 && MEIO <= 68) && (ANELAR >= 55 && ANELAR <= 68) && (MENOR >= 70 && MENOR <= 80) && (toqueIndPol <= 30) && (MPU.acceleration.x >= 0))){ //PRECISA ENCOSTAR OS SENSORES INDICADOR E POLEGAR
    return 'G';
  }else if((((POLEGAR >= 88) && (INDICADOR >= 88) && (MEIO >= 88) && (ANELAR >= 60 && ANELAR <= 70) && (MENOR >= 70 && MENOR <= 80)) && (MPU.acceleration.y <= 4))){ //VERIFICAR SENSORES
    return 'H';
  }else if(((POLEGAR >= 70 && POLEGAR <= 80) && (INDICADOR >= 50 && INDICADOR <= 70) && (MEIO >= 60 && MEIO <= 70) && (ANELAR >= 60 && ANELAR <= 80) && (MENOR >= 88) && (MPU.acceleration.y <= 4))){
    return 'I';
  }else if(((POLEGAR >= 70 && POLEGAR <= 80) && (INDICADOR >= 50 && INDICADOR <= 70) && (MEIO >= 60 && MEIO <= 70) && (ANELAR >= 70 && ANELAR <= 80) && (MENOR >= 88) && ((MPU.acceleration.y >= 7)))){ //VERIFICAR OUTROS SENSORES
    return 'J';
  }else if((((POLEGAR >= 85) && (INDICADOR >= 85) && (MEIO >= 85) && (ANELAR >= 60 && ANELAR <= 70) && (MENOR >= 70 && MENOR <= 80)) && ((MPU.acceleration.y <= 7)) && ((MPU.acceleration.x >= 8)))){ //VERIFICAR SENSORES
    return 'K';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 88) && (MEIO >= 55 && MEIO <= 70) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR >= 70 && MENOR <= 80) && (toqueIndPol >= 30))){ //toquee entre polegar e indicador n te
    return 'L';
  }else if(((POLEGAR >= 80 && POLEGAR <= 87) && (INDICADOR >= 87) && (MEIO >= 87) && (ANELAR >= 87) && (MENOR <= 87)) && (toqueIndMeio <= 30) && (MPU.acceleration.x <= 0)){
    return 'M';
  }else if(((POLEGAR >= 80) && (INDICADOR >= 87) && (MEIO >= 87) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR <= 87)) && (toqueIndMeio <= 30) && (MPU.acceleration.x <= 0)){
    return 'N';
  }else if(((POLEGAR <= 80) && (INDICADOR >= 60 && INDICADOR <= 80) && (MEIO >= 70 && MEIO <= 80) && (ANELAR >= 70 && ANELAR <= 80) && (MENOR >= 75 && MENOR <= 87))){
    return 'O';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 88) && (MEIO >= 88) && (ANELAR >= 60 && ANELAR <= 70) && (MENOR >= 70 && MENOR <= 80)) && ((MPU.acceleration.y >= 8 && MPU.acceleration.y <= 10)) && ((MPU.acceleration.x <= 2.30))){ //VERICAR SENSORES
    return 'P';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 88) && (MEIO >= 65 && MEIO <= 75) && (ANELAR >= 60 && ANELAR <= 70) && (MENOR >= 75 && MENOR <= 85) && (toqueIndPol <= 30) && (MPU.acceleration.x <= 0))){//mesma que o G virado para baixo
    return 'Q';
  }else if(((POLEGAR <= 75) && (INDICADOR <= 87) && (MEIO >= 85) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR <= 85))){ //VERIFICAR SENSORES
    return 'R';
  }else if((POLEGAR <= 70) && (INDICADOR >= 50 && INDICADOR <= 65) && (MEIO >= 60 && MEIO <= 70) && (ANELAR >= 60 && ANELAR <= 70) && (MENOR >= 65 && MENOR <= 80)){
    return 'S';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 75 && INDICADOR <= 85) && (MEIO >= 88) && (ANELAR >= 88) && (MENOR >= 88))){
    return 'T';
  }else if(((POLEGAR <= 80) && (INDICADOR >= 88) && (MEIO >= 87) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR <= 87)) && (toqueIndMeio <= 30) && (MPU.acceleration.y <= 4)){
    return 'U';
  }else if(((POLEGAR <= 80) && (INDICADOR >= 89) && (MEIO >= 87) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR <= 87)) && (MPU.acceleration.y <= 4)){ //MESMO QUE "U" MAIS ABERTO
    return 'V';
  }else if(((POLEGAR >= 70 && POLEGAR <= 80) && (INDICADOR >= 70 && INDICADOR <= 80) && (MEIO >= 55 && MEIO <= 70) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR <= 80))){
    return 'X';
  }else if((((POLEGAR >= 80 && POLEGAR <= 87) && (INDICADOR >= 87) && (MEIO >= 87) && (ANELAR >= 87) && (MENOR <= 87)) && (toqueIndMeio >= 35) && (MPU.acceleration.y <= 4))){
    return 'W';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 60 && INDICADOR <= 70) && (MEIO >= 65 && MEIO <= 75) && (ANELAR >= 60 && ANELAR <= 80) && (MENOR >= 88))){
    return 'Y';
  }else if(((POLEGAR >= 70 && POLEGAR <= 83) && (INDICADOR >= 88) && (MEIO >= 55 && MEIO <= 70) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR <= 80) && (MPU.acceleration.x <= 4))){ //VERIFICAR SENSORES
    return 'Z';
  }
  
  return ' ';
}

/**
 * Verifica os valores dos angulos para capturar os números
 */
char getNumber(float POLEGAR, float INDICADOR, float MEIO, float ANELAR, float MENOR, sensors_event_t MPU, float toqueIndPol, float toqueIndMeio) {
if(((POLEGAR >= 75 && POLEGAR <= 85) && (INDICADOR >= 70 && INDICADOR <= 80) && (MEIO >= 70 && MEIO <= 80) && (ANELAR >= 70 && ANELAR <= 80) && (MENOR >= 75 && MENOR <= 87))){
    return '0';
  }else if(((POLEGAR >= 70 && POLEGAR <= 83) && (INDICADOR >= 88) && (MEIO >= 55 && MEIO <= 70) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR <= 80))){
    return '1';
  }else if(((POLEGAR <= 80) && (INDICADOR >= 89) && (MEIO >= 87) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR <= 87)) && (toqueIndPol >= 35)){
    return '2';
  }else if((((POLEGAR >= 80 && POLEGAR <= 87) && (INDICADOR >= 87) && (MEIO >= 87) && (ANELAR >= 87) && (MENOR <= 87)) && (toqueIndMeio >= 35) && (MPU.acceleration.y <= 4))){
    return '3';
  }else if(((POLEGAR <= 80 && POLEGAR >= 65) && (INDICADOR >= 88) && (MEIO >= 88) && (ANELAR >= 88) && (MENOR >= 88)) && (toqueIndMeio >= 35)){
    return '4';
  }else if(((POLEGAR >= 75 && POLEGAR <= 85) && (INDICADOR >= 65 && INDICADOR <= 80) && (MEIO >= 65 && MEIO <= 80) && (ANELAR >= 55 && ANELAR <= 70) && (MENOR >= 65 && MENOR <= 75))){
    return '5';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 55 && INDICADOR <= 70) && (MEIO >= 60 && MEIO <= 75) && (ANELAR >= 60 && ANELAR <= 75) && (MENOR >= 78 && MENOR <= 85) && (MPU.acceleration.y >= 2 && MPU.acceleration.y <= 4))){ 
    return '6';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 88) && (MEIO >= 60 && MEIO <= 70) && (ANELAR >= 60 && ANELAR <= 70) && (MENOR >= 70 && MENOR <= 80)) && (toqueIndPol <= 35)){ 
    return '7';
  }else if(((POLEGAR >= 70 && POLEGAR <= 83) && (INDICADOR >= 55 && INDICADOR <= 70) && (MEIO >= 60 && MEIO <= 70) && (ANELAR >= 60 && ANELAR <= 70) && (MENOR >= 68 && MENOR <= 85))){
    return '8';
  }else if(((POLEGAR >= 88) && (INDICADOR >= 55 && INDICADOR <= 70) && (MEIO >= 60 && MEIO <= 75) && (ANELAR >= 60 && ANELAR <= 75) && (MENOR >= 78 && MENOR <= 85) && (MPU.acceleration.y >= 6))){ 
    return '9';
  };

  return ' '; 
}
