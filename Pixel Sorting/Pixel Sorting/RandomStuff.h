#pragma once
#include "Bitmap.h"

float getPixelBrightness(Pixel pixel)
{
    return (pixel.red + pixel.green + pixel.blue) / 3.0f;
}

void nudgeTowardsBrighterPixel(int x, int y, BitmapImage& image)
{
    Pixel currentPixel = image.getPixel(x, y);
    float currentBrightness = getPixelBrightness(currentPixel);

    int brightestX = x;
    int brightestY = y;
    float maxBrightness = currentBrightness;

    // compute 3×3 bounds
    int minX = (x > 0) ? x - 1 : x;
    int maxX = (x < image.getWidth() - 1) ? x + 1 : x;
    int minY = (y > 0) ? y - 1 : y;
    int maxY = (y < image.getHeight() - 1) ? y + 1 : y;

    // scan neighbors and find brightest
    for (int currY = minY; currY <= maxY; currY++) {
        for (int currX = minX; currX <= maxX; currX++) {
            Pixel neighbor = image.getPixel(currX, currY);
            float neighborBrightness = getPixelBrightness(neighbor);

            if (neighborBrightness > maxBrightness) {
                maxBrightness = neighborBrightness;
                brightestX = currX;
                brightestY = currY;
            }
        }
    }

    // swap current pixel with brightest found pixel
    if (brightestX != x || brightestY != y) {
        Pixel brightestPixel = image.getPixel(brightestX, brightestY);
        image.setPixel(brightestX, brightestY, currentPixel);
        image.setPixel(x, y, brightestPixel);
    }
}

void nudgeTowardsDimmerPixel(int x, int y, BitmapImage& image)
{
    Pixel currentPixel = image.getPixel(x, y);
    float currentBrightness = getPixelBrightness(currentPixel);

    int brightestX = x;
    int brightestY = y;
    float minBrightness = currentBrightness;

    // compute 3×3 bounds
    int minX = (x > 0) ? x - 1 : x;
    int maxX = (x < image.getWidth() - 1) ? x + 1 : x;
    int minY = (y > 0) ? y - 1 : y;
    int maxY = (y < image.getHeight() - 1) ? y + 1 : y;

    // scan neighbors and find dimmest
    for (int currY = minY; currY <= maxY; currY++) {
        for (int currX = minX; currX <= maxX; currX++) {
            Pixel neighbor = image.getPixel(currX, currY);
            float neighborBrightness = getPixelBrightness(neighbor);

            if (neighborBrightness < minBrightness) {
                minBrightness = neighborBrightness;
                brightestX = currX;
                brightestY = currY;
            }
        }
    }

    // swap current pixel with dimmest found pixel
    if (brightestX != x || brightestY != y) {
        Pixel brightestPixel = image.getPixel(brightestX, brightestY);
        image.setPixel(brightestX, brightestY, currentPixel);
        image.setPixel(x, y, brightestPixel);
    }
}

//  stupid function should probably delete lol
std::string generateProgressBar(float percentFilled, int charWidth)
{
    std::string output = "";
    output += "|";
    for (int i = 0; i < charWidth; i++)
    {
        if (i <= percentFilled * charWidth)
            output += "#";
        else
            output += " ";
    }
    output += "|";

    return output;
}