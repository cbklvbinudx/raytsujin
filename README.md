# raytsujin
Taiko No Tatsujin clone that reads .osu files

Made with [raylib](https://github.com/raysan5/raylib) and [raygui](https://github.com/raysan5/raygui)

To begin playing drag a file with a .osu extension onto the window and press the button in the middle. You can go back to the menu by pressing ESC and you can quit the game by pressing ESC twice.

![Screenshot](https://i.imgur.com/mnX4HzJ.png)

# Default keybindings
Katsu (Blue): **D, K**

Don (Red): **F, J**

Retry: **`**

Toggle fullscreen: **F**

Raise volume: **Up arrow**

Lower volume: **Down arrow**

# Building
Make sure to rebuild your raylib to support .jpg files.

It can be done by uncommenting 
```
#define SUPPORT_FILEFORMAT_JPG      1
```
in *../raylib/src/config.h* and rebuilding.
