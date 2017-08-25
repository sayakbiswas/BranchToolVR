#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "../library/imebra/include/image.h"

#ifdef __APPLE__
   #include "TargetConditionals.h"
#endif

/// \addtogroup group_objectivec
///
/// @{

#ifdef TARGET_OS_IPHONE

UIImage* getImage(puntoexe::ptr<puntoexe::imebra::image> image, puntoexe::ptr<puntoexe::imebra::transforms::transform> transforms);
#else
/// \brief Convert an Imebra image into an UIImage (on iOS) or NSImage (on OS-X) ready to
///        be rendered.
///
/// This method uses puntoexe::imebra::drawBitmap.
///
/// See \ref quick_tour_image_display for more information regarding the steps necessary
///  to produce an image ready to be displayed.
///
/// @param image     the Imebra image to be transformed into UIImage or NSImage
/// @param transform a transform or a collection of transforms to be applied to the
///                   image before it is converted into an UIImage or NSImage.
///                  Color transforms and bit shifting don't need to be specified
///                   and are applied directly by getImage().\n
///                  In this parameter you should specify the modality transform
///                   in the case you didn't use puntoexe::imebra::dataSet::getModalityImage()
///                   (which applies the modality transform for you) and the presentation
///                   transform (see puntoexe::imebra::transforms::VOILUT).\n\n
///                  To apply multiple transforms use puntoexe::imebra::transforms::transformsChain
///
NSImage* getImage(puntoexe::ptr<puntoexe::imebra::image> image, puntoexe::ptr<puntoexe::imebra::transforms::transform> transforms);
#endif

///@}
