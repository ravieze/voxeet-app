#ifndef MEDIA_ENGINE_IOS_MEDIA_STREAM
#define MEDIA_ENGINE_IOS_MEDIA_STREAM

#import <Foundation/Foundation.h>
#import <WebRTC/RTCMacros.h>
#import "AudioTrack.h"
#import "VideoTrack.h"

typedef NS_ENUM(NSUInteger, MediaStreamType) {
    Camera = 0,
    ScreenShare = 1,
    Custom = 2
};

RTC_OBJC_EXPORT
@interface MediaStream: NSObject

/** An identifier for this media stream. */
@property(nonatomic, readonly) NSString* streamId;

/** Current stream type. */
@property(nonatomic, readonly) MediaStreamType type;

/** The audio tracks in this stream. */
@property(nonatomic, strong) NSArray<AudioTrack*>* audioTracks;

/** The video tracks in this stream. */
@property(nonatomic, strong, readonly) NSArray<VideoTrack*>* videoTracks;

- (instancetype)initWithType:(MediaStreamType)type;

@end

#endif // MEDIA_ENGINE_IOS_MEDIA_STREAM
