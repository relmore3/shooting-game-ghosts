/*___________________________________________________________________
|
|	File: version.h
|
| (C) Copyright 2013 Abonvita Software LLC.
| Licensed under the GX Toolkit License, Version 1.0.
|___________________________________________________________________*/

#define APPLICATION_NAME          "GX Toolkit App"
#define APPLICATION_VERSION       "1.0"
#define DIRECTX_VERSION_REQUIRED  0x90000
#define DIRECTX_VERSION_STR       "9.0"

#define USING_DIRECTX_9

#ifdef USING_DIRECTX_9
  #define EVENT_DRIVER    evDRIVER_DX9
  #define MOUSE_DRIVER    msDRIVER_DX9
  #define GRAPHICS_DRIVER gxDRIVER_DX9
#endif
#ifdef USING_DIRECTX_8
  #define EVENT_DRIVER    evDRIVER_DX8
  #define MOUSE_DRIVER    msDRIVER_DX8
  #define GRAPHICS_DRIVER gxDRIVER_DX8
#endif
