#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"

#define DHTPIN D4     // what digital pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

// Initialize Wifi connection to the router
char ssid[] = "nama_wifi";     // your network SSID (name)
char password[] = "password_wifi"; // your network key

// Initialize Telegram BOT
#define BOTtoken "isi_dengan_token_dari_botfather"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done
bool Start = false;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/start") {
      String welcome = "Selamat datang di pengamat suhu dan kelembaban, " + from_name + ".";
      bot.sendMessage(chat_id, welcome);
    }
    
    if (text == "/suhu") {
        float t = dht.readTemperature();
        String temperatur = "suhu saat ini adalah: ";
        temperatur += String(t);
        temperatur += " *Celcius";
        bot.sendMessage(chat_id, temperatur);
    }

    if (text == "/kelembaban") {
        float h = dht.readHumidity();
        String kelembaban = "kelembaban saat ini adalah: ";
        kelembaban += String(h);
        kelembaban += " persen";
        bot.sendMessage(chat_id, kelembaban);
    }

    if (text == "/suhu_kelembaban") {
        float h = dht.readHumidity();
        String kelembaban = "kelembaban saat ini adalah: ";
        kelembaban += String(h);
        kelembaban += " persen";
        float t = dht.readTemperature();
        String temperatur = "suhu saat ini adalah: ";
        temperatur += String(t);
        temperatur += " *Celcius";
        String suhu_kelembaban = temperatur + " dan " + kelembaban;
        bot.sendMessage(chat_id, suhu_kelembaban);
    }
  }
}


void setup() {
  dht.begin();
  Serial.begin(115200);
  
  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
