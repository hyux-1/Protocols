/*++

 Created by HermitCrabs on 03/06/2012.
 Copyright 2012-2014 The HermitCrab Labs. All rights reserved.

Module Name:

  BootLog.c

Abstract:

Revision History

  1.7   Support DEBUG_CONSOLE Error Level.
  1.6   Use External CalculateTSC
  1.5   Implement Variable save.
  1.4   Add SaveLog function.
  1.3   Control Serial and Console output.
  1.2   Support FormatString and Marker
  1.1   Add Timing functions.
  1.0   Initial Version.

--*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PcdLib.h>
#include <Library/PicoCpuLib.h>
#include <Library/PicoFileLib.h>

#include <Protocol/BootLog.h>

#include <Guid/BootLogVariable.h>
#include <Guid/OzmosisSettingsVariable.h>

#include "Macros.h"
#include "Ozmosis.h"
#include "VariableNames.h"

#define BOOT_LOG_VERSION 0x0107
#define BOOT_LOG_LINE_BUFFER_SIZE 1024
#define BOOT_LOG_SERIAL_BUFFER_SIZE 256
#define BOOT_LOG_FILE_PATH_BUFFER_SIZE 256

CHAR16    *mTimingTxt = NULL;

UINT8     *gSerialBuffer = NULL;
CHAR16    *LineBuffer = NULL;
CHAR16    *Buffer = NULL;
CHAR16    *LogFilePathName = NULL;

UINTN     BufferSize = 0;

UINT64    mTscFrequency = 0;
UINT64    mTscStart = 0;
UINT64    mTscLast = 0;

//
//
//

CHAR16 *
EFIAPI
GetTiming (
  IN  EFI_BOOT_LOG_PROTOCOL     *BootLog
  )
{
  UINT64    dTStartSec = 0;
  UINT64    dTStartMs = 0;
  UINT64    dTLastSec = 0;
  UINT64    dTLastMs = 0;
  UINT64    CurrentTsc = 0;

  if (mTimingTxt == NULL) {
    mTimingTxt = AllocateZeroPool (64);
    if (mTimingTxt == NULL) {
      return NULL;
    }
  }

  // Calibrate TSC for timings

  if (mTscFrequency == 0) {

    mTscFrequency = CalculateTSC (TRUE);

    if (mTscFrequency) {

      CurrentTsc = AsmReadTsc ();

      mTscStart = CurrentTsc;
      mTscLast = CurrentTsc;

    }

  }

  if (BootLog != NULL) {

    if (mTscFrequency > 0) {

      CurrentTsc = AsmReadTsc();

      dTStartMs  = DivU64x64Remainder (MultU64x32 (CurrentTsc - mTscStart, 1000), mTscFrequency, NULL);
      dTStartSec = DivU64x64Remainder (dTStartMs, 1000, &dTStartMs);
      dTLastMs   = DivU64x64Remainder (MultU64x32 (CurrentTsc - mTscLast, 1000), mTscFrequency, NULL);
      dTLastSec  = DivU64x64Remainder (dTLastMs, 1000, &dTLastMs);

      mTscLast = CurrentTsc;

    }

    HermitSPrint (mTimingTxt,
                  64,
                  OUTPUT_UNICODE | FORMAT_ASCII,
                  "%02d:%03d %02d:%03d ",
                  dTStartSec < 99 ? dTStartSec : 99,
                  dTStartMs < 999 ? dTStartMs : 999,
                  dTLastSec < 99 ? dTLastSec : 99,
                  dTLastMs < 999 ? dTLastMs : 999);

  }

  return mTimingTxt;
}

//
//
//

UINTN
EFIAPI
ConvertToSerial (
  IN  CHAR16  *UnicodeBuffer,
  OUT UINT8   *SerialBuffer
  )
{
  UINTN   Length = 0;

  if (UnicodeBuffer == NULL || SerialBuffer == NULL) {
    return 0;
  }

  // convert unicode buffer to ascii buffer skipping '\r'

  while (*UnicodeBuffer != '\0') {

    if (*UnicodeBuffer == L'\r') {
      UnicodeBuffer++;
    } else {
      *SerialBuffer++ = (CHAR8)((*UnicodeBuffer++) & 0xFF);
    }

    Length++;

  }

  *SerialBuffer = '\0';

  return Length;
}

EFI_STATUS
EFIAPI
BootLogAddEntry (
  IN  EFI_BOOT_LOG_PROTOCOL     *BootLog,
  IN  UINTN                     ErrorLevel,
  IN  CONST CHAR8               *FormatString,
  IN  VA_LIST                   Marker
  )
{
  EFI_STATUS    Status;
  UINTN         SerialBufferLength;
  EFI_TIME      LogStartTime;

  UINT64        MaximumVariableStorageSize;
  UINT64        RemainingVariableStorageSize;
  UINT64        MaximumVariableSize;
  UINTN         BootLogVariableSize;
  CHAR8         *AsciiBuffer;

  CHAR16        *Source;
  CHAR8         *Dest;

  UINTN         BootLogFlagsVariableSize;

  // Setup Version and Initial flags.

  if (BootLog->Version == 0) {

    BootLog->Version = BOOT_LOG_VERSION;

    // Check BootLogFlags Variable

    BootLogFlagsVariableSize = sizeof(BootLog->Flags);

    Status = gRT->GetVariable (kBootLogFlagsVariable,
                               &gEfiOzmosisSettingsGuid,
                               NULL,
                               &BootLogFlagsVariableSize,
                               (VOID *)&BootLog->Flags);
    if (EFI_ERROR(Status)) {

      BootLog->Flags = PcdGet8 (PcdBootLogFlags);

    }

  }

  if (BootLog->Flags == BOOT_LOG_DISABLE) {
    return EFI_WRITE_PROTECTED;    
  }

  // Allocate BootLog Buffer if not previously allocated.

  if (Buffer == NULL) {
    BufferSize = PcdGet32 (PcdBootLogMaximumLength) * sizeof(CHAR16);
    Buffer = AllocateZeroPool (BufferSize);
    if (Buffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  // Allocate Serial Buffer if not previously allocated.

  if (gSerialBuffer == NULL) {
    gSerialBuffer = AllocateZeroPool (BOOT_LOG_SERIAL_BUFFER_SIZE);
    if (gSerialBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    SerialBufferLength = 0;
  }

  if (LineBuffer == NULL) {
    LineBuffer = AllocateZeroPool (BOOT_LOG_LINE_BUFFER_SIZE);
    if (LineBuffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  // Create FilePath for Log

  if (BootLog->Flags & BOOT_LOG_FILE) {

    if (gStorageDevice != NULL && LogFilePathName == NULL) {

      LogFilePathName = AllocateZeroPool (BOOT_LOG_FILE_PATH_BUFFER_SIZE);

      if (LogFilePathName == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }

      HermitSPrint (LogFilePathName,
                    BOOT_LOG_FILE_PATH_BUFFER_SIZE,
                    OUTPUT_UNICODE | FORMAT_ASCII,
                    "%s%s",
                    PcdGetPtr (PcdPathRoot),
                    PcdGetPtr (PcdBootLogFolderPath));

      // Create The Log Folder

      if (FileExists (gStorageDevice, LogFilePathName) == FALSE) {
        CreatePath (gStorageDevice, LogFilePathName, EFI_FILE_DIRECTORY);
      }

      gRT->GetTime (&LogStartTime, NULL);

      HermitSPrint (LogFilePathName,
                    BOOT_LOG_FILE_PATH_BUFFER_SIZE,
                    OUTPUT_UNICODE | FORMAT_ASCII,
                    "%s%sbdmesg%d%02d%02d%02d%02d%02d.log",
                    PcdGetPtr (PcdPathRoot),
                    PcdGetPtr (PcdBootLogFolderPath),
                    LogStartTime.Year,
                    LogStartTime.Month,
                    LogStartTime.Day,
                    LogStartTime.Hour,
                    LogStartTime.Minute,
                    LogStartTime.Second);
    }

  }

  // Print String

  HermitVSPrint ((VOID *)LineBuffer,
                 BOOT_LOG_LINE_BUFFER_SIZE,
                 FORMAT_ASCII | OUTPUT_UNICODE | OUTPUT_CONSOLE,
                 FormatString,
                 Marker);

  // Add Entry

  if (*LineBuffer != '\0') {

    // Calculate Timing

    GetTiming (BootLog);

    if (StrSize(Buffer) + StrSize(LineBuffer) + StrSize (mTimingTxt) - 2 * sizeof (Buffer[0]) < BufferSize) {

      // Add To BootLog

      StrCat (Buffer, mTimingTxt);
      StrCat (Buffer, LineBuffer);

      // Send the print string to the Console Output device ?

      if ((BootLog->Flags & BOOT_LOG_CONSOLE) ||
          (ErrorLevel & DEBUG_CONSOLE))
      {
        if ((gST != NULL) && (gST->ConOut != NULL))
        {
          gST->ConOut->OutputString (gST->ConOut, mTimingTxt);
          gST->ConOut->OutputString (gST->ConOut, LineBuffer);
        }
      }

      // Write to Serial Port ?

      if (BootLog->Flags & BOOT_LOG_SERIAL) {
        SerialBufferLength = ConvertToSerial (mTimingTxt, gSerialBuffer);
        SerialBufferLength += ConvertToSerial (LineBuffer, (gSerialBuffer + SerialBufferLength));
        if (SerialBufferLength) {
          SerialPortWrite (gSerialBuffer, --SerialBufferLength);
        }
      }

      // Write to File ?

      if (BootLog->Flags & BOOT_LOG_FILE) {

        if (gStorageDevice != NULL && LogFilePathName != NULL) {

          Status = WriteFilePath (gStorageDevice,
                                  LogFilePathName,
                                  Buffer,
                                  StrSize (Buffer),
                                  EFI_FILE_ARCHIVE);

          if (Status != EFI_SUCCESS) {
            BootLog->Flags &= ~BOOT_LOG_FILE;
          }

        }
      }

      // Write to Variable ?

      if (BootLog->Flags & BOOT_LOG_VARIABLE) {

        Status = gRT->QueryVariableInfo (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                         &MaximumVariableStorageSize,
                                         &RemainingVariableStorageSize,
                                         &MaximumVariableSize);

        // Convert to ascii to save space.

        AsciiBuffer = AllocateZeroPool ((StrSize(Buffer) / sizeof (Buffer[0])) * sizeof (AsciiBuffer[0]));

        if (AsciiBuffer != NULL) {

          Source = Buffer;
          Dest = AsciiBuffer;

          BootLogVariableSize = 0;

          // Convert Unicode Buffer to Ascii Buffer

          while (*Source)
          {
            if (*Source != L'\r')
            {
              *Dest++ = (CHAR8)*Source;
              BootLogVariableSize++;
            }

            ++Source;
          }

          BootLogVariableSize = MIN(BootLogVariableSize, 65535 - 128);

          AsciiBuffer [BootLogVariableSize] = '\0';

          gRT->SetVariable (L"BootLog",
                            &gEfiBootLogVariableGuid,
                            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                            BootLogVariableSize,
                            AsciiBuffer);

          FreePool (AsciiBuffer);
        }
      }

    } else {

      return EFI_BUFFER_TOO_SMALL;

    }
  }

  return EFI_SUCCESS;
}

//
//
//

EFI_STATUS
EFIAPI
BootLogGetBootLog (
  IN  EFI_BOOT_LOG_PROTOCOL     *BootLog,
  OUT CHAR16                    **BootLogBuffer
  )
{
  *BootLogBuffer = Buffer;
  return EFI_SUCCESS;
}

//
//
//

EFI_STATUS
EFIAPI
BootLogSaveLog (
  IN  EFI_BOOT_LOG_PROTOCOL     *BootLog,
  IN  UINT32                    NonVolatile   OPTIONAL,
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath     OPTIONAL
  )
{
  return EFI_NOT_FOUND;
}

//
//
//

EFI_STATUS
EFIAPI
BootLogResetTimers (
  IN  EFI_BOOT_LOG_PROTOCOL     *BootLog
  )
{
  mTscFrequency = 0;
  return EFI_SUCCESS;
}

//

EFI_BOOT_LOG_PROTOCOL mEfiBootLogProtocol = {
  0,
  BOOT_LOG_DISABLE,
  BootLogAddEntry,
  BootLogGetBootLog,
  BootLogSaveLog,
  BootLogResetTimers,
};
