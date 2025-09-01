#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <conio.h>
#include "Bitmap.h"
#include "ImageRecreator.h"
#include "MosaicMaker.h"
#include "RandomStuff.h"

int main() {


    BitmapImage image1;
    if (!image1.load("C:\\Users\\diego\\Desktop\\photos\\datamosh\\brightness sorting\\input\\cloud1.bmp")) 
    {
        std::cerr << "failed to load bmp1\n";
        return 1;
    }

    BitmapImage image2;
    if (!image2.load("C:\\Users\\diego\\Desktop\\photos\\datamosh\\brightness sorting\\input\\van_level.bmp")) 
    {
        std::cerr << "failed to load bmp2\n";
        return 1;
    }

    MosaicMaker mosaicMaker;
    BitmapImage image = mosaicMaker.createMosaic(image1, image2, 1);

    image.save("C:\\Users\\diego\\Desktop\\photos\\datamosh\\brightness sorting\\output\\poopy.bmp");
}
