# PicoRetroDisplay

---

## RetroPico

RetroPico is a graphics library for retro computers using the Raspberry Pi Pico. It provides functions for drawing shapes, text, and sprites on a VGA display. The library supports PETSCII characters and includes features such as scrolling, image rendering, and ANSI escape code handling.

### Features

- **Text Rendering**: Supports standard 5x7 and BRL4 fonts.
- **Shape Drawing**: Functions for drawing lines, rectangles, circles, and rounded rectangles.
- **Sprite Handling**: Save, restore, and move sprites on the screen.
- **Scrolling**: Scroll the screen and tile map with customizable delay.
- **ANSI Escape Codes**: Handle ANSI escape codes for cursor positioning and screen clearing.
- **UART Communication**: Initialize and handle UART communication for serial input.

### Getting Started

1. **Initialize the VGA Display**:

   ```c
   initVGA();
   ```

2. **Draw Shapes**:

   ```c
   drawLine(10, 10, 100, 100, RED);
   drawRect(50, 50, 100, 100, GREEN);
   fillCircle(200, 200, 50, YELLOW);
   ```

3. **Render Text**:

   ```c
   setCursor(0, 0);
   setTextColor(WHITE);
   writeString("Hello, RetroPico!");
   ```

4. **Handle Serial Input**:

   ```c
   while (1) {
       handle_serial_input();
   }
   ```

### Command Reference

- **Change Text Color**:

  ```plaintext
  /TEXT [color_code]
  Example: /TEXT R   (Changes text color to Red)
  ```

- **Change Background Color**:

  ```plaintext
  /BACK [color_code]
  Example: /BACK B   (Changes background color to Dark Blue)
  ```

- **Clear Screen**:

  ```plaintext
  /CLS
  Example: /CLS
  ```

- **Draw Line**:

  ```plaintext
  /LINE x1 y1 x2 y2 color
  Example: /LINE 10 10 100 100 R   (Draws a red line from coordinates (10,10) to (100,100))
  ```

- **Draw Rectangle**:

  ```plaintext
  /RECT x y width height color
  Example: /RECT 50 50 100 100 G   (Draws a green rectangle with top-left corner at (50,50) and size 100x100)
  ```

- **Fill Rectangle**:

  ```plaintext
  /FILLRECT x y width height color
  Example: /FILLRECT 50 50 100 100 B   (Draws and fills a dark blue rectangle with top-left corner at (50,50) and size 100x100)
  ```

- **Draw Circle**:

  ```plaintext
  /CIRCLE x y radius color
  Example: /CIRCLE 200 200 50 Y   (Draws a yellow circle with center at (200,200) and radius 50)
  ```

- **Fill Circle**:

  ```plaintext
  /FILLCIRCLE x y radius color
  Example: /FILLCIRCLE 200 200 50 O   (Draws and fills a dark orange circle with center at (200,200) and radius 50)
  ```

- **Draw Rounded Rectangle**:

  ```plaintext
  /ROUNDRECT x y width height radius color
  Example: /ROUNDRECT 50 50 100 100 10 G   (Draws a green rounded rectangle with top-left corner at (50,50), size 100x100, and radius 10)
  ```

- **Fill Rounded Rectangle**:

  ```plaintext
  /FILLROUNDRECT x y width height radius color
  Example: /FILLROUNDRECT 50 50 100 100 10 B   (Draws and fills a dark blue rounded rectangle with top-left corner at (50,50), size 100x100, and radius 10)
  ```

- **Set Font**:

  ```plaintext

  /FONT [type]
  Example: /FONT STANDARD   (Sets the font to Standard 5x7)
  Example: /FONT BRL4   (Sets the font to BRL4)
  ```

- **Draw Smiley**:

  ```plaintext
  /SMILEY
  Example: /SMILEY
  ```

- **Move Sprite**:

  ```plaintext
  /MOVE_SPRITE start_x start_y end_x end_y delay_ms
  Example: /MOVE_SPRITE 50 50 200 200 100   (Moves the sprite from (50,50) to (200,200) with a delay of 100ms)
  ```

- **Scroll Map**:

  ```plaintext
  /SCROLL_MAP scroll_amount delay_ms
  Example: /SCROLL_MAP 10 100   (Scrolls the map to the left by 10 columns with a delay of 100ms)
  ```

- **Display Image**:

  ```plaintext
  /IMAGE x y width height image_data
  Example: /IMAGE 10 10 100 100 image_data   (Displays an image at coordinates (10,10) with size 100x100)
  ```

### ANSI Escape Codes

- **Cursor Position**:

  ```plaintext
  \033[row;colH
  Example: \033[10;20H   (Moves the cursor to row 10, column 20)
  ```

- **Clear Screen**:

  ```plaintext
  \033[2J
  Example: \033[2J   (Clears the entire screen)
  ```

- **Set Text Attributes**:

  ```plaintext
  \033[attribute_code m
  Example: \033[31m   (Sets the text color to red)
  ```

### Text Color Codes

- 30 - Black
- 31 - Red
- 32 - Dark Green
- 33 - Yellow
- 34 - Dark Blue
- 35 - Magenta
- 36 - Cyan
- 37 - White

### Color Codes

- R - Red
- G - Dark Green
- M - Medium Green
- C - Cyan
- Y - Yellow
- K - Black
- B - Dark Blue
- O - Dark Orange
- D - Magenta
- P - Light Pink
- r - Red
- g - Green
- b - Blue
- o - Orange
- c - Light Blue
- m - Pink
- k - White

### License

This project is licensed under the MIT License.

---
