#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#pragma pack(push, 1)  // pack the structs cause bmp is like that
struct BitmapFileHeader 
{
    uint16_t type;         // 'BM' (0x4D42) for bmp
    uint32_t fileSize;     // size in bytes
    uint16_t reserved1;    // unused
    uint16_t reserved2;    // unused
    uint32_t pixelDataOffset; // offset in file where pixel data begins
};

struct BitmapInfoHeader 
{
    uint32_t headerSize;
    int32_t  width;            
    int32_t  height;           // positive = bottom-up
    uint16_t colorPlanes;      // must be 1
    uint16_t bitsPerPixel;     // 24
    uint32_t compression;      // 0 (no compression)
    uint32_t imageSize;        // size of pixel data (may be 0 for uncompressed)
    int32_t  horizontalResolution; // pixels per meter
    int32_t  verticalResolution;   // pixels per meter
    uint32_t colorsInPalette;      // 0 = default
    uint32_t importantColors;      // 0 = all
};
#pragma pack(pop)

struct Pixel 
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};

class BitmapImage 
{
public:
    int getWidth() const
    {
        return width;
    }

    int getHeight() const
    {
        return height;
    }

    bool load(const std::string& filePath) 
    {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) return false;

        BitmapFileHeader fileHeader;
        BitmapInfoHeader infoHeader;

        // read headers
        file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

        // check that it’s really a bmp and that it’s 24-bit with no compression
        if (fileHeader.type != 0x4D42 || infoHeader.bitsPerPixel != 24 || infoHeader.compression != 0) 
        {
            std::cerr << "Invalid file: " + filePath + "\n";
            return false;
        }

        width = infoHeader.width;
        height = infoHeader.height;

        int bytesPerRow = width * 3;                  // mult by 3 for red green blue
        rowSize = ((bytesPerRow + 3) / 4) * 4;    // calc closest 4 byte boundary

        // allocate storage for all rows including padding
        rawData.resize(rowSize * height);

        // jump to the pixel data in the file and read it all
        file.seekg(fileHeader.pixelDataOffset, std::ios::beg);
        file.read(reinterpret_cast<char*>(rawData.data()), rawData.size());
        return true;
    }

    Pixel getPixel(int x, int y) const
    {
        int rowOffset = (height - 1 - y) * rowSize; // flip y cause bmp is like that
        int pixelOffset = rowOffset + x * 3;

        return 
        {
            rawData[pixelOffset + 0], // blue
            rawData[pixelOffset + 1], // green
            rawData[pixelOffset + 2]  // red
        };
    }


    void setPixel(int x, int y, const Pixel& pixel) 
    {
        int rowOffset = (height - 1 - y) * rowSize; // flip y cause bmp is like that
        int pixelOffset = rowOffset + x * 3;

        rawData[pixelOffset + 0] = pixel.blue;
        rawData[pixelOffset + 1] = pixel.green;
        rawData[pixelOffset + 2] = pixel.red;
    }

    bool save(const std::string& filePath) 
    {
        std::ofstream file(filePath, std::ios::binary);
        if (!file) return false;

        BitmapFileHeader fileHeader{};
        BitmapInfoHeader infoHeader{};

        // file header
        fileHeader.type = 0x4D42; // 'BM'
        fileHeader.pixelDataOffset = sizeof(fileHeader) + sizeof(infoHeader);
        fileHeader.fileSize = fileHeader.pixelDataOffset + rawData.size();

        // info header
        infoHeader.headerSize = sizeof(infoHeader);
        infoHeader.width = width;
        infoHeader.height = height;
        infoHeader.colorPlanes = 1;
        infoHeader.bitsPerPixel = 24;
        infoHeader.compression = 0;
        infoHeader.imageSize = rawData.size();

        // write everything back out
        file.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
        file.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));
        file.write(reinterpret_cast<char*>(rawData.data()), rawData.size());
        file.close();
        return true;
    }

    bool create(int w, int h, const Pixel& fill = { 0,0,0 })
    {
        if (w <= 0 || h <= 0) return false;

        width = w;
        height = h;

        int bytesPerRow = width * 3;                  // mult by 3 for red green blue
        rowSize = ((bytesPerRow + 3) / 4) * 4;    // calc closest 4 byte boundary

        // allocate storage for all rows including padding
        rawData.resize(rowSize * height);

        // fill pixels with the given color
        for (int y = 0; y < height; y++) 
        {
            for (int x = 0; x < width; x++) 
            {
                setPixel(x, y, fill);
            }
        }

        return true;
    }

private:
    std::vector<uint8_t> rawData; // raw bytes of pixel rows (with padding at row ends)
    int rowSize = 0;
    int width = 0;
    int height = 0;
};