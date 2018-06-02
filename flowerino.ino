#include <SPI.h>
#include <Ethernet.h>
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; // MAC della scheda ethernet

char server[] = "flowerino.altervista.org";
IPAddress ip(192, 168, 1, 130); //indirizzo ip dell'Arduino
String api_key = ""; //api key (registrazione sul sito flowerino.altervista.org)

EthernetClient client;

//definisco le variabili
String strURL = "";
float lum = 0;
float temp = 0;
float umid = 0;
float umidt = 0;
int sensorPin = A1;
int relepin = 3;
int redPin = 6;
int greenPin = 5;
int bluePin = 4;

void setup(){
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  dht.begin();
  if (Ethernet.begin(mac) == 0){
  Serial.println("Configurazione DHCP fallita!");
  Ethernet.begin(mac, ip);}
  delay(1000);
}

void loop(){
  UpdateTemp();
  while(client.available()){
  char c = client.read();
  Serial.print(c);}
  if (!client.connected()){
  Serial.println();
  Serial.println("Disconnesso.");
  client.stop();}

  //esegui la richiesta ogni 60 secondi
  delay(60000);
}

void UpdateTemp(){
  Serial.println("Connessione...");

  if (client.connect(server, 80)){
    Serial.println("Connesso");
    //acquisisco i valori dai sensori
    lum = analogRead(0);
    umidt= analogRead(sensorPin);
    Serial.print("Valore: ");
    Serial.println(umidt);
    temp= dht.readTemperature();
    umid= dht.readHumidity();
    if(umidt<500){digitalWrite(bluePin, HIGH);} // accende avviso acqua
    if(umidt>500){digitalWrite(bluePin, LOW);} // spegne avviso acqua
    //creo l'url utilizzando una stringa
    strURL = "GET /arduino.php?valore=";
    strURL += (int)lum;
    strURL += "&temp=";
    strURL += (int)temp;
    strURL += "&umid=";
    strURL += (int)umid;
    strURL += "&umidt=";
    strURL += (int)umidt;
    strURL += "&utente=";
    strURL += api_key;
    strURL += " HTTP/1.1";
    Serial.println(strURL);
    //invio la richiesta al server
    client.println(strURL);
    client.println("Host: flowerino.altervista.org");
    client.println("Connection: close");
    client.println();
    //chiudo la connessione
    client.stop();
  }
  else{
    Serial.println("Errore Connessione");
  }
}
