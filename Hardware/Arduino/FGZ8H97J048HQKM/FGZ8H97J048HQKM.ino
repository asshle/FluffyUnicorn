#include <LiquidCrystal.h>

int battPin = A0;

LiquidCrystal lcd(8,9,10,11,12,13);

void setup() 
{
  lcd.begin(16,2);
  pinMode(battPin, INPUT);
}

void loop() 
{
   int voltReading = analogRead(battPin);
   int volts = (voltReading/1023)*100;
   if (volts == 100)
   {
   lcd.setCursor(12,0);
   lcd.print(volts);
   lcd.print("%");
   delay(1000);
   lcd.clear();
   }
   else if (volts < 10)
   {
   lcd.setCursor(14,0);
   lcd.print(volts);
   lcd.print("%");
   delay(1000);
   lcd.clear();
   }
   else
   {
   lcd.setCursor(13,0);
   lcd.print(volts);
   lcd.print("%");
   delay(1000);
   lcd.clear();
   }
}
