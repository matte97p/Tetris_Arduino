// Define the variable to indicate if it is a cabinato setup
// #define iscabinato
#ifndef TETRIS_H
#define TETRIS_H

#include <Arduino.h>
#include <Wire.h>

// Joystick PS2
#define VRX A0
#define VRY A1
#define SW 12

// Control and response pins
#define BUZZER 13
#define PUSH 2
#define LED 8

// OLED display parameters
#define OLED_ADDRESS 0x3C                // I2C address of the OLED display
#define OLED_COMMAND 0x80                // Command prefix for OLED communication
#define OLED_DATA 0x40                   // Data prefix for OLED communication
#define OLED_DISPLAY_OFF 0xAE            // Command to turn off OLED display
#define OLED_DISPLAY_ON 0xAF             // Command to turn on OLED display
#define OLED_NORMAL_DISPLAY 0xA6         // Command for normal display mode
#define OLED_INVERSE_DISPLAY 0xA7        // Command for inverse display mode
#define OLED_SET_BRIGHTNESS 0x81         // Command to set OLED brightness
#define OLED_SET_ADDRESSING 0x20         // Command to set memory addressing mode
#define OLED_HORIZONTAL_ADDRESSING 0x00  // Parameter for horizontal memory addressing
#define OLED_VERTICAL_ADDRESSING 0x01    // Parameter for vertical memory addressing
#define OLED_PAGE_ADDRESSING 0x02        // Parameter for page memory addressing
#define OLED_SET_COLUMN 0x21             // Command to set column start and end address
#define OLED_SET_PAGE 0x22               // Command to set page start and end address

// Game graphical blocks
extern const bool BlockI[4][4];
extern const bool BlockJ[4][4];
extern const bool BlockL[4][4];
extern const bool BlockO[4][4];
extern const bool BlockS[4][4];
extern const bool BlockT[4][4];
extern const bool BlockZ[4][4];

// scoreTetris block font
extern const byte NumberFont[10][8] PROGMEM;

// Constants, variables, and control structures
#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_DOWN 3
#define KEY_ROTATE 4

extern byte uiKeyLeft;
extern byte uiKeyRight;
extern byte uiKeyDown;
extern byte uiKeyRotate;

// struct keyPress {
//   long left;
//   long right;
//   long down;
//   long rotate;
// };
// extern struct keyPress key;

// struct PieceSpace {
//   byte umBlock[4][4];  // Block matrix for current piece
//   char Row;            // Current row position of the piece
//   char Column;         // Current column position of the piece
// };
// extern PieceSpace currentPiece;
// extern PieceSpace oldPiece;
extern byte nextPiece;

extern byte pageArray[8];
extern byte scoreDisplayBuffer[8][6];
extern byte nextBlockBuffer[8][2];
extern bool optimizePageArray[8];
extern byte blockColumn[10];
extern byte tetrisScreen[14][25];
extern bool gameOverTetris;
extern bool gameStart;
extern unsigned long moveTime;
extern int pageStart;
extern int pageEnd;

extern int scoreTetris;
extern int acceleration;
extern int level;
extern int levelLineCount;
extern int dropDelay;

extern bool mainMenuTetris;

// Function prototypes
void playTetris();
void setupTetris();
void loopTetris();
void OLEDCommand(byte command);
void OLEDData(byte data);
void fillTetrisArray(byte value);
void fillTetrisScreen(byte value);
void drawTetrisScreen();
void drawTetrisLine(byte x);
void loadPiece(byte pieceNumber, byte row, byte column, bool loadScreen);
void drawPiece();
void drawLandedPiece();
void RotatePiece();
bool movePiece(char direction);
bool checkCollision();
void processCompletedLines();
void tetrisScreenToSerial();
bool processKeys();
void setScore(long scoreTetris, bool blank);
void setNextBlock(byte pieceNumber);
void drawBottom();
void drawSides();
void handleTetrisGameOver();

#endif // TETRIS_H
