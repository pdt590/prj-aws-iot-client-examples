/* ESP8266 AWS IoT
 *  
 * Simplest possible example (that I could come up with) of using an ESP8266 with AWS IoT.
 * No messing with openssl or spiffs just regular pubsub and certificates in string constants
 * 
 * This is working as at 7th Aug 2021 with the current ESP8266 Arduino core release 3.0.2
 * 
 * Author: Anthony Elder 
 * License: Apache License v2
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "MartinRouterKing";
const char* password = "Minhnam183";

// Find this awsEndpoint in the AWS Console: Manage - Things, choose your thing
// choose Interact, its the HTTPS Rest endpoint 
const char* awsEndpoint = "xxxxxx48r6u6k-ats.iot.ap-southeast-1.amazonaws.com";

// For the two certificate strings below paste in the text of your AWS 
// device certificate and private key:

// xxxxxxxxxx-certificate.pem.crt
static const char certificatePemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUGM7ITh10LkOkQZrDLQ5liTpg/EswDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTIxMDkzMDEyNTUz
OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKNTK+rwIwcYDIZuWZD2
s+8QkMuPvfopGS/W+Ffp0d3WaCK8cNxD0G1MfzcCXD97kqxNn16DTTK8uS4wv8CZ
6EssJ1EO1A0Ja8fALFMgcxu78lsDYNChjJk3qs5mV5K4tccVOdcSIzbaFYyI41PJ
cYemOux5WAIXjXNUq4kSlohJsRp/9SN/1e+ECSu3uUkLjAcC8HQr3eipZq5EPnCv
xwHgMpP1Lzv1zsBPNW9J9SlzMSQ3dqGx3MMN2U+cqRV+OaA9Ogt6Jgm1Un1XcdXa
XEP4ynMaZ2GrQu3iwyKJyIoZmw6sd9xWxPpnkCX9xX9trqvEoI+XZ8KkxnBb8/St
TG0CAwEAAaNgMF4wHwYDVR0jBBgwFoAU581W/dRv3JMoX155xBBuBuCtiuUwHQYD
VR0OBBYEFId8Pao2fdZGRrrpiDFPwhBZdVpLMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCa2pKhlSt4G0IKjTA65wzEMSIv
qb+TQcfsHSLiruO12iMa9XAijK7VjzJb+7vBZ+HpdNUK0O0sp8S4wC8IHDzUnEkl
2H9/O7nDAjeoa47LE95DHlMGLuRFI9CCl9PcINUhaJd1EHEJSV717wZFQZ63SafX
RbypVvBAoLYhsQAq+xKBacUz1Kj5j8rPq6fr63AOmpEOjx9n4/YOLHLf2P6ft0o4
ta0ArESA0vMYnoYqsZ6AlfBwcPTFHKV7sYgePcuKPLjEUpgViqFMxQ+TRonAks2u
7CdLlmzaiFD2xHoyR/ghiUuHEfp8FsSiCoYFrjT4r69wDISIUOe6hF4wtEO9
-----END CERTIFICATE-----
)EOF";

// xxxxxxxxxx-private.pem.key
static const char privatePemKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAo1Mr6vAjBxgMhm5ZkPaz7xCQy4+9+ikZL9b4V+nR3dZoIrxw
3EPQbUx/NwJcP3uSrE2fXoNNMry5LjC/wJnoSywnUQ7UDQlrx8AsUyBzG7vyWwNg
0KGMmTeqzmZXkri1xxU51xIjNtoVjIjjU8lxh6Y67HlYAheNc1SriRKWiEmxGn/1
I3/V74QJK7e5SQuMBwLwdCvd6KlmrkQ+cK/HAeAyk/UvO/XOwE81b0n1KXMxJDd2
obHcww3ZT5ypFX45oD06C3omCbVSfVdx1dpcQ/jKcxpnYatC7eLDIonIihmbDqx3
3FbE+meQJf3Ff22uq8Sgj5dnwqTGcFvz9K1MbQIDAQABAoIBAB0Xwaj14adv1ztT
CM1TxLV/A+vezP50t7HX3yD525JikOYvHwN9aPEvQTHxT7FXN8dNhGf3h5bUG0Bp
Gp8aS1CoqtA7jfZMiVBpoPuluhtknUUtEt/XBw575gciH9NY57hwxX7UEkXBDzmb
YLMaHQkGyVQ6QT0OKPo0SOH/16SrAt1nqhxL1/FBMzCvqQHU0qlqX38PoQ+jt/nS
VXhoIYmtcoQxomoAffVS7gG7qFg+GVqoGjvEo3Keyt9Uoy56aPZnR9v1ayFY/34Z
gSsvkSiH2Awz5UCfeSS2D25okCu76JrRR8DGirBI22F58c2VivG0kiJHOFrBNmYZ
Qgdjw8ECgYEA1ludGBVUVWdH4IsDIiVxnN90XBLG93TirTeWK5vGZOiVSqM8a6XO
a6dDxcZxIQVTTP08Ards0fl6j1E6V6w9uDeZ9RelAye8qjyGUZM00cDqW+THikMs
rjM8xpXaeMwiQcrlxKHNakkFTWhtPZsKPrP0Tq7jWH6R+Q4rPfBOW3kCgYEAww2Z
llAiMuxLUujVW/EUmDjyKVXKeRYIyIRbX2RK7MhPEXtaoY+C64At95rziV17NnUb
AtoF51KKAJKMMPm6y1Xy6PRrJq56omVfavq/PADzfYKz2Jch3ra39xnSCNEhZraZ
bQ3tJK92yy0F3f77FlalTGu0OJ/sAaI7Dwjvx5UCgYEAmP9w5DStKfv6wJlYFjI1
omuXyPQaEZvbAxmu1Ndtxq5/iHN0d70z1YDgjbRzIMeFSmlhQOsc/84anVqz0sn8
j5UYDWjf/Z2KOtPSZz0zsRcJ76ceiWAi7v9hPLjO4/OH6//e+fcm/9q4ryR+/D7C
FDkrglGvsYU9uU9Uq/PuabkCgYAu+g+LjDEwYoihp0H7qsxy/QQVwHvLPetPn9EB
4H3LGVMxuqvRTWcSoQRxJfiRlNof+4RN44SYeeCS0rjA8uRyJHkkcfl8j3kps5CP
i2STZwc9q0T1XTDjGs6jzKwxQ2zv9/prsXbz5u6NcIKBPO/sKWIWTbpSCPImUpQ2
wsg9IQKBgE578i904rRKF8TeK5J9Cw4CVKmokMyvzyraIMNBEZf8DtYckC5PQO7o
tK8g0CsRyPZK6e4VB8HJrH5/2aUfiKI9t1TB4rvwzlUE4entJkNfJddaBKZlTcFw
kJuCNhQXJM6garaRkZtfn0yxMzjzL+QyP0OUBLRXITk0qALWEPJx
-----END RSA PRIVATE KEY-----
)EOF";

// This is the AWS IoT CA Certificate from: 
// https://docs.aws.amazon.com/iot/latest/developerguide/managing-device-certs.html#server-authentication
// This one in here is the 'RSA 2048 bit key: Amazon Root CA 1' which is valid 
// until January 16, 2038 so unless it gets revoked you can leave this as is:
static const char caPemCrt[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

BearSSL::X509List client_crt(certificatePemCrt);
BearSSL::PrivateKey client_key(privatePemKey);
BearSSL::X509List rootCert(caPemCrt);

void setCurrentTime(void);
void pubSubCheckConnect(void);

WiFiClientSecure wiFiClient;
void msgReceived(char* topic, byte* payload, unsigned int len);
PubSubClient pubSubClient(awsEndpoint, 8883, msgReceived, wiFiClient); 

void setup() {
  Serial.begin(9600); Serial.println();
  Serial.println("ESP8266 AWS IoT Example");

  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  WiFi.waitForConnectResult();
  Serial.print(", WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  // get current time, otherwise certificates are flagged as expired
  setCurrentTime();

  wiFiClient.setClientRSACert(&client_crt, &client_key);
  wiFiClient.setTrustAnchors(&rootCert);
}

unsigned long lastPublish;
int msgCount;

void loop() {

  pubSubCheckConnect();

  if (millis() - lastPublish > 10000) {
    String msg = String("Hello from ESP8266: ") + ++msgCount;
    pubSubClient.publish("outTopic", msg.c_str());
    Serial.print("Published: "); Serial.println(msg);
    lastPublish = millis();
  }
}

void msgReceived(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on "); Serial.print(topic); Serial.print(": ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void pubSubCheckConnect() {
  if ( ! pubSubClient.connected()) {
    Serial.print("PubSubClient connecting to: "); Serial.print(awsEndpoint);
    while ( ! pubSubClient.connected()) {
      Serial.print(".");
      pubSubClient.connect("ESP8266thing");
    }
    Serial.println(" connected");
    pubSubClient.subscribe("inTopic");
  }
  pubSubClient.loop();
}

void setCurrentTime() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: "); Serial.print(asctime(&timeinfo));
}