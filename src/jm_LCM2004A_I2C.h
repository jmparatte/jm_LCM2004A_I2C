
#ifndef jm_LCM2004A_I2C_h
#define jm_LCM2004A_I2C_h

// Liquid Crystal Module 2004A
// ===========================
// 20 colons, 4 lines, blue or yellow LCD
// I2C interfaced with PCF8574

// I2C address:
// LCM2004A: 0x3F (PCF8574A)
// LCM2004: 0x27 (PCF8574)

// Backlight is ON by default (due to PCF8574 reset).

////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Stream.h>

#include <jm_PCF8574.h>

////////////////////////////////////////////////////////////////////////////////

// Clear display
// =============
// Clears entire display and sets DDRAM address 0 in address rd_IR.
//
// Execution time: 1520us ???
//
#define HD44780_CLEAR		((byte) 0b00000001)

// Return home
// ===========
//
// Sets DDRAM address 0 in address rd_IR.
// Also returns display from being shifted to original position.
// DDRAM contents remain unchanged.
//
// Execution time: 1520us
//
#define HD44780_HOME		((byte) 0b00000010)

// Entry mode set
// ==============
//
// Sets cursor mode direction and specifies display shift.
// These operations are performed during data write and read.
//
// I/D = 1: Increment
// I/D = 0: Decrement
// S   = 1: Accompanies display shift
//
// Execution time: 37us
//
#define HD44780_ENTRY		((byte) 0b00000100)
#define HD44780_ENTRY_I_D	((byte) 0b00000010)
#define HD44780_ENTRY_S		((byte) 0b00000001)

// Display on/off control
// ======================
//
// Sets entire display (D) on/off, cursor on/off (C),
// and blinking of cursor position character (B).
//
// Execution time: 37us
//
#define HD44780_CONTROL		((byte) 0b00001000)
#define HD44780_CONTROL_D	((byte) 0b00000100)
#define HD44780_CONTROL_C	((byte) 0b00000010)
#define HD44780_CONTROL_B	((byte) 0b00000001)

// Cursor or display shift
// =======================
//
// Moves cursor and shifts display without changing DDRAM contents.
//
// S/C = 1: Display shift
// S/C = 0: Cursor move
// R/L = 1: Shift to the right
// R/L = 0: Shift to the left
//
// Execution time: 37us
//
#define HD44780_SHIFT		((byte) 0b00010000)
#define HD44780_SHIFT_S_C	((byte) 0b00001000)
#define HD44780_SHIFT_R_L	((byte) 0b00000100)

// Function set
// ============
//
// Sets interface data length (DL), number of data lines (N), and character font (F).
//
// DL  = 1: 8 bits, 0: 4bit mode
// N   = 1: 2 lines, 0: 1 line
// F   = 1: 5x10 dots, 0: 5x8 dots
//
// Execution time: 37us
//
#define HD44780_FUNCTION	((byte) 0b00100000)
#define HD44780_FUNCTION_DL ((byte) 0b00010000)
#define HD44780_FUNCTION_N	((byte) 0b00001000)
#define HD44780_FUNCTION_F	((byte) 0b00000100)

// Set CGRAM address
// =================
//
// Sets CGRAM address.
// CGRAM data is sent and received after this setting.
//
// Execution time: 37us
//
#define HD44780_CGRAM		((byte) 0b01000000)
#define HD44780_CGRAM_ACG	((byte) 0b00111111)

// Set DDRAM address
// =================
//
// Sets DDRAM address.
// DDRAM data is sent and received after this setting.
//
// Execution time: 37us
//
#define HD44780_DDRAM		((byte) 0b10000000)
#define HD44780_DDRAM_ADD	((byte) 0b01111111)

// Read busy flag & address
// ========================
//
// Reads busy flag (BF) indicating internal operation is being performed
// and reads address rd_IR contents.
// BF  = 1: Internally operating
// BL  = 0: Instructions acceptable
// AC  = Address rd_IR used for both DD and CGRAM addresses
//
// Execution time: 0us
//
#define HD44780_READ		((byte) 0b00000000)
#define HD44780_READ_BF		((byte) 0b10000000)
#define HD44780_READ_AC		((byte) 0b01111111)

////////////////////////////////////////////////////////////////////////////////

#define LCM2004A_I2C_ADR1	((byte) 0x3F)		// default PCF8574A I2C address
#define LCM2004A_I2C_ADR2	((byte) 0x27)		// alternative PCF8574 I2C address

#define LCM2004A_I2C_DB		((byte) 0b11110000)	// P7-P4: Four high order data bus pins
#define LCM2004A_I2C_DB7	((byte) 0b10000000)	// P7
#define LCM2004A_I2C_DB6	((byte) 0b01000000)	// P6
#define LCM2004A_I2C_DB5	((byte) 0b00100000)	// P5
#define LCM2004A_I2C_DB4	((byte) 0b00010000)	// P4
#define LCM2004A_I2C_BL		((byte) 0b00001000)	// P3: LCD backlight
#define LCM2004A_I2C_E		((byte) 0b00000100)	// P2: Starts data read/write
#define LCM2004A_I2C_R_W	((byte) 0b00000010)	// P1: Selects Read/Write (1/0)
#define LCM2004A_I2C_RS		((byte) 0b00000001)	// P0: Selects Instruction/Data register (0/1)

////////////////////////////////////////////////////////////////////////////////

//class jm_LCM2004A_I2C : public Print
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

////////////////////////////////////////////////////////////////////////////////

#endif // jm_LCM2004A_I2C_h
