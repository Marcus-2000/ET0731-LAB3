#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Arduino.h> 
#include "ESP32_MailClient.h"

#define LED_PIN 2

// Defining the topics for the device to subscribe and publish to
#define aws_iot_sub_topic "fire-alarm/topic"
#define aws_iot_pub_topic "fire-alarm/fire_detected"

/* Change the following configuration options */
const char* ssid = "Marcus"; // SSID of WLAN
const char* password = "87654321";// password for WLAN access
const char* aws_iot_hostname = "a2hhmjt6d62qg5-ats.iot.us-east-1.amazonaws.com"; // Change Host name
// const char* aws_iot_sub_topic = "topic/hello"; //Change Subscription Topic name here
// const char* aws_iot_pub_topic = "another/topic/echo"; // Change publish topic name here
const char* aws_iot_pub_message = "Test.";
const char* aws_iot_pub_message2 = "There is a fire detected.";
const char* client_id = "MyIoT";

// change the ca certificate
const char* ca_certificate = "-----BEGIN CERTIFICATE-----\nMIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\nADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\nb24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\nMAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\nb3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\nca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\nIFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\nVOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\njgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\nAYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\nA4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\nU5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\nN+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\no/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\nrqXRfboQnoZsG4q5WTP468SQvvG5\n-----END CERTIFICATE-----";
// change the IOT device certificate
const char* iot_certificate = "-----BEGIN CERTIFICATE-----\nMIIDWTCCAkGgAwIBAgIUI2rsGx2fUS2mt8FjU/lc2XQqobcwDQYJKoZIhvcNAQEL\nBQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\nSW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDcyOTAyMDA0\nNloXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\nZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOqb7HgUzPk2cFzrnO9m\nqR2K7FuDN1vCE8dz0+/H8IaLk3+VRp9Ec1y6eGQKKWTpkZDnUqj8Tq+r5MXfDtxR\noYCfxjCQUpXrFzpQ5x4+/v4wgnYqK+5BnRPxjbvLdQHkd+kbmpcJKhXOOc7a17b2\nCae1y73QZmgH94s8B+ogOPgyWi2eWLfM/E9b4cAKt/b85c9EqWodaOFQw9sjOerK\nHu3NflVsDWcKy4YjJI5kqcpISX0Fn1e/VhpLZ2v0S+xoxP0Vnxz60Q1RDlblQyyk\ncFfUy6MLv9f6eHaLs6zO9lWlRv9QMGmLpJpRorZLWdRhaVon+zLYtnWItsrN3ddb\ntpUCAwEAAaNgMF4wHwYDVR0jBBgwFoAU1mhII5Ud9S/2y2JDDmm9f5j1pWowHQYD\nVR0OBBYEFC1sGMWjAur8pVRgrn8xnev2jnE+MAwGA1UdEwEB/wQCMAAwDgYDVR0P\nAQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQANvqMLeA1e5x7djMK4DniUE0TR\nmGMv78JSdLNbQCAzWreDHb1XEVAiSZ6rO8zc4cVc6XpJLJRuDKpiw1Tc78XE1DD2\nt0eXBivaxyQtvn32yCOgPJJfqh8rdAGHWaQgIuPOYme2jCPylnlcBqMcOhB0jWfy\n9SxDhV2ApAJbrZjQKXecmRJyCxu8HeGL/go9cD4gFKg3MfT8LMm7UpXbmnl21leq\nGm8JpQwbADI0yqwJB9ylCT7S8RxfqGUhobrIWy406t8OR+PRPGDTJ5qUSkOVnjWb\nOPNOW+yfkq+vUhGDxzwFX5rp76ixFgsXJdf9jKYcF602ZVWpVpPT88xDNth+\n-----END CERTIFICATE-----\n"; //Own certificate
// change to the private key of aws
const char* iot_privatekey = "-----BEGIN RSA PRIVATE KEY-----\nMIIEowIBAAKCAQEA6pvseBTM+TZwXOuc72apHYrsW4M3W8ITx3PT78fwhouTf5VG\nn0RzXLp4ZAopZOmRkOdSqPxOr6vkxd8O3FGhgJ/GMJBSlesXOlDnHj7+/jCCdior\n7kGdE/GNu8t1AeR36RualwkqFc45ztrXtvYJp7XLvdBmaAf3izwH6iA4+DJaLZ5Y\nt8z8T1vhwAq39vzlz0Spah1o4VDD2yM56soe7c1+VWwNZwrLhiMkjmSpykhJfQWf\nV79WGktna/RL7GjE/RWfHPrRDVEOVuVDLKRwV9TLowu/1/p4douzrM72VaVG/1Aw\naYukmlGitktZ1GFpWif7Mti2dYi2ys3d11u2lQIDAQABAoIBAFlYM/+u/DhUEQpx\n9/v7vp+vjj9zgxuAl4O3ipigLesvMGa/9Hsm2EASC1Mdw5HYmUTzszkDjPu9nIv2\nXHRu6fBP6GwKKklJlDjzInEUUIuA8c4KVVyQxD8aGH3SB6dG2Jt/lam7JbnUJZOv\nJ8NXg2nJA+QvfzBTCGSP4fu5dbieyaMLMcSU5RuL9nd2BLIS2gZKMeZ20ePjtOv6\ntsNoUtKYR/nZyr+IervGMByBskLGkF1BbDoGWCjoiEKidK7T+h1m4qxdCdunFATN\ncAB9sgezU7bkJrPudW49m3g1FFX3r0ROgZrwaofSrFaX6U9+OZDUpVimluCT7o/J\nlwcuQqkCgYEA+c/m08VlYK+6S4YjBRfYTF3Bh5WE5k7Rwhzb+Y6G5PCtkUz6n9Rn\nzUTsdmxg+uJh2Z1g1yW4tZ6MKUQ8/zYZNKmijUJ7L4RTY7bLPa39LbxG8rXUJ2m4\nYUwXbD7QLKWRLC76kes/EtLFPAwRBvdEFF81LvC4oZhwXHyGsu3Bb8sCgYEA8Gud\n/bKEEI0tfs35N2onU6l5M4HVP+8aQIAPyw6075LJc5tEeYTmYWYHETsqxqLhivmd\n/z3Lmtl9tSXHwl61D0aeIPr51Vj6OTO/9GgsMEYeMXG98uYM/C6g5qTyqIFTa5RP\nu3xpW0+mSeQDtxU/NgNbcLrfxgO8b69KEoNN5x8CgYBcEc3HGyeD/QAyOZvKksE7\nVAqxx9+ValNQsYY5cdgxpKqztKViJsyjENX9x2/ux2S86Y35fYOYHHkX9HhuryeX\nCzZHzcrM9n4tb/90Gh4Hry6O4YjxhbtY0MyJLpVW/yaEHERpZ0R10Yyz1tantJY4\n0+YsCLhjhmVTEOosJAh5BQKBgGdQp9U02KHQOWPLmbQhjwLZyjbDLWPLGUnsflzx\nVfm5544hhByAawwh7xmNhoVTy36QZEAiSxOhMO8ebChv0HjfDOb5U1FGakkYnv2l\noo+gIit3SHxQNfZNB2H2Mj0NTk7lrhnQjenqYXhRXvNXUxGgAbSA6dbLFjgcaMVp\n3ZlbAoGBAID11DRFY90MqIbKN2ie3lZwmkcVGf7urjoPMmzJwdXwMD679IS0OD4M\nj5SADOtrGfjJIfyU3xphjeIyz1yheN6JMQa0go7NDv2xt3MzZUFcw+xJNim2RUHS\n4/u4HxRRmthwxSLSXmBuscSYoTox2R2FkYoEuOT5jsAAIpi+s9qY\n-----END RSA PRIVATE KEY-----\n";

//WiFi or HTTP client for internet connection
HTTPClientESP32Ex http;

//The Email Sending data object contains config and data to send
SMTPData smtpData;

//Callback function to get the Email sending status
void sendCallback(SendStatus info);

#define SSID_HAS_PASSWORD //comment this line if your SSID does not have a password
/*Variables for sensor*/
int SmokeSensor = 34;
int SmokeSensorVal = 0;
int sensorThres = 300; //Smoke sensor threshold
int DataToSent = 0;

// Temperature
int TempSensor = 32;
float TempSensorVal = 0;
float Temp = 0;
int TempThres = 75;

// Buzzer
int buzzerPin = 14;

int j = 0;

/* Global Variables */
WiFiClientSecure client;
PubSubClient mqtt(client);

/* Functions */
void sub_callback(const char* topic, byte* payload, unsigned int length) {
  Serial.print("Topic: ");
  Serial.println(topic);

  Serial.print("Message: ");
  for (int i = 0; i < length; i++)
    Serial.print((char) payload[i]);
  Serial.println();

  if ((char) payload[0] == '1')
    digitalWrite(LED_PIN, HIGH);
  else if ((char) payload[0] == '0')
    digitalWrite(LED_PIN, LOW);

  mqtt.publish(aws_iot_pub_topic, aws_iot_pub_message);
}

void setup() {
  //Initializations
  Serial.begin(115200);
  Serial.print("Attempting WiFi connection on SSID: ");
  Serial.print(ssid);
  pinMode(LED_PIN, OUTPUT);
  ledcSetup(0,1E5,12);
  ledcAttachPin(14,0);
  digitalWrite(LED_PIN, LOW);

  WiFi.localIP();
  Serial.println(WiFi.localIP());
  
  // WiFi
  #ifdef SSID_HAS_PASSWORD
  WiFi.begin(ssid, password);
  #else
  WiFi.begin(ssid);
  #endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.print("\nWiFi connection succeeded.\n");

  client.setCACert(ca_certificate);
  client.setCertificate(iot_certificate);
  client.setPrivateKey(iot_privatekey);

  // AWS IoT MQTT uses port 8883
  mqtt.setServer(aws_iot_hostname, 8883);
  mqtt.setCallback(sub_callback);

  
}

void loop() {
  // reconnect on disconnect
  while (!mqtt.connected()) {
    Serial.print("Now connecting to AWS IoT: ");
    if (mqtt.connect(client_id)) {
      Serial.println("connected!");

      const char message3[25] = "{\"fire\": 1}";
      const char message4[25] = "{\"fire\": 0}";
      
      while(1) {         // while loop to repeatedly send data to AWS
        //Smoke detector portion
        SmokeSensorVal = analogRead(SmokeSensor);
        
        //LM35 portion
        TempSensorVal = analogRead(TempSensor);
        
        Temp = (100 * TempSensorVal) / 1023;

        Serial.print("Temperature is ");
        Serial.println(Temp);
        Serial.print("Smoke is ");
        Serial.println(SmokeSensorVal);

        
        if ((SmokeSensorVal > sensorThres) || (Temp > TempThres)) {
          j++;
        } else {
          j = 0;
        }

        if (j == 1) {
          mqtt.publish(aws_iot_pub_topic, message3); // Sending message to specific topic 
          ledcWriteTone(0,200);
          delay(5000);
        } else if(j == 0) {
          mqtt.publish(aws_iot_pub_topic, message4); // Sending message to specific topic
          ledcWriteTone(0,0);
        }

        delay(20000);
    }
  } else {
      Serial.print("failed with status code ");
      Serial.print(mqtt.state());
      Serial.println(" trying again in 5 seconds...");
      delay(5000);

  mqtt.loop();
}
  }
}
