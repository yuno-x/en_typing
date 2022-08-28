# en\_typing
This TUI (Text User Interface) program is a tool to train typing English from Japanese (or another language).
You can learn translation simultaneously.

## depending package
libncurses (If you install this package on debian (Based) distribution, it may be alright that you input a command "sudo apt install libncurses").

## build and run
You can build this program with following command.

    $ make

Then, the program can be run with following command.

    $ ./en_typing

If you want to clear all generated object, input following command.

    $ make clean

## word list
This program use word lists in directory "word\_lists".

If you see word list format, please refer to example word lists.
