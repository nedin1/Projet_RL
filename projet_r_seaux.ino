#include <stm32l475e_iot01.h>
#include <stm32l475e_iot01_tsensor.h>
#include <stm32l475e_iot01_hsensor.h>
#include <stm32l475e_iot01_psensor.h>
#include <SPI.h>
#include <WiFiST.h>
#include <WiFiUdpST.h>
#include <MQTTClient.h>

unsigned long lastMillis = 0;  
float sensor_value_T = 0;
float sensor_value_H = 0;
float sensor_value_P = 0;
String string_MQTT;
int16_t topic=0; 
SPIClass SPI_3(PC12, PC11, PC10); 
WiFiClass WiFi(&SPI_3, PE0, PE1, PE8, PB13);
int status = WL_IDLE_STATUS;

char ssid[] = "Ooredoo _M30_83E8"; 
char pass[] = "585266ED";

WiFiClient net;
MQTTClient client;
 
void setup() {
  BSP_TSENSOR_Init();// temperature
  BSP_HSENSOR_Init();// Humidité
  BSP_PSENSOR_Init(); // Pression 

Serial.begin(115200);

    while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid,pass);
    
    delay(10000);
  }
    Serial.println("Connected to wifi");
    printWifiStatus();
    
    client.begin("broker.hivemq.com", net);
    client.onMessage(messageReceived);
    Serial.print("\nconnecting...");  
   
    while (!client.connect("Nedin", "Nedin bouzaida", "xx"))
    {
    Serial.print(".");
    delay(1000);
  }
 Serial.println("\n connected!");
  client.subscribe("/temperature");  
}

void loop() 
{
   
    client.loop();

  if (!client.connected()) {
     client.disconnect();
   
     client.begin("broker.hivemq.com", net);
     delay(1000);
     string_MQTT =  String(sensor_value_T, 1);  
     client.publish("/test","Hello");
     topic=-1;  
    }
    
  if (millis() - lastMillis > 3000) 
 {
     topic++;
     Serial.print("\n *************TEMPERATURE*************** ");
     sensor_value_T = BSP_TSENSOR_ReadTemp();
     Serial.print("\n TEMPERATURE = ");
     Serial.print(sensor_value_T);
     Serial.print("\n *************************************** ");
          Serial.print("\n *************HUMIDITY****************** ");
     sensor_value_H = BSP_HSENSOR_ReadHumidity();
     Serial.print("\n Humidite = ");
     Serial.print(sensor_value_H);
     Serial.print("\n *************************************** ");
     Serial.print("\n *************PRESSURE****************** ");
     sensor_value_P = BSP_PSENSOR_ReadPressure();
     Serial.print("\n Pression = ");
     Serial.print(sensor_value_P);
     Serial.print("\n *************************************** ");

     valeur_pub();
}
}
void messageReceived(String &topic, String &payload) 
{
     Serial.print("\n -------------------------------------- ");
     Serial.println("\n\n incoming: " + topic + " - " + payload);
     Serial.print("\n\n -------------------------------------- ");
}
void printWifiStatus() 
{
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
}
void valeur_pub()
{
     string_MQTT =  String(sensor_value_T, 1); 
     client.publish("a","temperature en *C");
     client.publish("/temperature",string_MQTT);
       
     client.publish("b","Humidite en g/m^3  ");
     string_MQTT =  String(sensor_value_H, 1);  
     client.publish("/Humidite",string_MQTT);
      
     string_MQTT =  String(sensor_value_P, 1);
     client.publish("c","Pression en bar");  
     client.publish("/Pression",string_MQTT);
}

 






  
