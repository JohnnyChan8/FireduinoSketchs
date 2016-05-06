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
yl_sensor myswitch(26214, &ardu);
//replace first param value with ur u-apikey
yl_wifi_client client;
yl_messenger messenger(&client, "bb3f47349e887b7b6b08059a120cebe3", "api.yeelink.net");

int status = WL_IDLE_STATUS;
char ssid[] = "nanod";     //  your network SSID (name)
char pass[] = "12345678";  // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int switch_pin = 5;
int led_pin = 3;
int led_status = 0;

void setup()
{
  Serial.begin(115200); //for output information
  pinMode(switch_pin, OUTPUT);
  digitalWrite(switch_pin, HIGH);
  
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, HIGH);

  conectWifi();
}

void loop()
{
    updateYeelinkSwitch();
    delay(1000);
    if (led_status == 1)
      led_status = 0;
    else 
      led_status = 1;
    
    digitalWrite(led_pin, led_status);
}

int lastStatus = 3;
void updateYeelinkSwitch()
{
  yl_value_data_point dp2;

  Serial.println("enter updateYeelinkSwitch"); 
  
    int ret = myswitch.single_get(messenger, dp2);

  if (ret == true)
  {
    int curStatus = dp2.get_value();
    if (curStatus != lastStatus)
    {
      if (curStatus == 0)
      {
          Serial.println("turn off the LED"); 
          digitalWrite(switch_pin, HIGH);
      }
      else if (curStatus == 1)
      {
          Serial.println("turn on the LED"); 
          digitalWrite(switch_pin, LOW);
          delay(1000 * 3);
          digitalWrite(switch_pin, HIGH);
          Serial.println("turn off the LED"); 
          dp2.set_value(0);
          myswitch.single_post(messenger, dp2);
          Serial.println("post data ."); 
      }
      lastStatus = curStatus;
    }
  }
  
  Serial.println("exit updateYeelinkSwitch"); 
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
