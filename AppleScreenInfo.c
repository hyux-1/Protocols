/*++

 Created by HermitCrabs on 4/13/10.
 Copyright 2010-2015 The HermitCrab Labs. All rights reserved.

Module Name:

  AppleScreenInfo.c

Abstract:

Revision History

  1.0 Initial Version

--*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/AppleScreenInfo.h>

#include "Macros.h"

EFI_GUID gAppleScreenInfoProtocolGuid = APPLE_SCREEN_INFO_PROTOCOL_GUID;

EFI_STATUS
EFIAPI
GetScreenInfo (
  IN  APPLE_SCREEN_INFO_PROTOCOL    *This,
  OUT UINT64                        *BaseAddress,
  OUT UINT64                        *FrameBufferSize,
  OUT UINT32                        *BytesPerRow,
  OUT UINT32                        *Width,
  OUT UINT32                        *Height,
  OUT UINT32                        *ColorDepth
  )
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL      *GraphicsOutput;
  EFI_STATUS                        Status;

  DEBUG ((DEBUG_VERBOSE, "%a: Started\n", __FUNCTION_NAME__));

  Status = gBS->LocateProtocol (&gEfiGraphicsOutputProtocolGuid,
                                NULL,
                                (VOID **)&GraphicsOutput);

  if (!EFI_ERROR(Status)) {

    *FrameBufferSize  = (UINT64)GraphicsOutput->Mode->FrameBufferSize;
    *BaseAddress      = (UINT64)GraphicsOutput->Mode->FrameBufferBase;
    *Width            = (UINT32)GraphicsOutput->Mode->Info->HorizontalResolution;
    *Height           = (UINT32)GraphicsOutput->Mode->Info->VerticalResolution;
    *ColorDepth       = 32;
    *BytesPerRow      = (UINT32)(GraphicsOutput->Mode->Info->PixelsPerScanLine * 4);

    DEBUG ((DEBUG_INFO, "GetScreenInfo %d x %d x %d Base 0x%0X Size 0x%0X\n",
                        *Width,
                        *Height,
                        *ColorDepth,
                        *BaseAddress,
                        *FrameBufferSize));

  }

  DEBUG ((DEBUG_VERBOSE, "%a: Finished\n", __FUNCTION_NAME__));

  return Status;
}

APPLE_SCREEN_INFO_PROTOCOL mAppleScreenInfoProtocol = {
  GetScreenInfo
};
