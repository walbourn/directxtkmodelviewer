![DirectX Logo](https://github.com/Microsoft/DirectXTK/wiki/X_jpg.jpg)

# Model Viewer for DirectX Tool Kit for DirectX 11

Copyright (c) Microsoft Corporation. All rights reserved.

**February 29, 2019**

The DirectX Tool Kit Model Viewer is an interactive test application for validating ``.SDKMESH``, ``.VBO``, and ``.CMO`` files rendered using the DirectX Tool Kit.

This code is designed to build with Visual Studio 2017 ([15.9](https://walbourn.github.io/vs-2017-15-9-update/)).

## Notices

All content and source code for this package are subject to the terms of the [MIT License](http://opensource.org/licenses/MIT).

For the latest version of the Model Viewer for DirectX Tool Kit, bug reports and feature requests,
please visit the project site on [GitHub](http://aka.ms/directxtkmodelviewer).

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.


## Build Instructions
------------------

### PC

Run VS 2017

Open Project/Solution... "DirectXTKModelViewer_Desktop_201?.sln"

Ensure "DirectXTKModelViewer_Desktop_201?" is the "StartUp Project"

Build and Run (F5)


### Xbox One

Run VS 2017

Open Project/Solution... "DirectXTKModelViewer_XDK_201?.sln"

Ensure "DirectXTKModelViewer_XDK_201?" is the "StartUp Project"

Build and Run (F5) or use Deploy Solution

> On supported platforms, you can use the -render4K command-line option.

## USAGE

### PC

If a Xbox 360 or Xbox One gamepad controller is plugged into the PC it can be used to
control the viewer (see below for details). If you press the "View" button, an Open File
Dialog is used to select the model (.SDKMESH, .CMO, or .VBO) to load.

If no controller is plugged in, you can use keyboard & mouse controls. If you press the "O" key,
an Open File Dialog is used to select the model (.SDKMESH, .CMO, or .VBO) to load.

MOUSE:

   Press and hold LEFT mouse button to rotate view (SHIFT+LEFT button rotates object instead)

   Press and hold RIGHT mouse button to translate view in XY (SHIFT+RIGHT translates in Z)

   Scroll wheel controls zoom (i.e. distance between camera and focus point)

KEYBOARD:

    W/S and PageUp/PageDown translates in Z
    A/D and Left/Right translates in X
    Up/Down translates in Y
    Q/E rotate left/right

    B toggles culling mode
    C cycles background color
    G toggles the grid display
    H toggles HUD display
    J toggles the cross display
    R toggles wireframe
    T cycles tone-mapping operator

    [/] scales the FOV
    +/- scales the grid size

    O loads model          

    Enter/Backspace cycles Image-Based Lighting for PBR models

    Home key resets camera to default position

    End key resets model to default rotation

### Xbox One

For Xbox One, only the gamepad controls are available. When you press the "View" button, the tool enumerates
the .SDKMESH, .CMO, and .VBO files in the root of the D:\ drive and allows you to select one of them to load.

GAMEPAD:

    A - Change modes Orbit vs. FPS
    B - Toggles wireframe/culling mode
    X - Cycles grid, viewpoint cross, and HUD display
    Y - Cycle background color
    Menu - Cycle tone-mapping operator

    DPAD
        U - Translate Up
        D - Translate Down
        R - Translate Right
        L - Translate Left

    Right Trigger - Increment FOV
    Left Trigger - Decrement FOV

    Right Bumper - Increment translate sensitivity
    Left Bumper - Decrement translate sensitivity

    Left Thumbstick button - Frame scene extents

    Orbit
        Right Thumbstick
              R - Orbit Right
              L - Orbit Left
              U - Orbit Up
              D - Orbit Down
              Click - Reset View to Default

        Left Thumbstick
              R - Rotate (Roll) Right
              L - Rotate (Roll) Left
              U - Move towards view point
              D - Move away from view point

    FPS
        Right Thumbstick
              R - Rotate Right
              L - Rotate Left
              U - Rotate Up
              D - Rotate Down

        Left Thumbstick
              R - Translate Right
              L - Translate Left
              U - Translate Forward
              D - Translate Back

> For Xbox One, after you start up the viewer app, you can use the Xbox One XDK command prompt to copy data files to
the system:

```
xbcp /x/title <source directory> xd:\
```
