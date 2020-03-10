![tesc](https://user-images.githubusercontent.com/54913619/76170457-1f36e700-6193-11ea-9159-cef56f111b55.png)

![version](https://img.shields.io/badge/version-1.1-brightgreen)
[![license](https://img.shields.io/badge/license-MIT-blue)](LICENSE.md)
![cpp-version](https://img.shields.io/badge/C%2B%2B-≥17-blue)

**TesC** (**T**ext **es**cape **C**olorizer) is a tiny C++17 library to decorate console output with [ANSI codes](https://en.wikipedia.org/wiki/ANSI_escape_code)-based ostream manipulators

## Installation
1. [Download](https://github.com/qzminsky/tesc/archive/v1.1.0.zip) the library source;
2. `#include` the `tesc.h` file in your C++ project;
3. Enjoy!

> ⚠️ *Note that the library requires C++17 support*

## Usage
Output decoration is carried out using manipulators. The library provides three of them:

* `tesc::color` — to change current fore-/background colors;
* `tesc::font` — to change current font style settings;
* `tesc::reset` — to revert all settings to its default values.

### Colorizing the output
To change output text colors (applies to the following output only) use the `tesc::color` manipulator with corresponding parameters like this:

```C++
// For pretty usage
using namespace tesc;

std::cout << color{ bright(face::white) | back::red } << "Error badge";
```

Several parameters for a manipulator are joining with each other via operator `|`. Regarding colors, you can join background (enum `back`) and foreground (enum `face`) colors only. If you didn't specify one of them, the corresponding color stays the same as before.

Enumerations `tesc::back` and `tesc::face` contains the following color codes:

| Name      | FG Code | BG code |
|-----------|---------|---------|
| `black`   | 30      | 40      |
| `red`     | 31      | 41      |
| `green`   | 32      | 42      |
| `yellow`  | 33      | 43      |
| `blue`    | 34      | 44      |
| `magenta` | 35      | 45      |
| `cyan`    | 36      | 46      |
| `white`   | 37      | 47      |

Brighter colors can be obtained using the `tesc::bright(...)` function (see example above). It adds 60 to the code of a given color. For example, `bright(back::blue)` returns the color with code `104`.

### Font stylization
TesC may be used to set bold, italic or/and underlined text style. Use the `tesc::font` manipulator with the following `tesc::style` settings:

| Name        | ANSI Code     | Enum values |
|-------------|---------------|-------------|
| `normal`    | 22; 23; 24    | 0           |
| `bold`      | **1**; 23; 24 | 1           |
| `italic`    | **3**; 22; 24 | 2           |
| `underline` | **4**; 22; 23 | 4           |

As for `tesc::color`, these parameters are joinable as well. Note that if you haven't used some of settings, they will be automatically cancelled. For example:

```C++
// The best way is to use it locally (in function body)
using namespace tesc;

std::cout << font{ style::bold | style::underline } << "Important!"
          << font{ style::italic } << "Italic only";
```

### Settings reset
Use the `tesc::reset` manipulator (like `std::endl`) to revert all color and font style settings to its default values:

```C++
using namespace tesc;

std::cout << color{ face::black | bright(back::white) } << font{ style::bold } << "Stylized text"
          << reset << "Ordinary style";
//           ^^^^^
//           Just put it like this
```

### Saving settings
Since applying decoration stores as `static`, they can be saved for future. Use these static member functions to get their components:

* Foreground text color — `tesc::color::face()`;
* Background text color — `tesc::color::back()`;
* Current font style — `tesc::font::get_style()`.

## License
See the [LICENSE](LICENSE.md) file for license rights and limitations (MIT).
