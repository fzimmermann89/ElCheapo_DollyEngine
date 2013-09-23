// motor pins
#define MOTOR0_P 15
#define MOTOR0_DIR 13

// camera pins
#define CAMERA_PIN 12
#define FOCUS_PIN 12
#define IR_PIN 12

// lcd pins
#define LCD_RS  8
#define LCD_EN  9
#define LCD_D4  4
#define LCD_D5  5
#define LCD_D6  6
#define LCD_D7  7
#define LCD_BKL 10

// button pin
#define BUT_PIN A0


// how many buttons dow we have?
#define NUM_BUTTONS 5

// button return values
#define BUT0  1
#define BUT1  2
#define BUT2  3
#define BUT3  4
#define BUT4  5

// which buttons?
#define BUT_UP BUT1
#define BUT_DN BUT2
#define BUT_CT BUT4
#define BUT_LT BUT3
#define BUT_RT BUT0

// analog button read values
#define BUT0_VAL  0
#define BUT1_VAL  100
#define BUT2_VAL  255
#define BUT3_VAL  405
#define BUT4_VAL  640

// button variance range
#define BUT_THRESH  40

// how many ms does a button have
// to be held before triggering another
// action? (for scrolling, etc.)
#define HOLD_BUT_MS 200

// how much to increment for each cycle the button is held?
#define HOLD_BUT_VALINC 10

// ALT input debouncing time
#define ALT_TRIG_THRESH 250

