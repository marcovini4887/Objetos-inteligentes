# Objetos-inteligentes

Sistema de Monitoramento e Alarme de CO2 e fumaça
Este projeto implementa um sistema de monitoramento de concentração de CO2 com notificações via Telegram e publicação de dados no ThingSpeak. O sistema utiliza um ESP8266 para ler os valores de um sensor de fumaça e acionar um alarme LED caso a concentração de CO2 exceda um limite predefinido. Além disso, os dados são enviados periodicamente para o ThingSpeak, e notificações são enviadas via Telegram quando o alarme é acionado.
-----
-------------------------------------------------------------------------------------------------------------
-----
Funcionamento e Uso
.
Conexão à Rede WiFi: O ESP8266 se conecta à rede WiFi usando as credenciais fornecidas.
.
Leitura do Sensor de Fumaça: O valor de concentração de CO2 é lido do sensor de fumaça (conectado ao pino A0).
.
Publicação dos Dados no ThingSpeak: Os valores lidos são enviados ao ThingSpeak a cada 5 segundos.
.
Ação do Alarme: Se a concentração de CO2 ultrapassar o limite de 700 ppm, um LED vermelho é acionado.
.
Notificação via Telegram: Quando o alarme é acionado, uma notificação é enviada via Telegram.
.
Reconexão Automática: O sistema tenta se reconectar automaticamente ao broker MQTT caso a conexão seja perdida.

-----
-------------------------------------------------------------------------------------------------------------
-----
Software e Documentação do Código

Estrutura do Código
.
Imports de Dependências: As bibliotecas necessárias são incluídas no início do código.
.
Definição de Variáveis e Configurações: As variáveis de configuração são definidas, incluindo credenciais WiFi, token do Telegram, e configurações do ThingSpeak.
.
Inicialização de Serviços: Os serviços de WiFi, MQTT, e Telegram são inicializados.
.
Funções de Conexão: Funções para conectar ao WiFi e ao broker MQTT.
.
Funções de Controle do Alarme: Funções para ligar e desligar o alarme.
.
Loop Principal: O loop principal lê o sensor, verifica se o alarme deve ser acionado, envia dados ao ThingSpeak, e notifica via Telegram.
Descrição do Hardware Utilizado
-----
-------------------------------------------------------------------------------------------------------------
-----
Plataforma de Desenvolvimento: ESP8266
Descrição: Microcontrolador com WiFi integrado, utilizado para conectar à rede e enviar dados.
Função: Conectar ao WiFi, ler dados do sensor, e comunicar-se com ThingSpeak e Telegram.
Sensor de Fumaça: MQ-2 (ou similar)
.
Sensor de Fumaça: MQ-2 (ou similar)
Descrição: Sensor analógico utilizado para detectar concentrações de CO2.
Conexão: Pino A0 do ESP8266.
LED de Alarme: LED Vermelho
.
LED de Alarme: LED Vermelho
Descrição: LED utilizado para indicar quando a concentração de CO2 ultrapassa o limite.
Conexão: Pino D7 do ESP8266.
-----
-------------------------------------------------------------------------------------------------------------
-----
Documentação das Interfaces, Protocolos e Módulos de Comunicação
.
WiFi (ESP8266WiFi.h):
Função: Conectar o ESP8266 à rede WiFi.
Uso: WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
.
.
ThingSpeak (PubSubClient.h):
Função: Enviar dados ao serviço ThingSpeak via MQTT.
Configuração: client.setServer(mqttServer, mqttPort);
Publicação: client.publish(topic, payload);
.
.
Telegram (UniversalTelegramBot.h):
Função: Enviar notificações via Telegram.
Configuração: UniversalTelegramBot bot(BOT_TOKEN, clientSecure);
Envio de Mensagem: bot.sendMessage(CHAT_ID, message, "");
-----
-------------------------------------------------------------------------------------------------------------
-----
Instruções para Reproduzir
1 - Hardware:
Monte o circuito conforme descrito acima, conectando o sensor de fumaça ao pino A0 e o LED ao pino D7 do ESP8266.
.
2 - Software:
Instale as bibliotecas necessárias no Arduino IDE.
Copie o código fornecido para a IDE.
Atualize as credenciais WiFi e token do Telegram no código.
Faça upload do código para o ESP8266.
.
3 - Configuração do ThingSpeak:
Crie uma conta no ThingSpeak e configure um canal.
Atualize o código com as informações do canal ThingSpeak.
.
4 - Execução:
Conecte o ESP8266 à alimentação.
Monitore a saída serial para verificar as leituras do sensor e notificações.
Este projeto pode ser estendido para incluir mais sensores, diferentes tipos de notificações, ou integração com outros sistemas IoT.
