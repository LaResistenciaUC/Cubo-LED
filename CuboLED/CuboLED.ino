#include <Wire.h>
const byte eeprom1addr = 0x50;
const byte eeprom2addr = 0x51;

const byte layOrder[] = {0,1,2,3,4,5,6,7};
const byte latchOrder[] = {0,1,2,3,4,5,6,7};

byte frameBuffer[8][64]; //[rows][cols]  . Llamado  x[col][row]
byte bufferPointer;
const int fps = 30;
const float delayPerLayer = (înt)1000000/(fps*8);
unsigned long latchDelay = 1;

// Estado incial del cubo
byte frame[8][8] = {
    {255,  255,  15,  15,  15,  15,  255,  255}, 
    {0,  0,  0,  0,  0,  0,  0,  0}, 
    {0,  0,  0,  0,  0,  0,  0,  0},   
    {0,  0,  0,  0,  0,  0,  0,  0},   
    {0,  0,  0,  0,  0,  0,  0,  0},   
    {0,  0,  0,  0,  0,  0,  0,  0},   
    {0,  0,  0,  0,  0,  0,  0,  0},  
    {0,  0,  0,  0,  0,  0,  0,  0} 
  };
 
void frameWrite(byte* local_frame){
  //Vamos pasando por capas
  for(byte layer = 0; layer < 8; layer++)
  {
    byte dataArr[] = {1,2,4,8,16,32,64,128};
    byte lSelect = layOrder[layer];  //Selector de capa
    // Los OR ajustan los datos de entrada a las posiciones borradas con 0's  
    PORTD = (PORTD & B11100011) | lSelect << 2;
    //B 8 7 6 5 4 3 2 1 0
    byte addr = 0;
    PORTD = (PORTD & B00011111);          //Selector de latch, se pone en 0 al inicio
    for(int addr = 0; addr<8;addr++)
    {
      byte zero = 0;
      PORTB = (PORTB & B11000000) |  (zero & B00111111); //Carga data
      PORTC = (PORTC & B11111100) | ((zero & B11000000)>>6);   //Carga data
      PORTD = (PORTD & B00011111) | addr<<5; //Preparamos el siguiente latch y cargamos el dato del actual
    }
    PORTD = (PORTD & B00011111); 
    for(int pointer = 1; pointer < 9; pointer++)
    {
      if(pointer == 9){ pointer = 0 }
      byte data = local_frame[lSelect][pointer-1];
      PORTB = ((PORTB & B11000000) |  (data & B00111111)); //Carga data
      PORTC = (PORTC & B11111100) | ((data & B11000000)>>6);   //Carga data
      PORTD = (PORTD & B00011111) | pointer<<5; //Preparamos el siguiente latch y cargamos el dato del actual
      delayMicroseconds(16);
    }
    delayMicroseconds(delayPerLayer);
  }
}

byte mode = 0;
void setup(){
  pinMode(A2,OUTPUT);
  digitalWrite(A2,HIGH);
  Serial.begin(250000);
  delay(1000);
  mode = 0;
  if(mode == 2)
  {
    Serial.end();
  }
  DDRB |= B00111111; //Seteo de pines
  DDRC |= B00001111;
  DDRD |= B11111100;
}

// Testing samples
const byte pttrnA[] = { B11100000
                   ,B11100000
                   ,B11100000
                   ,B11100000
                   ,B11100000
                   ,B11100000
                   ,B11111111
                   ,B11111111};

const byte pttrnB[] = { B11111111
                  ,B11000011  
                  ,B11000011
                  ,B11111111
                  ,B11111000
                  ,B11001100
                  ,B11000110
                  ,B11000011};

void loop() {                //linea
  
}

void test(){
  unsigned long tprev = 0;
  byte it = 0;
  if(millis() - tprev > 1000)
  {
    byte next = it+1;
    if(next == 8){ next = 0 }
    for(int i = 0; i < 8; i++)
    {
      if(it%2) {frame[next][i] = pttrnA[i]}
      else {frame[next][i] = pttrnB[i]}
    }
    for(int i = 0; i < 8; i++)
    {
      frame[it][i] = 0;
    }
    tprev = millis();
    it++;
    if(it == 8) it = 0;
  }
  delay(5);
  frameWrite(frame); //Escribo cuadro... Esta rutina los escribe lo mas rápido posible
  }
}
