---------------------------------
Model Viewer for DirectX Tool Kit
---------------------------------

Copyright (c) Microsoft Corporation. All rights reserved.

November 12, 2015

------------------------------------------------------------------------------------------
BUILD INSTRUCTIONS

***PC***

Run VS 2013 or 2015

Open Project/Solution... "DirectXTKModelViewer_Desktop_201x.sln"

Ensure "DirectXTKModelViewer_Desktop_201x" is the "StartUp Project"

Build and Run (F5)


***Xbox One***

Open "Xbox One XDK Command Prompt"

cd to DirectXTK\Src\Shaders

run "CompileShaders.cmd xbox"

Run VS 2015

Open Project/Solution... "DirectXTKModelViewer_XDK_2015.sln"

Ensure "DirectXTKModelViewer_XDK_2015" is the "StartUp Project"

Build and Run (F5) or use Deploy Solution



------------------------------------------------------------------------------------------
USAGE

***PC***

If a GamePad is plugged into the PC it can be used to control the viewer (see below for details).
If you press the "View" button, a Open File Dialog is used to select the model (.SDKMESH,
.CMO, or .VBO) to load.

If no GamePad is plugged in, you can use Keyboard & Mouse controls. If you press the "O" key,
a Open File Dialog is used to select the model (.SDKMESH, .CMO, or .VBO) to load.

MOUSE:
   Press and hold LEFT mouse button to rotate view (SHIFT+LEFT button rotates object instead)

   Press and hold RIGHT mouse button to translate view in XY (SHIFT+RIGHT translates in Z)

   Scroll wheel controls zoom (i.e. distance between camera and focus point)

KEYBOARD:

   W/S and PageUp/PageDown translates in Z
   A/D and Left/Right translates in X
   Up/Down translates in Y

   Q/E glance left/right
   Space glance up
   X glance down

   C cycles background color
   G toggles the grid display
   H toggles HUD display
   J toggles the cross display
   R toggles wireframe

   O loads model          

   Home key resets camera to default position

   End key resets model to default rotation

   +/- scales the grid size

***Xbox One***

For Xbox One, only the GamePad controls are available. When you presse the "View" button, the tool enumerates
the SDKMESH, CMO, and VBO files in the root of the D:\ drive and allows you to select one of them to load.

GAMEPAD:

    A - Change modes Orbit vs. FPS
    B - Toggles wireframe
    X - Cycles grid, viewpoint cross, and HUD display
    Y - Cycle background color

    Right Trigger – Zoom In
    Left Trigger – Zoom Out

    DPAD
        U – Translate Up
        D – Translate Down
        R – Translate Right
        L – Translate Left

    Right Bumper – Increment translate sensitivity
    Left Bumper – Decrement translate sensitivity

    Left Thumbstick button - Frame scene extents

    Orbit
        Right Thumbstick
              R – Orbit Right
              L – Orbit Left
              U – Orbit Up
              D – Orbit Down
              Click – Reset View to Default

        Left Thumbstick
              R – Rotate (Roll) Right
              L – Rotate (Roll) Left
              U – Move Towards Origin
              D – Move Away from Origin

    FPS
        Right Thumbstick
              R – Look Right
              L – Look Left
              U – Look Up
              D – Look Down

        Left Thumbstick
              R – Translate Right
              L – Translate Left
              U – Translate Forward
              D – Translate Back

NOTE: For Xbox One, after you start up the viewer app, you can use the Xbox One XDK command prompt to copy data files to
the system:

  xbcp /x/title <source directory> xd:\
