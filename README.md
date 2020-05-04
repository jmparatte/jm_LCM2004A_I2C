# jm_LCM2004A_I2C Arduino Library

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

	bool _hd44780u_wr_4bit(uint8_t DB, bool RS, uint16_t us); // return OK
	int _hd44780u_rd_4bit(bool RS); // return byte (high nibble) or -1 (fail)

	bool _hd44780u_wr_8bit(uint8_t data, bool RS, uint16_t us); // return OK
	int _hd44780u_rd_8bit(bool RS); // return byte or -1 (fail)

public:

// LCM2004A...

	bool wr_command(uint8_t command, uint16_t us); // return OK
	int rd_command(); // return byte or -1 (fail)
	bool wr_data(uint8_t value); // return OK

	bool reset(uint8_t i2c_address); // return OK
	bool clear_display(); // return OK
	bool return_home(); // return OK
	bool entry_mode_set(bool I_D=true, bool S=false); // return OK
	bool display_control(bool D=true, bool C=false, bool B=false); // return OK
	bool cursor_display_shift(bool S_C=false, bool R_L=true); // return OK
	bool function_set(bool DL=false, bool N=true, bool F=false); // return OK (default 4 bits, 2 lines, 5x8 dots)
	bool set_cgram_addr(uint8_t ACG=0); // return OK
	bool set_ddram_addr(uint8_t ADD=0); // return OK

	bool set_cursor(int col, int row); // return OK

	bool write_cgram(uint8_t index, uint8_t count, const uint8_t font5x8[]);
#ifdef __AVR__
	bool write_cgram_P(uint8_t index, uint8_t count, const uint8_t font5x8_P[]);
#else
	#define write_cgram_P(index, count, font5x8_P) write_cgram(index, count, font5x8_P)
#endif

// LiquidCrystal compatibility...

	inline void clear();
	inline void home();
	inline void setCursor(uint8_t col, uint8_t row);

	// Turn the display on/off (quickly)
	inline void noDisplay();
	inline void display();

	// Turns the underline cursor on/off
	inline void noCursor();
	inline void cursor();

	// Turn on and off the blinking cursor
	inline void noBlink();
	inline void blink();

	// These commands scroll the display without changing the RAM
	inline void scrollDisplayLeft(void);
	inline void scrollDisplayRight(void);

	// This is for text that flows Left to Right
	inline void leftToRight(void);

	// This is for text that flows Right to Left
	inline void rightToLeft(void);

	// This will 'right justify' text from the cursor
	inline void autoscroll(void);

	// This will 'left justify' text from the cursor
	inline void noAutoscroll(void);

	inline void createChar(uint8_t location, uint8_t charmap[]);

	inline void command(uint8_t value);
	//inline size_t write(uint8_t value);

// LCM2004A_I2C...

	jm_LCM2004A_I2C();
	jm_LCM2004A_I2C(uint8_t i2c_address);
	operator bool();
	uint8_t i2c_address();

	bool begin(); // return OK
	bool begin(uint8_t i2c_address); // return OK
	void end();

	virtual size_t write(uint8_t value);
	using Print::write;
};
```

