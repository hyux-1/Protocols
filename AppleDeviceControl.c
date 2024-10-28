/*++

 Created by HermitCrabs on 09/2/2014.
 Copyright 2014-2015 The HermitCrab Labs. All rights reserved.

Module Name:

  AppleDeviceControl.c

Abstract:

Revision History

  1.0 Initial Version

--*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AppleDeviceControl.h>

#include "Macros.h"

EFI_GUID gAppleDeviceControlProtocolGuid = APPLE_DEVICE_CONTROL_PROTOCOL_GUID;

/*
 *
 */

EFI_STATUS
EFIAPI
AppleDeviceControlUnknown01 (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_INFO, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

/*
 *
 */

EFI_STATUS
EFIAPI
AppleDeviceControlConnectDisplay (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_INFO, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

/*
 *
 */

EFI_STATUS
EFIAPI
AppleDeviceControlUnknown03 (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_INFO, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

/*
 *
 */

EFI_STATUS
EFIAPI
AppleDeviceControlConnectAll (
  VOID
  )
{
  DEBUG ((DEBUG_INFO, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_INFO, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

/*
 *
 */

APPLE_DEVICE_CONTROL_PROTOCOL mAppleDeviceControlProtocol = {
  0,
  AppleDeviceControlConnectDisplay,
  0,
  AppleDeviceControlConnectAll
};