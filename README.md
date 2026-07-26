# NeBasicUNO
A very very basic basic for the Arduino Uno

**Upload**
Open `Code.c` and upload it to the Arduino using Arduino IDE or similer

## Commands
| Command | Syntax | Description |
|---|---|---|
| `PRINT` / `?` | `PRINT <expr>` | Prints a variable (`A`-`Z`) or literal integer. |
| `LET` | `LET <A-Z> = <expr>` | Assigns an integer or variable value to a target variable. |
| `VARS` | `VARS` | Displays all non-zero assigned variables. |
| `CLEAR` | `CLEAR` | Resets all variables (`A`-`Z`) to `0`. |
| `HIGH` | `HIGH <pin>` | Sets a digital pin mode to `OUTPUT` and drives it `HIGH`. |
| `LOW` | `LOW <pin>` | Sets a digital pin mode to `OUTPUT` and drives it `LOW`. |
| `TOGGLE` | `TOGGLE <pin>` | Inverts the current digital state of a pin. |
| `READ` | `READ <pin>` | Configures a pin to `INPUT_PULLUP` and prints state (`0` or `1`). |
| `AREAD` | `AREAD <pin>` | Reads and returns the raw ADC value from an analog pin. |
| `AWRITE` | `AWRITE <pin> <val>` | Emits PWM signal (`0`-`255`) on a supported pin. |
| `TONE` | `TONE <pin> <freq>` | Generates a square wave of specified frequency (Hz) on a pin. |
| `NOTONE` | `NOTONE <pin>` | Stops frequency generation on a pin. |
| `DELAY` | `DELAY <ms>` | Pauses execution for the specified milliseconds. |
| `RAND` | `RAND <var> <max>` | Stores a random value from `0` to `max - 1` into target variable. |
| `PEEK` | `PEEK <addr>` | Reads and displays the byte stored at a raw memory address. |
| `POKE` | `POKE <addr> <val>` | Writes a byte directly to a raw memory address. |
| `FREE` | `FREE` | Outputs remaining available SRAM in bytes. |
| `HELP` | `HELP` | Prints quick command syntax summary. |

**If you create a PR or Issue, I'm probably not going to see so I recommend just making a fork**
