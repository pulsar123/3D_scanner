#ifndef THREE_D_SCANNER_H
#define THREE_D_SCANNER_H

/* PCB pins

  - Stepper driver:
    Enable: D9
    Step: D5
    DIR: D6
    MS1: D8
    MS2: HIGH permanent
    MS3: D7

  - Nokia 5110:
    Reset: D4
    CE: LOW permanent
    DC: D2
    DIN: D0
    CLK: D1
    LED: D3

  - 4x4 keypad:
  -- columns:
    pin 1: D13
    pin 2: A0
    pin 3: A1
    pin 4: A2
  -- rows:
    pin 5: A3
    pin 6: D12
    pin 7: D11
    pin 8: D10

  - Camera / flash:
    AF camera: A4
    Flash: A5

  - Optional input pins:
    pin 2: A7
    pin 3: A6

*/


//////// Keypad stuff: ////////
/*  Keys:
  1/2: Time between shots, s
  3/A: Shots per rotation
  4/5: Register
  6/B: Number of additional shots
  7/8: Backlighting
  9/C:
   / D: rewind / fast forward
  0:
  #: start/stop
*/
const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
//byte rowPins[rows] = {A3, 12, 11, 10}; //connect to the row pinouts of the keypad
//byte colPins[cols] = {13, A0, A1, A2}; //connect to the column pinouts of the keypad; to make pin 13 usable with membrane keypad, one has to remove the "L" LED from the Arduino nano board with pliers
byte rowPins[rows] = {10, 11, 12, A3}; //connect to the row pinouts of the keypad
byte colPins[cols] = {A2, A1, A0, 13}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );
const unsigned long T_KEY_LAG = 500000; // time in us to keep a parameter change key pressed before it will start repeating
const unsigned long T_KEY_REPEAT = 100000; // time interval in us for repeating with parameter change keys

//////// LCD stuff: ////////
// LCD pins (Nokia 5110): following resistor scenario in https://learn.sparkfun.com/tutorials/graphic-lcd-hookup-guide
const int8_t PIN_LCD_DC = 2;  // Via 10 kOhm resistor
const int8_t PIN_LCD_LED = 3;  // Via 330 Ohm resistor
const int8_t PIN_LCD_DN_ = 0;  // Via 10 kOhm resistor
const int8_t PIN_LCD_SCL = 1;  // Via 10 kOhm resistor
const int8_t PIN_LCD_RST = 4;
#define SOFTWARE_SPI
// Create a pcd8544 object.
// The LCD has 6 lines (rows) and 14 columns
// Pin 10 has to be unused (will be used internally)
#ifdef SOFTWARE_SPI
// Software SPI emulation:
//pcd8544 lcd(PIN_LCD_DC, PIN_LCD_RST, PIN_LCD_SCE, PIN_LCD_DN_, PIN_LCD_SCL);
#else
// Hardware SPI
//pcd8544 lcd(PIN_LCD_DC, PIN_LCD_RST, PIN_LCD_SCE);
#endif
// Backlight levels:
const byte N_BACKLIGHT = 4;
const byte Backlight[] = {0, 110, 127, 255};


Adafruit_PCD8544 lcd = Adafruit_PCD8544(PIN_LCD_SCL, PIN_LCD_DN_, PIN_LCD_DC, PIN_LCD_RST);
// Multipliers to get column/raw from x/y coords:
const byte COL = 6;
const byte ROW = 8;

// Constants:

// Pin assignment
// Stepper motor:
const byte ENABLE_PIN = 9;
const byte STEP_PIN = 5;
const byte DIR_PIN = 6;
const byte MS1_PIN = 8;
const byte MS3_PIN = 7;


// Camera AF (shutter half-press; to trigger shots in FRSP mode)
const byte AF_PIN = A5;
// Flash trigger
const byte FLASH_PIN = A4;

// Microstepping used:
const int N_MICROSTEPS = 32;
// Full steps per one rotation:
const int STEPS_PER_ROTATION = 200;

const int DT_SHOTS_MAX = 999; // Maximum time between shots, 0.1 s
const int N_SHOTS_MAX = 999; // Maximum number of shots
const int EXTRA_SHOTS_MAX = 99; // Maximum number of extra shots
const int N_REGS = 10; // Number of memory registers

const unsigned long int DT_MESSAGE = 2000000; // Time to display help messages in us

// Constraints:
//   DT_SHOTS > DT_AF
//   FLASH_DELAY+DT_FLASH <= DT_AF
// Time to half-press the camera shutter button, us
const unsigned long DT_AF = 2000000;
// Delay for the flash, from the moment the half-press is initiated, us
const unsigned long FLASH_DELAY = 1100000;
// Flash trigger duration, us:
const unsigned long DT_FLASH = 100000;


// Derived parameters:
const float NTOT_MICROSTEPS = (float)(STEPS_PER_ROTATION*N_MICROSTEPS); // Microsteps per full rotation


// Structure to have custom parameters saved to EEPROM
struct regist
{
  int dt_shots; // Time between shots, 0.1 s
  int N_shots; // Number of shots per rotation
  int extra_shots; // Number of extra shots after full rotation
};
short SIZE_REG = sizeof(regist);
// Addresses in EEPROM:
const int ADDR_IREG = N_REGS * SIZE_REG;
const int ADDR_BACKLIGHT = N_REGS * SIZE_REG + sizeof(int);


struct global
{
  struct regist reg; // Custom parameters register
  unsigned long int t_step; // time of the last step
  unsigned long int i_step; // the motor's microstep
  unsigned long int t_shoot_start; // Time when shooting started
  unsigned long int last_step; // Number of microsteps to make
  float dt_microstep;  // Time step for one microstep, us
  byte en;  // Motor enable flag
  int i_shot; // Shot number
  int N_tot; // Number of shots to take
  byte AF_on;  // AF on/off flag
  byte flash_on;  // flash on/off flag
  byte init; // =1 when we just initiated a run
  char key_old; // peviously pressed key; used in keypad()
  unsigned long t;  // Time in us measured at the beginning of motor_control() module
  unsigned long t0;  // Time shen the run started
  unsigned long t_key_pressed; // Last time when a key was pressed
  unsigned long t_last_repeat; // Last time when a key was repeated (for parameter change keys)
  unsigned int N_repeats; // Counter of key repeats
  int ireg; // Current register
  int backlight; // Backlight brightness (0-3)
  byte run; // =1 when motor running, 0 when stopped
  byte shooting; // =1 when shooting, 0 when not
  unsigned long t_message; // Time when a message was displayed
  byte message; // 1 if a message is displayed
  char buffer[15];  // char buffer
};

struct global g;

unsigned long int dt=0;
#endif

