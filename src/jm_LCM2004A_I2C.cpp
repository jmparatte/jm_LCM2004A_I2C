
#include <jm_LCM2004A_I2C.h>

// ---------------------------------------------------------------------------

// LCM2004A_I2C...

// remark: just after hardware reset, backlight is ON due to PCF8574 reset.

bool jm_LCM2004A_I2C::_begin(byte i2c_address) // return OK
{
	if (_pcf8574.connected()) return false; // already connected ? !OK

	// begin PCF8574...

	if (!_pcf8574.begin(
		i2c_address,
		0xFF,							// all pins as OUTPUT (OPEN_DRAIN)
		LCM2004A_I2C_DB |				// P7-P4 HIGH (compatible with high order bus reading)
		(_BL ? LCM2004A_I2C_BL : 0) |	// backlight
		(false ? LCM2004A_I2C_E : 0) |	// E false (data not ready selected)
		(false ? LCM2004A_I2C_R_W : 0) |// R_W false (bus writting selected)
		(false ? LCM2004A_I2C_RS : 0)	// RS false (Instruction Register selected)
	)) return false; // fail ? !OK

//	// HD44780U software reset after Power On...
//
//	_pcf8574.wait((uint16_t) 40*1000); // wait 40ms after Power On

	return true;
}

// ---------------------------------------------------------------------------

jm_LCM2004A_I2C::jm_LCM2004A_I2C() :
	_i2c_address_else_auto(0x00), // LCM2004A I2C address auto
	_pcf8574(0x00),			// undefined I2C address
	_BL(true),				// Backlight On
	_entry_mode_set(0),
	_display_control(0),
	_cursor_display_shift(0),
	_function_set(0)
{}

jm_LCM2004A_I2C::jm_LCM2004A_I2C(byte i2c_address) :
	_i2c_address_else_auto(i2c_address), // LCM2004A I2C address set
	_pcf8574(i2c_address),	// set I2C address
	_BL(true),				// Backlight On
	_entry_mode_set(0),
	_display_control(0),
	_cursor_display_shift(0),
	_function_set(0)
{}

jm_LCM2004A_I2C::jm_LCM2004A_I2C(byte i2c_address, TwoWire & wire) :
	_i2c_address_else_auto(i2c_address), // LCM2004A I2C address set
	_pcf8574(i2c_address, wire), // set I2C address and wire bus
	_BL(true),				// Backlight On
	_entry_mode_set(0),
	_display_control(0),
	_cursor_display_shift(0),
	_function_set(0)
{}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::begin() // return OK
{
//	if (_pcf8574.i2c_address() != 0x00) // set/saved I2C address ?
	if (_i2c_address_else_auto != 0x00) // I2C address set ?
		return
//			begin(_pcf8574.i2c_address());
			begin(_i2c_address_else_auto);
	else
		return (
			begin(LCM2004A_I2C_ADR1) ||	// try LCM2004A I2C address else
			begin(LCM2004A_I2C_ADR2)	// try LCM2004 I2C address
		);
}

bool jm_LCM2004A_I2C::begin(byte i2c_address) // return OK
{
	if (!_begin(i2c_address)) return false; // fail ? !OK

	return reset();
}

bool jm_LCM2004A_I2C::end() // return OK
{
	return _pcf8574.end();
}

// ---------------------------------------------------------------------------

// P7-P4: Four high order data bus pins

bool jm_LCM2004A_I2C::highorder_wr(byte DB, bool RS, uint16_t us) // return OK
{
	byte s[3];
	for (size_t i=0; i<3; i++)
		s[i] = (
			(DB & LCM2004A_I2C_DB) |
			(_BL ? LCM2004A_I2C_BL : 0) |
			((i & 1) ? LCM2004A_I2C_E : 0) |
			0 |
			(RS ? LCM2004A_I2C_RS : 0)
		);
	_pcf8574.port_output((const byte *) &s, 3);
	if (!_pcf8574.connected()) return false;
	_pcf8574.wait(us + us/10); // add 10% HD44780U frequency shifting error (p.25)

	return true;
}

int jm_LCM2004A_I2C::highorder_rd(bool RS) // return byte (highorder) or -1 (fail)
{
	byte data;

	byte s[3];
	for (size_t i=0; i<3; i++)
		s[i] = (
			LCM2004A_I2C_DB |
			(_BL ? LCM2004A_I2C_BL : 0) |
			((i & 1) ? LCM2004A_I2C_E : 0) |
			LCM2004A_I2C_R_W |
			(RS ? LCM2004A_I2C_RS : 0)
		);
	_pcf8574.port_output((const byte *) &s[0], 2);
	if (!_pcf8574.connected()) return -1;
	data = _pcf8574.port_input();
	if (!_pcf8574.connected()) return -1;
	_pcf8574.port_output((const byte *) &s[2], 1);
	if (!_pcf8574.connected()) return -1;

	return (data & LCM2004A_I2C_DB);
}

// ---------------------------------------------------------------------------

// P7-P0: Eight data bus pins

bool jm_LCM2004A_I2C::databus_wr(byte DB, bool RS, uint16_t us) // return OK
{
	if (!highorder_wr((byte) (DB << 0), RS, 0)) return false;
	if (!highorder_wr((byte) (DB << 4), RS, us)) return false;

	return true;
}

int jm_LCM2004A_I2C::databus_rd(bool RS) // return byte or -1 (fail)
{
	int result1; if ((result1 = highorder_rd(RS)) == -1) return -1;
	int result2; if ((result2 = highorder_rd(RS)) == -1) return -1;

	return ((result1 >> 0) | (result2 >> 4));
}

// ---------------------------------------------------------------------------

// BL: backlight

bool jm_LCM2004A_I2C::backlight(bool BL) // return OK
{
//	if (!_pcf8574) return false;

	_BL = BL; // save BL state

//	// write BL bit...
//	byte port_shadow = _pcf8574.port_shadow();
//	byte port_shadow_bl = (_BL ? LCM2004A_I2C_BL : 0);
//	port_shadow |= port_shadow_bl;
//	port_shadow &= port_shadow_bl | ~LCM2004A_I2C_BL;
//	//
//	return (_pcf8574.port_write(port_shadow) == 1); // write and return OK
	byte data = (
		(LCM2004A_I2C_DB) |
		(_BL ? LCM2004A_I2C_BL : 0) |
		(0 ? LCM2004A_I2C_E : 0) |
		(0 ? LCM2004A_I2C_R_W : 0) |
		(0 ? LCM2004A_I2C_RS : 0)
	);
//	return (_pcf8574.port_write(data) == 1);
	_pcf8574.port_output(data);
	return (_pcf8574.connected());
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::reset() // return OK
{
	// HD44780U software reset after Power On...

	_pcf8574.wait((uint16_t) 40*1000); // wait 40ms after Power On

//	Hardware Reset
//	--------------
//	1.   Display clear
//	2.   Function set:
//			DL = 1; 8-bit interface data
//			N = 0; 1-line display
//			F = 0; 5 × 8 dot character font
//	3.   Display on/off control:
//			D = 0; Display off
//			C = 0; Cursor off
//			B = 0; Blinking off
//	4.   Entry mode set:
//			I/D = 1; Increment by 1
//			S = 0; No shift

	// Initializing by Instruction...

	if (!highorder_wr(HD44780_FUNCTION | HD44780_FUNCTION_DL, false, 4100)) return false;
	if (!highorder_wr(HD44780_FUNCTION | HD44780_FUNCTION_DL, false, 100)) return false;
	if (!highorder_wr(HD44780_FUNCTION | HD44780_FUNCTION_DL, false, 37)) return false;

	// 8bit mode after above sequence.

	// switch from 8bit mode to 4bit mode...

	if (!highorder_wr(HD44780_FUNCTION | 0                  , false, 37)) return false;

	// confirm 4bit mode and set 4x20 characters, 5x8 dots...

	if (!function_set(false, true, false)) return false;		// Function set default: 4bit mode; 2 lines; 5x8 dots
	if (!display_control(false, false, false)) return false;	// Display control: Display off; Cursor off; Blink off
	if (!clear_display()) return false;							// Clear display
	if (!entry_mode_set(true, false)) return false;				// Entry mode set: Increment; not Accompanies display shift
	if (!cursor_display_shift(false, true)) return false;		// Cursor display shift: no Display shift/Cursor move; Shift to the right

	if (!display_control(true, false, false)) return false;		// Display control: Display on; Cursor off; Blink off

	return true;
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::clear_display() // return OK
{
	return instreg_wr(HD44780_CLEAR, 1520); // 1520 ?
}

bool jm_LCM2004A_I2C::return_home() // return OK
{
	return instreg_wr(HD44780_HOME, 1520);
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::entry_mode_set(byte ems) // return OK
{
	_entry_mode_set = ems;
	return instreg_wr(_entry_mode_set, 37);
}

bool jm_LCM2004A_I2C::display_control(byte dc) // return OK
{
	_display_control = dc;
	return instreg_wr(_display_control, 37);
}

bool jm_LCM2004A_I2C::cursor_display_shift(byte cds) // return OK
{
	_cursor_display_shift = cds;
	return instreg_wr(_cursor_display_shift, 37);
}

bool jm_LCM2004A_I2C::function_set(byte fs) // return OK
{
	_function_set = fs;
	return instreg_wr(_function_set, 37);
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::set_cgram_addr(byte ACG) // return OK
{
	return instreg_wr(HD44780_CGRAM | (ACG & HD44780_CGRAM_ACG), 37);
}

bool jm_LCM2004A_I2C::set_ddram_addr(byte ADD) // return OK
{
	return instreg_wr(HD44780_DDRAM | (ADD & HD44780_DDRAM_ADD), 37);
}

// ---------------------------------------------------------------------------

byte jm_LCM2004A_I2C::entry_mode_set()
{
	return _entry_mode_set;
}

byte jm_LCM2004A_I2C::display_control()
{
	return _display_control;
}

byte jm_LCM2004A_I2C::cursor_display_shift()
{
	return _cursor_display_shift;
}

byte jm_LCM2004A_I2C::function_set()
{
	return _function_set;
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::entry_mode_set(bool I_D, bool S) // return OK
{
	return entry_mode_set(HD44780_ENTRY | (I_D ? HD44780_ENTRY_I_D : 0) | (S ? HD44780_ENTRY_S : 0));
}

bool jm_LCM2004A_I2C::display_control(bool D, bool C, bool B) // return OK
{
	return display_control(HD44780_CONTROL | (D ? HD44780_CONTROL_D : 0) | (C ? HD44780_CONTROL_C : 0) | (B ? HD44780_CONTROL_B : 0));
}

bool jm_LCM2004A_I2C::cursor_display_shift(bool S_C, bool R_L) // return OK
{
	return cursor_display_shift(HD44780_SHIFT | (S_C ? HD44780_SHIFT_S_C : 0) | (R_L ? HD44780_SHIFT_R_L : 0));
}

bool jm_LCM2004A_I2C::function_set(bool DL, bool N, bool F) // return OK
{
	return function_set(HD44780_FUNCTION | (DL ? HD44780_FUNCTION_DL : 0) | (N ? HD44780_FUNCTION_N : 0) | (F ? HD44780_FUNCTION_F : 0));
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::set_cursor(int col, int row) // return OK
{
	return set_ddram_addr(col + ((row & 1) ? 64 : 0) + ((row & 2) ? 20 : 0));
}

// ---------------------------------------------------------------------------

bool jm_LCM2004A_I2C::write_cgram(byte index, byte count, const byte font5x8[]) // return OK
{
	if (!set_cgram_addr(index*8)) return false;

//	if (write(font5x8, count*8) != count*8) return false;
	for (byte i=0; i<count; i++)
		for (byte j=0; j<8; j++)
//			if (datareg_wr( * ((const byte *) font5x8++) ) == 0) return false;
			if (datareg_wr( * ((const byte *) font5x8++) ) == false) return false;

	return true;
}

#ifdef __AVR__

bool jm_LCM2004A_I2C::write_cgram_P(byte index, byte count, const byte font5x8_P[]) // return OK
{
	if (!set_cgram_addr(index*8)) return false;

	for (byte i=0; i<count; i++)
		for (byte j=0; j<8; j++)
//			if (datareg_wr(pgm_read_byte_near((const byte *) font5x8_P++)) == 0) return false;
			if (datareg_wr(pgm_read_byte_near((const byte *) font5x8_P++)) == false) return false;

	return true;
}

#endif

// ---------------------------------------------------------------------------

// LiquidCrystal compatibility...

// ---------------------------------------------------------------------------

/********** high level commands, for the user! */

void jm_LCM2004A_I2C::clear()
{
//  command(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
//  delayMicroseconds(2000);  // this command takes a long time!
	clear_display();
}

void jm_LCM2004A_I2C::home()
{
//  command(LCD_RETURNHOME);  // set cursor position to zero
//  delayMicroseconds(2000);  // this command takes a long time!
	return_home();
}

void jm_LCM2004A_I2C::setCursor(byte col, byte row)
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
void jm_LCM2004A_I2C::noDisplay() {
//  _displaycontrol &= ~LCD_DISPLAYON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
	_display_control &= ~HD44780_CONTROL_D;
	instreg_wr(_display_control, 37);
}
void jm_LCM2004A_I2C::display() {
//  _displaycontrol |= LCD_DISPLAYON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
	_display_control |= HD44780_CONTROL_D;
	instreg_wr(_display_control, 37);
}

// Turns the underline cursor on/off
void jm_LCM2004A_I2C::noCursor() {
//  _displaycontrol &= ~LCD_CURSORON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
	_display_control &= ~HD44780_CONTROL_C;
	instreg_wr(_display_control, 37);
}
void jm_LCM2004A_I2C::cursor() {
//  _displaycontrol |= LCD_CURSORON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
	_display_control |= HD44780_CONTROL_C;
	instreg_wr(_display_control, 37);
}

// Turn on and off the blinking cursor
void jm_LCM2004A_I2C::noBlink() {
//  _displaycontrol &= ~LCD_BLINKON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
	_display_control &= ~HD44780_CONTROL_B;
	instreg_wr(_display_control, 37);
}
void jm_LCM2004A_I2C::blink() {
//  _displaycontrol |= LCD_BLINKON;
//  command(LCD_DISPLAYCONTROL | _displaycontrol);
	_display_control |= HD44780_CONTROL_B;
	instreg_wr(_display_control, 37);
}

// These commands scroll the display without changing the RAM
void jm_LCM2004A_I2C::scrollDisplayLeft(void) {
//  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
	_cursor_display_shift = (HD44780_SHIFT | (true ? HD44780_SHIFT_S_C : 0) | (false ? HD44780_SHIFT_R_L : 0));
	instreg_wr(_cursor_display_shift, 37);
}
void jm_LCM2004A_I2C::scrollDisplayRight(void) {
//  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
	_cursor_display_shift = (HD44780_SHIFT | (true ? HD44780_SHIFT_S_C : 0) | (true ? HD44780_SHIFT_R_L : 0));
	instreg_wr(_cursor_display_shift, 37);
}

// This is for text that flows Left to Right
void jm_LCM2004A_I2C::leftToRight(void) {
//  _displaymode |= LCD_ENTRYLEFT;
//  command(LCD_ENTRYMODESET | _displaymode);
	_entry_mode_set |= HD44780_ENTRY_I_D;
	instreg_wr(_entry_mode_set, 37);
}

// This is for text that flows Right to Left
void jm_LCM2004A_I2C::rightToLeft(void) {
//  _displaymode &= ~LCD_ENTRYLEFT;
//  command(LCD_ENTRYMODESET | _displaymode);
	_entry_mode_set &= ~HD44780_ENTRY_I_D;
	instreg_wr(_entry_mode_set, 37);
}

// This will 'right justify' text from the cursor
void jm_LCM2004A_I2C::autoscroll(void) {
//  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
//  command(LCD_ENTRYMODESET | _displaymode);
	_entry_mode_set |= HD44780_ENTRY_S;
	instreg_wr(_entry_mode_set, 37);
}

// This will 'left justify' text from the cursor
void jm_LCM2004A_I2C::noAutoscroll(void) {
//  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
//  command(LCD_ENTRYMODESET | _displaymode);
	_entry_mode_set &= ~HD44780_ENTRY_S;
	instreg_wr(_entry_mode_set, 37);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void jm_LCM2004A_I2C::createChar(byte location, byte charmap[]) {
//  location &= 0x7; // we only have 8 locations 0-7
//  command(LCD_SETCGRAMADDR | (location << 3));
//  for (int i=0; i<8; i++) {
//    write(charmap[i]);
//  }
	write_cgram(location, 1, charmap);
}

/*********** mid level commands, for sending data/cmds */

void jm_LCM2004A_I2C::command(byte value) {
//  send(value, LOW);
	instreg_wr(value, 0);
}

//size_t jm_LCM2004A_I2C::write(byte value) {
//  send(value, HIGH);
//  return 1; // assume success
//}
// already defined.

//------------------------------------------------------------------------------

// END.
