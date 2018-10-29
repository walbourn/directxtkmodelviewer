------------------------------------------------
Model Viewer for DirectX Tool Kit for DirectX 11
------------------------------------------------

Copyright (c) Microsoft Corporation. All rights reserved.

October 29, 2018

The DirectX Tool Kit Model Viewer is an interactive test application for
validating .SDKMESH, .VBO, and .CMO files rendered using the DirectX Tool Kit.

The source is written for Visual Studio 2015 or 2017. It is recommended you use
VS 2015 Update 3 with the Windows 10 Anniversary SDK (14393) or VS 2017 (15.7 update) or later
and the Windows 10 April 2018 Update SDK (17134).

All content and source code for this package are subject to the terms of the MIT License.
<http://opensource.org/licenses/MIT>.

For the latest version of the Model Viewer for DirectX Tool Kit, bug reports and feature requests, 
please visit the GitHub project.

http://aka.ms/directxtkmodelviewer

This project has adopted the Microsoft Open Source Code of Conduct. For more information see the
Code of Conduct FAQ or contact opencode@microsoft.com with any additional questions or comments.

https://opensource.microsoft.com/codeofconduct/


------------------
BUILD INSTRUCTIONS
------------------

***PC***

Run VS 2015 or 2017

Open Project/Solution... "DirectXTKModelViewer_Desktop_201?.sln"

Ensure "DirectXTKModelViewer_Desktop_201?" is the "StartUp Project"

Build and Run (F5)


***Xbox One***

Run VS 2015 or 2017

Open Project/Solution... "DirectXTKModelViewer_XDK_201?.sln"

Ensure "DirectXTKModelViewer_XDK_201?" is the "StartUp Project"

Build and Run (F5) or use Deploy Solution

NOTE: On supported platforms, you can use the -render4K command-line option.


------------------
USAGE
------------------

***PC***

If a Xbox 360 or Xbox One gamepad controller is plugged into the PC it can be used to
control the viewer (see below for details). If you press the "View" button, a Open File
Dialog is used to select the model (.SDKMESH, .CMO, or .VBO) to load.

If no controller is plugged in, you can use keyboard & mouse controls. If you press the "O" key,
a Open File Dialog is used to select the model (.SDKMESH, .CMO, or .VBO) to load.

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

   Home key resets camera to default position

   End key resets model to default rotation

***Xbox One***

For Xbox One, only the gamepad controls are available. When you presse the "View" button, the tool enumerates
the .SDKMESH, .CMO, and .VBO files in the root of the D:\ drive and allows you to select one of them to load.

GAMEPAD:

    A - Change modes Orbit vs. FPS
    B - Toggles wireframe/culling mode
    X - Cycles grid, viewpoint cross, and HUD display
    Y - Cycle background color
    Menu - Cycle tone-mapping operator

    DPAD
        U – Translate Up
        D – Translate Down
        R – Translate Right
        L – Translate Left

    Right Trigger - Increment FOV
    Left Trigger – Decrement FOV

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
              U – Move towards view point
              D – Move away from view point

    FPS
        Right Thumbstick
              R – Rotate Right
              L – Rotate Left
              U – Rotate Up
              D – Rotate Down

        Left Thumbstick
              R – Translate Right
              L – Translate Left
              U – Translate Forward
              D – Translate Back

NOTE: For Xbox One, after you start up the viewer app, you can use the Xbox One XDK command prompt to copy data files to
the system:

    xbcp /x/title <source directory> xd:\


-------------
RELEASE NOTES
-------------

* The VS 2017 projects make use of /permissive- for improved C++ standard conformance. Use of a Windows 10 SDK prior to
  the Fall Creators Update (16299) or an Xbox One XDK prior to June 2017 QFE 4 may result in failures due to problems
  with the system headers. You can work around these by disabling this switch in the project files which is found
  in the <ConformanceMode> elements.

* The VS 2017 projects require the 15.5 update or later. For Win32 classic desktop projects with the 15.5
  or 15.6 updates, you need to install the standalone Windows 10 SDK (17134) which is otherwise included in
  the 15.7/15.8 update. Older VS 2017 updates will fail to load the projects due to use of the <ConformanceMode> element.


--------------- 
RELEASE HISTORY
---------------

October 29, 2018
    Updated for October 25, 2018 release of DirectX Tool Kit
    Supports loading SDKMESH files using legacy DEC3N compressed normals with an approximation

August 20, 2018
    Updated for August 17, 2018 release of DirectX Tool Kit
    Cleanup project files

May 31, 2018
    Updated for May 31, 2018 release of DirectX Tool Kit
    VS 2017 updated for Windows 10 April 2018 Update SDK (17134)

April 24, 2018
    Updated for April 23, 2018 release of DirectX Tool Kit
    Code cleanup

November 2, 2017
    Updated for November 1, 2017 release of DirectX Tool Kit
    VS 2017 updated for Windows 10 Fall Creators Update SDK (16299)
    ALT+ENTER shortcut added for fullscreen toggle

July 20, 2017
    Added HDR10 display output support
    Retired VS 2013 projects

June 23, 2017
    Added tone-mapping
    Use larger fonts on high-resolution displays
    Updated for the June 21, 2017 release of DirectX Tool Kit

April 28, 2017
    Added VS 2017 projects
    Updated for the April 24, 2017 release of DirectX Tool Kit

November 9, 2016
    Updated for the October 6, 2016 release of DirectX Tool Kit

September 29, 2016
    Updated for the September 15, 2016 release of DirectX Tool Kit
    Major code cleanup

November 13, 2015
    Updated control scheme, improved HUD, and additional toggles

October 9, 2015
    VS 2015 desktop projects

October 1, 2015
    Initial version
