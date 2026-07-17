## Personal C Coding Style Conventions (Josue Boisvert)

Code element |           Convention           | Example
--- |:------------------------------:| ---
Defines |            ALL_CAPS            | `#define PLATFORM_DESKTOP`
Macros |            ALL_CAPS            | `#define MIN(a,b) (((a)<(b))?(a):(b))`
Variables |           snake_case           | `int screen_width = 0;`, `float target_frame_time = 0.016f;`
Local variables |           snake_case           | `Vector2 player_position = { 0 };`
Global variables |           snake_case           | `bool fullscreen = false;`
Constants |           snake_case           | `const int max_value = 8;`
Pointers |        MyType *pointer         | `Texture2D *array = NULL;`
float values |          always x.xf           | `float gravity = 10.0f`
Operators |         value1*value2          | `int product = value*6;`
Operators |         value1/value2          | `int division = value/4;`
Operators |        value1 + value2         | `int sum = value + 10;`
Operators |        value1 - value2         | `int res = value - 5;`
Enum |           TitleCase            | `enum TextureFormat`
Enum members |            ALL_CAPS            | `PIXELFORMAT_UNCOMPRESSED_R8G8B8`
Struct |           TitleCase            | `struct Texture2D`, `struct Material`
Struct typedef |           TitleCase            | `typedef struct Texture { ... } Texture;`
Struct members |           snake_case           | `texture.width`, `color.r`
Functions |           snake_case           | `process_inputs()`, `load_player_data()`
Functions params |           snake_case           | `width`, `height`
Ternary Operator | (condition)? result1 : result2 | `printf("Value is 0: %s", (value == 0)? "yes" : "no");`

Other conventions:
 - All defined variables are ALWAYS initialized
 - Four spaces are used, instead of TABS
 - Trailing spaces are always avoided
 - Control flow statements are followed **by a space**
 - Single line flow have statements enclosed in brackets
 - variables names with number don't have underscore around the numbers: **value1foo** and not ~~value_1_foo~~
```c
if (condition) { value = 0; }

while (!WindowShouldClose()) {

}

for (int i = 0; i < NUM_VALUES; i++) { printf("%i", i); }

switch (value) {
    case 0:
        break;
    case 2:
        break;
    default:
        break;
}
```
 - All conditions are always between parenthesis, but not boolean values:
```c
if ((value > 1) && (value < 50) && value_active) {

}

if ( value > 1 ) {} \\ except for single condition
```
 - Braces and curly brackets open on the same line and close on a new line:
```c
void SomeFunction() {
   // TODO: Do something here!
   
   funciton_with_lots_of_parameters(
       value1,
       value2,
       //...
   );  
}
```
 - Consider using parameter structs instead if a function needs a large number of parameters:
```c
struct FuncParam {
    int value1;
    int value2;
    //... more than 6 or 7 parameters
}

FuncParam params = {
    //...
}
some_function(params);
// some_functions(value1, value2, ... );
```

## Files and Directories Naming Conventions

  - Directories are named using `snake_case`: `resources/models`, `resources/fonts`
  - Files are named using `snake_case`: `main_title.png`, `cubicmap.png`, `sound.wav`

_NOTE: Spaces and special characters are always avoided in the files/dir naming!_

## Games/Examples Directories Organization Conventions

 - Resource files are organized by context and usage in the game. Loading requirements for data are also considered (grouping data when required).
 - Descriptive names are used for the files, just reading the name of the file it should be possible to know what is that file and where fits in the game.

```
resources/audio/fx/long_jump.wav
resources/audio/music/main_theme.ogg
resources/screens/logo/logo.png
resources/screens/title/title.png
resources/screens/gameplay/background.png
resources/characters/player.png
resources/characters/enemy_slime.png
resources/common/font_arial.ttf
resources/common/gui.png
```
_NOTE: Some resources require to be loaded all at once while other require to be loaded only at initialization (gui, font)._
