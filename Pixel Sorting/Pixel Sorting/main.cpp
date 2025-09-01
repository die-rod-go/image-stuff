#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include <conio.h>
#include "Bitmap.h"
#include "MosaicMaker.h"
#include "RandomStuff.h"

int main() {


    //  image that's donating pixels
    BitmapImage image1;
    if (!image1.load("PUT IMAGE PATH HERE")) 
    {
        std::cerr << "failed to load bmp1\n";
        return 1;
    }

    // image that will be recreated with those pixels
    BitmapImage image2;
    if (!image2.load("PUT IMAGE PATH HERE")) 
    {
        std::cerr << "failed to load bmp2\n";
        return 1;
    }

    // make output
    MosaicMaker mosaicMaker;
    // i didn't go over this in the post but you can also grab and rearrange "blocks" of pixels
    // instead of individual ones. so you can created cut up mosaics and stuff. it's hard to explain but
    // 
    // change this number                                        |
    // and you'll see what I mean                                v
    BitmapImage image = mosaicMaker.createMosaic(image1, image2, 1);
    image.save("PUT OUTPUT PATH AND NAME HERE");
}
