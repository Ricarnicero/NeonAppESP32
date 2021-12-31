#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;


int claxon = GPIO_NUM_9;

int accesorios = GPIO_NUM_0;
int ign1 = GPIO_NUM_1;
int ign2 = GPIO_NUM_2;
int start = GPIO_NUM_4;


int altas = GPIO_NUM_5;
int faros = GPIO_NUM_6;
int qDer = GPIO_NUM_7;
int qIzq = GPIO_NUM_8;
int segurosOff = GPIO_NUM_19;
int segurosOn = GPIO_NUM_18;

int cajuela = GPIO_NUM_10;

int pins[19] = {accesorios,ign1,ign2,start,altas,faros,qDer,qIzq};

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define PRENDER LOW
#define APAGAR HIGH

void ClaxonLuces(int numVeces){
  digitalWrite(faros,PRENDER);
  for(int i = 0; i < numVeces ; i++){    
    digitalWrite(claxon,PRENDER);
    delay(100);    
    digitalWrite(claxon,APAGAR);
    delay(50);
  }
  digitalWrite(faros,APAGAR);
}

void ClaxonLuces(int numVeces,int customDelay){
  digitalWrite(faros,PRENDER);
  for(int i = 0; i < numVeces ; i++){    
    digitalWrite(claxon,PRENDER);
    delay(customDelay);    
    digitalWrite(claxon,APAGAR);
    delay(customDelay-50);
  }
  digitalWrite(faros,APAGAR);
}

void activarAlarma(){
  ClaxonLuces(2);
  digitalWrite(segurosOn,PRENDER);
  delay(300);
  digitalWrite(segurosOn,APAGAR);
}

void desactivarAlarma(){
  ClaxonLuces(1);
  digitalWrite(segurosOff,PRENDER);
  delay(300);
  digitalWrite(segurosOff,APAGAR);
}
void ponerSeguros(){
  digitalWrite(segurosOn,PRENDER);
  delay(300);
  digitalWrite(segurosOn,APAGAR);
}

void quitarSeguros(){
  digitalWrite(segurosOff,PRENDER);
  delay(300);
  digitalWrite(segurosOff,APAGAR);
}

void secuenciaDeArranque(){
  digitalWrite(accesorios ,PRENDER);
  digitalWrite(ign1 ,PRENDER);
  digitalWrite(ign2 ,PRENDER);
  delay(100);
  digitalWrite(start ,PRENDER);
  delay(1100);
  digitalWrite(start ,APAGAR);
  quitarSeguros();
}

void secuenciaDeArranqueChida(){
  quitarSeguros();
  digitalWrite(qDer,PRENDER);
  delay(1500);
  digitalWrite(qIzq,PRENDER);
  delay(1500);
  digitalWrite(altas,PRENDER);
  delay(2500);
  digitalWrite(altas,APAGAR);
  digitalWrite(faros ,PRENDER);
  delay(700);
  digitalWrite(altas,PRENDER);
  delay(1500);
  digitalWrite(altas,APAGAR);
  delay(700);
  digitalWrite(altas,PRENDER);
  digitalWrite(faros ,APAGAR);
  delay(600);
  digitalWrite(altas,APAGAR);
  delay(800);
  digitalWrite(qIzq,APAGAR);
  delay(200);
  digitalWrite(qDer,APAGAR);
  delay(200);
  
  digitalWrite(accesorios ,PRENDER);
  digitalWrite(ign1 ,PRENDER);
  digitalWrite(ign2 ,PRENDER);
  delay(100);
  digitalWrite(start ,PRENDER);
  delay(1100);
  digitalWrite(start ,APAGAR);
}

void secuenciaDeApagado(){
  digitalWrite(ign2 ,APAGAR);
  digitalWrite(ign1 ,APAGAR);
  digitalWrite(accesorios ,APAGAR);
  quitarSeguros();
}

void abrirCajuela(){
   digitalWrite(cajuela ,PRENDER);
  delay(300);
  digitalWrite(cajuela ,APAGAR);
}

void apagarDejarAcc(){
  digitalWrite(ign1 ,APAGAR);
  digitalWrite(ign2 ,APAGAR);
  quitarSeguros();
}

void onConnected(){
  digitalWrite(qDer,PRENDER);digitalWrite(qIzq,PRENDER);
  delay(300);
  digitalWrite(qDer,APAGAR);digitalWrite(qIzq,APAGAR);
  delay(500);
  digitalWrite(qDer,PRENDER);digitalWrite(qIzq,PRENDER);
  delay(300);
  digitalWrite(qDer,APAGAR);digitalWrite(qIzq,APAGAR);
}

void onDisconnected(){
  digitalWrite(qDer,PRENDER);digitalWrite(qIzq,PRENDER);
  delay(300);
  digitalWrite(qDer,APAGAR);digitalWrite(qIzq,APAGAR);
  
  ponerSeguros();
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      onConnected();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      onDisconnected();
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        /* 
        Serial.println("*********");
        Serial.println("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++){
          Serial.print(rxValue[i]);
          Serial.print(" -> ");
          int ascii = rxValue[i];
          Serial.print(ascii);
          Serial.print(" | ");
        }        
        Serial.println();
        Serial.println("*********");        
        */
        int primerByte = rxValue[0];
        
        if (rxValue.length()==4 && primerByte == 200){
          int numPin = std::atoi(rxValue.substr(1,2).c_str());
          int accion =  std::atoi(rxValue.substr(3,3).c_str());
          Serial.println(numPin);
          Serial.println(accion);
          switch(accion){
            case 0:
              digitalWrite(pins[numPin-1],APAGAR);
              break;
            case 1:
              digitalWrite(pins[numPin-1],PRENDER);
              break;
            case 2:
              secuenciaDeArranque();
              break;
            case 3:
              secuenciaDeApagado();
              break;
            case 4:
              secuenciaDeArranqueChida();
              break;
            case 5:
              activarAlarma();
              break;
            case 6:
              desactivarAlarma();
              break;
            case 7:
              abrirCajuela();
              break;
            case 8:
              apagarDejarAcc();
              break; 
          }
        }
      }
    }
};

void initPins(){
  pinMode(claxon, OUTPUT);
  digitalWrite(claxon,APAGAR);
  
  pinMode(accesorios, OUTPUT);
  digitalWrite(accesorios,APAGAR);
  
  pinMode(ign1, OUTPUT);
  digitalWrite(ign1,APAGAR);
  
  pinMode(ign2, OUTPUT);
  digitalWrite(ign2,APAGAR);
  
  pinMode(start, OUTPUT);
  digitalWrite(start,APAGAR);
  
  pinMode(altas, OUTPUT);
  digitalWrite(altas,APAGAR);
  
  pinMode(faros, OUTPUT);
  digitalWrite(faros,APAGAR);
  
  pinMode(qDer, OUTPUT);
  digitalWrite(qDer,APAGAR);
  
  pinMode(qIzq, OUTPUT);
  digitalWrite(qIzq,APAGAR);
  
  pinMode(segurosOff, OUTPUT);
  digitalWrite(segurosOff,APAGAR);
  
  pinMode(segurosOn, OUTPUT);
  digitalWrite(segurosOn,APAGAR);
  
  pinMode(cajuela, OUTPUT);
  digitalWrite(cajuela,APAGAR);
}

void setup() {
  Serial.begin(115200);
  
  initPins();
  
    
  // Create the BLE Device
  BLEDevice::init("ESP32test");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID_TX,
                        BLECharacteristic::PROPERTY_NOTIFY
                      );

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE
                                          );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {

  /*if (deviceConnected) {
    pTxCharacteristic->setValue(&txValue, 1);
    pTxCharacteristic->notify();
    txValue++;
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }*/

  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }
}
