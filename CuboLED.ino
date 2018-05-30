#include <Wire.h>

/* Cubo LED LR
 * 
 * 
 * Cubo LED de 8x8x8 con memoria de 512kb incorporada
 * 
 * Un cuadro tiene 512 bits, o 64 bytes
 * v0.7
 * 
 * 
 * Conexiones
 * PORT # ARDUPIN ASSIG 
 * B  0 8   DATA BUS 0 - 5
 * B  1 9   "
 * B  2 10  "
 * B  3 11  "
 * B  4 12  "
 * B  5 13  "
 * B  6 XTAL
 * B  7 XTAL
 * 
 * C  0 14/A0 DATA BUS 6 
 * C  1 15/A1 DATA BUS 7
 * C  2 16/A2 STATUS
 * C  3 17/A3 OE Latch
 * C  4 18/A4 SDA
 * C  5 19/A5 SCL
 * 
 * D  0 0 Serial RX
 * D  1 1 Serial TX
 * D  2 2 LAYERSWITCH 0 - 2
 * D  3 3 "
 * D  4 4 "
 * D  5 5 LATCHSWITCH 0 - 2 
 * D  6 6 "
 * D  7 7 "
 */
const byte eeprom1addr = 0x50;
const byte eeprom2addr = 0x51;

const byte layOrder[] = {0,1,2,3,4,5,6,7};
const byte latchOrder[] = {0,1,2,3,4,5,6,7};

byte frameBuffer[8][64]; //[rows][cols]  . Llamado  x[col][row]
byte bufferPointer;
int fps = 60;
unsigned long latchDelay = 1;

byte frame[8][8] = {  {255,  255,  15,  15,  15,  15,  255,  255}, 
                               {0,  0,  0,  0,  0,  0,  0,  0}, 
                               {0,  0,  0,  0,  0,  0,  0,  0},   
                               {0,  0,  0,  0,  0,  0,  0,  0},   
                               {0,  0,  0,  0,  0,  0,  0,  0},   
                               {0,  0,  0,  0,  0,  0,  0,  0},   
                               {0,  0,  0,  0,  0,  0,  0,  0},  
                               {0,  0,  0,  0,  0,  0,  0,  0} };
void frameWrite(unsigned long layerDelay)
{
  //Vamos pasando por capas
  for(byte layer = 0; layer < 8; layer++)
  {
    byte dataArr[] = {1,2,4,8,16,32,64,128};
    byte lSelect = layOrder[layer];  //Selector de capa
    PORTD = (PORTD & B11100011) | lSelect << 2;
//    Serial.println("layerdata");
//    Serial.println(lSelect,BIN);
//    Serial.println(lSelect << 2,BIN);
//    Serial.println(PORTD,BIN);
//    B 8 7 6 5 4 3 2 1 0
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
    Serial.println("++++++++++++++++++++++ \n LAYER");
    for(int pointer = 1; pointer < 9; pointer++)
    //for(int pointer = 8*layer; pointer < (8*layer)+8; pointer++) //Selector de dato
    {
      if(pointer == 9) pointer = 0;
      byte data = frame[lSelect][pointer-1];
      //Serial.println("-");
      //Serial.println(lSelect,BIN);
      //Serial.println(data,BIN);
      //Serial.println(PORTB,BIN);
      //Serial.println(PORTC,BIN);
      //Serial.println("+");
      PORTB = ((PORTB & B11000000) |  (data & B00111111)); //Carga data
      PORTC = (PORTC & B11111100) | ((data & B11000000)>>6);   //Carga data
      //Serial.println(PORTB,BIN);
      //Serial.println(PORTC,BIN);
      //Serial.println("-");
      PORTD = (PORTD & B00011111) | pointer<<5; //Preparamos el siguiente latch y cargamos el dato del actual
      delayMicroseconds(16);
      //Serial.print("Byte n ");
      //Serial.println(pointer);
      //Serial.println(compose);
      //Serial.println((data & B11000000)>>6);      
      //Serial.println(data);
      //Serial.println(PORTB,BIN);
      //Serial.println(PORTC,BIN);
      //Serial.println(PORTD,BIN);
      
    }
    delay(1);
  }
}

byte mode = 0;
void setup()
{
  pinMode(A2,OUTPUT);
  digitalWrite(A2,HIGH);
  Serial.begin(250000);
  Serial.println(F("START"));
  Serial.println(F("MODE?"));
  delay(1000);
  /*
  if(Serial.available())
  {
    byte mode = Serial.read()-'0'; //0 reproduce desde serial, 1 carga a eeprom, 2 lee desde eeprom
    if(mode > 2)
    {
      Serial.println(F("ERROR"));
      while(true) __asm__("nop");
    }
    else Serial.println(F("OK"));
  }
  else //correr desde memoria
  {
    mode = 2;
  }
  */
  mode = 0;
  if(mode == 2)
  {
    Serial.end();
  }
  DDRB |= B00111111; //Seteo de pines
  DDRC |= B00001111;
  DDRD |= B11111100;
  Serial.println(F("READY"));
}
long del = 10000;

void loop() 
{                //linea 

  /*byte frame[] = {
    255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255,
    255,255,255,255,255,255,255,255
  };*/
  /*  byte frame[] = { 0,255,255,255,255,255,255,255, 
                     255,255,255,255,255,255,255,255, 
                     0,  0,  0,  0,  0,  0,  0,  0,   
                     0,  0,  0,  0,  0,  0,  0,  0,   
                     0,  0,  0,  0,  0,  0,  0,  0,   
                     0,  0,  0,  0,  0,  0,  0,  0,   
                     255,255,255,255,255,255,255,255, 
                     255,255,255,255,255,255,255,255, };*/
  unsigned long tprev = 0;
  byte it = 0;
  byte pttrnA[] = { B11100000
                   ,B11100000
                   ,B11100000
                   ,B11100000
                   ,B11100000
                   ,B11100000
                   ,B11111111
                   ,B11111111};
  byte pttrnB[] = { B11111111
                   ,B11000011  
                   ,B11000011
                   ,B11111111
                   ,B11111000
                   ,B11001100
                   ,B11000110
                   ,B11000011};
  while(mode==0)
  {
    /*
     * El gran dilema del modo "Stream", es si confiar en que el programa host mantenga un timing de fps correcto, o manejarlo directamente via hardware
     * Al manejarlo vía hardware, podrían haber discrepancias de timing, en particular porque 60 fps no divide exacto el segundo en partes enteras, haciendo complicado manejar via delays, incluso NOPs
     * Provisionalmente se manejará en modo stream
     */
    if(millis() - tprev > 1000)
    {
      byte next = it+1;
      if(next == 8) next = 0;
      for(int i = 0; i < 8; i++)
      {
        if(it%2) frame[next][i] = pttrnA[i];
        else frame[next][i] = pttrnB[i];
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
    frameWrite(del); //Escribo cuadro... Esta rutina los escribe lo mas rápido posible
  }
}
