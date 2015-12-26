# VisualLiveSystem
Visual Live System is a new visual jockey software for professional live performance.
Compatible with Mac and PC, he is able to render complex real time 3D scene fully based on the sound.
He have an intuitive interface thinked like a DJ solution, 2 channels of real time effects and a master pannel for mixing theses.
Midi controllers is supported.


<img src="http://www.aduprat.com/pub/vls.jpg">

More details soon.

## Setup:

Dependencies:
* [qt 5](http://www.qt.io/download/)
* [bass](http://www.un4seen.com/)
* [rtaudio](https://www.music.mcgill.ca/~gary/rtaudio/)
* [portmidi](http://portmedia.sourceforge.net/portmidi/)
* [glew](http://glew.sourceforge.net/)



Compile:
 * Clone this repo with '''git clone --recursive https://github.com/XT95/VisualLiveSystem.git''' this will download somes depancies automatically
 * Download glew (currently using glew-1.13.0 shall be updated to the lastest version) paste the glew-1.13.0 folder into the depends folder. 
 * Copy the correct libs (32 or 64 bits) into the "depends/lib" directory (ex: glew32.lib rtaudio_static.lib and portmidi_s.lib for Visual Studio)
 * Open /VisualLiveSystem/VisualLiveSystem.pro with Qt Creator, setup the binary output to /release and compil it.
 * You can do the same thing with /SceneEditor.


## Usage:

Visual Live System
 * If you have a midi controller just go to the midi controller view, make a new profil and assign yours faders like of you want.
 * After that, open the visual manager select some few scenes and drag'n'drop of the channels A and B. Apply.
 * Go to the post processing tabulation, and add some post fx (the order change the result). Apply.
 * Get the output audio of your DJ on input of your computer (if you just want to make a test, the microphone should be correct).
 * Now, open the projector's window, move them to the projector screen and double clik for the fullscreen mode.
 * Enjoy by making beautiful transition between the channels when you have a drop :) !
