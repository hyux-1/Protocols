/*++

 Created by HermitCrabs on 10/8/2014.
 Copyright 2014-2015 The HermitCrab Labs. All rights reserved.

Module Name:

  AppleImagePngCodec.c

Abstract:

  Implement AppleImageCodec Protocol For PNG Images

Revision History

  1.0 Initial Version

--*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/UgaDraw.h>
#include <Protocol/AppleImageCodec.h>

#include "Macros.h"
#include "PicoPng.h"

/**

  PngRecognizeImageData

  @param[in]  ImageBuffer
  @param[in]  ImageSize

  @retval EFI_SUCCESS
  @retval other

**/

EFI_STATUS
EFIAPI
PngRecognizeImageData (
  VOID              *ImageBuffer,
  UINTN             ImageSize
  )
{
  PNG_INFO    *Png;

  Png = PngDecode (ImageBuffer, (UINT32)ImageSize, TRUE);

  if (Png == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: - %r\n", __FUNCTION_NAME__, EFI_UNSUPPORTED));
    return EFI_UNSUPPORTED;
  }

  PngAllocFreeAll ();

  return EFI_SUCCESS;
}

/**

  PngGetImageDims

  @param[in]  ImageBuffer
  @param[in]  ImageSize
  @param[out] ImageWidth
  @param[out] ImageHeight

  @retval EFI_SUCCESS
  @retval other

**/

EFI_STATUS
EFIAPI
PngGetImageDims (
  VOID              *ImageBuffer,
  UINTN             ImageSize,
  UINT32            *ImageWidth,
  UINT32            *ImageHeight
  )
{
  PNG_INFO    *Png;

  Png = PngDecode (ImageBuffer, (UINT32)ImageSize, TRUE);

  if (Png == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: - %r\n", __FUNCTION_NAME__, EFI_UNSUPPORTED));
    return EFI_UNSUPPORTED;
  }

  *ImageWidth = (UINT32)Png->Width;
  *ImageHeight = (UINT32)Png->Height;

  PngAllocFreeAll ();

  return EFI_SUCCESS;
}

/**

  PngDecodeImageData

  @param[in]  ImageBuffer
  @param[in]  ImageSize
  @param[out] RawImageData
  @param[out] RawImageDataSize

  @retval EFI_SUCCESS
  @retval other

**/

EFI_STATUS
EFIAPI
PngDecodeImageData (
  VOID              *ImageBuffer,
  UINTN             ImageSize,
  EFI_UGA_PIXEL     **RawImageData,
  UINTN             *RawImageDataSize
  )
{
  PNG_INFO      *Png;
  EFI_UGA_PIXEL *Image;
  UINTN         DecodedImageSize;

  Png = PngDecode (ImageBuffer, (UINT32)ImageSize, TRUE);

  if (Png == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: - %r\n", __FUNCTION_NAME__, EFI_UNSUPPORTED));
    return EFI_UNSUPPORTED;
  }

  DecodedImageSize = Png->Width * Png->Height * sizeof(EFI_UGA_PIXEL);

  if (DecodedImageSize) {

    Image = (EFI_UGA_PIXEL *)AllocatePool (DecodedImageSize);

    if (Image == NULL) {
      DEBUG ((EFI_D_ERROR, "%a: Error Allocating Buffer\n", __FUNCTION_NAME__));
      return EFI_OUT_OF_RESOURCES;
    }

    *RawImageData = Image;
    *RawImageDataSize = DecodedImageSize;

    CopyMem (Image, Png->Image->Data, DecodedImageSize);

  }

  PngAllocFreeAll ();

  return EFI_SUCCESS;
}

/**

  PngUnknownFunction04

  @retval EFI_SUCCESS
  @retval other

**/

EFI_STATUS
EFIAPI
PngUnknownFunction04 (
  VOID              *ImageBuffer,
  UINTN             ImageSize,
  UINT32            *ImageWidth,
  UINT32            *ImageHeight,
  UINTN             Unknown
  )
{
  DEBUG ((DEBUG_WARN, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_WARN, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

/**

  PngUnknownFunction05

  @retval EFI_SUCCESS
  @retval other

**/

EFI_STATUS
EFIAPI
PngUnknownFunction05 (
  VOID              *ImageBuffer,
  UINTN             ImageSize,
  EFI_UGA_PIXEL     **RawImageData,
  UINTN             *RawImageDataSize,
  UINTN             Unknown
  )
{
  DEBUG ((DEBUG_WARN, "%a: Started\n", __FUNCTION_NAME__));
  DEBUG ((DEBUG_WARN, "%a: Finished\n", __FUNCTION_NAME__));
  return EFI_SUCCESS;
}

/**

  PNG Image Codec Protocol Instance.

**/

APPLE_IMAGE_CODEC_PROTOCOL mAppleImagePngCodecProtocol = {

  // Version
  0x20000,

  // FileExt
  SIGNATURE_64 ('P', 'N', 'G', 0x0, 0x0, 0x0, 0x0, 0x0),

  // RecognizeImageData
  PngRecognizeImageData,

  // GetImageDims
  PngGetImageDims,

  // DecodeImageData
  PngDecodeImageData,

  // Unknown4
  PngUnknownFunction04,

  // Unknown5
  PngUnknownFunction05

};
