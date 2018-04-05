ISO
===

Isometric (Sort-Of) Engine

Requires [SFML 2.0 Library - 32 bit](http://www.sfml-dev.org/download/sfml/2.0/)

Installation
===

1. Follow the tutorial for Visual Studio [here](http://www.sfml-dev.org/tutorials/2.0/start-vc.php).  Do not use the static library methods.
2. Include the "sfml-main.lib" in the linker libraries as well.
3. Once you are able to build, copy all required SFML dlls and the "assets" folder to your build and release folders.
4. Go to Project Settings > Configuration Properties > Debugging and set the working directory to "$(TargetDir)"
5. Run from VS should now work correctly and assets should load.

Project Goals
===

1. Create an efficient and flexible C++ object-oriented game engine.
2. Compile without any warnings or issues. 
3. Utilize the "job" paradigm to effectively utilize system resources.
4. Create a fully-function map editor GUI which can load, edit, and export maps.
5. Implement a simple rigid-body physics engine for entities.
6. Prototype common game elements as C++ based objects and implement general methods for use.
7. Implement a visibility graph and A* search algorithm for entities to traverse the environment if needed.
8. Create a user interface template which makes prototyping simple and fast.
