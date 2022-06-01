#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

#define L1 2
#define L2 3
#define Buzz 4
#define BTNT 5
#define BTNB 6

#if defined(ARDUINO_FEATHER_ESP32) // Feather Huzzah32
  #define TFT_CS         14
  #define TFT_RST        15
  #define TFT_DC         32

#elif defined(ESP8266)
  #define TFT_CS         4
  #define TFT_RST        16                                            
  #define TFT_DC         5

#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif

// For 1.44" and 1.8" TFT with ST7735 use:
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

float p = 3.1415926;
char board[24];
char board_list[24][24];

void setup(void) {

  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(Buzz, OUTPUT);
  pinMode(BTNT, INPUT);
  pinMode(BTNB, INPUT);
    
  Serial.begin(9600);
  
  tft.initR(INITR_144GREENTAB);
  memset(board, 0, 24 * sizeof(char));
  
  printBoard(board);

  ///////////////////////////////////////////////////////////////////

  for (int i = 0; i < 4; ++i) {
        // # FOR PLAYER 1, STAGE 1
        digitalWrite(L1, HIGH);
        digitalWrite(L2, LOW);
        bool finished1 = false;

        while (!finished1){
            Serial.println("\n PLAYER 1: Place a piece '1': ");
            int pos1 = readPosition();
            
            if(pos1 == 99){
                exit(0);
            }

            if (board[pos1] == 0){
                board[pos1] = 1;
                if (isMill(pos1, board)){
                    bool itemPlaced = false;

                    while (!itemPlaced){
                      Serial.println("\nA Mill is formed.\nRemove a 2 piece: ");
                      
                        int pos2 = readPosition();
                        if ( (board[pos2] == 2 && !isMill(pos2, board)) || ((isMill(pos2, board) && numOfPieces(board, 1) == 3))){
                            board[pos2] = 0;
                            itemPlaced = true;
                        } else{
                            Serial.println("Invalid Position! Try again!");
                        }

                    }
                }

                finished1 = true;
            }

            else{
                Serial.println("There is already a piece in position!");
                Serial.println(pos1);
            }
        }

        printBoard(board);
        ////////////////////////////////////////////////////////////////
        // # FOR PLAYER 2, STAGE 1

        digitalWrite(L2, HIGH);
        digitalWrite(L1, LOW);

        bool finished2 = false;

        while (!finished2){

            Serial.println("\n PLAYER 2: Place a piece '2': ");

            int pos1 = readPosition();
            
            if(pos1 == 99){
                exit(0);
            }
            
            if (board[pos1] == 0){
                board[pos1] = 2;

                if (isMill(pos1, board)){
                    bool itemPlaced = false;

                    while (!itemPlaced){
                       Serial.println("\nA Mill is formed.\nRemove a 1 piece: ");
                       
                        int pos2 = readPosition();
                        if ((board[pos2] == 1 && !isMill(pos2, board)) || (isMill(pos2, board) && numOfPieces(board, 2) == 3)){
                            board[pos2] = 0;
                            itemPlaced = true;
                        } else{
                            Serial.println("Invalid Position! Try again!");
                        }
                    }
                 }
              finished2 = true;
              } else{
                Serial.println("There is already a piece in position!");
                Serial.println(pos1);
            }
        }

        printBoard(board);
  }

  ///////////////////////////////////////////////////////////////////

}

void loop() {



  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// # PLAYER 1 STAGE 2 MOVE
  
    printBoard(board);
    bool userMoved = false;
    bool only3;
    int pos1;
    while (!userMoved) {
        bool movable = false;
        digitalWrite(L1, HIGH);
        digitalWrite(L2, LOW);

        if (numOfPieces(board, 1) == 3) {
            only3 = true;
        } else {
            only3 = false;
        }

        while (!movable) {
          Serial.println("PLAYER 1: Which '1' piece will you move?: ");
          pos1 = readPosition();

          while (board[pos1] != 1) {
            Serial.println("Invalid. Try again.");
            Serial.println("PLAYER 1: Which '1' piece will you move?: ");
           
            pos1 = readPosition();
          }
          if (only3) {
            movable = true;
            Serial.println("Stage 3 for Player 1. Allowed to Fly");
            break;
          }

          char* possibleMoves = adjacentLocations(pos1);

          for (int adjpos = 0; adjpos < 4; ++adjpos) {
            if (possibleMoves[adjpos] != -1 && board[possibleMoves[adjpos]] == 0) {
              movable = true;
              break;
            }
          }
          
          if (movable == false) {
            Serial.println("No empty adjacent pieces!");
          }
        }

        bool userPlaced = false;

        while (!userPlaced){
        Serial.println("'1' New Position is : ");
  
        int newpos1 = readPosition();

        if(findElem(newpos1, adjacentLocations(pos1)) == true || only3) {
          if (board[newpos1] == 0) {
            board[pos1] = 0;
            board[newpos1] = 1;

            if (isMill(newpos1, board)) {

              bool userRemoved = false;
              while (!userRemoved){
                printBoard(board);

                Serial.println("Mill Formed. Remove a '2' piece: ");
                    int removepos1 = readPosition();

                    if ( (board[removepos1] == 2 && !isMill(removepos1, board)) || (isMill(removepos1, board) && numOfPieces(board, 1) == 3)) {
                      board[removepos1] = 0;
                      userRemoved = true;
                    }else{
                        Serial.println("Invalid Position");
                    }
                }
             }

              userPlaced = true;
              userMoved = true;
             } else {
                   Serial.println("Invalid Position");
              }
          } else{
                Serial.println("Only adjacent locations in Stage 2. Try again.");
          }
      }
    }

//    possibleMoves_stage2or3(board, 1, board_list);

    if(numOfPieces(board, 2) < 3){
      printBoard(board);
      tone(Buzz,150);
      delay(500);
      tone(Buzz, 300);
      delay(1000);
      noTone(Buzz);
      exit(0);
    } else {
        printBoard(board);
    }

/*  if(findPos(board_list) == 0) {
        Serial.println("-----------\n");
        Serial.println("    TIE    \n");
        Serial.println("-----------\n");
        Serial.println("Exit");
        exit(0);
    } 
*/

  //////////////////////////////////////////////////////

   // # PLAYER 2 STAGE 2 MOVE

    userMoved = false;
    while (!userMoved){
        bool movable = false;
        digitalWrite(L1, LOW);
        digitalWrite(L2, HIGH);

        if (numOfPieces(board, 2) == 3){
            only3 = true;
        } else {
            only3 = false;
        }

        while (!movable){

          Serial.println("\nPLAYER 2: Which '2' piece will you move?: ");

            pos1 = readPosition();

            while (board[pos1] != 2){
                Serial.println("Invalid. Try again.\n");
                Serial.println("\nPLAYER 2: Which '2' piece will you move?: ");
                pos1 = readPosition();
            }

            if (only3){
                movable = true;
                Serial.println("Stage 3 for Player 2. Allowed to Fly\n");
                break;
            }

            char* possibleMoves = adjacentLocations(pos1);

            for (int adjpos = 0; adjpos < 4; ++adjpos) {
              if (possibleMoves[adjpos] != -1 && board[possibleMoves[adjpos]] == 0) {
                movable = true;
                break;
              }
            }

            if(!movable)
                Serial.println("No empty adjacent pieces!\n");
        }

        bool userPlaced = false;

        while (!userPlaced){

          Serial.println("'2' New Position is : ");
          int newpos1 = readPosition();

          if(findElem(newpos1, adjacentLocations(pos1)) == true || only3){

                if (board[newpos1] == 0) {
                    board[pos1] = 0;
                    board[newpos1] = 2;

                    if (isMill(newpos1, board)) {
                        bool userRemoved = false;
                        while (!userRemoved){
                            printBoard(board);

                            Serial.println("Mill Formed. Remove a '1' piece: ");

                            int removepos1 = readPosition();

                            if ( (board[removepos1] == 1 && !isMill(removepos1, board)) || (isMill(removepos1, board) && numOfPieces(board, 2) == 3)){
                                board[removepos1] = 0;
                                userRemoved = true;
                            } else{
                                Serial.println("Invalid Position\n");
                            }
                        }
                    }

                    userPlaced = true;
                    userMoved = true;
                } else{
                    Serial.println("Invalid Position\n");
                }
            } else {
                Serial.println("Only adjacent locations in Stage 2. Try again.\n");
            }
        }
    }

    if(numOfPieces(board, 1) < 3){
      printBoard(board);
      tone(Buzz,150,300);
      delay(500);
      tone(Buzz, 300,900);
      exit(0);
    } else {
        printBoard(board);
    }

/*    possibleMoves_stage2or3(board, 2, board_list);

  if(findPos(board_list) == 0) {
        Serial.println("-----------\n");
        Serial.println("    TIE    \n");
        Serial.println("-----------\n");
        Serial.println("Exit");
        exit(0);
    } 
*/
}

int readPosition(){
  char c0 = 55;

  String inString = "";
  unsigned stringToInt;

  while(c0 == 55) {
    int btnText = digitalRead(BTNT);
    int btnBoard = digitalRead(BTNB);
    
    if(Serial.available() > 0){
      int inChar = Serial.read();
      if(isDigit(inChar))
          inString +=(char)inChar;
      if(inChar == '\n'){
        stringToInt = inString.toInt();
        return stringToInt;   
      }
    } else if (btnText == HIGH) {
          tftPrintText();
      
      } else if (btnBoard == HIGH) {
          printBoard(board);
      
      }
  }
}

void printBoard(char board1[]){
  tft.fillScreen(0xFEAD);
  testdrawrects(ST77XX_BLACK);

  //Up line
  for(int16_t i = 4; i <= 44; i++){
    tft.drawPixel(tft.width()/2, i, ST77XX_BLACK);
    delay(0);
  }

  //Down line
  for(int8_t i = tft.height() - 45; i <= tft.height()- 5; i++){
    tft.drawPixel(tft.width()/2, i, ST77XX_BLACK);
    delay(0);
  }

  //Left line
  for(int8_t i = 4; i <= 44; i++){
    tft.drawPixel(i, tft.height()/2, ST77XX_BLACK);
    delay(0);
  }

   //Right line
  for(int8_t i = tft.width() - 45; i <= tft.width() - 5; i++){
    tft.drawPixel(i, tft.height()/2, ST77XX_BLACK);
    delay(0);
  }

  for(int8_t i = 0; i < 24; i++){
    drawPiecePos(i, board1[i]);
    delay(0);
  }
}

void drawPiece(uint16_t x, uint16_t y, uint16_t color) {
    tft.fillCircle(x, y, 5, color);
}

void testdrawrects(uint16_t color) {
  for (int16_t x=0; x < tft.width(); x+=40) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
  }
}

void drawPiecePos(uint16_t pos, uint16_t player){
  uint16_t color;
  if(player != 0) {
    if(player == 1)
      color = ST77XX_GREEN;
    else
      color = ST77XX_RED;
    if(pos == 0){
      drawPiece(5, 5, color);
    } else if(pos == 1) {
      drawPiece(tft.width()/2, 5, color);
    } else if(pos == 2) {
      drawPiece(tft.width() - 5, 5, color);
    } else if(pos == 3) {
      drawPiece(5, tft.height()/2, color);
    } else if(pos == 4) {
      drawPiece(tft.width() - 5, tft.height()/2, color);
    } else if(pos == 5) {
      drawPiece(5, tft.height() - 5, color);
    } else if(pos == 6) {
      drawPiece(tft.width() / 2, tft.height() - 5, color);
    } else if(pos == 7) {
      drawPiece(tft.width() - 5, tft.height() - 5, color);
    } else if(pos == 8) {
      drawPiece(25, 25, color);
    } else if(pos == 9) {
      drawPiece(tft.width()/2, 25, color);
    } else if(pos == 10) {
      drawPiece(tft.width() - 25, 25, color);
    } else if(pos == 11) {
      drawPiece(25, tft.height()/2, color);
    } else if(pos == 12) {
      drawPiece(tft.width() - 25, tft.height()/2, color);
    } else if(pos == 13) {
      drawPiece(25, tft.height() - 25, color);
    } else if(pos == 14) {
      drawPiece(tft.width()/2, tft.height() - 25, color);
    } else if(pos == 15) {
      drawPiece(tft.width() - 25, tft.height() - 25, color);
    } else if(pos == 16) {
      drawPiece(45, 45, color);
    } else if(pos == 17) {
      drawPiece(tft.width()/2, 45, color);
    } else if(pos == 18) {
      drawPiece(tft.width() - 45, 45, color);
    } else if(pos == 19) {
      drawPiece(45, tft.height()/2, color);
    } else if(pos == 20) {
      drawPiece(tft.width() - 45, tft.height()/2, color);
    } else if(pos == 21) {
      drawPiece(45, tft.height() - 45, color);
    } else if(pos == 22) {
      drawPiece(tft.width()/2, tft.height() - 45, color);
    } else if(pos == 23) {
      drawPiece(tft.width() - 45, tft.height() - 45, color);
    }
  }
}

// All neighbors for the given position
char* adjacentLocations(int pos){
  if(pos == 0){
    static char res[] = {1,3,-1,-1};
    return res;
  } else if(pos == 1){
    static char res[] = {0, 2, 9,-1};
    return res;
  } else if(pos == 2){
    static char res[] = {1, 4,-1,-1};
    return res;
  } else if(pos == 3){
    static char res[] = {0, 5, 11,-1};
    return res;
  } else if(pos == 4){
    static char res[] = {2, 7, 12,-1};
    return res;
  } else if(pos == 5){
    static char res[] = {3, 6,-1,-1};
    return res;
  } else if(pos == 6){
    static char res[] = {5, 7, 14,-1};
    return res;
  } else if(pos == 7){
    static char res[] = {4, 6,-1,-1};
    return res;
  } else if(pos == 8){
    static char res[] = {9, 11,-1,-1};
    return res;
  } else if(pos == 9){
    static char res[] = {1, 8, 10, 17};
    return res;
  } else if(pos == 10){
    static char res[] = {9, 12,-1,-1};
    return res;
  } else if(pos == 11){
    static char res[] = {3, 8, 13, 19};
    return res;
  } else if(pos == 12){
    static char res[] = {4, 10, 15, 20};
    return res;
  } else if(pos == 13){
    static char res[] = {11, 14,-1,-1};
    return res;
  } else if(pos == 14){
    static char res[] = {6, 13, 15, 22};
    return res;
  } else if(pos == 15){
    static char res[] = {12, 14,-1,-1};
    return res;
  } else if(pos == 16){
    static char res[] = {17, 19,-1,-1};
    return res;
  } else if(pos == 17){
    static char res[] = {9, 16, 18,-1};
    return res;
  } else if(pos == 18){
    static char res[] = {17, 20,-1,-1};
    return res;
  } else if(pos == 19){
    static char res[] = {11, 16, 21,-1};
    return res;
  } else if(pos == 20){
    static char res[] = {12, 18, 23,-1};
    return res;
  } else if(pos == 21){
    static char res[] = {19, 22,-1,-1};
    return res;
  } else if(pos == 22){
    static char res[] = {21, 23, 14,-1};
    return res;
  } else if(pos == 23){
    static char res[] = {20, 22,-1,-1};
    return res;
  }
}

// # Function to check if 2 positions have the player on them
bool isPlayer(int player, char board1[], int p1, int p2){
  return (board1[p1] == player && board1[p2] == player);
}

// # Function to check if a player can make a mill in the next move.
bool checkNextMill(int position, char board1[], int player){
    bool mill[] = {
        (isPlayer(player, board1, 1, 2)   || isPlayer(player, board1, 3, 5)),
        (isPlayer(player, board1, 0, 2)   || isPlayer(player, board1, 9, 17)),
        (isPlayer(player, board1, 0, 1)   || isPlayer(player, board1, 4, 7)),
        (isPlayer(player, board1, 0, 5)   || isPlayer(player, board1, 11, 19)),
        (isPlayer(player, board1, 2, 7)   || isPlayer(player, board1, 12, 20)),
        (isPlayer(player, board1, 0, 3)   || isPlayer(player, board1, 6, 7)),
        (isPlayer(player, board1, 5, 7)   || isPlayer(player, board1, 14, 22)),
        (isPlayer(player, board1, 2, 4)   || isPlayer(player, board1, 5, 6)),
        (isPlayer(player, board1, 9, 10)  || isPlayer(player, board1, 11, 13)),
        (isPlayer(player, board1, 8, 10)  || isPlayer(player, board1, 1, 17)),
        (isPlayer(player, board1, 8, 9)   || isPlayer(player, board1, 12, 15)),
        (isPlayer(player, board1, 3, 19)  || isPlayer(player, board1, 8, 13)),
        (isPlayer(player, board1, 20, 4)  || isPlayer(player, board1, 10, 15)),
        (isPlayer(player, board1, 8, 11)  || isPlayer(player, board1, 14, 15)),
        (isPlayer(player, board1, 13, 15) || isPlayer(player, board1, 6, 22)),
        (isPlayer(player, board1, 13, 14) || isPlayer(player, board1, 10, 12)),
        (isPlayer(player, board1, 17, 18) || isPlayer(player, board1, 19, 21)),
        (isPlayer(player, board1, 1, 9)   || isPlayer(player, board1, 16, 18)),
        (isPlayer(player, board1, 16, 17) || isPlayer(player, board1, 20, 23)),
        (isPlayer(player, board1, 16, 21) || isPlayer(player, board1, 3, 11)),
        (isPlayer(player, board1, 12, 4)  || isPlayer(player, board1, 18, 23)),
        (isPlayer(player, board1, 16, 19) || isPlayer(player, board1, 22, 23)),
        (isPlayer(player, board1, 6, 14)  || isPlayer(player, board1, 21, 23)),
        (isPlayer(player, board1, 18, 20) || isPlayer(player, board1, 21, 22))
    };

    return mill[position];
}

// # Return True if a player has a mill on the given position
bool isMill(int position, char board1[]){
    char p = board1[position];

    if(p != 0){
        // # If there is some player on that position
        return checkNextMill(position, board1, p);
    }
    else
        return false;
}

// # Function to return number of pieces owned by a player on the board
int numOfPieces(char board1[], char value) {
  int i, count = 0;
  for (i = 0; i < 24; i++) {
    if (board1[i] == value) {
        count++;
    }
  }

  return count;
}

char findPos(char board_list[24][24]) {
  int i = 0;
  for (i = 0; i < 24; ++i) {
    if(board_list[i][0] == -1)
      return i;
  }

  return i;
}

void changeBoard(char board1[24], char board_list[24][24], int position) {
  memcpy(board_list[position], board1, 24 * sizeof(char));
}

void removePiece(char board_copy[24], char board_list[24][24], int player){
  int op;

  for (int i = 0; i < 24; ++i) {
    if(player == 1)
      op = 2;
    if(player == 2)
      op = 1;

    if(board_copy[i] == op) {
      if(!isMill(i, board_copy)) {
        char new_board[24];
        memcpy(new_board, board_copy, 24 * sizeof(char));
        new_board[i] = 0;

        char pos = findPos(board_list);
        changeBoard(new_board, board_list, pos);
      }
    }
  }
}

// # Generating all possible moves for stage 2 of the game
void possibleMoves_stage2(char board1[24], int player, char board_list[24][24]){

  for (int i = 0; i < 24; ++i) {
        if (board1[i] == player){
            char* adjacent_list = adjacentLocations(i);

            for (int pos = 0; pos < 4; ++pos) {
                if (board1[adjacent_list[pos]] == 0){
                    // # If the location is empty, then the piece can move there
                    char board_copy[24];
                    memcpy(board_copy, board1, 24 * sizeof(char));
                    board_copy[pos] = player;

                    if (isMill(pos, board_copy)){
                        // # in case of mill, remove Piece
                        removePiece(board_copy, board_list, player);
                    } else{
                      char pos = findPos(board_list);
                      changeBoard(board_copy, board_list, pos);
                    }
                }
            }
        }
    }
}

// # Generating all possible moves for stage 3 of the game
void possibleMoves_stage3(char board1[24], int player, char board_list[24][24]){
  for (int i = 0; i < 24; ++i) {
        if (board1[i] == player) {
          for (int j = 0; j < 24; ++j){
                if (board1[j] == 0){
                    char board_copy[24];
                    memcpy(board_copy, board1, 24 * sizeof(char));
                    // # The piece can fly to any empty position, not only adjacent ones
                    board_copy[i] = 0;
                    board_copy[j] = player;

                    if (isMill(j, board_copy)){
                        removePiece(board_copy, board_list, player);
                    } else{
                        char pos = findPos(board_list);
                        changeBoard(board_copy, board_list, pos);
                    }
                }
            }
        }
    }
}

// # Checks if game is in stage 2 or 3
void possibleMoves_stage2or3(char board1[24], int player, char board_list[24][24]){
  memset(board_list, -1, 576 * sizeof(char));
    if (numOfPieces(board1, player) == 3){
        possibleMoves_stage3(board1, player, board_list);
    }
    else{
        possibleMoves_stage2(board1, player, board_list);
    }
}

bool findElem(int pos1, char neighbors[]){
  for (int i = 0; i < 4; ++i)
  {
    if(neighbors[i] == pos1)
      return true;
  }

  return false;
}


void tftPrintText() {
  tft.setTextWrap(true);
  tft.fillScreen(0xFEAD);
  tft.setCursor(5, 5);
  tft.setTextColor(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.println("00-------01-------02");
  tft.println(" |        |        |");
  tft.println(" |  08----09----10 |");
  tft.println(" |  |     |     |  |");
  tft.println(" |  |  16-17-18 |  |");
  tft.println(" |  |  |     |  |  |");
  tft.println(" |  |  |     |  |  |");
  tft.println("03--11-19   20-12--04");
  tft.println(" |  |  |     |  |  |");
  tft.println(" |  |  |     |  |  |");
  tft.println(" |  |  21-22-23 |  |");
  tft.println(" |  |     |     |  |");
  tft.println(" |  13----14----15 |");
  tft.println(" |        |        |");
  tft.println("05-------06-------07");
  
  
}
