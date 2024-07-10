// Mini-Arcade TETRIS con display Oled
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Define the variable to indicate if it is a cabinato setup
// #define iscabinato

#include <Arduino.h>
#include <Wire.h>
#include "Tetris.h"

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
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Game graphical blocks
const bool BlockI[4][4] = {
  { 0, 1, 0, 0 },
  { 0, 1, 0, 0 },
  { 0, 1, 0, 0 },
  { 0, 1, 0, 0 },
};
const bool BlockJ[4][4] = {
  { 0, 1, 0, 0 },
  { 0, 1, 0, 0 },
  { 1, 1, 0, 0 },
  { 0, 0, 0, 0 },
};
const bool BlockL[4][4] = {
  { 0, 1, 0, 0 },
  { 0, 1, 0, 0 },
  { 0, 1, 1, 0 },
  { 0, 0, 0, 0 },
};
const bool BlockO[4][4] = {
  { 0, 0, 0, 0 },
  { 0, 1, 1, 0 },
  { 0, 1, 1, 0 },
  { 0, 0, 0, 0 },
};
const bool BlockS[4][4] = {
  { 0, 0, 0, 0 },
  { 0, 1, 1, 0 },
  { 1, 1, 0, 0 },
  { 0, 0, 0, 0 },
};
const bool BlockT[4][4] = {
  { 0, 0, 0, 0 },
  { 1, 1, 1, 0 },
  { 0, 1, 0, 0 },
  { 0, 0, 0, 0 },
};
const bool BlockZ[4][4] = {
  { 0, 0, 0, 0 },
  { 1, 1, 0, 0 },
  { 0, 1, 1, 0 },
  { 0, 0, 0, 0 },
};
// scoreTetris block font
const byte NumberFont[10][8] PROGMEM = {
  { 0x00, 0x1c, 0x22, 0x26, 0x2a, 0x32, 0x22, 0x1c },
  { 0x00, 0x1c, 0x08, 0x08, 0x08, 0x08, 0x0c, 0x08 },
  { 0x00, 0x3e, 0x02, 0x04, 0x18, 0x20, 0x22, 0x1c },
  { 0x00, 0x1c, 0x22, 0x20, 0x18, 0x20, 0x22, 0x1c },
  { 0x00, 0x10, 0x10, 0x3e, 0x12, 0x14, 0x18, 0x10 },
  { 0x00, 0x1c, 0x22, 0x20, 0x20, 0x1e, 0x02, 0x3e },
  { 0x00, 0x1c, 0x22, 0x22, 0x1e, 0x02, 0x04, 0x18 },
  { 0x00, 0x04, 0x04, 0x04, 0x08, 0x10, 0x20, 0x3e },
  { 0x00, 0x1c, 0x22, 0x22, 0x1c, 0x22, 0x22, 0x1c },
  { 0x00, 0x0c, 0x10, 0x20, 0x3c, 0x22, 0x22, 0x1c }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Constants, variables, and control structures
#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_DOWN 3
#define KEY_ROTATE 4

byte uiKeyLeft = 2;
byte uiKeyRight = 3;
byte uiKeyDown = 4;
byte uiKeyRotate = 5;

struct keyPress {
  long left;
  long right;
  long down;
  long rotate;
};
struct PieceSpace {
  byte umBlock[4][4];  // Block matrix for current piece
  char Row;            // Current row position of the piece
  char Column;         // Current column position of the piece
};

byte pageArray[8] = { 0 };                     // Array to store OLED display page data
byte scoreDisplayBuffer[8][6] = { { 0 } };     // Buffer for displaying scoreTetris on OLED
byte nextBlockBuffer[8][2] = { { 0 } };        // Buffer for displaying next block on OLED
bool optimizePageArray[8] = { 0 };             // Flag array to optimize OLED page update
byte blockColumn[10] = { 0 };                  // Array to manage block columns on the Tetris screen
byte tetrisScreen[14][25] = { { 1 }, { 1 } };  // Matrix representing the Tetris screen
PieceSpace currentPiece = { 0 };               // Current active Tetris piece
PieceSpace oldPiece = { 0 };                   // Previous state of the Tetris piece
byte nextPiece = 0;                            // ID of the next Tetris piece
keyPress key = { 0 };                          // Structure to manage button press times
bool gameOverTetris = false;                   // Flag indicating game over state
bool gameStart = true;                         // Flag indicating game start state
unsigned long moveTime = 0;                    // Timestamp for movement timing
int pageStart = 0;                             // Starting OLED page for update
int pageEnd = 0;                               // Ending OLED page for update

int scoreTetris = 0;    // Current game scoreTetris
int acceleration = 0;   // Acceleration factor for piece drop speed
int level = 0;          // Current game level
int levelLineCount = 0; // Number of lines cleared in the current level
int dropDelay = 1000;   // Initial delay between automatic piece drops

bool mainMenuTetris = false;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
/**
 * Used only in the cabinated version with more games
 */
void playTetris(){
  mainMenuTetris = false;
  level = 0;
  setupTetris();
  while(!mainMenuTetris) {
    loopTetris();
  }
}

#ifndef iscabinato
/**
 * @note    that 2 func are replace on cabinet version
 *          setup ->  setupTetris
 *          loop  ->  loopTetris
 */
void setup() {
  setupTetris();
}
void loop() {
  loopTetris();
}
#endif

/**
 * Setup function for Arduino initialization.
 * This function initializes serial communication, I2C communication, and various pins.
 * It also initializes the OLED display, sets up the Tetris screen, initializes random seed,
 * and sets pin modes for push button, buzzer, and LED.
 */
void setupTetris() {
  Serial.begin(9600);    // Initialize serial communication at 9600 baud rate
  while (!Serial) { ; }  // Wait for Serial to be ready

  Wire.begin();           // Initialize I2C communication
  Wire.setClock(400000);  // Set I2C clock speed to 400kHz

  // OLED display initialization sequence
  OLEDCommand(OLED_DISPLAY_OFF);   // Turn off display
  delay(20);
  OLEDCommand(OLED_DISPLAY_ON);    // Turn on display
  delay(20);
  OLEDCommand(OLED_NORMAL_DISPLAY);// Set display to normal mode
  delay(20);
  OLEDCommand(0x8D);               // Set charge pump regulator
  delay(20);
  OLEDCommand(0x14);               // Enable charge pump
  delay(20);
  OLEDCommand(OLED_NORMAL_DISPLAY);// Set display to normal mode again

  fillTetrisScreen(0);  // Initialize tetris screen with value 0

  randomSeed(analogRead(7));  // Seed the random number generator with analog input pin 7
  pinMode(PUSH, INPUT);       // Set push button pin mode to input
  pinMode(BUZZER, OUTPUT);    // Set buzzer pin mode to output
  pinMode(LED, OUTPUT);        // Set pin 13 (LED on some Arduino boards) mode to output
  digitalWrite(LED, HIGH);    // Turn on LED
  delay(100);
  digitalWrite(LED, LOW);     // Turn off LED
  delay(200);
  digitalWrite(LED, HIGH);    // Turn on LED
  delay(50);
  digitalWrite(LED, LOW);     // Turn off LED
}

/**
 * @brief Main game loop for Tetris.
 * 
 * This function initializes the game, displays the initial setup on the OLED screen,
 * handles game logic such as piece movements and scoring, and manages game over conditions.
 * 
 * Steps performed:
 * 1. Initializes game parameters and clears Tetris arrays.
 * 2. Prepares the initial Tetris screen display.
 * 3. Draws initial game visuals and sets up the OLED display.
 * 4. Sets up the initial piece and next piece to be displayed.
 * 5. Enters the game loop where it continuously moves the current piece down, 
 *    updates the display, and processes user input.
 * 6. Handles delays between piece movements based on game level.
 * 7. Ends the game when game over condition is met, plays a sequence of tones indicating 
 *    the end of the game.
 */
void loopTetris() {
  if(!mainMenuTetris) {
    gameOverTetris = false;  // Reset game over flag
    scoreTetris = 0;         // Reset scoreTetris
  }

  // Initialize and set up the Tetris game
  fillTetrisArray(1);                    // Clear Tetris array
  fillTetrisScreen(2);                   // Fill Tetris screen with initial background
  drawTetrisScreen();                    // Draw Tetris screen
  delay(200);                            // Delay for visual effect
  fillTetrisScreen(3);                   // Fill Tetris screen with different background
  drawTetrisScreen();                    // Draw Tetris screen
  delay(200);                            // Delay for visual effect
  drawSides();                           // Draw side borders on the screen
  drawBottom();                          // Draw bottom border on the screen
  tetrisScreenToSerial();                // Output Tetris screen to serial monitor
  OLEDCommand(OLED_INVERSE_DISPLAY);     // Inverse the OLED display for effect
  delay(200);                            // Delay for visual effect
  OLEDCommand(OLED_NORMAL_DISPLAY);      // Restore normal OLED display
  loadPiece(random(1, 7), 20, 5, true);  // Load initial Tetris piece
  drawTetrisScreen();                    // Draw Tetris screen with the piece
  nextPiece = random(1, 7);              // Generate next piece
  setNextBlock(nextPiece);               // Set up next piece display
  setScore(0, false);                    // Initialize and display initial scoreTetris
  delay(300);                            // Delay for visual effect
  setScore(0, true);                     // Alternate scoreTetris display for effect
  delay(300);                            // Delay for visual effect
  setScore(0, false);                    // Restore normal scoreTetris display

  // Game loop: continues until game over
  while (!gameOverTetris) {
    movePiece('D');       // Move current piece down
    drawPiece();          // Draw updated piece on screen
    drawTetrisScreen();   // Draw updated Tetris screen
    moveTime = millis();  // Reset move time for next piece movement

    // Process user input and manage piece drop timing
    while (millis() - moveTime < (dropDelay - (level * 50))) {
      processKeys();  // Process user inputs
    }
  }

  handleTetrisGameOver();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/**
 * Sends a command byte to the OLED display via I2C.
 * 
 * @param command The command byte to send.
 */
void OLEDCommand(byte command) {
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(OLED_COMMAND);  // Prefix byte indicating command mode
  Wire.write(command);       // Actual command byte
  Wire.endTransmission();
}

/**
 * Sends a data byte to the OLED display via I2C.
 * 
 * @param data The data byte to send.
 */
void OLEDData(byte data) {
  Wire.beginTransmission(OLED_ADDRESS);
  Wire.write(OLED_DATA);  // Prefix byte indicating data mode
  Wire.write(data);       // Actual data byte
  Wire.endTransmission();
}

/**
 * Fills the entire tetrisScreen array with the specified value.
 * The array dimensions are 14 columns by 24 rows.
 * The bottom 3 rows (rows 21 to 23) are specifically cleared to 0 after filling.
 * 
 * @param value The value to fill the tetrisScreen array with.
 */
void fillTetrisArray(byte value) {
  // Fill the entire tetrisScreen array with the specified value
  for (char r = 0; r < 24; r++) {
    for (char c = 0; c < 14; c++) {
      tetrisScreen[c][r] = value;
    }
  }

  // Clear the bottom 3 rows (rows 21 to 23) of tetrisScreen
  for (char r = 21; r < 24; r++) {
    for (char c = 0; c < 14; c++) {
      tetrisScreen[c][r] = 0;
    }
  }
}

/**
 * Fills the playable area of the tetrisScreen array with the specified value.
 * The playable area is defined as columns 2 to 11 (10 columns) and rows 1 to 20 (20 rows).
 * 
 * @param value The value to fill the playable area of the tetrisScreen array with.
 */
void fillTetrisScreen(byte value) {
  // Fill the playable area of tetrisScreen with the specified value
  for (int r = 1; r < 21; r++) {
    for (int c = 2; c < 12; c++) {
      tetrisScreen[c][r] = value;
    }
  }
}

/**
 * Updates the OLED display with the current state of the tetrisScreen array.
 * 
 * @details This function iterates through the tetrisScreen array to detect complete lines
 *          marked with values 2 or 3. If a complete line is found, it extracts the block data
 *          for that line into the blockColumn array and clears tetrisScreen at those positions.
 *          After processing a complete line, it calls drawTetrisLine() to draw the line on the OLED display.
 *          The function exits the loop after drawing the first complete line found in the screen.
 * 
 * @note    This function assumes the OLED display and necessary drawing functions are properly set up.
 *          It modifies the global variables tetrisScreen and blockColumn.
 */
void drawTetrisScreen() {
  for (byte r = 1; r < 21; r++) {
    for (byte c = 2; c < 12; c++) {
      if ((tetrisScreen[c][r] == 2) | (tetrisScreen[c][r] == 3)) {
        // Extract block data for the line and clear tetrisScreen if it's a complete line
        for (byte i = 0; i < 10; i++) {
          blockColumn[i] = tetrisScreen[i + 2][r];
          if (tetrisScreen[i + 2][r] == 3) tetrisScreen[i + 2][r] = 0;
        }
        // Draw the line on the OLED display
        drawTetrisLine((r - 1) * 6);
        break;  // Exit the loop after drawing the line
      }
    }
  }
}

/**
 * Updates the OLED display with Tetris blocks or clears them based on the blockColumn values.
 * 
 * This function clears and redraws Tetris blocks on the OLED display at a specified horizontal position (column).
 * It examines each column's block value in the blockColumn array and modifies the pageArray accordingly.
 * After updating the pageArray, it determines which pages need to be refreshed (marked by optimizePageArray),
 * calculates the start and end pages, sets up the OLED display addressing, and finally sends the updated
 * page data to the OLED display for rendering.
 * 
 * @param x Starting horizontal position (column) on the OLED display. This is internally incremented by 1 for OLED addressing.
 */
void drawTetrisLine(byte x) {
  memset(optimizePageArray, 0, 8);
  memset(pageArray, 0, 8);
  x++;

  //Disegna il blocco [colonna 0]
  if (blockColumn[0] == 2 | blockColumn[0] == 1) {
    pageArray[0] = pageArray[0] | B11111001;
    optimizePageArray[0] = 1;
  }
  //Cancella il blocco [colonna 0]
  if (blockColumn[0] == 3) {
    pageArray[0] = pageArray[0] | B00000001;
    pageArray[0] = pageArray[0] & B00000111;
    optimizePageArray[0] = 1;
  }
  //Disegna il blocco [colonna 1]
  if (blockColumn[1] == 2 | blockColumn[1] == 1) {
    pageArray[1] = pageArray[1] | B00111110;
    optimizePageArray[1] = 1;
  }
  //Cancella il blocco [colonna 1]
  if (blockColumn[1] == 3) {
    pageArray[1] = pageArray[1] & B11000001;
    optimizePageArray[1] = 1;
  }
  //Disegna il blocco [colonna 2]
  if (blockColumn[2] == 2 | blockColumn[2] == 1) {
    pageArray[1] = pageArray[1] | B10000000;
    optimizePageArray[1] = 1;
    pageArray[2] = pageArray[2] | B00001111;
    optimizePageArray[2] = 1;
  }
  //Cancella il blocco [colonna 2]
  if (blockColumn[2] == 3) {
    pageArray[1] = pageArray[1] & B01111111;
    optimizePageArray[1] = 1;
    pageArray[2] = pageArray[2] & B11110000;
    optimizePageArray[2] = 1;
  }
  //Disegna il blocco [colonna 3]
  if (blockColumn[3] == 2 | blockColumn[3] == 1) {
    pageArray[2] = pageArray[2] | B11100000;
    optimizePageArray[2] = 1;
    pageArray[3] = pageArray[3] | B00000011;
    optimizePageArray[3] = 1;
  }
  //Cancella il blocco [colonna 3]
  if (blockColumn[3] == 3) {
    pageArray[2] = pageArray[2] & B00011111;
    optimizePageArray[2] = 1;
    pageArray[3] = pageArray[3] & B11111100;
    optimizePageArray[3] = 1;
  }
  //Disegna il blocco [colonna 4]
  if (blockColumn[4] == 2 | blockColumn[4] == 1) {
    pageArray[3] = pageArray[3] | B11111000;
    optimizePageArray[3] = 1;
  }
  //Cancella il blocco [colonna 4]
  if (blockColumn[4] == 3) {
    pageArray[3] = pageArray[3] & B00000111;
    optimizePageArray[3] = 1;
  }
  //Disegna il blocco [colonna 5]
  if (blockColumn[5] == 2 | blockColumn[5] == 1) {
    pageArray[4] = pageArray[4] | B00111110;
    optimizePageArray[4] = 1;
  }
  //Cancella il blocco [colonna 5]
  if (blockColumn[5] == 3) {
    pageArray[4] = pageArray[4] & B11000001;
    optimizePageArray[4] = 1;
  }
  //Disegna il blocco [colonna 6]
  if (blockColumn[6] == 2 | blockColumn[6] == 1) {
    pageArray[4] = pageArray[4] | B10000000;
    optimizePageArray[4] = 1;
    pageArray[5] = pageArray[5] | B00001111;
    optimizePageArray[5] = 1;
  }
  //Cancella il blocco [colonna 6]
  if (blockColumn[6] == 3) {
    pageArray[4] = pageArray[4] & B01111111;
    optimizePageArray[4] = 1;
    pageArray[5] = pageArray[5] & B11110000;
    optimizePageArray[5] = 1;
  }
  //Disegna il blocco [colonna 7]
  if (blockColumn[7] == 2 | blockColumn[7] == 1) {
    pageArray[5] = pageArray[5] | B11100000;
    optimizePageArray[5] = 1;
    pageArray[6] = pageArray[6] | B00000011;
    optimizePageArray[6] = 1;
  }
  //Cancella il blocco [colonna 7]
  if (blockColumn[7] == 3) {
    pageArray[5] = pageArray[5] & B00011111;
    optimizePageArray[5] = 1;
    pageArray[6] = pageArray[6] & B11111100;
    optimizePageArray[6] = 1;
  }
  //Disegna il blocco [colonna 8]
  if (blockColumn[8] == 2 | blockColumn[8] == 1) {
    pageArray[6] = pageArray[6] | B11111000;
    optimizePageArray[6] = 1;
  }
  //Cancella il blocco [colonna 8]
  if (blockColumn[8] == 3) {
    pageArray[6] = pageArray[6] & B00000111;
    optimizePageArray[6] = 1;
  }
  //Disegna il blocco [colonna 9]
  if (blockColumn[9] == 2 | blockColumn[9] == 1) {
    pageArray[7] = pageArray[7] | B10111110;
    optimizePageArray[7] = 1;
  }
  //Cancella il blocco [colonna 9]
  if (blockColumn[9] == 3) {
    pageArray[7] = pageArray[7] | B10000000;  //create side wall
    pageArray[7] = pageArray[7] & B11000001;
    optimizePageArray[7] = 1;
  }

  // Determine the first and last pages that need to be updated
  byte pageStart = 0;
  byte pageEnd = 0;
  for (int page = 0; page < 8; page++) {
    if (optimizePageArray[page]) {
      pageStart = page;
      break;
    }
  }
  for (int page = 7; page >= 0; page--) {
    if (optimizePageArray[page]) {
      pageEnd = page;
      break;
    }
  }

  // Set OLED display addressing
  OLEDCommand(OLED_SET_ADDRESSING);
  OLEDCommand(OLED_VERTICAL_ADDRESSING);
  OLEDCommand(OLED_SET_COLUMN);
  OLEDCommand(x);
  OLEDCommand(x + 4);
  OLEDCommand(OLED_SET_PAGE);
  OLEDCommand(pageStart);
  OLEDCommand(pageEnd);

  // Draw the updated pages on the OLED display
  for (int col = 0; col < 5; col++) {
    for (int page = pageStart; page <= pageEnd; page++) {
      OLEDData(pageArray[page]);
    }
  }
}

/**
 * Loads a Tetris piece into the currentPiece structure and optionally updates the game screen.
 * The piece is selected based on the given pieceNumber and copied into currentPiece.umBlock.
 * @param pieceNumber Number representing the Tetris piece to load (1 to 7).
 * @param row Row position to start loading the piece on the game screen.
 * @param column Column position to start loading the piece on the game screen.
 * @param loadScreen Flag to indicate whether to update the game screen with the loaded piece.
 */
void loadPiece(byte pieceNumber, byte row, byte column, bool loadScreen) {
  // Copy the chosen Tetris piece into currentPiece.umBlock based on pieceNumber
  switch (pieceNumber) {
    case 1: memcpy(currentPiece.umBlock, BlockI, 16); break;
    case 2: memcpy(currentPiece.umBlock, BlockJ, 16); break;
    case 3: memcpy(currentPiece.umBlock, BlockL, 16); break;
    case 4: memcpy(currentPiece.umBlock, BlockO, 16); break;
    case 5: memcpy(currentPiece.umBlock, BlockS, 16); break;
    case 6: memcpy(currentPiece.umBlock, BlockT, 16); break;
    case 7: memcpy(currentPiece.umBlock, BlockZ, 16); break;
    default: return;  // Invalid pieceNumber, do nothing
  }

  // Set the initial row and column positions for the piece
  currentPiece.Row = row;
  currentPiece.Column = column;

  // If loadScreen flag is set, update the game screen with the new piece
  if (loadScreen) {
    // Store the current piece before updating the screen
    oldPiece = currentPiece;

    // Draw the piece on the screen
    for (byte blockRow = 0; blockRow < 4; blockRow++) {
      for (byte blockColumn = 0; blockColumn < 4; blockColumn++) {
        if (currentPiece.umBlock[blockRow][blockColumn]) {
          // Calculate screen coordinates and draw the piece
          byte screenRow = row + blockRow;
          byte screenColumn = column + blockColumn;
          tetrisScreen[screenRow][screenColumn] = 2;
        }
      }
    }
  }
}

/**
 * Updates the tetrisScreen with the positions of the old and current pieces.
 * Erases the previous position of the old piece and draws the current position of the new piece.
 * 
 * @details This function iterates through the umBlock matrices of oldPiece and currentPiece
 *          to update the tetrisScreen array, which represents the game screen.
 *          For the old piece, it erases its previous position by setting the corresponding
 *          tetrisScreen elements to 3 (indicating erasure).
 *          For the current piece, it draws it by setting the corresponding tetrisScreen elements to 2.
 *          This function assumes that tetrisScreen is already initialized and accessible globally.
 *          It relies on the global variables oldPiece and currentPiece to determine the position and shape
 *          of the pieces to be drawn.
 */
void drawPiece() {
  byte pieceRow = 0;              // Row index of the piece's matrix
  byte pieceColumn = 0;           // Column index of the piece's matrix

  // Erase previous position of the old piece
  for (char c = oldPiece.Column; c < oldPiece.Column + 4; c++) {
    for (char r = oldPiece.Row; r < oldPiece.Row + 4; r++) {
      if (oldPiece.umBlock[pieceColumn][pieceRow]) {
        tetrisScreen[c][r] = 3;  // Set to 3 for erasure
      }
      pieceRow++;
    }
    pieceRow = 0;
    pieceColumn++;
  }

  // Draw current position of the new piece
  pieceRow = 0;                  // Reset pieceRow for the current piece
  pieceColumn = 0;               // Reset pieceColumn for the current piece

  // Draw current piece on the screen
  for (char c = currentPiece.Column; c < currentPiece.Column + 4; c++) {
    for (char r = currentPiece.Row; r < currentPiece.Row + 4; r++) {
      if (currentPiece.umBlock[pieceColumn][pieceRow]) {
        tetrisScreen[c][r] = 2;  // Set to 2 for drawing current piece
      }
      pieceRow++;
    }
    pieceRow = 0;
    pieceColumn++;
  }
}

/**
 * Draws the landed Tetris piece on the Tetris screen and processes completed lines.
 * 
 * @details This function iterates through the umBlock matrix of currentPiece
 *          to draw the landed piece on the tetrisScreen array, setting elements to 1.
 *          After drawing, it calls processCompletedLines() to handle line completion
 *          and scoring based on the updated tetrisScreen.
 *          This function assumes that tetrisScreen is already initialized and accessible globally.
 *          It uses the global variable currentPiece to determine the position and shape
 *          of the piece to be drawn.
 */
void drawLandedPiece() {
  byte pieceRow = 0;              // Row index of the piece's matrix
  byte pieceColumn = 0;           // Column index of the piece's matrix

  // Draw landed piece on tetrisScreen
  for (char c = currentPiece.Column; c < currentPiece.Column + 4; c++) {
    for (int r = currentPiece.Row; r < currentPiece.Row + 4; r++) {
      if (currentPiece.umBlock[pieceColumn][pieceRow]) {
        tetrisScreen[c][r] = 1;  // Set to 1 for landed piece
      }
      pieceRow++;
    }
    pieceRow = 0;
    pieceColumn++;
  }

  // Process completed lines and update screen
  processCompletedLines();
}

/**
 * Rotates the current Tetris piece 90 degrees clockwise.
 * Saves the current piece state before rotation.
 * Checks for collisions after rotation and reverts to the previous state if a collision occurs.
 * Implements LED blinking logic during rotation.
 */
bool led = true;
void RotatePiece() {
  byte i, j;
  byte rotatedPiece[4][4] = { 0 };  // Temporary array to store rotated piece

  // Save the current piece state
  memcpy(oldPiece.umBlock, currentPiece.umBlock, 16);
  oldPiece.Row = currentPiece.Row;
  oldPiece.Column = currentPiece.Column;

  // Rotate the piece 90 degrees clockwise
  for (i = 0; i < 4; ++i) {
    for (j = 0; j < 4; ++j) {
      rotatedPiece[j][i] = currentPiece.umBlock[4 - i - 1][j];
    }
  }

  // Update the current piece with the rotated version
  memcpy(currentPiece.umBlock, rotatedPiece, 16);

  // Check collision after rotation and revert if necessary
  if (checkCollision()) {
    currentPiece = oldPiece;  // Revert to the previous piece state
  }

  // LED blink logic
  if (led) {
    digitalWrite(LED, HIGH);
    led = false;
  } else {
    digitalWrite(LED, LOW);
    led = true;
  }
  delay(1);
  digitalWrite(LED, LOW);  // Ensure LED is turned off after rotation
}

/**
 * Moves the current Tetris piece in the specified direction.
 * Handles collision detection with existing blocks on the Tetris screen.
 * Handles piece landing, loading the next piece, and checks for game over conditions as necessary.
 * 
 * @param direction The direction to move the piece ('L' for left, 'R' for right, 'D' for down).
 * @return True if the piece has landed, false otherwise.
 */
bool movePiece(char direction) {
  bool pieceLanded = false;
  oldPiece = currentPiece;  // Save the current piece state

  switch (direction) {
    case 'L':  // Move left
      currentPiece.Column = currentPiece.Column - 1;
      break;
    case 'R':  // Move right
      currentPiece.Column = currentPiece.Column + 1;
      break;
    case 'D':  // Move down
      currentPiece.Row = currentPiece.Row - 1;
      break;
    default:
      // Handle invalid direction (optional)
      break;
  }

  // Check collision with existing blocks
  if (checkCollision()) {
    currentPiece = oldPiece;  // Revert to old piece state
    if (direction == 'D') {
      drawLandedPiece();   // Draw landed piece
      pieceLanded = true;  // Piece has landed
    }
  }

  // Handle piece landing
  if (pieceLanded) {
    loadPiece(nextPiece, 19, 4, false);  // Load next piece at top
    delay(100);
    acceleration = 0;

    // Check for game over condition
    if (checkCollision()) gameOverTetris = true;
    else {
      loadPiece(nextPiece, 19, 4, true);  // Load next piece normally
      acceleration = 0;
    }

    // Generate new next piece and update display
    nextPiece = random(1, 7);
    setNextBlock(nextPiece);
  }

  return pieceLanded;
}

/**
 * Checks for collision between the current Tetris piece and existing blocks on the Tetris screen.
 * Returns true if collision is detected, false otherwise.
 */
bool checkCollision() {
  byte pieceRow = 0;
  byte pieceColumn = 0;
  char column = currentPiece.Column;
  char row = currentPiece.Row;

  // Iterate over the current piece's blocks
  for (char c = column; c < column + 4; c++) {
    for (char r = row; r < row + 4; r++) {
      if (currentPiece.umBlock[pieceColumn][pieceRow]) {
        if (tetrisScreen[c][r] == 1) return true;  // Collision detected
      }
      pieceRow++;
    }
    pieceRow = 0;
    pieceColumn++;
  }
  return false;  // No collision detected
}

/**
 * Processes completed lines in the Tetris game screen.
 * Detects and clears full lines, updates the game state, scores points, and adjusts the game level.
 */
void processCompletedLines() {
  char rowCheck = 0;
  char ColumnCheck = 0;
  bool fullLine = false;
  bool noLine = true;
  char linesProcessed = 0;
  char clearedLines = 0;
  char topRow = 0;
  char bottomRow = 0;
  char currentRow = 0;
  int amountScored = 0;

  // Determine the bottom row based on current piece position
  if (currentPiece.Row < 1) bottomRow = 1;
  else bottomRow = currentPiece.Row;

  // Check each row for completed lines
  for (int rowCheck = bottomRow; rowCheck < currentPiece.Row + 4; rowCheck++) {
    fullLine = true;
    // Check each column in the current row
    for (ColumnCheck = 2; ColumnCheck < 12; ColumnCheck++) {
      if (tetrisScreen[ColumnCheck][rowCheck] == 0) {
        fullLine = false;
        break;
      }
    }
    // If a full line is found, mark it and increment lines processed
    if (fullLine) {
      for (char c = 2; c < 12; c++) {
        tetrisScreen[c][rowCheck] = 3;  // Mark the full line
      }
      bottomRow = rowCheck + 1;  // Update the bottom row for further checks
      linesProcessed++;
      delay(77);  // Delay for visual effect
    }
    drawTetrisScreen();  // Update the Tetris screen after marking
  }

  // If lines were processed (full lines found), clear and shift remaining lines
  if (linesProcessed) {
    clearedLines = linesProcessed;
    while (clearedLines) {
      // Play sound effects for each cleared line
      tone(BUZZER, 1000, 40);
      delay(50);
      tone(BUZZER, 2000, 60);
      delay(50);
      tone(BUZZER, 500, 80);
      delay(50);

      // Shift lines above the cleared line downwards
      for (currentRow = 1; currentRow < 20; currentRow++) {
        noLine = true;
        for (char c = 2; c < 12; c++) {
          if (tetrisScreen[c][currentRow]) noLine = false;
        }
        if (noLine) {
          // Shift lines above down by one row
          for (int r = currentRow + 1; r < 20; r++) {
            for (char c = 2; c < 12; c++) {
              if (tetrisScreen[c][r]) tetrisScreen[c][r - 1] = 2;  // Mark to be shifted
              else tetrisScreen[c][r - 1] = 3;                     // Mark as cleared space
            }
          }
        }
      }

      // Clear marked lines and shift remaining blocks
      for (char r = 1; r < 24; r++) {
        for (char c = 2; c < 12; c++) {
          if (tetrisScreen[c][r] == 2) tetrisScreen[c][r] = 1;  // Shift blocks down
        }
      }
      clearedLines--;
      drawTetrisScreen();  // Update screen after shifting
    }
  }

  // Calculate scoreTetris based on the number of lines cleared
  switch (linesProcessed) {
    case 1: amountScored = 40 * (level + 1); break;
    case 2: amountScored = 100 * (level + 1); break;
    case 3: amountScored = 300 * (level + 1); break;
    case 4:
      amountScored = 1200 * (level + 1);
      OLEDCommand(OLED_INVERSE_DISPLAY);  // Flash screen for Tetris (4 lines cleared)
      delay(100);
      OLEDCommand(OLED_NORMAL_DISPLAY);
      break;
  }

  // Update scoreTetris display and game scoreTetris
  for (long s = scoreTetris; s < scoreTetris + amountScored; s = s + (1 * (level + 1))) {
    setScore(s, false);
  }
  scoreTetris = scoreTetris + amountScored;
  setScore(scoreTetris, false);

  // Update line count for level up check
  levelLineCount = levelLineCount + linesProcessed;
  if (levelLineCount > 10) {
    level++;             // Level up if necessary
    levelLineCount = 0;  // Reset line count for next level
    // Flash the screen to indicate level up
    OLEDCommand(OLED_INVERSE_DISPLAY);
    delay(100);
    OLEDCommand(OLED_NORMAL_DISPLAY);
    delay(100);
    OLEDCommand(OLED_INVERSE_DISPLAY);
    delay(100);
    OLEDCommand(OLED_NORMAL_DISPLAY);
  }

  // Clear Tetris screen for next piece
  for (char r = bottomRow; r <= topRow; r++) {
    for (char c = 2; c < 12; c++) {
      if (tetrisScreen[c][r]) tetrisScreen[c][r] = 1;  // Reset marked rows
    }
  }
}

/**
 * Prints the current state of the Tetris game screen to the Serial monitor.
 * Displays each cell's state (0-3) in a grid format.
 */
void tetrisScreenToSerial() {
  for (int r = 0; r < 24; r++) {
    for (int c = 0; c < 14; c++) {
      Serial.print(tetrisScreen[c][r], DEC);  // Print each cell's state
    }
    Serial.println();  // Move to next row in the output
  }
  Serial.println();  // Separate screen states for clarity in Serial monitor
}

/**
 * Processes joystick commands for Tetris gameplay.
 * Detects joystick movements and triggers corresponding actions such as moving left, right, down, or rotating.
 * Updates the game state and triggers necessary visual updates on the OLED screen.
 * 
 * @return True if a valid key was pressed and processed; otherwise, false.
 */
bool processKey = true;
int Debounce = 0;
bool processKeys() {
  char uiKeyCode = 0;      // Initialize variable to store detected key code
  bool keypressed = true;  // Flag indicating if a key press was detected

  // Constants for joystick thresholds and timing adjustments
  int leftRight = 300 - acceleration;
  int rotate = 700;
  int down = 110 - acceleration;

  // Read joystick analog and digital inputs
  int Val_X_J = analogRead(VRX);
  int Val_Y_J = analogRead(VRY);
  int Val_B_J = digitalRead(SW);

  // Joystick movement detection and key handling
  if (Val_X_J <= 200) {  // Joystick moved left
    Debounce++;          // Debounce counter for stable detection
    if (Debounce > 10) {
      if (processKey) {
        uiKeyCode = KEY_LEFT;
        key.left = millis();    // Record time of left movement
        tone(BUZZER, 500, 10);  // Emit a short beep
        delay(20);
      }
      if (millis() < key.left + leftRight) {
        processKey = false;  // Limit frequency of left movements
      } else {
        processKey = true;
        acceleration = acceleration + 70;                        // Adjust acceleration
        if (acceleration > leftRight) acceleration = leftRight;  // Limit acceleration
      }
    }
  } else if (Val_X_J >= 800) {  // Joystick moved right
    Debounce++;
    if (Debounce > 10) {
      if (processKey) {
        uiKeyCode = KEY_RIGHT;
        key.right = millis();   // Record time of right movement
        tone(BUZZER, 500, 10);  // Emit a short beep
        delay(20);
      }
      if (millis() < key.right + leftRight) {
        processKey = false;  // Limit frequency of right movements
      } else {
        processKey = true;
        acceleration = acceleration + 70;                        // Adjust acceleration
        if (acceleration > leftRight) acceleration = leftRight;  // Limit acceleration
      }
    }
  } else if (Val_Y_J >= 800) {  // Joystick moved down
    Debounce++;
    if (Debounce > 10) {
      if (processKey) {
        uiKeyCode = KEY_DOWN;
        key.down = millis();    // Record time of downward movement
        tone(BUZZER, 700, 30);  // Emit a longer beep
        delay(20);
      }
      if (millis() < key.down + down) {
        processKey = false;  // Limit frequency of downward movements
      } else {
        processKey = true;
        acceleration = acceleration + 40;              // Adjust acceleration
        if (acceleration > down) acceleration = down;  // Limit acceleration
      }
    }
  } else if (Val_Y_J <= 200) {  // Joystick moved up (typically for rotation)
    Debounce++;
    if (Debounce > 10) {
      if (processKey) {
        uiKeyCode = KEY_ROTATE;
        key.rotate = millis();  // Record time of rotation
        tone(BUZZER, 500, 10);  // Emit a short beep
        delay(20);
      }
      if (millis() < key.rotate + rotate) {
        processKey = false;  // Limit frequency of rotation
      } else {
        processKey = true;
      }
    }
  } else {
    acceleration = 0;  // Reset acceleration when no movement detected
    processKey = true;
    Debounce = 0;  // Reset debounce counter
  }

  // Handle the detected key code and trigger corresponding actions
  switch (uiKeyCode) {
    case KEY_LEFT: movePiece('L'); break;
    case KEY_RIGHT: movePiece('R'); break;
    case KEY_DOWN: movePiece('D'); break;
    case KEY_ROTATE: RotatePiece(); break;
    default: keypressed = false; break;
  }

  // If a valid key was pressed, update the display
  if (keypressed) {
    drawPiece();         // Redraw the game piece
    drawTetrisScreen();  // Update the Tetris game screen
  }

  return keypressed;  // Return whether a valid key was processed
}

/**
 * Sets the scoreTetris display on the OLED screen using a 7-segment style font.
 * 
 * @param scoreTetris The scoreTetris to display.
 * @param blank If true, clears the scoreTetris display; otherwise, displays the scoreTetris.
 */
void setScore(long scoreTetris, bool blank) {
  // Initialize variables for each digit of the scoreTetris
  long ones = (scoreTetris % 10);
  long tens = ((scoreTetris / 10) % 10);
  long hundreds = ((scoreTetris / 100) % 10);
  long thousands = ((scoreTetris / 1000) % 10);
  long tenthousands = ((scoreTetris / 10000) % 10);
  long hunderedthousands = ((scoreTetris / 100000) % 10);

  byte font = 0;
  char bytes_out[8];
  memset(scoreDisplayBuffer, 0, sizeof(scoreDisplayBuffer));

  // Display sixth digit of the scoreTetris
  for (int v = 0; v < 8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[hunderedthousands][v]);
  for (int i = 0; i < 8; i++) {
    scoreDisplayBuffer[i][0] |= bytes_out[i] >> 1;
  }

  // Display fifth digit of the scoreTetris
  for (int v = 0; v < 8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[tenthousands][v]);
  for (int i = 0; i < 8; i++) {
    scoreDisplayBuffer[i][0] |= (bytes_out[i] << 6);
    scoreDisplayBuffer[i][1] |= bytes_out[i] >> 1;
  }

  // Display fourth digit of the scoreTetris
  for (int v = 0; v < 8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[thousands][v]);
  for (int i = 0; i < 8; i++) {
    scoreDisplayBuffer[i][1] |= (bytes_out[i] << 6);
    scoreDisplayBuffer[i][2] |= bytes_out[i] >> 1;
  }

  // Display third digit of the scoreTetris
  for (int v = 0; v < 8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[hundreds][v]);
  for (int i = 0; i < 8; i++) {
    scoreDisplayBuffer[i][2] |= (bytes_out[i] << 6);
    scoreDisplayBuffer[i][3] |= bytes_out[i] >> 1;
  }

  // Display second digit of the scoreTetris
  for (int v = 0; v < 8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[tens][v]);
  for (int i = 0; i < 8; i++) {
    scoreDisplayBuffer[i][3] |= (bytes_out[i] << 6);
    scoreDisplayBuffer[i][4] |= bytes_out[i] >> 1;
  }

  // Display first digit of the scoreTetris
  for (int v = 0; v < 8; v++) bytes_out[v] = pgm_read_byte(&NumberFont[ones][v]);
  for (int i = 0; i < 8; i++) {
    scoreDisplayBuffer[i][4] |= (bytes_out[i] << 6);
    scoreDisplayBuffer[i][5] |= bytes_out[i] >> 1;
  }

  // Set OLED display parameters for the scoreTetris display
  OLEDCommand(OLED_SET_ADDRESSING);       // Set vertical addressing mode
  OLEDCommand(OLED_VERTICAL_ADDRESSING);  // Set vertical addressing mode
  OLEDCommand(OLED_SET_COLUMN);           // Set column start and end (120 to 127)
  OLEDCommand(120);
  OLEDCommand(127);
  OLEDCommand(OLED_SET_PAGE);  // Set page start and end (0 to 5)
  OLEDCommand(0);
  OLEDCommand(5);

  // Display the scoreTetris on the OLED
  for (int p = 0; p < 8; p++) {
    for (int c = 0; c < 6; c++) {
      if (blank) {
        OLEDData(0);  // Blank out the scoreTetris if requested
      } else {
        OLEDData(scoreDisplayBuffer[p][c]);  // Display the scoreTetris digit
      }
    }
  }
}

/**
 * Sets the next block to be displayed on the OLED screen based on the provided piece number.
 * Uses a predefined buffer to store the block pattern.
 * 
 * @param pieceNumber The number representing the type of Tetris piece (1 to 7).
 */
void setNextBlock(byte pieceNumber) {
  // Clear the buffer
  memset(nextBlockBuffer, 0, sizeof(nextBlockBuffer));

  // Define Tetris block patterns based on pieceNumber
  switch (pieceNumber) {
    case 1:
      // L Piece - 1
      for (int k = 2; k < 6; k++) {
        nextBlockBuffer[k][0] = B01110111;
        nextBlockBuffer[k][1] = B01110111;
      }
      break;
    case 2:
      // J Piece - 2
      for (int k = 0; k < 3; k++) {
        nextBlockBuffer[k][0] = B01110000;
        nextBlockBuffer[k][1] = B01110111;
      }
      for (int k = 4; k < 7; k++) {
        nextBlockBuffer[k][0] = B01110000;
      }
      break;
    case 3:
      // L Piece - 3
      for (int k = 0; k < 3; k++) {
        nextBlockBuffer[k][0] = B01110000;
      }
      for (int k = 4; k < 7; k++) {
        nextBlockBuffer[k][0] = B01110000;
        nextBlockBuffer[k][1] = B01110111;
      }
      break;
    case 4:
      // O Piece - 4
      for (int k = 0; k < 3; k++) {
        nextBlockBuffer[k][0] = B01110000;
        nextBlockBuffer[k][1] = B00000111;
      }
      for (int k = 4; k < 7; k++) {
        nextBlockBuffer[k][0] = B01110000;
        nextBlockBuffer[k][1] = B00000111;
      }
      break;
    case 5:
      // S Piece - 5
      for (int k = 0; k < 3; k++) {
        nextBlockBuffer[k][0] = B01110000;
        nextBlockBuffer[k][1] = B00000111;
      }
      for (int k = 4; k < 7; k++) {
        nextBlockBuffer[k][0] = B00000000;
        nextBlockBuffer[k][1] = B11101110;
      }
      break;
    case 6:
      // T Piece - 6
      for (int k = 0; k < 3; k++) {
        nextBlockBuffer[k][0] = B01110000;
        nextBlockBuffer[k][1] = B01110111;
      }
      for (int k = 4; k < 7; k++) {
        nextBlockBuffer[k][0] = B00000000;
        nextBlockBuffer[k][1] = B00001110;
      }
      break;
    case 7:
      // Z Piece - 7
      for (int k = 0; k < 3; k++) {
        nextBlockBuffer[k][0] = B01110000;
        nextBlockBuffer[k][1] = B00000111;
      }
      for (int k = 4; k < 7; k++) {
        nextBlockBuffer[k][0] = B11101110;
        nextBlockBuffer[k][1] = B00000000;
      }
      break;
  }

  // Set OLED display parameters for displaying the next block
  OLEDCommand(OLED_SET_ADDRESSING);       // Set vertical addressing mode
  OLEDCommand(OLED_VERTICAL_ADDRESSING);  // Set vertical addressing mode
  OLEDCommand(OLED_SET_COLUMN);           // Set column start and end (120 to 127)
  OLEDCommand(120);
  OLEDCommand(127);
  OLEDCommand(OLED_SET_PAGE);  // Set page start and end (6 to 7)
  OLEDCommand(6);
  OLEDCommand(7);

  // Display the next block pattern on the OLED
  for (int p = 0; p < 8; p++) {
    for (int c = 0; c < 2; c++) {
      OLEDData(nextBlockBuffer[p][c]);
    }
  }
}

/**
 * Draws a solid horizontal line at the bottom of the OLED display.
 * Uses vertical addressing mode and sets the column and page boundaries.
 */
void drawBottom() {
  // Set vertical addressing mode
  OLEDCommand(OLED_SET_ADDRESSING);
  OLEDCommand(OLED_VERTICAL_ADDRESSING);

  // Set column start and end (0 to 0)
  OLEDCommand(OLED_SET_COLUMN);
  OLEDCommand(0);
  OLEDCommand(0);

  // Set page start and end (0 to 7)
  OLEDCommand(OLED_SET_PAGE);
  OLEDCommand(0);
  OLEDCommand(7);

  // Draw a solid line across the bottom row (page 7)
  for (int column = 0; column < 128; column++) {
    OLEDData(255);  // 255 (0xFF) sets all pixels in the column to ON
  }
}

/**
 * Draws vertical lines on both sides of the OLED display.
 * Uses vertical addressing mode and sets the column and page boundaries.
 */
void drawSides() {
  // Set vertical addressing mode
  OLEDCommand(OLED_SET_ADDRESSING);
  OLEDCommand(OLED_VERTICAL_ADDRESSING);

  // Set left side columns (0 to 127)
  OLEDCommand(OLED_SET_COLUMN);
  OLEDCommand(0);
  OLEDCommand(127);

  // Set page start and end (0 to 7)
  OLEDCommand(OLED_SET_PAGE);
  OLEDCommand(0);
  OLEDCommand(7);

  // Draw vertical lines on both sides
  for (int row = 0; row < 128; row++) {
    for (int column = 0; column < 8; column++) {
      if (column == 0) {
        OLEDData(1);  // Turn on pixel for the leftmost column
      } else if (column == 7) {
        OLEDData(128);  // Turn on pixel for the rightmost column
      } else {
        OLEDData(0);  // Turn off pixels for columns in between
      }
    }
  }
}

/**
 * @brief Handles the game over sequence for Tetris.
 * 
 * This function executes a sequence of actions when the game is over:
 * - Inverses the OLED display to indicate game end visually.
 * - Plays a series of tones through the buzzer to create an audible game over effect.
 * - Delays between each tone to control the tempo of the sequence.
 * - After the sequence, sets mainMenuTetris flag to true for returning to the main menu.
 */
void handleTetrisGameOver() {
  OLEDCommand(OLED_INVERSE_DISPLAY);   // Inverse the OLED display for effect
  
  // Play tones indicating game over
  tone(BUZZER, 500, 200);   // Play tone
  delay(250);               // Delay between tones
  
  OLEDCommand(OLED_NORMAL_DISPLAY);    // Restore normal OLED display
  
  tone(BUZZER, 500, 200);   // Play tone
  delay(250);               // Delay between tones
  
  OLEDCommand(OLED_INVERSE_DISPLAY);   // Inverse the OLED display for effect
  
  tone(BUZZER, 500, 200);   // Play tone
  delay(250);               // Delay between tones
  
  OLEDCommand(OLED_NORMAL_DISPLAY);    // Restore normal OLED display
  
  tone(BUZZER, 800, 150);   // Play tone
  delay(200);               // Delay between tones
  
  OLEDCommand(OLED_INVERSE_DISPLAY);   // Inverse the OLED display for effect
  
  tone(BUZZER, 500, 500);   // Play tone
  delay(500);               // Delay between tones
  
  OLEDCommand(OLED_NORMAL_DISPLAY);    // Restore normal OLED display
  
  tone(BUZZER, 600, 1000);  // Play tone
  delay(2000);               // Delay after game over for tone sequence
  
  mainMenuTetris = true;    // Set flag to return to main menu
  #ifndef iscabinato
    gameOverTetris = false;
  #endif
}