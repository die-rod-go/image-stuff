#include "MosaicMaker.h"
#include <algorithm>
#include <cstdint>
#include <time.h>

uint32_t MosaicMaker::packRgb(uint8_t red, uint8_t green, uint8_t blue)
{
    uint32_t hex = (red << 16) | (green << 8) | (blue);
    return	hex;
}

BitmapImage MosaicMaker::createMosaic(const BitmapImage& originalImage, const BitmapImage& referenceImage, int blockSize)
{
    if (blockSize <= 0)
        return BitmapImage{};

    // find common area that is divisible by blockSize
    int commonWidth = std::min(originalImage.getWidth(), referenceImage.getWidth());
    int commonHeight = std::min(originalImage.getHeight(), referenceImage.getHeight());
    int blocksAcross = commonWidth / blockSize;
    int blocksDown = commonHeight / blockSize;
    int mosaicWidth = blocksAcross * blockSize;
    int mosaicHeight = blocksDown * blockSize;

    // create output image
    BitmapImage mosaicImage;
    mosaicImage.create(mosaicWidth, mosaicHeight);

    std::vector<BlockRecord> originalBlocks;
    std::vector<BlockRecord> referenceBlocks;

    for (int blockY = 0; blockY < blocksDown; blockY++) 
    {
        for (int blockX = 0; blockX < blocksAcross; blockX++) 
        {
            int startPixelX = blockX * blockSize;
            int startPixelY = blockY * blockSize;

            BlockRecord originalRecord;
            originalRecord.blockX = blockX;
            originalRecord.blockY = blockY;
            originalRecord.averageColorSRGB = computeAverageBlockColor(originalImage, startPixelX, startPixelY, blockSize);
            originalBlocks.push_back(originalRecord);

            BlockRecord referenceRecord;
            referenceRecord.blockX = blockX;
            referenceRecord.blockY = blockY;
            referenceRecord.averageColorSRGB = computeAverageBlockColor(referenceImage, startPixelX, startPixelY, blockSize);
            referenceBlocks.push_back(referenceRecord);
        }
    }

    // sort both sets of blocks by their average color (sorta. rn this is a rough approximation)
    auto cmp = [](const BlockRecord& a, const BlockRecord& b) {
        return a.averageColorSRGB < b.averageColorSRGB;
    };
    std::sort(originalBlocks.begin(), originalBlocks.end(), cmp);
    std::sort(referenceBlocks.begin(), referenceBlocks.end(), cmp);

    // map blocks in original to blocks in reference and copy to new image
    int numberOfBlocks = std::min(originalBlocks.size(), referenceBlocks.size());
    for (int i = 0; i < numberOfBlocks; i++) 
    {
        //  grabbing from same index of sorted list gives blocks that most closely match in average color
        BlockRecord& originalBlock = originalBlocks[i];
        BlockRecord& referenceBlock = referenceBlocks[i];

        // convert block coordinates back into pixel coordinates
        int destinationStartPixelX = referenceBlock.blockX * blockSize;
        int destinationStartPixelY = referenceBlock.blockY * blockSize;
        int sourceStartPixelX = originalBlock.blockX * blockSize;
        int sourceStartPixelY = originalBlock.blockY * blockSize;

        copyBlock(originalImage, mosaicImage, sourceStartPixelX, sourceStartPixelY, destinationStartPixelX, destinationStartPixelY, blockSize);
    }

    return mosaicImage;
}

uint32_t MosaicMaker::computeAverageBlockColor(const BitmapImage& image, int startPixelX, int startPixelY, int blockSize)
{
    // clamp to image bounds
    int endPixelX = std::min(image.getWidth(), startPixelX + blockSize);
    int endPixelY = std::min(image.getHeight(), startPixelY + blockSize);

    std::uint64_t sumRed = 0;
    std::uint64_t sumGreen = 0;
    std::uint64_t sumBlue = 0;
    std::uint64_t sampleCount = 0;

    for (int y = startPixelY; y < endPixelY; y++) 
    {
        for (int x = startPixelX; x < endPixelX; x++) 
        {
            Pixel pixel = image.getPixel(x, y);
            sumBlue += pixel.blue;
            sumGreen += pixel.green;
            sumRed += pixel.red;
            sampleCount++;
        }
    }

    // don't really know when this would happen but ill put it here just in case lol
    if (sampleCount == 0) 
        return 0;

    // calc averages and return 24 bit integer representation
    uint8_t averageRed = sumRed / sampleCount;
    uint8_t averageGreen = sumGreen / sampleCount;
    uint8_t averageBlue = sumBlue / sampleCount;
    return packRgb(averageRed, averageGreen, averageBlue);
}

void MosaicMaker::copyBlock(
    const BitmapImage& sourceImage,
    BitmapImage& destinationImage,
    int sourceStartPixelX,
    int sourceStartPixelY,
    int destinationStartPixelX,
    int destinationStartPixelY,
    int blockSize
)
{
    // clamp to image edge
    int sourceEndPixelX = std::min(sourceImage.getWidth(), sourceStartPixelX + blockSize);
    int sourceEndPixelY = std::min(sourceImage.getHeight(), sourceStartPixelY + blockSize);

    // loop through block and copy pixels
    for (int yOffset = 0; yOffset < (sourceEndPixelY - sourceStartPixelY); yOffset++)
    {
        for (int xOffset = 0; xOffset < (sourceEndPixelX - sourceStartPixelX); xOffset++)
        {
            Pixel pixel = sourceImage.getPixel(sourceStartPixelX + xOffset, sourceStartPixelY + yOffset);
            destinationImage.setPixel(destinationStartPixelX + xOffset, destinationStartPixelY + yOffset, pixel);
        }
    }
}
