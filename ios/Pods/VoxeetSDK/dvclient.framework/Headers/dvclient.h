/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2012-2013 by Dolby Laboratories.
 *                Copyright (C) 2014-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DVCLIENT_H_
#define DVCLIENT_H_

#include <time.h>

/** @file
 * Dolby Voice Client C interface.
 */


/**
 * @defgroup DVC_Functions Dolby Voice Client C API functions
 * @defgroup DVC_Errors Dolby Voice Client C API error codes
 * @defgroup DVC_StructureEnum Dolby Voice Client C API enum and structures
 */


#define DVC_VERSION_MAJOR 3
#define DVC_VERSION_MINOR 2
#define DVC_VERSION_PATCH 0


/**
 * Header version in X.Y.Z format.
 * - X is the major version.
 * - Y is the minor version.
 * - Z is the patch version.
 *
 * @ingroup DVC_StructureEnum
 */
#define DVC_VERSION "3.2.0"


#ifdef DVC_STATIC_API
	#define DVC_API
#else
	#ifdef DVC_BUILDING_API
		#ifdef _WIN32
			#define DVC_API __declspec(dllexport)
		#elif defined(__GNUC__)
			#define DVC_API __attribute__((visibility("default")))
		#else
			#define DVC_API
		#endif
	#else
		#define DVC_API
	#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif


#define DVC_MAX_OUTPUT_CHANNELS (6)
#define DVC_DEVICE_GUID_SIZE    (24)
#define DVC_MAX_NAME_SIZE       (255)
#define DVC_MAX_PATH_SIZE       (1023)
#define DVC_MAX_CLIENT_ID_SIZE  (256)
#define DVC_MAX_NATIVE_ID_SIZE  (1023)
#define DVC_MAX_REPORT_BLOCKS   (32)


/**
 * Handle used to reference a client instance created with DVC_CreateClient().
 *
 * @see DVC_CreateClient() function.
 * @see DVC_DestroyClient() function.
 * @ingroup DVC_StructureEnum
 */
typedef unsigned int DVC_Handle;


/**
 * Handle used as reference to invalid client.
 *
 * @ingroup DVC_StructureEnum
 */
#define DVC_INVALID_HANDLE 0

/**
 * Handle used to reference a loaded wave file instance created with DVC_OpenWaveFile().
 *
 * @see DVC_OpenWaveFile() function.
 * @see DVC_PlayWave() function.
 * @see DVC_StopWave() function.
 * @see DVC_CloseWave() function.
 * @ingroup DVC_StructureEnum
 */
typedef unsigned int DVC_WaveHandle;


/**
 * Event handle definition.
 *
 * @note In Windows this handle can be cast to and used as a Windows API HANDLE and can be waited on using the Windows
 * functions WaitForSingleObject() or WaitForMultipleObjects().\n
 * In Linux/MacOS X/iOS and Android, this handle can be cast to and used as a file descriptor and can be waited on
 * using select().
 *
 * @see DVC_DoCallbacks() function.
 * @see DVC_GetEventHandle() function.
 * @ingroup DVC_StructureEnum
 */
#ifdef WIN32
typedef void* DVC_EventHandle;
#else
typedef int DVC_EventHandle;
#endif


/**
 * The unique id used by the media server to identify a client connection. The id is derived by media server using data
 * in RTP packets. @DVC_ClientId is always a null-terminated string; an empty string indicates an invalid client ID.
 *
 * @see DVC_GetTalkingLevels()
 * @ingroup DVC_StructureEnum
 */
typedef char DVC_ClientId[DVC_MAX_CLIENT_ID_SIZE + 1];


/**
 * DVC library error codes.
 *
 * Most DVC functions return one of these error codes to report the API call's status. DVC functions should never return
 * an error value greater than \ref DVC_ERROR_UNKNOWN and such event should be reported to Dolby support.
 *
 * @note Use the DVC_ErrorToString() function to translate an error code into a human readable string.
 *
 * @see DVC_ErrorToString() function.
 * @anchor DVC_Error
 * @ingroup DVC_Errors
 */
typedef enum _DVC_Error
{
	DVC_ERROR_NONE                                          = 0  /**< No error.*/
	, DVC_ERROR_NOT_INITIALIZED                             = 1  /**< Library is not initialized.*/
	, DVC_ERROR_UNABLE_TO_CREATE_LOGGER                     = 2  /**< Unable to create logger.*/
	, DVC_ERROR_INCOMPATIBLE_API_VERSION                    = 3  /**< Dynamic client library is incompatible with the
	                                                                  library interface application was compiled with.*/
	, DVC_ERROR_NULL_PARAM                                  = 4  /**< Mandatory parameter is \p NULL.*/
	, DVC_ERROR_INVALID_PARAM                               = 5  /**< Parameter has invalid value.*/
	, DVC_ERROR_MISMATCHED_PARAMS                           = 6  /**< The combination of parameters is invalid.*/
	, DVC_ERROR_INVALID_HANDLE                              = 7  /**< Handle is invalid.*/
	, DVC_ERROR_INVALID_STATE                               = 8  /**< Component is not in the required state
	                                                                  for the specified action to happen.*/
	, DVC_ERROR_DEVICE_NOT_AVAILABLE                        = 9  /**< Device does not exist or is not accessible.*/
	, DVC_ERROR_RESOURCE_ALLOCATION_FAILED                  = 10 /**< Failed to allocate required resources.*/
	, DVC_ERROR_SIZE_TOO_SMALL                              = 11 /**< Size of the provided array is too small.*/
	, DVC_ERROR_FILE_OPEN_FAILED                            = 12 /**< Failed to open the specified file.*/
	, DVC_ERROR_TIMEOUT                                     = 13 /**< Operation timed out.*/
	, DVC_ERROR_INTERRUPTED                                 = 14 /**< Operation was interrupted.*/
	, DVC_ERROR_DEVICE_DATABASE_INCOMPATIBLE                = 15 /**< Device database is incompatible or outdated.*/
	, DVC_ERROR_DEVICE_DATABASE_CORRUPTED                   = 16 /**< Device database structure is corrupted.*/
	, DVC_ERROR_LICENSE_REQUIRED                            = 17 /**< The license for codec is missing/outdated.*/
	, DVC_ERROR_NOT_PERMITTED                               = 18 /**< Requested operation is not premitted.*/
	, DVC_ERROR_NOT_SUPPORTED                               = 19 /**< Request is not supported on selected platform.*/
	, DVC_ERROR_AUDIO_UNAVAILABLE                           = 20 /**< Unable to initialize as the audio could not be activated.*/
	, DVC_ERROR_UNKNOWN                                     = 21 /**< Unhandled system error.*/
} DVC_Error;


/**
 * List of possible connection statuses; used in @DVC_Status structure.
 *
 * @note In stateless connection status will be @DVC_CONNECTION_STATUS_CONNECTED even if there is no exchange
 * of RTP packages
 *
 * @see DVC_Status structure.
 * @see DVC_GetClientStats() function.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_ConnectionStatus
{
	DVC_CONNECTION_STATUS_PENDING                           = 0 /**< Not connected; Packets exchange will not occur.*/
	, DVC_CONNECTION_STATUS_CONNECTING                      = 1 /**< Connecting to the remote side.*/
	, DVC_CONNECTION_STATUS_CONNECTED                       = 2 /**< Connected to the remote side.*/
	, DVC_CONNECTION_STATUS_FAILED                          = 3 /**< Connection has failed and will not retry to connect
	                                                                 to the remote side; check @DVC_Status.error for
	                                                                 more details.*/
	, DVC_CONNECTION_STATUS_CONNECTED_DEGRADED_DOWNLINK     = 5 /**< Connected to the remote side, but downlink
	                                                                 is degraded.*/
	, DVC_CONNECTION_STATUS_CONNECTED_DEGRADED_UPLINK       = 6 /**< Connected to the remote side, but uplink
	                                                                 is degraded.*/
	, DVC_CONNECTION_STATUS_CONNECTED_DEGRADED              = 7 /**< Connected to the remote side, but downlink
	                                                                 and uplink are degraded.*/
} DVC_ConnectionStatus;


/**
 * List of possible encoding types, used in @DVC_CodecConfig.
 *
 * Some of these encoding types are supported only if a separate license agreement is signed. For details, see
 * \ref LicensedFeatures.
 *
 * @see DVC_CodecConfig structure.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_RtpEncoding
{
	DVC_RTP_ENCODING_DVC2                                   = 0 /**< DVC-2 encoding type.*/
	, DVC_RTP_ENCODING_PCMU                                 = 1 /**< PCMU encoding type. See \ref g711_codec_license.*/
	, DVC_RTP_ENCODING_PCMA                                 = 2 /**< PCMA encoding type. See \ref g711_codec_license.*/
	, DVC_RTP_ENCODING_G722                                 = 3 /**< G722 encoding type. See \ref g722_codec_license.*/
	, DVC_RTP_ENCODING_G729A                                = 4 /**< G729A encoding type. Reserved.*/
	, DVC_RTP_ENCODING_G729AB                               = 5 /**< G729AB encoding type. Reserved.*/
	, DVC_RTP_ENCODING_L16                                  = 6 /**< L16 encoding type. Reserved.*/
	, DVC_RTP_ENCODING_OPUS                                 = 7 /**< OPUS encoding type. Reserved.*/
	, DVC_RTP_ENCODING_TELEPHONE_EVENT                      = 8 /**< DTMF encoding type.*/
} DVC_RtpEncoding;


/**
 * Structure that holds the status of a client's connection to the media server. On failure, \p error will be set
 * to indicate the problem.
 *
 * Possible errors include:
 * - @DVC_ERROR_RESOURCE_ALLOCATION_FAILED \n
 *   Failed to setup the media communication
 * - @DVC_ERROR_TIMEOUT \n
 *   The connection to the remote media server timed out
 * - @DVC_ERROR_NOT_PERMITTED \n
 *   The media server responded but does not support features required by this client
 *
 * @see DVC_GetClientStats() function.
 * @see DVC_ClientStats structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_Status
{
	DVC_ConnectionStatus connection_status;                 /**< Current connection status.*/
	DVC_Error error;                                        /**< If status is @DVC_CONNECTION_STATUS_FAILED,
	                                                             identifies the reason for the failure.*/
} DVC_Status;


/**
 * Structure containing statistics on input/output audio.
 *
 * @note  \p transmit_level and \p  output_level can be used to provide a real-time visual indicator of input and output
 * audio levels.
 * There are two ways to obtain audio statistics data:
 * - DVC_GetAudioStats() returns statistics from a system perspective, which is valid even if there is no library
 *   client. The data include audio activity from all sources: connection, input or output tests, and comfort noise
 *   being generated by DVC rendering engine. This version of audio statistic data is more suitable for use in the
 *   application's device selection, or input/output test UI's.
 * - DVC_GetClientStats() returns audio statistics as part of a DVC_ClientStats structure, which contains audio
 *   statistics from the perspective of that client and does not contain input and output test statistics. The data only
 *   include audio activity to and from the given connection. This version of audio statistic data is more suitable for
 *   use in the application's in-call voice activity UI.
 *
 * @see DVC_GetAudioStats() function.
 * @see DVC_GetClientStats() function.
 * @see DVC_ClientStats structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_AudioStats
{
	/* Audio statistics - Input*/
	float transmit_level;                                   /**< Most recent voice level (dB), value range: -100..0.*/
	float avg_gain;                                         /**< Average AGC gain (dB), value range: -10..+40.*/
	float snr;                                              /**< Estimated average signal-to-noise level (dB), value
	                                                             range: -100..+100.*/
	float ser;                                              /**< Estimated average signal-to-echo level, (dB), value
	                                                             range: -100..+100.*/
	unsigned long in_audio_tick_count;                      /**< Number of input thread ticks.*/

	/*Audio Diagnostics - Input*/
	int mic_clipping;                                       /**< 1 if the microphone input is clipping, 0 otherwise.*/
	int typing_noise_suppression;                           /**< 1 if typing noise suppression is active, 0 otherwise*/

	/* Audio statistics - Output*/
	float output_level;                                     /**< Average energy levels of the output channels (dB),
	                                                             value range: -100..0.*/
	unsigned long out_audio_tick_count;                     /**< Number of output thread ticks.*/
} DVC_AudioStats;


/**
 * Structure containing connection client statistics and other connection details; used with the DVC_GetClientStats().
 * function.
 *
 * @note For state codecs it is important to poll the \p status of a connection after having called
 * DVC_StartRtpCommunication() to make sure the connection has been connected properly or react to error
 * conditions. Alternatively the application may also setup callbacks to handle status changes.
 * @note For stateless codecs status will be @DVC_CONNECTION_STATUS_CONNECTED even when packets are not arriving.
 *
 * @see DVC_GetClientStats() function.
 * @see DVC_Status structures.
 * @see DVC_AudioStats structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_ClientStats
{
	DVC_Status         status;                              /**< Connection status (refer to @DVC_Status
	                                                             structure).*/
	DVC_ClientId       client_id;                           /**< Unique id used by the media server to identify the
	                                                             client.*/
	DVC_RtpEncoding    codec;                               /**< Codec currently used, see @DVC_RtpEncoding
	                                                             enumeration.*/
	/* Network statistics - Downlink*/
	unsigned int       downlink_jitter;                     /**< Estimated average amount of jitter in milliseconds in
	                                                             packets received from the server in last 10 seconds.*/
	unsigned int       downlink_latency;                    /**< Target latency in milliseconds for the downlink jitter
	                                                             buffer.*/
	unsigned int       downlink_packet_loss_count;          /**< Number of downlink audio packets lost or discarded.*/
	unsigned int       downlink_packet_loss_event_count;    /**< Number of downlink audio packet lost events.*/
	unsigned int       downlink_packet_played_count;        /**< Number of downlink audio packets played normally.*/
	unsigned int       downlink_packet_corrected_count;     /**< Number of corrected downlink audio packets.*/
	unsigned long long downlink_packet_count;               /**< Number of RTP packets received from the remote side
	                                                             since startup.*/
	unsigned long long downlink_payload_bytes;              /**< Total payload bytes of RTP packets received from the
	                                                             remote side since startup.*/
	unsigned int       downlink_time_since_last_packet;     /**< Time in milliseconds since last RTP packet has been
	                                                             received.*/

	/* Network statistics - Uplink*/
	unsigned int       uplink_jitter;                       /**< Estimated average amount of jitter in milliseconds in
	                                                             packets received by the server, in last 10 seconds.*/
	unsigned int       uplink_latency;                      /**< Target latency in milliseconds for the uplink jitter
	                                                             buffer.*/
	unsigned int       uplink_packet_loss_count;            /**< Number of uplink audio packets lost or discarded.*/
	unsigned int       uplink_packet_loss_event_count;      /**< Number of uplink audio packet lost events.*/
	unsigned int       uplink_packet_played_count;          /**< Number of uplink audio packets played normally.*/
	unsigned int       uplink_packet_corrected_count;       /**< Number of corrected uplink audio packets.*/
	unsigned long long uplink_packet_count;                 /**< Number of RTP packets sent to the media server
	                                                             since startup.*/
	unsigned long long uplink_payload_bytes;                /**< Total payload bytes of RTP packets sent to the
	                                                             media server since startup.*/
	unsigned int       uplink_time_since_last_packet;       /**< Time in milliseconds since last RTP packet has
	                                                             been sent.*/

	unsigned int       round_trip_time;                     /**< Packet round trip time to media server in
	                                                             milliseconds.*/

	/* Audio statistics - Input/Output*/
	DVC_AudioStats     audio_stats;                         /**< Input/output audio statistics as seen by this client.*/
	int                io_latency;                          /**< Estimated input/output latency (ms), value range:
	                                                             0..600, -1 if estimation invalid.*/
} DVC_ClientStats;


/**
 * Structure containing RTCP statistics' reception report block. The fields in this structure come directly from RTCP
 * packets and the detailed description can be found in RFC 3550, see https://tools.ietf.org/html/rfc3550#section-6.4.1
 *
 * @see DVC_RtcpStats structure.
 * @see DVC_GetRtcpStats() function.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_RtcpReportBlock
{
	unsigned int source_id;                                 /**< Sender's SSRC.*/
	unsigned char fraction_lost;                            /**< Fraction lost.*/
	unsigned long cumulative_packets_lost;                  /**< Cumulative number of packets lost.*/
	unsigned int highest_sequence;                          /**< Extended highest sequence number received.*/
	unsigned int jitter;                                    /**< Inter arrival jitter.*/
	unsigned int last_sr_time;                              /**< Last SR timestamp (LSR).*/
	unsigned int last_sr_delay;                             /**< Delay since last SR (DLSR).*/
} DVC_RtcpReportBlock;


/**
 * Structure containing RTCP statistics' sender report or receiver report. The fields in this structure come directly
 * from RTCP packets and the detailed description can be found in RFC 3550,
 * see https://tools.ietf.org/html/rfc3550#section-6.4.1
 *
 * @see DVC_GetRtcpStats() function.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_RtcpStats
{
	unsigned int sender_id;                                 /**< Sender's SSRC.*/
	unsigned long long ntp_timestamp;                       /**< NTP timestamp.*/
	unsigned int rtp_timestamp;                             /**< RTP timestamp.*/
	unsigned int senders_packet_count;                      /**< Sender's packet count.*/
	unsigned int senders_octet_count;                       /**< Sender's octet count.*/
	unsigned int num_report_blocks;                         /**< Reception report count (RC).*/
	DVC_RtcpReportBlock report_blocks[DVC_MAX_REPORT_BLOCKS]; /**< Reception report blocks.*/
} DVC_RtcpStats;


/**
 * Structure describing talking volume of a participants, as heard by the local client. This structure is used
 * by function DVC_GetTalkingLevels() to return talking levels of participants currently being heard by the client.
 *
 * @note Data from DVC_TalkingLevel can be used to implement talking indicators in a client's participant roster.
 *
 * @see DVC_GetTalkingLevels() function.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_TalkingLevel
{
	DVC_ClientId id;                                        /**< Unique id used by the media server to identify the
	                                                             participant who is currently being heard.*/
	float level;                                            /**< The participant's talking volume; a number in
	                                                             logarithmic scale between 0.0f and 1.0f.*/
} DVC_TalkingLevel;

/**
 * List of physical device types.
 *
 * @see DVC_GetCurrentDevice() function.
 * @see DVC_GetAvailableDevices() functions.
 * @see DVC_Device structure.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_DevicePhysicalType
{
	DVC_DEVICE_PHYSICAL_TYPE_UNKNOWN                         = 0 /**< Unknown type.*/
	, DVC_DEVICE_PHYSICAL_TYPE_LINE_IO                       = 1 /**< Line in/out.*/
	, DVC_DEVICE_PHYSICAL_TYPE_BUILTIN_IO                    = 2 /**< Built-in audio device.*/
	, DVC_DEVICE_PHYSICAL_TYPE_HANDSET                       = 3 /**< Handset.*/
	, DVC_DEVICE_PHYSICAL_TYPE_HEADSET                       = 4 /**< Headset.*/
	, DVC_DEVICE_PHYSICAL_TYPE_SPEAKERPHONE                  = 5 /**< Speaker phone.*/
	, DVC_DEVICE_PHYSICAL_TYPE_BLUETOOTH                     = 6 /**< Bluetooth.*/
	, DVC_DEVICE_PHYSICAL_TYPE_HDMI                          = 7 /**< HDMI.*/
	, DVC_DEVICE_PHYSICAL_TYPE_AIRPLAY                       = 8 /**< AirPlay.*/
	, DVC_DEVICE_PHYSICAL_TYPE_DISPLAYPORT                   = 9 /**< DisplayPort.*/
	, DVC_DEVICE_PHYSICAL_TYPE_SPEAKERPHONE_ECHO_SUPPRESSING = 10 /**< Speaker phone, with echo suppressing. */
	, DVC_DEVICE_PHYSICAL_TYPE_SPEAKERPHONE_ECHO_CANCELLING  = 11 /**< Speaker phone, with echo cancelling. */
} DVC_DevicePhysicalType;


/**
 * Audio direction flags.
 *
 * @see DVC_Device structure.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_Direction
{
	DVC_DIRECTION_NONE                                      = 0 /**< No direction, used by invalid device.*/
	, DVC_DIRECTION_INPUT                                   = 1 /**< Input.*/
	, DVC_DIRECTION_OUTPUT                                  = 2 /**< Output.*/
	, DVC_DIRECTION_INPUT_AND_OUTPUT = DVC_DIRECTION_INPUT | DVC_DIRECTION_OUTPUT /**< Both input and output.*/
} DVC_Direction;


/**
 * List of possible quality alarm types; used in DVC_AlarmStatusCallback() callback.
 *
 * @see DVC_AlarmStatusCallback() callback.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_AlarmType
{
	DVC_ALARM_TYPE_NOISE_LEVEL                              = 0 /**< Noise level alarm.*/
	, DVC_ALARM_TYPE_MIC_CLIPPING                           = 1 /**< Mic clipping alarm.*/
	, DVC_ALARM_TYPE_NETWORK_UP                             = 2 /**< Network uplink alarm.*/
	, DVC_ALARM_TYPE_NETWORK_DOWN                           = 3 /**< Network downlink alarm.*/
} DVC_AlarmType;


/**
 * List of possible quality alarm severities; used in DVC_AlarmStatusCallback() callback.
 *
 * @see DVC_AlarmStatusCallback() callback
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_AlarmSeverity
{
	DVC_ALARM_SEVERITY_GREEN                                = 0 /**< Normal level*/
	, DVC_ALARM_SEVERITY_AMBER                              = 1 /**< Warning level*/
	, DVC_ALARM_SEVERITY_RED                                = 2 /**< Error level*/
	, DVC_ALARM_SEVERITY_UNKNOWN                            = 3 /**< Unknown level*/
} DVC_AlarmSeverity;


/**
 * Operating system audio control selector for volume and mute change operations.
 *
 * @see DVC_MuteCallback() callback.
 * @see DVC_VolumeCallback() callback.
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_AudioControl
{
	DVC_AUDIO_CONTROL_MASTER                                = 0 /**< Master level audio control - all applications using
	                                                                 audio subsystem may be affected.*/
	, DVC_AUDIO_CONTROL_APP                                 = 1 /**< Application level audio control - only the current
	                                                                 application is affected.*/
} DVC_AudioControl;


/**
 * List of supported DTMF types to send.
 *
 * @see DVC_SendDtmf() function.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_DtmfType
{
	DVC_DTMF_TYPE_OUT_OF_BAND                               = 0 /**< Out-of-band DTMF type.*/
	, DVC_DTMF_TYPE_IN_BAND                                 = 1 /**< In-band DTMF type.*/
} DVC_DtmfType;


/**
 * List of possible comfort noise levels used in DVC_SetComfortNoiseLevel() and DVC_GetComfortNoiseLevel() functions.
 * Default comfort noise level restores configuration stored in device database for currently selected output device.
 *
 * @see DVC_SetComfortNoiseLevel() function.
 * @see DVC_GetComfortNoiseLevel() function.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_ComfortNoiseLevel
{
	DVC_COMFORT_NOISE_DEFAULT                               = 0 /**< Default comfort noise level based on device
	                                                                 database entry.*/
	, DVC_COMFORT_NOISE_MEDIUM                              = 1 /**< Normal comfort noise level.*/
	, DVC_COMFORT_NOISE_LOW                                 = 2 /**< Minimal comfort noise level.*/
	, DVC_COMFORT_NOISE_OFF                                 = 3 /**< Comfort noise is disabled.*/
} DVC_ComfortNoiseLevel;


/**
 * List of possible capture modes used in DVC_SetCaptureMode() and DVC_GetCaptureMode() functions. Default capture mode
 * restores configuration stored in device database for currently selected input device.
 *
 * @note Personal capture mode causes choosing most dominant speaker to be transmitted into the conference. Other
 * speakers may be not transmitted, however this setting may improve overall audio quality in high noise.
 * @note Group capture mode allows transmitting speakers with significant level differences. This setting may have
 * negative impact on noise introduced into the audio packets.
 * @note Transparent Input Processing Mode allows capturing and sending non-voice audio end-to-end. This mode still
 * offers echo suppression however, due to its transparent nature, no noise suppression is applied, and so a low end
 * microphone or a noisy environment may introduce the noise into the conference.
 *
 * @see DVC_SetCaptureMode() function.
 * @see DVC_GetCaptureMode() function.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_CaptureMode
{
	DVC_CAPTURE_MODE_DEFAULT                                = 0 /**< Default capture mode setting based on device
	                                                                 database entry.*/
	, DVC_CAPTURE_MODE_PERSONAL                             = 1 /**< Personal capture mode.*/
	, DVC_CAPTURE_MODE_GROUP                                = 2 /**< Group capture mode.*/
	, DVC_CAPTURE_MODE_TRANSPARENT                          = 3 /**< Transparent capture mode.*/
} DVC_CaptureMode;


/**
 * List of available device selection policies, which determine how the input and output devices are selected for use.
 *
 * - DVC_DEVICE_POLICY_DEFAULT \n
 *   Given the table is there, should be "Use the default policy" for the platform:\n
 *   | Policy                                | Platform              |
 *   | ------------------------------------- | --------------------- |
 *   | DVC_DEVICE_POLICY_IOS                 | iOS                   |
 *   | DVC_DEVICE_POLICY_DESKTOP             | macOS, Linux, Windows |
 *   | DVC_DEVICE_POLICY_ANDROID             | Android               |
 * - DVC_DEVICE_POLICY_DESKTOP \n
 *   (Desktop only) At start-up, system default device will be used; the client will "follow" the system default, i.e.
 *   change current device when the default changes. On a call to DVC_SetDevice(), the indicated device
 *   will be used, until a different device is chosen via a subsequent call to DVC_SetDevice(), or it becomes
 *   unavailable. When it does become unavailable, the system returns to the initial state, i.e. goes back to
 *   "following" the system default, and the preference for indicated device is not persisted (when the device becomes
 *   available again, it will not be automatically used). This is the policy suggested for most applications; if
 *   they present a device selection UI to the user, it should pass user selection to DVC_SetDevice().
 *   if they do not present a device selection UI to the user and simply always want to use the system default, this policy
 *   provides such behaviour if DVC_SetDevice is never called. This is the default policy for desktop systems.
 * - DVC_DEVICE_POLICY_APPLICATION_CONTROLLED \n
 *   (Desktop only) Initially, and when a device previously in use becomes unavailable, no device is selected
 *   (there is no audio). On a call to DVC_SetDevice(), the indicated device will be used, until a different device
 *   is chosen via a subsequent call to DVC_SetDevice(), or it becomes unavailable. This policy is intended
 *   for applications which want to completely control device selection, to implement a custom device switching logic
 *   not covered by the default policy. Note however, that such an application must handle callbacks and select a device
 *   from the available list, or else it will have no audio.
 * - DVC_DEVICE_POLICY_IOS \n
 *   (iOS only) On iOS the application has no control over choice of audio devices; this is the default and only policy
 *   for iOS, which is to use the device chosen by the OS. DVC_SetDevice() is not supported.
 * - DVC_DEVICE_POLICY_ANDROID \n
 *   (Android only) Mimic typical behaviour of a typical Android voice application. A device selection API
 *   may be presented to the user, but sometimes devices will be switched automatically: when Bluetooth is connected
 *   at startup time, it is chosen. When Headphones/Headset or Bluetooth becomes connected later,
 *   it is chosen (last connected is chosen rule apply). When Headphones/Headset in use becomes unavailable,
 *   Bluetooth is chosen if is available, otherwise the OS routing selection is accepted (typically Handset).
 *   When Bluetooth in use becomes unavailable, Headphones/Headset are chosen if are available,
 *   otherwise the OS routing selection is accepted (typically Handset).
 *   This is the default policy on Android.
 * - DVC_DEVICE_POLICY_ANDROID_SIMPLE \n
 *   (Android only) Initially use whatever device the system routing chooses (typically the Handset or
 *   Headphones/Headset). On a call to DVC_SetDevice(), the indicated device will be used, until a different device
 *   is chosen via a subsequent call to DVC_SetDevice(), or it becomes unavailable. When the current device become
 *   unavailable, let the system re-route audio to its defult device (again typically Handset or Headphones/Headset).
 *   This is the recommended policy for Android to implement custom device selection rules on top of it.
 *
 * @see DVC_SetDevice() function.
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_DeviceSelectionPolicy
{
	DVC_DEVICE_POLICY_DEFAULT                               = 0 /**< Use default policy for platform.*/
	, DVC_DEVICE_POLICY_DESKTOP                             = 1 /**< (desktop only) Use device selected by DVC_SetDevice().
	                                                                 If not present or function was not called, use
	                                                                 system default.*/
	, DVC_DEVICE_POLICY_APPLICATION_CONTROLLED              = 2 /**< (desktop only) Use only device selected by
	                                                                 DVC_SetDevice(). If not present or function was not
	                                                                 called, audio is disabled.*/
	, DVC_DEVICE_POLICY_IOS                                 = 3 /**< (iOS only) Use device chosen by system;
	                                                                 DVC_SetDevice() is not supported.*/
	, DVC_DEVICE_POLICY_ANDROID                             = 4 /**< (Android only) Typical Android application
	                                                                 behavior.*/
	, DVC_DEVICE_POLICY_ANDROID_SIMPLE                      = 5 /**< (Android only) Use default device (handset or
	                                                                 headphones) initially; use device selected by
	                                                                 DVC_SetDevice(). On disconnect, system chooses
	                                                                 new device (headset or headphones)*/
} DVC_DeviceSelectionPolicy;


/**
 * Choice whether the actual audio APIs should be used or if audio should be recorded to and played back from WAV files.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_AudioSystemMode
{
	DVC_PHYSICAL_AUDIO                                      = 0 /**< Use actual audio capabilities of the host
	                                                                 platform*/
	, DVC_FILE_AUDIO                                        = 1 /**< Use files for audio playback and capture*/
} DVC_AudioSystemMode;


/**
 * Choice whether the typing noise suppression should be used.
 *
 * @note Typing noise suppression is used only in desktop applications.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_TypingNoiseSuppression
{
	DVC_TYPING_NOISE_SUPPRESSION_DEFAULT                    = 0 /**< Defaults to \ref DVC_TYPING_NOISE_SUPPRESSION_ENABLED.*/
	, DVC_TYPING_NOISE_SUPPRESSION_ENABLED                  = 1 /**< Enable typing noise suppresion support*/
	, DVC_TYPING_NOISE_SUPPRESSION_DISABLED                 = 2 /**< Disable typing noise suppresion support*/
} DVC_TypingNoiseSuppression;


/**
 * List of possible input or output device states. Any status other than @DVC_DEVICE_STATUS_OK is considered an error
 * status.
 *
 * @note In future versions, more statuses may be added, and any condition previously marked with
 * a @DVC_DEVICE_STATUS_OTHER may be assigned a new value, so application should not rely on this.
 *
 * @see DVC_DeviceStatusCallback() callback.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_DeviceStatus
{
	DVC_DEVICE_STATUS_OK                                    = 0 /**< Device working.*/
	, DVC_DEVICE_STATUS_AUDIO_TIMEOUT                       = 1 /**< Timed out waiting for audio from device.*/
	, DVC_DEVICE_STATUS_NO_MICROPHONE_PERMISSION            = 2 /**< Application is not permitted to access the device.*/
	, DVC_DEVICE_STATUS_EXCLUSIVELY_USED                    = 3 /**< The device is exclusively used by another process.*/
	, DVC_DEVICE_STATUS_UNSUPPORTED_SAMPLE_RATE             = 4 /**< Device does not work in any sample rate supported
	                                                                 by Dolby Voice Client. It can be configured improperly
	                                                                 in OS or damaged. This value can be returned once
	                                                                 after checking all supported settings.*/
	, DVC_DEVICE_STATUS_AUDIO_UNAVAILABLE                   = 5 /**< System audio service is not available.*/
	, DVC_DEVICE_STATUS_OTHER                               = 6 /**< Audio not working for another reason.*/
} DVC_DeviceStatus;


/**
 * Persistent global unique identifier (GUID) for audio devices.
 *
 * This fixed size byte array uniquely identifies an audio device on the system. It is persistent across multiple
 * executions and through system reboots. Device GUID's are generated by DVC and set in DVC_Device structures returned
 * by DVC_GetAvailableDevices().
 *
 * @note Device GUID should be treated as a fixed size array of bytes and not a C string. Comparing DVC_DeviceGuid
 * instances thus should be done using memory compare of the two arrays.
 *
 * @see DVC_GetDevice() function.
 * @see DVC_GetCurrentDevice() function.
 * @see DVC_GetAvailableDevices() function.
 * @see DVC_Device structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_DeviceGuid
{
	char data[DVC_DEVICE_GUID_SIZE];                        /**< Persistent fixed size device GUID.*/
} DVC_DeviceGuid;


/**
 * Native device information for the Android OpenSLES audio subsystem.
 *
 * @see DVC_NativeDeviceInfo structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_AndroidNativeDeviceInfo
{
	unsigned int route;                                     /**< A type of this device (See Java AudioDeviceInfo.TYPE_XXXX).*/
} DVC_AndroidNativeDeviceInfo;


/**
 * Native device information for the Windows WASAPI audio subsystem.
 *
 * @see DVC_NativeDeviceInfo structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_WindowsNativeDeviceInfo
{
	char guid[DVC_MAX_NATIVE_ID_SIZE + 1];                  /**< The unique ID used by WASAPI.*/
} DVC_WindowsNativeDeviceInfo;


/**
 * Native device information for the iOS CoreAudio audio subsystem.
 *
 * @see DVC_NativeDeviceInfo structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_IosNativeDeviceInfo
{
	char id[DVC_MAX_NATIVE_ID_SIZE + 1];                    /**< The unique ID used by iOS. See UID
	                                                             in AVAudioSessionPortDescription.*/
	long data_source_id;                                    /**< The data source id, if any. 0 if none. See dataSourceID
	                                                             in AVAudioSessionDataSourceDescription.*/
} DVC_IosNativeDeviceInfo;


/**
 * Native device information for the macOS CoreAudio audio subsystem.
 *
 * @see DVC_NativeDeviceInfo structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_MacOsNativeDeviceInfo
{
	unsigned int id;                                        /**< The AudioObjectID of the CoreAudio object.*/
} DVC_MacOsNativeDeviceInfo;


/**
 * Native device information for the Linux PulseAudio audio subsystem.
 *
 * @see DVC_NativeDeviceInfo structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_LinuxNativeDeviceInfo
{
	unsigned int index;                                     /**< An index of the PulseAudio device (See pa_sink_info
	                                                             structure for sinks, and pa_source_info structure for
	                                                             sources).*/
	float base_volume;                                      /**< Unamplified/unattenuated volume of the device.*/
} DVC_LinuxNativeDeviceInfo;


/**
 * Audio native device information structure. The native device information provides the data that uniquely identifies
 * the device in the operating system. The application must not use members of this union which contain information
 * for a different platform than the one for which the application is built.
 *
 * @see DVC_Device structure.
 * @ingroup DVC_StructureEnum
 */
typedef union _DVC_NativeDeviceInfo
{
	DVC_AndroidNativeDeviceInfo android_opensles;           /**< Android (OpenSLES) native device info.*/
	DVC_WindowsNativeDeviceInfo windows_wasapi;             /**< Windows (WASAPI) native device info.*/
	DVC_IosNativeDeviceInfo ios_coreaudio;                  /**< iOS (CoreAudio) native device info.*/
	DVC_MacOsNativeDeviceInfo macos_coreaudio;              /**< macOS (CoreAudio) native device info.*/
	DVC_LinuxNativeDeviceInfo linux_pulseaudio;             /**< Linux (PulseAudio) native device info.*/
} DVC_NativeDeviceInfo;


/**
 * Audio device structure that describes input and output devices.
 *
 * @see DVC_GetDevice() function.
 * @see DVC_GetCurrentDevice() function.
 * @see DVC_GetAvailableDevices() function.
 * @see DVC_Direction enumeration.
 * @see DVC_DevicePhysicalType  enumeration.
 * @see DVC_NativeDeviceInfo structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_Device
{
	DVC_DeviceGuid guid;                                    /**< Unique global device id, persistent across multiple
	                                                             executions.*/
	char name[DVC_MAX_NAME_SIZE + 1];                       /**< Device name as reported by the operating system as
	                                                             a UTF-8 encoded string.*/
	DVC_DevicePhysicalType physical_type;                   /**< The physical type of the device.*/
	DVC_Direction direction;                                /**< Supported device direction (bitfield).*/
	DVC_Direction default_device;                           /**< Audio direction for which this device is system default
	                                                             device (bitfield). Only one device will be the default
	                                                             input or output device.*/
	DVC_NativeDeviceInfo native_device_info;                /**< Device properties in OS-specific format.
	                                                             Check @DVC_NativeDeviceInfo for more informations.*/
} DVC_Device;

/**
 * Input processing information for the Android audio subsystem.
 *
 * @see DVC_InputProcessingProperties structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_AndroidInputProcessingProperties
{
	int recording_preset;                                   /**< Identifier of the Android recording preset used by this
	                                                             device (See Java MediaRecorder.AudioSource).*/
	int mode;                                               /**< Identifier of the Android audio mode used by this
	                                                             device (See Java AudioManager.getMode()).*/
} DVC_AndroidInputProcessingProperties;

/**
 * Output processing information for the Android audio subsystem.
 *
 * @see DVC_OutputProcessingProperties structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_AndroidOutputProcessingProperties
{
	int output_stream_type;                                 /**< Identifier of the Android audio stream type used by
	                                                             this device (See Java AudioManager.STREAM_XXX).*/
	int volume_stream_type;                                 /**< Identifier of the Android audio stream type used for
	                                                             adjusting volume of this device (See Java
	                                                             AudioManager.adjustStreamVolume()).*/
	int mode;                                               /**< Identifier of the Android audio mode used by this
	                                                             device (See Java AudioManager.getMode()).*/
} DVC_AndroidOutputProcessingProperties;

/**
 * Audio output processing properties. The application must not use members of this union which contain information
 * for a different platform than the one for which the application is built.
 *
 * @see DVC_OutputProcessingCallback callback.
 * @ingroup DVC_StructureEnum
 */
typedef union _DVC_OutputProcessingProperties
{
	DVC_AndroidOutputProcessingProperties android_properties; /**< Android processing properties.*/
} DVC_OutputProcessingProperties;

/**
 * Audio input processing properties. The application must not use members of this union which contain information
 * for a different platform than the one for which the application is built.
 *
 * @see DVC_InputProcessingCallback callback.
 * @ingroup DVC_StructureEnum
 */
typedef union _DVC_InputProcessingProperties
{
	DVC_AndroidInputProcessingProperties android_properties; /**< Android processing properties.*/
} DVC_InputProcessingProperties;

/**
 * Connection status callback function.
 *
 * @note Callback will be called if any part of the connection status has changed.
 *
 * @param handle                                            [in] Handle of the client that changed status.
 * @param current_connection_status                         [in] Current connection status.
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_ConnectionStatusCallback)(
	DVC_Handle handle
	, DVC_Status current_connection_status
	, void* context
	);


/**
 * Audio test callback.
 *
 * @note Callback will be called whenever a test completes.
 * @note Update of audio statistics (DVC_AudioStats) aquired by DVC_GetClientStats or DVC_GetAudioStats functions is blocked during execution of the tests.
 *
 * @param context                                           [in] The void pointer identifier provided in the call that
 *                                                               started the test.
 *
 * @see DVC_StartOutputTest() function.
 * @see DVC_StartInputTestRecord() function.
 * @see DVC_StartInputTestPlay() function.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_AudioTestCallback)(
	void* context
	);


/**
 * Play wave-file callback.
 *
 * @note Callback will be called whenever a playback completes.
 *
 * @param context                                           [in] The void pointer identifier provided in the call that
 *                                                               started the test.
 *
 * @see DVC_PlayWave() function.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_PlayWaveCallback)(
	void* context
	);


/**
 * Client id callback function.
 *
 * @note Callback will be called whenever the client id of this client changes. This will occur during the connection
 * process of the client after a call to DVC_StartRtpCommunication(). Additionally, when the client id is no longer
 * considered valid this will fire with an invalid client id (an empty string).
 *
 * @param handle                                            [in] The handle of the client whose id changed.
 * @param id                                                [in] The new id for the client. This is the id the media
 *                                                               server use to identify this client's connection.
 * @param context                                           [in] The void pointer identifier provided in DVC_Config
 *                                                               structure.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_ClientIdCallback)(
	DVC_Handle handle
	, DVC_ClientId id
	, void* context
	);


/**
 * Quality alarm status change callback function.
 *
 * @note Callback will be called whenever the severity of an alarm has changed. You can compare the current and previous
 * values to find how the status has changed.
 *
 * @param handle                                            [in] Handle of the client that changed status.
 * @param type                                              [in] The type of the alarm that has changed.
 * @param severity                                          [in] Severity of the alarm.
 * @param context                                           [in] The void pointer identifier provided in DVC_Config
 *                                                               structure.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_AlarmStatusCallback)(
	DVC_Handle handle
	, DVC_AlarmType type
	, DVC_AlarmSeverity severity
	, void* context
	);


/**
 * List of available log levels used in DVC_LoggingParams.
 *
 * @see DVC_LoggingParams structure.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_LogLevel
{
	DVC_LOG_DEFAULT       = 0                               /**< Default log level (Same as @DVC_LOG_INFO).*/
	, DVC_LOG_NONE        = 1                               /**< No logs.*/
	, DVC_LOG_EMERGENCY   = 2                               /**< Complete system failure.*/
	, DVC_LOG_ALERT       = 4                               /**< Failure of an important component.*/
	, DVC_LOG_CRITICAL    = 8                               /**< Failure of an ancillary or support component.*/
	, DVC_LOG_ERROR       = 16                              /**< Failure of a component that has been handled or is
	                                                             likely to be handled properly.*/
	, DVC_LOG_WARNING     = 32                              /**< Internal errors and small failures that have been
	                                                             handled.*/
	, DVC_LOG_NOTICE      = 64                              /**< Potential failures or strange events but may be
	                                                             considered part of normal operation.*/
	, DVC_LOG_INFO        = 128                             /**< Standard information about the running system.*/
	, DVC_LOG_DEBUG       = 512                             /**< Detailed information useful for debugging, may affect
	                                                             performance.*/
} DVC_LogLevel;


/**
 * Log callback function.
 *
 * @param level                                             [in] Level of the log message, see @DVC_LogLevel.
 * @param timestamp_sec                                     [in] Time in seconds since midnight UTC time,
 *                                                               January 1 1970.
 * @param timestamp_usec                                    [in] Time in micro-seconds from \p timestamp_sec when the
 *                                                               message was logged.
 * @param subsystem                                         [in] String identifying the internal subsystem which logged
 *                                                               the message.
 * @param msg_id                                            [in] A unique string identifying the logged message or an
 *                                                               empty string.
 * @param message                                           [in] Log message from the logging system.
 * @param context                                           [in] The void pointer identifier provided
 *                                                               in @DVC_LoggingParams.
 *
 * @see DVC_LoggingParams
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_LogCallback)(
	DVC_LogLevel level
	, time_t timestamp_sec
	, unsigned int timestamp_usec
	, const char* subsystem
	, const char* msg_id
	, const char* message
	, void* context
	);


/**
 * Notification of pause or resume of audio input or output.
 *
 * The direction parameter indicates whether input or output is paused:
 * | Direction                      | Description                           |
 * | ------------------------------ | ------------------------------------- |
 * | DVC_DIRECTION_NONE             | not paused for either input or output |
 * | DVC_DIRECTION_INPUT            | input only paused                     |
 * | DVC_DIRECTION_OUTPUT           | output only paused                    |
 * | DVC_DIRECTION_INPUT_AND_OUTPUT | both input and output paused          |
 *
 * @param direction                                         [in] The direction that this event refers to.
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @see DVC_PauseAudio function.
 * @see DVC_ResumeAudio function.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_PauseCallback)(
	DVC_Direction direction
	, void* context
	);


/**
 * Callback function for sending packets.
 *
 * @param handle                                            [in] Handle of the client that generated the packet.
 * @param data                                              [in] Pointer to buffer containing RTP packet.
 * @param len                                               [in] Length of the data pointed to by \p data.
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_PacketCallback)(
	DVC_Handle handle
	, const void* data
	, unsigned int len
	, void* context
	);


/**
 * Volume callback function.
 *
 * @note Callback will be called whenever input or output volume changed on operating system level (master or
 * application). This mainly happens when user modifies volume level in an operating system audio settings or in audio
 * device settings.
 * @note This callback may also be called after a successful call to DVC_SetSystemVolume() function.
 *
 * @param control                                           [in] An audio control for which the change happened.
 * @param direction                                         [in] The direction that this event refers to.
 * @param volume                                            [in] Current volume level (between 0.0f and 1.0f).
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_VolumeCallback)(
	DVC_AudioControl control
	, DVC_Direction direction
	, float volume
	, void* context
	);


/**
 * Mute callback function.
 *
 * @param control                                           [in] An audio control for which the change happened.
 * @param direction                                         [in] The direction that this event refers to.
 * @param mute                                              [in] 1 if muted and 0 otherwise.
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @note Callback will be called whenever input or output was muted/unmuted on operating system level (master or
 * application). This mainly happens when user changes mutes in operating system audio settings or in audio device
 * settings.
 * @note This callback may also be called after a successful call to DVC_SetSystemMute() function.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_MuteCallback)(
	DVC_AudioControl control
	, DVC_Direction direction
	, int mute
	, void* context
	);


/**
 * Device callback function.
 *
 * @param guid                                              [in] Guid of added device.
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @see DVC_DeviceCallbacksConfig structure.
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_DeviceCallback)(
	DVC_DeviceGuid guid
	, void* context
	);

/**
 * Input processing properties changed callback function.
 *
 * @param properties                                        [in] Input audio processing properties
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @see DVC_DeviceCallbacksConfig structure.
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_InputProcessingCallback)(
		DVC_InputProcessingProperties properties
		, void* context
);

/**
 * Output processing properties changed callback function.
 *
 * @param properties                                        [in] Output audio processing properties
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @see DVC_DeviceCallbacksConfig structure.
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_OutputProcessingCallback)(
		DVC_OutputProcessingProperties properties
		, void* context
);

/**
 * Current device changed callback function.
 *
 * @param direction                                         [in] Direction of device that has been set as current.
 * @param no_device                                         [in] 1 if there is no device currently in use in given
 *                                                               direction, otherwise 0
 * @param guid                                              [in] If no_device is 0, guid of current device, otherwise
 *                                                               unspecified.
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @see DVC_DeviceCallbacksConfig structure.
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_CurrentDeviceCallback)(
	DVC_Direction direction
	, int no_device
	, DVC_DeviceGuid guid
	, void* context
	);


/**
 * Current state of device callback function.
 *
 * @note Called when device enters or recovers from an error state. In this case, recovery can also mean switching
 * to a working device after the previous one reported error state.
 *
 * @param direction                                         [in] The direction that this event refers to.
 * @param status                                            [in] Current state of device.
 * @param context                                           [in] The void pointer identifier provided in @DVC_Config.
 *
 * @see DVC_DeviceCallbacksConfig structure.
 * @see DVC_Config structure.
 * @see DVC_DeviceStatus enumeration.
 * @ingroup DVC_StructureEnum
 */
typedef void (*DVC_DeviceStatusCallback)(
	DVC_Direction direction
	, DVC_DeviceStatus status
	, void* context
	);

/**
 * Logging parameters structure used in @DVC_Config.
 *
 * @note If no level is specified, @DVC_LOG_INFO is used.
 *
 * @see DVC_LogCallback() callback.
 * @see DVC_Initialize() function.
 * @see DVC_LogLevel structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_LoggingParams
{
	DVC_LogLevel level;                                     /**< The log level to use. Only messages of this and higher
	                                                             levels will be logged.*/
	const char* file;                                       /**< Name of log-file, set to \p NULL to disable logging to
	                                                             file.*/
} DVC_LoggingParams;


/**
 * List of possible connection modes, used in  structure.
 *
 * @see DVC_Dvc2CodecParameters structure.
 * @see DVC_StartRtpCommunication() function.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_ConnectionMode
{
	DVC_CONNECTION_MODE_DEFAULT                             = 0 /**< Defaults to @DVC_CONNECTION_MODE_CS.*/
	, DVC_CONNECTION_MODE_CC                                = 1 /**< Client-client connection.*/
	, DVC_CONNECTION_MODE_CS                                = 2 /**< Client-server connection.*/
} DVC_ConnectionMode;


/**
 * List of possible sources types, used in @DVC_Dvc2CodecParameters structure.
 *
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_Dvc2CodecParameters structure.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_SourceSide
{
	DVC_SOURCE_SIDE_CLIENT                                  = 0 /**< Client source side, set by hardware endpoint
	                                                                 and client library.*/
	, DVC_SOURCE_SIDE_SERVER                                = 1 /**< Server source side, set by the server.*/
} DVC_SourceSide;


/**
 * List of possible FEC bitrates used during RTP communication.
 *
 * @see DVC_SetFec() function.
 * @ingroup DVC_StructureEnum
 */
typedef enum _DVC_FecBitrate
{
	DVC_FEC_BITRATE_DEFAULT                                = 0 /**< Defaults to \ref DVC_FEC_BITRATE_16P0.*/
	, DVC_FEC_BITRATE_DISABLED                             = 1 /**< FEC disabled.*/
	, DVC_FEC_BITRATE_6P4                                  = 2 /**< 6.4 kbps FEC bitrate */
	, DVC_FEC_BITRATE_8P0                                  = 3 /**< 8.0 kbps FEC bitrate */
	, DVC_FEC_BITRATE_9P6                                  = 4 /**< 9.6 kbps FEC bitrate */
	, DVC_FEC_BITRATE_11P2                                 = 5 /**< 11.2 kbps FEC bitrate */
	, DVC_FEC_BITRATE_12P8                                 = 6 /**< 12.8 kbps FEC bitrate */
	, DVC_FEC_BITRATE_14P4                                 = 7 /**< 14.4 kbps FEC bitrate */
	, DVC_FEC_BITRATE_16P0                                 = 8 /**< 16.0 kbps FEC bitrate */
	, DVC_FEC_BITRATE_17P6                                 = 9 /**< 17.6 kbps FEC bitrate */
	, DVC_FEC_BITRATE_19P2                                 = 10 /**< 19.2 kbps FEC bitrate */
	, DVC_FEC_BITRATE_20P8                                 = 11 /**< 20.8 kbps FEC bitrate */
	, DVC_FEC_BITRATE_22P4                                 = 12 /**< 22.4 kbps FEC bitrate */
	, DVC_FEC_BITRATE_24P0                                 = 13 /**< 24.0 kbps FEC bitrate */
} DVC_FecBitrate;


/**
 * License enabling additional licence controlled features.
 *
 * @note Please refer to \ref LicensedFeatures for further details.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_License
{
	const void* license;                                    /**< License data.*/
	unsigned int license_size;                              /**< License data size.*/
} DVC_License;


#if defined(__ANDROID__) || defined(ANDROID)
/**
 * DVC Android system specific configuration structure to be assigned to DVC_Config::platform_specific_config,
 * before calling DVC_Initialize(). This structure specifies configuration parameters that cannot be modified after
 * initialization.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
#include <jni.h>
typedef struct _DVC_AndroidSystemConfig
{
	JavaVM* jvm;                                            /**< Pointer to a valid Java Virtual Machine.*/
	jobject android_context;                                /**< Reference to Android application Context.*/
	int use_native_logger;                                  /**< Set to 1 to enable and 0 to disable additional log
	                                                             output going to native logger.*/
} DVC_AndroidSystemConfig;
#endif


/**
 * Codec configuration used in @DVC_ProtocolConfig. Call DVC_GetAvailableCodecConfigs() to get all available
 * options.
 *
 * @see DVC_ProtocolConfig structure.
 * @see DVC_Dvc2CodecParameters structure.
 * @see DVC_GetAvailableCodecConfigs() function.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_CodecConfig
{
	DVC_RtpEncoding encoding;                               /**< The encoding of the RTP stream.*/
	unsigned int clock_rate;                                /**< Codec clock rate, used for selected codec and DTMF.*/
	const void* encoding_parameters;                        /**< Codec specific parameters, based on encoding type.
	                                                             Available structures are DVC_Dvc2CodecParameters for
	                                                             @DVC_RTP_ENCODING_DVC2 encoding type and
	                                                             DVC_TelephoneEventsParameters for
	                                                             @DVC_RTP_ENCODING_TELEPHONE_EVENT encoding type.*/
} DVC_CodecConfig;


/**
 * DVC codec parameters; used by DVC_CodecConfig().
 *
 * @note Supported protocol versions can be retrieved using DVC_GetAvailableCodecConfigs() function.
 *
 * @see DVC_CodecConfig() structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_Dvc2CodecParameters
{
	DVC_ConnectionMode connection_mode;                     /**< Set connection mode, defaults to
	                                                             \ref DVC_CONNECTION_MODE_CS.*/
	DVC_SourceSide source;                                  /**< Source type, normally it should be set to
	                                                             \ref DVC_SOURCE_SIDE_CLIENT.*/
	unsigned int protocol_version;                          /**< Bitstream protocol version. Call
	                                                             DVC_GetAvailableCodecConfigs() to get all
	                                                             available options. If protocol_version is set to 0
	                                                             and connection_mode is set to
	                                                             \ref DVC_CONNECTION_MODE_CS, library will do automatic
	                                                             protocol version negotiation.*/
} DVC_Dvc2CodecParameters;


/**
 * DVC codec parameters used in @DVC_CodecConfig.
 *
 * @see DVC_CodecConfig() structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_TelephoneEventsParameters
{
	const char* supported_events;                           /**< Supported telephone events.*/
} DVC_TelephoneEventsParameters;


/**
 * DVC RTP/RTCP protocol configuration structure to be used in @DVC_RtpCommunicationConfig. The structure contains
 * optional configurations for RTP/RTCP, including:
 * - RTP payload type
 * - DTMF payload type
 * - turn on/off RTCP
 * - Codec configuration
 * It can be used when calling DVC_StartRtpCommunication() function to set the parameters above to non-default
 * values.
 *
 * @note By default, DVC uses the following configuration
 *  - RTP payload type 97
 *  - DTMF payload type 101
 *  - RTCP is not enabled
 *
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_CodecConfig structures.
 * @see DVC_RtpCommunicationConfig structure
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_ProtocolConfig
{
	signed char rtp_payload_type;                           /**< RTP payload type for media traffic. Valid values are
	                                                             >=-1 and <=127, if -1, default value of 97 will be used
	                                                             for DVC2 and default for other supported codecs as
	                                                             defined in RFC 3551. If enable_rtcp is set and
	                                                             multiplex mode is used (as described in rfc5761),
	                                                             payload types should not be in range of 72-76.*/
	signed char dtmf_payload_type;                          /**< Payload type for DTMF. Valid values are >=-1 and <=127,
	                                                             if -1, default value of 101 will be used. Must
	                                                             be different than RTP payload.*/
	int enable_rtcp;                                        /**< Set to non-zero to enable RTCP support and 0 to
	                                                             disable. Supported packet types are SR(200), RR(201),
	                                                             SDES(202) and BYE(203).*/
	DVC_CodecConfig codec_config;                           /**< DVC RTP codec configuration.*/
} DVC_ProtocolConfig;


/**
 * DVC configuration structure to be used in DVC_StartRtpCommunication().
 *
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_ProtocolConfig structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_RtpCommunicationConfig
{
	const DVC_ProtocolConfig* protocol_config;              /**< Optional RTP/RTCP configurations, can be set to \p NULL
	                                                             to use default settings.*/
	void* reserved;                                         /**< Reserved field, use \p NULL unless advised specifically
	                                                             by Dolby engineering.*/
} DVC_RtpCommunicationConfig;


/**
 * Structure with zipped state dump created using DVC_CreateStateDump().
 *
 * The \p data contains a binary form of zipped file that provide information about the internal state of the library
 * and some logging history. If audio logging is turned on, there is also an audio dump of the individual audio logs.
 * State dumps are extremely useful for debugging issues with the client library.
 *
 * @see DVC_CreateStateDump() function.
 * @see DVC_DeleteStateDump() function.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_ZippedStateDump
{
	const void* data;                                       /**< Pointer to the zip data.*/
	unsigned int size;                                      /**< Size of zipped data in bytes.*/
} DVC_ZippedStateDump;


/**
 * DVC library callbacks configuration structure to be used in @DVC_CallbacksConfig.
 *
 * @see DVC_Initialize() function.
 * @see DVC_Config structure.
 * @see DVC_CallbacksConfig structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_LibraryCallbacksConfig
{
	DVC_ConnectionStatusCallback connection_status_changed; /**< Function pointer for connection status change
	                                                             callbacks, set to \p NULL for no callbacks.*/
	DVC_ClientIdCallback client_id_changed;                 /**< Function pointer for client id change callbacks,
	                                                             set to \p NULL for no callbacks.*/
	DVC_LogCallback log_entry_ready;                        /**< Function pointer for log callbacks, set to \p NULL
	                                                             for no callbacks.*/
	DVC_AlarmStatusCallback alarm_status_changed;           /**< Function pointer for alarm status callbacks,
	                                                             set to \p NULL for no callbacks.*/
} DVC_LibraryCallbacksConfig;


/**
 * DVC packets callbacks configuration structure to be used in @DVC_CallbacksConfig.
 *
 * @note DVC_PacketsCallbacksConfig::rtp_packet_ready is required.
 *
 * @see DVC_Initialize() function.
 * @see DVC_Config structure.
 * @see DVC_CallbacksConfig structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_PacketsCallbacksConfig
{
	DVC_PacketCallback rtp_packet_ready;                    /**< Function pointer for RTP packets callbacks.*/
	DVC_PacketCallback rtcp_packet_ready;                   /**< Function pointer for RTCP packets callbacks,
	                                                             set to \p NULL for no callbacks.*/
} DVC_PacketsCallbacksConfig;


/**
 * DVC device callbacks configuration structure to be used in @DVC_CallbacksConfig.
 *
 * @see DVC_Initialize() function.
 * @see DVC_Config structure.
 * @see DVC_CallbacksConfig structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_DeviceCallbacksConfig
{
	DVC_DeviceCallback device_added;                        /**< Function pointer for device added callback,
	                                                             set to \p NULL for no callbacks.*/
	DVC_DeviceCallback device_removed;                      /**< Function pointer for device removed callback,
	                                                             set to \p NULL for no callbacks.*/
	DVC_DeviceCallback device_properties_changed;           /**< Function pointer for device properties change callback,
	                                                             set to \p NULL for no callbacks.*/
	DVC_InputProcessingCallback input_processing_changed;   /**< Function pointer for device processing properties change callback,
	                                                             set to \p NULL for no callbacks.*/
	DVC_OutputProcessingCallback output_processing_changed; /**< Function pointer for device processing properties change callback,
	                                                             set to \p NULL for no callbacks.*/
	DVC_CurrentDeviceCallback current_device_changed;       /**< Function pointer for current device changed callback,
	                                                             set to \p NULL for no callbacks.*/
	DVC_DeviceStatusCallback device_status_changed;         /**< Function pointer for device status callback,
	                                                             set to \p NULL for no callbacks.*/
	DVC_PauseCallback pause_state_changed;                  /**< Function pointer for pause and resume callbacks,
	                                                             set to \p NULL for no callbacks.*/
	DVC_VolumeCallback volume_changed;                      /**< Function pointer for volume change callbacks,
	                                                             set to \p NULL for no callbacks.*/
	DVC_MuteCallback mute_state_changed;                    /**< Function pointer for mute change callbacks,
	                                                             set to \p NULL for no callbacks.*/
} DVC_DeviceCallbacksConfig;


/**
 * DVC library callbacks configuration structure to be used in @DVC_Config.
 *
 * @tip Value of \p callback_context will be provided in callback call as \p context parameter. It can be used to cast
 * on any user defined type, and should be valid as long as DVC_Shutdown() called.
 *
 * @see DVC_Initialize() function.
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_CallbacksConfig
{
	DVC_LibraryCallbacksConfig library_callbacks_config;    /**< Structure with library callbacks.*/
	DVC_PacketsCallbacksConfig packets_callbacks_config;    /**< Structure with packets callbacks.*/
	DVC_DeviceCallbacksConfig device_callbacks_config;      /**< Structure with device callbacks.*/
	void* callback_context;                                 /**< Pointer value passed as 'context' parameter
	                                                             to callbacks, unless otherwise indicated in callback
	                                                             description. Not dereferenced or otherwise interpreted
	                                                             by DVC library.*/
} DVC_CallbacksConfig;


/**
 * Audio configuration used to set non runtime setting or initial state of audio.
 *
 * @see DVC_Config structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_AudioConfig
{
	DVC_DeviceSelectionPolicy device_selection_policy;             /**< Device selection policy to be used at initialization.*/
	DVC_Direction initial_pause;                                   /**< Directions in which audio should be paused after
	                                                                   initialization */
	DVC_AudioSystemMode audio_system_mode;                         /**< Default is DVC_PHYSICAL_AUDIO */
	DVC_TypingNoiseSuppression typing_noise_suppression;           /**< Enable or disable typing noise suppression*/
} DVC_AudioConfig;


/**
 * DVC library configuration structure to be used in DVC_Initialize(). This structure specifies configuration parameters
 * that cannot be modified after initialization. If one of these parameter has to be changed, all conference must be
 * left, and DVC_Shutdown() must be called to allow a new call to DVC_Initialize().
 *
 * @note Value of \p platform_specific_config field depends on the platform:
 * - For Android platform assign platform_specific_config to a properly filled @DVC_AndroidSystemConfig structure.
 * - For other platforms set it to \p NULL.
 *
 * @see DVC_Initialize() function.
 * @see DVC_AndroidSystemConfig structure.
 * @ingroup DVC_StructureEnum
 */
typedef struct _DVC_Config
{
	DVC_CallbacksConfig callbacks_config;                   /**< Configuration for callback functions.*/
	DVC_LoggingParams logging;                              /**< Logging related parameters.*/
	const char* crash_dump_file_path;                       /**< Path to crash dump file location,
	                                                             set to \p NULL to disable crash dumps.*/
	DVC_License license;                                    /**< License enabling additional license controlled
	                                                             features, can be \p NULL.*/
	DVC_AudioConfig audio_config;                           /**< Configuration for audio*/
	void* reserved;                                         /**< Reserved field, use \p NULL unless advised specifically
	                                                             by Dolby engineering.*/
	void* platform_specific_config;                         /**< Reserved field, use \p NULL unless advised specifically
	                                                             by Dolby engineering.*/
} DVC_Config;


/**
 * Initializes the library, allocates resources, and starts necessary internal threads.
 *
 * @tip Tracing problems with that function require setting file logger in the DVC_LoggingParams structure. Other logging options might not work until DVC_Initialize() finishes.
 * @note This function must be called before using any of the DVC library functions that may return a
 * @DVC_ERROR_NOT_INITIALIZED error. The macro @DVC_VERSION must be passed as the \p version parameter.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Another thread is currently executing
 *                                                                    DVC_Shutdown().
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p config or \p version is \p NULL.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p config is invalid.
 * @retval DVC_ERROR_INCOMPATIBLE_API_VERSION               [FAILURE] Library in use is not compatible with DVC
 *                                                                    interface version the application was
 *                                                                    compiled with.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] Library is initialized.
 * @retval DVC_ERROR_UNABLE_TO_CREATE_LOGGER                [FAILURE] Log level or log file in \p config is invalid.
 *                                                                    On Android this might also mean missing
 *                                                                    platform specific config.
 * @retval DVC_ERROR_RESOURCE_ALLOCATION_FAILED             [FAILURE] Required resource could not be allocated.
 * @retval DVC_ERROR_MISSING_SYSTEM_OPTIONS                 [FAILURE] Missing system options.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] Configuration is not supported on this platform.
 * @retval DVC_ERROR_AUDIO_UNAVAILABLE                      [FAILURE] Unable to initialize as the audio could not be activated.
 *
 * @ingroup DVC_Functions
 * @see DVC_Config structure.
 * @see DVC_Shutdown() function.
 * @see DVC_Error enumeration.
 */
DVC_API DVC_Error DVC_Initialize(
	const DVC_Config* config                                /**<[in] Pointer to a valid @DVC_Config structure.*/
	, const char* version                                   /**<[in] Pass the value of the macro @DVC_VERSION.*/
	);


/**
 * Cleans up internal library state and deallocates resources.
 *
 * @note This function must be called once after a successful call to DVC_Initialize()
 * when finished using the DVC client interface. If the application closes or
 * unloads the DVC dynamic library without calling DVC_Shutdown() the results
 * are undefined.
 * @note After a call to DVC_Shutdown(), DVC functions which may return a @DVC_ERROR_NOT_INITIALIZED error will
 * return @DVC_ERROR_NOT_INITIALIZED until DVC_Initialize() is called again.
 * @note The application must not call any DVC functions during a call to DVC_Shutdown(). Doing so may cause undefined
 * behaviour.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 *
 * @see DVC_Initialize() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_Shutdown(
	);


/**
 * Creates a client handle that can be used to start a connection. All subsequent operations on this client will
 * require the \p handle provided by this function.
 *
 * @note The newly created client does not start communication with media server until DVC_StartRtpCommunication()
 * is called.
 * @note The newly created client will be in the following default state:
 * \li The client is not muted.
 * \li The audio is not muted.
 * \li The audio volume starts at a value of 1.0.
 * \li The audio codec is set to @DVC_RTP_ENCODING_DVC2.
 * \li Audio logging is disabled.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p handle is \p NULL.
 *
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_DestroyClient() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_CreateClient(
	DVC_Handle* handle                                      /**<[out] Pointer to variable that will be set to the handle
	                                                                  assigned to this client. Subsequent operations on
	                                                                  the client will require this handle.*/
	);


/**
 * Destroys the client associated with the given \p handle.
 *
 * @note Once DVC_DestroyClient() has been called on a client handle, the
 * handle becomes invalid and cannot be used in any subsequent API calls.
 * @note You must NOT call any DVC functions that take a @DVC_Handle during a call to
 * DVC_DestroyClient(), doing so may cause undefined behaviour.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 *
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_DestroyClient(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	);


/**
 * Obtains an event handle that can be waited upon to determine whether callbacks
 * are waiting to be executed using DVC_DoCallbacks().
 *
 * On Windows platform this handle can be cast to and used as a Windows API \p HANDLE and can be waited
 * on using the Windows functions \p WaitForSingleObject() or \p WaitForMultipleObjects().
 * In Linux/MacOS X/iOS and Android, this handle can be cast to and used as a file descriptor
 * and can be waited on using \p select().
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p handle is \p NULL.
 *
 * @see DVC_DoCallbacks() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetEventHandle(
	DVC_EventHandle* handle                                 /**< [out] Pointer to variable to be filled out
	                                                                   with a @DVC_EventHandle object.*/
	);


/**
 * Executes any pending callbacks.
 *
 * The DVC library implements most callbacks as deferred callbacks. When callbacks are triggered, e.g. due to audio
 * device changes or an audio test was completed or cancelled, the callbacks are pushed into a list to be executed when
 * DVC_DoCallbacks() is called. In this way the application is in complete control of when and in which thread callbacks
 * happen, to avoid potential deadlocking issues. The exception to this are callbacks defined in
 * DVC_PacketsCallbacksConfig, which are called immediately in their own thread.
 *
 * @tip DVC_GetEventHandle() provides an event handle that can be waited on to be alerted when a callback is pending.
 * @note Use DVC_DoCallbacks() with care. DVC_DoCallbacks() holds a lock and only releases the lock when
 * DVC_DoCallbacks() returns. This means the internal DVC lock is held during callbacks into your code.
 * Be careful of potential deadlocks if you need to hold one of your own locks in a callback.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 *
 * @see DVC_StartOutputTest() function.
 * @see DVC_StartInputTestPlay() function.
 * @see DVC_GetEventHandle() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_DoCallbacks(
	);


/**
 * Returns the version string of the DVC library.
 *
 * The version string is always built the same way: "X.Y.Z".
 * - X is the major revision number.
 * - Y is the minor revision number.
 * - Z is the patch revision number.
 *
 * @note Libraries with the same major and minor versions are backwards compatible.
 *
 * @return A pointer to a null-terminated string containing the DVC library version.
 *
 * @ingroup DVC_Functions
 */
DVC_API const char* DVC_GetVersion(
	);


/**
 * Returns the build number of the DVC library.
 *
 * @return A pointer to a null-terminated string containing the DVC library build number.
 *
 * @ingroup DVC_Functions
 */
DVC_API const char* DVC_GetBuildNumber(
	);


/**
 * Starts communication using external RTP transport. After calling this function DVC will start sending RTP packets
 * using @DVC_PacketsCallbacksConfig::rtp_packet_ready function provided to DVC_Config and will accept RTP packets
 * passed to DVC_HandleRtpPacket() function. If RTCP is enabled and @DVC_PacketsCallbacksConfig::rtcp_packet_ready are
 * set DVC will also send RTCP packets using @DVC_PacketsCallbacksConfig::rtcp_packet_ready function and will accept
 * RTCP packets passed to DVC_HandleRtcpPacket(). The client can stop the communication by calling
 * DVC_StopRtpCommunication().
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p rtp_communication_config
 *                                                                    configuration is invalid or not supported.
 * @retval DVC_ERROR_MISMATCHED_PARAMS                      [FAILURE] The provided combination of parameters in
 *                                                                    \p rtp_communication_config is invalid
 *                                                                    or not supported.
 * @retval DVC_ERROR_LICENSE_REQUIRED                       [FAILURE] The license for codecs is missing/outdated.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] The RTCP callback was not provided when
 *                                                                    initializing DVC library.
 *
 * @see DVC_HandleRtpPacket() function.
 * @see DVC_StopRtpCommunication() functions.
 * @see DVC_RtpCommunicationConfig structure.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StartRtpCommunication(
	DVC_Handle handle                                            /**<[in] Valid client handle as returned
	                                                                 by a successful call to DVC_CreateClient().*/
	, const DVC_RtpCommunicationConfig* rtp_communication_config /**<[in] Optional RTP configurations, can be set to
	                                                                 \p NULL to use default settings.*/
	);


/**
 * Stops communication using external RTP transport. After calling this function DVC will stop generating RTP and RTCP
 * packets.
 *
 * @tip The client can restart the communication by calling DVC_StartRtpCommunication() again.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 *
 * @see DVC_HandleRtpPacket() function.
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StopRtpCommunication(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	);


/**
 * Passes received RTP packet to the DVC.
 *
 * @note Invalid messages will not immediately return any error code unless they are \p NULL or zero length.
 * @note \p data must be provided in big-endian (network) byte order.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p data is \p NULL.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p len is zero.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] RTP communication is not started or is
 *                                                                    already stopped.
 *
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_StopRtpCommunication() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_HandleRtpPacket(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, const void* data                                      /**<[in] Pointer to buffer containing RTP packet.*/
	, unsigned int len                                      /**<[in] Length of the data pointed to by \p data.*/
	);


/**
 * Passes received RTCP packet to the DVC.
 *
 * @note Invalid messages will not return any error code unless they are \p NULL or zero length.
 * @note \p data must be provided in big-endian (network) byte order.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p data is \p NULL.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p len is zero.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] RTP communication is not started or is
 *                                                                    already stopped, or RTCP is disabled for current
 *                                                                    client connection.
 *
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_StopRtpCommunication() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_HandleRtcpPacket(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, const void* data                                      /**<[in] Pointer to buffer containing RTCP packet.*/
	, unsigned int len                                      /**<[in] Length of the data pointed to by \p data.*/
	);


/**
 * Checks if provided codec configuration is supported.
 *
 * @note The configuration may not be supported if the license for a specific codec is missing.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p codec_config is \p NULL.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p codec_config is not supported.
 *
 * @see DVC_CodecConfig structure.
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_IsCodecConfigSupported(
	const DVC_CodecConfig* codec_config                     /**<[in] Codec configuration to check for support.*/
	);


/**
 * Retrieves supported codec configurations array ordered by preference. Codec configurations returned by this function
 * should be used to fill DVC_ProtocolConfig structure.
 *
 * @note Memory for DVC_CodecConfig is allocated and managed inside client library.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p len or \p codec_config is \p NULL.
 *
 * @see DVC_CodecConfig structure.
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetAvailableCodecConfigs(
	const DVC_CodecConfig** codec_config                    /**<[out] Pointer to an array of DVC_CodecConfig structures
	                                                                  with supported configurations.*/
	, unsigned int* len                                     /**<[out] Length of the number of configurations returned.*/
	);


/**
 * Enables additional audio content streaming capability for RTP communication.
 * After a successful call to this function, DVC mixes:
 * - speech captured from current input device with audio content data passed to DVC_HandleAudioContent() function
 *   and pushes the packetised result to DVC_PacketsCallbacksConfig::rtp_packet_ready.
 * - audio stream received using DVC_HandleRtpPacket() with audio content data passed to DVC_HandleAudioContent()
 *   function and renders the result to current output device.
 *
 * @note Audio content streaming will be disabled automatically if DVC_StopRtpCommunication() is called.
 *
 * @note After enabling audio content streaming, codecs working in discontinued transmission mode (e.g. DVC-2) will
 * start operating in continuous transmission mode.
 *
 * @note Audio content streaming with codecs other than DVC-2 may require additional media server configuration prior
 * to enabling streaming to work correctly.
 *
 * @note Enabling audio content streaming may cause AGC to adapt slower.
 *
 * @note Audio content streaming functionality requires microphone permissions (if applicable on particular platform).
 *
 * @note Audio content streaming functionality requires working both input and output devices.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] RTP communication is not started or streaming already enabled.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 *
 * @see DVC_HandleAudioContent() function.
 * @see DVC_DisableAudioContentStreaming() function.
 * @see DVC_StartRtpCommunication() function.
 * @see DVC_StopRtpCommunication() function.
 * @see DVC_Config structure.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_EnableAudioContentStreaming(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	);


/**
 * Disables additional audio content streaming capability for RTP communication. After calling this function DVC will
 * stop mixing audio.
 *
 * @note Audio content streaming will be disabled automatically if DVC_StopRtpCommunication() is called.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 *
 * @see DVC_HandleAudioContent() function.
 * @see DVC_EnableAudioContentStreaming() function.
 * @see DVC_StopRtpCommunication() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_DisableAudioContentStreaming(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	);


/**
 * Mutes or unmutes the conference client's audio content on local device.
 *
 * @note When audio content is muted, it will not be rendered locally but will still be sent into the conference.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] DVC library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] Audio content streaming is not enabled or
 *                                                                    was already disabled.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 *
 * @see DVC_HandleAudioContent() function.
 * @see DVC_EnableAudioContentStreaming() function.
 * @see DVC_DisableAudioContentStreaming() function.
 * @see DVC_GetAudioContentLocalMute() function.
 * @see DVC_StopRtpCommunication() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetAudioContentLocalMute(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call to DVC_CreateClient().*/
	, int mute                                              /**<[in] Mutes the audio content locally if mute
	                                                                 is non-zero, unmutes otherwise. */
);

/**
 * Retrieves the current conference client's audio content local mute status.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] DVC library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] Audio content streaming is not enabled
 *                                                                    or was already disabled.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 *
 * @see DVC_HandleAudioContent() function.
 * @see DVC_EnableAudioContentStreaming() function.
 * @see DVC_DisableAudioContentStreaming() function.
 * @see DVC_SetAudioContentLocalMute() function.
 * @see DVC_StopRtpCommunication() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetAudioContentLocalMute(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, int *muted                                            /**<[in] Pointer to variable to be filled out with 1
	                                                                 if muted locally and 0 otherwise.*/
);


/**
 * Passes audio content data to the DVC.
 *
 * @note Provided \p data buffer must contain 20 ms of uncompressed audio data in PCM/L16, 1 channel, 32kHz format,
 * big-endian (network) byte order.
 * @note Audio content is only processed when current input and output devices are valid and not paused.
 * Data is temporarily discarded when device change event occurs.
 * @note Audio content will be processed regardless of any input or output mute.
 * @note Audio data must be provided regularly every 20 ms with a sufficiently low jitter, ideally less than 20 ms.
 * @note Audio content streaming can be paused and resumed using DVC_PauseAudio() and DVC_ResumeAudio() APIs.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p data is \p NULL.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p len is zero or \p data is invalid.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] Audio content streaming is not enabled or was
 *                                                                    already disabled.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 *
 * @see DVC_EnableAudioContentStreaming() function.
 * @see DVC_DisableAudioContentStreaming() function.
 * @see DVC_SetAudioContentLocalMute() function.
 * @see DVC_GetAudioContentLocalMute() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_HandleAudioContent(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, const void* data                                      /**<[in] Pointer to buffer containing audio content data.*/
	, unsigned int len                                      /**<[in] Length of the data pointed to by \p data.*/
	);


/**
 * Selects the audio device to be used by all connections; all connections share the same input and output devices.
 * What device will actually be used at any given time, and whether this function may be called, depends on the device
 * selection policy in force at the time of the call.
 *
 * Support for device \p direction is dependent on platform being used:
 * - DVC_DIRECTION_INPUT or DVC_DIRECTION_OUTPUT is valid on desktop platforms only.
 * - DVC_DIRECTION_INPUT_AND_OUTPUT is valid on Android platform only.
 *
 * @note On iOS use system device control to change device.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] The specified device does not exist.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p direction invalid.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 * @retval DVC_ERROR_NOT_PERMITTED                          [FAILURE] Device selection not possible e.g. due to device seleciton policy.
 *
 * @see DVC_DeviceSelectionPolicy enumeration.
 * @see DVC_DeviceGuid structure.
 * @see DVC_Direction enumeration.
 * @see DVC_Config structure.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetDevice(
	DVC_Direction direction                                 /**<[in] The direction (input/output or both) in which the
	                                                                 device should be used.*/
	, DVC_DeviceGuid device_guid                            /**<[in] Guid of device to be used.*/
	);


/**
 * Gets the device information for a specified device GUID.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p device is \p NULL.
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] The specified device does not exist.
 *
 * @see DVC_Device structure.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetDevice(
	DVC_DeviceGuid guid                                     /**<[in] The GUID of the device to lookup.*/
	, DVC_Device* device                                    /**<[out] Pointer to a DVC_Device structure to fill in.*/
	);


/**
 * Resets the audio device which is currently being used.
 * This function will stop and resume audio processing on device for specified \p direction.
 * It is useful for situations where device failed or could not be used (e.g. due to exclusive use by other
 * application) and library do not have feedback from system needed for self recovery.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 *
 * @see DVC_Direction enumeration.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_ResetDevice(
	DVC_Direction direction                                 /**<[in] The direction (input/output or both) in which the
	                                                                 device should be reset.*/
	);


/**
 * Retrieves information about the device currently being used.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] Parameter \p direction is not valid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p device is \p NULL.
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] No device is currently in use. The provided
 *                                                                    parameter \p device must be disregarded.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 *
 * @see DVC_Device structure.
 * @see DVC_Direction enumeration.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetCurrentDevice(
	DVC_Direction direction                                 /**<[in] The direction (input/output or both) in which the
	                                                                 device currently being used.*/
	, DVC_Device* device                                    /**<[out] pointer to DVC_Device structure to be filled with
	                                                                  current input audio device information.*/
	);


/**
 * Fills in the array provided with the audio devices available on the system.
 *
 * @note If \p devices is too small to hold all the devices, @DVC_ERROR_SIZE_TOO_SMALL will be returned and \p len will
 * be set to the minimum size required.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p len or \p devices is \p NULL.
 * @retval DVC_ERROR_SIZE_TOO_SMALL                         [FAILURE] The provided array is too small. The \p len
 *                                                                    parameter contains the minimum size of the array.
 *
 * @see DVC_Device structure.
 * @see DVC_SetDevice()
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetAvailableDevices(
	DVC_Device* devices                                     /**<[out] Array of DVC_Device structures to be filled
	                                                                  in with available devices.*/
	, unsigned int* len                                     /**<[in/out] Length of the supplied array; changed to the
	                                                                     number of devices found on return.*/
	);


/**
 * Pauses audio processing on selected \p direction.
 *
 * When this function is called audio processing on selected \p direction will be paused. It will trigger
 * a DVC_PauseCallback(). This function is intended to be used when the application needs to submit to an OS request
 * to stop processing audio but does not want to leave the conference.
 *
 * @note When audio processing is paused DVC_StartOutputTest(), DVC_StartInputTestRecord() and DVC_StartInputTestPlay()
 * functions will asynchronously return @DVC_ERROR_INTERRUPTED error.
 * @note Calling DVC_PauseAudio() with @DVC_DIRECTION_INPUT_AND_OUTPUT, when neither input nor output is paused,
 * might cause one direction to be paused earlier than the other, with might result in two callbacks, first with
 * direction of either @DVC_DIRECTION_INPUT or @DVC_DIRECTION_OUTPUT, then @DVC_DIRECTION_INPUT_AND_OUTPUT.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 *
 * @see DVC_PauseCallback() callback.
 * @see DVC_ResumeAudio() function.
 * @see DVC_Direction enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_PauseAudio(
	DVC_Direction direction                                 /**<[in] The direction (input/output or both) in which the
	                                                                 audio processing should be paused.*/
	);


/**
 * Resumes audio processing on selected \p direction.
 *
 * When this function is called audio processing on selected \p direction will be resumed. It will trigger
 * a DVC_PauseCallback().
 *
 * @note Calling DVC_ResumeAudio() with DVC_DIRECTION_INPUT_AND_OUTPUT parameter, when both input and output
 * are paused, might cause one direction to be resumed earlier than the other. This might results in two
 * DVC_PauseCallback(), first with direction of either DVC_DIRECTION_INPUT or DVC_DIRECTION_OUTPUT,
 * then DVC_DIRECTION_NONE.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 *
 * @see DVC_PauseCallback() callback.
 * @see DVC_PauseAudio() function.
 * @see DVC_Direction enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_ResumeAudio(
	DVC_Direction direction                                 /**<[in] The direction (input/output or both) in which the
	                                                                 audio processing should be resumed.*/
	);


/**
 * Starts playback to conference from file.
 *
 * @note Works only if file audio mode is used as Audio System Mode during library initialization.
 * @note The file playback will be looped until stopped.
 * @note If the function is called while another or the same file is being played back, the current playback will be
 * stopped and the indicated file will be played from the beginning.
 * @note It is required that files conform to the WAVE audio file format. Additionally these files must have a sample
 * rate of 32KHz, be single channel (mono) and be in IEEE 32-bit floating point data format.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] Library is not running in file audio mode.
 * @retval DVC_ERROR_FILE_OPEN_FAILED                       [FAILURE] Audio file could not be opened.
 *
 * @see DVC_AudioSystemMode enumeration.
 * @see DVC_StopFilePlayback() function.
 * @see DVC_Config structure.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StartFilePlayback(
	const char* input_file                                  /**<[in] A pointer to a null-terminated string containing
	                                                                 the audio file to read from.*/
	);


/**
 * Stops playback to conference from file.
 *
 * @note Works only if file audio mode is used as Audio System Mode during library initialization.
 * @note If playback is stopped and restarted from the same file, it will play from the beginning of the file.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] Library is not running in file audio mode.
 *
 * @see DVC_AudioSystemMode enumeration.
 * @see DVC_StartFilePlayback() function.
 * @see DVC_Initialize() function.
 * @see DVC_Config structure.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StopFilePlayback(
	);


/**
 * Starts recording audio to file.
 *
 * @note Works only if file audio mode is used as Audio System Mode during library initialization.
 * @note If the file already exists, it will be overwritten.
 * @note The format of the file written will always be in the WAVE audio file format, single channel (mono),
 * with a sample rate of 32KHz and data in IEEE 32-bit floating point format.
 * @note Maximum allowed size of output file is limited to 4 gigabytes.
 * If size exceeds the limit, file will be kept open, but data will not be written to it.
 * @note If there is no disk space to write to output file, file will be kept open,
 * but data will not be written to it.
 *
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] Library is not running in file audio mode.
 * @retval DVC_ERROR_FILE_OPEN_FAILED                       [FAILURE] Audio file could not be opened.
 *
 * @see DVC_AudioSystemMode enumeration.
 * @see DVC_StopFileRecording() function.
 * @see DVC_Initialize() function.
 * @see DVC_Config structure.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StartFileRecording(
	const char* output_file                                 /**<[in] A pointer to a null-terminated string containing
	                                                                 the audio file to write to.*/
	);


/**
 * Stops recording audio to file.
 *
 * @note Works only if file audio is used for Audio System Mode.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] Not running in file audio mode.
 *
 * @see DVC_AudioSystemMode enumeration.
 * @see DVC_StartFileRecording() function.
 * @see DVC_Config structure.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StopFileRecording(
	);


/**
 * Allows the caller to set the minimum target delay applied to packets through the downlink jitter buffer.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p latency is not in the valid range
 *                                                                    of 0 to 900.
 *
 * @see DVC_GetDownlinkMinimumLatency() function.
 * @see DVC_CreateClient() function.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetDownlinkMinimumLatency(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, unsigned int latency                                  /**<[in] Minimum downlink target latency in ms, quantized
	                                                                 to a 20ms boundary.*/
	);


/**
 * Retrieves the current minimum target delay applied to packets through the downlink jitter buffer.
 *
 * @note In practice minimum target downlink latency is obtained by taking the maximum of the downlink_latency
 * reported in DVC_ClientStats and the latency returned by DVC_GetDownlinkMinimumLatency().
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p latency is \p NULL.
 *
 * @see DVC_SetDownlinkMinimumLatency() function.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetDownlinkMinimumLatency(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, unsigned int* latency                                 /**<[out] Minimum downlink target latency in ms.*/
	);


/**
 * Configures capture mode setting for current input device.
 *
 * @note Capture mode setting set in \p mode is global: it affects all devices and all conferences.
 * When a device is changed, capture mode is kept.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p mode is invalid.
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] Input device is not avaliable.
 * @retval DVC_ERROR_NOT_PERMITTED                          [FAILURE] Set \p mode is not permimtted for current device.
 *
 * @see DVC_GetCaptureMode() function.
 * @see DVC_CaptureMode enumeration.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetCaptureMode(
	DVC_CaptureMode mode                                    /**<[in] Set capture mode to \p mode.*/
	);


/**
 * Retrieves capture mode setting configured for current input device.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p mode is \p NULL.
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] Input device is not avaliable.
 *
 * @see DVC_SetCaptureMode() function.
 * @see DVC_CaptureMode enumeration.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetCaptureMode(
	 DVC_CaptureMode* mode                                  /**<[out] Returns capture mode in \p mode.*/
	);


/**
* Configures comfort noise level for output devices.
*
* @note Comfort noise level setting set in \p level is valid as long as new value is not applied.
*
* @retval DVC_ERROR_NONE                                   [SUCCESS]
* @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p level is invalid.
* @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
*
* @see DVC_GetComfortNoiseLevel() function.
* @see DVC_ComfortNoiseLevel enumeration.
* @see DVC_Error enumeration.
* @ingroup DVC_Functions
*/
DVC_API DVC_Error DVC_SetComfortNoiseLevel(
	DVC_ComfortNoiseLevel level                             /**<[in] Set comfort noise level to \p level.*/
	);


/**
 * Retrieves comfort noise level setting configured for output devices.
 *
 * @note If comfort noise level was overridden by DVC_SetComfortNoiseLevel (i.e. set to any value other
 * than DVC_COMFORT_NOISE_DEFAULT), \p level will be set to the overriding value. If it was not overridden,
 * the default value for current output device will be output. If there is no override and no current output
 * device, \p level will be set to DVC_COMFORT_NOISE_DEFAULT.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p level is \p NULL.
 *
 * @see DVC_SetComfortNoiseLevel() function.
 * @see DVC_ComfortNoiseLevel enumeration.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetComfortNoiseLevel(
	DVC_ComfortNoiseLevel* level                            /**<[out] Returns comfort noise level in \p level.*/
	);


/**
 * Sets the volume gain for current input device instead of value stored in the device database.
 *
 * @note Supported on desktop platforms only.
 * @note Volume \p gain is valid only for the current input device and will be reset to default value
 * after the device change event occurs.
 * @note Valid value for gain is >= -10 and \p gain <= 40.
 * @note Superfluous use of this API may cause automatic gain control to hit upper boundary and it will have negative
 * impact on audio quality.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p gain is invalid (must be between
 *                                                                    -10 and 40 inclusive).
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] Input device is not avaliable.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 *
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetInputGain(
	int gain                                                /**<[in] Input gain in dB applied locally on the audio
	                                                                 stream before audio processing algorithms.*/
	);


/**
 * Mutes/unmutes input/output on the operating system level (master or application).
 *
 * This API modifies operating system mute status on application level or entire system level. In order to control
 * audio mute status, please use DVC_SetMute() function.
 *
 * @note Please be aware that the applied mute status may not be persistent and may change due to eg. device change.
 * The actual behaviour depends on the operating system. Please refer to an Integration Guide for further details.
 * @note The application level volume control is only available on Windows (output only) and Linux platforms.
 * @note A change to operating system level mute status will trigger a mute change callback.
 * @note It is advised not to change master mute on operating system level without user interaction as this affects
 * audio for all applications.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p control or \p direction
 *                                                                    is invalid.
 * @retval DVC_ERROR_NOT_PERMITTED                          [FAILURE] Setting system mute on current device is not
 *                                                                    permitted.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 *
 * @see DVC_MuteCallback() callback.
 * @see DVC_DoCallbacks() function.
 * @see DVC_SetMute() function.
 * @see DVC_AudioControl enumeration.
 * @see DVC_Direction enumeration.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetSystemMute(
	DVC_AudioControl control                                /**<[in] An audio control for which the mute is to be set.*/
	, DVC_Direction direction                               /**<[in] The direction that this request refers to.*/
	, int mute                                              /**<[in] Mutes the audio if \p mute is non zero, unmutes
	                                                                 the audio otherwise.*/
	);

/**
 * Adjusts the input/output volume on operating system level (master or application).
 *
 * This API modifies the operating system volume on the application or the entire system level.
 *
 * @note Please be aware that the applied volume may not be persistent and may change due to eg. device change.
 * The actual behaviour depends on the operating system. Please refer to an Integration Guide for further details.
 * @note Application level volume control is only available on Windows (output only) and Linux platforms.
 * @note A change to operating system level volume will trigger a volume change callback.
 * @note It is advised not to change master volume on operating system level without user interaction as this affects
 * audio for all applications.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p control, \p direction or \p volume
 *                                                                    is invalid.
 * @retval DVC_ERROR_NOT_PERMITTED                          [FAILURE] Setting system volume on current device is not
 *                                                                    permitted.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 *
 * @see DVC_DoCallbacks() callback.
 * @see DVC_VolumeCallback() function.
 * @see DVC_AudioControl enumeration.
 * @see DVC_Direction enumeration.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetSystemVolume(
	DVC_AudioControl control                                /**<[in] An audio control for which the volume is
	                                                                 to be set.*/
	, DVC_Direction direction                               /**<[in] The direction that this request refers to.*/
	, float volume                                          /**<[in] New volume to be applied, a value between 0.0f
	                                                                 and 1.0f.*/
	);


/**
 * Mutes or unmutes the client's audio.
 *
 * @note Correct values of \p direction parameter are DVC_DIRECTION_INPUT or DVC_DIRECTION_OUTPUT.
 * @note When input is muted, the client will process audio, but will not send it into the communication.
 * When output is muted, the client will receive audio from the media server, but will not render it.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p direction is invalid.
 *
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetMute(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, DVC_Direction direction                               /**<[in] The direction that this request refers to.*/
	, int mute                                              /**<[in] Mutes the client if \p mute is non zero, unmutes
	                                                                 the client otherwise.*/
	);


/**
 * Indicates whether a client is muted. A client may be muted by calling DVC_SetMute() or by other circumstances,
 * such as running an audio test or degraded network conditions. It is not impacted by muting the current audio
 * device e.g. in the operating system, calling DVC_SetSystemMute(), or using a hardware button.
 *
 * @note Correct values of \p direction parameter are DVC_DIRECTION_INPUT or DVC_DIRECTION_OUTPUT.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p muted is \p NULL.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The proviced \p direction is invalid.
 *
 * @see DVC_SetMute(), \ref DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetMute(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, DVC_Direction direction                               /**<[in] The direction that this request refers to.*/
	, int* muted                                            /**<[out] Pointer to variable to be filled out with 1 if
	                                                                  muted and 0 otherwise.*/
	);


/**
 * Set the device tuning database to be used by the client.
 *
 * Client will start using the provided database immediately after this call, input and output devices will be restarted.
 *
 * Provided device database is compatible when the following criteria are met:
 * - MAJOR version component is equal to the MAJOR version component of a built-in device database.
 * - applied device database version is equal or higher than the built-in device database.
 * - device database claims compatibility with the DVC version it is applied to.
 *
 * @note Please refer to the device database release notes for the DVC compatibility information.
 * @note Please note, that applied device database is valid as long as the process using the DVC library is running.
 * Device database needs to be reapplied after restarting the process.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p size is zero.
 * @retval DVC_ERROR_DEVICE_DATABASE_INCOMPATIBLE           [FAILURE] Device database is incompatible or outdated.
 * @retval DVC_ERROR_DEVICE_DATABASE_CORRUPTED              [FAILURE] Internal database structure is corrupted.
 *
 * @see DVC_GetDeviceDatabaseVersion() function.
 *
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetDeviceDatabase(
	const unsigned char* database                           /**<[in] Memory buffer containing the contents of the device
	                                                                 database, if \p NULL the built-in database will be
	                                                                 restored.*/
	, unsigned int size                                     /**<[in] Size of database memory buffer.*/
	);


/**
 * Retrieves the currently used device database version
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p major, \p minor or \p patch
 *                                                                    is \p NULL.
 *
 * @see DVC_SetDeviceDatabase() function.
 *
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetDeviceDatabaseVersion(
	unsigned int* major                                     /**<[out] The major version number.*/
	, unsigned int* minor                                   /**<[out] The minor version number.*/
	, unsigned int* patch                                   /**<[out] The patch version number.*/
	);


/**
 * Sends DTMF in-band or out-of-band codes
 *
 * @note DTMF payload type should be provided during DVC_StartRtpCommunication().
 * @note Duration for out-of-band DTMF is 200ms and for in-band DTMF is 70ms.
 * @note Supported DTMF codes: ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', '#', '*'].
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p tones is \p NULL.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p tones is empty or contains wrong
 *                                                                    characters or parameter \p type is invalid.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] RTP communication is not started.
 * @retval DVC_ERROR_NOT_PERMITTED                          [FAILURE] The provided DTMF \p type is not supported
 *                                                                    for currently used codec.
 *
 * @see DVC_ProtocolConfig structure.
 * @see DVC_DtmfType enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SendDtmf(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, const char* tones                                     /**<[in] null-terminated string containing list
	                                                                 of DTMF tones to send.*/
	, DVC_DtmfType type                                     /**<[in] Type of DTMF, options are: in-band and
	                                                                 out-of-band.*/
	);


/**
 * Loads a wave file making it ready to play.
 *
 * @note Supported sample rates are 8000, 16000, 22050, 24000, 32000, 44100, 48000, 88200, 96000, 192000.
 * @note Supported formats are 16bit PCM, 32bit IEEE float.
 * @note Maximum supported file length is 60 seconds.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p filename is \p NULL.
 * @retval DVC_ERROR_FILE_OPEN_FAILED                       [FAILURE] Failed to open \p filename.
 *
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_OpenWaveFile(
	const char* filename                                    /**<[in] The name of the wave file to load.*/
	, DVC_WaveHandle* handle                                /**<[out] Pointer to a @DVC_WaveHandle to be filled with
	                                                                  a valid wave handle.*/
	);


/**
 * Loads wave from memory making it ready to play
 *
 * @note The \p data provided must include the wav headers.
 * @note Supported sample rates are 8000, 16000, 22050, 24000, 32000, 44100, 48000, 88200, 96000, 192000.
 * @note Supported formats are 16bit PCM, 32bit IEEE float.
 * @note Maximum supported file length is 60 seconds.
 * @note The \p data provided will be copied to the internal buffer.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p data is \p NULL.
 *
 * @see DVC_PlayWave() function.
 * @see DVC_CloseWave() function.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_OpenWaveData(
	const void* data                                        /**<[in] Pointer to the wav file data.*/
	, unsigned int len                                      /**<[in] Length of the data pointed to by \p data.*/
	, DVC_WaveHandle* handle                                /**<[out] Pointer to a @DVC_WaveHandle to be filled with
	                                                                  a valid wave handle.*/
	);


/**
 * Plays the wave-file opened with DVC_OpenWaveFile() or DVC_OpenWaveData().
 *
 * @note The parameter \p gain is a additive factor in dB applied for wave samples
 * @note Valid value for \p gain number between -96 and 30 inclusive.
 * @note Default \p gain is 0.0f
 * @note \p gain equal to -96 will mute the wave file.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] Output device is not avaliable.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \a gain is invalid.
 *
 * @see DVC_StopWave() function.
 * @see DVC_OpenWaveFile() function.
 * @see DVC_OpenWaveData() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_PlayWave(
	DVC_WaveHandle handle                                   /**<[in] Handle to wave.*/
	, float gain                                            /**<[in] Additive factor to apply for wave samples.*/
	, DVC_PlayWaveCallback callback                         /**<[in] Callback to call when the wave is done playing,
	                                                                 may be \p NULL.*/
	, void* callback_context                                /**<[in] Context pointer to be passed back to callback
	                                                                 function, may be \p NULL.*/
	);


/**
 * Stops playback of a currently playing wave file.
 *
 * @note If \p handle is valid and DVC_PlayWave() was not called, @DVC_ERROR_NONE is returned.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 *
 * @see DVC_OpenWaveFile() function.
 * @see DVC_OpenWaveData() function.
 * @see DVC_PlayWave() function.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StopWave(
	DVC_WaveHandle handle                                   /**<[in] Handle to wave.*/
	);


/**
 * Closes a wave.
 *
 * @note If the wave specified by handle is currently being played it will be automatically stopped
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 *
 * @see DVC_OpenWaveFile() function.
 * @see DVC_OpenWaveData() function.
 * @see DVC_PlayWave() function.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_CloseWave(
	DVC_WaveHandle handle                                   /**<[in] Handle to wave.*/
	);


/**
 * Plays spatial audio to test audio output.
 *
 * If provided, the callback will be executed during the next DVC_DoCallbacks()
 * call after the test finished or was interrupted (for example by another
 * test, a call to DVC_StopOutputTest(), current device change or audio processing pause).
 *
 * @note Only one output or input test can run at a time. Calling DVC_StartOutputTest() before the current input
 * or output test has finished will return \ref DVC_ERROR_INVALID_STATE.
 * @note The callback is not triggered if this function call returns an error.
 * @note While in output audio test, people speaking in the conference cannot be heard.
 * @note When audio processing is paused
 * invoking DVC_StartOutputTest() will return \ref DVC_ERROR_DEVICE_NOT_AVAILABLE error.
 * @note It is important not to call any DVC functions beside DVC_ErrorToString() within the provided callback
 * to avoid possible deadlocking issues.
 * @note Update of audio statistics (DVC_AudioStats) aquired by DVC_GetClientStats or DVC_GetAudioStats functions is blocked during execution of this test.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] The library is already in an output or input test.
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] Output device is not avaliable.
 *
 * @see DVC_StopOutputTest() function.
 * @see DVC_DoCallbacks() function.
 * @see DVC_PauseAudio() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StartOutputTest(
	DVC_AudioTestCallback callback                          /**<[in] Pointer to a callback function to call when output
	                                                                 test is done, may be \p NULL.*/
	, void* callback_context                                /**<[in] Context pointer to be passed back to callback
	                                                                 function, may be \p NULL.*/
	);


/**
 * Interrupts and cancels the audio output test if it is currently running.
 * If a callback was set when calling DVC_StartOutputTest(), it will be triggered
 * with error code @DVC_ERROR_INTERRUPTED.
 * If a conference output test is not running, this function does not have any effect.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 *
 * @see DVC_StartOutputTest() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StopOutputTest(
	);


/**
 * Starts recording audio for an input test.
 *
 * The audio will be recorded in a cyclic buffer of up to 10 seconds and can be played back
 * by a call to DVC_StartInputTestPlay().
 * If provided, callback \p callback will be executed during the next DVC_DoCallbacks()
 * call after the test finished or was interrupted (for example by another test, a call to DVC_StopInputTest(),
 * current device change or audio processing pause).
 *
 * @note Input test is done is two separate phases: recording and playback.
 *  - Recording start when calling this function and ends when starting the playback phase
 *    with DVC_StartInputTestPlay().
 *  - Playback phase, starts with a call to DVC_StartInputTestPlay() and
 *    ends once the recorded audio has finished playing back.
 * @note Only one output or input test can run at a time. Calling DVC_StartInputTestRecord() before
 * the current input or output test has finished will return @DVC_ERROR_INVALID_STATE.
 * @note While recording the input audio test, the client is not sending audio to any conference.
 * @note When audio processing is paused
 * invoking DVC_StartInputTestRecord() will return \ref DVC_ERROR_DEVICE_NOT_AVAILABLE error.
 * @note Update of audio statistics (DVC_AudioStats) aquired by DVC_GetClientStats or DVC_GetAudioStats functions is blocked during execution of this test.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] The library is already in an output or input test.
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] Input device is not avaliable.
 *
 * @see DVC_StartInputTestPlay() function.
 * @see DVC_StopInputTest() function.
 * @see DVC_PauseAudio() function.
 * @see DVC_DoCallbacks() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StartInputTestRecord(
	DVC_AudioTestCallback callback                          /**<[in] Pointer to a callback function to call when input
	                                                                 test recording is done, may be \p NULL.*/
	, void* callback_context                                /**<[in] Context pointer to be passed back to callback
	                                                                 function, may be \p NULL.*/
	);


/**
 * Plays the audio recorded by a previous call to DVC_StartInputTestRecord().
 *
 * Refer to DVC_StartInputTestRecord() for a description of the input test steps. If provided, callback \p callback will
 * be executed during the next DVC_DoCallbacks() call after the test finished or was interrupted (for example
 * by another test or a call to DVC_StopInputTest(), current device change or audio processing pause).
 *
 * @note It is important not to call any DVC functions beside DVC_ErrorToString() within the provided callback to avoid
 * possible deadlocking issues.
 * @note Only one output or input test can run at a time.
 * Starting a new test before the previous has completed will return @DVC_ERROR_INVALID_STATE.
 * @note Callback is not executed if this function returns an error.
 * @note While playing the input audio test, people speaking in the conference cannot be heard.
 * @note When audio processing is paused invoking DVC_StartInputTestPlay() will return DVC_ERROR_DEVICE_NOT_AVAILABLE error.
 * @note Update of audio statistics (DVC_AudioStats) aquired by DVC_GetClientStats or DVC_GetAudioStats functions is blocked during execution of this test.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] The library was not recording input before this
 *                                                                    call.
 * @retval DVC_ERROR_DEVICE_NOT_AVAILABLE                   [FAILURE] Output device is not avaliable.
 *
 * @see DVC_StartInputTestRecord() function.
 * @see DVC_StopInputTest() function.
 * @see DVC_DoCallbacks() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StartInputTestPlay(
	DVC_AudioTestCallback callback                          /**<[in] Pointer to a callback function to call when input
	                                                                 test play is done, may be \p NULL.*/
	, void* callback_context                                /**<[in] Context pointer to be passed back to callback
	                                                                 function, may be \p NULL.*/
	);


/**
 * Interrupts and cancels the audio input test if it is currently running.
 *
 * @note If a callback was set when calling DVC_StartInputTestPlay(), it will be executed.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 *
 * @see DVC_StartInputTestRecord() function.
 * @see DVC_StartInputTestPlay() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_StopInputTest(
	);


/**
 * Retrieves talking levels and client ID's of other participants currently heard by the client.
 *
 * @tip The talking levels can be used to implement active speaker notification in client side roster. This function
 * should be polled regularly so that application can correctly render the most up to date talking levels.
 * @note If @DVC_ERROR_SIZE_TOO_SMALL is returned, len will contain the number of talking levels. This can be used
 * to find out how big the \p talking_levels array need to be. However, as the number of talking levels may change rapidly,
 * it can be beneficial to allocate more memory slots than required by \p len.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p talking_levels or \p len
 *                                                                    \p NULL.
 * @retval DVC_ERROR_SIZE_TOO_SMALL                         [FAILURE] The size of the talking_levels array is too small
 *                                                                    for the data. \p len is modified to the size
 *                                                                    required for the failed call.
 * @retval DVC_ERROR_NOT_PERMITTED                          [FAILURE] Getting talking levels is supported only for DVC-2
 *                                                                    codecs.
 *
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetTalkingLevels(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, DVC_TalkingLevel* talking_levels                      /**<[out] Pointer to an allocated array that will be filled
	                                                                  with talking levels of participants currently
	                                                                  being heard.*/
	, unsigned int* len                                     /**<[in/out] Length of the supplied array, changed to the
	                                                                     number of talking levels on success. In case
	                                                                     @DVC_ERROR_SIZE_TOO_SMALL is returned, \p len
	                                                                     contains the number of talking levels.*/
	);


/**
 * Retrieves system input/output audio statistics.
 *
 * @tip \p output_level and \p transmit_level can be used to provide a real-time visual indicator of input and output
 * audio levels.
 * @note Using DVC_StartOutputTest(), DVC_StartInputTestRecord(), DVC_StartInputTestPlay() functions block audio statistics update for the time of the test execution.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p stats is \p NULL.
 *
 * @see DVC_AudioStats structure.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetAudioStats(
	DVC_AudioStats* stats                                   /**<[out] Pointer to a DVC_AudioStats structure to be filled
	                                                                  with information.*/
	);


/**
 * Retrieves statistics and other information about the state of the client.
 *
 * @note Using DVC_StartOutputTest(), DVC_StartInputTestRecord(), DVC_StartInputTestPlay() functions block audio statistics update for the time of the test execution.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p stats is \p NULL.
 *
 * @see DVC_ClientStats structure.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetClientStats(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, DVC_ClientStats* stats                                /**<[out] Pointer to a DVC_ClientStats structure to be
	                                                                  filled with information.*/
	);


/**
 * Retrieves uplink and downlink RTCP statistics.
 *
 * Calling this function is allowed only for clients which are in the RTCP-enabled communication. If this function
 * is called for the client which is not in the RTCP-enabled communication, then error will be returned.
 *
 * The caller must pass pointers to allocated DVC_RtcpStats structures as arguments. If this function returns success,
 * the structures will be filled with the contents of the last RTCP Sender Report or Receiver Report payloads.
 *
 * If the downlink_stats pointer is not \p NULL, the structure will be filled with stats obtained from the server. These
 * stats describe the RTP stream which is sent to the server. If the client did not obtain Sender Report yet,
 * the contents of downlink_stats will be zeroed.
 *
 * If the uplink_stats pointer is not \p NULL, the structure will be filled with stats generated
 * locally and sent to the server. These stats describe the RTP stream which is sent by the server.
 * If the client did not send Sender Report yet, the contents of uplink_stats will be zeroed.
 *
 * If both uplink_stats and downlink_stats are \p NULL pointers, this function does nothing, but still
 * performs all checks and may return error if the client is not in the RTCP-enabled connection.
 *
 * If the last packet obtained or sent to the server is not a Sender Report but a Receiver Report,
 * then the fields specific to Sender Report in the stats will be zeroed.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] The RTCP is disabled for current client
 *                                                                    connection.
 *
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetRtcpStats(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, DVC_RtcpStats* downlink_stats                         /**<[out] The pointer to the RTCP Sender/Receiver Report
	                                                                  structure last obtained by the client from the
	                                                                  server.*/
	, DVC_RtcpStats* uplink_stats                           /**<[out] The pointer to the RTCP Sender/Receiver Report
	                                                                  structure last sent by the client to the server.*/
	);


/**
 * Dumps the library's internal state and audio logs (if audio logging is enabled) to memory. The dump consists
 * of compressed text files dumps of the internal state plus a log history and audio logs if audio logging has been
 * enabled with the DVC_SetAudioLoggingLength() function. The application could choose to write this dump to file
 * on disk or upload to a remote server for debugging/support purposes.
 *
 * @note Memory allocated for dump must be disposed of with a call to DVC_DeleteStateDump().
 * @note Calling DVC_CreateStateDump() again does not invalidate previous memory dumps.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p state_dump is \p NULL
 * @retval DVC_ERROR_RESOURCE_ALLOCATION_FAILED             [FAILURE] Could not allocate memory for state dump.
 *
 * @see DVC_ZippedStateDump structure.
 * @see DVC_DeleteStateDump() function.
 * @see DVC_SetAudioLoggingLength() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_CreateStateDump(
	DVC_Handle handle                                       /**<[in] \p handle of the client to dump the state
	                                                                 for or @DVC_INVALID_HANDLE to dump the state
	                                                                 of the entire library.*/
	, DVC_ZippedStateDump* state_dump                       /**<[out] The address of structure that will contain created
	                                                                  state dump and it's size.*/
	);


/**
 * Frees all the memory allocated for a given state dump returned by DVC_CreateStateDump() function.
 *
 * @note This function may be used when Library is not initialized.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p state_dump is \p NULL.
 *
 * @see DVC_CreateStateDump() function.
 * @see DVC_ZippedStateDump structure.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_DeleteStateDump(
	DVC_ZippedStateDump* state_dump                         /**<[in] Pointer to the state dump.*/
	);


/**
 * Configures internal audio logging used for test and debugging purposes. Enabling audio logs add extra .wav files
 * to the audio dumps generated when calling DVC_CreateStateDump().
 *
 * @note By default, audio logging is disabled.
 * @note While audio logging is enabled, the DVC client library has a significantly increased memory usage as
 * it temporarily stores received and sent audio data. Please consider the available memory as audio logging requires
 * tens of megabytes per minute.
 * @note When \ref DVC_SetAudioLoggingLength function is unable to allocate memory, audio logging will be disabled -
 * audio logging length returned by \ref DVC_GetAudioLoggingLength will be equal to 0 and no new files will be written.
 * User may then try to enable audio logging again by call to \ref DVC_SetAudioLoggingLength with lower length value.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p length exceeds maximum value.
 * @retval DVC_ERROR_RESOURCE_ALLOCATION_FAILED             [FAILURE] Library is unable to allocate required memory
 *                                                                    for desired \p length.
 *
 * @see DVC_CreateStateDump() function.
 * @see DVC_GetAudioLoggingLength() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetAudioLoggingLength(
	unsigned int length                                     /**<[in] Set 0 to disable audio logging, otherwise enables
	                                                                 audio logging for a specified amount of seconds
	                                                                 (maximum 3600).*/
	);


/**
 * Retrieves current internal audio logging length setting.
 *
 * @note By default, audio logging is disabled.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p length is \p NULL.
 *
 * @see DVC_CreateStateDump() function.
 * @see DVC_SetAudioLoggingLength() function.
 * @see DVC_Error enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_GetAudioLoggingLength(
	unsigned int* length                                    /**<[out] Pointer to variable that will be set to 0 if audio
	                                                                  logging is disabled or to the audio logging length
	                                                                  setting otherwise.*/
	);


/**
 * Converts an @DVC_Error error code into a human readable string.
 *
 * @return A pointer to a null-terminated string containing the error description.
 *
 * @ingroup DVC_Functions
 */
DVC_API const char* DVC_ErrorToString(
	const DVC_Error error                                   /**<[in] The error code to be translated.*/
	);


/**
 * Set a key and value used for system description in server syslog.
 *
 * @note Key other than "client_type", create an arbitrary key-value dictionary entry that may be shown in statistics.
 * @note Key equal to "client_type", accept a value of "DESKTOP" (default), "MOBILE", "WEB_PLUGIN" or "DVCP".
 * @note Supported only with connection to server by using DVC-2 codec. For other codecs function will not have
 * any effect.
 * @note Setting system info on selected client is allowed before calling DVC_StartRtpCommunication(). System info field
 *       will be send once after handshake.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_NULL_PARAM                             [FAILURE] The provided \p key or \p value is \p NULL.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p value or \p key is invalid.
 * @retval DVC_ERROR_MISMATCHED_PARAMS                      [FAILURE] The combination of provided \p value and \p key
 *                                                                    is invalid.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] RTP communication was started.
 *
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetSystemInfoField(
	DVC_Handle handle                                       /**<[in] Handle of the client to dump the state for or
	                                                                 @DVC_INVALID_HANDLE to dump the state of the entire
	                                                                 library.*/
	, const char* key                                       /**<[in] Provide "client_type" or a system descriptor key,
	                                                                 e.g. "BROWSER_AGENT", "FINGERPRINT", "MAC_ADDRESS"
	                                                                 .*/
	, const char* value                                     /**<[in] Valid client type or an arbitrary descriptor value,
	                                                                 e.g. "Firefox".*/
	);


/**
 * Notify keyboard usage. This function is optional and may be used to inform typing noise suppression algorithms about keyboard usage.
 *
 * @note This function is not mandatory for typing noise suppression algorithms to work.
 * @note If decided to be used, it should be called every time key press or key release is detected.
 * @note State of typing suppression can be observed via typing_noise_suppression member in DVC_AudioStats structure.
 * @note Supported on desktop platforms only.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_NOT_SUPPORTED                          [FAILURE] The functionality is not supported on this platform.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] Typing noise suppression was not enabled when
 *                                                                    initializing DVC library.
 *
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_NotifyKeyboardUsage(
	);


/**
 * Sets the Forward Error Correction (FEC) bitrate for the client specified by \p handle.
 * Forward Error Correction will be disabled if delay is set to 0 and bitrate is set to \ref DVC_FEC_BITRATE_DISABLED.
 * Forward Error Correction will be enabled if delay is set to a value greater than 0 and
 * smaller than maximum delay value and bitrate is set to value other than \ref DVC_FEC_BITRATE_DISABLED.
 *
 * Default FEC delay is platform dependent (3 on mobiles, 2 on desktops). Set delay to -1 to use default value.
 * Default FEC bitrate value is \ref DVC_FEC_BITRATE_16P0. Set bitrate to \ref DVC_FEC_BITRATE_DEFAULT to use default value.
 *
 * @note In case client and server are communicating using the protocol that doesn't have possibility to use FEC,
 * this method will return \ref DVC_ERROR_NOT_PERMITTED error.
 *
 * @note FEC delay - delay in frames between sending of audio packet and sending of its FEC equivalent.
 *
 * @note FEC bitrate - bitrate used for one FEC frame.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided FEC delay or FEC bitrate is out
 *                                                                    of bounds.
 * @retval DVC_ERROR_INVALID_STATE                          [FAILURE] RTP communication is not started or connected.
 * @retval DVC_ERROR_MISMATCHED_PARAMS                      [FAILURE] The provided combination of \p delay and
 *                                                                    \p bitrate is invalid (one of parameters sets FEC
 *                                                                    enabled, second sets FEC disabled).
 * @retval DVC_ERROR_NOT_PERMITTED                          [FAILURE] FEC cannot be set due to the protocol used
 *                                                                    between client and remote.
 * @see DVC_CreateClient() function.
 * @see \ref DVC_FecBitrate enumeration.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetFec(
	DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
	                                                                 call of DVC_CreateClient().*/
	, int delay                                             /**<[in] New value for the FEC delay.
	                                                                 Value range: -1 as default, 0 as disabled,
	                                                                 1..7 as enabled with specified FEC delay.*/
	, DVC_FecBitrate bitrate                                /**<[in] New value for the FEC bitrate.*/
);


/**
 * Sets network probing parameters.
 *
 * @note Network probing parameters set by this function will be used by current and future RTP connections
 *       associated with provided client \p handle.
 * @note Maximum valid value for \p gap_length is 3600000 milliseconds (one hour).
 * @note Maximum valid value for \p burst_length is 60000 milliseconds (one minute).
 * @note Values of \p gap_length and \p burst_length will be rounded up to the nearest multiple of 20 milliseconds.
 *
 * @retval DVC_ERROR_NONE                                   [SUCCESS]
 * @retval DVC_ERROR_NOT_INITIALIZED                        [FAILURE] Library is not initialized.
 * @retval DVC_ERROR_INVALID_HANDLE                         [FAILURE] The provided \p handle is invalid.
 * @retval DVC_ERROR_INVALID_PARAM                          [FAILURE] The provided \p gap_length or
 *                                                                    \p burst_length is not in the valid range.
 * @see DVC_Error enumeration.
 * @see DVC_CreateClient() function.
 * @ingroup DVC_Functions
 */
DVC_API DVC_Error DVC_SetNetworkProbingParameters(
    DVC_Handle handle                                       /**<[in] Valid client handle as returned by a successful
                                                                     call of DVC_CreateClient().*/
    , unsigned int gap_length                               /**<[in] The elapsed time (in milliseconds) since the last
                                                                     talk burst or last probing packets burst before
                                                                     the subsequent burst is triggered.*/
    , unsigned int burst_length                             /**<[in] The length (in milliseconds) of the probing packets
                                                                     burst. Note that a talk burst will cancel
                                                                     an active probing burst.
                                                                     Set to 0 to disable probing.*/
    );

#ifdef __cplusplus
}
#endif

#endif // DVCLIENT_H_

