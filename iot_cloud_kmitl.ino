// ArduinoIoTCloud - Version: Latest 
#include <ArduinoIoTCloud.h>

// ESP Line Notify - Version: Latest 
#include <ESP_Line_Notify.h>
#include <ESP_Line_Notify_Const.h>

#include <ESP_Line_Notify_Error.h>
#include <ESP_Line_Notify_Net.h>
#include <ESP_Line_Notify_Utils.h>
#include <FS_Config.h>
#include <SDHelper.h>

#include <DHT.h>
#include <DHT_U.h>

const int ledPin = 5;
const int buzzerPin = 4;
const int dustSensorPin = A2;
const float dustThreshold = 0.5;

// Adafruit HMC5883 Unified - Version: Latest 
#include <Adafruit_HMC5883_U.h>

#include "thingProperties.h"
#define DHTPIN 3  //DHT digital pin2
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE);

LineNotifyClient line;
void sendingCallback(LineNotifySendingResult result);

const int AOUTpin=A1;//the AOUT pin of the CO sensor goes into analog pin A0 of the arduino
int value;


void setup() {
  Serial.begin(115200);
  delay(1500);
  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  
  line.token = "XHFuTCh9dOZxsVunZcMk1jGDALyAr8zCBAPG6MRSepI";
  LineNotifySendingResult result = LineNotify.send(line);
  
  dht.begin();
  pinMode(buzzerPin, OUTPUT); // Set buzzer pin as OUTPUT
  pinMode(ledPin, OUTPUT);
  
  Serial.println();
  // Serial.println(LINE.getVersion());
  // LINE.setToken("lXOc3YnYtN1Ta3EXO8S1c9YHXQmSmrRFKrl8EtJsbuA");
  
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
  
  onDustChange();
  onTempChange();
  onHumendityChange();
  onCoChange();
  
  line.message = "Hello world";
  delay(500);
//  LineNotify();
}


/*
  Since Temp is READ_WRITE variable, onTempChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onTempChange()  {
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  temp = t;
}

void onHumendityChange() {
  float h = dht.readHumidity();
  Serial.print(F("Humidity: "));
  Serial.print(h);
  humendity = h;
}
/*
  Since Co3 is READ_WRITE variable, onCo3Change() is
  executed every time a new value is received from IoT Cloud.
*/
void onCoChange() {
  Serial.println(value);
  value= analogRead(AOUTpin);//reads the analaog value from the CO sensor's AOUT pin
  float voltage = value * (5.0 / 1023.0);
  float ppm = (voltage - 0.4) * 100 / 0.6;
  co = ppm;
  
  Serial.print("CO Concentration: ");
  Serial.print(ppm);
  Serial.println(" ppm");

   if (ppm >= 400) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
    delay(1500); 
    digitalWrite(buzzerPin, LOW);
  } else {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }
  
  delay(500);
}

void onDustChange() {
    int sensorValue = analogRead(dustSensorPin);
    
  // Calculate voltage (adjust Vref if necessary)
  float voltage = sensorValue * (5.0 / 1023.0);

  // Calculate dust density using a linear equation (adjust parameters based on calibration)
  float dustDensity = 0.17 * voltage - 0.1;
  if (dustDensity > dustThreshold) {
    // Sound the buzzer
    digitalWrite(buzzerPin, HIGH);
  } else {
    // Silence the buzzer
    digitalWrite(buzzerPin, LOW);
  }
  
  // Print the dust density value
  Serial.print("Dust Density: ");
  Serial.print(dustDensity);
  Serial.println(" pcs/0.01cf");
  dust = dustDensity;
  // Wait for some time before the next reading
  delay(500);
}

//void LineNotify() {
//  
//  if(value >= 90) {
//    LINE.notifyPicture("");
//    LINE.notify( "CO3:Over default");
//    delay(6000);
//  }
  
//}


}
