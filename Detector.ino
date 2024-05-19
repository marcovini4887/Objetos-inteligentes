//---------------------------------- Imports de dependências do projeto---------
#include <ESP8266WiFi.h>  
#include <WiFiClientSecure.h>
#include <ThingSpeak.h>
#include <UniversalTelegramBot.h>
#include <PubSubClient.h>
//------------------------------------------------------------------------------

//--------------------------------- Definição de variaveis de ambiente e configurações ---------
#define WIFI_SSID "VIVOFIBRA-D5D4"  // Nome da rede wifi
#define WIFI_PASSWORD "722369D5D4" // Senha da rede wifi
#define BOT_TOKEN "6981692260:AAGJPiBXZP06EbBbHCwyRrCJUs37t-iPLls"  // Token do bot do telegram
#define CHAT_ID "5708886922" // ID do chat do telegram

#define THINGSPEAK_WRITE_INTERVAL 5000 // Intervalo em ms entre envios de dados ao ThingSpeak
#define ALARM_LED_PIN D7    // IO do led vermelho
#define SMOKE_SENSOR A0     // IO do sensor de fumaça

#define ALARM_LIMIT 700       // Limite de concentração de CO2 para acionamento do alarme
#define ALARM_INTERVAL 30000 // Tempo em ms em que o alarme pode ser acionado novamente

const char* mqttServer = "mqtt3.thingspeak.com"; // Broker do ThingSpeak
const int mqttPort = 1883; //Porta pública do serviço
const char* mqttUser = "Nw0kJz0YJjcpMBgFNh0zIAY";
const char* mqttClientId = "Nw0kJz0YJjcpMBgFNh0zIAY";
const char* mqttPassword = "m/F6PrbTpHJCE5e66267v7L4";

const char* topic = "channels/2552884/publish"; //channels/<channelID>/publish

const int field1 = 1; //concetracao de fumaça/gas
const int field2 = 2; //alarme acionado? bit 1 ou 0

unsigned long lastTime;
unsigned long currentTime; 
unsigned long alarmTurnedOffTime;

bool alarmFlag;
bool commandFlag;
int turnOffCommand;
float sensorValue;
//-----------------------------------------------------------------------------------------------

//---------------------------- Inicialização de serviços ----------------------------------------
X509List cert(TELEGRAM_CERTIFICATE_ROOT); //Certificado de segurança de inicialização da API do robô do Telegram
WiFiClientSecure clientSecure; //Certificado de segurança do WiFi
WiFiClient espClient; //Cliente do WiFi
PubSubClient client(espClient); //Serviço de publicação de mensagem do MQTT
UniversalTelegramBot bot(BOT_TOKEN, clientSecure); //Inicialização do BOT do Telegram
//-----------------------------------------------------------------------------------------------

void ConnectToWiFi(void){
  // ------------ Configura root certificate para api.telegram.org ------------------------------
  configTime(0, 0, "pool.ntp.org"); 
  clientSecure.setTrustAnchors(&cert);
  //---------------------------------------------------------------------------------------------

  // ------------ Tenta conectar ao wifi --------------------------------------------------------
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
  //---------------------------------------------------------------------------------------------
}

void reconnect() {
  // ------------ Tenta conectar ao broker MQTT --------------------------------------------------------
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect(mqttClientId, mqttUser, mqttPassword)) {
      Serial.println("conectado");
    } else {
      Serial.print("falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
  //---------------------------------------------------------------------------------------------
}

void SendTelegramNotification(String message) {
  bot.sendMessage(CHAT_ID, message, "");
  Serial.println(message);
}

void TurnAlarmOn(){ // Liga led de alarme
  alarmFlag = true;
  digitalWrite(ALARM_LED_PIN, HIGH); 
  Serial.println("Atenção! Alarme acionado!");
}

void TurnAlarmOff(){ // Desliga led de alarme
  alarmFlag = false;
  digitalWrite(ALARM_LED_PIN, LOW);
}
 
void setup(){  
    Serial.begin(115200);
    delay(50);

    // Inicializa outputs (váriaveis de processo)
    pinMode(ALARM_LED_PIN, OUTPUT);
    alarmFlag = false;
    commandFlag = false;
    turnOffCommand = 0;

    // Conecta ao wifi
    ConnectToWiFi();

    // Setup do Broker MQTT
    client.setServer(mqttServer, mqttPort);

    lastTime = 0;
}

void loop(){
  // ------------  TryCatch de reconexão do broker MQTT -------------------------------
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
  //------------------------------------------------------------------------------------
  
  // -------------- Regra de negócio de validação do valor lido pelo sensor ------------
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
  // -----------------------------------------------------------------------------------


  // Verifica se é o momento de enviar dados ao ThingSpeak
  if((currentTime - lastTime > THINGSPEAK_WRITE_INTERVAL))
  {
    // ----- Formatando o string para publicação da mensagem ----------------------------
    char payload[100];
    sprintf(payload, "field1=%d&field2=%d", sensorValue, alarmFlag);
    // -----------------------------------------------------------------------------------
    
    // ----- Envio da mensagem para o ThingSpeak -----------------------------------------
    client.publish(topic, payload);
    // -----------------------------------------------------------------------------------

    if(alarmFlag){
      SendTelegramNotification("O alarme de incêndio foi acionado.");
    }
    lastTime = currentTime;
  }

  delay(2500);
}
