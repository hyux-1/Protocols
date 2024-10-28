/*++

Ported by HermitCrabs on 30/06/2015.
Copyright 2015 Apple Inc. All rights reserved.

Module Name:

AppleKeyMapDatabase.c

Abstract:

Reversed from AppleKeyMapAggregator.efi and AppleEvent.efi, which are Apple Inc. property
Decompiled by Download-Fritz

Revision History

1.0 Initial Version

--*/

#include <Uefi.h>

#include <Protocol/AppleKeyMapAggregator.h>
#include <Protocol/AppleKeyMapDatabase.h>

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/PicoMiscLib.h>

#include "AppleKeyMapInternal.h"

//
//
//

// InitializeAppleKeyMapAggregator
///
/// @param[in] ImageHandle      The firmware allocated handle for the EFI image.  
/// @param[in] SystemTable      A pointer to the EFI System Table.
///
/// @retval EFI_SUCCESS         The entry point is executed successfully.
/// @retval EFI_ALREADY_STARTED The protocol has already been installed.
EFI_STATUS
InitializeAppleKeyMapAggregator (
  VOID
  ) // start
{
  EFI_STATUS               Status;

  UINTN                    NumberHandles;
  EFI_HANDLE               *Buffer;
  APPLE_KEY_MAP_AGGREGATOR *Aggregator;

  Status = gBS->LocateHandleBuffer (ByProtocol,
                  					&gAppleKeyMapDatabaseProtocolGuid,
                  					NULL,
                  					&NumberHandles,
                  					&Buffer);

  if (!EFI_ERROR (Status)) {
    Status = EFI_DEVICE_ERROR;

    if (Buffer != NULL) {
      gBS->FreePool ((VOID *)Buffer);
    }
  } else {
    Aggregator                                        = (APPLE_KEY_MAP_AGGREGATOR *)AllocateZeroPool (sizeof (*Aggregator));
    Aggregator->Signature                             = APPLE_KEY_MAP_AGGREGATOR_SIGNATURE;
    Aggregator->NextKeyStrokeIndex                    = 3000;
    Aggregator->DatabaseProtocol.Revision             = APPLE_KEY_MAP_DATABASE_PROTOCOL_REVISION;
    Aggregator->DatabaseProtocol.CreateKeyStrokes     = CreateKeyStrokes;
    Aggregator->DatabaseProtocol.RemoveKeyStrokes     = RemoveKeyStrokes;
    Aggregator->DatabaseProtocol.SetKeyStrokeKeys     = SetKeyStrokesKeys;
    Aggregator->AggregatorProtocol.Revision           = APPLE_KEY_MAP_AGGREGATOR_PROTOCOL_REVISION;
    Aggregator->AggregatorProtocol.GetKeyStrokes      = GetKeyStrokes;
    Aggregator->AggregatorProtocol.ContainsKeyStrokes = ContainsKeyStrokes;

    InitializeListHead (&Aggregator->KeyStrokesInfoList);

    SafeInstallProtocolInterface (&gAppleKeyMapAggregatorProtocolGuid, &Aggregator->AggregatorProtocol, gImageHandle, TRUE);
    SafeInstallProtocolInterface (&gAppleKeyMapDatabaseProtocolGuid, &Aggregator->DatabaseProtocol, gImageHandle, TRUE);
  }

  return Status;
}