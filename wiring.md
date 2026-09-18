# Wiring

Suggested wire colours: red for 5V, black or blue for GND.

## Arduino to everything else

| Arduino pin | Connects to |
|---|---|
| 5V | breadboard + rail |
| GND | breadboard − rail |
| D2 | LCD pin 14 (D7) |
| D3 | LCD pin 13 (D6) |
| D4 | LCD pin 12 (D5) |
| D5 | LCD pin 11 (D4) |
| D7 | Button A |
| D8 | Button B |
| D11 | LCD pin 6 (E) |
| D12 | LCD pin 4 (RS) |
| D13 | 220 Ω → LED long leg |

The buttons need no resistors: the sketch uses the internal pull-ups, so each button's other leg goes straight to the − rail. Use two diagonally opposite legs so the button isn't permanently closed.

## LCD1602

| LCD pin | Name | Connects to |
|---|---|---|
| 1 | VSS | − rail |
| 2 | VDD | + rail |
| 3 | V0 | potentiometer middle leg |
| 4 | RS | D12 |
| 5 | RW | − rail |
| 6 | E | D11 |
| 7–10 | D0–D3 | nothing |
| 11 | D4 | D5 |
| 12 | D5 | D4 |
| 13 | D6 | D3 |
| 14 | D7 | D2 |
| 15 | A | 220 Ω → + rail |
| 16 | K | − rail |

The potentiometer's outer legs go to the + and − rails. It sets the contrast; without it the screen shows either nothing or a row of solid blocks.

## Building it in Tinkercad Circuits

Tinkercad has no import format for circuits, so the layout has to be dragged together by hand.

1. Open Circuits and create a new circuit. Drag in: Arduino Uno R3, small breadboard, LCD 16x2, potentiometer, 2 pushbuttons, LED, 2 resistors set to 220 Ω.
2. As far as I know Tinkercad's programmable boards are the Uno and the ATtiny, so use the Uno. The sketch runs the same on it.
3. Drop the LCD onto the breadboard so its 16 pins each land in their own column, then wire the two tables above. Hovering over a pin shows its name.
4. Put each pushbutton across the breadboard's center gap and wire two diagonally opposite legs.
5. Open Code, switch to Text mode, and paste `morse_decoder/morse_decoder.ino` over the default sketch. `LiquidCrystal` is available in the simulator.
6. Start Simulation, then click and hold the buttons to type.

Notes:
- Adjust contrast by clicking the potentiometer and dragging its knob until the characters appear.
- The simulator runs slower than real hardware, so long holds are painful. Lower `BACKSPACE_MS` to about 800 while testing there, and put it back before uploading to the real board.
