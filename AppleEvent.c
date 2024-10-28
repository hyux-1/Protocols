/*++

 Created by HermitCrabs on 27/5/2014.
 Copyright 2014-2015 The HermitCrab Labs. All rights reserved.

Module Name:

  AppleEvent.c

Abstract:

Revision History

  1.1 Revise
  1.0 Initial Version

--*/

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AppleEvent.h>

#include "Macros.h"

EFI_GUID gAppleEventProtocolGuid = APPLE_EVENT_PROTOCOL_GUID;
/*
 *
 */

EFI_STATUS
EFIAPI
AppleEventRegisterHandler (
  IN  UINT32             EventType,
  IN  VOID               *NotifyFunction,
  OUT APPLE_EVENT_HANDLE **EventHandle,
  IN  VOID               *NotifyContext
  )
{
  return EFI_SUCCESS;
}

/*
 *
 */

EFI_STATUS
EFIAPI
AppleEventUnregisterHandler (
  IN APPLE_EVENT_HANDLE *EventHandle
  )
{
  return EFI_SUCCESS;
}

/*
 *
 */

EFI_STATUS
EFIAPI
AppleEventSetCursorPosition (
  IN APPLE_DIMENSION    *Position
  )
{
  return EFI_SUCCESS;
}

/*
 *
 */

EFI_STATUS
EFIAPI
AppleEventSetEventName (
  IN OUT APPLE_EVENT_HANDLE *EventHandle,
  IN     CHAR8              *EventName
  )
{
  return EFI_SUCCESS;
}

/*
 *
 */

EFI_STATUS
EFIAPI
AppleEventIsCapsLockActive (
  IN OUT BOOLEAN *CapsLockAcvtive
  )
{
  return EFI_SUCCESS;
}

/*
 *
 */

APPLE_EVENT_PROTOCOL mAppleEventProtocol = {
  1,
  AppleEventRegisterHandler,
  AppleEventUnregisterHandler,
  AppleEventSetCursorPosition,
  AppleEventSetEventName,
  AppleEventIsCapsLockActive
};
