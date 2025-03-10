#include <stdio.h>
#include <oled.h>
#include <avr/io.h>
#include <timer.h>
#include <uart.h>
#include <gpio.h>
#include <avr/interrupt.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define LEFT  3
#define RIGHT 4
#define UP    5
#define DOWN  6
#define SNAKE_SPEED 5
#define STARTING_X_POS 32
#define STARTING_Y_POS 32
#define WINDOW_MIN_X 1
#define WINDOW_MAX_X SCREEN_WIDTH-1
#define WINDOW_MIN_Y 1
#define WINDOW_MAX_Y SCREEN_HEIGHT-1

int direction;                              // Current direction of snakes movement
int prev_dir;                               // Direction of snakes movement in his last move
int cycle_cnt;                              // Counter of cycles of the main program
int x_pos;                                  // X position of snakes head
int y_pos;                                  // Y position of snakes head
int pix[SCREEN_WIDTH][SCREEN_HEIGHT][1];    // Array of all pixels. Pixel can either be unoccupied (-1) 
                                            // or show direction of snakes movement from that pixel (3-6)
int x_treat;                                // X position of treat
int y_treat;                                // Y position of treat
time_t t;                                   // Variable for creating randomness
int x_tail;                                 // X position of tail
int y_tail;                                 // Y position of tail
int temp;                                   // Temporary variable for storing pixel data
bool death;                                 // Whether the snake is dead (=true) or alive (=false)
int score;                                  // Current player score defined by (the amount of treats eaten*10)

/*
* Function:   reset
* --------------------------  
* Sets all global variables to theirs starting values.
* Displays game window, players starting position and randomly placed treat
*/
void reset(){
    direction = RIGHT;
    prev_dir = RIGHT;
    cycle_cnt = 0;
    x_pos = STARTING_X_POS;
    y_pos = STARTING_Y_POS;
    x_tail = STARTING_X_POS-2;
    y_tail = STARTING_Y_POS;
    temp = 0;
    death = false;
    score = 0;

    for (int i = 0; i<SCREEN_WIDTH; i++){
        for (int j = 0; j<SCREEN_HEIGHT; j++){
            pix[i][j][0] = -1;
        }
    }

    /* Generating new treat position.
    *   x_treat ∈ (WINDOW_MIN_X, WINDOW_MAX_X)
    *   y_treat ∈ (WINDWO_MIN_Y, WINDOW_MAX_Y)
    */
    while (1){
        x_treat = rand() % (WINDOW_MAX_X-1);
        y_treat = rand() % (WINDOW_MAX_Y-1);
        if ((x_treat > WINDOW_MIN_X) && (y_treat > WINDOW_MIN_Y)){
            if ((pix[x_treat][y_treat][0] == -1) && (x_treat != x_pos) && (y_treat != y_pos))
                break;
        }
    }

    oled_clrscr();
    oled_drawRect(WINDOW_MIN_X, WINDOW_MIN_Y, WINDOW_MAX_X-WINDOW_MIN_X, WINDOW_MAX_Y-WINDOW_MIN_Y, WHITE);
    oled_drawPixel(STARTING_X_POS-2, STARTING_Y_POS, WHITE);
    oled_drawPixel(STARTING_X_POS-1, STARTING_Y_POS, WHITE);
    oled_drawPixel(STARTING_X_POS-0, STARTING_Y_POS, WHITE);
    pix[STARTING_X_POS-2][STARTING_Y_POS][0] = RIGHT;
    pix[STARTING_X_POS-1][STARTING_Y_POS][0] = RIGHT;
    pix[STARTING_X_POS-0][STARTING_Y_POS][0] = -1;
    oled_drawPixel(x_treat, y_treat, WHITE);
    oled_display();
}
// end of reset()


void main(){
    // Initiation of display, buttons and random number generation
    uart_init(UART_BAUD_SELECT(115200, F_CPU));
    TIM2_OVF_16MS
    TIM2_OVF_ENABLE
    sei();

    GPIO_mode_input_pullup(&DDRD, LEFT);
    GPIO_mode_input_pullup(&DDRD, RIGHT);
    GPIO_mode_input_pullup(&DDRD, UP);
    GPIO_mode_input_pullup(&DDRD, DOWN);

    oled_init(OLED_DISP_ON);
    oled_clrscr();
    oled_display();

    srand((unsigned) time(&t));

    reset();
}
// end of main()


ISR(TIMER2_OVF_vect){
    // Game is running (player has not died yet):
    if (!death) {

        // Reads buttons and sets direction based on pressed button
        if (GPIO_read(&PIND,LEFT) == 0)
            direction = LEFT;
        else if (GPIO_read(&PIND,RIGHT) == 0)
            direction = RIGHT;
        else if (GPIO_read(&PIND,UP) == 0)
            direction = UP;
        else if (GPIO_read(&PIND,DOWN) == 0)
            direction = DOWN;

        cycle_cnt++;
        // Snake moves only every (16MS * SNAKE_SPEED)
        if (cycle_cnt >= SNAKE_SPEED) {
            cycle_cnt = 0;

            // Current direction can't be direct opposite of previous direction
            if (((prev_dir==RIGHT) && (direction==LEFT)) || ((prev_dir==LEFT) && (direction==RIGHT)))
                direction = prev_dir;
            else if (((prev_dir==UP) && (direction==DOWN)) || ((prev_dir==DOWN) && (direction==UP)))
                direction = prev_dir;
            else
                prev_dir = direction;

            // Changing snake head position
            pix[x_pos][y_pos][0] = direction;
            switch (direction) {
                case LEFT:
                    x_pos--;
                    break;
                case RIGHT:
                    x_pos++;
                    break;
                case UP:
                    y_pos--;
                    break;
                case DOWN:
                    y_pos++;
                    break;
            }

    	    // Determining snakes death. Either by leaving game window or biting itself.
            if ((x_pos <= WINDOW_MIN_X) || (x_pos >= WINDOW_MAX_X) || (y_pos <= WINDOW_MIN_Y) || (y_pos >= WINDOW_MAX_Y))
                death = true;
            if (pix[x_pos][y_pos][0] != -1)
                death = true;

            // Displaying snakes head
            oled_drawPixel(x_pos, y_pos, WHITE);

            // Determining, whether the treat has been eaten. If so, generate a new treat position and displays it.
            if ((x_pos == x_treat) && (y_pos == y_treat)){
                score += 10;
                while (1){
                    x_treat = rand() % (WINDOW_MAX_X-1);
                    y_treat = rand() % (WINDOW_MAX_Y-1);
                    if ((x_treat > WINDOW_MIN_X) && (y_treat > WINDOW_MIN_Y)){
                        if ((pix[x_treat][y_treat][0] == -1) && (x_treat != x_pos) && (y_treat != y_pos))
                            break;
                    }
                }
                oled_drawPixel(x_treat, y_treat, WHITE);
            }
            // Treat has not been eaten in this move. Moves snakes tail in direction of its value in pix array
            else {
                oled_drawPixel(x_tail, y_tail, BLACK);
                temp = pix[x_tail][y_tail][0];
                pix[x_tail][y_tail][0] = -1;
                switch (temp) {
                    case LEFT:
                        x_tail--;
                        break;
                    case RIGHT:
                        x_tail++;
                        break;
                    case UP:
                        y_tail--;
                        break;
                    case DOWN:
                        y_tail++;
                        break;
                }
            }
        }
        // Finishes displaying
        oled_display();
    }
    // Game is not running (player has died):
    else {
        // Displays "GAME OVER" and players final score
        oled_clrscr();
        oled_charMode(DOUBLESIZE);
        oled_puts("GAME OVER");
        oled_charMode(NORMALSIZE);
        oled_gotoxy(0, 2);
        oled_puts("Score: " + score);
        oled_display();

        // If any button is pressed, resets the game to its starting state - no need to start the whole program again
        if ((GPIO_read(&PIND,LEFT) == 0) || (GPIO_read(&PIND,RIGHT) == 0) || (GPIO_read(&PIND,UP) == 0) || (GPIO_read(&PIND,DOWN) == 0))
            reset();
    }
}
// end of ISR(TIMER2_OVF_vect)