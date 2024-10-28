/** @file

**/

#ifndef _FV_ON_FV2_THUNK_H_
#define _FV_ON_FV2_THUNK_H_

#include <PiDxe.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/FirmwareVolume.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>

EFI_STATUS
EFIAPI
InstallFirmwareVolumeProtocol (
  VOID
  )
;

#endif
