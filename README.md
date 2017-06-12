
Spherical Chess is an implementation of "chess on the dot" utilizing C++14 and OpenGL 3.3.
Chess on the dot is a variation of the classical game of chess. The game board is a sphere instead of a flat square surface.
This adds a dimension to the game, allowing pieces to move across what would otherwise be the end of the board.

Building
========

Currently requires Visual Studio 2015, a free version is available for download [here](https://www.visualstudio.com/downloads/).
Ensure the submodule [spherical-chess-libs](https://github.com/skl131313/spherical-chess-libs) has been cloned into the working directory.
This submodule contains the external library binary files this project relies on, and is needed to compile. You can ensure the submodule
included when cloning this repository using the following git command:

    git clone --recursive https://github.com/skl131313/spherical-chess

Open the project file "sphericalchess.sln" with Visual Studio and then proceed to compile and run the program.

Todo
====

Currently the game is functioning and a game of chess can be played but some work still needs to be done.
Missing are some functionality such as menus which still need to be implemented.
Possibly some netcode to allow for online multiplayer, currently only a local game between players is allowed.
Rendering also needs work, allow for animated backgrounds to make the scene more interesting.
