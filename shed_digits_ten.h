//9x11 space per glyph - effective 8x10 usable with 1 px in between..
//so each line of a glyph is 1 byte.. for numbers..
//for letters for the labels, actually will use a 4x11 font (3x10 usable).. probably should encode those
// using 4x bitmaps and make them like 8 high only...
// will need a 'backspace' character for between the label and temperature though..

//coercing to height 12 to add another temp sensor without further mods needed..
const uint8_t shed_digits_Bitmaps[] PROGMEM = {
	0b01111100, 	// 0 - 1 using 2x8 bits
	0b00111101, 	

	0b01111110, 	// 2 - 9	
	0b10111101, // 1
	0b11000011, // 2
	0b11000011, // 3
	0b01111100, // 4
	0b01111100, // 5
	0b00000011, // 6
	0b00000011, // 7
	0b00111101, // 8
	0b01111110, // 2+8=10 	8
	0b10111101, // 1
	0b11000011, // 2
	0b11000011, // 3
	0b00111100, // 4
	0b00111100, // 5
	0b11000011, // 6
	0b11000011, // 7
	0b10111101, // 8
	0b01111110, // 18 	0
//todo
	0b10111101, // 1
	0b11000011, // 2
	0b11000011, // 3
	0b11000011, // 4
	0b00000000, // 5
	0b00000000, // 6
	0b11000011, // 7
	0b11000011, // 8
	0b11000011, // 9
	0b10111101, // 10
    // 36 - 2
	0b01111110, // 0 - 0
	0b00111101, // 1
	0b00000011, // 2
	0b00000011, // 3
	0b00000011, // 4
	0b01111100, // 5
	0b01111100, // 6
	0b11000000, // 7
	0b11000000, // 8
	0b11000000, // 9
	0b10111100, // 10
	// 47 -3
	0b01111110, // 0
	0b00111101, // 1
	0b00000011, // 2
	0b00000011, // 3
	0b00000011, // 4
	0b01111100, // 5
	0b01111100, // 6
	0b00000011, // 7
	0b00000011, // 8
	0b00000011, // 9
	0b00111101, // 10
	// 58 - 5
	0b01111110, // 0
	0b10111100, // 1
	0b11000000, // 2
	0b11000000, // 3
	0b11000000, // 4
	0b01111100, // 5
	0b01111100, // 6
	0b00000011, // 7
	0b00000011, // 8
	0b00000011, // 9
	0b00111101, // 10
	// 69 - 6
	0b01111110, // 0
	0b10111100, // 1
	0b11000000, // 2
	0b11000000, // 3
	0b11000000, // 4
	0b01111100, // 5
	0b01111100, // 6
	0b11000011, // 7
	0b11000011, // 8
	0b11000011, // 9
	0b10111101, // 10
	// 80 - 7
	0b01111110, // 0
	0b00111101, // 1
	0b00000011, // 2
	0b00000011, // 3
	0b00000011, // 4
	0b00000000, // 5
	0b00000000, // 6
	0b00000011, // 7
	0b00000011, // 8
	0b00000011, // 9
	0b00000001, // 10
	// 91 - 4
	0b10000001, // 0
	0b11000011, // 1
	0b11000011, // 2
	0b11000011, // 4
	0b01111100, // 5
	0b01111100, // 6
	0b00000011, // 7
	0b00000011, // 8
	0b00000011, // 9
	0b00000001, // 10
	// 102 - ! for °F
	0b01110111, // 0
	0b01010100, // 1
	0b01110110, // 2
	0b00000100, // 3
	0b00000100, // 4
	// 106 - R (4x8)
	0b11001100, // 0
	0b10101010, // 1
	0b11001100, // 2
	0b10101010, // 3
	// 110 - S (4x8)
	0b01100110,
	0b10000100,
	0b00100010,
	0b11001100,
	// 114 - H
	0b10101010,
	0b10101110,
	0b10101010,
	0b10101010,
	// 118 - E
	0b11101110,
	0b10001110,
	0b10001000,
	0b11101110,
	// 122 - D
	0b11001100, // 0
	0b10101010, // 1
	0b10101010, // 2
	0b11001100, // 3
	// 126 - A
	0b01000100, // 0
	0b10101010, // 1
	0b11101110, // 2
	0b10101010, // 3
	// 130 - N 
	0b10011101,
	0b11011011,
	0b10111011,
	0b10111001,
	// 134 - B (4x8)
	0b11001100, // 0
	0b10101100, // 1
	0b10101010, // 2
	0b11001100, // 3
	// 138 - O (4x8)
	0b01000100, // 0
	0b10101010, // 1
	0b10101010, // 2
	0b01000100, // 3
	// 142 - I (4x8)
	0b11101110,
	0b01000100,
	0b01000100,
	0b11101110,
	// 146	- F 
	0b11101110,
	0b10001000,
	0b11001100,
	0b10001000,
	// 150 - T
	0b11101110,
	0b01000100,
	0b01000100,
	0b01000100,
	// 154 - W 
	0b10011001,
	0b10011101,
	0b10111111,
	0b10101011,
	// 158 - M
	0b10101011,
	0b10111111,
	0b10011101,
	0b10011001,
	// 162 - V
	0b10101010,
	0b10101010,
	0b10101010,
	0b01001010,

};

const GFXglyph shed_digits_Glyphs[] PROGMEM = {
  {    0,    1,    1,   9,    0,   -10 },   // 0x20 ' '
  {  101,    8,    5,   9,   -1,   -10 },   // 0x21 '!' for °F .. x advance shortened to make it fit..
  {    2,    8,   12,   9,    0,   -10 },   // 0x22 '"' 
  {    3,    8,   12,   9,    0,   -10 },   // 0x23 '#'
  {    0,    0,    0,   0,    0,   -10 },   // 0x24 '$' 
  {    5,    8,   12,   9,    0,   -10 },   // 0x25 '%'
  {    6,    8,   12,   9,    0,   -10 },   // 0x26 '&'
  {    7,    8,   12,   9,    0,   -10 },   // 0x27 ''' 
  {    8,    8,   12,   9,    0,   -10 },   // 0x28 '('
  {    9,    8,   12,   9,    0,   -10 },   // 0x29 ')'
  {   10,    8,   12,   9,    0,   -10 },   // 0x2A '*'
  {   11,    8,   12,   9,    0,   -10 },   // 0x2B '+'
  {    0,    1,    1,   3,    0,   -10 },   // 0x2C ','
  {    0,    2,    1,   4,    0,    -4 },   // 0x2D '-' - need this for spacing stuff...
  {    0,    2,    1,   4,    0,   -10 },   // 0x2E '.' - using to space stuff
  {    0,    2,   10,   3,    0,   -10 },   // 0x2F '/' - grabbing this to use as alternate for '1'
  {   18,    8,   10,   9,    0,   -10 },   // 0x30 '0'
  {    0,    2,    8,   9,    6,    -9 },   // 0x32 '1'
  {   36,    8,   12,   9,    0,   -10 },   // 0x31 '2'
  {   47,    8,   12,   9,    0,   -10 },   // 0x33 '3'
  {   90,    8,   11,   9,    0,    -9 },   // 0x34 '4'
  {   58,    8,   12,   9,    0,   -10 },   // 0x35 '5'
  {   69,    8,   12,   9,    0,   -10 },   // 0x36 '6'
  {   80,    8,   11,   9,    0,   -10 },   // 0x37 '7'
  {   10,    8,   10,   9,    0,   -10 },   // 0x38 '8'
  {    2,    8,   10,   9,    0,   -10 },   // 0x39 '9'
  {   26,    3,    4,   4,    0,   -10 },   // 0x3A ':'
  {   27,    3,    4,   4,    0,   -10 },   // 0x3B ';'
  {   28,    3,    4,   4,    0,   -10 },   // 0x3C '<'
  {   29,    3,    4,   4,    0,   -10 },   // 0x3D '='
  {   30,    3,    4,   4,    0,   -10 },   // 0x3E '>'
  {   31,    3,    4,   4,    0,   -10 },   // 0x3F '?'
  {   32,    3,    4,   4,    0,   -10 },   // 0x40 '@'
  {  126,    4,    8,   4,    0,   -10 },   // 0x41 'A'
  {  134,    4,    8,   4,    0,   -10 },   // 0x42 'B'
  {    0,    4,    8,   4,    0,   -10 },   // 0x43 'C'
  {  122,    4,    8,   4,    0,   -10 },   // 0x44 'D'
  {  118,    4,    8,   4,    0,   -10 },   // 0x45 'E'
  {  146,    4,    8,   4,    0,   -10 },   // 0x46 'F'
  {    0,    4,    8,   4,    0,   -10 },   // 0x47 'G'
  {  114,    4,    8,   4,    0,   -10 },   // 0x48 'H'
  {  142,    4,    8,   4,    0,   -10 },   // 0x49 'I'
  {    0,    4,    8,   4,    0,   -10 },   // 0x4A 'J'
  {    0,    4,    8,   4,    0,   -10 },   // 0x4B 'K'
  {    0,    4,    8,   4,    0,   -10 },   // 0x4C 'L'
  {  158,    4,    8,   4,    0,   -10 },   // 0x4D 'M'
  {  130,    4,    8,   4,    0,   -10 },   // 0x4E 'N'
  {  138,    4,    8,   4,    0,   -10 },   // 0x4F 'O'
  {    0,    4,    8,   4,    0,   -10 },   // 0x50 'P'
  {    0,    4,    8,   4,    0,   -10 },   // 0x51 'Q'
  {  106,    4,    8,   4,    0,   -10 },   // 0x52 'R'
  {  110,    4,    8,   4,    0,   -10 },   // 0x53 'S'
  {  150,    4,    8,   4,    0,   -10 },   // 0x54 'T'
  {    0,    4,    8,   4,    0,   -10 },   // 0x55 'U'
  {  162,    4,    8,   4,    0,   -10 },   // 0x56 'V'
  {  154,    4,    8,   4,    0,   -10 },   // 0x57 'W'
  {    0,    4,    8,   4,    0,   -10 },   // 0x58 'X'
  {    0,    4,    8,   4,    0,   -10 },   // 0x59 'Y'
  {    0,    4,    8,   4,    0,   -10 },   // 0x5A 'Z'
};

const GFXfont shed_digits PROGMEM = {(uint8_t *) shed_digits_Bitmaps,  (GFXglyph *)shed_digits_Glyphs, 0x20, 0x5A,  11};