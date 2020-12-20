// Controls
#define joyX A4
#define joyY A5
// Display
#define ROW_1 2
#define ROW_2 3
#define ROW_3 4
#define ROW_4 5
#define ROW_5 6
#define ROW_6 7
#define ROW_7 8
#define ROW_8 9
#define COL_1 10
#define COL_2 11
#define COL_3 12
#define COL_4 13
#define COL_5 A0
#define COL_6 A1
#define COL_7 A2
#define COL_8 A3

// Consts for Score
byte NONE[] = { B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111 };
byte nums[][8] = {
  { B11000011, B10111101, B10011101, B10101101, B10110101, B10111001, B10111101, B11000011 }, // 0
  { B11100111, B11100011, B11100111, B11100111, B11100111, B11100111, B11100111, B10000001 }, // 1
  { B11000011, B10111101, B10111111, B11011111, B11101111, B11110111, B11111011, B10000001 }, // 2
  { B11000011, B10111101, B10111111, B11000111, B10111111, B10111111, B10111101, B11000011 }, // 3
  { B10111101, B10111101, B10111101, B10111101, B10000001, B10111111, B10111111, B10111111 }, // 4
  { B10000001, B11111101, B11111101, B11000001, B10111111, B10111111, B10111101, B11000011 }, // 5
  { B11000011, B10111101, B11111101, B11000001, B10111101, B10111101, B10111101, B11000011 }, // 6
  { B10000001, B10111111, B10111111, B11011111, B11101111, B11101111, B11110111, B11110111 }, // 7
  { B11000011, B10111101, B10111101, B11000011, B10111101, B10111101, B10111101, B11000011 }, // 8
  { B11000011, B10111101, B10111101, B10111101, B10000011, B10111111, B10111101, B11000011 }  // 9
};

// Consts for Loops
const byte rows[] = { ROW_1, ROW_2, ROW_3, ROW_4, ROW_5, ROW_6, ROW_7, ROW_8 };
const byte col[] = { COL_1, COL_2, COL_3, COL_4, COL_5, COL_6, COL_7, COL_8 };
const byte bodyLength = 64;

int xBodies[bodyLength], yBodies[bodyLength];
int xPos, yPos, xDir = 1, yDir, xFood, yFood, visibleBody = 2, ticks;
byte board[8];
bool dead;

void setup() {
  Serial.begin(9600);

  for (byte i = 2; i <= 13; i++)
    pinMode(i, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);

  xFood = random(0, 8);
  yFood = random(1, 8);
}

void reset() {
  for (int i = 0; i < bodyLength; i++) {
    xBodies[i] = 0;
    yBodies[i] = 0;
  }

  // Clears Board
  for (int i = 0; i < 8; i++) board[i] = ~0;

  xPos = 0;
  yPos = 0;
  xDir = 1;
  yDir = 0;
  xFood = random(0, 8);
  yFood = random(1, 8);
  visibleBody = 2;
  ticks = 0;
  dead = false;

  delay(500);
}

void loop() {
  delay(5);

  if (dead) {
    int decimalPlace = visibleBody / 10;

    if (ticks < 100)
      if (decimalPlace != 0) drawScreen(nums[decimalPlace]);
      else ticks += 135;
    else if (ticks < 135) drawScreen(NONE);
    else if (ticks < 235) drawScreen(nums[visibleBody - (10 * decimalPlace)]);
    else reset();
  }
  else {
    // Gets Joystick Inputs and Sets Range from [0, 1023] to [-1, 1]
    int xValue = analogRead(joyX) / 1023.0 * 2 - 1;
    int yValue = analogRead(joyY) / 1023.0 * 2 - 1;

    /*/ Prints Input Values
      Serial.print(xValue);
      Serial.print("\t");
      Serial.println(yValue);
      //*/

    // Gets One Direction
    if (xValue != 0) {
      if (xValue != -xDir) {
        xDir = xValue;
        yDir = 0;
      }
    }
    else if (yValue != 0) {
      if (yValue != -yDir) {
        xDir = 0;
        yDir = yValue;
      }
    }

    if (ticks % 25 == 0) {
      ticks = 0;

      // Moves Body
      for (int i = 0; i < bodyLength - 1; i++) {
        xBodies[i] = xBodies[i + 1];
        yBodies[i] = yBodies[i + 1];
      }
      xBodies[bodyLength - 1] = xPos;
      yBodies[bodyLength - 1] = yPos;

      // Moves Player and Checks for Looping
      if (xDir != 0) {
        xPos += xDir;
        if (xPos < 0) xPos = 7;
        else if (xPos >= 8) xPos = 0;
      }
      else {
        yPos += yDir;
        if (yPos < 0) yPos = 7;
        else if (yPos >= 8) yPos = 0;
      }

      // Checks Collision
      for (int i = bodyLength - 1; i >= bodyLength - visibleBody; i--) {
        if (xPos == xBodies[i] && yPos == yBodies[i]) {
          ticks = 0;
          visibleBody++;
          dead = true;
        }
      }
      // Checks Eat Food
      if (xPos == xFood && yPos == yFood) {
        visibleBody++;

        while (xPos == xFood && yPos == yFood) {
          xFood = random(0, 8);
          yFood = random(0, 8);
        }
      }

      // Clears Board
      for (int i = 0; i < 8; i++) board[i] = ~0;

      // Places Player
      lightPoint(xPos, yPos);
      // Places Body
      for (int i = bodyLength - 1; i >= bodyLength - visibleBody; i--)
        lightPoint(xBodies[i], yBodies[i]);

      // Places Food
      lightPoint(xFood, yFood);
    }

    // Draws Screen
    drawScreen(board);
    //printScreen(board);
  }

  ticks++;
}

// Turns a Point On
void lightPoint(byte x, byte y) {
  board[y] &= ~(1UL << x);
}

// Draws Array to 8x8 Matrix
void  drawScreen(byte buffer2[]) {
  // Turn on each row in series
  for (byte i = 0; i < 8; i++) // count next row
  {
    digitalWrite(rows[i], HIGH);    // initiate whole row
    for (byte a = 0; a < 8; a++)    // count next row
    {
      digitalWrite(col[a], (buffer2[i] >> (7 - a)) & 0x01); // initiate whole column

      delayMicroseconds(100);       // uncoment deley for diferent speed of display

      digitalWrite(col[a], 1);      // reset whole column
    }
    digitalWrite(rows[i], LOW);     // reset whole row
    // otherwise last row will intersect with next row
  }
}

// Draws a Binary Representation of a Byte Array to Serial Monitor
void printScreen(byte b[]) {
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      Serial.print((b[y] >> x) & 0X01);
      Serial.print(" ");
    }
    Serial.println();
  }
  delay(2000);
}
