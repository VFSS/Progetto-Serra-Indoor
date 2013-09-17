#include <wprogram.h>
#include "SensorTemp.h"
#include "GestioneTemp_h"
#include "MotoreTetto.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

	
	
GestioneTemp::GestioneTemp()
{
 Serial.begin(9600); // configuro la seriale
 UMIDTERR_MAX2=500; // valore di umidità massimo oltre il quale si chiude il tetto
 TEMP_MAX=33.0; // valore di temperatura massima al di sotto del quale si può chiudere il tetto
 TEMP_MAX2=25.0; // valore di temperatura massima al di sotto del quale si può aprire il tetto
 TEMP_MIN=5.0; // valore di temperatura minima al di sopra del quale si può chiudere il tetto
 TEMP_MIN2=11.0; // valore di temperatura massima al di sopra del quale si può aprire il tetto
}	


GestioneTemp::controlloTemp(int & umid, float & temp, boolean & tetto, boolean & dir)
{
  SensorTemp.letturaTA(temp);  // lettura del valore di temperatura
  // controllo se la temperatura rilevata è dentro un certo range altrimenti passo il comando alla funzione di chiusura del tetto  
  if(temp > TEMP_MAX || temp < TEMP_MIN && !tetto)
  {
     MotoreTetto.AvvioMotore(dir); // chiudo il tetto
	 dir = false; // imposto la direzzione contraria di rotazione
	 tetto = true; // imposto la variabile di stato del tetto a true cioè il tetto è chiuso
  }
  // se il tetto è chiuso e la temperatura è compresa tra 10 e 30 gradi e l'umidità del terreno è minore di 700 allora il tetto è riaperto  
  else if (temp < TEMP_MAX2  && temp > TEMP_MIN2 && umid < UMIDTERR_MAX2 && tetto) 
  {
     MotoreTetto.AvvioMotore(dir); // apro il tetto
	 dir = true; // imposto la direzzione contraria di rotazione
	 tetto = false; // imposto la variabile di stato del tetto a false cioè il tetto è aperto
  }
}
}