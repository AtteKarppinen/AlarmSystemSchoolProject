#include <NewPing.h>
#define MAX_DISTANCE 100         // Suurin etäisyys mitä mitataan centtimetreinä

const int nappi = 2;             // Pinni 2 napin käytössä
const int summeri = 3;           // Pinni 3 summerin käytössä
const int napinKaytto = 4;       // Input jolla otetaan nappi käyttöön/pois käytöstä
const int wifiValmiusTila = 5;   // Input, jolla laitetaan hälytin valmiustilaan
const int TRIGGER_PIN = 6;       // Määritetään ultraääni trigger pinnille 6
const int ECHO_PIN  =  7;        // Määritetään ultraääni echo pinnille 7
const int greenLed = 8;          // Vihreä ledi pinnistä 8
const int yellowLed = 9;         // Keltainen ledi pinnistä 9
const int redLed = 10;           // Punainen ledi pinnistä 10
const int tilaIlmaisin = 11;     // Wifi moduulille ilmaistaan mikä tila päällä
const int wifiModuuliIn = 12;    // Input. Moduulilta tuleva tieto. Nyt käytössä tilanvaihto-nappina
const int tilaIlmaisin2 = 13;    // Toinen tarvittava tilan ilmaisu

float alaraja = 1 - 0.40;   // Raja-arvoja muuttamalla laitteen herkkyyttä voidaan säätää. Jos haluat esim. 20% herkkyyden
float ylaraja = 1 + 0.40;   // aseta ala- ja ylarajan jälkimmäisen luvun paikalle desimaaliluku, tässä tapauksessa 0.20
float etaisyys;
int laskuri = 1;
unsigned long viimeisinMillisekunti = 0;
const long tauko = 100;
int ledState = LOW;
boolean halytys = false;                            // Boolean sisältää kaksi mahdollista tilaa, mikä sopii projektiin

NewPing tutka(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup, johon määritellään pinnit ja maksimietäisyys

void alarm()    // Hälytysfunktio. Tänne määritellään mitä tapahtuu jos hälytys laukeaa
{
  // Tila 1 1
  digitalWrite(tilaIlmaisin, HIGH);
  digitalWrite(tilaIlmaisin2, HIGH);

  // Tämä loop on tehty ledien välkyntää varten ilman delay toimintoa (näin summeri voi soida samaan aikaan kun ledit välkkyvät)
  for (int x = 1; x < 10; x++)
  {
    unsigned long nykyinenMillisekunti = millis();
    if (nykyinenMillisekunti - viimeisinMillisekunti >= tauko)
    {
      // tallentaa viimeisimmän hetken, kun ledi välkkyi
      viimeisinMillisekunti = nykyinenMillisekunti;

      // vaihtaa ledin tilan päinvastaiseksi
      if (ledState == LOW)
      {
        ledState = HIGH;
      }
      else
      {
        ledState = LOW;
      }
    }
    digitalWrite(greenLed, ledState);
    digitalWrite(yellowLed, ledState);
    digitalWrite(redLed, ledState);
    digitalWrite(summeri, HIGH);
    delay(1);
    digitalWrite(summeri, LOW);
    delay(1);

    // Painonappi aloittaa loopin alusta palaten oletustilaan
    if (digitalRead(nappi) == HIGH)
    {
      loop();
    }
    // Sama homma, mutta etätoimintona
    else if (digitalRead(wifiValmiusTila) == HIGH)
    {
      laskuri = 1;
    }
  }
}

void valmiusTila()    // Tämä funktio on ensimmäinen tila ohjelman käynnistyessa ja myös oletustila
{
  digitalWrite(greenLed, HIGH);
  digitalWrite(yellowLed, LOW);
  digitalWrite(redLed, LOW);
  halytys = false;

  // Tila 0 0
  digitalWrite(tilaIlmaisin, LOW);
  digitalWrite(tilaIlmaisin2, LOW);
}

void mittausTila()    // Tämä funktio vastaa vartioitavan esineen etäisyyden mittauksesta
{
  // Tila 0 1
  digitalWrite(tilaIlmaisin, LOW);
  digitalWrite(tilaIlmaisin2, HIGH);

  // Painonappi tai etäyhteys siirtää järjestelmän valvontatilaan
  if (digitalRead(nappi) == HIGH || digitalRead(wifiModuuliIn) == HIGH)
  {
    laskuri = 3;
  }

  // Mahdollisuus palata oletustilaan etäyhteyden avulla
  if (digitalRead(wifiValmiusTila) == HIGH)
  {
    laskuri = 1;
  }

  delay(200); //mittaa etäisyyden 200ms välein
  unsigned int us = tutka.ping();
  etaisyys = us / US_ROUNDTRIP_CM;
  digitalWrite(greenLed, HIGH);
  digitalWrite(yellowLed, HIGH);
  digitalWrite(redLed, LOW);
  Serial.print("Etaisyys:   ");
  Serial.print(etaisyys);
  Serial.println("cm");
}


void valvontaTila()   // Tämä funktio käyttää mittaustilan määrittelemää etäisyyttä ja aloittaa oikean "valvomisen"
{
  // Tila 1 0
  digitalWrite(tilaIlmaisin, HIGH);
  digitalWrite(tilaIlmaisin2, LOW);

  //jos Painonappia painetaan valvontatilassa, aloitetaan main-loop alusta
  if (digitalRead(nappi) == HIGH)
  {
    loop();
  }

  // Mahdollisuus palata oletustilaan etäyhteyden avulla
  if (digitalRead(wifiValmiusTila) == HIGH)
  {
    laskuri = 1;
  }

  digitalWrite(greenLed, LOW);
  digitalWrite(yellowLed, HIGH);
  digitalWrite(redLed, LOW);

  if (halytys == true)
  {
    alarm();
  }
  else
  {
    delay(50);// 50ms odotus mittausten välissä
    unsigned int uS = tutka.ping();
    unsigned int distance = uS / US_ROUNDTRIP_CM;
    Serial.println(distance);
    // Tässä kohtaa on määritelty etäisyyden muutos, joka aiheuttaa hälytyksen
    if (distance < etaisyys * alaraja && distance > 0 || distance > etaisyys * ylaraja)
    {
      halytys = true;
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(nappi, INPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(summeri, OUTPUT);
  pinMode(wifiModuuliIn, INPUT);
  pinMode(wifiValmiusTila, INPUT);
  pinMode(napinKaytto, INPUT);
  pinMode(tilaIlmaisin, OUTPUT);
  pinMode(tilaIlmaisin2, OUTPUT);
  digitalWrite(tilaIlmaisin, LOW);
  digitalWrite(tilaIlmaisin2, LOW);
  valmiusTila();  //valmiustila on käynnissä kun virrat tulee päälle
}

void loop()
{
  // napinTila lukee nappi-pinnistä, painetaanko painonappia vai ei
  int napinTila = digitalRead(nappi);
  delay(150);

  // Jos wifimoduulilta tulee voltteja wifiValmiusTila inputtiin, laittaa ohjelma valmiusTila-funktion päälle
  if (digitalRead(wifiValmiusTila) == HIGH)
  {
    laskuri = 1;
  }

  // Turvallisuussyistä, pystymme deaktivoimaan painonapin wifimoduulin avulla
  if (digitalRead(napinKaytto) == HIGH)
  {
    napinTila = NULL;
  }

  // Joko painonapista tai etänä wifimoduulin avulla, ohjelma lisää laskuriin arvon ja muuttaa näin hälytysjärjestelmän tilaa
  if (napinTila == HIGH || digitalRead(wifiModuuliIn) == HIGH)
  {
    laskuri = laskuri + 1;
    Serial.println(laskuri);
    delay(150);
  }

  // Switch case eri tiloille ja loopit tarvittaessa
  switch (laskuri)
  {
    case 1:
      valmiusTila();
      break;
    case 2:
      do
      {
        mittausTila();
      } while (laskuri == 2);
      break;
    case 3:
      do
      {
        valvontaTila();
      } while (laskuri == 3);
    default:
      laskuri = 1;
  }
}

