#include <WiFi.h>
#include <SPI.h>
#include <yl_data_point.h>
#include <yl_device.h>
#include <yl_wifi_client.h>
#include <yl_messenger.h>
#include <yl_sensor.h>
#include <yl_value_data_point.h>
#include <yl_sensor.h>

//replace 2633 3539 with ur device id and sensor id
yl_device ardu(15305);
yl_sensor therm(26233, &ardu);
yl_sensor hum(384789, &ardu);

//replace first param value with ur u-apikey
yl_wifi_client client;
yl_messenger messenger(&client, "bb3f47349e887b7b6b08059a120cebe3", "api.yeelink.net");

int status = WL_IDLE_STATUS;
char ssid[] = "nanod";     //  your network SSID (name)
char pass[] = "12345678";  // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)

#include <dht22.h>
#define DHT22PIN 9
dht22 DHT22;

void setup()
{
  Serial.begin(115200); //for output information
  conectWifi();
}

int dht22_status; 
void loop()
{
    read_dht22();
    post_data2yeelink();
}

void post_data2yeelink()
{
    float humidity;
    float temperature;
    
    humidity = (float)DHT22.humidity / 10;
    temperature = (float)DHT22.temperature / 10;

    if (dht22_status == DHTLIB_OK)
    {
      yl_value_data_point dp(temperature);
      if (false == therm.single_post(messenger, dp))
      {
          Serial.println("request_post temperature error");     
      }
      else
          Serial.println("request_post temperature ok");
        
      delay(1000 * 10);
      
      yl_value_data_point dp1(humidity);
      if (false == hum.single_post(messenger, dp1))
      {
          Serial.println("request_post humidity error");     
      }
      else
          Serial.println("request_post humidity ok");
    }
}

void read_dht22()
{
    dht22_status = DHT22.read(DHT22PIN);
    
    switch (dht22_status)
    {
      case DHTLIB_OK: 
          float humidity;
          float temperature;
          humidity = (float)DHT22.humidity / 10;
          temperature = (float)DHT22.temperature / 10;
       
          Serial.print("DHT22  T:");
          Serial.print((float)temperature, 2);
          Serial.print("  H:");
          Serial.println((float)humidity, 2);
          break;
      case DHTLIB_ERROR_CHECKSUM: 
          Serial.println("DHT22 Read Checksum error"); 
          break;
      case DHTLIB_ERROR_TIMEOUT: 
          Serial.println("DHT22 Read time out error"); 
          break;
      default: 
          Serial.println("DHT22 Read Unknown error"); 
          break;
    }
}

void conectWifi()
{

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to wifi");
  printWifiStatus();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
