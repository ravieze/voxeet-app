//
//  MediaEngine.h
//  MediaEngine
//
//  Created by Thomas Gourgues on 21/08/12.
//  Copyright (c) 2012-2018 Thomas Gourgues. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "MediaEngineDelegate.h"
#import "MediaCryptoDelegate.h"
#import "WebRTC/RTCVideoRenderer.h"
#import "WebRTC/RTCAudioSession.h"
#import "WebRTC/RTCAudioSessionConfiguration.h"
#import "WebRTC/RTCDispatcher.h"
#import "WebRTC/RTCVideoFrame.h"

RTC_OBJC_EXPORT
@interface MediaEngine: NSObject

@property(nonatomic, assign) id <MediaEngineDelegate> delegate;
@property(nonatomic, assign) id <MediaCryptoDelegate> cryptoDelegate;

- (id)initWithLocalPeer:(NSString*)localPeerId video:(BOOL)video microphone:(BOOL)microphone constraints:(NSDictionary<NSString*, NSString*>*)constraints;
- (void)attachMediaStream:(id<RTCVideoRenderer>)renderer withStream:(MediaStream*)stream;
- (void)unattachMediaStream:(id<RTCVideoRenderer>)renderer withStream:(MediaStream*)stream;
- (MediaStream*)getMediaStream:(NSString*)peerId;
- (MediaStream*)getScreenShareStream;
- (BOOL)needSwitchToPstn;
- (NSError*)createConnectionWithPeer:(NSString*)peerId isMaster:(BOOL)isMaster;
- (BOOL)closeConnectionWithPeer:(NSString*)peerId;
- (NSError*)createAnswerForPeer:(NSString*)peerId;
- (NSError*)setDescriptionForPeer:(NSString*)peerId withSsrc:(long)ssrc type:(NSString*)type andSdp:(NSString*)sdp;
- (NSError*)setCandidateForPeer:(NSString*)peerId withsdpMid:(NSString*)sdpMid sdpIndex:(NSInteger)sdpMLineIndex andSdp:(NSString *)sdp;
- (BOOL)setPositionForPeer:(NSString*)peerId withAngle:(double)angle andPosition:(double)position;
- (BOOL)setPositionForPeer:(NSString*)peerId withAngle:(double)angle position:(double)position andGain:(float)gain;
- (BOOL)setGain:(float)gain forPeer:(NSString*)peerId;
- (void)setMute:(BOOL)mute;
- (void)setMuteOutput:(BOOL)mute;
- (NSDictionary*)getAlarmsStatus;
- (NSData*)createClientDump;
- (void)releaseClientDump;
- (void)setListener:(BOOL)isListener;
- (void)setTransparentMode:(BOOL)transparentMode;
- (void)setPeerMute:(NSString*)peerId mute:(BOOL)mute;
- (void)setAudioOptions:(BOOL)ns agc:(BOOL)agc ec:(BOOL)ec typingDetection:(BOOL)typingDetection;
- (NSString*)getLocalStats:(NSString*)peerId;
- (double)getLocalVuMeter;
- (double)getVuMeterForPeer:(NSString*)peerId;
- (double)getLocalAudioLevel;
- (NSDictionary<NSString*, NSNumber*>*)getAudioLevels;
- (float)getAudioLevelWithUserId:(NSString*)userId conferenceId:(NSString*)conferenceId;
- (void)setLoudSpeakerStatus:(BOOL)isEnable;
- (BOOL)isLoudSpeakerActive;
- (void)setHardwareAec:(BOOL)isHardwareAEC;
- (BOOL)isHardwareAec;
- (void)stop;
- (void)dealloc;
- (void)startAudio;
- (void)stopAudio;
- (void)startVideoWithCompletion:(void (^)(NSError*))completionHandler;
- (void)startVideoWithCompletion:(void (^)(NSError*))completionHandler defaultFrontFacing:(BOOL)isDefaultFrontFacing;
- (void)flipCameraWithCompletion:(void (^)(NSError*))completionHandler;
- (void)stopVideoWithCompletion:(void (^)(void))completionHandler;
- (void)adaptOutputFormatWithWidth:(int)width height:(int)height fps:(int)fps;
- (void)startScreenShare:(BOOL)broadcast completionHandler:(void (^)(NSError*))completionHandler;
- (void)updateScreenShare:(CMSampleBufferRef)sampleBuffer withRotation:(RTCVideoRotation)rotation;
- (void)stopScreenShareWithCompletion:(void (^)(NSError*))completionHandler;
- (void)resetAudioDevices;

@end
