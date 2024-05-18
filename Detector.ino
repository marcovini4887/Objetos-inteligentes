#include <ESP8266WiFi.h>  
#include <WiFiClientSecure.h>
#include <ThingSpeak.h>
#include <UniversalTelegramBot.h>

#define WIFI_SSID "VIVOFIBRA-D5D4"  // Nome da rede wifi
#define WIFI_PASSWORD "722369D5D4" // Senha da rede wifi
#define BOT_TOKEN "6981692260:AAGJPiBXZP06EbBbHCwyRrCJUs37t-iPLls"  // Token do bot do telegram
#define CHAT_ID "5708886922" // ID do chat do telegram

#define THINGSPEAK_WRITE_INTERVAL 5000 // Intervalo em ms entre envios de dados ao ThingSpeak
#define ALARM_LED_PIN D7    // IO do led vermelho
#define SMOKE_SENSOR A0     // IO do sensor de fumaça

#define ALARM_LIMIT 700       // Limite de concentração de CO2 para acionamento do alarme
#define ALARM_INTERVAL 30000 // Tempo em ms em que o alarme pode ser acionado novamente

unsigned long myChannelNumber = 2552884;          // Número do canal do ThingSpeak
const char * myWriteAPIKey = "9WKB1OC1Z3Y02TZT";  // Chave de escrita do canal do ThingSpeak
const char * myReadAPIKey = "3LMA21ZFUKG2K7LU";  // Chave de leitura do canal do ThingSpeak

const int field1 = 1;
const int field2 = 2;
const int field3 = 3;

unsigned long lastTime;
unsigned long currentTime; 
unsigned long alarmTurnedOffTime;

bool alarmFlag;
bool commandFlag;
int turnOffCommand;
float sensorValue;

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure clientSecure;
WiFiClient client;
UniversalTelegramBot bot(BOT_TOKEN, clientSecure);

void ConnectToWiFi(void){
  // Configura root certificate para api.telegram.org
  configTime(0, 0, "pool.ntp.org"); 
  clientSecure.setTrustAnchors(&cert);

  // Tenta conectar ao wifi
  Serial.println("Conectando à rede WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado com sucesso!");  
  Serial.println("IP obtido: ");
  Serial.println(WiFi.localIP());
}

void SendTelegramNotification(String message) {
  bot.sendMessage(CHAT_ID, message, "");
  Serial.println(message);
}

void TurnAlarmOn(){
  alarmFlag = true;
  digitalWrite(ALARM_LED_PIN, HIGH); // Liga led de alarme
  Serial.println("Atenção! Alarme acionado!");
}

void TurnAlarmOff(){
  alarmFlag = false;
  digitalWrite(ALARM_LED_PIN, LOW); // Desliga led de alarme
}
 
void setup(){  
    Serial.begin(115200);
    delay(50);

    // Inicializa outputs
    pinMode(ALARM_LED_PIN, OUTPUT);
    alarmFlag = false;
    commandFlag = false;
    turnOffCommand = 0;

    // Conecta ao wifi
    ConnectToWiFi();

    // Inicializa ThingSpeak
    lastTime = 0; 
    ThingSpeak.begin(client);
}
 
void loop(){
  int sensorValue = analogRead(SMOKE_SENSOR) * 8;
  
  Serial.print("Concentracao de CO2 (ppm): ");
  Serial.println(sensorValue);

  currentTime = millis();
  
  if(commandFlag){
    // Verifica se já passou o tempo necessário para verificação de fumaça
    if((currentTime - alarmTurnedOffTime > ALARM_INTERVAL)){
      commandFlag = false; // alarme pode ser ligado novamente se houver fumaça
    }  
  }else{
    // Verifica se o valor do sensor está dentro do limite
    if(sensorValue > ALARM_LIMIT){
      TurnAlarmOn();
    }else{
      TurnAlarmOff();
    }     
  }

  // Verifica se é o momento de enviar dados ao ThingSpeak
  if((currentTime - lastTime > THINGSPEAK_WRITE_INTERVAL))
  {
    ThingSpeak.setField(field1, sensorValue);
    ThingSpeak.setField(field2, alarmFlag);
    ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if(alarmFlag){
      SendTelegramNotification("O alarme de incêndio foi acionado.");
    }
    lastTime = currentTime;
  }

  delay(2500);
}
