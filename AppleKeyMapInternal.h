/*++

  Ported by HermitCrabs on 07/10/2015.
  Copyright 2015 Apple Inc. All rights reserved.

Module Name:

  AppleKeyMapInternal.h

Abstract:

  Reversed from AppleKeyMapAggregator.efi and AppleEvent.efi, which are Apple Inc. property
  Decompiled by Download-Fritz

Revision History

  1.0 Initial Version

--*/

#ifndef _APPLE_KEY_MAP_IMPL_H_
#define _APPLE_KEY_MAP_IMPL_H_

#include <Protocol/AppleKeyMapDatabase.h>
#include <Protocol/AppleKeyMapAggregator.h>

#define APPLE_KEY_MAP_PROTOCOLS_REVISION            0x10000
#define APPLE_KEY_MAP_DATABASE_PROTOCOL_REVISION    APPLE_KEY_MAP_PROTOCOLS_REVISION
#define APPLE_KEY_MAP_AGGREGATOR_PROTOCOL_REVISION  APPLE_KEY_MAP_PROTOCOLS_REVISION

// APPLE_KEY_STROKES_SIGNATURE
#define APPLE_KEY_STROKES_INFO_SIGNATURE  SIGNATURE_32 ('K', 'e', 'y', 'S')

// APPLE_KEY_MAP_AGGREGATOR_SIGNATURE
#define APPLE_KEY_MAP_AGGREGATOR_SIGNATURE  SIGNATURE_32 ('K', 'e', 'y', 'A')

// KEY_STROKES_FROM_LIST_ENTRY
#define KEY_STROKES_FROM_LIST_ENTRY(ListEntry) BASE_CR (ListEntry, APPLE_KEY_STROKES_INFO, Hdr.This)

// AGGREGATOR_FROM_AGGREGATOR_PROTOCOL
#define AGGREGATOR_FROM_AGGREGATOR_PROTOCOL(This) BASE_CR (This, APPLE_KEY_MAP_AGGREGATOR, AggregatorProtocol)

// AGGREGATOR_FROM_DATABASE_PROTOCOL
#define AGGREGATOR_FROM_DATABASE_PROTOCOL(This) BASE_CR (This, APPLE_KEY_MAP_AGGREGATOR, DatabaseProtocol)

// _APPLE_KEY_MAP_AGGREGATOR
typedef struct _APPLE_KEY_MAP_AGGREGATOR {
  UINTN                             Signature;           ///< @biref 
  UINTN                             NextKeyStrokeIndex;  ///< @biref 
  APPLE_KEY                         *KeyBuffer;          ///< @biref 
  UINTN                             KeyBufferLengths;          ///< @biref 
  LIST_ENTRY                        KeyStrokesInfoList;          ///< @biref 
  APPLE_KEY_MAP_DATABASE_PROTOCOL   DatabaseProtocol;    ///< @biref 
  APPLE_KEY_MAP_AGGREGATOR_PROTOCOL AggregatorProtocol;  ///< @biref 
} APPLE_KEY_MAP_AGGREGATOR;

// _APPLE_KEY_STROKES_INFO
typedef struct _APPLE_KEY_STROKES_INFO {
  struct {
    UINTN              Signature;   ///< @biref 
    LIST_ENTRY         This;        ///< @biref 
    UINTN              Index;       ///< @biref 
    UINTN              KeyBufferLength;  ///< @biref 
    UINTN              NoKeys;      ///< @biref 
    APPLE_MODIFIERS Modifiers;   ///< @biref 
  }         Hdr;                    ///< @biref 
  APPLE_KEY Keys;                   ///< @biref 
} APPLE_KEY_STROKES_INFO;

// CreateKeyStrokes
/// @brief Creates a new key set with a given number of keys allocated. The index within the database is returned.
///
/// @param[in]  This                       A pointer to the protocol instance.
/// @param[in]  KeysLength                 The amount of keys to allocate for the key set.
/// @param[out] Index                      The assigned index of the created key set.
///
/// @return Returned is the status of the operation.
/// @retval EFI_SUCCESS                    A key set with the given number of keys allocated has been created.
/// @retval EFI_OUT_OF_RESOURCES           The memory necessary to complete the operation could not be allocated.
/// @retval other                          An error returned by a sub-operation.

EFI_STATUS
EFIAPI
CreateKeyStrokes (
  IN     APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN     UINTN                            KeysLength,
  OUT UINTN                            *Index
  );

// RemoveKeyStrokes
/// @brief Removes a key set specified by its index from the database.
///
/// @param[in]  This                        A pointer to the protocol instance.
/// @param[in]  Index                       The index of the key set to remove.
///
/// @return Returned is the status of the operation.
/// @retval EFI_SUCCESS                     The specified key set has been removed.
/// @retval EFI_NOT_FOUND                   No key set could be found for the given index.
/// @retval other                           An error returned by a sub-operation.

EFI_STATUS
EFIAPI
RemoveKeyStrokes (
  IN APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN UINTN                            Index
  );

// SetKeyStrokesKeys
/// @brief Sets the keys of a key set specified by its index to the given Keys buffer.
///
/// @param[in] This                       A pointer to the protocol instance.
/// @param[in] Index                      The index of the key set to edit.
/// @param[in] Modifiers                  The key modifiers manipulating the given keys.
/// @param[in] NoKeys                     The number of keys contained in Keys.
/// @param[in] Keys                       An array of keys to add to the specified key set.
///
/// @return Returned is the status of the operation.
/// @retval EFI_SUCCESS                    The given keys were set for the specified key set.
/// @retval EFI_OUT_OF_RESOURCES           The memory necessary to complete the operation could not be allocated.
/// @retval EFI_NOT_FOUND                  No key set could be found for the given index.
/// @retval other                          An error returned by a sub-operation.

EFI_STATUS
EFIAPI
SetKeyStrokesKeys (
  IN APPLE_KEY_MAP_DATABASE_PROTOCOL  *This,
  IN UINTN                            Index,
  IN APPLE_MODIFIERS                  Modifiers,
  IN UINTN                            NoKeys,
  IN APPLE_KEY                        *Keys
  );

// GET_PRESSED_KEYS
/// @brief Returns all pressed keys and key modifiers into the appropiate buffers.
///
/// @param[in]  This                             A pointer to the protocol instance.
/// @param[out] Modifiers                        The modifiers manipulating the given keys.
/// @param[out] NoKeys                           On input the number of keys allocated.
///                                              On output the number of keys returned into Keys.
/// @param[out] Keys                             A Pointer to a caller-allocated the pressed keys get returned in.
///
/// @return                                      Returned is the status of the operation.
/// @retval     EFI_SUCCESS                      The pressed keys have been returned into Keys.
/// @retval     EFI_BUFFER_TOO_SMALL             The memory required to return the value exceeds the size of the allocated buffer.
///                                              The required number of keys to allocate to complete the operation has been returned into NoKeys.
/// @retval     other                            An error returned by a sub-operation.

EFI_STATUS
EFIAPI
GetKeyStrokes (
  IN     APPLE_KEY_MAP_AGGREGATOR_PROTOCOL  *This,
  OUT APPLE_MODIFIERS                    *Modifiers,
  OUT UINTN                              *NoKeys,
  OUT APPLE_KEY                          *Keys
  );

// CONTAINS_PRESSED_KEYS
/// @brief Returns whether or not a list of keys and their modifiers are part of the database of pressed keys.
///
/// @param[in]     This                          A pointer to the protocol instance.
/// @param[in]     Modifiers                     The modifiers manipulating the given keys.
/// @param[in]     NoKeys                        The number of keys present in Keys.
/// @param[in,out] Keys                          The list of keys to check for. The children may be sorted in the process.
/// @param[in]     ExactMatch                    Specifies whether Modifiers and Keys should be exact matches or just contained.
///
/// @return                                      Returns whether or not a list of keys and their modifiers are part of the database of pressed keys.
/// @retval        EFI_SUCCESS                   The queried keys are part of the database.
/// @retval        EFI_NOT_FOUND                 The queried keys could not be found.

EFI_STATUS
EFIAPI
ContainsKeyStrokes (
  IN     APPLE_KEY_MAP_AGGREGATOR_PROTOCOL *This,
  IN     APPLE_MODIFIERS                Modifiers,
  IN     UINTN                             NoKeys,
  IN OUT APPLE_KEY                         *Keys,
  IN     BOOLEAN                           ExactMatch
  );

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
  );

#endif // ifndef _APPLE_KEY_MAP_IMPL_H_
