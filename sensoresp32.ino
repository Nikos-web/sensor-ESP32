#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTPIN 4         
#define DHTTYPE DHT11
#define LDRPIN 34         

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "FIAP-IOT";
const char* password = "F!@p25.IOT";

const char* mqttServer = "54.221.163.3"; 
const int mqttPort = 1883;
const char* mqttUser = "nicolas";
const char* mqttPassword = "banana";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  Serial.print("Conectando-se ao WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("ESP32Client", mqttUser, mqttPassword)) {
      Serial.println("Conectado ao broker MQTT!");
    } else {
      Serial.print("Falhou com o erro ");
      Serial.print(client.state());
      Serial.println(" — tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(2000);
  setup_wifi();
  client.setServer(mqttServer, mqttPort);
}

bool readDHT11(float &t, float &h) {
  const int maxTentativas = 5;
  int tentativas = 0;

  do {
    t = dht.readTemperature();
    h = dht.readHumidity();
    tentativas++;
    delay(200);
  } while ((isnan(t) || isnan(h)) && tentativas < maxTentativas);

  return !(isnan(t) || isnan(h));
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperatura, umidade;
  int ldrValue = analogRead(LDRPIN);

  if (readDHT11(temperatura, umidade)) {
    String payload = "Temperatura: " + String(temperatura) +
                     " C, Umidade: " + String(umidade) +
                     "%, LDR: " + String(ldrValue);
    client.publish("sensor/dht11", payload.c_str());
    Serial.println(payload);
  } else {
    Serial.println("Falha ao ler o DHT11!");
  }

  delay(10000);
}
