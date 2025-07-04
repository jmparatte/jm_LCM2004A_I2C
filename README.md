# jm_LCM2004A_I2C Arduino Library

2025-07-03: v2.0.0 - `jm_LCM2004A_I2C` is now inherited from `Stream` Arduino class. Now it's possible to `read` the screen and realize function like `PrintScreen`. Also, the I2C bus can now be freely selected.
The library is now licensed LGPLv2.1 (previously LGPLv3.0).2020-08-25: v1.0.2 - Added compatibility with LiquidCrystal library.  
2020-05-11: v1.0.1 - Checked with architectures AVR, SAM and ESP32.  
2020-05-04: v1.0.0 - Initial commit.  



### LCM2004A LCD Display Module interfaced for I2C

##### LCM2004A models

- <https://www.banggood.com/search/i2c-2004-204-20-x-4-character-lcd-display-module.html?from=nav>
- <https://ch.farnell.com/powertip/pc2004ars-awa-a-q/lcd-modul-stn-reflect-20x4/dp/1671507>

##### LCM2004A datasheets

- <https://www.beta-estore.com/download/rk/RK-10290_410.pdf>
- <http://www.farnell.com/datasheets/50586.pdf>

##### LCM2004A schematic

- <https://www.mpja.com/download/35054opdata.pdf>

##### HD447800 datasheet

- <https://www.sparkfun.com/datasheets/LCD/HD44780.pdf>

##### PCF8574 datasheet

- <https://www.ti.com/lit/ml/scyb031/scyb031.pdf>
- <https://www.ti.com/lit/ds/symlink/pcf8574.pdf>
- <https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf>


### jm_LCM2004A_I2C Class

```
class jm_LCM2004A_I2C : public Stream
{
protected:

// LCM2004A_I2C...

	byte _i2c_address_else_auto; // LCM2004A I2C address set else auto (0x00)

	jm_PCF8574 _pcf8574; // LCM2004A I2C device

// LCM2004A...

	bool _BL; // the saved backlight state of LCM2004A

// HD44780...

	byte _entry_mode_set;
	byte _display_control;
	byte _cursor_display_shift;
	byte _function_set;

	bool _begin(byte i2c_address); // return OK

public:

// LCM2004A_I2C...

	jm_PCF8574 & pcf8574() {return _pcf8574;}

	TwoWire & wire() {return _pcf8574.wire();}
	byte i2c_address() {return _pcf8574.i2c_address();}
	bool connected() {return _pcf8574.connected();}

	jm_LCM2004A_I2C();
	jm_LCM2004A_I2C(byte i2c_address);
	jm_LCM2004A_I2C(byte i2c_address, TwoWire & wire);

	operator bool() {return connected();}

	bool begin(); // return OK
	bool begin(byte i2c_address); // return OK
	bool end(); // return OK

	virtual int available() {return 0;}
	virtual int read() {return datareg_rd();}
	virtual int peek() {return -1;}

	virtual int availableForWrite() {return 0;}
	virtual size_t write(byte value) {return (size_t) datareg_wr(value);}
	using Print::write;

// LCM2004A...

	// P7-P4: Four high order data bus pins
	bool highorder_wr(byte DB, bool RS, uint16_t us); // return OK
	int highorder_rd(bool RS); // return byte (highorder) or -1 (fail)

	// P7-P0: Eight data bus pins
	bool databus_wr(byte data, bool RS, uint16_t us); // return OK
	int databus_rd(bool RS); // return byte or -1 (fail)

	// IR: Instruction Register
	bool instreg_wr(byte data, uint16_t us) {return databus_wr(data, false, us);} // return OK
	int instreg_rd() {return databus_rd(false);} // return byte or -1 (fail)

	// DR: Data Register
	bool datareg_wr(byte data) {return databus_wr(data, true, 0);} // return OK
	int datareg_rd() {return databus_rd(true);} // return byte or -1 (fail)

	// BL: Backlight
	bool backlight(bool BL); // return OK
	bool backlight() {return _BL;} // return BL state

// HD44780...

	bool reset(); // return OK

	bool clear_display(); // return OK
	bool return_home(); // return OK

	bool entry_mode_set(byte ems); // return Entry mode set current value
	bool display_control(byte dc); // return Display control current value
	bool cursor_display_shift(byte ds); // return Cursor display shift current value
	bool function_set(byte fs); // return Function set current value

	bool entry_mode_set(bool I_D, bool S); // return OK
	bool display_control(bool D, bool C, bool B); // return OK
	bool cursor_display_shift(bool S_C, bool R_L); // return OK
	bool function_set(bool DL, bool N, bool F); // return OK

	bool set_cgram_addr(byte ACG); // return OK
	bool set_ddram_addr(byte ADD); // return OK

	byte entry_mode_set(); // return Entry mode set current value
	byte display_control(); // return Display control current value
	byte cursor_display_shift(); // return Cursor display shift current value
	byte function_set(); // return Function set current value

	bool set_cursor(int col, int row); // return OK

	bool write_cgram(byte index, byte count, const byte font5x8[]); // return OK
#ifdef __AVR__
	bool write_cgram_P(byte index, byte count, const byte font5x8_P[]); // return OK
#else
	#define write_cgram_P(index, count, font5x8_P) write_cgram(index, count, font5x8_P)
#endif

// LiquidCrystal compatibility...

	void clear();
	void home();
	void setCursor(byte col, byte row);

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

	void createChar(byte location, byte charmap[]);

	void command(byte value);
	//size_t write(byte value);
};
```

