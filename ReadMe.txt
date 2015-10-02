---------------------------------
Model Viewer for DirectX Tool Kit
---------------------------------

Copyright (c) Microsoft Corporation. All rights reserved.

October 1, 2015

------------------------------------------------------------------------------------------
BUILD INSTRUCTIONS

***PC***

Run VS 2012

Open Project/Solution... "DirectXTKModelViewer_Desktop_2013.sln"

Ensure "DirectXTKModelViewer_Desktop_2013" is the "StartUp Project"

Build and Run (F5)


***Xbox One***

Open "Xbox One XDK Command Prompt"

cd to DirectXTK\Src\Shaders

run "CompiledShaders.cmd xbox"

Run VS 2015

Open Project/Solution... "DirectXTKModelViewer_XDK_2015.sln"

Ensure "DirectXTKModelViewer_XDK_2015" is the "StartUp Project"

Build and Run (F5)



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

   Press and hold RIGHT mouse button to translate view

   Scroll wheel controls zoom (i.e. distance between camera and focus point)

KEYBOARD:
   Up, Down, Right, Left, PageUp, and PageDown translate the view

   Home key resets camera to default position

   End key resets model to default rotation

   G toggles the grid display

   +/- scales the grid size

   W toggles wireframe

   H toggles HUD display

   B cycles background color

   O loads model          


***Xbox One***

For Xbox One, only the GamePad controls are available. When you presse the "View" button, the tool enumerates
the SDKMESH, CMO, and VBO files in the root of the D:\ drive and allows you to select one of them to load.

GAMEPAD:
   Left thumb stick controls translation of view: X/Y by deafult, or Z if left stick button is depressed

   Right thumb stick controls rotation of camera

   Right/left trigger controls 'zoom' (i.e. distance between camera and focus point)

   DPAD Up cycle backgroundc olor

   DPAD Down toggles wireframe

   DPAD Left toggles the grid display

   DPAD Right toggles the HUD display

   Y resets camera/model to default position

NOTE: For Xbox One, after you start up the viewer app, you can use the Xbox One XDK command prompt to copy data files to
the system:

  xpcp /x/title <source directory> xd:\
