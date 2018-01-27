#include <ESP8266WiFi.h>

//VIIMEISIN 13.12.2016

WiFiServer server(80); //määrittää serveriksi portin 80

const int seuraava = 4; //d2
const int valmius = 2;  //d4
const int napinKaytto = 0; //d3
const int tilaIlmaisin = 14; //d5
const int tilaIlmaisin2 = 16; //d0
int tilaLaskuri = 0;

void setup() {

  pinMode(seuraava, OUTPUT);
  digitalWrite(seuraava, LOW);
  pinMode(valmius, OUTPUT);
  digitalWrite(valmius, LOW);
  pinMode(napinKaytto, OUTPUT);
  digitalWrite(napinKaytto, LOW);
  pinMode(tilaIlmaisin, INPUT);
  pinMode(tilaIlmaisin2, INPUT);
  
  WiFi.mode(WIFI_AP); //wifi moodi yhteyspiste
  WiFi.softAP("testi_verkko", "12345678"); //SSID ja salasana vähintään 8 merkkiä
  server.begin(); //käynnistää palvelimen

  Serial.begin(115200); //käynnistää yhteyden sarjamonitoriin
  IPAddress HTTPS_ServerIP = WiFi.softAPIP(); //määrittää ip-osoitteen
  Serial.print("IP osoite on: "); //tulostaa ip-osoitteen sarjamonitoriin
  Serial.println(HTTPS_ServerIP);
}


void loop() 
{
  /*tarkistaa onko joku yhdistänyt selaimella (client) wifikorttiin
     jos ei niin tarkistetaan uudestaan
     kun on yhdistetty, tulostetaan sarjamonitoriin teksti
  */
  WiFiClient client = server.available(); //tarkistaa onko joku yhdistänyt selaimella wifikorttiin
  if (!client) {
    return;
  }
  Serial.println("someone has connected");

  String request = client.readStringUntil('\r'); //lukee selaimella on kirjoitettu ja tulostaa tekstin monitoriin
  Serial.println(request);

  if (request.indexOf("/VALMIUS") != -1)
  {
    digitalWrite(valmius, HIGH);
    delay(150);
    digitalWrite(valmius, LOW);
  }
  else if (request.indexOf("/ON") != -1)
  {
    digitalWrite(seuraava, HIGH);
    delay(150);
    digitalWrite(seuraava, LOW);
  }
  else if (request.indexOf("/DEAKTIVOI") != -1)
  {
    digitalWrite(napinKaytto, HIGH);
  }
  else if (request.indexOf("/AKTIVOI") != -1)
  {
    digitalWrite(napinKaytto, LOW);
  }


  //Nettisivu
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html<\r\n";
  s += "<head><title>**A.A.S.I.2000**</title><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\"/><meta http-equiv=\"refresh\" content=\"1; url=/\">"
       "<style>"
       "input.MyButton"
       "{"
       "width: 300px;"
       "padding: 20px;"
       "cursor: pointer;"
       "font-weight: bold;"
       "font-size: 150%;"
       "background: #3366cc;"
       "color: #fff;"
       "border: 1px solid #3366cc;"
       "border-radius: 10px;"
       "-moz-box-shadow:: 6px 6px 5px #999;"
       "-webkit-box-shadow:: 6px 6px 5px #999;"
       "box-shadow:: 6px 6px 5px #999;"
       "}"
       "input.MyButton:hover"
       "{"
       "color: #ffff00;"
       "background: #000;"
       "border: 1px solid #fff;"
       "-moz-box-shadow:: 5px 5px 4px #adadad;"
       "-webkit-box-shadow:: 5px 5px 4px #adadad;"
       "box-shadow:: 5px 5px 4px #adadad;"
       "}"
       " </style> "
       " </head> ";
  s += "<h1 style=font-size:300%;>* * Awesome Automatic Security Interface 2000 * * </h1> ";
  s += "<p1>Laitteen tila:</p>";
  s += "<br>";
  if (digitalRead(tilaIlmaisin) == LOW && digitalRead(tilaIlmaisin2) == LOW)
  {
    tilaLaskuri = 1;
    s += "<p2 style=font-size:200%; text-align:center;>Oletustila </p2>";
    s += "<br><br><br>";
    s += "<br><input class=\"MyButton\" type=\"button\" name=\"b1\" value=\"Valmiustila\" onclick =\"location.href='/VALMIUS'\">";
  }
  else if (digitalRead(tilaIlmaisin) == LOW && digitalRead(tilaIlmaisin2) == HIGH)
  {
    tilaLaskuri = 2;
    s += "<p2 style=font-size:200%;>Mittaustila </p2>";
    s += "<br><br><br>";
    s += "<br><input class=\"MyButton\" type=\"button\" name=\"b1\" value=\"Valmiustila\" onclick =\"location.href='/VALMIUS'\">";
  }
  else if (digitalRead(tilaIlmaisin) == HIGH && digitalRead(tilaIlmaisin2) == LOW)
  {
    s += "<p2 style=font-size:200%;>Valvontatila </p2>";
    s += "<br><br><br>";
    s += "<br><input class=\"MyButton\" type=\"button\" name=\"b1\" value=\"Valmiustila\" onclick =\"location.href='/VALMIUS'\">";
  }
  else if (digitalRead(tilaIlmaisin) == HIGH && digitalRead(tilaIlmaisin2) == HIGH)
  {
    tilaLaskuri = 3;
    s += "<p2 style=font-size:200%;>HÄLYTYS!</p2>";
    s += "<br><br><br>";
    s += "<br><input class=\"MyButton\" type=\"button\" name=\"b1\" value=\"Halytys pois\" onclick =\"location.href='/VALMIUS'\">";
  }
  s += "<br><br><br>";
  s += "<br> <input class=\"MyButton\" type = \"button\" name=\"b1\" value=\"Seuraava tila\" onclick =\"location.href='/ON'\">";
  s += "<br><br><br>";
  if(digitalRead(napinKaytto) == LOW)
  {
    s += "<br><input class=\"MyButton\" type=\"button\" name=\"b1\" value=\"Deaktivoi nappi\" onclick =\"location.href='/DEAKTIVOI'\">";
  } 
  else if(digitalRead(napinKaytto) == HIGH)
  {
    s += "<br><input class=\"MyButton\" type=\"button\" name=\"b1\" value=\"Aktivoi nappi\" onclick =\"location.href='/AKTIVOI'\">";
  }
  s += "</html>\n";

  //lähettää nettisivun selaimelle
  client.flush(); //clear previous info in the stream
  client.print(s); // Send the response to the client
  delay(1);
  Serial.println("Client disonnected");
}

