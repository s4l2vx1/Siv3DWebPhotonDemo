/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#include "PhotonVoice-cpp/inc/VoiceClient.h"

/** @file PhotonVoice-cpp/inc/Voice.h */

namespace ExitGames
{
	namespace Voice
	{
		using namespace Common;
		using namespace Common::Helpers;
		using namespace Common::MemoryManagement;

		VoiceCreateOptions::VoiceCreateOptions()
			: mInterestGroup(0)
			, mTargetPlayersEnabled(false)
			, mDebugEchoMode(false)
			, mReliable(false)
			, mEncrypt(false)
			, mFragment(false)
			, mFEC(0)
		{
		}

		VoiceCreateOptions& VoiceCreateOptions::setEncoder(const SharedPointer<IEncoder>& v)
		{
			mspEncoder = v;
			return *this;
		}

		VoiceCreateOptions& VoiceCreateOptions::setInterestGroup(nByte v)
		{
			mInterestGroup = v;
			return *this;
		}

		VoiceCreateOptions& VoiceCreateOptions::setTargetPlayers(const int* targetPlayers, int numTargetPlayers)
		{
			mTargetPlayersEnabled = true;
			mTargetPlayers = JVector<int>(targetPlayers, numTargetPlayers);
			return *this;
		}

		VoiceCreateOptions& VoiceCreateOptions::setDebugEchoMode(bool v)
		{
			mDebugEchoMode = v;
			return *this;
		}

		VoiceCreateOptions& VoiceCreateOptions::setReliable(bool v)
		{
			mReliable = v;
			return *this;
		}

		VoiceCreateOptions& VoiceCreateOptions::setEncrypt(bool v)
		{
			mEncrypt = v;
			return *this;
		}

		VoiceCreateOptions& VoiceCreateOptions::setFragment(bool v)
		{
			mFragment = v;
			return *this;
		}

		VoiceCreateOptions& VoiceCreateOptions::setFEC(int v) {
			mFEC = v;
			return *this;
		}



		int LocalVoice::getChannelId(void) const
		{
			return mChannelId;
		}

		/**
		  If InterestGroup != 0, voice's data is sent only to clients listening to this group.*/
		nByte LocalVoice::getInterestGroup(void) const
		{
			return mInterestGroup;
		}

		void LocalVoice::setInterestGroup(nByte group)
		{
			mInterestGroup = group;
		}

		/**
		  Returns Info structure assigned on local voice cration.*/
		const VoiceInfo& LocalVoice::getInfo(void) const
		{
			return mInfo;
		}

		nByte LocalVoice::getId(void) const
		{
			return mId;
		}

		nByte LocalVoice::getEvNumber(void) const
		{
			return mEvNumber;
		}

		bool LocalVoice::getTransmitEnabled(void) const
		{
			return mTransmitEnabled;
		}
		void LocalVoice::setTransmitEnabled(bool transmitEnabled)
		{
			mTransmitEnabled = transmitEnabled;
		}

		/**
		  Returns true if stream broadcasts.*/
		bool LocalVoice::getIsCurrentlyTransmitting(void) const
		{
			return GETTIMEMS() - mLastTransmitTime < NO_TRANSMIT_TIMEOUT_MS;
		}

		/**
		  Sent frames counter.*/
		int LocalVoice::getFramesSent(void) const
		{
			return mFramesSent;
		}

		/**
		  Sent frames counter.*/
		int LocalVoice::getFramesSentFragmented(void) const
		{
			return mFramesSentFragmented;
		}

		/**
		  Sent frames counter.*/
		int LocalVoice::getFramesSentFragments(void) const
		{
			return mFramesSentFragments;
		}

		/**
		  Sent frames bytes counter.*/
		int LocalVoice::getFramesSentBytes(void) const
		{
			return mFramesSentBytes;
		}

		/**
		  Send data reliable.*/
		bool LocalVoice::getReliable(void) const
		{
			return mReliable;
		}

		void LocalVoice::setReliable(bool reliable)
		{
			mReliable = reliable;
		}

		/**
		  Send data encrypted.*/
		bool LocalVoice::getEncrypt(void) const
		{
			return mEncrypt;
		}

		void LocalVoice::setEncrypt(bool encrypt)
		{
			mEncrypt = encrypt;
		}

		/**
		  Fragment large outgoing frames.*/
		bool LocalVoice::getFragment(void) const
		{
			return mFragment;
		}

		void LocalVoice::setFragment(bool fragment)
		{
			mFragment = fragment;
		}

		/**
		  If > 0, send a FEC frame after N regular frames.*/
		int LocalVoice::getFEC(void) const
		{
			return mFEC;
		}

		void LocalVoice::setFEC(int fec)
		{
			mFEC = fec;
		}

		/**
		  If true, outgoing stream routed back to client via server same way as for remote client's streams.
		  @details
		  Can be swithed any time. OnRemoteVoiceInfoAction and OnRemoteVoiceRemoveAction are triggered if required.
		  This functionality availability depends on transport.*/
		bool LocalVoice::getDebugEchoMode(void) const
		{
			return mDebugEchoMode;
		}

		void LocalVoice::setDebugEchoMode(bool mode)
		{
			if(mDebugEchoMode != mode)
			{
				mDebugEchoMode = mode;
				if(isJoined())
				{
					JVector<int> empty;
					if(mDebugEchoMode)
					{
						sendVoiceInfoAndConfigFrame(true, &empty);
					}
					else
					{
						sendVoiceRemove(true, &empty);
					}
				}
			}
		}

		/**
		  Disables target player. The stream is broadcast to evryone in the room.
		  @sa
		   setTargetPlayers()*/
		void LocalVoice::setTargetPlayersDisabled()
		{
			if(mTargetPlayersEnabled)
			{
				sendVoiceRemove(false, &mTargetPlayers);
				sendVoiceInfoAndConfigFrame(false, NULL);
			}

			mTargetPlayersEnabled = false;
		}

		/**
		  Set the target players list.
		  @details
		  After this call the stream never reaches a player not specified in the list (possibly except the sending client itself in debug echo mode).
		  Passing (NULL, 0) as parameters stops sending to anyone.
		  @param targetPlayers an array of integer values that correspond to the player numbers of the intended receivers
		  @param numTargetPlayers the element count of the array that is passed for targetPlayers
		  @sa
		   setTargetPlayersDisabled(), setDebugEchoMode()*/
		void LocalVoice::setTargetPlayers(const int* targetPlayers, int numTargetPlayers)
		{
			JVector<int> tpNew = JVector<int>(targetPlayers, numTargetPlayers);

			if(isJoined())
			{
				if(mTargetPlayersEnabled)
				{
					JVector<int> tpAdd;
					JVector<int> tpRemove;
					for(unsigned int i=0; i<mTargetPlayers.getSize(); ++i)
						if(!tpNew.contains(mTargetPlayers[i]))
							tpRemove.addElement(mTargetPlayers[i]);
					for(unsigned int i=0; i<tpNew.getSize(); ++i)
						if(!mTargetPlayers.contains(tpNew[i]))
							tpAdd.addElement(tpNew[i]);

					sendVoiceRemove(false, &tpRemove);
					sendVoiceInfoAndConfigFrame(false, &tpAdd);
				}
				else // either old or new is enabled, no need to find the diff between sets
				{
					sendVoiceInfoAndConfigFrame(false, &tpNew);
				}
				// else both are null, no action required
			}

			mTargetPlayersEnabled = true;
			mTargetPlayers = tpNew;
		}

		JString LocalVoice::getName(void) const
		{
			return JString(L"Local v#") + mId + L" ch#" + mpVoiceClient->channelStr(mChannelId);
		}

		JString LocalVoice::getLogPrefix(void) const
		{
			return JString(L"[PV] ") + getName();
		}

		void LocalVoice::setupEncoder(void)
		{
			if(!mspEncoder)
			{
				EGLOG(DebugLevel::INFO, getLogPrefix() + L": Creating default encoder");
				mspEncoder = createDefaultEncoder(mInfo);
			}
			mspEncoder->setOutput(this, sendFrame);
		}

		IEncoder* LocalVoice::createDefaultEncoder(const VoiceInfo& info)
		{
			return allocate<UnsupportedCodecError>(L"LocalVoice", info.getCodec(), mLogger);
		}

		LocalVoice::LocalVoice(const Voice::Logger& logger) // for dummy voices
			: mLogger(logger)
			, mId(0)
			, mChannelId(0)
			, mEvNumber(0)
			, mInfo()
			, mDebugEchoMode(false)
			, mTargetPlayersEnabled(false)
			, mpVoiceClient(NULL)
			, mThreadingEnabled(false)
			, mspEncoder()
			, mInterestGroup(0)
			, mTransmitEnabled(false)
			, mLastTransmitTime(0)
			, mFramesSent(0)
			, mFramesSentFragmented(0)
			, mFramesSentFragments(0)
			, mFramesSentBytes(0)
			, mReliable(false)
			, mEncrypt(false)
			, mFragment(false)
			, mFEC(0)
			, mNoTransmitCnt(0)
			, mFecFlags(FrameFlags::None)
			, mFecFrameNumber(0)
			, mFecTotSize(0)
			, mFecMaxSize(0)
			, mFecCnt(0)
		{
		}

		Buffer<nByte> test() {
			return Buffer<nByte>();
		}

		LocalVoice::LocalVoice(const Voice::Logger& logger, VoiceClient* pVoiceClient, nByte id, const VoiceInfo& voiceInfo, int channelId, const VoiceCreateOptions& options)
			: mLogger(logger)
			, mId(id)
			, mChannelId(channelId)
			, mEvNumber(0)
			, mInfo(voiceInfo)
			, mDebugEchoMode(options.mDebugEchoMode)
			, mTargetPlayersEnabled(options.mTargetPlayersEnabled)
			, mTargetPlayers(options.mTargetPlayers)
			, mpVoiceClient(pVoiceClient)
			, mThreadingEnabled(pVoiceClient->getThreadingEnabled())
			, mspEncoder(options.mspEncoder)
			, mInterestGroup(options.mInterestGroup)
			, mTransmitEnabled(true)
			, mLastTransmitTime(0)
			, mFramesSent(0)
			, mFramesSentFragmented(0)
			, mFramesSentFragments(0)
			, mFramesSentBytes(0)
			, mReliable(options.mReliable)
			, mEncrypt(options.mEncrypt)
			, mFragment(options.mFragment)
			, mFEC(options.mFEC)
			, mNoTransmitCnt(0)
			, mFecFlags(FrameFlags::None)
			, mFecFrameNumber(0)
			, mFecTotSize(0)
			, mFecMaxSize(0)
			, mFecCnt(0)
		{
		}

		LocalVoice::~LocalVoice(void)
		{
			if(mspEncoder)
				mspEncoder->close();
		}

		void LocalVoice::service(void)
		{
			if(mpVoiceClient->getTransport()->isChannelJoined(mChannelId) && mTransmitEnabled)
			{
				Buffer<nByte> b;
				FrameFlags::Enum flags;
				do {
					b = mspEncoder->dequeueOutput(flags);
					if(b.getSize())
						sendFrame(b, flags);
				} while(b.getSize());
			}
		}

		// voiceClient is null for dummy voices
		bool LocalVoice::isJoined(void) const
		{
			return mpVoiceClient && mpVoiceClient->getTransport()->isChannelJoined(mChannelId);
		}

		// prevents calling ITransport event method on empty targets (targetMe is false and targetPlayers is [])
		bool LocalVoice::targetExits(bool targetMe, const JVector<int>* targetPlayers)
		{
			return targetMe || targetPlayers == NULL || targetPlayers->getSize() > 0;
		}

		void LocalVoice::onJoinChannel(void)
		{
			sendVoiceInfoAndConfigFrame(mDebugEchoMode, mTargetPlayersEnabled ? &mTargetPlayers : NULL);
		}

		void LocalVoice::onLeaveChannel(void)
		{
			sendVoiceRemove(mDebugEchoMode, mTargetPlayersEnabled ? &mTargetPlayers : NULL);
		}

		void LocalVoice::onPlayerJoin(int playerId)
		{
			if(!mTargetPlayersEnabled || mTargetPlayers.contains(playerId))
			{
				JVector<int> tp(&playerId, 1);
				sendVoiceInfoAndConfigFrame(false, &tp);
			}
			else
			{
				EGLOG(DebugLevel::INFO, getLogPrefix() + L": player " + playerId + " join is ignored becuase it's not in target players");
			}
		}

		JString LocalVoice::getTargetStr(bool targetMe, const JVector<int>* targetPlayers) const
		{
			JString targetStr;
			if(targetPlayers)
			{
				targetStr = targetPlayers->toString();
			}
			else
			{
				targetStr = L"others";
			}
			if(targetMe)
			{
				targetStr += (targetStr.length() > 0 ? L" and " : L"");
				targetStr += L"me";
			}

			return targetStr;
		}

		void LocalVoice::sendVoiceInfoAndConfigFrame(bool targetMe, const JVector<int>* targetPlayers)
		{
			if(targetExits(targetMe, targetPlayers))
			{
				JString targetStr = getTargetStr(targetMe, targetPlayers);

				EGLOG(DebugLevel::INFO, getLogPrefix() + L": Sending voice info to " + targetStr + ": " + mInfo.toString() + " ev=" + mEvNumber);
				mpVoiceClient->getTransport()->sendVoiceInfo(*this, mChannelId, targetMe, targetPlayers ? targetPlayers->getCArray() : NULL, targetPlayers ? targetPlayers->getSize() : 0);

				if(mConfigFrame.getSize())
				{
					EGLOG(DebugLevel::INFO, getLogPrefix() + L": Sending config frame to " + targetStr);
					sendFrame0(mConfigFrame, FrameFlags::Config, targetMe, targetPlayers, 0, true);
				}
			}
		}

		// targetPlayers is NULL: all players
		void LocalVoice::sendVoiceRemove(bool targetMe, const JVector<int>* targetPlayers)
		{
			if(targetExits(targetMe, targetPlayers))
			{
				EGLOG(DebugLevel::INFO, getLogPrefix() + L": Sending voice remove to " + getTargetStr(targetMe, targetPlayers));
				mpVoiceClient->getTransport()->sendVoiceRemove(*this, mChannelId, targetMe, targetPlayers ? targetPlayers->getCArray() : NULL, targetPlayers ? targetPlayers->getSize() : 0);
			}
		}

		void LocalVoice::sendFrame(void* opaque, const Buffer<nByte>& buffer, FrameFlags::Enum flags)
		{
			static_cast<LocalVoice*>(opaque)->sendFrame(buffer, flags);
		}

		void LocalVoice::sendFrame(const Buffer<nByte>& compressed, FrameFlags::Enum flags)
		{
			if(flags & FrameFlags::Config)
			{
				if(mConfigFrame.getSize() != 0)
				{
					if(mConfigFrame.getSize() == compressed.getSize() && !memcmp(mConfigFrame.getArray(), compressed.getArray(), mConfigFrame.getSize()))
					{
						EGLOG(DebugLevel::ALL, getLogPrefix() + L": Got config frame from encoder, " + mConfigFrame.getSize() + " bytes: repeated, not sending");

						return;
					}
					else
					{
						mConfigFrame = compressed;
						EGLOG(DebugLevel::INFO, getLogPrefix() + L": Got config frame from encoder, " + mConfigFrame.getSize() + " bytes: updated, sending");
					}
				}
				else
				{
					mConfigFrame = compressed;
					EGLOG(DebugLevel::INFO, getLogPrefix() + L": Got config frame from encoder, " + mConfigFrame.getSize() + " bytes: initial, senfing");
				}
			}

			if(mpVoiceClient->getTransport()->isChannelJoined(mChannelId) && mTransmitEnabled)
				sendFrame0(compressed, flags, mDebugEchoMode, mTargetPlayersEnabled ? &mTargetPlayers : NULL, mInterestGroup, mReliable);
		}

		void LocalVoice::sendFrame0(const Buffer<nByte>& compressed, FrameFlags::Enum flags, bool targetMe, const Common::JVector<int>* targetPlayers, nByte interestGroup, bool reliable)
		{

			if(!targetExits(targetMe, targetPlayers))
				return;

			bool fragment = mFragment && !(flags & FrameFlags::Config); // fragmentation of config frames is not supported (see RemoteVoice.configFrameQueue)

			// sending reliably breaks timing
			// consider sending multiple EndOfStream packets for reliability
			if(flags & FrameFlags::EndOfStream)
			{
				//                reliable = true;
			}

			SendFrameParams sendFramePar(mDebugEchoMode, targetPlayers ? targetPlayers->getCArray() : NULL, targetPlayers ? targetPlayers->getSize() : 0, interestGroup, reliable, mEncrypt);

			int maxFragSize = fragment ? mpVoiceClient->getTransport()->getPayloadFragmentSize(sendFramePar) : 0;

			if(maxFragSize <= 0 || compressed.getSize() <= maxFragSize)
				sendFrameEvent(compressed, flags, sendFramePar);
			else
			{
				// We add 1 byte with fragments count to the end of the 1st fragement buffer.
				int totCount = compressed.getSize() + 1; // +1 byte
				int fragCountInt = (totCount + maxFragSize - 1) / maxFragSize;
				// TODO: too many fragments check
				nByte fragCount = static_cast<nByte>(fragCountInt);
				for(nByte i=0; i<fragCount; ++i)
				{
					bool last = i == fragCount - 1;

					nByte flagsFrag = flags;
					if(i > 0) // not 1st
						flagsFrag |= FrameFlags::FragNotBeg;

					if(!last)
						flagsFrag |= FrameFlags::FragNotEnd;

					if(!i)
					{
						int fragSize = maxFragSize + 1;
						Buffer<nByte> frag(fragSize);
						// copy the 1st fragment data to add 1 byte with fragment count
						memcpy(frag.getArray(), compressed.getArray(), fragSize - 1);
						frag.getArray()[maxFragSize] = fragCount;
						sendFrameEvent(frag, (FrameFlags::Enum)flagsFrag, sendFramePar);
					}
					else
					{
						int fragSize = last ? compressed.getSize() % maxFragSize : maxFragSize;
						Buffer<nByte> frag(compressed, i * maxFragSize, fragSize);
						sendFrameEvent(frag, (FrameFlags::Enum)flagsFrag, sendFramePar);
					}

					mFramesSentFragments++;
				}

				EGLOG(DebugLevel::ALL, getLogPrefix() + L": ev#" + mEvNumber + " fr#" + mFramesSent + " c#" + fragCount + " Fragmented sent from events " + (nByte)(mEvNumber - fragCount) + "-" + mEvNumber + ", size: " + compressed.getSize() + ", flags: " + static_cast<nByte>(flags));
				mFramesSentFragmented++;
			}

			mFramesSent++;
			mFramesSentBytes += compressed.getSize();

			if(compressed.getSize() > 0 && !(flags & FrameFlags::Config)) // otherwise the frame is config or control (EOS)
				mLastTransmitTime = GETTIMEMS();
		}

		void LocalVoice::resetFEC()
		{
			//memset(mFecBuffer.getArray(), 0, mFecBuffer.getSize() - FEC_INFO_SIZE); // save a bit on info tail nulling
			mFecBuffer = Buffer<nByte>();
			mFecFlags = 0;
			mFecFrameNumber = 0;
			mFecTotSize = 0;
			mFecMaxSize = 0;
			mFecCnt = 0;
		}

		// Optionally injects FEC events
		void LocalVoice::sendFrameEvent(const Buffer<nByte>& data, FrameFlags::Enum flags, const SendFrameParams& sendFramePar)
		{
			int fec = mFEC;

//            if(mEvNumber % 7 != 0)
			mpVoiceClient->getTransport()->sendFrame(data, flags, mEvNumber, (nByte)mFramesSent, mId, mChannelId, sendFramePar);
//?			if(mDebugEchoMode)
			{
				Lockguard lock(mEventTimestamps);
				mEventTimestamps.put(mEvNumber, GETTIMEMS());
			}
			mEvNumber++;

			if(fec > 0)
			{
				if(mFecBuffer.getSize() < data.getSize() + FEC_INFO_SIZE)
				{
					Buffer<nByte> tmp = mFecBuffer;
					mFecBuffer = Buffer<nByte>(data.getSize() + FEC_INFO_SIZE);
					memcpy(mFecBuffer.getArray(), tmp.getArray(), mFecMaxSize);
					memset(mFecBuffer.getArray() + mFecMaxSize, 0, data.getSize() - mFecMaxSize); // already nulled during allocation?
				}

				for(int i=0; i<data.getSize(); ++i)
				{
					mFecBuffer.getArray()[i] ^= data.getArray()[i];
				}
				mFecMaxSize = mFecMaxSize < data.getSize() ? data.getSize() : mFecMaxSize;
				mFecFlags ^= flags;
				mFecFrameNumber ^= (nByte)mFramesSent;
				mFecTotSize += data.getSize();
				mFecCnt++;
				if(mFecCnt >= fec)
				{
					nByte* arr = mFecBuffer.getArray();
					arr[mFecMaxSize + 0] = mFecFrameNumber;
					arr[mFecMaxSize + 1] = mFecFlags;
					arr[mFecMaxSize + 2] = mFecTotSize & 0xff;
					arr[mFecMaxSize + 3] = (mFecTotSize >> 8) & 0xff;
					arr[mFecMaxSize + 4] = mEvNumber - mFecCnt;
					// assign evNumber to the FEC event but do not increment it to avoid timing and decoding issues (lost FEC event cannot be distinguished from regular lost event)
					// FEC events processed in a separate queue, so numbers do not clash
					// it's easier to process FEC event if its number is 1 more than the last xored event number
					// frame number is not relevant, passing event number instead
					mpVoiceClient->getTransport()->sendFrame(mFecBuffer, FrameFlags::FEC, mEvNumber, mEvNumber, mId, mChannelId, sendFramePar);

					resetFEC();
				}
			}
		}

		/// <summary>Remove this voice from it's VoiceClient (using VoiceClient.RemoveLocalVoice</summary>.</summary>
		void LocalVoice::removeSelf(void)
		{
			if(mpVoiceClient) // dummy voice can try to remove self
				mpVoiceClient->removeLocalVoice(*this);
		}

		JString& LocalVoice::toString(JString& retStr, bool withTypes) const
		{
			return retStr
				+= JString(L"{")
				+ L"id=" + mId
				+ L" channelId=" + mChannelId
				+ L" evNumber=" + mEvNumber
				+ L" info=" + mInfo.toString(withTypes)
				+ L" debugEchoMode=" + (mDebugEchoMode?L"true":L"false")
				+ L" encoder=" + L"{" + TypeName::get(mspEncoder) + L"}"
				+ L" interestGroup=" + mInterestGroup
				+ L" targetPlayers=" + (mTargetPlayersEnabled ? mTargetPlayers.toString() : "Disabled")
				+ L" transmitEnabled=" + (mTransmitEnabled?L"true":L"false")
				+ L" isCurrentlyTransmitting=" + (getIsCurrentlyTransmitting()?L"true":L"false")
				+ L" framesSent=" + mFramesSent
				+ L" framesSentBytes=" + mFramesSentBytes
				+ L" reliable=" + (mReliable?L"true":L"false")
				+ L" encrypt=" + (mEncrypt?L"true":L"false")
				+ L" noTransmitCnt" + mNoTransmitCnt
				+ L"}";
		}
	}
}