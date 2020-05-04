
#include <Wire.h>

#include <jm_LCM2004A_I2C.h>

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::_hd44780u_wr_4bit(uint8_t DB, bool RS, uint16_t us) // return OK
{
	uint8_t s[3];
	for (size_t i=0; i<3; i++)
		s[i] = (
			(DB & LCM2004A_I2C_DB) |
			(_BL ? LCM2004A_I2C_BL : 0) |
			((i & 1) ? LCM2004A_I2C_E : 0) |
			0 |
			(RS ? LCM2004A_I2C_RS : 0)
		);
	if (_pcf8574.write((const uint8_t *) &s, 3) != 3) return false;
	_pcf8574.wait(us + us/10); // add 10% HD44780U frequency shifting error (p.25)

	return true;
}

int jm_LCM2004A_I2C::_hd44780u_rd_4bit(bool RS) // return byte (high nibble) or -1 (fail)
{
	int result;

	uint8_t s[3];
	for (size_t i=0; i<3; i++)
		s[i] = (
			0 |
			(_BL ? LCM2004A_I2C_BL : 0) |
			((i & 1) ? LCM2004A_I2C_E : 0) |
			LCM2004A_I2C_R_W |
			(RS ? LCM2004A_I2C_RS : 0)
		);
	if (_pcf8574.write((const uint8_t *) &s[0], 2) != 2) return -1;
	if ((result = _pcf8574.read()) == -1) return -1;
	if (_pcf8574.write((const uint8_t *) &s[2], 1) != 1) return -1;

	return (result & LCM2004A_I2C_DB);
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::_hd44780u_wr_8bit(uint8_t command, bool RS, uint16_t us) // return OK
{
	if (!_hd44780u_wr_4bit((uint8_t) (command << 0), RS, 0)) return false;
	if (!_hd44780u_wr_4bit((uint8_t) (command << 4), RS, us)) return false;

	return true;
}

int jm_LCM2004A_I2C::_hd44780u_rd_8bit(bool RS) // return byte or -1 (fail)
{
	int result1; if ((result1 = _hd44780u_rd_4bit(RS)) == -1) return -1;
	int result2; if ((result2 = _hd44780u_rd_4bit(RS)) == -1) return -1;

	return ((result1 >> 0) | (result2 >> 4));
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::wr_command(uint8_t command, uint16_t us) // return OK
{
	return _hd44780u_wr_8bit(command, false, us);
}

int jm_LCM2004A_I2C::rd_command() // return byte or -1 (fail)
{
	return _hd44780u_rd_8bit(false);
}

bool jm_LCM2004A_I2C::wr_data(uint8_t value) // return OK
{
	return _hd44780u_wr_8bit(value, true, 0);
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::reset(uint8_t i2c_address) // return OK
{
	if (!_pcf8574) if (!_pcf8574.begin(i2c_address)) return false;

	// HD44780U software reset after Power On...

	_pcf8574.wait((uint16_t) 40*1000); // wait 40ms

	if (!_hd44780u_wr_4bit(HD44780U_SET | HD44780U_SET_DL, false, 4100)) return false;
	if (!_hd44780u_wr_4bit(HD44780U_SET | HD44780U_SET_DL, false, 100)) return false;
	if (!_hd44780u_wr_4bit(HD44780U_SET | HD44780U_SET_DL, false, 37)) return false;
	if (!_hd44780u_wr_4bit(HD44780U_SET                  , false, 37)) return false;

	// HD44780U set 4bit mode, 4x20 characters, 5x8 dots...

	if (!function_set()) return false;						// Function set default: 4 bits, 2 lines, 5x8 dots

	// HD44780U software reset after Power On...

	if (!display_control(false)) return false;				// Display control: Display off else default
	if (!clear_display()) return false;						// Clear display
	if (!entry_mode_set()) return false;					// Entry mode set default: increment
//	if (!display_control(true, true, true)) return false;	// Display control: Display on, Cursor on, Blink on
	if (!display_control()) return false;					// Display control default: Display on, Cursor off, Blink off

	return true;
}

bool jm_LCM2004A_I2C::clear_display() // return OK
{
	return wr_command(HD44780U_CLEAR, 1520); // 1520 ?
}

bool jm_LCM2004A_I2C::return_home() // return OK
{
	return wr_command(HD44780U_HOME, 1520);
}

bool jm_LCM2004A_I2C::entry_mode_set(bool I_D, bool S) // return OK
{
	return wr_command(HD44780U_ENTRY | (I_D ? HD44780U_ENTRY_I_D : 0) | (S ? HD44780U_ENTRY_S : 0), 37);
}

bool jm_LCM2004A_I2C::display_control(bool D, bool C, bool B) // return OK
{
	return wr_command(HD44780U_DISPLAY | (D ? HD44780U_DISPLAY_D : 0) | (C ? HD44780U_DISPLAY_C : 0) | (B ? HD44780U_DISPLAY_B : 0), 37);
}

bool jm_LCM2004A_I2C::cursor_display_shift(bool S_C, bool R_L) // return OK
{
	return wr_command(HD44780U_CURSOR | (S_C ? HD44780U_CURSOR_S_C : 0) | (R_L ? HD44780U_CURSOR_R_L : 0), 37);
}

bool jm_LCM2004A_I2C::function_set(bool DL, bool N, bool F) // return OK (default 4 bits, 2 lines, 5x8 dots)
{
	return wr_command(HD44780U_SET | (DL ? HD44780U_SET_DL : 0) | (N ? HD44780U_SET_N : 0) | (F ? HD44780U_SET_F : 0), 37);
}

bool jm_LCM2004A_I2C::set_cgram_addr(uint8_t ACG) // return OK
{
	return wr_command(HD44780U_CGRAM | (ACG & HD44780U_CGRAM_ACG), 37);
}

bool jm_LCM2004A_I2C::set_ddram_addr(uint8_t ADD) // return OK
{
	return wr_command(HD44780U_DDRAM | (ADD & HD44780U_DDRAM_ADD), 37);
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::set_cursor(int col, int row) // return OK
{
	return set_ddram_addr(col + ((row & 1) ? 64 : 0) + ((row & 2) ? 20 : 0));
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::write_cgram(uint8_t index, uint8_t count, const uint8_t font5x8[]) // return OK
{
	if (!set_cgram_addr(index * 8)) return false;

	if (write(font5x8, count * 8) == 0) return false;

	return true;
}

#ifdef __AVR__

bool jm_LCM2004A_I2C::write_cgram_P(uint8_t index, uint8_t count, const uint8_t font5x8_P[]) // return OK
{
	if (!set_cgram_addr(index * 8)) return false;

	for (uint8_t i=0; i<count; i++)
		for (uint8_t j=0; j<8; j++)
			if (write(pgm_read_byte_near((const uint8_t  *) font5x8_P++)) == 0) return false;

	return true;
}

#endif

// ---------------------------------------------------------------------------

/********** high level commands, for the user! */

inline void jm_LCM2004A_I2C::clear()
{
//  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
//  delayMicroseconds(2000);  // this command takes a long time!
	clear_display();
}

inline void jm_LCM2004A_I2C::home()
{
//  command(LCD_RETURNHOME);  // set cursor position to zero
//  delayMicroseconds(2000);  // this command takes a long time!
	return_home();
}

inline void jm_LCM2004A_I2C::setCursor(uint8_t col, uint8_t row)
{
//  const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
//  if ( row >= max_lines ) {
//    row = max_lines - 1;    // we count rows starting w/0
//  }
//  if ( row >= _numlines ) {
//    row = _numlines - 1;    // we count rows starting w/0
//  }
//
//  command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
	set_cursor(col, row);
}

// Turn the display on/off (quickly)
inline void jm_LCM2004A_I2C::noDisplay() {
//  _displaycontrol &= ~LCD_DISPLAYON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
inline void jm_LCM2004A_I2C::display() {
//  _displaycontrol |= LCD_DISPLAYON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
inline void jm_LCM2004A_I2C::noCursor() {
//  _displaycontrol &= ~LCD_CURSORON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
inline void jm_LCM2004A_I2C::cursor() {
//  _displaycontrol |= LCD_CURSORON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
inline void jm_LCM2004A_I2C::noBlink() {
//  _displaycontrol &= ~LCD_BLINKON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
}
inline void jm_LCM2004A_I2C::blink() {
//  _displaycontrol |= LCD_BLINKON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
inline void jm_LCM2004A_I2C::scrollDisplayLeft(void) {
//  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
inline void jm_LCM2004A_I2C::scrollDisplayRight(void) {
//  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
inline void jm_LCM2004A_I2C::leftToRight(void) {
//  _displaymode |= LCD_ENTRYLEFT;
//  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
inline void jm_LCM2004A_I2C::rightToLeft(void) {
//  _displaymode &= ~LCD_ENTRYLEFT;
//  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
inline void jm_LCM2004A_I2C::autoscroll(void) {
//  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
//  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
inline void jm_LCM2004A_I2C::noAutoscroll(void) {
//  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
//  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
inline void jm_LCM2004A_I2C::createChar(uint8_t location, uint8_t charmap[]) {
//  location &= 0x7; // we only have 8 locations 0-7
//  command(LCD_SETCGRAMADDR | (location << 3));
//  for (int i=0; i<8; i++) {
//    write(charmap[i]);
//  }
	write_cgram(location, 1, charmap);
}

/*********** mid level commands, for sending data/cmds */

inline void jm_LCM2004A_I2C::command(uint8_t value) {
//  send(value, LOW);
	wr_command(value, 0);
}

//inline size_t jm_LCM2004A_I2C::write(uint8_t value) {
//  send(value, HIGH);
//  return 1; // assume success
//}

// ---------------------------------------------------------------------------

jm_LCM2004A_I2C::jm_LCM2004A_I2C() :
	_pcf8574(0x00),			// undefined module I2C address
	_BL(true)				// Backlight On
{
}

jm_LCM2004A_I2C::jm_LCM2004A_I2C(uint8_t i2c_address) :
	_pcf8574(i2c_address),	// set module I2C address
	_BL(true)				// Backlight On
{
}

jm_LCM2004A_I2C::operator bool()
{
	return _pcf8574;
}

uint8_t jm_LCM2004A_I2C::i2c_address()
{
	return _pcf8574.i2c_address();
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::begin() // return OK
{
	if (_pcf8574 || (_pcf8574.i2c_address() != 0x00)) return reset(_pcf8574.i2c_address()); // redo reset() ?

	return (reset(LCM2004A_I2C_ADR1) || reset(LCM2004A_I2C_ADR2));
}

bool jm_LCM2004A_I2C::begin(uint8_t i2c_address) // return OK
{
	return reset(i2c_address);
}

void jm_LCM2004A_I2C::end()
{
	_pcf8574.end();
}

inline size_t jm_LCM2004A_I2C::write(uint8_t value)
{
//	if (!_hd44780u_wr_8bit(value, true, 0)) return 0; else return 1;
	if (!wr_data(value)) return 0; else return 1;
}

//------------------------------------------------------------------------------

// END.
