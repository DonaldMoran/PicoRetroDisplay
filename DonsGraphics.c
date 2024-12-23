/*
 * Command Reference:
 * 
 * Change Text Color:
 * /TEXT [color_code]
 * Example: /TEXT R   (Changes text color to Red)
 *
 * Change Background Color:
 * /BACK [color_code]
 * Example: /BACK B   (Changes background color to Dark Blue)
 *
 * Clear Screen:
 * /CLS
 * Example: /CLS
 *
 * Draw Line:
 * /LINE x1 y1 x2 y2 color
 * Example: /LINE 10 10 100 100 R   (Draws a red line from coordinates (10,10) to (100,100))
 *
 * Draw Rectangle:
 * /RECT x y width height color
 * Example: /RECT 50 50 100 100 G   (Draws a green rectangle with top-left corner at (50,50) and size 100x100)
 *
 * Fill Rectangle:
 * /FILLRECT x y width height color
 * Example: /FILLRECT 50 50 100 100 B   (Draws and fills a dark blue rectangle with top-left corner at (50,50) and size 100x100)
 *
 * Draw Circle:
 * /CIRCLE x y radius color
 * Example: /CIRCLE 200 200 50 Y   (Draws a yellow circle with center at (200,200) and radius 50)
 *
 * Fill Circle:
 * /FILLCIRCLE x y radius color
 * Example: /FILLCIRCLE 200 200 50 O   (Draws and fills a dark orange circle with center at (200,200) and radius 50)
 *
 * Draw Rounded Rectangle:
 * /ROUNDRECT x y width height radius color
 * Example: /ROUNDRECT 50 50 100 100 10 G   (Draws a green rounded rectangle with top-left corner at (50,50), size 100x100, and radius 10)
 *
 * Fill Rounded Rectangle:
 * /FILLROUNDRECT x y width height radius color
 * Example: /FILLROUNDRECT 50 50 100 100 10 B   (Draws and fills a dark blue rounded rectangle with top-left corner at (50,50), size 100x100, and radius 10)
 *
 * Set Font:
 * /FONT [type]
 * Example: /FONT STANDARD   (Sets the font to Standard 5x7)
 * Example: /FONT BRL4   (Sets the font to BRL4)
 *
 * Draw Smiley:
 * /SMILEY
 * Example: /SMILEY
 *
 * Move Sprite:
 * /MOVE_SPRITE start_x start_y end_x end_y delay_ms
 * Example: /MOVE_SPRITE 50 50 200 200 100   (Moves the sprite from (50,50) to (200,200) with a delay of 100ms)
 *
 * Scroll Map:
 * /SCROLL_MAP scroll_amount delay_ms
 * Example: /SCROLL_MAP 10 100   (Scrolls the map to the left by 10 columns with a delay of 100ms)
 *
 * Display Image:
 * /IMAGE x y width height image_data
 * Example: /IMAGE 10 10 100 100 image_data   (Displays an image at coordinates (10,10) with size 100x100)
 *
 * ANSI Escape Codes:
 * 
 * Cursor Position:
 * \033[row;colH
 * Example: \033[10;20H   (Moves the cursor to row 10, column 20)
 *
 * Clear Screen:
 * \033[2J
 * Example: \033[2J   (Clears the entire screen)
 *
 * Set Text Attributes:
 * \033[attribute_code m
 * Example: \033[31m   (Sets the text color to red)
 * 
 * Text Color Codes:
 * 30 - Black, 31 - Red, 32 - Dark Green, 33 - Yellow, 34 - Dark Blue
 * 35 - Magenta, 36 - Cyan, 37 - White
 *
 * Color Codes:
 * R - Red, G - Dark Green, M - Medium Green, C - Cyan, Y - Yellow
 * K - Black, B - Dark Blue, O - Dark Orange, D - Magenta
 * P - Light Pink, r - Red, g - Green, b - Blue, o - Orange
 * c - Light Blue, m - Pink, k - White
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "vga16_graphics.h"
#include <stdbool.h>

extern unsigned short cursor_y, cursor_x;
extern char textcolor, textbgcolor;
extern unsigned char textsize;

#define BUFFER_SIZE 40
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16
#define COLS (SCREEN_WIDTH / CHAR_WIDTH)
#define ROWS (SCREEN_HEIGHT / CHAR_HEIGHT)

#define BLACK 0x0
#define DARK_GREEN 0x1
#define MED_GREEN 0x2
#define GREEN 0x3
#define DARK_BLUE 0x4
#define BLUE 0x5
#define LIGHT_BLUE 0x6
#define CYAN 0x7
#define RED 0x8
#define DARK_ORANGE 0x9
#define ORANGE 0xA
#define YELLOW 0xB
#define MAGENTA 0xC
#define PINK 0xD
#define LIGHT_PINK 0xE
#define WHITE 0xF

typedef struct {
    char character;
    char color;
    char bgcolor;
    bool is_sprite;
} Tile;

Tile tile_map[ROWS][COLS];

// Forward declarations
void redraw_screen();
void clear_screen();
void render_tile_map();
void set_tile(int row, int col, char character, char color, char bgcolor, bool is_sprite);
void drawImage(int x, int y, int width, int height, const char* image);
void drawPETSCIIChar(int x, int y, uint8_t c, char color); // Add this line

char parse_color_code(const char *color_code) {
    if (strcmp(color_code, "R") == 0 || strcmp(color_code, "RED") == 0) return RED;
    if (strcmp(color_code, "G") == 0 || strcmp(color_code, "DARK_GREEN") == 0) return DARK_GREEN;
    if (strcmp(color_code, "M") == 0 || strcmp(color_code, "MED_GREEN") == 0) return MED_GREEN;
    if (strcmp(color_code, "C") == 0 || strcmp(color_code, "CYAN") == 0) return CYAN;
    if (strcmp(color_code, "Y") == 0 || strcmp(color_code, "YELLOW") == 0) return YELLOW;
    if (strcmp(color_code, "K") == 0 || strcmp(color_code, "BLACK") == 0) return BLACK;
    if (strcmp(color_code, "B") == 0 || strcmp(color_code, "DARK_BLUE") == 0) return DARK_BLUE;
    if (strcmp(color_code, "O") == 0 || strcmp(color_code, "DARK_ORANGE") == 0) return DARK_ORANGE;
    if (strcmp(color_code, "D") == 0 || strcmp(color_code, "MAGENTA") == 0) return MAGENTA;
    if (strcmp(color_code, "P") == 0 || strcmp(color_code, "LIGHT_PINK") == 0) return LIGHT_PINK;
    if (strcmp(color_code, "r") == 0) return RED;
    if (strcmp(color_code, "g") == 0 || strcmp(color_code, "GREEN") == 0) return GREEN;
    if (strcmp(color_code, "b") == 0 || strcmp(color_code, "BLUE") == 0) return BLUE;
    if (strcmp(color_code, "o") == 0 || strcmp(color_code, "ORANGE") == 0) return ORANGE;
    if (strcmp(color_code, "c") == 0 || strcmp(color_code, "LIGHT_BLUE") == 0) return LIGHT_BLUE;
    if (strcmp(color_code, "m") == 0 || strcmp(color_code, "PINK") == 0) return PINK;
    if (strcmp(color_code, "k") == 0 || strcmp(color_code, "WHITE") == 0) return WHITE;
    return WHITE; // Default to white if the color code is invalid
}


typedef struct {
    char character;
    char color;
    char bgcolor;
    bool is_standard_font; // Track if the character uses the standard font or BRL4
} ScreenCell;

ScreenCell screen_buffer[ROWS][COLS];
ScreenCell spriteBackground[8][8];

int cursor_row = 0;
int cursor_col = 0;
char current_bg_color = BLACK;
char current_text_color = GREEN;
bool use_standard_font = true; // Declare globally

const uint8_t smiley[8] = {
    0b00111100,
    0b01000010,
    0b10100101,
    0b10000001,
    0b10100101,
    0b10011001,
    0b01000010,
    0b00111100
};

extern unsigned char vga_data_array[]; // Add this line

void saveBackground(int x, int y) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int row = y + i;
            int col = x + j;
            if (row < SCREEN_HEIGHT && col < SCREEN_WIDTH) {
                int screen_row = row / CHAR_HEIGHT;
                int screen_col = col / CHAR_WIDTH;
                spriteBackground[i][j] = screen_buffer[screen_row][screen_col];
            } else {
                spriteBackground[i][j].character = ' ';
                spriteBackground[i][j].color = current_bg_color;
                spriteBackground[i][j].bgcolor = current_bg_color;
                spriteBackground[i][j].is_standard_font = true; // Default to standard font
            }
        }
    }
}

void restoreBackground(int x, int y) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int row = y + i;
            int col = x + j;
            if (row < SCREEN_HEIGHT && col < SCREEN_WIDTH) {
                ScreenCell cell = spriteBackground[i][j];
                drawPixel(col, row, cell.bgcolor); // Restore the background color
                if (cell.character != ' ') {
                    if (cell.is_standard_font) {
                        drawChar((col / CHAR_WIDTH) * CHAR_WIDTH, (row / CHAR_HEIGHT) * CHAR_HEIGHT, cell.character, cell.color, cell.bgcolor, 1);
                    } else {
                        drawCharBig((col / CHAR_WIDTH) * CHAR_WIDTH, (row / CHAR_HEIGHT) * CHAR_HEIGHT, cell.character, cell.color, cell.bgcolor);
                    }
                }
            }
        }
    }
}

void clearSprite(int x, int y, int width, int height) {
    restoreBackground(x, y);
}

void drawSprite(int x, int y, const uint8_t sprite[], int width, int height, char color) {
    saveBackground(x, y);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (sprite[i] & (1 << (width - 1 - j))) {
                drawPixel(x + j, y + i, color); // Use drawPixel to place each sprite pixel
            }
        }
    }
}

void move_sprite(int start_x, int start_y, int end_x, int end_y, int delay_ms) {
    int x = start_x;
    int y = start_y;
    int dx = (end_x > start_x) ? 1 : -1;
    int dy = (end_y > start_y) ? 1 : -1;

    // Save the initial background before starting the move
    saveBackground(x, y);

    while (x != end_x || y != end_y) {
        clearSprite(x, y, 8, 8);
        x += (x != end_x) ? dx : 0;
        y += (y != end_y) ? dy : 0;
        saveBackground(x, y);
        drawSprite(x, y, smiley, 8, 8, YELLOW);
        render_tile_map(); // Ensure the tile map is rendered after each move
        sleep_ms(delay_ms);
    }

    clearSprite(x, y, 8, 8);
    saveBackground(x, y);
    drawSprite(x, y, smiley, 8, 8, YELLOW);
    render_tile_map(); // Ensure the tile map is rendered at the final position
}

// UART initialization
void init_uart() {
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    stdio_uart_init_full(uart0, 115200, 0, 1);
    printf("UART Initialized.\n");
}

// Console initialization
void init_console() {
    cursor_row = 0;
    cursor_col = 0;
    memset(screen_buffer, ' ', sizeof(screen_buffer));
    fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, current_bg_color);
}

void scroll_standard_font() {
    for (int y = 1; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            screen_buffer[y - 1][x] = screen_buffer[y][x];
            if (screen_buffer[y - 1][x].is_standard_font) {
                drawChar(x * CHAR_WIDTH, (y - 1) * CHAR_HEIGHT, screen_buffer[y - 1][x].character, screen_buffer[y - 1][x].color, current_bg_color, 1);
            } else {
                drawCharBig(x * CHAR_WIDTH, (y - 1) * CHAR_HEIGHT, screen_buffer[y - 1][x].character, screen_buffer[y - 1][x].color, current_bg_color);
            }
        }
    }
    for (int x = 0; x < COLS; x++) {
        screen_buffer[ROWS - 1][x].character = ' ';
        screen_buffer[ROWS - 1][x].color = current_text_color;
        screen_buffer[ROWS - 1][x].bgcolor = current_bg_color;
        screen_buffer[ROWS - 1][x].is_standard_font = true; // Ensure the last row is using the standard font
        drawChar(x * CHAR_WIDTH, (ROWS - 1) * CHAR_HEIGHT, ' ', current_text_color, current_bg_color, 1);
    }
}

void scroll_brl4_font() {
    int brl4_char_height = 16;
    int brl4_rows = SCREEN_HEIGHT / brl4_char_height;

    for (int y = 1; y < brl4_rows; y++) {
        for (int x = 0; x < COLS; x++) {
            screen_buffer[y - 1][x] = screen_buffer[y][x];
            if (screen_buffer[y - 1][x].is_standard_font) {
                drawChar(x * CHAR_WIDTH, (y - 1) * CHAR_HEIGHT, screen_buffer[y - 1][x].character, screen_buffer[y - 1][x].color, current_bg_color, 1);
            } else {
                drawCharBig(x * CHAR_WIDTH, (y - 1) * brl4_char_height, screen_buffer[y - 1][x].character, screen_buffer[y - 1][x].color, current_bg_color);
            }
        }
    }

    // Clear the last row to prevent garbling
    for (int x = 0; x < COLS; x++) {
        screen_buffer[brl4_rows - 1][x].character = ' ';
        screen_buffer[brl4_rows - 1][x].color = current_text_color;
        screen_buffer[brl4_rows - 1][x].bgcolor = current_bg_color;
        screen_buffer[brl4_rows - 1][x].is_standard_font = false; // Ensure the last row is using the BRL4 font
        drawCharBig(x * CHAR_WIDTH, (brl4_rows - 1) * brl4_char_height, ' ', current_text_color, current_bg_color);
    }
}

// Optimized scroll screen function
void scroll_screen() {
    int scroll_height = use_standard_font ? CHAR_HEIGHT : 16; // Determine scroll height based on current font

    // Scroll the screen buffer
    for (int row = 1; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            screen_buffer[row - 1][col] = screen_buffer[row][col];
        }
    }

    // Clear the last row
    for (int col = 0; col < COLS; col++) {
        screen_buffer[ROWS - 1][col].character = ' ';
        screen_buffer[ROWS - 1][col].color = current_text_color;
        screen_buffer[ROWS - 1][col].bgcolor = current_bg_color;
        screen_buffer[ROWS - 1][col].is_standard_font = use_standard_font;
    }

    // Redraw the screen buffer
    redraw_screen();
}

// Function to clear the screen buffer and redraw the screen
void clear_and_redraw_screen() {
    fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, current_bg_color); // Clear the entire screen
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            screen_buffer[row][col].character = ' ';
            screen_buffer[row][col].color = current_text_color;
            screen_buffer[row][col].bgcolor = current_bg_color;
            screen_buffer[row][col].is_standard_font = use_standard_font;
        }
    }
    redraw_screen();
}

// Function to redraw the screen buffer
void redraw_screen() {
    fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, current_bg_color); // Clear the entire screen
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            ScreenCell cell = screen_buffer[row][col];
            if (cell.character != ' ') {
                if (cell.is_standard_font) {
                    drawChar(col * CHAR_WIDTH, row * CHAR_HEIGHT, cell.character, cell.color, cell.bgcolor, 1);
                } else {
                    drawCharBig(col * CHAR_WIDTH, row * CHAR_HEIGHT, cell.character, cell.color, cell.bgcolor);
                }
            }
        }
    }
}

// Change font, clear the screen, and reset the cursor position
void change_font(bool standard_font) {
    use_standard_font = standard_font;
    clear_screen(); // Clear the screen
    cursor_row = 0; // Reset cursor position
    cursor_col = 0;
}

// Function to clear the screen
void clear_screen() {
    // Clear the screen buffer and fill the screen with the background color
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            screen_buffer[row][col].character = ' ';
            screen_buffer[row][col].color = current_text_color;
            screen_buffer[row][col].bgcolor = current_bg_color;
            set_tile(row, col, ' ', current_text_color, current_bg_color, false);
        }
    }
    fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, current_bg_color); // Ensure the entire screen is filled with the background color
    // Reset the cursor position
    cursor_row = 0;
    cursor_col = 0;
}

// Change background color
void change_background_color(char color) {
    current_bg_color = color;
    fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, current_bg_color);
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            screen_buffer[row][col].bgcolor = current_bg_color; // Ensure we update the screen buffer background color
            set_tile(row, col, screen_buffer[row][col].character, screen_buffer[row][col].color, current_bg_color, false);
            drawChar(col * CHAR_WIDTH, row * CHAR_HEIGHT, screen_buffer[row][col].character, screen_buffer[row][col].color, current_bg_color, 1);
        }
    }
}

// Change text color
void change_text_color(char color) {
    current_text_color = color;
}

void update_cursor_and_scroll() {
    cursor_col = 0;
    cursor_row++;
    if (cursor_row >= ROWS) {
        cursor_row = ROWS - 1;
        scroll_screen();
    }
}

void update_console(char c) {
    if (c == '\r' || c == '\n') {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= ROWS) {
            cursor_row = ROWS - 1;
            scroll_screen();
        }
    } else if (c == 8 || c == 127) { // Handle backspace
        if (cursor_col > 0) {
            cursor_col--;
        } else if (cursor_row > 0) {
            cursor_row--;
            cursor_col = COLS - 1;
        }
        screen_buffer[cursor_row][cursor_col].character = ' ';
        screen_buffer[cursor_row][cursor_col].color = current_text_color;
        screen_buffer[cursor_row][cursor_col].bgcolor = current_bg_color;
        if (use_standard_font) {
            drawChar(cursor_col * CHAR_WIDTH, cursor_row * CHAR_HEIGHT, ' ', current_text_color, current_bg_color, 1);
        } else {
            drawCharBig(cursor_col * CHAR_WIDTH, cursor_row * 16, ' ', current_text_color, current_bg_color);
        }
    } else if (c >= 32 && c <= 126) {
        screen_buffer[cursor_row][cursor_col].character = c;
        screen_buffer[cursor_row][cursor_col].color = current_text_color;
        screen_buffer[cursor_row][cursor_col].bgcolor = current_bg_color;
        screen_buffer[cursor_row][cursor_col].is_standard_font = use_standard_font; // Track font type
        if (use_standard_font) {
            drawChar(cursor_col * CHAR_WIDTH, cursor_row * CHAR_HEIGHT, c, current_text_color, current_bg_color, 1);
        } else {
            drawCharBig(cursor_col * CHAR_WIDTH, cursor_row * 16, c, current_text_color, current_bg_color);
        }
        cursor_col++;
        if (cursor_col >= COLS) {
            cursor_col = 0;
            cursor_row++;
            if (cursor_row >= ROWS) {
                cursor_row = ROWS - 1;
                scroll_screen();
            }
        }
    }
}

void move_cursor(int row, int col) {
    cursor_row = row;
    cursor_col = col;
    if (cursor_row >= ROWS) cursor_row = ROWS - 1;
    if (cursor_col >= COLS) cursor_col = COLS - 1;
}

void set_text_attributes(const char *seq) {
    int attribute_code = 0;
    // Manually parse the sequence to extract the attribute code
    sscanf(seq, "%d", &attribute_code);

    switch (attribute_code) {
        case 30: current_text_color = BLACK; break;
        case 31: current_text_color = RED; break;
        case 32: current_text_color = DARK_GREEN; break;
        case 33: current_text_color = YELLOW; break;
        case 34: current_text_color = DARK_BLUE; break;
        case 35: current_text_color = MAGENTA; break;
        case 36: current_text_color = CYAN; break;
        case 37: current_text_color = WHITE; break;
        default: current_text_color = WHITE; break; // Default to white if the attribute code is invalid
    }
}

void handle_ansi_escape(const char *seq) {
    if (seq[0] == '[') {
        int row, col;
        switch (seq[1]) {
            case 'H': // Cursor position
                sscanf(seq + 2, "%d;%d", &row, &col);
                move_cursor(row - 1, col - 1);
                break;
            case '2': // Clear screen
                if (seq[2] == 'J') {
                    clear_screen();
                }
                break;
            case '3': // Set text attributes (color)
                set_text_attributes(seq + 1); // Adjusted to start from the correct position
                break;
            case '9': // Set text attributes (color)
                set_text_attributes(seq + 1); // Adjusted to start from the correct position
                break;
            default:
                break;
        }
    }
}

void scroll_map(int scroll_amount, int delay_ms) {
    for (int i = 0; i < scroll_amount; i++) {
        // Shift the tile map to the left
        for (int row = 0; row < ROWS; row++) {
            for (int col = 0; col < COLS - 1; col++) {
                tile_map[row][col] = tile_map[row][col + 1];
            }
            // Set the rightmost column to empty tiles
            set_tile(row, COLS - 1, ' ', current_text_color, current_bg_color, false);
        }

        // Render the tile map
        render_tile_map();

        // Delay for a short period
        sleep_ms(delay_ms);
    }
}

void drawPETSCIIString(int x, int y, const char* str, char color) {
    while (*str) {
        drawPETSCIIChar(x, y, *str++, color);
        x += 8;
        if (x >= SCREEN_WIDTH) {
            x = 0;
            y += 8;
        }
    }
}

void handle_serial_input() {
    static int index = 0;
    static char c;
    static char command[BUFFER_SIZE];
    bool command_mode = false;
    bool ansi_mode = false;
    char ansi_seq[BUFFER_SIZE];
    int ansi_index = 0;

    while (index < BUFFER_SIZE - 1) {
        if (uart_is_readable(uart0)) {
            c = uart_getc(uart0);
            // Remove the debug print statement
            // printf("Received character: %c\n", c);

            if (ansi_mode) {
                if (c == 'm' || c == 'H' || c == 'J') {
                    ansi_seq[ansi_index++] = c;
                    ansi_seq[ansi_index] = '\0';
                    handle_ansi_escape(ansi_seq);
                    ansi_mode = false;
                    ansi_index = 0;
                } else {
                    ansi_seq[ansi_index++] = c;
                }
            } else if (command_mode) {
                uart_putc(uart0, c);
                command[index++] = c;

                if (c == '\r' || c == '\n' || index == BUFFER_SIZE - 1) {
                    command[index] = '\0';
                    command_mode = false;

                    if (strncmp(command, "/TEXT ", 6) == 0 || strncmp(command, "TEXT ", 5) == 0) {
                        char color_code[20];
                        sscanf(command + 6, "%s", color_code);
                        change_text_color(parse_color_code(color_code));
                    } else if (strncmp(command, "/BACK ", 6) == 0 || strncmp(command, "BACK ", 5) == 0) {
                        char color_code[20];
                        sscanf(command + 6, "%s", color_code);
                        change_background_color(parse_color_code(color_code));
                    } else if (strncmp(command, "/CLS", 4) == 0) {
                        clear_screen();
                        //uart_puts(uart0, "\nScreen cleared.\n"); // Optional feedback
                    } else if (strncmp(command, "/LINE ", 6) == 0) {
                        int x1, y1, x2, y2;
                        char color_code[20];
                        sscanf(command + 6, "%d %d %d %d %s", &x1, &y1, &x2, &y2, color_code);
                        drawLine(x1, y1, x2, y2, parse_color_code(color_code));
                    } else if (strncmp(command, "/RECT ", 6) == 0) {
                        int x, y, width, height;
                        char color_code[20];
                        sscanf(command + 6, "%d %d %d %d %s", &x, &y, &width, &height, color_code);
                        drawRect(x, y, width, height, parse_color_code(color_code));
                    } else if (strncmp(command, "/FILLRECT ", 10) == 0) {
                        int x, y, width, height;
                        char color_code[20];
                        sscanf(command + 10, "%d %d %d %d %s", &x, &y, &width, &height, color_code);
                        fillRect(x, y, width, height, parse_color_code(color_code));
                    } else if (strncmp(command, "/CIRCLE ", 8) == 0) {
                        int x, y, radius;
                        char color_code[20];
                        sscanf(command + 8, "%d %d %d %s", &x, &y, &radius, color_code);
                        drawCircle(x, y, radius, parse_color_code(color_code));
                    } else if (strncmp(command, "/FILLCIRCLE ", 12) == 0) {
                        int x, y, radius;
                        char color_code[20];
                        sscanf(command + 12, "%d %d %d %s", &x, &y, &radius, color_code);
                        fillCircle(x, y, radius, parse_color_code(color_code));
                    } else if (strncmp(command, "/ROUNDRECT ", 11) == 0) {
                        int x, y, width, height, radius;
                        char color_code[20];
                        sscanf(command + 11, "%d %d %d %d %d %s", &x, &y, &width, &height, &radius, color_code);
                        drawRoundRect(x, y, width, height, radius, parse_color_code(color_code));
                    } else if (strncmp(command, "/FILLROUNDRECT ", 15) == 0) {
                        int x, y, width, height, radius;
                        char color_code[20];
                        sscanf(command + 15, "%d %d %d %d %d %s", &x, &y, &width, &height, &radius, color_code);
                        fillRoundRect(x, y, width, height, radius, parse_color_code(color_code));
                    } else if (strncmp(command, "/FONT ", 6) == 0 || strncmp(command, "FONT ", 5) == 0) {
                        if (strstr(command, "STANDARD") != NULL) {
                            change_font(true);
                            uart_puts(uart0, "\nFont set to Standard 5x7.\n");
                        } else if (strstr(command, "BRL4") != NULL) {
                            change_font(false);
                            uart_puts(uart0, "\nFont set to BRL4.\n");
                        } else {
                            uart_puts(uart0, "\nInvalid font type.\n");
                        }
                    } else if (strncmp(command, "/SMILEY", 7) == 0 || strncmp(command, "SMILEY", 6) == 0) {
                        int x = 50, y = 50;
                        saveBackground(x, y); // Save initial background state
                        for (int i = 0; i < 50; i++) {
                            clearSprite(x, y, 8, 8); // Clear previous sprite
                            x += 2; // Update position
                            y += 2;
                            saveBackground(x, y); // Save background at new position
                            drawSprite(x, y, smiley, 8, 8, YELLOW); // Draw new sprite
                            sleep_ms(100); // Delay for a short period
                        }
                    } else if (strncmp(command, "/MOVE_SPRITE ", 13) == 0) {
                        int start_x, start_y, end_x, end_y, delay_ms;
                        sscanf(command + 13, "%d %d %d %d %d", &start_x, &start_y, &end_x, &end_y, &delay_ms);
                        move_sprite(start_x, start_y, end_x, end_y, delay_ms);
                    } else if (strncmp(command, "/SCROLL_MAP ", 12) == 0) {
                        int scroll_amount, delay_ms;
                        sscanf(command + 12, "%d %d", &scroll_amount, &delay_ms);
                        scroll_map(scroll_amount, delay_ms);
                    } else if (strncmp(command, "/IMAGE ", 7) == 0) {
                        int x, y, width, height;
                        char image_data[BUFFER_SIZE];
                        sscanf(command + 7, "%d %d %d %d %s", &x, &y, &width, &height, image_data);
                        // Convert image_data string to actual pixel data
                        char image[width * height];
                        for (int i = 0; i < width * height; i++) {
                            image[i] = image_data[i] - '0'; // Convert char to int
                        }
                        drawImage(x, y, width, height, image);
                    } else if (strncmp(command, "/PETSCII ", 9) == 0) {
                        int x, y;
                        char color_code[20];
                        char text[BUFFER_SIZE];
                        sscanf(command + 9, "%d %d %s %[^\n]", &x, &y, color_code, text);
                        drawPETSCIIString(x, y, text, parse_color_code(color_code));
                    } else {
                        uart_puts(uart0, "\nUnknown command.\n");
                    }

                    index = 0; // Reset command index
                }
            } else {
                if (c == '/') {
                    command_mode = true;
                    index = 0;
                    command[index++] = c; // Start with '/'
                } else if (c == '\033') { // ANSI escape sequence start
                    ansi_mode = true;
                    ansi_index = 0;
                } else {
                    uart_putc(uart0, c);
                    update_console(c);

                    if (c == '\r' || c == '\n') {
                        break;
                    }
                }
            }
        }
    }
}

// Function to initialize the tile map
/*void init_tile_map() {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            set_tile(row, col, ' ', current_text_color, current_bg_color, false);
        }
    }
}*/

// Function to set a tile in the tile map
void set_tile(int row, int col, char character, char color, char bgcolor, bool is_sprite) {
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
        tile_map[row][col].character = character;
        tile_map[row][col].color = color;
        tile_map[row][col].bgcolor = bgcolor;
        tile_map[row][col].is_sprite = is_sprite;
    }
}

// Function to get a tile from the tile map
/*Tile get_tile(int row, int col) {
    if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
        return tile_map[row][col];
    } else {
        Tile empty_tile = {' ', current_text_color, current_bg_color, false};
        return empty_tile;
    }
}*/

// Function to render the tile map to the screen
void render_tile_map() {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            Tile tile = tile_map[row][col];
            if (tile.is_sprite) {
                drawSprite(col * CHAR_WIDTH, row * CHAR_HEIGHT, smiley, 8, 8, tile.color);
            } else {
                drawChar(col * CHAR_WIDTH, row * CHAR_HEIGHT, tile.character, tile.color, tile.bgcolor, 1);
            }
        }
    }
}

void init_screen_buffer() {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            screen_buffer[row][col].character = ' ';
            screen_buffer[row][col].color = current_text_color;
            screen_buffer[row][col].bgcolor = current_bg_color;
            screen_buffer[row][col].is_standard_font = true; // Default to standard font
        }
    }
}

/*void populate_tile_map() {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            char character = (col % 2 == 0) ? 'A' : 'B'; // Example pattern
            set_tile(row, col, character, current_text_color, current_bg_color, false);
        }
    }
}*/

void drawImage(int x, int y, int width, int height, const char* image) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            char color = image[i * width + j];
            drawPixel(x + j, y + i, color);
        }
    }
}

// Define a complete PETSCII font (example for characters 'A' to 'Z', 'a' to 'z', numbers, and special characters)
const uint8_t petscii_font[256][8] = {
    // Character 0x20 (' ')
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},
    // Character 0x21 ('!')
    {0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00000000, 0b00000000, 0b00010000, 0b00000000},
    // Character 0x22 ('"')
    {0b01001000, 0b01001000, 0b01001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},
    // Character 0x23 ('#')
    {0b01001000, 0b01001000, 0b11111100, 0b01001000, 0b11111100, 0b01001000, 0b01001000, 0b00000000},
    // Character 0x24 ('$')
    {0b00010000, 0b01111100, 0b10000000, 0b01111100, 0b00000010, 0b01111100, 0b00010000, 0b00000000},
    // Character 0x25 ('%')
    {0b11000000, 0b11000110, 0b00001100, 0b00011000, 0b00110000, 0b01100011, 0b00000011, 0b00000000},
    // Character 0x26 ('&')
    {0b00110000, 0b01001000, 0b00110000, 0b01101100, 0b10010010, 0b10001100, 0b01110010, 0b00000000},
    // Character 0x27 (''')
    {0b00010000, 0b00010000, 0b00010000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},
    // Character 0x28 ('(')
    {0b00001000, 0b00010000, 0b00100000, 0b00100000, 0b00100000, 0b00010000, 0b00001000, 0b00000000},
    // Character 0x29 (')')
    {0b00100000, 0b00010000, 0b00001000, 0b00001000, 0b00001000, 0b00010000, 0b00100000, 0b00000000},
    // Character 0x2A ('*')
    {0b00000000, 0b00101000, 0b00010000, 0b01111100, 0b00010000, 0b00101000, 0b00000000, 0b00000000},
    // Character 0x2B ('+')
    {0b00000000, 0b00010000, 0b00010000, 0b01111100, 0b00010000, 0b00010000, 0b00000000, 0b00000000},
    // Character 0x2C (',')
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010000, 0b00010000, 0b00100000},
    // Character 0x2D ('-')
    {0b00000000, 0b00000000, 0b00000000, 0b01111100, 0b00000000, 0b00000000, 0b00000000, 0b00000000},
    // Character 0x2E ('.')
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010000, 0b00010000, 0b00000000},
    // Character 0x2F ('/')
    {0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b10000000, 0b00000000},
    // Character 0x30 ('0')
    {0b00111000, 0b01000100, 0b01001100, 0b01010100, 0b01100100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x31 ('1')
    {0b00010000, 0b00110000, 0b01010000, 0b00010000, 0b00010000, 0b00010000, 0b01111100, 0b00000000},
    // Character 0x32 ('2')
    {0b00111000, 0b01000100, 0b00000100, 0b00001000, 0b00110000, 0b01000000, 0b01111100, 0b00000000},
    // Character 0x33 ('3')
    {0b00111000, 0b01000100, 0b00000100, 0b00011000, 0b00000100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x34 ('4')
    {0b00001000, 0b00011000, 0b00101000, 0b01001000, 0b01111100, 0b00001000, 0b00001000, 0b00000000},
    // Character 0x35 ('5')
    {0b01111100, 0b01000000, 0b01111000, 0b00000100, 0b00000100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x36 ('6')
    {0b00111000, 0b01000000, 0b01111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x37 ('7')
    {0b01111100, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b00100000, 0b00100000, 0b00000000},
    // Character 0x38 ('8')
    {0b00111000, 0b01000100, 0b01000100, 0b00111000, 0b01000100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x39 ('9')
    {0b00111000, 0b01000100, 0b01000100, 0b00111100, 0b00000100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x3A (':')
    {0b00000000, 0b00010000, 0b00010000, 0b00000000, 0b00000000, 0b00010000, 0b00010000, 0b00000000},
    // Character 0x3B (';')
    {0b00000000, 0b00010000, 0b00010000, 0b00000000, 0b00000000, 0b00010000, 0b00010000, 0b00100000},
    // Character 0x3C ('<')
    {0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000000},
    // Character 0x3D ('=')
    {0b00000000, 0b00000000, 0b01111100, 0b00000000, 0b01111100, 0b00000000, 0b00000000, 0b00000000},
    // Character 0x3E ('>')
    {0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b00000000},
    // Character 0x3F ('?')
    {0b00111000, 0b01000100, 0b00000100, 0b00001000, 0b00010000, 0b00000000, 0b00010000, 0b00000000},
    // Character 0x40 ('@')
    {0b00111000, 0b01000100, 0b01011100, 0b01010100, 0b01011100, 0b01000000, 0b00111100, 0b00000000},
    // Character 0x41 ('A')
    {0b00111000, 0b01000100, 0b01000100, 0b01111100, 0b01000100, 0b01000100, 0b01000100, 0b00011100},
    // Character 0x42 ('B')
    {0b01111100, 0b01000100, 0b01000100, 0b01111100, 0b01000100, 0b01000100, 0b01111100, 0b00000000},
    // Character 0x43 ('C')
    {0b00111100, 0b01000010, 0b01000000, 0b01000000, 0b01000000, 0b01000010, 0b00111100, 0b00000000},
    // Character 0x44 ('D')
    {0b01111000, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b01111000, 0b00000000},
    // Character 0x45 ('E')
    {0b01111110, 0b01000000, 0b01000000, 0b01111100, 0b01000000, 0b01000000, 0b01111110, 0b00000000},
    // Character 0x46 ('F')
    {0b01111110, 0b01000000, 0b01000000, 0b01111100, 0b01000000, 0b01000000, 0b01000000, 0b00000000},
    // Character 0x47 ('G')
    {0b00111100, 0b01000010, 0b01000000, 0b01001110, 0b01000010, 0b01000010, 0b00111100, 0b00000000},
    // Character 0x48 ('H')
    {0b01000100, 0b01000100, 0b01000100, 0b01111100, 0b01000100, 0b01000100, 0b01000100, 0b00000000},
    // Character 0x49 ('I')
    {0b00111000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00111000, 0b00000000},
    // Character 0x4A ('J')
    {0b00011110, 0b00000100, 0b00000100, 0b00000100, 0b00000100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x4B ('K')
    {0b01000100, 0b01001000, 0b01010000, 0b01100000, 0b01010000, 0b01001000, 0b01000100, 0b00000000},
    // Character 0x4C ('L')
    {0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01111110, 0b00000000},
    // Character 0x4D ('M')
    {0b01000010, 0b01100110, 0b01011010, 0b01000010, 0b01000010, 0b01000010, 0b01000010, 0b00000000},
    // Character 0x4E ('N')
    {0b01000010, 0b01100010, 0b01010010, 0b01001010, 0b01000110, 0b01000010, 0b01000010, 0b00000000},
    // Character 0x4F ('O')
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x50 ('P')
    {0b01111100, 0b01000100, 0b01000100, 0b01111100, 0b01000000, 0b01000000, 0b01000000, 0b00000000},
    // Character 0x51 ('Q')
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b01010100, 0b01001000, 0b00110100, 0b00000000},
    // Character 0x52 ('R')
    {0b01111100, 0b01000100, 0b01000100, 0b01111100, 0b01010000, 0b01001000, 0b01000100, 0b00000000},
    // Character 0x53 ('S')
    {0b00111100, 0b01000010, 0b01000000, 0b00111000, 0b00000100, 0b01000010, 0b00111100, 0b00000000},
    // Character 0x54 ('T')
    {0b01111110, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00000000},
    // Character 0x55 ('U')
    {0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x56 ('V')
    {0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b00101000, 0b00010000, 0b00000000},
    // Character 0x57 ('W')
    {0b01000010, 0b01000010, 0b01000010, 0b01000010, 0b01011010, 0b01100110, 0b01000010, 0b00000000},
    // Character 0x58 ('X')
    {0b01000010, 0b01000010, 0b00100100, 0b00011000, 0b00100100, 0b01000010, 0b01000010, 0b00000000},
    // Character 0x59 ('Y')
    {0b01000100, 0b01000100, 0b01000100, 0b00111000, 0b00010000, 0b00010000, 0b00010000, 0b00000000},
    // Character 0x5A ('Z')
    {0b01111110, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01111110, 0b00000000},
    // Character 0x5B ('[')
    {0b00111000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00100000, 0b00111000, 0b00000000},
    // Character 0x5C ('\\')
    {0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000000},
    // Character 0x5D (']')
    {0b00111000, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00111000, 0b00000000},
    // Character 0x5E ('^')
    {0b00010000, 0b00101000, 0b01000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},
    // Character 0x5F ('_')
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01111110, 0b00000000},
    // Character 0x60 ('`')
    {0b00010000, 0b00001000, 0b00000100, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000},
    // Character 0x61 ('a')
    {0b00000000, 0b00000000, 0b00111000, 0b00000100, 0b00111100, 0b01000100, 0b00111100, 0b00000000},
    // Character 0x62 ('b')
    {0b01000000, 0b01000000, 0b01111000, 0b01000100, 0b01000100, 0b01000100, 0b01111000, 0b00000000},
    // Character 0x63 ('c')
    {0b00000000, 0b00000000, 0b00111100, 0b01000000, 0b01000000, 0b01000000, 0b00111100, 0b00000000},
    // Character 0x64 ('d')
    {0b00000100, 0b00000100, 0b00111100, 0b01000100, 0b01000100, 0b01000100, 0b00111100, 0b00000000},
    // Character 0x65 ('e')
    {0b00000000, 0b00000000, 0b00111000, 0b01000100, 0b01111100, 0b01000000, 0b00111100, 0b00000000},
    // Character 0x66 ('f')
    {0b00001100, 0b00010000, 0b00111100, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00000000},
    // Character 0x67 ('g')
    {0b00000000, 0b00000000, 0b00111100, 0b01000100, 0b01000100, 0b00111100, 0b00000100, 0b00111000},
    // Character 0x68 ('h')
    {0b01000000, 0b01000000, 0b01111000, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b00000000},
    // Character 0x69 ('i')
    {0b00010000, 0b00000000, 0b00110000, 0b00010000, 0b00010000, 0b00010000, 0b00111000, 0b00000000},
    // Character 0x6A ('j')
    {0b00001000, 0b00000000, 0b00011000, 0b00001000, 0b00001000, 0b00001000, 0b01001000, 0b00110000},
    // Character 0x6B ('k')
    {0b01000000, 0b01000000, 0b01001000, 0b01010000, 0b01100000, 0b01010000, 0b01001000, 0b00000000},
    // Character 0x6C ('l')
    {0b00110000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00010000, 0b00111000, 0b00000000},
    // Character 0x6D ('m')
    {0b00000000, 0b00000000, 0b01100100, 0b01011010, 0b01000010, 0b01000010, 0b01000010, 0b00000000},
    // Character 0x6E ('n')
    {0b00000000, 0b00000000, 0b01111000, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b00000000},
    // Character 0x6F ('o')
    {0b00000000, 0b00000000, 0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000, 0b00000000},
    // Character 0x70 ('p')
    {0b00000000, 0b00000000, 0b01111000, 0b01000100, 0b01000100, 0b01111000, 0b01000000, 0b01000000},
    // Character 0x71 ('q')
    {0b00000000, 0b00000000, 0b00111100, 0b01000100, 0b01000100, 0b00111100, 0b00000100, 0b00000100},
    // Character 0x72 ('r')
    {0b00000000, 0b00000000, 0b01011100, 0b01100000, 0b01000000, 0b01000000, 0b01000000, 0b00000000},
    // Character 0x73 ('s')
    {0b00000000, 0b00000000, 0b00111100, 0b01000000, 0b00111000, 0b00000100, 0b01111000, 0b00000000},
    // Character 0x74 ('t')
    {0b00010000, 0b00010000, 0b00111100, 0b00010000, 0b00010000, 0b00010000, 0b00001100, 0b00000000},
    // Character 0x75 ('u')
    {0b00000000, 0b00000000, 0b01000100, 0b01000100, 0b01000100, 0b01000100, 0b00111100, 0b00000000},
    // Character 0x76 ('v')
    {0b00000000, 0b00000000, 0b01000100, 0b01000100, 0b01000100, 0b00101000, 0b00010000, 0b00000000},
    // Character 0x77 ('w')
    {0b00000000, 0b00000000, 0b01000010, 0b01000010, 0b01000010, 0b01011010, 0b00100100, 0b00000000},
    // Character 0x78 ('x')
    {0b00000000, 0b00000000, 0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100, 0b00000000},
    // Character 0x79 ('y')
    {0b00000000, 0b00000000, 0b01000100, 0b01000100, 0b01000100, 0b00111100, 0b00000100, 0b00111000},
    // Character 0x7A ('z')
    {0b00000000, 0b00000000, 0b01111100, 0b00001000, 0b00010000, 0b00100000, 0b01111100, 0b00000000},
    // ...define other characters as needed...
};

// Function to draw a PETSCII character with the top facing the top of the screen
void drawPETSCIIChar(int x, int y, uint8_t c, char color) {
    if (c < 0x20 || c > 0x7E) { // Allow characters from ' ' (0x20) to '~' (0x7E)
        printf("Character code out of range: %c (0x%02X)\n", c, c);
        return;
    }
    uint8_t index = c - 0x20; // Map characters from 0x20 to 0x7E
    for (int i = 0; i < 8; i++) {
        uint8_t line = petscii_font[index][i]; // Correctly map the character
        for (int j = 0; j < 8; j++) {
            if (line & (1 << (7 - j))) {
                drawPixel(x + j, y + i, color);
            } else {
                drawPixel(x + j, y + i, current_bg_color); // Clear the background
            }
        }
    }
}

int main() {
    init_uart();
    initVGA();
    init_console();
    init_screen_buffer(); // Initialize the screen buffer
    //init_tile_map(); // Initialize the tile map

    // Populate the tile map with some characters
    //populate_tile_map();

    // Render the initial tile map
    render_tile_map();

    // Test the scroll_map function with different scroll amounts and delay values
    scroll_map(10, 100); // Scroll the map to the left by 10 columns with a delay of 100ms

    // Print all PETSCII characters as a test
    int x = 0;
    int y = 100;
    for (uint8_t c = 0x20; c <= 0x7E; c++) {
        drawPETSCIIChar(x, y, c, YELLOW);
        x += 8;
        if (x >= SCREEN_WIDTH) {
            x = 0;
            y += 8;
        }
    }

    while (1) {
        handle_serial_input();
    }

    return 0;
}

// Functions being used from vga16_graphics.c
extern void fillCircleHelper(short x0, short y0, short r, unsigned char cornername, short delta, char color);
extern void drawRoundRect(short x, short y, short w, short h, short r, char color);
extern void fillRoundRect(short x, short y, short w, short h, short r, char color);
extern void fillRect(short x, short y, short w, short h, char color);
extern void drawChar(short x, short y, unsigned char c, char color, char bg, unsigned char size);
extern void setCursor(short x, short y);
extern void setTextSize(unsigned char s);
extern void setTextColor(char c);
extern void setTextColor2(char c, char b);
extern void setTextWrap(char w);
extern void tft_write(unsigned char c);
extern void writeString(char* str);
extern void setTextColorBig(char color, char background);
extern void drawCharBig(short x, short y, unsigned char c, char color, char bg);
extern void writeStringBig(char* str);
extern void writeStringBold(char* str);

// Functions being used from vga16_graphics.c
extern void drawPixel(short x, short y, char color);
extern void drawVLine(short x, short y, short h, char color);
extern void drawHLine(short x, short y, short w, char color);
extern void drawLine(short x0, short y0, short x1, short y1, char color);
extern void drawRect(short x, short y, short w, short h, char color);
extern void drawCircle(short x0, short y0, short r, char color);
extern void drawCircleHelper(short x0, short y0, short r, unsigned char cornername, char color);
extern void fillCircle(short x0, short y0, short r, char color);
extern void fillCircleHelper(short x0, short y0, short r, unsigned char cornername, short delta, char color);
extern void drawRoundRect(short x, short y, short w, short h, short r, char color);
extern void fillRoundRect(short x, short y, short w, short h, short r, char color);
extern void fillRect(short x, short y, short w, short h, char color);
extern void drawChar(short x, short y, unsigned char c, char color, char bg, unsigned char size);
extern void setCursor(short x, short y);
extern void setTextSize(unsigned char s);
extern void setTextColor(char c);
extern void setTextColor2(char c, char b);
extern void setTextWrap(char w);
extern void tft_write(unsigned char c);
extern void writeString(char* str);
extern void setTextColorBig(char color, char background);
extern void setTextColor(char c);
extern void drawCharBig(short x, short y, unsigned char c, char color, char bg);
extern void writeStringBig(char* str);
extern void writeStringBold(char* str);
extern void setCursor(short x, short y);
extern void setTextSize(unsigned char s);
extern void setTextColor(char c);
extern void setTextColor2(char c, char b);
extern void setTextWrap(char w);
extern void tft_write(unsigned char c);
extern void writeString(char* str);
extern void setTextColorBig(char color, char background);
extern void drawCharBig(short x, short y, unsigned char c, char color, char bg);
extern void writeStringBig(char* str);
extern void writeStringBold(char* str);
extern void drawCharBig(short x, short y, unsigned char c, char color, char bg);
extern void writeStringBold(char* str);
extern void setTextColor2(char c, char b);
extern void setTextWrap(char w);
extern void tft_write(unsigned char c);
extern void writeString(char* str);
extern void setTextColorBig(char color, char background);
extern void drawCharBig(short x, short y, unsigned char c, char color, char bg);
extern void writeStringBig(char* str);
extern void writeStringBold(char* str);
extern void drawCharBig(short x, short y, unsigned char c, char color, char bg);
extern void writeStringBig(char* str);
extern void writeStringBold(char* str);
extern void writeStringBig(char* str);
extern void writeStringBold(char* str);
extern void setTextColor2(char c, char b);
extern void setTextWrap(char w);
extern void tft_write(unsigned char c);
extern void writeString(char* str);
extern void setTextColorBig(char color, char background);
extern void drawCharBig(short x, short y, unsigned char c, char color, char bg);
extern void writeStringBig(char* str);
extern void writeStringBold(char* str);