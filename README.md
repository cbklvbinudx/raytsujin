# raytsujin
Taiko No Tatsujin clone that reads .osu files

Made with [raylib](https://github.com/raysan5/raylib) and [raygui](https://github.com/raysan5/raygui)

To begin playing drag a file with a .osu extension onto the window and press the button in the middle. You can go back to the menu by pressing ESC and you can quit the game by pressing ESC twice.

![Screenshot](https://i.imgur.com/TZltVtU.jpg)

# Default keybindings
Katsu (Blue): D, K

Don (Red): F, J

Retry: `

# Building
Make sure to enable SUPPORT_FILEFORMAT_JPG in your build options.

Example in CMake:
```
set(CUSTOMIZE_BUILD On)
set(SUPPORT_FILEFORMAT_JPG On)
```
