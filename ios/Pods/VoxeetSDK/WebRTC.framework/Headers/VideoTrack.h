#ifndef MEDIA_STREAM_IOS_VIDEO_TRACK
#define MEDIA_STREAM_IOS_VIDEO_TRACK

#import <Foundation/Foundation.h>
#import <WebRTC/RTCMacros.h>

RTC_OBJC_EXPORT
@interface VideoTrack : NSObject

@property(nonatomic, readonly) NSString *trackId;

@end

#endif // MEDIA_STREAM_IOS_VIDEO_TRACK
