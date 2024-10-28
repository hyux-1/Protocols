/*++

 Created by HermitCrabs on 04/13/2010.
 Copyright 2010-2015 The HermitCrab Labs. All rights reserved.

Module Name:

  AppleDeviceProperty.c

Abstract:

Revision History

--*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PicoFileLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/AppleDeviceProperty.h>
#include <Guid/FileInfo.h>

#include "Darwin.h"

APPLE_DEVICE_PROPERTY gDevicePropertyProtocol;

EFI_GUID gAppleDevicePropertyProtocolGuid = APPLE_DEVICE_PROPERTY_GUID;

//
//
//

VOID *
AllocMem (
  IN  UINTN                                 uiAllocSize
  )
{
  VOID                                      *pBuffer;

  if (EFI_ERROR(gBS->AllocatePool (EfiBootServicesData, uiAllocSize, (VOID **)&pBuffer))) {
    return NULL;
  }

  return pBuffer;
}

VOID FreeMem (
  IN  VOID                                  *Buffer
  )
{
  FreePool(Buffer);
}

BOOLEAN
CompareDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL              *DevicePath1,
  IN  EFI_DEVICE_PATH_PROTOCOL              *DevicePath2
  )
{
  UINTN                                     Size1;
  UINTN                                     Size2;

  Size1 = GetDevicePathSize (DevicePath1);
  Size2 = GetDevicePathSize (DevicePath2);

  if (Size1 != Size2) {
    return FALSE;
  }

  return ((BOOLEAN)(CompareMem ((VOID *)DevicePath1, (VOID *)DevicePath2, Size1) == 0));
}

EFI_STATUS
EFIAPI
Unknown_00 (
  IN  APPLE_DEVICE_PROPERTY_PROTOCOL      *This
  )
{
  DEBUG ((DEBUG_VERBOSE, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_VERBOSE, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
Unknown_01 (
  IN  APPLE_DEVICE_PROPERTY_PROTOCOL      *This
  )
{
  DEBUG ((DEBUG_VERBOSE, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_VERBOSE, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

APPLE_DEVICE_PROPERTY_DEVICE *
FindDevice (
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath
  )
{
  APPLE_DEVICE_PROPERTY_DEVICE  *pDevice = gDevicePropertyProtocol.RootDevice;

  while (pDevice != NULL) {

    if (CompareDevicePath (pDevice->DevicePath, DevicePath) == TRUE) {
      return pDevice;
    }

    pDevice = (APPLE_DEVICE_PROPERTY_DEVICE *)pDevice->Next;
  }

  return pDevice;
}

APPLE_DEVICE_PROPERTY_DEVICE *
GetLastDevice (
  )
{
  APPLE_DEVICE_PROPERTY_DEVICE  *pDevice = gDevicePropertyProtocol.RootDevice;

  if (pDevice != NULL) {

    while (pDevice->Next != NULL) {

      pDevice = (APPLE_DEVICE_PROPERTY_DEVICE *)pDevice->Next;

    }
  }

  return pDevice;
}

APPLE_DEVICE_PROPERTY_DEVICE *
NewDevice (
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath
  )
{
  APPLE_DEVICE_PROPERTY_DEVICE  *pDevice = NULL;

  pDevice = AllocMem (sizeof(APPLE_DEVICE_PROPERTY_DEVICE));

  if (pDevice != NULL) {

    pDevice->RootProperty = NULL;
    pDevice->Prev = NULL;
    pDevice->Next = NULL;
    pDevice->NumProperties = 0;
    pDevice->DevicePath = DuplicateDevicePath ((EFI_DEVICE_PATH_PROTOCOL *)DevicePath);

  }

  return pDevice;
}

APPLE_DEVICE_PROPERTY_DEVICE *
AddDevice (
  EFI_DEVICE_PATH_PROTOCOL                  *DevicePath
  )
{
  APPLE_DEVICE_PROPERTY_DEVICE  *pNewDevice  = NewDevice(DevicePath);
  APPLE_DEVICE_PROPERTY_DEVICE  *pLastDevice = GetLastDevice();

  if (pNewDevice != NULL) {

    if (pLastDevice != NULL) {

      pLastDevice->Next = (VOID *)pNewDevice;
      pNewDevice->Prev  = (VOID *)pLastDevice;

    } else {

      gDevicePropertyProtocol.NumDevices = 0;
      gDevicePropertyProtocol.Version    = 1;
      gDevicePropertyProtocol.Size       = sizeof(gDevicePropertyProtocol.Size) + sizeof(gDevicePropertyProtocol.Version) + sizeof(gDevicePropertyProtocol.NumDevices);
      gDevicePropertyProtocol.RootDevice = pNewDevice;

    }

    gDevicePropertyProtocol.NumDevices++;

  }

  return pNewDevice;
}

APPLE_DEVICE_PROPERTY_PROPERTY *
GetLastProperty (
  IN  APPLE_DEVICE_PROPERTY_DEVICE    *pDevice
  )
{
  APPLE_DEVICE_PROPERTY_PROPERTY  *pProperty = NULL;

  if (pDevice != NULL) {

    pProperty = pDevice->RootProperty;

    if (pProperty != NULL) {

      while (pProperty->Next != NULL) {

        pProperty = pProperty->Next;

      }
    }
  }

  return pProperty;
}

APPLE_DEVICE_PROPERTY_PROPERTY *
FindProperty (
  IN  APPLE_DEVICE_PROPERTY_DEVICE      *Device,
  IN  CHAR16                            *Property
  )
{
  APPLE_DEVICE_PROPERTY_PROPERTY      *pProperty = Device->RootProperty;

  while (pProperty != NULL) {

    if (StrCmp (pProperty->Property, Property) == 0) {

      break;

    }

    pProperty = (APPLE_DEVICE_PROPERTY_PROPERTY *)pProperty->Next;

  }

  return pProperty;
}

APPLE_DEVICE_PROPERTY_PROPERTY *
NewProperty (
  IN  CHAR16                                *Property,
  IN  CHAR8                                 *Data,
  IN  UINTN                                 Length
  )
{
  APPLE_DEVICE_PROPERTY_PROPERTY      *pProperty;

  pProperty = AllocMem (sizeof(APPLE_DEVICE_PROPERTY_PROPERTY));

  if (pProperty != NULL) {

    pProperty->Property = (CHAR16 *)AllocMem (StrSize (Property));

    if (pProperty->Property != NULL) {

      CopyMem (pProperty->Property, Property, StrSize (Property));

      pProperty->Data = (CHAR8 *) AllocMem (Length);

      if (pProperty->Data != NULL) {

        CopyMem (pProperty->Data, Data, Length);

        pProperty->Length   = Length;
        pProperty->Prev     = NULL;
        pProperty->Next     = NULL;

      } else {

        FreeMem (pProperty->Property);
        pProperty->Property = NULL;

        FreeMem (pProperty);
        pProperty           = NULL;

      }

    } else {

      FreeMem (pProperty);
      pProperty = NULL;

    }
  }

  return pProperty;
}

APPLE_DEVICE_PROPERTY_PROPERTY *
AddProperty (
  IN  APPLE_DEVICE_PROPERTY_DEVICE      *Device,
  IN  CHAR16                            *Property,
  IN  CHAR8                             *Data,
  IN  UINTN                             Length
  )
{
  APPLE_DEVICE_PROPERTY_PROPERTY  *pNewProperty  = NewProperty (Property, Data, Length);
  APPLE_DEVICE_PROPERTY_PROPERTY  *pLastProperty = GetLastProperty (Device);

  UINTN   TotalSize = 0;

  if (pNewProperty != NULL) {

    if (pLastProperty != NULL) {

      pLastProperty->Next = (VOID *)pNewProperty;
      pNewProperty->Prev  = (VOID *)pLastProperty;

    } else {

      Device->NumProperties = 0;
      TotalSize = sizeof(Device->NumProperties) + sizeof(Device->Size) + GetDevicePathSize (Device->DevicePath);
      Device->Size = (UINT32)TotalSize;
      gDevicePropertyProtocol.Size += (UINT32)TotalSize;
      Device->RootProperty = pNewProperty;

    }

    Device->NumProperties++;

    TotalSize = Length + StrSize (Property) + (sizeof(UINT32) * 2);

    Device->Size += (UINT32)TotalSize;
    gDevicePropertyProtocol.Size += (UINT32)TotalSize;

  }

  return pNewProperty;
}

APPLE_DEVICE_PROPERTY_PROPERTY *
ReplaceProperty (
  IN  APPLE_DEVICE_PROPERTY_DEVICE      *Device,
  IN  APPLE_DEVICE_PROPERTY_PROPERTY    *OldProperty,
  IN  CHAR16                            *Property,
  IN  CHAR8                             *Data,
  IN  UINTN                             Length
  )
{
  APPLE_DEVICE_PROPERTY_PROPERTY  *pPrev  = (APPLE_DEVICE_PROPERTY_PROPERTY *)OldProperty->Prev;
  APPLE_DEVICE_PROPERTY_PROPERTY  *pNext  = (APPLE_DEVICE_PROPERTY_PROPERTY *)OldProperty->Next;
  APPLE_DEVICE_PROPERTY_PROPERTY  *pNewProperty = NewProperty (Property, Data, Length);

  INTN    TotalSize = 0;

  if (pNewProperty != NULL) {

    TotalSize = (Length + StrSize (Property) + (sizeof(UINT32) * 2)) - (OldProperty->Length + (sizeof(UINT32) * 2) + StrSize (OldProperty->Property));
    Device->Size += (UINT32)TotalSize;
    gDevicePropertyProtocol.Size += (UINT32)TotalSize;

    FreeMem (OldProperty->Property);
    FreeMem (OldProperty->Data);

    OldProperty->Data   = NULL;
    OldProperty->Length = 0;
    OldProperty->Next   = NULL;
    OldProperty->Prev   = NULL;

    pPrev->Next         = (VOID *)pNewProperty;
    pNewProperty->Prev  = (VOID *)pPrev;
    pNext->Prev         = (VOID *)pNewProperty;
    pNewProperty->Next  = (VOID *)pNext;

  }

  return pNewProperty;
}

EFI_STATUS
EFIAPI
SetWithDevicePath (
  IN  APPLE_DEVICE_PROPERTY_PROTOCOL    *This,
  IN  EFI_DEVICE_PATH_PROTOCOL          *DevicePath,
  IN  CHAR16                            *Property,
  IN  CHAR8                             *Data,
  IN  UINTN                             Length
  )
{
  EFI_STATUS    Status;
  APPLE_DEVICE_PROPERTY_DEVICE    *pDevice   = NULL;
  APPLE_DEVICE_PROPERTY_PROPERTY  *pProperty = NULL;
  CHAR16    *DevicePathText = NULL;
  CHAR8     *DataString;

  pDevice = FindDevice (DevicePath);

  if (pDevice != NULL) {

    pProperty = FindProperty (pDevice, Property);

    if (pProperty != NULL) {

      pProperty = ReplaceProperty (pDevice, pProperty, Property, Data, Length);

    } else {

      pProperty = AddProperty (pDevice, Property, Data, Length);

    }

  } else {

    pDevice = AddDevice (DevicePath);

    if (pDevice != NULL) {

      pProperty = AddProperty (pDevice, Property, Data, Length);

    }
  }

  Status = !(pDevice != NULL && pProperty != NULL);

  DevicePathText = DevicePathToText (DevicePath, FALSE, FALSE);

  DataString = ConvertDataToString (Data, Length);

  if (DataString != NULL) {

    LOG ((EFI_D_INFO, " Injected %s - \"%s\" = %a (%d) - %r\n",
                      DevicePathText,
                      Property,
                      DataString,
                      Length,
                      Status));

    FreePool (DataString);

  } else {

    LOG ((EFI_D_INFO, " Injected %s - \"%s\" = Data (%d) - %r\n",
                      DevicePathText,
                      Property,
                      Length,
                      Status));

  }


  if (DevicePathText) {
    FreePool (DevicePathText);
  }

  return Status;
}

EFI_STATUS
EFIAPI
Unknown_03 (
  IN  APPLE_DEVICE_PROPERTY_PROTOCOL  *This
  )
{
  DEBUG ((DEBUG_VERBOSE, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_VERBOSE, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

EFI_STATUS
LoadDevicePropertiesOveride (
  IN  APPLE_DEVICE_PROPERTY_PROTOCOL  *This
  )
{
  EFI_STATUS                Status = EFI_UNSUPPORTED;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_FILE                  *Directory;

  CHAR16    *DevicePathText;
  CHAR16    *DevPropFilename = L"dev-prop.bin";
  CHAR8     *DevPropBuffer = NULL;
  UINTN     DevPropLength = 0;
  UINT32    *Device = NULL;
  UINT32    Version;
  UINT32    NumDevices;
  UINT32    DeviceNumProperties;

  CHAR16    *Property;
  UINT32    PropertySize;

  CHAR8     *Data;
  UINT32    DataSize;

  STATIC BOOLEAN  AlreadyRun = FALSE;

  if (AlreadyRun) {
    return EFI_ALREADY_STARTED;
  }

  Status = OpenDirectory (gStorageDevice,
                          DarwinDirectoryPath,
                          EFI_FILE_MODE_READ,
                          EFI_FILE_DIRECTORY,
                          &Directory);

  if (!EFI_ERROR (Status)) {

    Status = LoadFile (gStorageDevice,
                       DarwinDirectoryPath,
                       DevPropFilename,
                       (VOID **)&DevPropBuffer,
                       &DevPropLength);

    if (!EFI_ERROR (Status)) {

      Device = (UINT32 *)DevPropBuffer;

      Version    = *Device++;
      NumDevices = *Device++;

      LOG ((EFI_D_INFO, "Loading %s%s v%d %d Bytes Contains %d devices - %r\n",
                         DarwinDirectoryPath,
                         DevPropFilename,
                         Version,
                         DevPropLength,
                         NumDevices,
                         Status));

      do {

        DeviceNumProperties = *Device++;

        LOG ((EFI_D_INFO, "Adding Device with %d Properties\n", DeviceNumProperties));

        DevicePath = DuplicateDevicePath ((EFI_DEVICE_PATH_PROTOCOL *)(UINTN)Device);
        DevicePathText = DevicePathToText (DevicePath, FALSE, FALSE);

        Device  = (UINT32 *)(((UINT8 *)Device) + GetDevicePathSize (DevicePath));

        do {

          PropertySize = *Device;
          Property     = (UINT16 *)((UINTN)(Device) + sizeof(UINT32));
          Device       = (UINT32 *)((UINTN)(Device) + PropertySize);

          DataSize     = *Device;
          Data         = (CHAR8 *)((UINTN)(Device) + sizeof(UINT32));
          Device       = (UINT32 *)((UINTN)(Device) + DataSize);

          DataSize    -= sizeof(UINT32);

          SetWithDevicePath (This,
                             DevicePath,
                             Property,
                             Data,
                             DataSize);

          DeviceNumProperties--;

        } while (DeviceNumProperties);


        FreePool (DevicePathText);
        FreePool (DevicePath);

        NumDevices--;

      } while (NumDevices);

      FreePool (DevPropBuffer);

    }
  }

  AlreadyRun = TRUE;

  return Status;
}

//
//
//

EFI_STATUS
EFIAPI
GetDeviceProperties (
  IN  APPLE_DEVICE_PROPERTY_PROTOCOL    *This,
  IN  CHAR8                                 *DevicePropertiesData,
  IN OUT UINT32                             *DevicePropertiesSize
  )
{
  EFI_STATUS                            Status     = EFI_UNSUPPORTED;
  APPLE_DEVICE_PROPERTY_DEVICE          *Device   = gDevicePropertyProtocol.RootDevice;
  APPLE_DEVICE_PROPERTY_PROPERTY        *pProperty = NULL;
  UINT32                                *Buffer  = NULL;
  UINTN                                 uiSize     = 0;

  DEBUG ((DEBUG_VERBOSE, "%a: Started\n", __FUNCTION_NAME__));

  // Attempt to load dev-prop.bin overide file

  LoadDevicePropertiesOveride (This);

  // Ensure DevicePropertiesData buffer is large enough

  if (gDevicePropertyProtocol.Size > *DevicePropertiesSize) {

    Status = EFI_BUFFER_TOO_SMALL;

  } else if (gDevicePropertyProtocol.Size > 0) {

      Buffer = (UINT32 *)DevicePropertiesData;

      *Buffer++ = gDevicePropertyProtocol.Size;
      *Buffer++ = gDevicePropertyProtocol.Version;
      *Buffer++ = gDevicePropertyProtocol.NumDevices;

      while (Device != NULL) {

        *Buffer++ = Device->Size;
        *Buffer++ = Device->NumProperties;

        uiSize = GetDevicePathSize (Device->DevicePath);

        CopyMem ((VOID *)Buffer, Device->DevicePath, uiSize);

        pProperty = Device->RootProperty;

        while (pProperty != NULL) {

          Buffer    = (UINT32 *)(((UINT8 *)Buffer) + uiSize);
          uiSize    = StrSize (pProperty->Property);
          *Buffer++ = (UINT32)(uiSize + sizeof(UINT32));

          CopyMem ((VOID *)Buffer, pProperty->Property, uiSize);

          Buffer    = (UINT32 *)(((UINT8 *)Buffer) + uiSize);
          uiSize    = pProperty->Length;
          *Buffer++ = (UINT32)(uiSize + sizeof(UINT32));

          CopyMem ((VOID *)Buffer, pProperty->Data, uiSize);

          pProperty = pProperty->Next;

        }

        Buffer = (UINT32 *)(((UINT8 *)Buffer) + uiSize);

        Device = Device->Next;

      }

      Status = EFI_SUCCESS;

  } else {

    Status = EFI_UNSUPPORTED;

  }

  *DevicePropertiesSize = gDevicePropertyProtocol.Size;

  LOG ((EFI_D_INFO, "GetDeviceProperties Buffer 0x%0X Size 0x%0X Protocol Size 0x%0X %d Devices - %r\n",
                     DevicePropertiesData,
                     *DevicePropertiesSize,
                     gDevicePropertyProtocol.Size,
                     gDevicePropertyProtocol.NumDevices,
                     Status));

  DEBUG ((DEBUG_VERBOSE, "%a: Finished\n", __FUNCTION_NAME__));

  return Status;
}

APPLE_DEVICE_PROPERTY_PROTOCOL mAppleDevicePropertyProtocol = {
  1,
  Unknown_01,
  SetWithDevicePath,
  Unknown_03,
  GetDeviceProperties
};
