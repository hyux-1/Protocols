/*++

Copyright (c) 2004 - 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  ConsoleControl.c

Abstract:

  Abstraction of a Text mode or GOP/UGA screen

--*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PicoPrintLib.h>

#include <Protocol/ConsoleControl.h>


EFI_GUID gEfiConsoleControlProtocolGuid = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;

EFI_CONSOLE_CONTROL_SCREEN_MODE CurrentConsoleMode = EfiConsoleControlScreenGraphics;

EFI_STATUS
EFIAPI
GetMode(
  IN  EFI_CONSOLE_CONTROL_PROTOCOL      *This,
  OUT EFI_CONSOLE_CONTROL_SCREEN_MODE   *Mode,
  OUT BOOLEAN                           *GopUgaExists,  OPTIONAL
  OUT BOOLEAN                           *StdInLocked    OPTIONAL
  )
{
  *Mode = CurrentConsoleMode;

  if (GopUgaExists) {
    *GopUgaExists = TRUE;
  }

  if (StdInLocked) {
    *StdInLocked = FALSE;
  }

  DEBUG ((EFI_D_INFO, "ConsoleControl->GetMode = %a\n",
                      *Mode == EfiConsoleControlScreenText ? "Text" : "Graphics"));

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
SetMode(
  IN  EFI_CONSOLE_CONTROL_PROTOCOL      *This,
  IN  EFI_CONSOLE_CONTROL_SCREEN_MODE   Mode
  )
{
  DEBUG ((EFI_D_INFO, "ConsoleControl->SetMode %a -> %a\n",
                      CurrentConsoleMode == EfiConsoleControlScreenText? "Text" : "Graphics",
                      Mode == EfiConsoleControlScreenText ? "Text" : "Graphics"));

  CurrentConsoleMode = Mode;

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
LockStdIn(
  IN  EFI_CONSOLE_CONTROL_PROTOCOL      *This,
  IN  CHAR16                            *Password
  )
{
  return EFI_SUCCESS;
}

EFI_CONSOLE_CONTROL_PROTOCOL mEfiConsoleControlProtocol = {
  GetMode,
  SetMode,
  LockStdIn
};