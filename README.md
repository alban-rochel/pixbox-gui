pixbox-gui
=========

This is the code for the GUI of the Pixbox project: http://pixbox-project.blogspot.fr/

Disclaimer
------------

This piece of code was not written in the idea of being distributed, it is provided as-is for the entertainment of whoever could want to try it. It is not properly commented either.

Lots of things are hard-coded, but should be easy to adapt.

Requirements:
* SDL libraries: SDL, SDL-image, SDL-ttf, SDL-mixer
* External libraries (SDL 2) should be in /media/BBB/lib
	* Otherwise, adapt build.sh
* SDL includes should be in /media/BBB/include/SDL
  * Otherwise, adapt build.sh
* Resources (pixmaps, sounds, fonts) should be in /media/BBB/pixbox/resources/.
  * Otherwise, adapt Content.h

* My own resources are provided in the resources folder.
  * Graphical resources
  * Sample CSV "database"
* Some resources I don't have the rights for should be added by yourself:
  * Game artworks in an "art" subdirectory of the resources folder: png files, 300 pixels max dimension
  * smb_bump.wav, smb_coin.wav sound effects (just use your favorite search engine)
  * PressStart2P.ttf font
  
Licence
--------

You can do whatever you please with this piece of code. I would just be happy if you could post a comment on the Pixbox project if you end up using it or deriving it!

How to build
-------

Just run build.sh - make sure it is executable beforehand.