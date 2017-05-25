#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_NeoPixel.h>

#define RST_PIN  9    //Pin 9 para el reset del RC522
#define SS_PIN  10   //Pin 10 para el SS (SDA) del RC522
#define RELE_PIN 2 //Pin 2 para el DI del Rele
#define PIX_PIN 3 //Pin 3 para el DI de la tira led

int total_pixels = 12; //Cantidad de pixels en la tira LED

MFRC522 mfrc522(SS_PIN, RST_PIN); ///Creamos el objeto para el RC522
Adafruit_NeoPixel pixels (total_pixels, PIX_PIN, NEO_GRB + NEO_KHZ800); ///Creamos el objeto para la tira LED
bool security = false;

byte ActualUID[4]; //almacenará el código del Tag leído
byte Usuarios[20][4] = {{0xB5, 0x56, 0x14, 0x53},{0xBB, 0x8A, 0x1E, 0x94}}; //Array de usuarios iniciales

void setup() {
  pixels.begin();
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init(); // Iniciamos el lector RC522
  Serial.println("LR access control: ");
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, HIGH); //Esta configuracion mantiene el accionador abajo
}


void loop() {
  // Definimos el color por defecto
  color_loop(127, 0, 2, 0, false);

  // Revisamos la presencia de tarjetas
  if ( mfrc522.PICC_IsNewCardPresent())
  {
    //Seleccionamos una tarjeta
    if ( mfrc522.PICC_ReadCardSerial())
    {
      // Enviamos serialemente su UID
      Serial.print(F("Card UID:"));
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        ActualUID[i] = mfrc522.uid.uidByte[i];
      }
      color_loop(0, 0, 127, 30, true);
      Serial.print("     ");
      //comparamos los UID para determinar si es uno de nuestros usuarios
      if (compareArray(ActualUID, Usuarios)) {
        Serial.println("Acceso concedido...");
        security = true;
      }
      else {
        Serial.println("Acceso denegado...");
        security = false;
        color_loop(0, 0, 0, 0, false);
        delay(20);
        pixels.show();
      }
      // Terminamos la lectura de la tarjeta tarjeta  actual
      mfrc522.PICC_HaltA();
    }

  }


  if (security == false && RELE_PIN != HIGH) {
    digitalWrite(RELE_PIN, HIGH);
  }
  else if (security == true) {
    digitalWrite(RELE_PIN, LOW);
    color_loop(0, 127, 0, 10, true);
    int time_out = 15000;
    color_loop(0, 127, 127, time_out / total_pixels, true);
    security = false;
    Serial.println("Acerque su tarjeta");
  }
}

// Funcion encargada de generar un loop de colores en
void color_loop(int r, int g, int b, int main_delay, bool internal_show) {
  for (int i; i <= total_pixels; i++) {
    pixels.setPixelColor(i, r, g, b);
    if (internal_show) {
      pixels.show();
    }
    delay(main_delay);
  }
  pixels.show();
}

//Función para comparar dos vectores
boolean compareArray(byte array1[], byte users[20][4])
{
  for (int i; i < sizeof(users); i++)
  {
    if (array1[0] == users[i][0] && array1[1] == users[i][1] && array1[2] == users[i][2] && array1[3] == users[i][3])
    {
      return (true);
    }
  }
  return (false);
}

