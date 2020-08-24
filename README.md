# jm_LCM2004A_I2C Arduino Library

2020-08-25: v1.0.2 - Added compatibility with LiquidCrystal library.  
2020-05-11: v1.0.1 - Checked with architectures AVR, SAM and ESP32.  
2020-05-04: v1.0.0 - Initial commit.  



### LCM2004A LCD Display Module interfaced for I2C

##### LCM2004A models

- https://www.banggood.com/search/i2c-2004-204-20-x-4-character-lcd-display-module.html?from=nav
- https://ch.farnell.com/powertip/pc2004ars-awa-a-q/lcd-modul-stn-reflect-20x4/dp/1671507

##### LCM2004A datasheets

- https://www.beta-estore.com/download/rk/RK-10290_410.pdf
- http://www.farnell.com/datasheets/50586.pdf

##### LCM2004A schematic

- https://www.mpja.com/download/35054opdata.pdf

##### HD447800 datasheet

- https://www.sparkfun.com/datasheets/LCD/HD44780.pdf

##### PCF8574 datasheet

- http://www.ti.com/lit/ml/scyb031/scyb031.pdf
- http://www.ti.com/lit/ds/symlink/pcf8574.pdf
- https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf



### jm_LCM2004A_I2C Class

```
class jm_LCM2004A_I2C : public Print
{
private:

protected:

	jm_PCF8574 _pcf8574; // the i2c interface device of LCM2004A
	bool _BL; // the saved backlight state of LCM2004A

	bool wr_highorder(uint8_t DB, bool RS, uint16_t us); // return OK
	int rd_highorder(bool RS); // return byte (high data) or -1 (fail)

	bool wr_databus(uint8_t data, bool RS, uint16_t us); // return OK
	int rd_databus(bool RS); // return byte or -1 (fail)

protected:

	uint8_t _entry_mode_set;
	uint8_t _display_control;
	uint8_t _cursor_display_shift;
	uint8_t _function_set;

public:

// LCM2004A...

	bool wr_instreg(uint8_t data, uint16_t us); // return OK
	int rd_instreg(); // return byte or -1 (fail)
	bool wr_datareg(uint8_t data); // return OK
	int rd_datareg(); // return byte or -1 (fail)

	bool reset(uint8_t i2c_address); // return OK

	bool clear_display(); // return OK
	bool return_home(); // return OK

	bool entry_mode_set(uint8_t ems); // return Entry mode set current value
	bool display_control(uint8_t dc); // return Display control current value
	bool cursor_display_shift(uint8_t ds); // return Cursor display shift current value
	bool function_set(uint8_t fs); // return Function set current value

	bool entry_mode_set(bool I_D, bool S); // return OK
	bool display_control(bool D, bool C, bool B); // return OK
	bool cursor_display_shift(bool S_C, bool R_L); // return OK
	bool function_set(bool DL, bool N, bool F); // return OK

	bool set_cgram_addr(uint8_t ACG); // return OK
	bool set_ddram_addr(uint8_t ADD); // return OK

	uint8_t entry_mode_set(); // return Entry mode set current value
	uint8_t display_control(); // return Display control current value
	uint8_t cursor_display_shift(); // return Cursor display shift current value
	uint8_t function_set(); // return Function set current value

	bool set_cursor(int col, int row); // return OK

	bool write_cgram(uint8_t index, uint8_t count, const uint8_t font5x8[]);
#ifdef __AVR__
	bool write_cgram_P(uint8_t index, uint8_t count, const uint8_t font5x8_P[]);
#else
	#define write_cgram_P(index, count, font5x8_P) write_cgram(index, count, font5x8_P)
#endif

// LiquidCrystal compatibility...

	void clear();
	void home();
	void setCursor(uint8_t col, uint8_t row);

	// Turn the display on/off (quickly)
	void noDisplay();
	void display();

	// Turns the underline cursor on/off
	void noCursor();
	void cursor();

	// Turn on and off the blinking cursor
	void noBlink();
	void blink();

	// These commands scroll the display without changing the RAM
	void scrollDisplayLeft(void);
	void scrollDisplayRight(void);

	// This is for text that flows Left to Right
	void leftToRight(void);

	// This is for text that flows Right to Left
	void rightToLeft(void);

	// This will 'right justify' text from the cursor
	void autoscroll(void);

	// This will 'left justify' text from the cursor
	void noAutoscroll(void);

	void createChar(uint8_t location, uint8_t charmap[]);

	void command(uint8_t value);
	//size_t write(uint8_t value);

// LCM2004A_I2C...

	jm_LCM2004A_I2C();
	jm_LCM2004A_I2C(uint8_t i2c_address);
	operator bool();
	uint8_t i2c_address();

	bool begin(); // return OK
	bool begin(uint8_t i2c_address); // return OK
	bool end();

	virtual size_t write(uint8_t value);
	using Print::write;
};
```

