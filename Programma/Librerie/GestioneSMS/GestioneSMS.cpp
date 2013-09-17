#include <wprogram.h>
#include "GestioneSMS.h"
#include <Arduino.h>
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"


	
// Construttore che definisce lunghezza massima dei messaggi da inciare e ricevere e la lunghezza del numero di telefono
// Stabilisce il numero di telefono al quale inviare il messaggio di approvazione
// Configura il gsm
GestioneSMS::GestioneSMS();
	
{
  Serial.begin(9600); // configuro la seriale
  LENGHT_MESS=180; // Lunghezza massima del messaggio
  LENGHT_NUM=12; // Lunghezza massima del numero
  numero="3333333333"; // numero al quale inviare il messaggio di approvazione di irrigazione
  pos=0; // pos attuale dell'ultimo messaggio ricevuto
  
  // Configuro il gsm
  if (gsm.begin(2400))
    Serial.println("\nstatus=READY");
  else Serial.println("\nstatus=IDLE");
}	


// Metodo che invia un messaggio ritorna true se è andato a buon fine l'invio flase altrimenti
// e dopo averlo inviato cambia il valore della variabile di stato inviato
boolean GestioneSMS::invioSMS(boolean & inviato, boolean & risposto, char * lon, char * lat)
{
	SMSGSM sms; // Creiamo un oggetto sms della classe SMSGSM  
	costruzioneMessaggio(lon, lat);  // Costruzione del messaggio da inviare
	if(sms.SendSMS(numero, messaggio)) // invio messaggio al numero registrato e il messaggio costruito
	{
	  Serial.println((String) numero);
      Serial.println((String) messaggio);
	  inviato=true;
	  risposto=false;
	  return true;
	}
	else return false;
}


// Metodo che legge il contenuto di un nuovo messaggio se esiste
// e dopo averlo letto cambia i valori delle variabili di stato inviato e risposto
void GestioneSMS::letturaSMS(boolean & inviato, boolean & risposto)
{
	SMSGSM sms; // Creiamo un oggetto sms della classe SMSGSM
 // Metto in pos la posizione del messaggio non letto altrimenti metto 0
	pos=sms.IsSMSPresent(SMS_UNREAD);
 // Controllo se pos è diverso da 0 cioè controllo se ci sono nuovi messaggi
	if(pos != 0) 
	{
 // Legge il messaggio in posizione pos e posiziona il numero di chi lo ha inviato in number e il testo in message di lunghezza massima LENGHT_MESS
		sms.GetSMS(pos,number,message, LENGHT_MESS);
		Serial.println(pos);
		Serial.println((String) number);
        Serial.println((String) messagge);
		inviato=false;
		risposto=true;
	  }
}


//Metodo che cancella un messaggio
void GestioneSMS::cancellazioneSMS()
{
	SMSGSM sms; // Creiamo un oggetto sms della classe SMSGSM
	sms.DeleteSMS(pos);  // Cancellazione del messaggio in posizione pos
	Serial.println(pos);
}


// Metodo che costruisce il messaggio da inviare per l'approvazione
void GestioneSMS::costruzioneMessaggio(char * lon, char * lat)
{
// Leggo il valore di longitudine e latitudine dal sensore GPS
  SensorGPS.letturaGPS(lon,lat);
// Concatenazione di stringhe per la formazione del meassaggio da inviare
  strcat(messaggio, "C'è necessità di irrigare la serra. Devo farlo? (Rispondi SI o NO inserendo la password\n");
  strcat(messaggio, (String)lon);  
  strcat(messaggio, (String)lat);
  Serial.println((String) messaggio);
}


// Metodo che controlla il contenuto dei nuovi messaggi
void GestioneSMS::controlloSMS(boolean & irrig)
{
// Leggo nuovi SMS se esistono
  letturaSMS(pos, number, message, LENGHT_MESS);
  // Controllo se esiste la password all'interno del messaggio
  if(controlloPassword())
   {
   // Controllo il contenuto del messaggio e se contiene SI approvo l'irrigazione
     controlloContenuto(irrig);
   }
}


// Metodo che controlla se all'interno del messaggio c'è la password esatta
boolean GestioneSMS::controlloPassword()
{
  char *p; // puntatore che utilizzo per verificare Password
  // Controllo che all'interno del messaggio esiste la sottostringa equivalente alla password altrimenti restituisco NULL
  p=strstr(message,"SERRA");
  if(p)
  {
    Serial.println("PASSWORD OK");
    return true;
  }
  else
  {
    cancellazioneSMS(); // Se nel messaggio la password è sbagliata allora il messaggio viene eliminato
    return false;
  }
}


// Metodo che controlla se all'interno del messaggio c'è l'approvazione o meno all'irrigazione
void GestioneSMS::controlloContenuto(boolean & irrig)
{
  char *p; // puntatore che utilizzo per verificare il contenuto del messaggio
  
  // Controlla se nel messaggio è contenuto SI e restituisce un puntatore alla prima occorrenza di SI in message, NULL se non lo trova
  p=strstr(message,"SI");
  if(p)
  {
    Serial.println("IRRIGATORE DA AVVIARE");
    irrig = true; // Approvo che l'irrigazione venga fatta nei tempi stabiliti
  }
  else
  {
    // Controlla se nel messaggio è contenuto NO e restituisce un puntatore alla prima occorrenza di NO in message, NULL se non lo trova
    p=strstr(message,"NO"); // se all'interno del messaggio c'è NO allora nessuna approvazione
    if(p)
    {
      Serial.println("IRRIGATORE DA NON AVVIARE");
    }
    else
    {
      cancellazioneSMS(); // Se nel messaggio non è contenuto nè SI nè NO allora il messaggio viene eliminato  
    }   
  } 
}
