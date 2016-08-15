# Installation notes
Here's some information about how to get Fugio to compile from the Github develop branch.

## OSX
Note: tested on Max OS X 10.10.  

- Update [Qt](https://www.qt.io/download-open-source/) to v5.7 (does not work on v5.5)

- Meet the [Fugio OSX Requirements](http://wiki.bigfug.com/Fugio_OS_X_Requirements)  
 `brew install fftw`  
 `brew install portaudio`  
 `brew install portmidi`  

- Follow [Lua installation notes](http://wiki.bigfug.com/Lua)  
 `brew install lua`  

- Fix ImageIO.dylib if getting linker errors with libTIFF, libJPEG and libPNG:  
_(careful, this might break other stuff)_  
`ln -s /System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/libJPEG.dylib /usr/local/lib/libJPEG.dylib`  
`ln -s /System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/libTIFF.dylib /usr/local/lib/libTIFF.dylib`  
`ln -s /System/Library/Frameworks/ImageIO.framework/Versions/A/Resources/libPng.dylib /usr/local/lib/libPNG.dylib`  

## Linux
(Not yet)

## RaspberryPi
(Not yet)

## Windows
(Not yet)