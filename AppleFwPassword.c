/*++

 Created by Jasmin Fazlic on 4/14/10.
 Copyright 2010-2015 The HermitCrab Labs. All rights reserved.

Module Name:

  AppleFwPassword.c

Abstract:

Revision History

  1.0 Initial Version

--*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AppleFwPassword.h>

#include "Macros.h"

EFI_GUID gAppleFirmwarePasswordProtocolGuid = APPLE_FIRMWARE_PASSWORD_PROTOCOL_GUID;

/*
 *
 */

EFI_STATUS
EFIAPI
AppleFwPasswordCheck (
  IN  APPLE_FIRMWARE_PASSWORD_PROTOCOL    *This,
  IN  OUT UINTN                           *Arg1
  )
{
  DEBUG ((DEBUG_INFO, "%a: Started\n", __FUNCTION_NAME__));
  *Arg1 = FALSE;
  DEBUG ((DEBUG_INFO, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

/*
 *
 */

APPLE_FIRMWARE_PASSWORD_PROTOCOL mAppleFirmwarePasswordProtocol = {
  0,
  0,
  0,
  0,
  AppleFwPasswordCheck
};