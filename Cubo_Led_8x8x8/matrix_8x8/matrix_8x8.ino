#include "LedControlMS.h"

#define NumMatrix 1       // Cuantas matrices vamos a usar


LedControl lc = LedControl(12, 11, 10, NumMatrix); // Creamos una instancia de LedControl
/*
  Pin 12 DIN
  Pin 11 CLK
  Pin 10 CS
*/

byte matrix[8] = {
  0x00,
  0xe4,
  0xf8,
  0x9c,
  0x5a,
  0x77,
  0x02,
  0x00,
};


void setup()
{
  Serial.begin(250000);
  delay(1000);
  for (int i = 0; i < NumMatrix ; i++)
  {
    lc.shutdown(i, false);   // Activar las matrices
    lc.setIntensity(i, 1);   // Poner el brillo a un valor intermedio
    lc.clearDisplay(i);      // Y borrar todo
  }
  for (int i; i < 8; i++) {
    lc.setColumn(0, i, matrix[i]);
  }
  delay(1000);
}

void loop()
{
  if (Serial.available() > 0) {
    byte matrix_release[8]
    {
      0,0,0,0,0,0,0,0
      };
    for (int i; i < 8; i++) {
      lc.setColumn(0, i, matrix_release[i]);
    }
    for (int i; i < 8; i++) {
      matrix_release[i] = Serial.read();
    }
    for (int i; i < 8; i++) {
      lc.setColumn(0, i, matrix_release[i]);
    }
  }
  delay(0);
}

