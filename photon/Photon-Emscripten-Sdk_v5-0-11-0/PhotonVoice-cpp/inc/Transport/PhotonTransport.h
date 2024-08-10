/* Exit Games Photon Voice - C++ Client Lib
 * Copyright (C) 2004-2024 Exit Games GmbH. All rights reserved.
 * http://www.photonengine.com
 * mailto:developer@photonengine.com
 */

#pragma once

#include "PhotonVoice-cpp/inc/Buffer.h"
#include "PhotonVoice-cpp/inc/VoiceInfo.h"
#include "PhotonVoice-cpp/inc/VoiceClient.h"

namespace ExitGames
{
	namespace Voice
	{
		class VoiceClient;
		class LocalVoice;

		// Photon transports basic support
		class PhotonTransportProtocol
		{
		public:
			PhotonTransportProtocol(const Voice::Logger& logger);

			Common::ValueObject<Common::Object*> buildVoicesInfo(const LocalVoice& v, bool logInfo) const;
			Common::ValueObject<Common::Object*> buildVoiceRemoveMessage(const LocalVoice& v) const;
			void onVoiceFrameEvent(VoiceClient& voiceClient, const Common::Object& content, int channelId, int playerId, int localPlayerId);
			void onVoiceEvent(VoiceClient& voiceClient, const Common::Object& content, int channelId, int playerId, int localPlayerId);
			int getPayloadFragmentSize(const Voice::SendFrameParams& par);
			nByte* buildFrameEvent(const Buffer<nByte>& mData, nByte mVoiceId, nByte mEvNumber, nByte frNumber, FrameFlags::Enum flags, int& outDataOffset);
		private:
			void onVoiceInfo(VoiceClient& voiceClient, int channelId, int playerId, const Common::Object& payload);
			void onVoiceRemove(VoiceClient& voiceClient, int channelId, int playerId, const Common::Object& payload);
			VoiceInfo createVoiceInfoFromEventPayload(const Common::Dictionary<nByte, Common::Object>& h);

			const Voice::Logger& mLogger;
		};
	}
}