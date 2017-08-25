#import <Foundation/Foundation.h>
#include <string>

/// \addtogroup group_objectivec ObjectiveC helpers
///
/// The ObjectiveC helper methods provide few functionalities that
///  are useful when compiling Imebra for OS-X or iOS.
///
/// In particular they provide translation between NSString and
/// std::wstring and generation of UIImage or NSImage objects from
/// Imebra images.
///
/// @{

/**
 * @brief Convert an NSString to a std::wstring accepted by imebra.
 * @param str an NSString
 * @return a std::wstring
 */
std::wstring NSStringToStringW ( NSString* str );


/**
 * @brief Convert a std::wstring to an NSString.
 * @param str an std::wstring
 * @return a NSString
 */
NSString* StringWToNSString ( const std::wstring& str );

///@}
