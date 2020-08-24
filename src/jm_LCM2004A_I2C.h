
#ifndef jm_LCM2004A_I2C_h
#define jm_LCM2004A_I2C_h

#include <stddef.h>
#include <stdint.h>
//#include <inttypes.h>

#include <Print.h>

#include <jm_PCF8574.h>

////////////////////////////////////////////////////////////////////////////////

// Clear display
// =============
// Clears entire display and sets DDRAM address 0 in address rd_IR.
//
// Execution time: 1520us ???
//
#define HD44780_CLEAR		((uint8_t) 0b00000001)

// Return home
// ===========
//
// Sets DDRAM address 0 in address rd_IR.
// Also returns display from being shifted to original position.
// DDRAM contents remain unchanged.
//
// Execution time: 1520us
//
#define HD44780_HOME		((uint8_t) 0b00000010)

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
#define HD44780_ENTRY		((uint8_t) 0b00000100)
#define HD44780_ENTRY_I_D	((uint8_t) 0b00000010)
#define HD44780_ENTRY_S		((uint8_t) 0b00000001)

// Display on/off control
// ======================
//
// Sets entire display (D) on/off, cursor on/off (C),
// and blinking of cursor position character (B).
//
// Execution time: 37us
//
#define HD44780_CONTROL		((uint8_t) 0b00001000)
#define HD44780_CONTROL_D	((uint8_t) 0b00000100)
#define HD44780_CONTROL_C	((uint8_t) 0b00000010)
#define HD44780_CONTROL_B	((uint8_t) 0b00000001)

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
#define HD44780_SHIFT		((uint8_t) 0b00010000)
#define HD44780_SHIFT_S_C	((uint8_t) 0b00001000)
#define HD44780_SHIFT_R_L	((uint8_t) 0b00000100)

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
#define HD44780_FUNCTION	((uint8_t) 0b00100000)
#define HD44780_FUNCTION_DL ((uint8_t) 0b00010000)
#define HD44780_FUNCTION_N	((uint8_t) 0b00001000)
#define HD44780_FUNCTION_F	((uint8_t) 0b00000100)

// Set CGRAM address
// =================
//
// Sets CGRAM address.
// CGRAM data is sent and received after this setting.
//
// Execution time: 37us
//
#define HD44780_CGRAM		((uint8_t) 0b01000000)
#define HD44780_CGRAM_ACG	((uint8_t) 0b00111111)

// Set DDRAM address
// =================
//
// Sets DDRAM address.
// DDRAM data is sent and received after this setting.
//
// Execution time: 37us
//
#define HD44780_DDRAM		((uint8_t) 0b10000000)
#define HD44780_DDRAM_ADD	((uint8_t) 0b01111111)

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
#define HD44780_READ		((uint8_t) 0b00000000)
#define HD44780_READ_BF		((uint8_t) 0b10000000)
#define HD44780_READ_AC		((uint8_t) 0b01111111)

////////////////////////////////////////////////////////////////////////////////

#define LCM2004A_I2C_ADR1	((uint8_t) 0x3F)		// default PCF8574A I2C address
#define LCM2004A_I2C_ADR2	((uint8_t) 0x27)		// alternative PCF8574 I2C address

#define LCM2004A_I2C_DB		((uint8_t) 0b11110000)	// P7-P4: Four high order data bus pins
#define LCM2004A_I2C_DB7	((uint8_t) 0b10000000)	// P7
#define LCM2004A_I2C_DB6	((uint8_t) 0b01000000)	// P6
#define LCM2004A_I2C_DB5	((uint8_t) 0b00100000)	// P5
#define LCM2004A_I2C_DB4	((uint8_t) 0b00010000)	// P4
#define LCM2004A_I2C_BL		((uint8_t) 0b00001000)	// P3: LCD backlight
#define LCM2004A_I2C_E		((uint8_t) 0b00000100)	// P2: Starts data read/write
#define LCM2004A_I2C_R_W	((uint8_t) 0b00000010)	// P1: Selects Read/Write (1/0)
#define LCM2004A_I2C_RS		((uint8_t) 0b00000001)	// P0: Selects Instruction/Data register (0/1)

////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////

#endif // jm_LCM2004A_I2C_h
