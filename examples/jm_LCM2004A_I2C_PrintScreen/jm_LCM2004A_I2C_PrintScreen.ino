#define __PROG__ "jm_LCM2004A_I2C_PrintScreen"

#include <Arduino.h>
#include <Wire.h>
#include <jm_LCM2004A_I2C.h>

//jm_LCM2004A_I2C lcd(0x3F); // LCM2004A
//jm_LCM2004A_I2C lcd(0x27); // LCM2004
jm_LCM2004A_I2C lcd; // LCM2004A|LCM2004 auto

void setup()
{
	Serial.begin(115200);
	delay(3000); // wait for USB Serial ready

	Serial.println();
	Serial.println(F(__PROG__ "..."));

	Wire.begin();
	lcd.begin();
	delay(1000);

#if 0
	lcd.backlight(!lcd.backlight());
	delay(1000);
	lcd.backlight(!lcd.backlight());
	delay(1000);
#else
//	lcd.pcf8574().pin_toggle(jm_PCF8574::mask_pin(LCM2004A_I2C_BL));
	lcd.pcf8574().pin_toggle(lcd.pcf8574().mask_pin(LCM2004A_I2C_BL));
	delay(1000);
//	lcd.pcf8574().pin_toggle(jm_PCF8574::mask_pin(LCM2004A_I2C_BL));
	lcd.pcf8574().pin_toggle(lcd.pcf8574().mask_pin(LCM2004A_I2C_BL));
	delay(1000);
#endif

	// write "Hello World!" 4 times...
	lcd.set_cursor(0, 0);
	lcd.print("Hello World!");
	lcd.set_cursor(2, 1);
	lcd.print("Hello World!");
	lcd.set_cursor(4, 2);
	lcd.print("Hello World!");
	lcd.set_cursor(6, 3);
	lcd.print("Hello World!");
	delay(1000);

	// read rows from lcd and print it...
	Serial.println("PrintScreen:");
	for (int row=0; row<4; row++) {
		lcd.set_cursor(0, row);
		Serial.print(row);
		Serial.print(':');
		for (int col=0; col<20; col++) {
			Serial.print((char) lcd.read());
		}
		Serial.println();
	}
	Serial.println("done.");
}

void loop()
{
}

