#include <Arduino.h>

#include <AWS_IOT.h>
#include <WiFi.h>

AWS_IOT hornbill;

char WIFI_SSID[] = "your-wifi-ssid";
char WIFI_PASSWORD[] = "your-wifi-password";
char HOST_ADDRESS[] = "a1zsrr48r6u6k-ats.iot.ap-southeast-1.amazonaws.com";
char CLIENT_ID[] = "esp32_hello_world";
char TOPIC_NAME[] = "$aws/things/esp32_hello_world/shadow/update";

int status = WL_IDLE_STATUS;
int tick = 0, msgCount = 0, msgReceived = 0;
char payload[512];
char rcvdPayload[512];

void mySubCallBackHandler(char *topicName, int payloadLen, char *payLoad)
{
  strncpy(rcvdPayload, payLoad, payloadLen);
  rcvdPayload[payloadLen] = 0;
  msgReceived = 1;
}

void setup()
{
  Serial.begin(9600);
  delay(2000);

  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // wait 5 seconds for connection:
    delay(5000);
  }

  Serial.println("Connected to wifi");

  if (hornbill.connect(HOST_ADDRESS, CLIENT_ID) == 0)
  {
    Serial.println("Connected to AWS");
    delay(1000);

    if (0 == hornbill.subscribe(TOPIC_NAME, mySubCallBackHandler))
    {
      Serial.println("Subscribe Successfull");
    }
    else
    {
      Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
      while (1)
        ;
    }
  }
  else
  {
    Serial.println("AWS connection failed, Check the HOST Address");
    while (1)
      ;
  }

  delay(2000);
}

void loop()
{

  if (msgReceived == 1)
  {
    msgReceived = 0;
    Serial.print("Received Message:");
    Serial.println(rcvdPayload);
  }
  if (tick >= 5) // publish to topic every 5seconds
  {
    tick = 0;
    sprintf(payload,"{\"state\":{\"reported\":{\"counter\":\"%d\"}}}", msgCount++);
    if (hornbill.publish(TOPIC_NAME, payload) == 0)
    {
      Serial.print("Publish Message:");
      Serial.println(payload);
    }
    else
    {
      Serial.println("Publish failed");
    }
  }
  vTaskDelay(1000 / portTICK_RATE_MS);
  tick++;
}