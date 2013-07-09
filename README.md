ISO
===

Isometric (Sort-Of) Engine

Requires [SFML 2.0 Library - 32 bit](http://www.sfml-dev.org/download/sfml/2.0/)

Installation
====

1. Follow the tutorial for Visual Studio [here](http://www.sfml-dev.org/tutorials/2.0/start-vc.php).  Do not use the static library methods.
2. Include the "sfml-main.lib" in the linker libraries as well.
3. Once you are able to build and run it, copy the assets folder to your build and release folders.
4. Go to Project Settings > Configuration Properties > Debugging and set the working directory to "$(TargetDir)"