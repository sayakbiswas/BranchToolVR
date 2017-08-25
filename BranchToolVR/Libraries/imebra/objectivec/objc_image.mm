#include "../library/imebra/include/imebra.h"
#import "objc_image.h"

void CGDataProviderCallbackFunc(void *info, const void *data, size_t size)
{
    // Release the shared pointer holding the memory
    ////////////////////////////////////////////////
    delete (puntoexe::ptr<puntoexe::memory>*)info;
}

#ifdef TARGET_OS_IPHONE
UIImage* getImage(puntoexe::ptr<puntoexe::imebra::image> image, puntoexe::ptr<puntoexe::imebra::transforms::transform> transforms)
#else
NSImage* getImage(puntoexe::ptr<puntoexe::imebra::image> image, puntoexe::ptr<puntoexe::imebra::transforms::transform> transforms)
#endif
{
    // Allocate the drawBitmap class
    ////////////////////////////////
    puntoexe::imebra::drawBitmap drawBitmap(image, transforms);

    // Get the amount of memory needed for the conversion
    /////////////////////////////////////////////////////
    imbxUint32 width, height;
    image->getSize(&width, &height);
    size_t memorySize = drawBitmap.getBitmap<puntoexe::imebra::drawBitmapRGBA, 4>(width, height, 0, 0, width, height, 0, 0);

    // Allocate the memory that will hold the result
    ////////////////////////////////////////////////
    puntoexe::ptr<puntoexe::memory> memory(puntoexe::memoryPool::getMemoryPool()->getMemory(memorySize));

    // Get the result raw data
    //////////////////////////
    drawBitmap.getBitmap<puntoexe::imebra::drawBitmapRGBA, 4>(width, height, 0, 0, width, height, memory->data(), memorySize);


    // Create a CGImage, then convert it to NSImage or UIImage
    //////////////////////////////////////////////////////////
    puntoexe::ptr<puntoexe::memory> * pCGDataMemory = new puntoexe::ptr<puntoexe::memory>(memory);
    CGDataProviderRef dataProviderRef = CGDataProviderCreateWithData(pCGDataMemory,
                                                                memory->data(),
                                                                width * height * 4,
                                                                CGDataProviderCallbackFunc);

    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrder32Big | kCGImageAlphaPremultipliedLast;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;

    CGImageRef imageRef = CGImageCreate(
                width, height,
                8, 32,
                width * 4,
                colorSpaceRef, bitmapInfo, dataProviderRef, NULL, YES, renderingIntent);


#ifdef TARGET_OS_IPHONE
    UIImage* returnImage = [[UIImage alloc] initWithCGImage:imageRef];
#else
    NSImage* returnImage = [[NSImage alloc] initWithCGImage:imageRef];
#endif
    CGDataProviderRelease(dataProviderRef);
    CGImageRelease(imageRef);
    CGColorSpaceRelease(colorSpaceRef);
    return returnImage;
}
