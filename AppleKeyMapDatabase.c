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
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/AppleKeyMapAggregator.h>
#include <Protocol/AppleKeyMapDatabase.h>
#include <Protocol/AmiEfiKeyCode.h>

#include "Macros.h"
#include "EfiKeycodeMap.h"
#include "AppleKeyMapInternal.h"

//
//
//

APPLE_KEY_STROKES_INFO *
EFIAPI
GetKeyStrokesByIndex (
  IN LIST_ENTRY  *List,
  IN UINTN       Index
  ) // sub_459
{
  APPLE_KEY_STROKES_INFO *KeyStrokes;

  BOOLEAN                Result;

  for (
    KeyStrokes = KEY_STROKES_FROM_LIST_ENTRY (GetFirstNode (List));
    KeyStrokes->Hdr.Index != Index;
    KeyStrokes = KEY_STROKES_FROM_LIST_ENTRY (GetNextNode (List, &KeyStrokes->Hdr.This))
    ) {
    Result = IsNull (List, &KeyStrokes->Hdr.This);

    if (Result) {
      KeyStrokes = NULL;
      break;
    }
  }

  return KeyStrokes;
}

// CreateKeyStrokes
/// @brief Creates a new key set with a given number of keys allocated. The index within the database is returned.
///
/// @param[in]  This   A pointer to the protocol instance.
/// @param[in]  NoKeys The amount of keys to allocate for the key set.
/// @param[out] Index  The assigned index of the created key set.
///
/// @return                      Returned is the status of the operation.
/// @retval EFI_SUCCESS          A key set with the given number of keys allocated has been created.
/// @retval EFI_OUT_OF_RESOURCES The memory necessary to complete the operation could not be allocated.
/// @retval other                An error returned by a sub-operation.

EFI_STATUS
EFIAPI
CreateKeyStrokes (
  IN     APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN     UINTN                            NoKeys,
     OUT UINTN                            *Index
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  UINTN                    NewKeysLength;
  VOID                     *Memory;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;

  Aggregator = AGGREGATOR_FROM_DATABASE_PROTOCOL (This);

  if (Aggregator->KeyBuffer != NULL) {
    gBS->FreePool ((VOID *)Aggregator->KeyBuffer);
  }

  NewKeysLength                = (Aggregator->KeyBufferLengths + NoKeys);
  Aggregator->KeyBufferLengths = NewKeysLength;
  Memory                       = AllocateZeroPool (NewKeysLength);
  Aggregator->KeyBuffer        = Memory;
  Status                       = EFI_OUT_OF_RESOURCES;

  if (Memory != NULL) {
    KeyStrokesInfo = (APPLE_KEY_STROKES_INFO *)AllocateZeroPool ((sizeof (*KeyStrokesInfo) + (NoKeys * sizeof (APPLE_KEY))));
    Status     = EFI_OUT_OF_RESOURCES;

    if (KeyStrokesInfo != NULL) {
      KeyStrokesInfo->Hdr.Signature       = APPLE_KEY_STROKES_INFO_SIGNATURE;
      KeyStrokesInfo->Hdr.KeyBufferLength = NoKeys;
      KeyStrokesInfo->Hdr.Index           = Aggregator->NextKeyStrokeIndex;
      ++Aggregator->NextKeyStrokeIndex;

      InsertTailList (&Aggregator->KeyStrokesInfoList, &KeyStrokesInfo->Hdr.This);

      Status = EFI_SUCCESS;
      *Index = KeyStrokesInfo->Hdr.Index;
    }
  }

  return Status;
}

// RemoveKeyStrokes
/// @brief Removes a key set specified by its index from the database.
///
/// @param[in]  This  A pointer to the protocol instance.
/// @param[in]  Index The index of the key set to remove.
///
/// @return               Returned is the status of the operation.
/// @retval EFI_SUCCESS   The specified key set has been removed.
/// @retval EFI_NOT_FOUND No key set could be found for the given index.
/// @retval other         An error returned by a sub-operation.

EFI_STATUS
EFIAPI
RemoveKeyStrokes (
  IN APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN UINTN                            Index
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;

  Aggregator     = AGGREGATOR_FROM_DATABASE_PROTOCOL (This);
  KeyStrokesInfo = GetKeyStrokesByIndex (&Aggregator->KeyStrokesInfoList, Index);
  Status         = EFI_NOT_FOUND;

  if (KeyStrokesInfo != NULL) {
    Aggregator->KeyBufferLengths -= KeyStrokesInfo->Hdr.KeyBufferLength;

    RemoveEntryList (&KeyStrokesInfo->Hdr.This);
    gBS->FreePool ((VOID *)KeyStrokesInfo);

    Status = EFI_SUCCESS;
  }

  return Status;
}

// SetKeyStrokesKeys
/// @brief Sets the keys of a key set specified by its index to the given Keys buffer.
///
/// @param[in] This      A pointer to the protocol instance.
/// @param[in] Index     The index of the key set to edit.
/// @param[in] Modifiers The key modifiers manipulating the given keys.
/// @param[in] NoKeys    The number of keys contained in Keys.
/// @param[in] Keys      An array of keys to add to the specified key set.
///
/// @return                      Returned is the status of the operation.
/// @retval EFI_SUCCESS          The given keys were set for the specified key set.
/// @retval EFI_OUT_OF_RESOURCES The memory necessary to complete the operation could not be allocated.
/// @retval EFI_NOT_FOUND        No key set could be found for the given index.
/// @retval other                An error returned by a sub-operation.

EFI_STATUS
EFIAPI
SetKeyStrokesKeys (
  IN APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN UINTN                            Index,
  IN APPLE_MODIFIERS                  Modifiers,
  IN UINTN                            NoKeys,
  IN APPLE_KEY                        *Keys
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;

  Aggregator = AGGREGATOR_FROM_DATABASE_PROTOCOL (This);
  KeyStrokesInfo = GetKeyStrokesByIndex (&Aggregator->KeyStrokesInfoList, Index);
  Status     = EFI_NOT_FOUND;

  if (KeyStrokesInfo != NULL) {
    Status = EFI_OUT_OF_RESOURCES;

    if (KeyStrokesInfo->Hdr.KeyBufferLength >= NoKeys) {
      KeyStrokesInfo->Hdr.NoKeys    = NoKeys;
      KeyStrokesInfo->Hdr.Modifiers = Modifiers;

      gBS->CopyMem ((VOID *)&KeyStrokesInfo->Keys, (VOID *)Keys, (NoKeys * sizeof (*Keys)));

      Status = EFI_SUCCESS;
    }
  }

  return Status;
}
