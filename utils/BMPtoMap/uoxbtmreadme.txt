November 21, 2000

Initial program release

====================
Program Requirements
====================

Up to date common dialog control (ver 6, pref SP4)

================
Current features
================

* Takes a 768x512 BMP and converts to a UO map
* Customizable tile translation support
  * Can add new tile translation
  * Can modify existing tile translation
    * Use spin controls for tiles and stuff
    * Click on coloured boxes to select colour from common dialog
    * Can modify target altitude via spin control
* Can paint in the main window
  * Colour is the currently selected tile translation on the left (lo colour if trabskatuib)
  * Brush sizes from menu on top
* Makes map0.mul in the directory it was executed from, therefore, best not run it in your main UO directory
* Load tile translation files
  * Extension of .btm
* Save tile translation
  * Saves out to trans.btm
* Possible to save out current pic to a file (savedpic.bmp)

===================
Future enhancements
===================

Add support for removing tile translation
Flood fill tool
Map->Bmp
Zoom in facility 

==========
Known bugs
==========

None