![DirectX Logo](https://raw.githubusercontent.com/wiki/Microsoft/DirectXTK/X_jpg.jpg)

# Model Viewer for DirectX Tool Kit for DirectX 11

Copyright (c) Microsoft Corporation.

**March 22, 2022**

The DirectX Tool Kit Model Viewer is an interactive test application for validating ``.SDKMESH``, ``.VBO``, and ``.CMO`` files rendered using the DirectX Tool Kit.

This code is designed to build with Visual Studio 2017 ([15.9](https://walbourn.github.io/vs-2017-15-9-update/)) or VS 2019 (16.4 or later).

## Notices

All content and source code for this package are subject to the terms of the [MIT License](https://github.com/walbourn/directxtkmodelviewer/blob/main/LICENSE).

For the latest version of the Model Viewer for DirectX Tool Kit, bug reports and feature requests,
please visit the project site on [GitHub](http://aka.ms/directxtkmodelviewer).

## Build Instructions

### PC

Run VS 2019

Open Project/Solution... ``DirectXTKModelViewer_Desktop_2019.sln``

Ensure ``DirectXTKModelViewer_Desktop_2019`` is the "StartUp Project"

Build and Run (F5)

### Xbox One

Run VS 2017

Open Project/Solution... ``DirectXTKModelViewer_XDK_2017.sln``

Ensure ``DirectXTKModelViewer_XDK_2017`` is the "StartUp Project"

Build and Run (F5) or use Deploy Solution

> On supported platforms, you can use the -render4K command-line option.

## Usage

### PC

If a Xbox 360 or Xbox One gamepad controller is plugged into the PC it can be used to control the viewer (see below for details). If you press the "View" button, an Open File
Dialog is used to select the model (.SDKMESH, .CMO, or .VBO) to load.

If no controller is plugged in, you can use keyboard & mouse controls. If you press the "O" key, an Open File Dialog is used to select the model (.SDKMESH, .CMO, or .VBO) to load.

#### Mouse

* Press and hold LEFT mouse button to rotate view (SHIFT+LEFT button rotates object instead)

* Press and hold RIGHT mouse button to translate view in XY (SHIFT+RIGHT translates in Z)

* Scroll wheel controls zoom (i.e. distance between camera and focus point)

#### Keyboard

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
    L toggles lighting vs. unlit (BasicEffect only)
    T cycles tone-mapping operator
    N cycles bone rendering mode (World vs. Rigid/Skinnned)

    [/] scales the FOV
    +/- scales the grid size

    O loads model          

    Enter/Backspace cycles Image-Based Lighting for PBR models

    Home key resets camera to default position

    End key resets model to default rotation

### Xbox One

For Xbox One, only the gamepad controls are available. When you press the "View" button, the tool enumerates the .SDKMESH, .CMO, and .VBO files in the root of the D:\ drive and allows you to select one of them to load.

#### GamePad

    A - Change modes Orbit vs. FPS
    B - Toggles wireframe/culling/lighting mode
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
              Click - cycles bone rendering mode

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
              Click - cycles bone rendering mode

For Xbox One, after you start up the viewer app, you can use the Xbox One XDK command prompt to copy data files to
the system:

```
xbcp /x/title <source directory> xd:\
```

## Contributing

This project welcomes contributions and suggestions. Most contributions require you to agree to a Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## Trademarks

This project may contain trademarks or logos for projects, products, or services. Authorized use of Microsoft trademarks or logos is subject to and must follow [Microsoft's Trademark & Brand Guidelines](https://www.microsoft.com/en-us/legal/intellectualproperty/trademarks/usage/general). Use of Microsoft trademarks or logos in modified versions of this project must not cause confusion or imply Microsoft sponsorship. Any use of third-party trademarks or logos are subject to those third-party's policies.

