/*++

 Created by HermitCrabs on 09/2/2014.
 Copyright 2014-2015 The HermitCrab Labs. All rights reserved.

Module Name:

  AppleKeyMapAggregator.c

Abstract:

Revision History

  1.1 Revised AppleKeyMapAggregator
  1.0 Initial Version

--*/

#include <Uefi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/AppleKeyMapLib.h>
#include <Protocol/AppleKeyMapAggregator.h>
#include <Protocol/AmiEfiKeyCode.h>

#include "Macros.h"
#include "EfiKeycodeMap.h"
#include "AppleKeyMapInternal.h"

//
//
//

VOID
BubbleSort (
  IN OUT UINT16  *Operand,
  IN     UINTN   NoChilds
  ) // sub_72C
{
  UINTN  NoRemainingChilds;
  UINTN  Index;
  UINTN  NoRemainingChilds2;
  UINT16 *OperandPtr;
  UINT16 FirstChild;

  ASSERT (Operand != NULL);
  ASSERT (NoChilds != 0);

  if (Operand > 0) {
    ++Operand;
    NoRemainingChilds = (NoChilds - 1);
    Index             = 1;

    do {
      NoRemainingChilds2 = NoRemainingChilds;
      OperandPtr         = Operand;

      if (Index < NoChilds) {
        do {
          FirstChild = Operand[-1];

          if (FirstChild > *OperandPtr) {
            Operand[-1] = *OperandPtr;
            *OperandPtr = FirstChild;
          }

          ++OperandPtr;
          --NoRemainingChilds2;
        } while (NoRemainingChilds2 > 0);
      }

      ++Index;
      ++Operand;
    } while ((NoRemainingChilds--) > 0);
  }
}

// GET_PRESSED_KEYS
/// @brief Returns all pressed keys and key modifiers into the appropiate buffers.
///
/// @param[in]  This      A pointer to the protocol instance.
/// @param[out] Modifiers The modifiers manipulating the given keys.
/// @param[out] NoKeys    On input the number of keys allocated.
///                       On output the number of keys returned into Keys.
/// @param[out] Keys      A Pointer to a caller-allocated the pressed keys get returned in.
///
/// @return                      Returned is the status of the operation.
/// @retval EFI_SUCCESS          The pressed keys have been returned into Keys.
/// @retval EFI_BUFFER_TOO_SMALL The memory required to return the value exceeds the size of the allocated buffer.
///                              The required number of keys to allocate to complete the operation has been returned into NoKeys.
/// @retval other                An error returned by a sub-operation.

EFI_STATUS
EFIAPI
GetKeyStrokes (
  IN     APPLE_KEY_MAP_AGGREGATOR_PROTOCOL  *This,
     OUT APPLE_MODIFIERS                    *Modifiers,
     OUT UINTN                              *NoKeys,
     OUT APPLE_KEY                          *Keys
  )
{
  EFI_STATUS               Status;

  APPLE_KEY_MAP_AGGREGATOR *Aggregator;
  APPLE_KEY_STROKES_INFO   *KeyStrokesInfo;
  APPLE_MODIFIERS          DbModifiers;
  BOOLEAN                  Result;
  UINTN                    DbNoKeyStrokes;
  UINTN                    Index;
  UINTN                    Index2;
  APPLE_KEY                Key;

  Aggregator     = AGGREGATOR_FROM_AGGREGATOR_PROTOCOL (This);
  KeyStrokesInfo = KEY_STROKES_FROM_LIST_ENTRY (GetFirstNode (&Aggregator->KeyStrokesInfoList));
  Result         = IsNull (&Aggregator->KeyStrokesInfoList, &KeyStrokesInfo->Hdr.This);

  if (Result) {
    *NoKeys        = 0;
    DbNoKeyStrokes = 0;
    DbModifiers    = 0;
  } else {
    DbModifiers    = 0;
    DbNoKeyStrokes = 0;

    do {
      DbModifiers |= KeyStrokesInfo->Hdr.Modifiers;

      if (KeyStrokesInfo->Hdr.NoKeys > 0) {
        Index = 0;

        do {
          Key = (&KeyStrokesInfo->Keys)[Index];
          ++Index;

          for (Index2 = 0; Index2 < DbNoKeyStrokes; ++Index2) {
            if (Aggregator->KeyBuffer[Index2] == Key) {
              break;
            }
          }

          if (DbNoKeyStrokes == Index2) {
            Aggregator->KeyBuffer[DbNoKeyStrokes] = Key;
            ++DbNoKeyStrokes;
          }
        } while (Index < KeyStrokesInfo->Hdr.NoKeys);
      }

      KeyStrokesInfo = KEY_STROKES_FROM_LIST_ENTRY (GetNextNode (&Aggregator->KeyStrokesInfoList, &KeyStrokesInfo->Hdr.This));
      Result         = IsNull (&Aggregator->KeyStrokesInfoList, &KeyStrokesInfo->Hdr.This);
    } while (!Result);

    Result  = (BOOLEAN)(DbNoKeyStrokes > *NoKeys);
    *NoKeys = DbNoKeyStrokes;
    Status  = EFI_BUFFER_TOO_SMALL;

    if (Result) {
      goto Return;
    }
  }

  *Modifiers = DbModifiers;
  *NoKeys    = DbNoKeyStrokes;
  Status     = EFI_SUCCESS;

  if (Keys != NULL) {
    gBS->CopyMem ((VOID *)Keys, (VOID *)Aggregator->KeyBuffer, (DbNoKeyStrokes * sizeof (*Keys)));
  }

Return:
  return Status;
}

// CONTAINS_PRESSED_KEYS
/// @brief Returns whether or not a list of keys and their modifiers are part of the database of pressed keys.
///
/// @param[in]     This       A pointer to the protocol instance.
/// @param[in]     Modifiers  The modifiers manipulating the given keys.
/// @param[in]     NoKeys     The number of keys present in Keys.
/// @param[in,out] Keys       The list of keys to check for. The children may be sorted in the process.
/// @param[in]     ExactMatch Specifies whether Modifiers and Keys should be exact matches or just contained.
///
/// @return                      Returns whether or not a list of keys and their modifiers are part of the database of pressed keys.
/// @retval        EFI_SUCCESS   The queried keys are part of the database.
/// @retval        EFI_NOT_FOUND The queried keys could not be found.

EFI_STATUS
EFIAPI
ContainsKeyStrokes (
  IN     APPLE_KEY_MAP_AGGREGATOR_PROTOCOL  *This,
  IN     APPLE_MODIFIERS                    Modifiers,
  IN     UINTN                              NoKeys,
  IN OUT APPLE_KEY                          *Keys,
  IN     BOOLEAN                            ExactMatch
  )
{
  EFI_STATUS      Status;

  UINTN           DbNoKeys;
  APPLE_MODIFIERS DbModifiers;
  APPLE_KEY       DbKeys[8];
  INTN            Result;
  UINTN           Index;
  UINTN           DbIndex;

  DbNoKeys = (sizeof (DbKeys) / sizeof (*DbKeys));
  Status   = This->GetKeyStrokes (This, &DbModifiers, &DbNoKeys, DbKeys);

  if (!EFI_ERROR (Status)) {
    if (ExactMatch) {
      Status = EFI_NOT_FOUND;

      if ((DbModifiers == Modifiers) && (DbNoKeys == NoKeys)) {
        BubbleSort (Keys, NoKeys);
        BubbleSort (DbKeys, DbNoKeys);

        Result = CompareMem ((VOID *)Keys, (VOID *)DbKeys, (NoKeys * sizeof (*Keys)));

        if (Result == 0) {
          Status = EFI_SUCCESS;
        }
      }
    } else {
      Status = EFI_NOT_FOUND;

      if ((DbModifiers & Modifiers) == Modifiers) {
        for (Index = 0; Index < NoKeys; ++Index) {
          for (DbIndex = 0; DbIndex < DbNoKeys; ++DbIndex) {
            if (Keys[Index] == DbKeys[DbIndex]) {
              break;
            }
          }

          if (DbNoKeys == DbIndex) {
            break;
          }

          Status = EFI_SUCCESS;
        }
      }
    }
  }

  return Status;
}
