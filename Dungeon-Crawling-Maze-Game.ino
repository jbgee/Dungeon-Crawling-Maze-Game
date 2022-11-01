// Dungeon Crawling Maze Game 
// Written By Jackson B. Gee
// include drawPlayer in drawMaze to reduce total function calls.
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define BLOCK_SIZE 8 //Number of pixels in maze block. Unit size on screen
#define ANALOGXPIN 0 //analog pin connected to x output 
#define ANALOGYPIN 1 //analog pin connected to y output
#define INTBITS 16
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int8_t SPEED_FACTOR = 1;

int8_t playerX = 11;       //Players position on screen. Initialized as starting Position
int8_t playerY = 6;

uint8_t endX = 3;              //Ending Position on screen
uint8_t endY = 6; 

uint8_t endXPanel = 0;         //Top left corner position when ending exists
uint8_t endYPanel = 16;

uint8_t printOriginX = 1;      //Must also declare the Top Left Corner of maze matrix when game starts
uint8_t printOriginY = 16;

uint8_t player[8]={B00000000, //Declares the Matrix used to draw the player figure
                   B00011000,
                   B00011000,
                   B01111110,
                   B00011000,
                   B00100100,
                   B01100110,
                   B00000000};
                        
uint8_t end[8] = {B00000000, //Defines the Matrix used to draw the game end flag
                  B00011110,
                  B00011110,
                  B00011110,
                  B00010000,
                  B00010000,
                  B01111110,
                  B00000000};
    

                               
int maze[][3]={{0b1111111111111111,0b1111111111111111,0b1111111111111111},  //Declare the matrix as ints in binary form so that each value is stored in a single bit instead of 16
               {0b1000110000110101,0b1010001000000001,0b1101010000010101},  // Threfore instead of using 24x48 matrix of ints each using 16 bits each the new matrix
               {0b1010001110000001,0b1000100010111101,0b1000001010000101},  // uses 24x3 matrix of ints saving lots of Dynamic Memory (1/16th)
               {0b1010100010111101,0b1011110110100001,0b1011100010111101},
               {0b1010111000000101,0b1010010010101011,0b1010111010000001},
               {0b1011101101110101,0b1010111110111000,0b0010101111110111},
               {0b1000100000010100,0b0010000010101101,0b1100000011010001},
               {0b1110111111111111,0b1111111011101111,0b1111110111011101},
                 
               {0b1110111111111111,0b1111111011101111,0b1111110111011101},  //2nd Row of Panels
               {0b1010001000010101,0b1111000010000001,0b1000110100010101},
               {0b1000100011000000,0b0001011111011100,0b0010000101110101},
               {0b1111111111111111,0b1101001000000011,0b1111011101000101},
               {0b1000001000000011,0b1101100011111001,0b1000010001010001},
               {0b1011101110111000,0b0001101110111111,0b1011110101010111},
               {0b1010000000101011,0b1111000010100001,0b1000000100010001},
               {0b1011111111101111,0b1111011110111101,0b1111111111111111},
                 
               {0b1011111111101111,0b1111011110111101,0b1111111111111111},  //3rd Row of Panels
               {0b1000110001000001,0b1001000010101101,0b1000000000000001},
               {0b1110010101011101,0b1011111010100001,0b1110111000100011},
               {0b1100010101010111,0b1010001000101000,0b0010100000100011},
               {0b1101010100000100,0b0000100011101011,0b1110111011101111},
               {0b1001010101010101,0b1110111110001001,0b1000100010101011},
               {0b1010010101010001,0b1000000000101011,0b1000111011101111},
               {0b1111111111111111,0b1111111111111111,0b1111111111111111}};

enum direction{UP,DOWN,RIGHT,LEFT,NONE}; //Enumerate possible movement directions

void setup() {
   Serial.begin(9600);
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64. Initializes Screen
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }

   display.clearDisplay();      //Clears Initial Display Then draws the Initial Maze Panel and Game Figure Location
   drawMaze(printOriginY,printOriginX,0,0);
   drawPlayer();
   display.display();
   delay(500);
 
}

void loop() {
  direction inputDirection = getDirection();
  
  if(inputDirection != NONE){  
    movePlayer(inputDirection);
    if(isOffScreen()){
      scrollScreen();
      }     
    }

  if(gameIsFinished()){ 
    endGame();
  }
}

direction getDirection(){ //
  int xInput = analogRead(ANALOGXPIN);
  int yInput = analogRead(ANALOGYPIN);
  if((yInput>500)&&(yInput<524)&&(xInput>1000))
    return UP;
  if((yInput>500)&&(yInput<524)&&(xInput<24))
    return DOWN;
  if((yInput>1000)&&(xInput<524)&&(xInput>500))
    return RIGHT;
  if((yInput<24)&&(xInput<524)&&(xInput>500))
    return LEFT;
  return NONE;
}

void drawMaze(int y,int x,int originx, int originy){  //Function takes Current X and Y matrix values and origin from which to print their grids
for(int mazeRow = 0;mazeRow<8;mazeRow++){
  for(int mazeColumn = 0;mazeColumn<16;mazeColumn++){
    if(((maze[mazeRow+y][x])&(1<<((INTBITS-1)-mazeColumn)))){    //Uses bit masking to read the individual bit values in the matrix and test for 1s or 0s
      display.fillRect(originx+mazeColumn*BLOCK_SIZE,originy+mazeRow*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE,WHITE);
      display.fillRect(originx+(mazeColumn*BLOCK_SIZE)+BLOCK_SIZE/4,originy+(mazeRow*BLOCK_SIZE)+BLOCK_SIZE/4,BLOCK_SIZE/2,BLOCK_SIZE/2,BLACK);
    }    
  }
 }
 if((y==endYPanel)&&(x==endXPanel)){          //If printing Panel with ending, Prints the end in Final Position
  drawEnd(originx,originy);
 }
  return;
}

void clearPlayer(){                  //Clears an 8x8 square at player position
  display.fillRect(playerX*BLOCK_SIZE,playerY*BLOCK_SIZE,BLOCK_SIZE,BLOCK_SIZE,BLACK);
  display.display();
  return;
}

void drawPlayer(){                   //Function to Draw game player at players x,y coordinates on screen
  for(int m=0;m<8;m++){
        for(int n=0;n<8;n++){
          if((player[m])&(1<<7-n)){
          display.drawPixel((playerX*BLOCK_SIZE)+n,(playerY*BLOCK_SIZE)+m,WHITE);
          }
        }
  }
  return;
}

void movePlayer(direction playerDirection){ //takes intended movement, if that direction is clear, clears game player and redraws in new location
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
  display.display();
  delay(150/SPEED_FACTOR);
  return;
}

bool directionIsClear(direction playerDirection){ //takes intended direction and return true if that direction is clear, false if there is a block in the way
  if((playerDirection==UP) && ((maze[(playerY-1+printOriginY)][(printOriginX+playerX/INTBITS)])&(1<<((INTBITS-1)-playerX%INTBITS))))
    return false;
  else if((playerDirection==DOWN) && ((maze[(playerY+1+printOriginY)][(printOriginX+playerX/INTBITS)])&(1<<((INTBITS-1)-playerX%INTBITS))))
    return false;
  else if((playerDirection==RIGHT) && ((maze[(playerY+printOriginY)][(printOriginX+(playerX+1)/INTBITS)])&(1<<((INTBITS-1)-(playerX+1)%INTBITS))))
    return false;
  else if((playerDirection==LEFT) && ((maze[(playerY+printOriginY)][(printOriginX+(playerX-1)/INTBITS)])&(1<<((INTBITS-1)-(playerX-1)%INTBITS))))
    return false;
  else
    return true;
}

bool isOffScreen(){ //checks if the player has moved outside the bounds of the screen
  if((playerY==-1)||(playerY==8)||(playerX==16)||(playerX==-1))
    return true;
  else
    return false;
}

void scrollScreen(){  //scrolls the screen depending on where the player has moved
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
      drawMaze(printOriginY,printOriginX+(-1*scrollDirection),(-128*scrollDirection)+(m*BLOCK_SIZE*scrollDirection),0);
      display.display();
      delay(100/SPEED_FACTOR);
      }
      printOriginX=printOriginX+-1*scrollDirection;
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
        delay(100/SPEED_FACTOR);
        }
      printOriginY=printOriginY+-8*scrollDirection;
  }
}

void drawEnd(int originx,int originy){ //draws ending flag
  for(int m=0;m<8;m++){
        for(int n=0;n<8;n++){
          if(end[m]&(1<<7-n)){                //Draws Flag From Matrix Peviously Defining 1s and 0s
            display.drawPixel((endX*BLOCK_SIZE)+n+originx,(endY*BLOCK_SIZE)+m+originy,WHITE);          
          }
        }
  }
}

bool gameIsFinished(){  //checks if player is on final position
  return ((printOriginX==endXPanel)&&(printOriginY==endYPanel)&&(playerY==endY)&&(playerX==endX));
}

void endGame(){ //Prints Congratulations Message and User Score/Time in Seconds and puts arduino in infinite loop
  unsigned long totalTime;
  totalTime= millis();
  totalTime /=1000;
  char buf[8];
  ltoa(totalTime,buf,10);
  display.clearDisplay();                            
  display.setTextSize(2); 
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
