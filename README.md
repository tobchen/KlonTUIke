# KlonTUIke (Solitaire)

KlonTUIke is a Klondike (solitaire) to be played in a terminal.

## Compile

The root directory contains a simple Makefile (requiring gcc to be the C
compiler and ncurses or even ncursesw to be installed).

`make standard` or just `make` compiles standard KlonTUIke which uses strictly
ASCII characters. `make unicode` compiles a unicode KlonTUIke that makes use of
unicode card suit symbols but needs ncursesw to be installed and needs a unicode
terminal to be played. The binarys are then found in the bin-directory with
klontuike being standard KlonTUIke and klontuike_unicode being the unicode
version.

## Play

KlonTUIke has the same
[rules](https://en.wikipedia.org/wiki/Klondike_(solitaire)#Rules) as any
Klondike. This version turns one reserved card at a time.

Hit `n` to start a new game (ending the current game), `q` to quit the
application.

Move the cursor using the arrow keys and use enter to select or place cards or
turning the reserve stack. To append selected cards to a tableau it's not
required to actually have the cursor on the last card of said tableau.

Mouse controls are planned for future releases. As well as an undo function.

## Todo

* Mouse controls
* Undo
* Tests
