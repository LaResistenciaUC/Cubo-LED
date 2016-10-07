#include <Wire.h>

/* Cubo LED LR
 * 
 * Cubo LED de 8x8x8 con memoria de 512kb incorporada
 * 
 * Un cuadro tiene 512 bits, o 64 bytes
 * v0.0
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
unsigned long ifi = 0; //inter-frame interval

void latchWrite(byte data, byte addr)
{
  PORTD = (PORTD & B00011111) | addr<<5;          //Selector de latch
  PORTB = (PORTB & B11000000) | data & B00111111; //Tomo los bytes inferiores
  PORTC = (PORTC & B11111100) | data>>6;          //Bytes superiores
}

void frameWrite(byte frame[], long layerDelay)
{
  //Vamos pasando por capas
  byte latchDelay = 1;
  for(byte layer = 0; layer < 8; layer++)
  {
    byte lSelect = layOrder[layer];  //Selector de capa
    PORTD = (PORTD & B11000111) | lSelect<<3;
    byte addr = 1;
    PORTD = (PORTD & B00011111);          //Selector de latch, se pone en 0 al inicio
    for(int pointer = 0; pointer < 512; pointer++, addr++) //Selector de dato
    {
      byte data = frame[pointer];
      
      PORTB = (PORTB & B11000000) | data & B00111111; //Carga data
      PORTC = (PORTC & B11111100) | data>>6;          
      delayMicroseconds(latchDelay);
      PORTD = (PORTD & B00011111) | addr<<5;          //Preparamos el siguiente latch y cargamos el dato del actual
      byte ender = data & B00111111 | data>>6;
      if(addr == 7)
      {
        addr = 0;
        PORTD = (PORTD & B00011111); //cargamos
        delayMicroseconds(layerDelay);
      }
    }
  }
}

byte mode = 0;
void setup()
{
  pinMode(A2,OUTPUT);
  digitalWrite(A2,HIGH);
  ifi = floor(1000000/fps); //Microsegundos entre cuadros
  Serial.begin(115200);
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
void loop() 
{
  byte frame[64];
  while(mode==0)
  {
    /*
     * El gran dilema del modo "Stream", es si confiar en que el programa host mantenga un timing de fps correcto, o manejarlo directamente via hardware
     * Al manejarlo vía hardware, podrían haber discrepancias de timing, en particular porque 60 fps no divide exacto el segundo en partes enteras, haciendo complicado manejar via delays, incluso NOPs
     */
    if(Serial.available()>=63) //recibo cuadro
    {
      Serial.readBytes(frame, 64);
    }
    frameWrite(frame,12); //Escribo cuadro... Esta rutina los escribe lo mas rápido posible
  }
}
