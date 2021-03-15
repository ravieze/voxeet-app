#import <WebRTC/RTCMacros.h>
#import <Foundation/Foundation.h>

RTC_OBJC_EXPORT
@protocol MediaCryptoDelegate <NSObject>

- (NSInteger)encrypt:(NSInteger)type
                ssrc:(NSInteger)ssrc
               frame:(const UInt8*)frame
           frameSize:(NSInteger)frameSize
      encryptedFrame:(UInt8*)encryptedFrame
  encryptedFrameSize:(NSInteger)encryptedFrameSize;

- (NSInteger)getMaxCiphertextByteSize:(NSInteger)type size:(NSInteger)size;

- (NSInteger)decrypt:(NSInteger)type
                ssrc:(NSInteger)ssrc
      encryptedFrame:(const UInt8*)encryptedFrame
  encryptedFrameSize:(NSInteger)encryptedFrameSize
               frame:(UInt8*)frame
           frameSize:(NSInteger)frameSize;

- (NSInteger)getMaxPlaintextByteSize:(NSInteger)type size:(NSInteger)size;

@end
