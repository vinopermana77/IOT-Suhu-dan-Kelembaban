#define BLYNK_TEMPLATE_ID "TMPLBpu3l6CU"
#define BLYNK_DEVICE_NAME "Monitoring Kandang Reptil"
#define BLYNK_AUTH_TOKEN "6H4-aPl6OW1dBKjW6p2CDYsvXl5c8qnI"
#define ON_Board_LED 2
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

#define DHTTYPE DHT11
#include <Fuzzy.h>

//WiFi Rumah
//char auth[] = BLYNK_AUTH_TOKEN;
//char ssid[] = "Jarvis";  
//char pass[] = "1sampai8";

//WiFi HP
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Redmi 6A";  
char pass[] = "77sampai77";

// Set Type
#define DHTPIN D3         
#define DHTTYPE DHT11     
#define RelayPin 16      

// Initialize
Fuzzy *fuzzy = new Fuzzy();
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

WidgetLCD vlcd2(V3);
WidgetLCD vlcd(V0);


void setup() {
  
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(RelayPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  dht.begin();  
  timer.setInterval(2500L, sendSensor);

// Fuzziifikasi
// fuzzy input suhu
FuzzySet *dingin  = new FuzzySet(0, 0, 25, 29);
FuzzySet *normal  = new FuzzySet(25, 29, 31, 35);
FuzzySet *panas   = new FuzzySet(31, 35, 60, 60);

// fuzzy input kelembaban
FuzzySet *kering  = new FuzzySet(0, 0, 20, 40);
FuzzySet *sedang  = new FuzzySet(30, 40, 60, 70);
FuzzySet *lembab  = new FuzzySet(60, 80, 100, 100);

// fuzzy output relay
FuzzySet *high  = new FuzzySet(0, 0, 30, 70);
FuzzySet *low  = new FuzzySet(30, 70, 100, 100);

//  fuzzy input
FuzzyInput *suhu = new FuzzyInput(1);

suhu->addFuzzySet(dingin);
suhu->addFuzzySet(normal);
suhu->addFuzzySet(panas);
fuzzy->addFuzzyInput(suhu);

FuzzyInput *kelembaban = new FuzzyInput(2);

kelembaban->addFuzzySet(kering);
kelembaban->addFuzzySet(sedang);
kelembaban->addFuzzySet(lembab);
fuzzy->addFuzzyInput(kelembaban);

//output
FuzzyOutput *relay = new FuzzyOutput(1);

relay->addFuzzySet(low);
relay->addFuzzySet(high);
fuzzy->addFuzzyOutput(relay);

//fuzzy rule 1 
FuzzyRuleAntecedent *dingin_kering = new FuzzyRuleAntecedent();
dingin_kering->joinWithAND(dingin, kering);

FuzzyRuleConsequent *relay_low1 = new FuzzyRuleConsequent();
relay_low1->addOutput(low);

FuzzyRule *fuzzyRule1 = new FuzzyRule(1, dingin_kering, relay_low1);
fuzzy->addFuzzyRule(fuzzyRule1);

//fuzzy rule 2
FuzzyRuleAntecedent *dingin_sedang = new FuzzyRuleAntecedent();
dingin_sedang->joinWithAND(dingin, sedang);

FuzzyRuleConsequent *relay_low2 = new FuzzyRuleConsequent();
relay_low2->addOutput(low);

FuzzyRule *fuzzyRule2 = new FuzzyRule(2, dingin_sedang, relay_low2);
fuzzy->addFuzzyRule(fuzzyRule2);

//fuzzy rule 3
FuzzyRuleAntecedent *dingin_lembab = new FuzzyRuleAntecedent();
dingin_lembab->joinWithAND(dingin, lembab);

FuzzyRuleConsequent *relay_low3 = new FuzzyRuleConsequent();
relay_low3->addOutput(low);

FuzzyRule *fuzzyRule3 = new FuzzyRule(3, dingin_lembab, relay_low3);
fuzzy->addFuzzyRule(fuzzyRule3);

//fuzzy rule 4
FuzzyRuleAntecedent *normal_kering = new FuzzyRuleAntecedent();
normal_kering->joinWithAND(normal, kering);

FuzzyRuleConsequent *relay_high4 = new FuzzyRuleConsequent();
relay_high4->addOutput(high);

FuzzyRule *fuzzyRule4 = new FuzzyRule(4, normal_kering, relay_high4);
fuzzy->addFuzzyRule(fuzzyRule4);

//fuzzy rule 5
FuzzyRuleAntecedent *normal_sedang = new FuzzyRuleAntecedent();
normal_sedang->joinWithAND(normal, sedang);

FuzzyRuleConsequent *relay_high5 = new FuzzyRuleConsequent();
relay_high5->addOutput(high);

FuzzyRule *fuzzyRule5 = new FuzzyRule(5, normal_sedang, relay_high5);
fuzzy->addFuzzyRule(fuzzyRule5);

//fuzzy rule 6
FuzzyRuleAntecedent *normal_lembab = new FuzzyRuleAntecedent();
normal_lembab->joinWithAND(normal, lembab);

FuzzyRuleConsequent *relay_high6 = new FuzzyRuleConsequent();
relay_high6->addOutput(high);

FuzzyRule *fuzzyRule6 = new FuzzyRule(6, normal_lembab, relay_high6);
fuzzy->addFuzzyRule(fuzzyRule6);

//fuzzy rule 7
FuzzyRuleAntecedent *panas_kering = new FuzzyRuleAntecedent();
panas_kering->joinWithAND(panas, kering);

FuzzyRuleConsequent *relay_high7 = new FuzzyRuleConsequent();
relay_high7->addOutput(high);

FuzzyRule *fuzzyRule7 = new FuzzyRule(7, panas_kering, relay_high7);
fuzzy->addFuzzyRule(fuzzyRule7);

//fuzzy rule 8
FuzzyRuleAntecedent *panas_sedang = new FuzzyRuleAntecedent();
panas_sedang->joinWithAND(panas, sedang);

FuzzyRuleConsequent *relay_high8 = new FuzzyRuleConsequent();
relay_high8->addOutput(high);

FuzzyRule *fuzzyRule8 = new FuzzyRule(8, panas_sedang, relay_high8);
fuzzy->addFuzzyRule(fuzzyRule8);

//fuzzy rule 9
FuzzyRuleAntecedent *panas_lembab = new FuzzyRuleAntecedent();
panas_lembab->joinWithAND(panas, lembab);

FuzzyRuleConsequent *relay_high9 = new FuzzyRuleConsequent();
relay_high9->addOutput(high);

FuzzyRule *fuzzyRule9 = new FuzzyRule(9, panas_lembab, relay_high9);
fuzzy->addFuzzyRule(fuzzyRule9);
}

void sendSensor(){
  int h = dht.readHumidity();
  float t = dht.readTemperature(); 
  if (isnan(h) || isnan(t)) {
    Serial.println("Sensor DHT11 tidak terbaca!");
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print(" C");
  lcd.setCursor(0, 1);
  lcd.print("Humidity: ");
  lcd.print(h);
  lcd.print(" %");
  
  // Defuzzify
  fuzzy->setInput(1, t);
  fuzzy->setInput(2, h);
  fuzzy->fuzzify();

  float output = fuzzy->defuzzify(1);

  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V2, h);

  
  Serial.print("Temp: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Nilai Output: ");
  Serial.println(output);
  Serial.println("-------------");

  //Kondisi Suhu
  if(t >= 34){
  vlcd.print(0,0,"Temp    :  Panas");
  }else if ( t >= 27){ 
  vlcd.print(0,0,"Temp    : Normal");
  }else if (t = 26){
  vlcd.print(0,0,"Temp    : Dingin");
  }

  //Kondisi Kelembaban
  if(h >= 80){
  vlcd.print(0,1,"Humidity: Lembab");
  }else if ( h >= 60){ 
  vlcd.print(0,1,"Humidity: Normal");
  }else if ( h <= 59){
  vlcd.print(0,1,"Humidity: Kering");
  }

  //Kondisi Lampu
  if(t >= 28){
  vlcd2.print(0,0,"Lampu : OFF");
  digitalWrite(RelayPin, HIGH);
  }
  else if (t <= 26){
  vlcd2.print(0,0,"Lampu :  ON");
  digitalWrite(RelayPin, LOW);
  }
  
  
}

void loop() 
{
  Blynk.run();
  timer.run();
}
