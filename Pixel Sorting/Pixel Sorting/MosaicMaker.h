#pragma once
#include "Bitmap.h"
#include <cstdint>
#include <vector>

class MosaicMaker
{
public:
    BitmapImage createMosaic(const BitmapImage& originalImage, const BitmapImage& referenceImage, int blockSize = 8);

private:
    struct BlockRecord
    {
        int blockX;
        int blockY;
        uint32_t averageColorSRGB;
    };

    uint32_t computeAverageBlockColor(const BitmapImage& image, int startPixelX, int startPixelY, int blockSize);

    void copyBlock(
        const BitmapImage& sourceImage,
        BitmapImage& destinationImage,
        int sourceStartPixelX,
        int sourceStartPixelY,
        int destinationStartPixelX,
        int destinationStartPixelY,
        int blockSize
    );

    uint32_t packRgb(uint8_t red, uint8_t green, uint8_t blue);
};
