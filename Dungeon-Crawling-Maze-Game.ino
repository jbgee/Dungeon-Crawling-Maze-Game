// Dungeon Crawling Maze Game 
// Written By Jackson B. Gee

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BLOCK_SIZE 8
#define ANALOGXPIN 0 //analog pin connected to x output 
#define ANALOGYPIN 1 //analog pin connected to y output

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int8_t SPEED_FACTOR = 1;

int8_t playerX = 6;       //Delcare the starting Position of the Figure!!
int8_t playerY = 6;

uint8_t endX = 3;              //Declare the Position of the ending flag
uint8_t endY = 6; 

uint8_t endXPanel = 0;         //Defines the Top Left Corner of the Panel in which the final flag resides
uint8_t endYPanel = 16;

uint8_t printOriginX = 0;      //Must also declare the Top Left Corner of the Panel You wish to start on !
uint8_t printOriginY = 16;

uint8_t player[8] ={B00000000, //Declares the Matrix used to draw the game figure
                 B00011000,
                 B00011000,
                 B01111110,
                 B00011000,
                 B00100100,
                 B01100110,
                 B00000000};
                        
uint8_t end[8] = {B00000000, //Defines the Matrix used to draw the end flags
                   B00011110,
                   B00011110,
                   B00011110,
                   B00010000,
                   B00010000,
                   B01111110,
                   B00000000};
    

                               
uint8_t maze[][6]={{B11111111,B11111111,B11111111,B11111111,B11111111,B11111111},  //Declare the matrix as unsigned char so that each value is stored in a single bit instead of 8 or 16
                     {B10001100,B00110101,B10100010,B00000001,B11010100,B00010101},  // Threfore instead of using 24x48 matrix of ints each using 16 bits each the new matrix
                     {B10100011,B10000001,B10001000,B10111101,B10000010,B10000101},  // uses 24x6 matrix of unsigned char using 8 bits each saving lots of Dynamic Memory
                     {B10101000,B10111101,B10111101,B10100001,B10111000,B10111101},
                     {B10101110,B00000101,B10100100,B10101011,B10101110,B10000001},
                     {B10111011,B01110101,B10101111,B10111000,B00101011,B11110111},
                     {B10001000,B00010100,B00100000,B10101101,B11000000,B11010001},
                     {B11101111,B11111111,B11111110,B11101111,B11111101,B11011101},
                     
                     {B11101111,B11111111,B11111110,B11101111,B11111101,B11011101},  //Starts the 2nd Row of Panels
                     {B10100010,B00010101,B11110000,B10000001,B10001101,B00010101},
                     {B10001000,B11000000,B00010111,B11011100,B00100001,B01110101},
                     {B11111111,B11111111,B11010010,B00000011,B11110111,B01000101},
                     {B10000010,B00000011,B11011000,B11111001,B10000100,B01010001},
                     {B10111011,B10111000,B00011011,B10111111,B10111101,B01010111},
                     {B10100000,B00101011,B11110000,B10100001,B10000001,B00010001},
                     {B10111111,B11101111,B11110111,B10111101,B11111111,B11111111},
                     
                     {B10111111,B11101111,B11110111,B10111101,B11111111,B11111111},  //Starts the 3rd Row of Panels
                     {B10001100,B01000001,B10010000,B10101101,B10000000,B00000001},
                     {B11100101,B01011101,B10111110,B10100001,B11101110,B00100011},
                     {B11000101,B01010111,B10100010,B00101000,B00101000,B00100011},
                     {B11010101,B00000100,B00001000,B11101011,B11101110,B11101111},
                     {B10010101,B01010101,B11101111,B10001001,B10001000,B10101011},
                     {B10100101,B01010001,B10000000,B00101011,B10001110,B11101111},
                     {B11111111,B11111111,B11111111,B11111111,B11111111,B11111111}};

enum direction{UP,DOWN,RIGHT,LEFT,NONE};
void setup() {
   Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }

   display.clearDisplay();      //Clears Initial Display Then draws the Initial Maze Panel and Initial Game Figure Location
   drawMaze(printOriginY,printOriginX,0,0);
   drawPlayer();
   display.display();
   delay(500);
 
}

void loop() {
  direction inputDirection = getDirection();
  
  if(inputDirection != NONE){  //Upward Movement if Joystick is facing upwards!
    movePlayer(inputDirection);
    display.display();
    delay(150*SPEED_FACTOR);
    if(isOffScreen()){
      scrollScreen();
      }     
    }

  if(gameIsFinished()){   //Checks to see if Game Piece has arrived at final Location
    endGame();
  }
}

direction getDirection(){
  int xInput = analogRead(ANALOGXPIN);
  int yInput = analogRead(ANALOGYPIN);

  if((yInput>500)&&(yInput<520)&&(xInput>1000))
    return UP;
  if((yInput>500)&&(yInput<520)&&(xInput<10))
    return DOWN;
  if((yInput>1000)&&(xInput<520)&&(xInput>500))
    return RIGHT;
  if((yInput<10)&&(xInput<520)&&(xInput>500))
    return LEFT;
  return NONE;
}

void drawMaze(int y,int x,int originx, int originy){  //Function takes Current X and Y matrix values and origin from which to print their grids
for(int mazeRow = 0;mazeRow<8;mazeRow++){
  for(int mazeColumn = 0;mazeColumn<16;mazeColumn++){
    if(((maze[mazeRow+y][(x+mazeColumn/8)])&(1<<(7-mazeColumn%8)))){    //Uses bit masking to read the individual bit values in the matrix and test for 1s or 0s
      display.fillRect(originx+mazeColumn*BLOCK_SIZE,originy+mazeRow*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE,WHITE);
      display.fillRect(originx+(mazeColumn*BLOCK_SIZE)+BLOCK_SIZE/4,originy+(mazeRow*BLOCK_SIZE)+BLOCK_SIZE/4,BLOCK_SIZE/2,BLOCK_SIZE/2,BLACK);
    }    
  }
 }
 if((y==endYPanel)&&(x==endXPanel)){          //If on the Final Panel, Prints the Flag in Final Position
  drawEnd(originx,originy);
 }
  return;
}

void clearPlayer(){                  //Clears an 8x8 square starting from Top Left Corner Provided to Function
  display.fillRect(playerX*BLOCK_SIZE,playerY*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE,BLACK);
  display.display();
  return;
}

void drawPlayer(){                   //Function to Draw game Piece Given Coordinates on Current Screen if viewed as an 8x16 grid
  for(int m=0;m<8;m++){
        for(int n=0;n<8;n++){
          if((player[m])&(1<<7-n)){
          display.drawPixel((playerX*BLOCK_SIZE)+n,(playerY*BLOCK_SIZE)+m,WHITE);
          }
        }
  }
  return;
}

void movePlayer(direction playerDirection){
  if(directionIsClear(playerDirection)){
    clearPlayer();
    if(playerDirection==UP)
      playerY -= 1;
    if(playerDirection==DOWN)
      playerY += 1;
    if(playerDirection==RIGHT)
      playerX += 1;
    if(playerDirection==LEFT)
      playerX -= 1;
    drawPlayer();
  }
  return;
}

bool directionIsClear(direction playerDirection){
  if((playerDirection==UP) && ((maze[(playerY-1+printOriginY)][(printOriginX+playerX/8)])&(1<<(7-playerX%8))))
    return false;
  else if((playerDirection==DOWN) && ((maze[(playerY+1+printOriginY)][(printOriginX+playerX/8)])&(1<<(7-playerX%8))))
    return false;
  else if((playerDirection==RIGHT) && ((maze[(playerY+printOriginY)][(printOriginX+(playerX+1)/8)])&(1<<(7-(playerX+1)%8))))
    return false;
  else if((playerDirection==LEFT) && ((maze[(playerY+printOriginY)][(printOriginX+(playerX-1)/8)])&(1<<(7-(playerX-1)%8))))
    return false;
  else
    return true;
}

bool isOffScreen(){
  if((playerY==-1)||(playerY==8)||(playerX==16)||(playerX==-1))
    return true;
  else
    return false;
}

void scrollScreen(){
  int8_t scrollDirection = 0;
  if((playerX==16)||(playerX==-1)){
    if(playerX==16)
      scrollDirection = -1;
    else if(playerX == -1)
      scrollDirection = 1;
    for(int m=1;m<=16;m++){
      display.clearDisplay();
      drawMaze(printOriginY,printOriginX,m*BLOCK_SIZE*scrollDirection,0);
      playerX = playerX + scrollDirection;
      drawPlayer();
      drawMaze(printOriginY,printOriginX+(-2*scrollDirection),(-128*scrollDirection)+(m*BLOCK_SIZE*scrollDirection),0);
      display.display();
      delay(100*SPEED_FACTOR);
      }
      printOriginX=printOriginX+-2*scrollDirection;
      return;
  }
  else{
    if(playerY==8)
      scrollDirection = -1;
    else if(playerY == -1)
      scrollDirection = 1;
    for(int m=1;m<=8;m++){
        display.clearDisplay();
        drawMaze(printOriginY+-8*scrollDirection,printOriginX,0,(-64*scrollDirection)+m*BLOCK_SIZE*scrollDirection);
        playerY = playerY + scrollDirection;
        drawPlayer();
        drawMaze(printOriginY,printOriginX,0,m*BLOCK_SIZE*scrollDirection);
        display.display();
        delay(100*SPEED_FACTOR);
        }
      printOriginY=printOriginY+-8*scrollDirection;
  }
}

void drawEnd(int originx,int originy){
  for(int m=0;m<8;m++){
        for(int n=0;n<8;n++){
          if(end[m]&(1<<7-n)){                //Draws Flag From Matrix Peviously Defining 1s and 0s
            display.drawPixel((endX*BLOCK_SIZE)+n+originx,(endY*BLOCK_SIZE)+m+originy,WHITE);          
          }
        }
  }
}

bool gameIsFinished(){
  return ((printOriginX==endXPanel)&&(printOriginY==endYPanel)&&(playerY==6)&&(playerX==3));
}

void endGame(){
  unsigned long t;
  t= millis();
  t= t/1000;
  char buf[8];
  ltoa(t,buf,10);
  display.clearDisplay();                             //Prints Congratulations Message and User Score/Time in Seconds
  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println(F("CONGRATS"));
  display.setCursor(12,18);
  display.println(F("YOU WIN!"));
  display.setCursor(8,36);
  display.println(F("TIME:"));
  display.setCursor(67,36);
  display.println(buf);
  display.display();    
  while(1){};
}
