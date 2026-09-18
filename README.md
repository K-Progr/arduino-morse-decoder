# Two-button Morse decoder

A Morse code text entry device for an Arduino Uno or Nano. You type with **two buttons only** and the decoded text appears on a 16x2 LCD wired on the same breadboard.

- **Button A (key)** — tap for a dot, hold for a dash. An LED shows which one you're about to get: it stays lit for a dot and goes dark once the press counts as a dash.
- **Button B (next letter)** — tap to add the letter, tap with nothing typed for a space, hold to backspace.
- **Both buttons together** — clear everything.

Line 1 of the LCD shows your text and scrolls after 15 characters. Line 2 shows the dots and dashes you're typing plus a live preview of the letter they will become, for example `.-          =A`.

## Hardware

| Part | Qty |
|---|---|
| Arduino Uno or Nano | 1 |
| LCD1602, 16-pin (no I2C backpack) | 1 |
| 10 kΩ potentiometer (LCD contrast) | 1 |
| Tactile push button | 2 |
| LED, 5 mm | 1 |
| 220 Ω resistor (LED, LCD backlight) | 2 |
| Breadboard and jumper wires | — |


## Repository layout

```
morse_decoder/morse_decoder.ino    the decoder (upload this)
tests/test_screen/                 step 1: does the LCD work?
tests/test_buttons/                step 2: do the buttons and LED work?
hardware/wiring.md                 pin map and a Tinkercad build recipe
```

Each sketch sits in a folder with the same name, which the Arduino IDE requires.

## Upload

1. Install the Arduino IDE and select your board and port.
2. Open `morse_decoder/morse_decoder.ino`.
3. Click Upload. `LiquidCrystal` ships with the IDE, so there is nothing to install.

If the screen shows a row of solid blocks, the contrast is off or a data wire is wrong. Check RS and E first; they are the pair most often swapped.

## How it decodes

Instead of a 36-entry lookup table, the sketch walks a binary tree stored as one string. It starts at index 1, a dot moves to `index * 2` and a dash to `index * 2 + 1`, and the character sits at the index you land on. Five symbols cover A–Z and 0–9; a `*` in the string means that pattern is not a character.

## Tuning

The timing values are at the top of the sketch.

| Setting | What it controls |
|---|---|
| `DASH_MS` | Shortest press that counts as a dash |
| `DEBOUNCE_MS` | Ignores the flicker when a button's contacts close |
| `BACKSPACE_MS` | How long to hold B before it deletes |
| `REPEAT_MS` | Time between repeated deletes |

## Status and next steps

The decoder works as a standalone device. Planned next: a client–server mode that sends each committed letter to a program on a computer over USB serial.

## License

GPL-3.0-or-later. See [LICENSE](LICENSE).
