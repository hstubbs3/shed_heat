const uint8_t shed_font_4x5_Bitmaps[] PROGMEM = {
    0b00000000, // ' '  0

    0b11010110, 0b11110110, // '*'  1,2
    0b10110100, // '"' (3x2)  3

    0b01011111, 0b10011111, 0b10100000, // '#' 4-6

    0b01111010, 0b01100101, // '$' 7-8
    0b11100001, 0b01100000, // '?' 9-10
    0b01101000, 0b10001000, // '(' 11-12
    0b01100001, 0b00010001, // ')' 13-14
    0b01101111, // '+' (4x3)  15
    0b01100001, 0b11100000, // ':' (3x3) ;' (3x4) 16-17

    0b10010010, 0b01001001, // '%' 18-19
    0b10010101, 0b01101010, // 'X' 20-21
    0b10011001, 0b10011111, // 'W' 22-23
    0b11110100, 0b00100010, // 'T' 24-25
    0b00101100, // ',' (4x2) 26

    0b00110110, 0b11000110, 0b00110000, // '<' 27-29

    0b11110000, // '=' (4x3)  30
    0b11111100, // '!' (2x5) '-' (4x1) '.' (2x2) '_' (4x1) 31
    0b11000110, 0b00110110, 0b11000000, // '>' 32-34

    0b11110100, 0b00100010, // 'I' 35-36
    0b11110001, 0b00011001, // 'J' 37-38
    0b01101001, 0b10001001, // 'C' 39-40
    0b01101001, 0b10001001, // 'G' 40-41
    0b11111000, 0b11101000, // 'E' 42-43
    0b11111000, 0b11101000, // 'F' 44-45
    0b10001000, 0b10001000, // 'L' 46-47
    0b11111111, 0b11111001, // 'M'
    0b10011101, 0b10111011, // 'N'
    0b10011001, 0b11111001, // 'H'
    0b10011010, 0b11001010, // K
    0b10011001, 0b11110001, // '4'
    0b00010010, 0b01001000, // '/'
    0b10000100, 0b00100001, 0b00010000, // '\'

    0b10011001, 0b11110110, // 'Y'
    0b01101001, 0b00101001, // '3'
    0b01101101, 0b10111011, // '0'
    0b01101001, 0b00100100, // 2' 
    0b11111000, 0b11100001, // '1'  (1x5) '5'
    0b01101001, 0b01101001, // '8'
    0b01101000, 0b11101001, // '6'    
    0b01101001, 0b01110001, // '9'
    0b01101001, 0b11111001, // 'A'
    0b10011001, 0b10011001, // 'U'
    0b01101001, 0b10011011, // 'Q'
    0b01111000, 0b01100001, // 'S'
    0b11101001, 0b11111010, // 'R'
    0b10011001, 0b10011010, // 'V'
    0b01101001, 0b01111011, // '&'
    0b01111000, 0b10111011, // '@'
    0b11110001, 0b01101000, // 'Z'
    0b11111001, 0b10011001, // 'O'
    0b11110111, // ' (using as degree mark) (3x3) 
    0b11101001, 0b11101001, // 'B'
    0b11101001, 0b10011001, // 'D' 
    0b11101001, 0b11111000, 0b10000000, // 'P'

    0b11101000, 0b10001000, 0b10000000, // '['

    0b01110001, 0b00010001, 0b01110000, // ']'

    0b01010100, // '^' (3x2)

};

const GFXglyph shed_font_4x5_Glyphs[] PROGMEM = {
  {     0,   1,   1,   4,    0,   -5 },   // 0x20 ' '
  {     0,   1,   1,   4,    0,   -5 },   // 0x21 '!'
  {     0,   1,   1,   4,    0,   -5 },   // 0x22 '"'
  {     0,   1,   1,   4,    0,   -5 },   // 0x23 '#'
  {     0,   1,   1,   4,    0,   -5 },   // 0x24 '$'
  {     0,   1,   1,   4,    0,   -5 },   // 0x25 '%'
  {     0,   1,   1,   4,    0,   -5 },   // 0x26 '&'
  {     0,   1,   1,   4,    0,   -5 },   // 0x27 ''' as degree...
  {     0,   1,   1,   4,    0,   -5 },   // 0x28 '('
  {     0,   1,   1,   4,    0,   -5 },   // 0x29 ')'
  {     0,   1,   1,   4,    0,   -5 },   // 0x2A '*'
  {     0,   1,   1,   4,    0,   -5 },   // 0x2B '+'
  {     0,   1,   1,   4,    0,   -5 },   // 0x2C ','
  {     0,   1,   1,   4,    0,   -5 },   // 0x2D '-'
  {     0,   1,   1,   4,    0,   -5 },   // 0x2E '.'
  {     0,   1,   1,   4,    0,   -5 },   // 0x2F '/'
  {     0,   4,   5,   4,    0,   -5 },   // 0x30 '0'
  {     0,   1,   5,   4,    0,   -5 },   // 0x31 '1'
  {     0,   1,   1,   4,    0,   -5 },   // 0x32 '2'
  {     0,   1,   1,   4,    0,   -5 },   // 0x33 '3'
  {     0,   1,   1,   4,    0,   -5 },   // 0x34 '4'
  {     0,   1,   1,   4,    0,   -5 },   // 0x35 '5'
  {     0,   1,   1,   4,    0,   -5 },   // 0x36 '6'
  {     0,   1,   1,   4,    0,   -5 },   // 0x37 '7'
  {     0,   1,   1,   4,    0,   -5 },   // 0x38 '8'
  {     0,   1,   1,   4,    0,   -5 },   // 0x39 '9'
  {     0,   1,   1,   4,    0,   -5 },   // 0x3A ':'
  {     0,   1,   1,   4,    0,   -5 },   // 0x3B ';'
  {     0,   1,   1,   4,    0,   -5 },   // 0x3C '<'
  {     0,   1,   1,   4,    0,   -5 },   // 0x3D '='
  {     0,   1,   1,   4,    0,   -5 },   // 0x3E '>'
  {     0,   1,   1,   4,    0,   -5 },   // 0x3F '?'
  {     0,   1,   1,   4,    0,   -5 },   // 0x40 '@'
  {     0,   1,   1,   4,    0,   -5 },   // 0x41 'A'
  {     0,   1,   1,   4,    0,   -5 },   // 0x42 'B'
  {     0,   1,   1,   4,    0,   -5 },   // 0x43 'C'
  {     0,   1,   1,   4,    0,   -5 },   // 0x44 'D'
  {     0,   1,   1,   4,    0,   -5 },   // 0x45 'E'
  {     0,   1,   1,   4,    0,   -5 },   // 0x46 'F'
  {     0,   1,   1,   4,    0,   -5 },   // 0x47 'G'
  {     0,   1,   1,   4,    0,   -5 },   // 0x48 'H'
  {     0,   1,   1,   4,    0,   -5 },   // 0x49 'I'
  {     0,   1,   1,   4,    0,   -5 },   // 0x4A 'J'
  {     0,   1,   1,   4,    0,   -5 },   // 0x4B 'K'
  {     0,   1,   1,   4,    0,   -5 },   // 0x4C 'L'
  {     0,   1,   1,   4,    0,   -5 },   // 0x4D 'M'
  {     0,   1,   1,   4,    0,   -5 },   // 0x4E 'N'
  {     0,   1,   1,   4,    0,   -5 },   // 0x4F 'O'
  {     0,   1,   1,   4,    0,   -5 },   // 0x50 'P'
  {     0,   1,   1,   4,    0,   -5 },   // 0x51 'Q'
  {     0,   1,   1,   4,    0,   -5 },   // 0x52 'R'
  {     0,   1,   1,   4,    0,   -5 },   // 0x53 'S'
  {     0,   1,   1,   4,    0,   -5 },   // 0x54 'T'
  {     0,   1,   1,   4,    0,   -5 },   // 0x55 'U'
  {     0,   1,   1,   4,    0,   -5 },   // 0x56 'V'
  {     0,   1,   1,   4,    0,   -5 },   // 0x57 'W'
  {     0,   1,   1,   4,    0,   -5 },   // 0x58 'X'
  {     0,   1,   1,   4,    0,   -5 },   // 0x59 'Y'
  {     0,   1,   1,   4,    0,   -5 },   // 0x5A 'Z'
  {     0,   1,   1,   4,    0,   -5 },   // 0x5B '['
  {     0,   1,   1,   4,    0,   -5 },   // 0x5C '\'
  {     0,   1,   1,   4,    0,   -5 },   // 0x5D ']'
  {     0,   1,   1,   4,    0,   -5 },   // 0x5E '^'
  {     0,   1,   1,   4,    0,   -5 }    // 0x5F '_'  
};


const GFXfont shed_font_4x5 PROGMEM = {(uint8_t *) shed_font_4x5_Bitmaps,  (GFXglyph *)shed_font_4x5_Glyphs, 0x20, 0x5A,  6};