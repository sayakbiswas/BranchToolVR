#import "objc_helpers.h"

std::wstring NSStringToStringW ( NSString* str )
{
    NSStringEncoding pEncode    =   CFStringConvertEncodingToNSStringEncoding ( kCFStringEncodingUTF32LE );   
    NSData* pSData              =   [ str dataUsingEncoding : pEncode ];    
    return std::wstring ( (wchar_t*) [ pSData bytes ], [ pSData length] / sizeof ( wchar_t ) );   
}


NSString* StringWToNSString ( const std::wstring& str )
{
    NSString* pString = [ [ NSString alloc ]    
                        initWithBytes : (char*)str.data()   
                               length : str.size() * sizeof(wchar_t)
                             encoding : CFStringConvertEncodingToNSStringEncoding ( kCFStringEncodingUTF32LE ) ];   
    return pString;   
}
