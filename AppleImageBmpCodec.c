/*++

 Created by HermitCrabs on 11/8/2014.
 Copyright 2014-2015 The HermitCrab Labs. All rights reserved.

Module Name:

  AppleImageBmpCodec.c

Abstract:

  Implement AppleImageCodec Protocol For BMP Images

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

#include <IndustryStandard/Bmp.h>

#include "Macros.h"
#include "PicoBmp.h"

/**

  BmpRecognizeImageData

  @param[in]  ImageBuffer
  @param[in]  ImageSize

  @retval EFI_SUCCESS
  @retval EFI_UNSUPPORTED
  @retval EFI_INVALID_PARAMETER

**/

EFI_STATUS
EFIAPI
BmpRecognizeImageData (
  VOID              *ImageBuffer,
  UINTN             ImageSize
  )
{
  BMP_IMAGE_HEADER    *Bitmap = (BMP_IMAGE_HEADER *)ImageBuffer;

  // Check Parameters

  if (ImageSize == 0 || ImageBuffer == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if (Bitmap->CharB != 'B' || Bitmap->CharM != 'M')
  {
    return EFI_UNSUPPORTED;
  }

  // 0 - No Compression
  // 1 - RLE8
  // 2 - RLE4
  // 3 - BITFIELDS

  if (Bitmap->CompressionType != 0) {
    //return EFI_UNSUPPORTED;
  }

  if (Bitmap->BitPerPixel != 8 && Bitmap->BitPerPixel != 32)
  {
    //return EFI_UNSUPPORTED;
  }

  //Bitmap->PixelWidth = 24;
  //Bitmap->PixelHeight = 24;

  return EFI_SUCCESS;

}

/**

  BmpGetImageDims

  @param[in]  ImageBuffer
  @param[in]  ImageSize
  @param[out] ImageWidth
  @param[out] ImageHeight

  @retval EFI_SUCCESS
  @retval EFI_UNSUPPORTED
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES

**/

EFI_STATUS
EFIAPI
BmpGetImageDims (
  VOID              *ImageBuffer,
  UINTN             ImageSize,
  UINT32            *ImageWidth,
  UINT32            *ImageHeight
  )
{
  BMP_IMAGE_HEADER    *Bitmap = (BMP_IMAGE_HEADER *)ImageBuffer;
  EFI_STATUS    Status;

  // Validate ImageBuffer

  Status = BmpRecognizeImageData (ImageBuffer, ImageSize);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Check Parameters

  if (ImageWidth == NULL || ImageHeight == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  *ImageWidth  = Bitmap->PixelWidth < 0 ? -Bitmap->PixelWidth : Bitmap->PixelWidth;
  *ImageHeight = Bitmap->PixelHeight < 0 ? -Bitmap->PixelHeight : Bitmap->PixelHeight;

  DEBUG ((DEBUG_INFO, "BMP Image %d x %d %d Bit\n",
                      *ImageWidth,
                      *ImageHeight,
                      Bitmap->BitPerPixel));

  return Status;

}

/**

  BmpDecodeImageData

  @param[in]  ImageBuffer
  @param[in]  ImageSize
  @param[out] RawImageData
  @param[out] RawImageDataSize

  @retval EFI_SUCCESS
  @retval EFI_UNSUPPORTED
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES

**/

EFI_STATUS
EFIAPI
BmpDecodeImageData (
  VOID              *ImageBuffer,
  UINTN             ImageSize,
  EFI_UGA_PIXEL     **RawImageData,
  UINTN             *RawImageDataSize
  )
{
  EFI_STATUS    Status;

  // Validate ImageBuffer

  Status = BmpRecognizeImageData (ImageBuffer, ImageSize);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  // Check Parameters

  if (RawImageDataSize == NULL || RawImageData == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  // Decode BMP Inverting Alpha

  return BmpDecode (ImageBuffer, ImageSize, TRUE, (VOID **)RawImageData, RawImageDataSize);
}

/**

  @param[in]  ImageBuffer
  @param[in]  ImageSize
  @param[out] ImageWidth
  @param[out] ImageHeight

  @retval EFI_SUCCESS
  @retval EFI_UNSUPPORTED
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES

**/

EFI_STATUS
EFIAPI
BmpUnknownFunction04 (
  VOID              *ImageBuffer,
  UINTN             ImageSize,
  UINT32            *ImageWidth,
  UINT32            *ImageHeight,
  UINTN             Unknown
  )
{
  DEBUG ((DEBUG_WARN, "%a: Started\n", __FUNCTION_NAME__));

  return BmpGetImageDims (ImageBuffer,
                          ImageSize,
                          ImageWidth,
                          ImageHeight);
}

/**

  @param[in]  ImageBuffer
  @param[in]  ImageSize
  @param[out] RawImageData
  @param[out] RawImageDataSize
  @param

  @retval EFI_SUCCESS
  @retval EFI_UNSUPPORTED
  @retval EFI_INVALID_PARAMETER
  @retval EFI_OUT_OF_RESOURCES

**/

EFI_STATUS
EFIAPI
BmpUnknownFunction05 (
  VOID              *ImageBuffer,
  UINTN             ImageSize,
  EFI_UGA_PIXEL     **RawImageData,
  UINTN             *RawImageDataSize,
  UINTN             Unknown
  )
{
  DEBUG ((DEBUG_WARN, "%a: Started\n", __FUNCTION_NAME__));

  return BmpDecodeImageData (ImageBuffer,
                             ImageSize,
                             RawImageData,
                             RawImageDataSize);
}

/**

  BMP Image Codec Protocol Instance.

**/

APPLE_IMAGE_CODEC_PROTOCOL mAppleImageBmpCodecProtocol = {

  // Version
  0x20000,

  // FileExt
  SIGNATURE_64 ('B', 'M', 'P', 0x0, 0x0, 0x0, 0x0, 0x0),

  // RecognizeImageData
  BmpRecognizeImageData,

  // GetImageDims
  BmpGetImageDims,

  // DecodeImageData
  BmpDecodeImageData,

  // Unknown4
  BmpUnknownFunction04,

  // Unknown5
  BmpUnknownFunction05

};
