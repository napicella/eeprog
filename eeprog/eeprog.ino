// eeprog - 28C64/28C256 EEPROM programmer firmware (for Arduino Nano)

////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////

// Note: constants for I/O pins match the net names used in the
// schematic

// Control pins for the 2 74HC595 shift registers used to
// generate addresses
#define SD0        2    // serial data input
#define SCLK0      3    // shift register clock
#define SCLR0      4    // clear shift register contents
#define RCLK0      5    // output register clock

// Control pins for the 74HC595 shift register used to
// send data to the data bus
#define SD2        6    // serial data input
#define SCLK2      7    // shift register clock
#define SCLR2      8    // clear shift register contents
#define RCLK2      9    // output register clock
#define DOE        12   // output enable

// EEPROM control pins
#define ROM_CE     10   // EEPROM chip enable
#define ROM_WE     11   // EEPROM write enable
#define ROM_OE     13   // EEPROM output enable

// I/O pins for 74HC165 shift register used to read data
// from the EEPROM
#define RDPL       A0   // parallel load
#define RDCP       A1   // shift register clock
#define RDIN       A2   // input from shift register

// Data direction modes
#define MODE_READ  0    // read data from EEPROM
#define MODE_WRITE 1    // write data to EEPROM

////////////////////////////////////////////////////////////////////////
// Data
////////////////////////////////////////////////////////////////////////

uint16_t g_addr;

////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////

void setDataDir(uint8_t mode) {
  if (mode == MODE_WRITE) {
    digitalWrite(ROM_OE, HIGH); // de-assert EEPROM output enable
    digitalWrite(DOE, LOW);     // assert 74HC595 data output enable
  } else {
    digitalWrite(DOE, HIGH);    // de-assert 74HC595 data output enable
    digitalWrite(ROM_OE, LOW);  // assert EEPROM output enable
  }
}

void pulse(uint8_t pin) {
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
}

void pulseLow(uint8_t pin) {
  digitalWrite(pin, LOW);
  digitalWrite(pin, HIGH);
}

void setAddr(uint16_t addr) {
  for (uint8_t i = 0; i < 16; i++) {
    digitalWrite(SD0, (addr & 0x1) ? HIGH : LOW);
    pulse(SCLK0);
    addr >>= 1;
  }
  pulse(RCLK0);
}

void setData(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(SD2, (data & 0x1) ? HIGH : LOW);
    pulse(SCLK2);
    data >>= 1;
  }
  pulse(RCLK2);
}

void setup() {
  // Configure pin modes
  pinMode(SD0, OUTPUT);
  pinMode(SCLK0, OUTPUT);
  pinMode(SCLR0, OUTPUT);
  pinMode(RCLK0, OUTPUT);
  pinMode(SD2, OUTPUT);
  pinMode(SCLK2, OUTPUT);
  pinMode(SCLR2, OUTPUT);
  pinMode(RCLK2, OUTPUT);
  pinMode(DOE, OUTPUT);
  pinMode(ROM_CE, OUTPUT);
  pinMode(ROM_WE, OUTPUT);
  pinMode(ROM_OE, OUTPUT);
  pinMode(RDPL, OUTPUT);
  pinMode(RDCP, OUTPUT);
  pinMode(RDIN, INPUT);

  // De-assert EEPROM chip enable and write enable
  digitalWrite(ROM_CE, HIGH);
  digitalWrite(ROM_WE, HIGH);

  // Set initial data direction
  setDataDir(MODE_WRITE);
  
  Serial.begin(57600);

  // clear shift register contents
  pulseLow(SCLR0);
  pulseLow(SCLR2);

  // set clock signals low initially
  digitalWrite(SCLK0, LOW);
  digitalWrite(RCLK0, LOW);
  digitalWrite(SCLK2, LOW);
  digitalWrite(RCLK2, LOW);
  digitalWrite(RDCP, LOW);

  // set 74HC165 parallel load high (not asserted) initially
  digitalWrite(RDPL, HIGH);
}

uint8_t x;

void loop() {
  // For now, just test output to 74HC595 shift registers
  setAddr(g_addr);
  setData(x);
  delay(100);
  g_addr++;
  x--;
}