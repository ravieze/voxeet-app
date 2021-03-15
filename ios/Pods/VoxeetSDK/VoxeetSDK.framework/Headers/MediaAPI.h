//
//  MediaAPI.h
//  VoxeetMedia
//
//  Created by Gilles Bordas on 09/01/2014.
//  Copyright (c) 2014 Voxeet. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <WebRTC/WebRTC.h>
#import "SdpMessage.h"
#import "SdpDescription.h"
#import "SdpCandidates.h"
#import "PendingPeerOperation.h"

@interface MediaAPI: NSObject

@property (strong, nonatomic) NSMutableDictionary *pendingOperations;
@property (strong, nonatomic) SdpCandidates *peerCandidates;
@property (strong, nonatomic) MediaEngine *wrapper;
//@property (copy, nonatomic) void(^audioRouteChangedBlock)(NSNumber *);

- (id)initWithLocalPeer:(NSString *)localPeerId video:(BOOL)video microphone:(BOOL)microphone constraints:(NSDictionary<NSString*, NSString*>*)constraints;
- (void)stop;
- (SdpMessage *)createOfferForPeer:(NSString *)peerId isMaster:(BOOL)isMaster;
- (SdpMessage *)createAnswerForPeer:(NSString *)peerId withSSRC:(UInt32)ssrc offer:(SdpDescription *)offer andCandidates:(NSArray *)candidates isMaster:(BOOL)isMaster completionHandler:(void (^)(NSError *error))completionHandler;
- (void)unlockPeerOperation:(PendingPeerOperationType)type forPeer:(NSString *)peerId withObject:(id)object;

@end
