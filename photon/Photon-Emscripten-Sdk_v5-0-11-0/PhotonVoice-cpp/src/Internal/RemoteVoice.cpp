/* Exit Games Photon Voice - C++ Client ib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/VoiceClient.h"

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;

		FrameBuffer nullFrame;

		RemoteVoice::RemoteVoice(const Voice::Logger& logger, VoiceClient* pClient, const RemoteVoiceOptions& options, int channelId, int playerId, nByte voiceId, const VoiceInfo& info, nByte /*lastEventNumber*/) // 1st received event instead of 'lastEventNumber' parameter is used to init numbers (see 'started' field)
			: mpVoiceClient(pClient)
			, mInfo(info)
			, mOptions(options)
			, mLogger(logger)
			, mChannelId(channelId)
			, mDelayFrames(0)
			, mPlayerId(playerId)
			, mVoiceId(voiceId)
			, mThreadingEnabled(pClient->getThreadingEnabled())
			, mDecodeRunning(false)
			, mDecodeThreadExit(false)
			, mFrameWritePos(0)
			, mFrameReadPos(0)
			, mEventReadPos(0)
			, mFlushingFrameNum(-1) // if >= 0, we are flushing since the frame with this number: process the queue w/o delays until this frame encountered
			, mStarted(false)
			, mFecEventTimeout(FEC_EVENT_TIMEOUT_INF)
			, mFragmentDetected(false)
		{
			memset(mFecXoredEvents, 0, sizeof(mFecXoredEvents));

			if(!mOptions.mspDecoder)
			{
				EGLOG(DebugLevel::ERRORS, getLogPrefix() + L": decoder is null (set it with options Decoder property or SetOutput method in OnRemoteVoiceInfoAction)");
				return;
			}

			mDecodeRunning = true;

			if(mThreadingEnabled)
			{
				Thread::create(decodeThreadStarter, this);
			}
			else
			{
				EGLOG(DebugLevel::ALL, getLogPrefix() + L": Starting decode singlethreaded");
				mOptions.mspDecoder->open(mInfo);
			}
		}

		RemoteVoice::~RemoteVoice(void)
		{
			if(mThreadingEnabled)
			{
				mDecodeThreadExit = true;
				while(mDecodeRunning);
			}
			else
			{
				if(mOptions.mspDecoder)
					mOptions.mspDecoder->close();
			}

			if(mOptions.mpRemoteVoiceRemoveAction)
				mOptions.mpRemoteVoiceRemoveAction(mOptions.mpRemoteVoiceRemoveActionOpaque);
		}

		const VoiceInfo& RemoteVoice::getInfo(void) const
		{
			return mInfo;
		}

		const RemoteVoiceOptions& RemoteVoice::getOptions(void) const
		{
			return mOptions;
		}

		int RemoteVoice::getDelayFrames(void) const
		{
			return mDelayFrames;
		}

		void RemoteVoice::setDelayFrames(int v)
		{
			mDelayFrames = v;
		}

		int RemoteVoice::getChannelId(void) const
		{
			return mChannelId;
		}

		JString RemoteVoice::getName(void) const
		{
			return JString(L"Remote v#") + mVoiceId + " ch#" + mpVoiceClient->channelStr(mChannelId) + " p#" + mPlayerId;
		}

		JString RemoteVoice::getLogPrefix(void) const
		{
			return JString(L"[PV] ") + getName();
		}

		void RemoteVoice::receiveBytes(const FrameBuffer& receivedBytes, nByte evNumber)
		{
			if(!mDecodeRunning)
				return;

			if(receivedBytes.isConfig())
			{
				if(receivedBytes.getFlags() & FrameFlags::MaskFrag)
					EGLOG(DebugLevel::ERRORS, getLogPrefix() + L": ev#" + evNumber + " fr#" + receivedBytes.getFrameNum() + " wr#" + mFrameWritePos + ", flags: " + static_cast<nByte>(receivedBytes.getFlags()) + ": config frame can't be fragmented");
				else
				{
					// Prevents the very unlikely infinite growth.
					// IsEmpty seems to be faster than Count and the queue is mostly empty.
					while(mConfigFrameQueue.getSize() > 10)
						mConfigFrameQueue.dequeue();
					mConfigFrameQueue.enqueue(receivedBytes);
				}

				// put it also in the normal frame buffer to avoid processing it as a lost frame
			}
			// to avoid multiple empty frames injeciton to the decoder at startup when the current frame number is unknown.
			if(!mStarted && !receivedBytes.isFEC())
			{
				mStarted = true;
				mFrameReadPos = receivedBytes.getFrameNum();
				mFrameWritePos = receivedBytes.getFrameNum();
				mEventReadPos = evNumber;
			}

			if(receivedBytes.isFEC())
			{
				// store the event in FEC events queue
				mFecQueue.Swap(evNumber, receivedBytes);

				// fill xored events array at indexes from xor_start_ev to evNumber - 1 (see sendFrameEvent)
				// [..., flags, size_lsb, size_msb, xor_start_ev]
				for(nByte i=receivedBytes.getArray()[receivedBytes.getSize()-1]; i!=evNumber; ++i)
					 mFecXoredEvents[i] = evNumber;

				mFecEventTimeout = 0;
			}
			else
			{
				// store the event in the main events queue
				mEventQueue.Swap(evNumber, receivedBytes);

				if(receivedBytes.getFlags() & FrameFlags::EndOfStream)
					mFlushingFrameNum = evNumber;

				// there is no synchronization between multiple receiving threads, so the value can be > FEC_EVENT_TIMEOUT_INF, but with a reasonable number of threads this is not a problem
				if(mFecEventTimeout < FEC_EVENT_TIMEOUT_INF)
					++mFecEventTimeout;

				if((nByte)(mFrameWritePos - receivedBytes.getFrameNum()) > 127) // mFrameWritePos < receivedBytes.getFrameNum()
					mFrameWritePos = receivedBytes.getFrameNum();

				if((nByte)(receivedBytes.getFrameNum() - mFrameWritePos) > 127) // mFrameWritePos > receivedBytes.getFrameNum()
				{
					// late frame
					mpVoiceClient->mFramesLate++;
					EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + evNumber + " fr#" + receivedBytes.getFrameNum() + " wr#" + mFrameWritePos + " late: " + static_cast<nByte>(mFrameWritePos > receivedBytes.getFrameNum()) + " r/b " + receivedBytes.getSize() + ", flags: " + static_cast<nByte>(receivedBytes.getFlags()));
				}

				if(!mThreadingEnabled)
					decodeQueue();
			}

			// decrementing after Dispose() does not hurt
//			Interlocked.Decrement(ref receiving);
		}

		void RemoteVoice::decodeQueue()
		{
			nByte df = 0; // the delay between frame writer and reader.
			if(mFlushingFrameNum < 0) // the delay is always 0 if flushing
			{
				if(mDelayFrames > 0)
					df = mDelayFrames > 127 ? 127 : static_cast<nByte>(mDelayFrames); // leave half of the buffer for write/read jitter, 127 is ~1,5 sec. of video (kfi = 30, 30 fragments per kf) or 2.5-7.6 sec. of audio (20-60ms)
				else
					df = mFragmentDetected ? 1 : 0; // at least 1 frame delay required to ensure that all fragments have time to arrive
			}

			nByte maxFrameReadPos = mFrameWritePos - df;
			int nullFramesCnt = 0; // to avoid infinite loop when read frame position does not advance for some reason
			while(nullFramesCnt++ < 100 && (nByte)(maxFrameReadPos - mFrameReadPos) < 127) // maxFrameReadPos >= mFrameReadPos
			{
				while(mConfigFrameQueue.getSize() > 0)
					mOptions.mspDecoder->input(mConfigFrameQueue.dequeue());

				if(mFlushingFrameNum == mFrameReadPos)
				{
					// the frame is flushing, the next frame will be processed with delay
					mFlushingFrameNum = -1;
				}

				nByte eventReadPosPrev = mEventReadPos;
				nByte frameReadPosPrev = mFrameReadPos;
				mEventReadPos += processFrame(mEventReadPos, maxFrameReadPos);

				if(frameReadPosPrev != mFrameReadPos)
					nullFramesCnt = 0;

				for(nByte i=eventReadPosPrev; i!=mEventReadPos; ++i)
				{
					// clear the main event queue
					nByte clearSlot = (nByte)(i - QUEUE_CLEAR_LAG);
					mEventQueue.Swap(clearSlot, nullFrame);

					// clear FEC event queue
					mFecQueue.Swap(clearSlot, nullFrame);
				}
			}
		}

		void RemoteVoice::processLostEvent(nByte lostEvNum, FrameBuffer& lostEv)
		{
			nByte fecEvNum = mFecXoredEvents[lostEvNum];
			FrameBuffer& fecEv = mFecQueue.Lock(fecEvNum);
			if(fecEv.isFEC()) // FEC event exists
			{
				if(recoverLostEvent(lostEvNum, lostEv, fecEvNum, fecEv)) // puts recovered event in lost event's slot via ref f
					mpVoiceClient->mFramesRecovered++;
			}
			else
				EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + lostEvNum + " FEC failed to recover because of non-FEC event in FEC events lookup array at index " + fecEvNum + " (" + (fecEv.isEmpty() ? L"empty" : JString("flags: ") + static_cast<nByte>(fecEv.getFlags())) + ")");
			mFecQueue.Unlock(fecEvNum);
		}

		bool RemoteVoice::recoverLostEvent(nByte lostEvNum, FrameBuffer& lostEv, nByte fecEvNum, FrameBuffer& fecEv)
		{
			mpVoiceClient->mFramesTryFEC++;
			// see sendFrameEvent():
			// [..., flags, size_lsb, size_msb, xor_start_ev]
			int last = fecEv.getSize();
			nByte frNumber = fecEv.getArray()[last - 5];
			nByte flags = fecEv.getArray()[last - 4];
			int size = fecEv.getArray()[last - 3] + (fecEv.getArray()[last - 2] << 8);
			nByte from = fecEv.getArray()[last - 1];

			// lock all events required for xor
			// end event number = fecEvNum - 1 (see sendFrameEvent)
			for(nByte i=from; i!=fecEvNum; ++i)
			{
				if(i != lostEvNum) // all but lost
				{
					if(mEventQueue.Lock(i).isEmpty()) // another lost, can't recover: unlock all and abort
					{
						for(nByte j=from; j!=(nByte)(i+1); ++j)
							if(j != lostEvNum) // all but lost
								mEventQueue.Unlock(j);

						EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + lostEvNum + " FEC failed to recover from events " + from + "-" + fecEvNum + " because at least 2 events are lost");

						return false;
					}
				}
			}

			// xor directly into FEC event buffer and unlock xored frames
			for(nByte i=from; i!=fecEvNum; ++i)
			{
				if(i != lostEvNum) // all but lost
				{
					const FrameBuffer& xf = mEventQueue[i];
					for(int j=0; j<xf.getSize(); ++j)
						fecEv.getArray()[j] ^= xf.getArray()[j];
					flags ^= xf.getFlags();
					frNumber ^= xf.getFrameNum();
					size -= xf.getSize();
					mEventQueue.Unlock(i);
				}
			}

			if(size >= 0 && size <= fecEv.getSize())
			{
				// move FEC event with recovered data to the lost event's slot...
				lostEv = FrameBuffer(fecEv, 0, size, (FrameFlags::Enum)flags, frNumber);
				// ... from FEC event slot in FEC queue
				fecEv = nullFrame;
				EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + lostEvNum + " fr#" + lostEv.getFrameNum() + " FEC recovered from events " + from + "-" + fecEvNum + ", size: " + +size);
				return true;
			}
			else
			{
				EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + lostEvNum + " FEC failed to recover from FEC event of size " + fecEv.getSize() + " because of wrong resulting size " + size);
				return false;
			}
		}

		// returns the number of events we have advanced
		// the caller passes 'eventReadPos' field to this method and updates it with returned value for clarity
		nByte RemoteVoice::processFrame(nByte begEvNum, nByte maxFrameReadPos)
		{
			FrameBuffer& begEv = mEventQueue.Lock(begEvNum);

			// try to recover lost event if we had FEC events recenty
			if(begEv.isEmpty() && mFecEventTimeout < FEC_EVENT_TIMEOUT_INF)
				processLostEvent(begEvNum, begEv);

			if(begEv.isConfig()) // skip config frame processed in configFrameQueue
			{
				mEventQueue.Unlock(begEvNum);
				mFrameReadPos++;

				return 1;
			}

			if(begEv.isEmpty())
			{
				EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + begEvNum + " fr#" + begEv.getFrameNum() + " wr#" + mFrameWritePos + " rd#" + mFrameReadPos + " lost event");
				mEventQueue.Unlock(begEvNum);
				mpVoiceClient->mEventsLost++;

				return 1;
			}

			// issue null frames if mFrameReadPos is behind the current event frame
			while(mFrameReadPos != begEv.getFrameNum())
			{
				EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + begEvNum + " fr#" + begEv.getFrameNum() + " wr#" + mFrameWritePos + " rd#" + mFrameReadPos + " missing frame");
				mOptions.mspDecoder->input(nullFrame);
				mpVoiceClient->mFramesLost++;

				mFrameReadPos++;

				if((nByte)(maxFrameReadPos - mFrameReadPos) >= 127) // maxFrameReadPos < mFrameReadPos, wait for write pos to increment
				{
					mEventQueue.Unlock(begEvNum);

					// mFrameReadPos points to the next frame
					return 0;
				}
			}

			FrameFlags::Enum fragMask = (FrameFlags::Enum)(begEv.getFlags() & FrameFlags::MaskFrag);
			if(fragMask == FrameFlags::FragNotEnd)
			{
				mFrameReadPos++;

				// assemble fragmented
				// scan and lock fragments, move read pointer at the last read slot
				mFragmentDetected = true;
				bool partial = false; // some fragments lost
				nByte fragCount = begEv.getArray()[begEv.getSize()- 1]; // the count of fragments is in the last byte
				if(!fragCount)
				{
					EGLOG(DebugLevel::WARNINGS, getLogPrefix() + L": ev#" + begEvNum + " fr#" + begEv.getFrameNum() + " c#" + fragCount + " 1st event corrupted: 0 fragments count");
					mEventQueue.Unlock(begEvNum);

					return 1;
				}

				int begEvPayloadSize = begEv.getSize() - 1; // - last byte with count, all fragments but the last are of this size
				int maxPayloadSize = begEvPayloadSize * fragCount;

				Buffer<nByte> fragmented(maxPayloadSize);

				// read 1st fragment
				memcpy(fragmented.getArray(), begEv.getArray(), begEvPayloadSize);
				mEventQueue.Unlock(begEvNum);

				int payloadSize = begEvPayloadSize;
				// read all fragments, fill the buffer with 0s if a lost or unfragmented event is encountered, in the hope that the decoder can still get something useful from the partial frame rather than crashing on it
				for(nByte fragEvNum=static_cast<nByte>(begEvNum+1), i=1; i!=fragCount; ++fragEvNum, ++i)
				{
					mpVoiceClient->mFramesReceivedFragments++;

					FrameBuffer& fragEv = mEventQueue.Lock(fragEvNum);

					// try to recover lost event if we had FEC events recenty
					if(fragEv.isEmpty() && mFecEventTimeout < FEC_EVENT_TIMEOUT_INF)
						processLostEvent(fragEvNum, fragEv);

					if(fragEv.getFrameNum() == begEv.getFrameNum() && (fragEv.getFlags() & FrameFlags::FragNotBeg)) // intermediate or last fragment
					{
						int fragEvLength = fragEv.getSize() < begEvPayloadSize ? fragEv.getSize() : begEvPayloadSize; // normally all lenghts are 'begEvPayloadSize' except for the last which can be smaller
						memcpy(fragmented.getArray() + payloadSize, fragEv.getArray(), fragEvLength);
						payloadSize += fragEvLength;
					}
					else
					{
						// either lost event or the event not from this frame, fill the buffer with 0s
						// note: if we are here with the last fragment, the size of the frame will be wrong
						partial = true;
						memset(fragmented.getArray() + payloadSize, 0, begEvPayloadSize);
						payloadSize += begEvPayloadSize;
						EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + begEvNum + " fr#" + begEv.getFrameNum() + " c#" + fragCount + " Fragmented segment zeroed due to invalid fragment ev#" + fragEvNum + " fr#" + fragEv.getFrameNum() + ", flags:" + static_cast<nByte>(fragEv.getFlags()) + (fragEv.isEmpty() ? " NULL" : ""));
					}

					mEventQueue.Unlock(fragEvNum);
				}

				FrameBuffer fragFrame(fragmented, 0, payloadSize, begEv.getFlags(), begEv.getFrameNum());

				mpVoiceClient->mFramesReceivedFragmented++;
				if(partial)
					++mpVoiceClient->mFramesFragPart;
				EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + begEvNum + " fr#" + fragFrame.getFrameNum() + " c#" + fragCount + " Fragmented assembled from events " + begEvNum + "-" + (nByte)(begEvNum + fragCount - 1) + ", size: " + payloadSize + ", flags: " + static_cast<nByte>(begEv.getFlags()));

				mOptions.mspDecoder->input(fragFrame);

				return fragCount;
			}
			else if(fragMask == FrameFlags::None) // unfragemented
			{
				++mFrameReadPos;

				mOptions.mspDecoder->input(begEv);
			}
			else // unexected fragment
			{
				// EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + begEvNum + " fr#" + begEv.FrameNum + " wr#" + frameWritePos + " Unexpected Fragment" + ", flags: " + begEv.Flags);
				// we get here when the 1st fragment is lost
				// TODO: we could skip to the last event of continuous fragments segment (end of the frame in the best case) to avoid repeatedly calling processFrame() on each fragment
				mpVoiceClient->mEventsLost++;
			}

			mEventQueue.Unlock(begEvNum);

			return 1;
		}

		void RemoteVoice::decodeThreadStarter(void* args)
		{
			RemoteVoice* v = static_cast<RemoteVoice*>(args);
			v->decodeThread();
		}

		void RemoteVoice::decodeThread(void)
		{
			SharedPointer<IDecoder> decoder = mOptions.mspDecoder;

			EGLOG(DebugLevel::INFO, getLogPrefix() + L": Starting decode thread");
			decoder->open(mInfo);

			while(!decoder->getError().length())
			{
				if(mDecodeThreadExit)
					break;
				decodeQueue();
			}
			if(decoder->getError().length())
				EGLOG(DebugLevel::ERRORS, getLogPrefix() + L": Error in decode thread: " + decoder->getError());

			decoder->close();

			EGLOG(DebugLevel::INFO, getLogPrefix() + L": Exiting decode thread");

			mDecodeRunning = false;
		}

		JString& RemoteVoice::toString(JString& retStr, bool withTypes) const
		{
			return retStr
				+= JString(L"{")
				+ L"info=" + mInfo.toString(withTypes)
				+ L" options=" + mOptions.toString(withTypes)
				+ L" channelId=" + mChannelId
				+ L" playerId=" + mPlayerId
				+ L" voiceId=" + mVoiceId
				+ L"}";
		}
	}
}