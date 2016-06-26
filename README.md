# KlonTUIke (Solitaire)

*KlonTUIke* is a Klondike (solitaire) version to be played in a terminal.

## Compile

### Linux

To compile *KlonTUIke* on Linux you need *cmake* and *ncurses*, for the Unicode
version (that has card symbols instead of ASCII initials) you need *ncursesw*.

In the project's root type:
```
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
```
You can now play *KlonTUIke* by running *ktui* in the *build* directory.

To compile Unicode *KlonTUIke* type in additionally or alternatively
`make ktui_uni` which will create *ktui_uni* in the *build* directory.

Tested with *gcc 4.84*, *cmake 2.8.12.2*, *make 3.8*, *ncurses 5.9*,
*ncursesw 5.9*.

## Play

*KlonTUIke* has the same
[rules](https://en.wikipedia.org/wiki/Klondike_(solitaire)#Rules) as any
Klondike. This version turns one reserved card at a time.

Hit `n` to start a new game (ending the current game), `q` to quit the
application.

Move the cursor using the arrow keys and use `enter` to select or place cards or
turning the reserve stack. To append selected cards to a tableau it is not
required to actually have the cursor on the last card of said tableau.

Mouse controls are planned for future releases. As well as an undo function.

## Todo

* Mouse controls
* Undo
* Tests
