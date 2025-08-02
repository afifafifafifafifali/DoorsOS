#include "snake.h"
#include "gui/windows.h"
#include "ps2/kbio.h"
#include "mem/heap.h"
#include "libs/string.h"
#include "gui/colorama.h"
#include <stdbool.h>
#include "interrupts/timer.h"
#define BOARD_WIDTH 40
#define BOARD_HEIGHT 20
#define CELL_SIZE 10

typedef struct {
    int x, y;
} Point;

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

static Point snake[BOARD_WIDTH * BOARD_HEIGHT];
static int snake_length;
static Direction dir;

static Point food;

static bool game_over = false;

static uint32_t bgColor = COLOR_RGB_YELLOW;     
static uint32_t snakeColor = 0x00FF00;  // green
static uint32_t foodColor = 0xFF0000;   // red

static char input_buffer[64];  // buffer for ps2_kbio_read input

static void place_food(void) {
    while (1) {
        int x = rand() % BOARD_WIDTH;
        int y = rand() % BOARD_HEIGHT;

        bool conflict = false;
        for (int i = 0; i < snake_length; i++) {
            if (snake[i].x == x && snake[i].y == y) {
                conflict = true;
                break;
            }
        }
        if (!conflict) {
            food.x = x;
            food.y = y;
            return;
        }
    }
}

static void draw_cell(int x, int y, uint32_t color) {
    int px = x * CELL_SIZE;
    int py = y * CELL_SIZE;
    draw_rect(px, py, CELL_SIZE, CELL_SIZE, color);
}

static void draw_board(void) {
    draw_rect(0, 0, BOARD_WIDTH * CELL_SIZE, BOARD_HEIGHT * CELL_SIZE, bgColor);
    for (int i = 0; i < snake_length; i++) {
        draw_cell(snake[i].x, snake[i].y, snakeColor);
    }
    draw_cell(food.x, food.y, foodColor);
}

static bool point_equal(Point a, Point b) {
    return a.x == b.x && a.y == b.y;
}

static void move_snake(void) {
    // Move body
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Move head by direction
    switch (dir) {
        case UP:    snake[0].y--; break;
        case DOWN:  snake[0].y++; break;
        case LEFT:  snake[0].x--; break;
        case RIGHT: snake[0].x++; break;
    }

    // Wrap around board edges
    if (snake[0].x < 0) snake[0].x = BOARD_WIDTH - 1;
    if (snake[0].x >= BOARD_WIDTH) snake[0].x = 0;
    if (snake[0].y < 0) snake[0].y = BOARD_HEIGHT - 1;
    if (snake[0].y >= BOARD_HEIGHT) snake[0].y = 0;

    // Check collision with self
    for (int i = 1; i < snake_length; i++) {
        if (point_equal(snake[0], snake[i])) {
            game_over = true;
            return;
        }
    }

    // Check if food eaten
    if (point_equal(snake[0], food)) {
        if (snake_length < BOARD_WIDTH * BOARD_HEIGHT) {
            snake_length++;
            snake[snake_length - 1] = snake[snake_length - 2]; // extend tail
        }
        place_food();
    }
}

static void handle_input(void) {
    // ps2_kbio_read blocks until Enter, so we read one char and react on it:
    string_t input = ps2_kbio_read(input_buffer, sizeof(input_buffer));
    if (!input) return;
    char c = input[0];

    switch (c) {
        case 'w': if (dir != DOWN) dir = UP; break;
        case 's': if (dir != UP) dir = DOWN; break;
        case 'a': if (dir != RIGHT) dir = LEFT; break;
        case 'd': if (dir != LEFT) dir = RIGHT; break;
        case 'q': game_over = true; break;
    }
}

void snake_init(void) {
    snake_length = 4;
    dir = RIGHT;

    for (int i = 0; i < snake_length; i++) {
        snake[i].x = snake_length - 1 - i;
        snake[i].y = BOARD_HEIGHT / 2;
    }

    place_food();
    draw_rect(0, 0, BOARD_WIDTH * CELL_SIZE, BOARD_HEIGHT * CELL_SIZE, bgColor);
}

void snake_run(void) {
    snake_init();
    while (!game_over) {
        draw_board();
        handle_input();
        move_snake();

        timer_sleep_ms(100);  // Control game speed
    }

    // Game over screen
    draw_rect(0, 0, BOARD_WIDTH * CELL_SIZE, BOARD_HEIGHT * CELL_SIZE, 0x000000);
    kprint_color_at(5, BOARD_HEIGHT / 2, "GAME OVER! Press q to exit", 0xFF0000, true, 0x000000, true);

    // Wait until user presses 'q' to quit
    while (1) {
        string_t input = ps2_kbio_read(input_buffer, sizeof(input_buffer));
        if (input && input[0] == 'q') break;
    }
}
