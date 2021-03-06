# KlonTUIke (Solitaire)

KlonTUIke is a Klondike (solitaire) version to be played in a terminal.

## Compile

### Linux

To compile KlonTUIke on Linux you need cmake and ncurses (or even better
ncursesw for Unicode card suits).

In the project's root type:
```
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
```
You can now play KlonTUIke by running ktui in the build directory.

Tested with gcc 4.84, cmake 2.8.12.2, make 3.8 and ncursesw 5.9 on LinuxMint 17.

## Play

To play KlonTUIke you need a terminal with at least 27 columns, 21 lines and
color support.

KlonTUIke has the same
[rules](https://en.wikipedia.org/wiki/Klondike_(solitaire)#Rules) as any
Klondike. This version turns one reserved card at a time.

Hit `n` to start a new game (ending the current game), `q` to quit the
application.

Move the cursor using the arrow keys and use `enter` to select or place cards or
turning the reserve stack. To append selected cards to a tableau it is not
required to actually have the cursor on the last card of said tableau.

You can also use your mouse for selecting and playing cards or turning the
reserve stack. But bear in mind that mouse support is very rudimentary and
KlonTUIke is still best played with keyboard.

## Todo

* Undo
* Tests
