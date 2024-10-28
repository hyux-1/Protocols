/*++

 Created by HermitCrabs on 09/5/2014.
 Copyright 2014-2015 The HermitCrab Labs. All rights reserved.

Module Name:

  AppleNetBoot.c

Abstract:

Revision History

  1.0 Initial Version

--*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AppleNetBoot.h>

#include "Macros.h"

EFI_GUID gAppleNetBootProtocolGuid = APPLE_NET_BOOT_PROTOCOL_GUID;

/*
 *
 */

EFI_STATUS
EFIAPI
AppleNetBootGetDhcpResponse (
  IN  APPLE_NET_BOOT_PROTOCOL           *This,
  IN  UINTN                             *BufferSize,
  IN  VOID                              *DataBuffer
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
AppleNetBootGetBsdpResponse (
  IN  APPLE_NET_BOOT_PROTOCOL           *This,
  IN  UINTN                             *BufferSize,
  IN  VOID                              *DataBuffer
  )
{
  DEBUG ((DEBUG_INFO, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_INFO, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

/*
 *
 */

APPLE_NET_BOOT_PROTOCOL mAppleNetBootProtocol = {
  AppleNetBootGetDhcpResponse,
  AppleNetBootGetBsdpResponse,
};