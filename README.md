"# PicoRetroDisplay" 
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

### License

This project is licensed under the BSD-3-Clause License.

---

Feel free to customize this description further to better fit your project's specifics and goals.