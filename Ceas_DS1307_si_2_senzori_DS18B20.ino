
   //+++++++++++++++++++++++++++++++++++++++++++++++//
   //                                               //
   //   Ceas cu RTC DS 1307, LCD 20x2 si doua zone  //
   //     de temperatura, interior si exterior      //
   //        citite cu 2 senzori DS 18B20           //
   //        Versiunea V-1, GeoMar - 2o19           //
   //                                               //
   //+++++++++++++++++++++++++++++++++++++++++++++++//


#include <LiquidCrystal.h>                   //Biblioteca LCD
#include <Wire.h>                            //Biblioteca WIRE
#include "RTClib.h"                          //Biblioteca RTC-Ceas
#include <DallasTemperature.h>               //Biblioteca Senzor DS18B20
#include <OneWire.h>                         //Biblioteca OneWire

#define ONE_WIRE_BUS A3    // senzorii DS18B20 sunt conectati la pinul A3
#define precizie 9         // setare precizie 9 biti senzori DS

//-- variabila pentru afisare luna -----------------------------------------
const char* Months[] =
{"Dec", "Ian", "Feb", "Mar", "Apr", "Mai", "Iun", "Iul", "Aug", "Sep", "Oct", "Nov" };

//--- creare caracter termometru pentru afisare ----------------------------
byte termometru  [8] = {B00100, B01010, B01010, B01010, B01110, B11111, B11111, B01110};

//--- temperaturi de la senzorii DS 18B20 ----------------------------------
float tempC1;
float tempC2;

// ---- Variabile pentru meniu Ceas ----------------------------------------
int ora;
int minut;
int secunda;

// ---- variabile necesare pentru schimbare ciclica meniuri: Ceas -> Temp Int -> Temp Ext -----
int interval = 3000;              // intervalul la care se schimba meniurile (setat la 3 secunde)
unsigned long previousMillis = 0;
unsigned long currentMillis ;

//--- setare meniu principal & Butoane -------------------------------------
// variabile necesare pentru definire meniu principal si butoane
char meniu = 1;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensors1, sensors2;

//---- Definire pini pentru conectare LCD ----------------------------------
LiquidCrystal lcd (7,  //RS no digital 4
                   6,  //EN no digital 6
                   5,  //D4 no digital 11
                   4,  //D5 no digital 12
                   3,  //D6 no digital 13
                   2); //D7 no digital 14

RTC_DS1307 RTC;    //Tip de DS folosit la RTC - ceas

// -------------------------------------------------------------------------
void setup ()
{
  lcd.begin(20, 4);    // Initializare tip LCD 20x4
  RTC.begin();
  Wire.begin();

  lcd.createChar(1, termometru);   // creare caracter termomentru pentru afisare pe LCD

  //---- Citire valori din DS1307 pentru initializare ceas   ------------------
  DateTime now = RTC.now();
  now = RTC.now();
  ora = now.hour();
  minut = now.minute();
  secunda = now.second();

  if (! RTC.isrunning()) {
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  sensors.begin();
  // setare rezolutie 9 biti pentru DS-uri
  sensors.setResolution(sensors1, precizie);
  sensors.setResolution(sensors2, precizie);

  if (!sensors.getAddress(sensors1, 0));
  if (!sensors.getAddress(sensors2, 0));

}

// ---------------------------------------------------------------------------
void loop()
{
  Afisaremeniu();
  if (meniu < 4)meniuauto();
}

// --------------------------------------------------------------------------
void meniuauto()
{
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval)
  {
    lcd.clear();
    meniu ++;
    previousMillis = currentMillis;
    if (meniu > 3) {
      meniu = 1;
    }
  }
}

// -------------------------------------------------------------------------
void Afisaremeniu() {

  switch (meniu) {

    case 1:
      ceas();
      break;
    case 2:
      temperaturainterior();
      break;
    case 3:
      temperaturaexterior();
      break;
  }
}


// ---------------------------------------------------------------------------
void ceas()
{
  DateTime now = RTC.now();
  now = RTC.now();
  ora = now.hour();
  minut = now.minute();
  secunda = now.second();
  lcd.setCursor(0, 0);
  if (now.day() < 10)
    lcd.print(' ');
  lcd.print(now.day(), DEC);
  lcd.print('-');
  //lcd.print(now.month(), DEC);
  lcd.print(Months[now.month()]);
  lcd.print('-');
  lcd.print(now.year(), DEC);
  lcd.print(' ');
  lcd.setCursor(0, 1);
  lcd.print("--------------------");
  lcd.setCursor(0, 2);
  lcd.print("|");
  lcd.setCursor(6, 2);
  if (now.hour() < 10)
    lcd.print('0');
  lcd.print(ora);
  lcd.print(':');
  if (now.minute() < 10)
    lcd.print('0');
  lcd.print(minut);
  lcd.print(':');
  if (now.second() < 10)
    lcd.print('0');
  lcd.print(secunda);
  lcd.print(' ');
  lcd.setCursor(19, 2);
  lcd.print("|");
  lcd.setCursor(0, 3);
  lcd.print("--------------------");

  lcd.setCursor(12, 0);
  int dayofweek = now.dayOfTheWeek();
  switch (dayofweek) {
    case 1:
      lcd.print("   Luni");
      break;
    case 2:
      lcd.print("  Marti");
      break;
    case 3:
      lcd.print("Miercuri");
      break;
    case 4:
      lcd.print("   Joi");
      break;
    case 5:
      lcd.print(" Vineri");
      break;
    case 6:
      lcd.print(" Sambata");
      break;
    case 0:
      lcd.print("Duminica");
      break;
  }
}

// ---------------------------------------------------------------------------
void temperaturainterior()
{ sensors.requestTemperatures();
  tempC1 = sensors.getTempC(sensors1);
  lcd.setCursor(0, 0);
  lcd.print("Afisare  Temperatura");
  lcd.setCursor(0, 1);
  lcd.print("- senzor DS Interior");
  lcd.setCursor(0, 3);
  lcd.print("- Temp int ");
  lcd.write(1);
  lcd.setCursor(13, 3);
  lcd.print(tempC1, 1);
  lcd.setCursor(18, 3);
  lcd.write(0b11011111);
  lcd.print("C");
}

// ---------------------------------------------------------------------------
void temperaturaexterior()
{ sensors.requestTemperatures();
  tempC2 = sensors.getTempC(sensors2);
  lcd.setCursor(0, 0);
  lcd.print("Afisare  Temperatura");
  lcd.setCursor(0, 1);
  lcd.print("- senzor DS Exterior");
  lcd.setCursor(0, 3);
  lcd.print("- Temp ext ");
  lcd.write(1);
  lcd.setCursor(13, 3);
  lcd.print(tempC2, 1);
  lcd.setCursor(18, 3);
  lcd.write(0b11011111);
  lcd.print("C");
}

// --------------------- SFARSITUL PROGRAMULUI -------------------------------
