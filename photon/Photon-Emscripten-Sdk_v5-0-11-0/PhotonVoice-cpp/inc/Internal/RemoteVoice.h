/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "PhotonVoice-cpp/inc/RemoteVoiceOptions.h"
#include "PhotonVoice-cpp/inc/Buffer.h"

namespace ExitGames
{
	namespace Voice
	{
		class VoiceClient;
		class VoiceInfo;
		class IDecoder;
		class IDecoderDirect;

		class RemoteVoice : public Common::ToString
		{
			class RingBuffer
			{
				enum { SIZE = 256 };

				FrameBuffer buf[SIZE];
				Common::Helpers::Spinlock bufLock[SIZE];

			public:
				RingBuffer()
				{
					Clear();
				}

				FrameBuffer& Lock(int i)
				{
					bufLock[i].lock();
					return buf[i];
				}

				void Unlock(int i)
				{
					bufLock[i].unlock();
				}

				void Swap(int i, const FrameBuffer& f)
				{
					Lock(i);
					buf[i] = f;
					Unlock(i);
				}

				const FrameBuffer& operator[](int i)
				{
					return  buf[i];
				}

				void UnloclAll()
				{
					for(int i=0; i<SIZE; ++i)
						bufLock[i].unlock();
				}

				void Clear()
				{
					FrameBuffer c;
					for(int i=0; i<SIZE; ++i)
						buf[i] = c;
				}
			};

		public:
			using ToString::toString;

			RemoteVoice(const Voice::Logger& logger, VoiceClient* pClient, const RemoteVoiceOptions& options, int channelId, int playerId, nByte voiceId, const VoiceInfo& info, nByte lastEventNumber);
			virtual ~RemoteVoice(void);

			// Client.RemoteVoiceInfos support
			const VoiceInfo& getInfo(void) const;
			const RemoteVoiceOptions& getOptions(void) const;
			int getChannelId(void) const;
			void setDelayFrames(int);
			int getDelayFrames(void) const;

			void receiveBytes(const FrameBuffer& receivedBytes, nByte evNumber);

			virtual Common::JString& toString(Common::JString& retStr, bool withTypes=false) const;
		protected:
			Common::JString getName(void) const;
			Common::JString getLogPrefix(void) const;
		private:
			void decodeThread(void); // IDecoder* decoder
			void decodeQueue(void);
			void processLostEvent(nByte lostEvNum, FrameBuffer& lostEv);
			bool recoverLostEvent(nByte lostEvNum, FrameBuffer& lostEv, nByte fecEvNum, FrameBuffer& fecEv);
			nByte processFrame(nByte begEvNum, nByte maxFrameReadPos);

			void static decodeThreadStarter(void* args);

			VoiceClient* mpVoiceClient; // TODO: only minor features of client needed in RemoteVoice

			VoiceInfo mInfo;
			RemoteVoiceOptions mOptions;
			const Voice::Logger& mLogger;
			int mChannelId;

			// The delay between frameQueue writer and reader.
			// Originally designed for simple streams synchronization.
			// In DeliveryMode.UnreliableUnsequenced Photon transport mode, helps to save late events (although some out-of-order events are managed to process in order even with 0 delay)
			// For FEC, should be >= FEC injection interval to fully utilize a FEC event belonging to different frames. A fragmented frame may have FEC events belonging to this frame only.
			// As soon as the receiver finds a fragmented frame in the stream, it sets the actual delay to be at least 1 to ensure that all fragments have time to arrive.
			int mDelayFrames;

			int mPlayerId;
			nByte mVoiceId;
			bool mThreadingEnabled;

			std::atomic<bool> mDecodeRunning;
			std::atomic<bool> mDecodeThreadExit;

			RingBuffer mEventQueue;
			// updated by an event with the most recent frame number
			nByte mFrameWritePos;
			// the frame to read in the next processFrame() call
			nByte mFrameReadPos;
			// the event to read in the next processFrame() call
			nByte mEventReadPos;
			int mFlushingFrameNum = -1; // if >= 0, we are flushing since the frame with this number: process the queue w/o delays until this frame encountered
			// The queue of frames guaranteed to be processed.
			// These are currently only video config frames sent reliably w/o fragmentation.
			// Event queue processor can drop a config frame if it's delivered later than its neighbours.
			// Config frames are rare (usually 1 in decoder lifetime), we can use a dynamic queue for them.
			Common::Helpers::SpinLockable<Common::JQueue<FrameBuffer > > mConfigFrameQueue;
			bool mStarted = false;

			// FEC events are processed in a sepearate queue to avod timing and decoding issues (lost FEC event cannot be distinguished from regular lost event)
			RingBuffer mFecQueue;
			// every FEC event writes its event number at indexes of events it's xored from
			// it's not cleared from no more in use FEC events, so it can point to the wrong FEC event but in the worst case decoder processes corrupted frame instead missing
			nByte mFecXoredEvents[256];
			enum { FEC_EVENT_TIMEOUT_INF = 127 };
			// number of events since last FEC event, used to optimize FEC events presence check
			nByte mFecEventTimeout = FEC_EVENT_TIMEOUT_INF;

			// Keep already processed frames for FEC until the read pointer is ahead by that many slots.
			// Should be > FEC events injection period
			enum { QUEUE_CLEAR_LAG = 64 };

			// A simple way to ensure that the delay is at least 1 frame if the stream has fragmented frames: the flag set once and never reset even if the sender stops sending fragmented frames.
			// The first fragmented frame still may be processed too early with partial loss if DelayFrames is 0.
			bool mFragmentDetected;
		};
	}
}