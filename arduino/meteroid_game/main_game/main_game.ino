#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <limits.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET -1
#define WHITE SSD1306_WHITE
#define BLACK SSD1306_BLACK
#define INVERSE SSD1306_INVERSE
#define X_PIN 36
#define Y_PIN 39
#define B_PIN 13
#define SCL_PIN 17
#define SDA_PIN 16
#define GAME_WIN_MIN_X 2
#define GAME_WIN_MAX_X 110
#define GAME_WIN_MIN_Y 10
#define GAME_WIN_MAX_Y 60
#define PLAYER_SIZE_X 10
#define PLAYER_SIZE_Y 5
#define MAX_BULLETS 20
#define MAX_METEOR 20
#define SPEED_LIMITATION 500
#define BUTTON_ERROR 2
#define BULLET_SPEED 3
#define MARGIN_ERROR 3
#define APROX_MAX_VOLT 4000
#define MAX_NUM_OF_SCORES 10


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int x_read = analogRead(X_PIN);
int y_read = analogRead(Y_PIN);
int x_move = 0;
int y_move = 0;
int x_pos = (GAME_WIN_MAX_X + GAME_WIN_MIN_X) / 2;
int y_pos = (GAME_WIN_MAX_Y + GAME_WIN_MIN_Y) / 2;

int b_read = digitalRead(B_PIN);
int b_press_cnt = 0;
bool b_press = false;
bool bullet_exist[MAX_BULLETS];
int bullet_posX[MAX_BULLETS];
int bullet_posY[MAX_BULLETS];

bool meteor_spawn = false;
bool meteor_exist[MAX_METEOR];
int meteor_posX[MAX_METEOR];
int meteor_posY[MAX_METEOR];
int meteor_size[MAX_METEOR];
int meteor_speed[MAX_METEOR];
int meteor_max_chance = 100;
bool meteor_hard = false;

int choice = 0;
int chosen = -1;
int cnt = 0;
int y_read_cnt = 0;
int x_read_cnt = 0;
int myDelay = 10;
bool game_start = true;
bool game_end = false;
int score = 0;
int high_score = -1;
int x_cursor = 0;
int games_played = -1;

bool get_shot = false;
int lifes = 3;

char alphabet[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
char score_names[MAX_NUM_OF_SCORES][4] = { { "   " }, { "   " }, { "   " }, { "   " }, { "   " }, { "   " }, { "   " }, { "   " }, { "   " }, { "   " } };
int score_nums[MAX_NUM_OF_SCORES] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int score_order[MAX_NUM_OF_SCORES] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

#define IMG_HEIGHT 16
#define IMG_WIDTH 16
static const unsigned char PROGMEM heart_full[] = {
  0b00000000, 0b00000000,
  0b00011100, 0b00111000,
  0b00111110, 0b01111100,
  0b01100011, 0b11111110,
  0b11001111, 0b11111111,
  0b11011111, 0b11111111,
  0b11011111, 0b11111111,
  0b11111111, 0b11111111,
  0b01111111, 0b11111110,
  0b01111111, 0b11111110,
  0b00111111, 0b11111100,
  0b00011111, 0b11111000,
  0b00001111, 0b11110000,
  0b00000111, 0b11100000,
  0b00000011, 0b11000000,
  0b00000001, 0b10000000
};

static const unsigned char PROGMEM heart_empty[] = {
  0b00000000, 0b00000000,
  0b00011100, 0b00111000,
  0b00100010, 0b01000100,
  0b01000001, 0b10000010,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,
  0b10000000, 0b00000001,
  0b01000000, 0b00000010,
  0b01000000, 0b00000010,
  0b00100000, 0b00000100,
  0b00010000, 0b00001000,
  0b00001000, 0b00010000,
  0b00000100, 0b00100000,
  0b00000010, 0b01000000,
  0b00000001, 0b10000000
};
unsigned char heart_0[IMG_HEIGHT + IMG_WIDTH];
unsigned char heart_1[IMG_HEIGHT + IMG_WIDTH];
unsigned char heart_2[IMG_HEIGHT + IMG_WIDTH];

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);
  pinMode(B_PIN, INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  reset();

  display.clearDisplay();
}



void loop() {
  display.clearDisplay();
  delay(myDelay);

  y_read = analogRead(Y_PIN);
  b_read = digitalRead(B_PIN);

  mainMenuDisplay(choice);

  /*  POHYB PO HLAVNIM MENU
  *     y_read        precteny vstup pinu VRy na joysticku
  *     y_read_cnt    pocet cyklu pinu VRy v minimalnim nebo v maximalnim napeti
  *     choice        aktualne zvolena polozka v hlavnim menu
  */
  if (y_read == 0) {
    y_read_cnt++;
    if (y_read_cnt == MARGIN_ERROR) {
      choice--;
      if (choice < 0) {
        choice = 0;
      }
    }
  } else if (y_read > APROX_MAX_VOLT) {
    y_read_cnt++;
    if (y_read_cnt == MARGIN_ERROR) {
      choice++;
      if (choice > 3) {
        choice = 3;
      }
    }
  } else {
    y_read_cnt = 0;
  }



  checkButtonPress(MARGIN_ERROR);

  /*  ANIMACE PO VYBERU POLOZKY Z HLAVNIHO MENU
  *     myDelay       zpozdeni cyklu; zmena pro lepsi viditelnost animace lidskym okem
  *     cnt           pomocna promenna, pocet probehnutych cyklu animace
  */
  while (b_press == true) {
    display.clearDisplay();
    myDelay = 100;
    delay(myDelay);
    mainMenuDisplay(choice);
    delay(myDelay);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.println(F("PLAY"));
    display.println(F("SCORES"));
    display.println(F("EXIT"));
    display.display();
    cnt++;
    if (cnt > 2) {
      myDelay = 10;
      chosen = choice;
      b_press = false;
      b_press_cnt = 0;
      cnt = 0;
    }
  }

  // PRVNI ZVOLENA MOZNOST Z HLAVNIHO MENU = PLAY
  while (chosen == 0) {

    //    CEKANI NA ZACATEK HRY (stisk tlacitka uzivatelem)
    while (game_start) {
      display.clearDisplay();
      gameMainWindow();
      display.fillRect(GAME_WIN_MIN_X, GAME_WIN_MIN_Y, GAME_WIN_MAX_X - 1, GAME_WIN_MAX_Y, WHITE);
      display.setCursor(28, GAME_WIN_MIN_Y + 8);
      display.setTextSize(2);
      display.setTextColor(BLACK);
      display.println(F("START"));
      display.setCursor(15, GAME_WIN_MIN_Y + 25);
      display.setTextSize(1);
      display.println(F("Press <Button>"));
      display.setCursor(30, GAME_WIN_MIN_Y + 35);
      display.println(F("to begin"));

      checkButtonPress(BUTTON_ERROR);

      if (b_press) {
        game_start = false;
        b_press_cnt = 0;
        b_press = false;
      }
      display.display();
      delay(myDelay);
    }

    //AFTER GAME START
    display.clearDisplay();

    x_read = analogRead(X_PIN);
    y_read = analogRead(Y_PIN);


    /*  TVORBA BULLETU
    *     BUTTON_ERROR = pocet potrebnych 0V po sobe
    *     MAX_BULLETS  = maximalni pocet bulletu na obrazovce
    */
    checkButtonPress(BUTTON_ERROR);

    if (b_press) {
      for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullet_exist[i]) {
          bullet_exist[i] = true;
          bullet_posX[i] = x_pos;
          bullet_posY[i] = y_pos;
          break;
        }
      }
      b_press = false;
      //b_press_cnt = 0;   // <-- DRZENI TLACITKA BUDE OPAKOVANE STRILET
    }

    for (int i = 0; i < MAX_BULLETS; i++) {
      if (bullet_exist[i]) {
        display.drawPixel(bullet_posX[i], bullet_posY[i], WHITE);
        bullet_posX[i] += BULLET_SPEED;
        if (bullet_posX[i] > GAME_WIN_MAX_X) {
          bullet_exist[i] = false;
          bullet_posX[i] = 0;
          bullet_posY[i] = 0;
        }
      }
    }


    // METEOR SPAWN A POHYB

    if (random(0, meteor_max_chance) == 0) {
      meteor_spawn = true;
    }

    if (meteor_spawn) {
      for (int i = 0; i < MAX_METEOR; i++) {
        if (!meteor_exist[i]) {
          meteor_exist[i] = true;
          meteor_size[i] = random(4, 7);
          meteor_posX[i] = GAME_WIN_MAX_X + meteor_size[i];
          meteor_posY[i] = random(GAME_WIN_MIN_Y + meteor_size[i], GAME_WIN_MAX_Y - meteor_size[i]);
          if (meteor_hard) {
            meteor_speed[i] = random(1, 3);
          } else {
            meteor_speed[i] = 1;
          }
          break;
        }
      }
      meteor_spawn = false;
    }

    // SPAWN A REMOVE PO ODLETENI Z OBRAZOVKY
    for (int i = 0; i < MAX_METEOR; i++) {
      if (meteor_exist[i]) {
        display.fillCircle(meteor_posX[i], meteor_posY[i], meteor_size[i], WHITE);
        meteor_posX[i] -= meteor_speed[i];
        if (meteor_posX[i] < GAME_WIN_MIN_X - meteor_size[i]) {
          meteor_exist[i] = false;
        }
      }
    }

    if (score >= 1500) {
      meteor_max_chance = 50;
    } else if (score >= 1000) {
      meteor_max_chance = 75;
      meteor_hard = true;
    } else if (score >= 500) {
      meteor_max_chance = 25;
    } else if (score >= 250) {
      meteor_max_chance = 50;
    } else if (score >= 50) {
      meteor_max_chance = 75;
    }

    // RECTANGLE ON LEFTSIDE OF GAME SCREEN FOR COVERING METEORS
    display.fillRect(0, GAME_WIN_MIN_Y, GAME_WIN_MIN_X, SCREEN_HEIGHT, BLACK);


    // HITBOXES
    for (int mete = 0; mete < MAX_METEOR; mete++) {
      if (meteor_exist[mete]) {
        int mete_x_p = meteor_posX[mete] + meteor_size[mete];
        int mete_x_n = meteor_posX[mete] - meteor_size[mete];
        int mete_y_p = meteor_posY[mete] + meteor_size[mete];
        int mete_y_n = meteor_posY[mete] - meteor_size[mete];
        
        if (((x_pos >= mete_x_n) && (x_pos <= mete_x_p)) || ((x_pos - PLAYER_SIZE_X >= mete_x_n) && (x_pos - PLAYER_SIZE_X <= mete_x_p))) {
          if (((y_pos - PLAYER_SIZE_Y >= mete_y_n) && (y_pos - PLAYER_SIZE_Y <= mete_y_p)) || ((y_pos + PLAYER_SIZE_Y >= mete_y_n) && (y_pos + PLAYER_SIZE_Y <= mete_y_p))) {
            get_shot = true;
            meteor_exist[mete] = false;
            break;
          }
        }

        //BULLET HITBOX
        for (int bull = 0; bull < MAX_BULLETS; bull++) {
          if (bullet_exist[bull]) {
            if ((bullet_posX[bull] >= mete_x_n) && (bullet_posX[bull] <= mete_x_p)) {
              if ((bullet_posY[bull] >= mete_y_n) && (bullet_posY[bull] <= mete_y_p)) {
                display.fillCircle(meteor_posX[mete], meteor_posY[mete], meteor_size[mete], INVERSE);
                bullet_exist[bull] = false;
                meteor_exist[mete] = false;
                float score_multiplier = 1.0 + (10.0 - meteor_size[mete]) / 10.0;
                if (meteor_speed[mete] == 2)
                  score_multiplier *= 2;
                score += 10 * score_multiplier;
              }
            }
          }
        }
      }
    }

    if (get_shot) {
      lifes--;
      clearGameWindowOnHit();
      for (int i = 0; i < MAX_METEOR; i++) {
        meteor_exist[i] = false;
        meteor_posX[i] = 0;
        meteor_posY[i] = 0;
        meteor_size[i] = 0;
        meteor_speed[i] = 1;
      }
      get_shot = false;
    }

  

    switch (lifes) {
      case 0:
        game_end = true;
        memcpy(heart_0, heart_empty, sizeof(heart_empty[0]) * (IMG_HEIGHT + IMG_WIDTH));
      case 1:
        memcpy(heart_1, heart_empty, sizeof(heart_empty[0]) * (IMG_HEIGHT + IMG_WIDTH));
      case 2:
        memcpy(heart_2, heart_empty, sizeof(heart_empty[0]) * (IMG_HEIGHT + IMG_WIDTH));
      case 3:
        break;
    }



    /*  COMMANDY PRO POHYB HRACE
    *     SPEED_LIMITATION = nizsi je rychlejsi, vyssi je pomalejsi
    *
    */
    x_move = (x_read / SPEED_LIMITATION) - 3;
    y_move = (y_read / SPEED_LIMITATION) - 3;
    if (x_move > 3)
      x_move = 3;
    if (y_move > 3)
      y_move = 3;

    x_pos += x_move * 1;
    y_pos += y_move * 1;

    if (x_pos < GAME_WIN_MIN_X + PLAYER_SIZE_X + 1)
      x_pos = GAME_WIN_MIN_X + PLAYER_SIZE_X + 1;
    else if (x_pos > GAME_WIN_MAX_X - 2)
      x_pos = GAME_WIN_MAX_X - 2;

    if (y_pos < GAME_WIN_MIN_Y + PLAYER_SIZE_Y + 1)
      y_pos = GAME_WIN_MIN_Y + PLAYER_SIZE_Y + 1;
    else if (y_pos > GAME_WIN_MAX_Y - PLAYER_SIZE_Y - 2)
      y_pos = GAME_WIN_MAX_Y - PLAYER_SIZE_Y - 2;


    // PLAYER
    display.drawLine(x_pos, y_pos, x_pos - PLAYER_SIZE_X, y_pos - PLAYER_SIZE_Y, WHITE);
    display.drawLine(x_pos, y_pos, x_pos - PLAYER_SIZE_X, y_pos + PLAYER_SIZE_Y, WHITE);
    display.drawLine(x_pos - PLAYER_SIZE_X, y_pos - PLAYER_SIZE_Y, x_pos - PLAYER_SIZE_X, y_pos + PLAYER_SIZE_Y, WHITE);

    gameMainWindow();
    display.display();
    delay(myDelay);


    while (game_end) {
      display.clearDisplay();
      gameMainWindow();
      display.fillRect(GAME_WIN_MIN_X, GAME_WIN_MIN_Y, GAME_WIN_MAX_X - 1, GAME_WIN_MAX_Y, WHITE);
      display.setCursor(GAME_WIN_MIN_X + 1, GAME_WIN_MIN_Y + 8);
      display.setTextSize(2);
      display.setTextColor(BLACK);
      display.println(F("GAME OVER"));
      display.setCursor(15, GAME_WIN_MIN_Y + 25);
      display.setTextSize(1);
      display.println(F("Press <Button>"));
      display.setCursor(20, GAME_WIN_MIN_Y + 35);
      display.println(F("to continue"));


      checkButtonPress(BUTTON_ERROR);


      display.display();
      delay(myDelay);

      if (b_press) {

        b_press = false;

        display.clearDisplay();

        int y_offset = 10;
        myDelay = 100;
        while (true) {
          display.clearDisplay();

          display.setCursor(0, y_offset);
          display.setTextColor(WHITE);
          display.setTextSize(2);
          if (score > high_score)
            display.println(" NEW HIGH");
          else
            display.println("  FINAL");
          display.println("  SCORE");
          display.print("    ");
          display.println(score);

          display.display();
          delay(myDelay);

          cnt++;
          if (cnt > 3) {
            y_offset--;

            if (y_offset < -20)
              myDelay = 10;

            int letter_A = 0;
            int letter_B = 0;
            int letter_C = 0;
            int opt = 0;
            y_read_cnt = 0;
            int letter_change = 0;
            // ZADANI JMENA PO NOVEM SCORE
            while (y_offset < -33) {
              display.clearDisplay();

              display.setCursor(0, 0);
              display.setTextColor(WHITE);
              display.setTextSize(2);
              display.print("    ");
              display.println(score);

              display.setTextSize(1);
              display.println();
              display.println("    Choose name");
              display.println();

              display.setTextSize(2);


              // POSUN VODOROVNE
              x_read = analogRead(X_PIN);
              if (x_read == 0) {
                x_read_cnt++;
                if (x_read_cnt == MARGIN_ERROR) {
                  opt--;
                  if (opt < 0) {
                    opt = 0;
                  }
                }
              } else if (x_read > APROX_MAX_VOLT) {
                x_read_cnt++;
                if (x_read_cnt == MARGIN_ERROR) {
                  opt++;
                  if (opt > 3) {
                    opt = 3;
                  }
                }
              } else {
                x_read_cnt = 0;
              }

              //POSUN HORIZONTALNE
              y_read = analogRead(Y_PIN);

              if (y_read == 0) {
                y_read_cnt++;
                if (y_read_cnt == MARGIN_ERROR) {
                  letter_change = -1;
                }
              } else if (y_read > APROX_MAX_VOLT) {
                y_read_cnt++;
                if (y_read_cnt == MARGIN_ERROR) {
                  letter_change = 1;
                }
              } else {
                y_read_cnt = 0;
              }

              // ZMENA VYBRANEHO PISMENE
              switch (opt) {
                case 0:
                  letter_A += letter_change;
                  if (letter_A > 25)
                    letter_A = 0;
                  else if (letter_A < 0)
                    letter_A = 25;
                  break;
                case 1:
                  letter_B += letter_change;
                  if (letter_B > 25)
                    letter_B = 0;
                  else if (letter_B < 0)
                    letter_B = 25;
                  break;
                case 2:
                  letter_C += letter_change;
                  if (letter_C > 25)
                    letter_C = 0;
                  else if (letter_C < 0)
                    letter_C = 25;
                  break;
              }
              letter_change = 0;

              scoreWindowLettersDisplay(opt, letter_A, letter_B, letter_C);

              if (opt == 3) {
                checkButtonPress(BUTTON_ERROR);
                if (b_press) {
                  saveNameToScores(letter_A, letter_B, letter_C);
                  if (score > high_score)
                    high_score = score;
                  reset();
                  games_played++;
                  if (games_played > 6)
                    games_played = 6;
                  break;
                }
              }


              display.display();
              delay(myDelay);
            }
            // KONEC ZADANI JMENA PO NOVEM SCORE

            // nasledujici podminka bude splnena jen po odchodu z predchoziho while, ukoncujici while, ve kterem se nachazi
            if (y_offset < -33)
              break;
          }
        }
        reset();
      }  // if (b_press)
    }
  }
  // KONEC PRVNI ZVOLENE MOZNOSTI Z HLAVNIHO MENU = PLAY


  // DRUHA ZVOLENA MOZNOST Z HLAVNIHO MENU = SCORE
  while (chosen == 1) {
    display.clearDisplay();

    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.setTextSize(2);

    if (high_score != -1) {
      for (int i = x_cursor; i < x_cursor+4; i++) {
        if (score_names[i][0] != ' ') {
          display.print(score_names[i][0]);
          display.print(score_names[i][1]);
          display.print(score_names[i][2]);
          display.print(" | ");
          display.println(score_nums[i]);
        } else
          break;
      }

      x_read = analogRead(Y_PIN);
      if (x_read == 0) {
        x_read_cnt++;
        if (x_read_cnt == MARGIN_ERROR) {
          x_cursor--;
          if (x_cursor < 0)
            x_cursor = 0;
        }
      } else if (x_read > APROX_MAX_VOLT) {
        x_read_cnt++;
        if (x_read_cnt == MARGIN_ERROR) {
          x_cursor++;
          if (x_cursor > games_played)
            x_cursor = games_played;
        }
      } else {
        x_read_cnt = 0;
      }

    } else {
      display.println("There has");
      display.println("not been");
      display.println("set any");
      display.println("scores yet");
    }


    checkButtonPress(BUTTON_ERROR);
    if (b_press) {
      reset();
      break;
    }

    display.display();
    delay(myDelay);
  }
  // KONEC DRUHE ZVOLENE MOZNOSTI Z HLAVNIHO MENU = SCORE


  // TRETI ZVOLENA MOZNOST Z HLAVNIHO MENU = EXIT
  while (chosen == 2) {
    display.clearDisplay();
    display.display();
    delay(myDelay);
  }
  // KONEC TRETI ZVOLENE MOZNOSTI Z HLAVNIHO MENU = EXIT
}

void mainMenuDisplay(int input) {
  display.setCursor(0, 0);
  display.setTextSize(2);
  if (input == 0) {
    display.setTextColor(BLACK, WHITE);
    display.println(F("PLAY"));
    display.setTextColor(WHITE);
    display.println(F("SCORES"));
    display.println(F("EXIT"));
  } else if (input == 1) {
    display.setTextColor(WHITE);
    display.println(F("PLAY"));
    display.setTextColor(BLACK, WHITE);
    display.println(F("SCORES"));
    display.setTextColor(WHITE);
    display.println(F("EXIT"));
  } else if (input == 2) {
    display.setTextColor(WHITE);
    display.println(F("PLAY"));
    display.println(F("SCORES"));
    display.setTextColor(BLACK, WHITE);
    display.println(F("EXIT"));
  }
  display.display();
}

void reset() {
  lifes = 3;
  get_shot = false;
  y_read_cnt = 0;
  x_read_cnt = 0;
  cnt = 0;
  score = 0;
  choice = 0;
  b_press_cnt = 0;
  b_press = false;
  game_end = false;
  game_start = true;
  chosen = -1;
  x_cursor = 0;
  x_pos = (GAME_WIN_MAX_X + GAME_WIN_MIN_X) / 2;
  y_pos = (GAME_WIN_MAX_Y + GAME_WIN_MIN_Y) / 2;

  for (int i = 0; i < MAX_BULLETS; i++) {
    bullet_exist[i] = false;
    bullet_posX[i] = 0;
    bullet_posY[i] = 0;
  }
  for (int i = 0; i < MAX_METEOR; i++) {
    meteor_exist[i] = false;
    meteor_posX[i] = 0;
    meteor_posY[i] = 0;
    meteor_size[i] = 0;
    meteor_speed[i] = 1;
  }

  memcpy(heart_0, heart_full, sizeof(heart_full[0]) * (IMG_HEIGHT + IMG_WIDTH));
  memcpy(heart_1, heart_full, sizeof(heart_full[0]) * (IMG_HEIGHT + IMG_WIDTH));
  memcpy(heart_2, heart_full, sizeof(heart_full[0]) * (IMG_HEIGHT + IMG_WIDTH));
}

void checkButtonPress(int error_value) {
  b_read = digitalRead(B_PIN);
  if (b_read == LOW) {
    b_press_cnt++;
    if (b_press_cnt == error_value) {
      b_press = true;
    }
  } else {
    b_press_cnt = 0;
    b_press = false;
  }
}

void gameMainWindow() {
  display.fillRect(GAME_WIN_MAX_X, GAME_WIN_MIN_Y, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
  display.drawBitmap(112, 10, heart_0, IMG_WIDTH, IMG_HEIGHT, 1);
  display.drawBitmap(112, 26, heart_1, IMG_WIDTH, IMG_HEIGHT, 1);
  display.drawBitmap(112, 42, heart_2, IMG_WIDTH, IMG_HEIGHT, 1);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("SCORE: ");
  display.println(score);
  display.drawRect(GAME_WIN_MIN_X, GAME_WIN_MIN_Y, GAME_WIN_MAX_X - GAME_WIN_MIN_X, GAME_WIN_MAX_Y - GAME_WIN_MIN_Y, WHITE);
}

void clearGameWindowOnHit() {
  bool swap = false;
  for (int i = 0; i < GAME_WIN_MAX_Y; i += 4) {
    display.drawCircle(x_pos, y_pos, i, WHITE);

    display.fillRect(0, 0, GAME_WIN_MIN_X, SCREEN_HEIGHT, BLACK);
    display.fillRect(0, 0, SCREEN_WIDTH, GAME_WIN_MIN_Y, BLACK);
    display.fillRect(GAME_WIN_MAX_X, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);
    display.fillRect(0, GAME_WIN_MAX_Y, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);

    gameMainWindow();
    if (swap) {
      switch (lifes) {
        case 0:
          memcpy(heart_0, heart_empty, sizeof(heart_empty[0]) * (IMG_HEIGHT + IMG_WIDTH));
          break;
        case 1:
          memcpy(heart_1, heart_empty, sizeof(heart_empty[0]) * (IMG_HEIGHT + IMG_WIDTH));
          break;
        case 2:
          memcpy(heart_2, heart_empty, sizeof(heart_empty[0]) * (IMG_HEIGHT + IMG_WIDTH));
          break;
      }
      swap = false;
    } else {
      switch (lifes) {
        case 0:
          memcpy(heart_0, heart_full, sizeof(heart_full[0]) * (IMG_HEIGHT + IMG_WIDTH));
          break;
        case 1:
          memcpy(heart_1, heart_full, sizeof(heart_full[0]) * (IMG_HEIGHT + IMG_WIDTH));
          break;
        case 2:
          memcpy(heart_2, heart_full, sizeof(heart_full[0]) * (IMG_HEIGHT + IMG_WIDTH));
          break;
      }
      swap = true;
    }

    display.drawLine(x_pos, y_pos, x_pos - PLAYER_SIZE_X, y_pos - PLAYER_SIZE_Y, WHITE);
    display.drawLine(x_pos, y_pos, x_pos - PLAYER_SIZE_X, y_pos + PLAYER_SIZE_Y, WHITE);
    display.drawLine(x_pos - PLAYER_SIZE_X, y_pos - PLAYER_SIZE_Y, x_pos - PLAYER_SIZE_X, y_pos + PLAYER_SIZE_Y, WHITE);

    display.display();
    delay(60);
  }
}

void scoreWindowLettersDisplay(int opt, int letter_A, int letter_B, int letter_C) {
  switch (opt) {
    case 0:
      display.setTextColor(WHITE);
      display.print("  ");
      display.setTextColor(BLACK, WHITE);
      display.print(alphabet[letter_A]);
      display.setTextColor(WHITE);
      display.print(" ");
      display.print(alphabet[letter_B]);
      display.print(" ");
      display.print(alphabet[letter_C]);
      display.print(" ");
      display.print("OK");
      break;
    case 1:
      display.setTextColor(WHITE);
      display.print("  ");
      display.print(alphabet[letter_A]);
      display.print(" ");
      display.setTextColor(BLACK, WHITE);
      display.print(alphabet[letter_B]);
      display.setTextColor(WHITE);
      display.print(" ");
      display.print(alphabet[letter_C]);
      display.print(" ");
      display.print("OK");
      break;
    case 2:
      display.setTextColor(WHITE);
      display.print("  ");
      display.print(alphabet[letter_A]);
      display.print(" ");
      display.print(alphabet[letter_B]);
      display.print(" ");
      display.setTextColor(BLACK, WHITE);
      display.print(alphabet[letter_C]);
      display.setTextColor(WHITE);
      display.print(" ");
      display.print("OK");
      break;
    case 3:
      display.setTextColor(WHITE);
      display.print("  ");
      display.print(alphabet[letter_A]);
      display.print(" ");
      display.print(alphabet[letter_B]);
      display.print(" ");
      display.print(alphabet[letter_C]);
      display.print(" ");
      display.setTextColor(BLACK, WHITE);
      display.print("OK");
      break;
  }
}



void saveNameToScores(int letter_A, int letter_B, int letter_C) {
  int temp_int;
  char temp_char;
  for (int i = 0; i < MAX_NUM_OF_SCORES; i++) {
    if (score_nums[i] < score) {
      for (int j = MAX_NUM_OF_SCORES - 1; j > i; j--) {
        score_names[j][0] = score_names[j - 1][0];
        score_names[j][1] = score_names[j - 1][1];
        score_names[j][2] = score_names[j - 1][2];
        score_nums[j] = score_nums[j - 1];
      }
      score_names[i][0] = alphabet[letter_A];
      score_names[i][1] = alphabet[letter_B];
      score_names[i][2] = alphabet[letter_C];
      score_nums[i] = score;
      break;
    }
  };
}
