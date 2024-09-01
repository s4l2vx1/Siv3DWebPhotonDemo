﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2023 Ryo Suzuki
//	Copyright (c) 2016-2023 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

//-----------------------------------------------
//	Author (OpenSiv3D 実装会)
//	- mak1a
//	- Luke
//	- sthairno
//-----------------------------------------------

# include <Siv3D.hpp>
# include "Multiplayer_Photon.hpp"

namespace s3d::detail {
	static void LogIfError(const Multiplayer_Photon& photon, const int32 errorCode, const StringView errorString)
	{
		if (errorCode)
		{
			photon.logger(U"- [Multiplayer_Photon] errorCode: ", errorCode);
			photon.logger(U"- [Multiplayer_Photon] errorString: ", errorString);
		}
	}
}

// [WEB] extern js functions
namespace s3d::detail
{
	extern "C"
	{
		__attribute__((import_name("siv3dPhotonInitClient")))
		void siv3dPhotonInitClient(const char32* appID, const char32* appVersion, bool verbose, uint8 protocol);

		__attribute__((import_name("siv3dPhotonConnect")))
		bool siv3dPhotonConnect(const char32* userID, const char32* region);

		__attribute__((import_name("siv3dPhotonDisconnect")))
		void siv3dPhotonDisconnect();

		__attribute__((import_name("siv3dPhotonService")))
		void siv3dPhotonService();

		__attribute__((import_name("siv3dPhotonPing")))
		void siv3dPhotonPing();

		__attribute__((import_name("siv3dPhotonIsInLobby")))
		bool siv3dPhotonIsInLobby();

		__attribute__((import_name("siv3dPhotonIsJoinedToRoom")))
		bool siv3dPhotonIsJoinedToRoom();

		__attribute__((import_name("siv3dPhotonGetServerTime")))
		int32 siv3dPhotonGetServerTime();

		__attribute__((import_name("siv3dPhotonGetRoundTripTime")))
		int32 siv3dPhotonGetRoundTripTime();

		__attribute__((import_name("siv3dPhotonSetPingInterval")))
		void siv3dPhotonSetPingInterval(int32 interval);

		__attribute__((import_name("siv3dPhotonJoinRandomRoom")))
		bool siv3dPhotonJoinRandomRoom(uint8 maxPlayers, MatchmakingMode matchmakingMode, const char32* filter);

		__attribute__((import_name("siv3dPhotonJoinRandomOrCreateRoom")))
		bool siv3dPhotonJoinRandomOrCreateRoom(const char32* roomName, const char32* opt, uint8 maxPlayers, MatchmakingMode matchmakingMode, const char32* filter);

		__attribute__((import_name("siv3dPhotonJoinRoom")))
		bool siv3dPhotonJoinRoom(const char32* roomName, bool rejoin);

		__attribute__((import_name("siv3dPhotonCreateRoom")))
		bool siv3dPhotonCreateRoom(bool join, const char32* roomName, const char32* roomOpt);

		__attribute__((import_name("siv3dPhotonReconnectAndRejoin")))
		bool siv3dPhotonReconnectAndRejoin();

		__attribute__((import_name("siv3dPhotonLeaveRoom")))
		void siv3dPhotonLeaveRoom(bool willComeBack);

		__attribute__((import_name("siv3dPhotonChangeInterestGroup")))
		void siv3dPhotonChangeInterestGroup(int32 joinLen, const uint8* join, int32 leaveLen, const uint8* leave);

		__attribute__((import_name("siv3dPhotonRaiseEvent")))
		void siv3dPhotonRaiseEvent(uint8 eventCode, const char* data, const char32* opt);

		__attribute__((import_name("siv3dPhotonGetRoomList")))
		void siv3dPhotonGetRoomList(Array<RoomInfo>* array);

		__attribute__((import_name("siv3dPhotonGetRoomNameList")))
		void siv3dPhotonGetRoomNameList(Array<RoomName>* array);

		__attribute__((import_name("siv3dPhotonGetCurrentRoom")))
		void siv3dPhotonGetCurrentRoom(char32** name, int32* playerCount, int32* maxPlayers, bool* isOpen);

		__attribute__((import_name("siv3dPhotonGetRoomPlayerList")))
		void siv3dPhotonGetRoomPlayerList(Array<LocalPlayer>* array, LocalPlayerID filter);

		__attribute__((import_name("siv3dPhotonGetRoomPlayerIDList")))
		void siv3dPhotonGetRoomPlayerIDList(Array<LocalPlayerID>* array);

		__attribute__((import_name("siv3dPhotonGetIsVisibleInCurrentRoom")))
		bool siv3dPhotonGetIsVisibleInCurrentRoom();

		__attribute__((import_name("siv3dPhotonSetCurrentRoomVisible")))
		void siv3dPhotonSetCurrentRoomVisible(bool isVisible);

		__attribute__((import_name("siv3dPhotonSetCurrentRoomOpen")))
		void siv3dPhotonSetCurrentRoomOpen(bool isOpen);

		__attribute__((import_name("siv3dPhotonUpdateLocalPlayer")))
		void siv3dPhotonUpdateLocalPlayer(LocalPlayerID* localID, bool* isHost, bool* isActive);

		__attribute__((import_name("siv3dPhotonGetMasterClientID")))
		LocalPlayerID siv3dPhotonGetMasterClientID();

		__attribute__((import_name("siv3dPhotonSetUserName")))
		void siv3dPhotonSetUserName(const char32* userName);

		__attribute__((import_name("siv3dPhotonSetMasterClient")))
		void siv3dPhotonSetMasterClient(LocalPlayerID localPlayerID);

		__attribute__((import_name("siv3dPhotonGetPlayerCustomProperty")))
		char32* siv3dPhotonGetPlayerCustomProperty(const char32* key, LocalPlayerID actor);

		__attribute__((import_name("siv3dPhotonGetPlayerCustomProperties")))
		void siv3dPhotonGetPlayerCustomProperties(HashTable<String, String>* table, LocalPlayerID actor);

		__attribute__((import_name("siv3dPhotonSetPlayerCustomProperty")))
		void siv3dPhotonSetPlayerCustomProperty(const char32* key, const char32* value);

		__attribute__((import_name("siv3dPhotonRemovePlayerCustomProperties")))
		char32* siv3dPhotonRemovePlayerCustomProperties(int32 len, const char32** keys);

		__attribute__((import_name("siv3dPhotonGetRoomCustomProperty")))
		char32* siv3dPhotonGetRoomCustomProperty(const char32* key);

		__attribute__((import_name("siv3dPhotonGetRoomCustomProperties")))
		void siv3dPhotonGetRoomCustomProperties(HashTable<String, String>* table);

		__attribute__((import_name("siv3dPhotonSetRoomCustomProperty")))
		void siv3dPhotonSetRoomCustomProperty(const char32* key, const char32* value);

		__attribute__((import_name("siv3dPhotonRemoveRoomCustomProperties")))
		char32* siv3dPhotonRemoveRoomCustomProperties(int32 len, const char32** keys);

		__attribute__((import_name("siv3dPhotonGetVisibleRoomPropertyKeys")))
		void siv3dPhotonGetVisibleRoomPropertyKeys(Array<String>* array);

		__attribute__((import_name("siv3dPhotonSetVisibleRoomPropertyKeys")))
		void siv3dPhotonSetVisibleRoomPropertyKeys(int32 len, const char32** keys);
	}
}

// [WEB] PhotonDetail
namespace s3d
{
	struct Multiplayer_Photon::PhotonDetail
	{
		explicit PhotonDetail(Multiplayer_Photon& context) : m_context(context) {}

		Multiplayer_Photon& m_context;

		int32 m_countGamesRunning = 0;
		int32 m_countPlayersIngame = 0;
		int32 m_countPlayersOnline = 0;

		String m_userNameCache;
		String m_userIDCache;

		ClientState m_clientState = ClientState::Disconnected;

		LocalPlayer m_localPlayer;

		RoomInfo m_currentRoom;

		int32 m_pingInterval = 2000;

		bool joinRandomRoom(const int32 expectedMaxPlayers, MatchmakingMode matchmakingMode, StringView filter)
		{
			if (not InRange(expectedMaxPlayers, 0, 255))
			{
				return false;
			}

			bool result = detail::siv3dPhotonJoinRandomRoom(expectedMaxPlayers, matchmakingMode, filter.data());

			return result;
		}

		bool joinRandomOrCreateRoom(RoomNameView roomName, StringView opt, StringView filter, int32 expectedMaxPlayers, MatchmakingMode matchmakingMode)
		{
			if (not InRange(expectedMaxPlayers, 0, 255))
			{
				return false;
			}

			bool result = detail::siv3dPhotonJoinRandomOrCreateRoom(roomName.data(), opt.data(), expectedMaxPlayers, matchmakingMode, filter.data());

			return result;
		}

		bool joinRoom(const RoomNameView roomName, bool rejoin)
		{
			bool result = detail::siv3dPhotonJoinRoom(roomName.data(), rejoin);

			return result;
		}

		bool joinOrCreateRoom(RoomNameView roomName, StringView opt)
		{
			bool result = detail::siv3dPhotonCreateRoom(true, roomName.data(), opt.data());

			return result;
		}

		bool createRoom(RoomNameView roomName, StringView opt)
		{
			bool result = detail::siv3dPhotonCreateRoom(false, roomName.data(), opt.data());

			return result;
		}

		void leaveRoom(bool willComeBack)
		{
			if (not m_context.isInRoom())
			{
				return;
			}

			m_clientState = ClientState::LeavingRoom;

			detail::siv3dPhotonLeaveRoom(willComeBack);
		}

		void updateLocalPlayer()
		{
			detail::siv3dPhotonUpdateLocalPlayer(&m_localPlayer.localID, &m_localPlayer.isHost, &m_localPlayer.isActive);
		}

		void updateCurrentRoom()
		{
			char32* roomName;

			detail::siv3dPhotonGetCurrentRoom(&roomName, &m_currentRoom.playerCount, &m_currentRoom.maxPlayers, &m_currentRoom.isOpen);

			m_currentRoom.name = String(roomName);

			free(roomName);
		}

		Array<LocalPlayerID> getLocalPlayerIDList()
		{
			Array<LocalPlayerID> result;

			detail::siv3dPhotonGetRoomPlayerIDList(&result);

			return result;
		}

		void connectionErrorReturn(int32 errorCode)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::connectionErrorReturn() [サーバへの接続が失敗したときに呼ばれる]");
			m_context.logger(U"- [Multiplayer_Photon] errorCode: ", errorCode);

			m_context.connectionErrorReturn(errorCode);
		}

		void connectReturn(int32 errorCode, const String& errorString)
		{
			const auto& region = m_context.m_requestedRegion.value();

			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::connectReturn()");
			m_context.logger(U"[Multiplayer_Photon] region: ", region);

			detail::LogIfError(m_context, errorCode, errorString);

			m_context.connectReturn(errorCode, errorString, region, U"");
		}

		void disconnectReturn()
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::disconnectReturn() [サーバから切断されたときに呼ばれる]");

			m_context.disconnectReturn();
		}

		void leaveRoomReturn(int32 errorCode, const String& errorString)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::leaveRoomReturn() [ルームから退出した結果を処理する]");

			detail::LogIfError(m_context, errorCode, errorString);

			m_context.leaveRoomReturn(errorCode, errorString);
		}

		void joinRandomRoomReturn(LocalPlayerID playerID, int32 errorCode, const String& errorString)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::joinRandomRoomReturn()");
			m_context.logger(U"- [Multiplayer_Photon] playerID: ", playerID);

			detail::LogIfError(m_context, errorCode, errorString);

			m_context.joinRandomRoomReturn(playerID, errorCode, errorString);
		}

		void joinRandomOrCreateRoomReturn(LocalPlayerID playerID, int32 errorCode, const String& errorString)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::joinRandomOrCreateRoomReturn()");
			m_context.logger(U"- [Multiplayer_Photon] playerID: ", playerID);

			detail::LogIfError(m_context, errorCode, errorString);

			m_context.joinRandomOrCreateRoomReturn(playerID, errorCode, errorString);
		}

		void joinRoomReturn(LocalPlayerID playerID, int32 errorCode, const String& errorString)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::joinRoomReturn()");
			m_context.logger(U"- [Multiplayer_Photon] playerID: ", playerID);

			detail::LogIfError(m_context, errorCode, errorString);

			m_context.joinRoomReturn(playerID, errorCode, errorString);
		}

		void joinOrCreateRoomReturn(LocalPlayerID playerID, int32 errorCode, const String& errorString)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::joinOrCreateRoomReturn()");
			m_context.logger(U"- [Multiplayer_Photon] playerID: ", playerID);

			detail::LogIfError(m_context, errorCode, errorString);

			m_context.joinRoomReturn(playerID, errorCode, errorString);
		}

		void createRoomReturn(LocalPlayerID playerID, int32 errorCode, const String& errorString)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::createRoomReturn()");
			m_context.logger(U"- [Multiplayer_Photon] playerID: ", playerID);

			detail::LogIfError(m_context, errorCode, errorString);

			m_context.createRoomReturn(playerID, errorCode, errorString);
		}

		void joinRoomEventAction(LocalPlayerID playerID, bool myself)
		{
			Array<LocalPlayerID> localPlayerIDs = getLocalPlayerIDList();
			
			updateCurrentRoom();

			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::joinRoomEventAction() [誰か（自分を含む）が現在のルームに参加したときに呼ばれる]");
			m_context.logger(U"- [Multiplayer_Photon] playerID [参加した人の ID]: ", playerID);
			m_context.logger(U"- [Multiplayer_Photon] isSelf [自分自身の参加？]: ", myself);
			m_context.logger(U"- [Multiplayer_Photon] playerIDs [ルームの参加者一覧]: ", localPlayerIDs);

			m_context.joinRoomEventAction(m_context.getLocalPlayer(), localPlayerIDs, myself);
		}

		void leaveRoomEventAction(LocalPlayerID playerID, bool isSuspended)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::leaveRoomEventAction()");
			m_context.logger(U"- [Multiplayer_Photon] playerID: ", playerID);
			m_context.logger(U"- [Multiplayer_Photon] isInactive: ", isSuspended);

			m_context.leaveRoomEventAction(playerID, isSuspended);
		}

		void customEventAction(LocalPlayerID playerID, uint8 eventCode, char* message)
		{
			Blob blob = Base64::Decode(message, s3d::SkipValidation::Yes); 

			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::customEventAction(Deserializer<MemoryReader>)");
			m_context.logger(U"[Multiplayer_Photon] playerID: ", playerID);
			m_context.logger(U"[Multiplayer_Photon] eventCode: ", eventCode);
			m_context.logger(U"[Multiplayer_Photon] data: ", blob.size(), U" bytes (serialized)");

			Deserializer<MemoryViewReader> reader{blob.data(), blob.size()};

			if (m_context.m_table.contains(eventCode)) {
				auto& receiver = m_context.m_table[eventCode];
				(receiver.second)(m_context, receiver.first, playerID, reader);
			}
			else {
				m_context.customEventAction(playerID, eventCode, reader);
			}
		}

		void onRoomListUpdate()
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::onRoomListUpdate()");

			m_context.onRoomListUpdate();
		}

		void onRoomPropertiesChange()
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::onRoomPropertiesChange()");

			m_context.onRoomPropertiesChange({});
		}

		void onPlayerPropertiesChange(const int playerID)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::onPlayerPropertiesChange()");

			m_context.onPlayerPropertiesChange(playerID, {});
		}

		void onMasterClientChanged(const int newHostID, const int oldHostID)
		{
			m_context.logger(U"[Multiplayer_Photon] Multiplayer_Photon::onHostChange()");

			m_context.onHostChange(newHostID, oldHostID);
		}

		int32 getTimePingInterval()
		{
			return m_pingInterval;
		}

		void setTimePingInterval(int32 interval)
		{
			m_pingInterval = interval;
			detail::siv3dPhotonSetPingInterval(interval);
		}
	};
}

static Multiplayer_Photon::PhotonDetail* g_detail = nullptr;

// [WEB] extern C callback functions
namespace s3d::detail
{
	enum class PhotonCallbackCode : uint8 {
    	ConnectionErrorReturn = 1,
    	ConnectReturn = 11,
    	DisconnectReturn = 12,
    	LeaveRoomReturn = 21,
    	JoinRandomRoomReturn = 22,
    	JoinRandomOrCreateRoomReturn = 23,
    	JoinRoomReturn = 24,
        JoinOrCreateRoomReturn = 25,
    	CreateRoomReturn = 26,
	};

	extern "C"
	{
		__attribute__((used, export_name("siv3dPhotonGetRoomListCallback")))
		void siv3dPhotonGetRoomListCallback(Array<RoomInfo>* array, char32* name, int32 maxPlayers, int32 playerCount, bool isOpen)
		{
			array->push_back({ String(name), playerCount, maxPlayers, isOpen });
			free(name);
		}

		__attribute__((used, export_name("siv3dPhotonGetRoomNameListCallback")))
		void siv3dPhotonGetRoomNameListCallback(Array<RoomName>* array, char32* name)
		{
			array->push_back(String(name));
			free(name);
		}

		__attribute__((used, export_name("siv3dPhotonGetRoomPlayerListCallback")))
		void siv3dPhotonGetRoomPlayerListCallback(Array<LocalPlayer>* array, int32 id, char32* userName, char32* userID, bool isHost, bool isActive)
		{
			array->push_back(LocalPlayer{ id, String(userName), String(userID), isHost, isActive });
			free(userName);
			free(userID);
		}

		__attribute__((used, export_name("siv3dPhotonGetRoomPlayerIDListCallback")))
		void siv3dPhotonGetRoomPlayerIDListCallback(Array<LocalPlayerID>* array, int32 id)
		{
			array->push_back(id);
		}

		__attribute__((used, export_name("siv3dPhotonGeneralCallback")))
		void siv3dPhotonGeneralCallback(uint8 callback, int32 errorCode, char32* errorString_, LocalPlayerID player)
		{
			String errorString { errorString_ != nullptr ? errorString_ : U"" };

			if (g_detail)
			{
				switch (static_cast<PhotonCallbackCode>(callback))
				{
				case PhotonCallbackCode::ConnectionErrorReturn:
					g_detail->connectionErrorReturn(errorCode);
					break;
				case PhotonCallbackCode::ConnectReturn:
					g_detail->connectReturn(errorCode, errorString);
					break;
				case PhotonCallbackCode::DisconnectReturn:
					g_detail->disconnectReturn();
					break;
				case PhotonCallbackCode::LeaveRoomReturn:
					g_detail->leaveRoomReturn(errorCode, errorString);
					break;
				case PhotonCallbackCode::JoinRandomRoomReturn:
					g_detail->joinRandomRoomReturn(player, errorCode, errorString);
					break;
				case PhotonCallbackCode::JoinRandomOrCreateRoomReturn:
					g_detail->joinRandomOrCreateRoomReturn(player, errorCode, errorString);
					break;
				case PhotonCallbackCode::JoinRoomReturn:
					g_detail->joinRoomReturn(player, errorCode, errorString);
					break;
				case PhotonCallbackCode::JoinOrCreateRoomReturn:
					g_detail->joinOrCreateRoomReturn(player, errorCode, errorString);
					break;
				case PhotonCallbackCode::CreateRoomReturn:
					g_detail->createRoomReturn(player, errorCode, errorString);
					break;
				};
			}
			
			free(static_cast<void*>(errorString_));
		}

		__attribute__((used, export_name("siv3dPhotonClientStateChangeCallback")))
		void siv3dPhotonClientStateChangeCallback(int32 state)
		{
			if (not g_detail) return;

			g_detail->m_clientState = static_cast<ClientState>(state);
		}

		__attribute__((used, export_name("siv3dPhotonAppStateChangeCallback")))
		void siv3dPhotonAppStateChangeCallback(int32 countGamesRunning, int32 countPlayersIngame, int32 countPlayersOnline)
		{
			if (not g_detail) return;

			g_detail->m_countGamesRunning = countGamesRunning;
			g_detail->m_countPlayersIngame = countPlayersIngame;
			g_detail->m_countPlayersOnline = countPlayersOnline;
		}

		__attribute__((used, export_name("siv3dPhotonActorJoinCallback")))
		void siv3dPhotonActorJoinCallback(LocalPlayerID playerID, bool myself)
		{
			if (not g_detail) return;
			
			g_detail->joinRoomEventAction(playerID, myself);
		}

		__attribute__((used, export_name("siv3dPhotonActorLeaveCallback")))
		void siv3dPhotonActorLeaveCallback(LocalPlayerID playerID, bool isSuspended)
		{
			if (not g_detail) return;

			g_detail->leaveRoomEventAction(playerID, isSuspended);
		}

		__attribute__((used, export_name("siv3dPhotonCustomEventCallback")))
		void siv3dPhotonCustomEventCallback(LocalPlayerID playerID, uint8 code, char* message)
		{
			if (not g_detail) return;

			g_detail->customEventAction(playerID, code, message);

			free(message);
		}

		__attribute__((used, export_name("siv3dPhotonGetCustomPropertiesCallback")))
		void siv3dPhotonGetCustomPropertiesCallback(HashTable<String, String>* table, char32* key_, char32* value_)
		{
			(*table)[StringView(key_)] = String(value_);

			free(key_);
			free(value_);
		}

		__attribute__((used, export_name("siv3dPhotonGetVisibleRoomPropertyKeysCallback")))
		void siv3dPhotonGetVisibleRoomPropertyKeysCallback(Array<String>* array, char32* key_)
		{
			array->push_back(String(key_));

			free(key_);
		}

		__attribute__((used, export_name("siv3dPhotonOnRoomListUpdateCallback")))
		void siv3dPhotonOnRoomListUpdateCallback()
		{
			if (not g_detail) return;

			g_detail->onRoomListUpdate();
		}

		__attribute__((used, export_name("siv3dPhotonOnRoomPropertiesChangeCallback")))
		void siv3dPhotonOnRoomPropertiesChangeCallback()
		{
			if (not g_detail) return;

			g_detail->onRoomPropertiesChange();
		}

		__attribute__((used, export_name("siv3dPhotonOnPlayerPropertiesChangeCallback")))
		void siv3dPhotonOnPlayerPropertiesChangeCallback(LocalPlayerID playerID)
		{
			if (not g_detail) return;

			g_detail->onPlayerPropertiesChange(playerID);
		}

		__attribute__((used, export_name("siv3dPhotonOnHostChangeCallback")))
		void siv3dPhotonOnHostChangeCallback(LocalPlayerID newHost, LocalPlayerID oldHost)
		{
			if (not g_detail) return;

			g_detail->onMasterClientChanged(newHost, oldHost);
		}
	}
}

// [WEB] detail
namespace s3d::detail
{
	enum class EventCaching
	{
		DoNotCache,
		MergeCache,
		ReplaceCache,
		RemoveCache,
		AddToRoomCache,
		AddToRoomCacheGlobal,
		RemoveFromRoomCache,
		RemoveFromRoomCacheForActorsLeft,
	};

	enum class ReceiverGroup
	{
		Others,
		All,
		MasterClient,
	};

	String HashTableToJSON(const HashTable<String, String>& table)
	{
		JSON json = table;

		return json.formatMinimum();
	}

	String RoomCreateOptionToJSON(const RoomCreateOption& roomCreateOption)
	{
		JSON json{};

		json[U"isOpen"] = roomCreateOption.isOpen();
		json[U"maxPlayers"] = roomCreateOption.maxPlayers();
		json[U"customGameProperties"] = roomCreateOption.properties();
		json[U"propsListedInLobby"] = roomCreateOption.visibleRoomPropertyKeys();
		json[U"playerTTL"] = roomCreateOption.rejoinGracePeriod().value_or(-1ms).count();
		json[U"roomTTL"] = roomCreateOption.roomDestroyGracePeriod().count();

		return json.formatMinimum();
	}

	String MultiplayerEventToJSON(const MultiplayerEvent& eventOption)
	{
		JSON json{};

		if (eventOption.targetGroup() != 0)
		{
			json[U"interestGroup"] = eventOption.targetGroup();
		}
		EventCaching cache = EventCaching::DoNotCache;
		ReceiverGroup receiver = ReceiverGroup::Others;
		switch (eventOption.receiverOption())
		{
		case EventReceiverOption::Others:
			break;
		case EventReceiverOption::Others_CacheUntilLeaveRoom:
			cache = EventCaching::AddToRoomCache;
			break;
		case EventReceiverOption::Others_CacheForever:
			cache = EventCaching::AddToRoomCacheGlobal;
			break;
		case EventReceiverOption::All:
			receiver = ReceiverGroup::All;
			break;
		case EventReceiverOption::All_CacheUntilLeaveRoom:
			receiver = ReceiverGroup::All;
			cache = EventCaching::AddToRoomCache;
			break;
		case EventReceiverOption::All_CacheForever:
			receiver = ReceiverGroup::All;
			cache = EventCaching::AddToRoomCacheGlobal;
			break;
		case EventReceiverOption::Host:
			receiver = ReceiverGroup::MasterClient;
			break;
		};
		json[U"cache"] = static_cast<int32>(cache);
		if (static_cast<int32>(receiver) != 0)
		{
			json[U"receivers"] = static_cast<int32>(receiver);
		}
		if (eventOption.targetList())
		{
			json[U"targetActors"] = eventOption.targetList().value();
		}

		return json.formatMinimum();
	}

	String MultiplayerEventToJSON(const Array<LocalPlayerID>& targets)
	{
		JSON json{};

		if (targets.size() > 0)
		{
			json[U"targetActors"] = targets;
		}

		return json.formatMinimum();
	}

	String MultiplayerEventToJSON(EventCaching cache)
	{
		JSON json{};

		json[U"cache"] = static_cast<int32>(cache);

		return json.formatMinimum();
	}

	String MultiplayerEventToJSON(EventCaching cache, const Array<LocalPlayerID>& targets)
	{
		JSON json{};

		json[U"cache"] = static_cast<int32>(cache);
		json[U"targetActors"] = targets;

		return json.formatMinimum();
	}
}

// [Common] RoomCreateOption, TargetGroup, MultiplayerEvent
namespace s3d {

	// RoomCreateOption

	RoomCreateOption& RoomCreateOption::isVisible(bool isVisible)
	{
		m_isVisible = isVisible;
		return *this;
	}

	RoomCreateOption& RoomCreateOption::isOpen(bool isOpen)
	{
		m_isOpen = isOpen;
		return *this;
	}

	RoomCreateOption& RoomCreateOption::publishUserId(bool publishUserId)
	{
		m_publishUserId = publishUserId;
		return *this;
	}

	RoomCreateOption& RoomCreateOption::maxPlayers(int32 maxPlayers)
	{
		if (not InRange(maxPlayers, 0, 255))
		{
			throw Error{ U"[Multiplayer_Photon] MaxPlayers must be in a range of 0 to 255" };
		}
		m_maxPlayers = maxPlayers;
		return *this;
	}

	RoomCreateOption& RoomCreateOption::properties(const HashTable<String, String>& properties)
	{
		m_properties = properties;
		return *this;
	}

	RoomCreateOption& RoomCreateOption::visibleRoomPropertyKeys(const Array<String>& visibleRoomPropertyKeys)
	{
		m_visibleRoomPropertyKeys = visibleRoomPropertyKeys;
		return *this;
	}

	RoomCreateOption& RoomCreateOption::rejoinGracePeriod(const Optional<Milliseconds>& rejoinGracePeriod)
	{
		m_rejoinGracePeriod = rejoinGracePeriod;
		return *this;
	}

	RoomCreateOption& RoomCreateOption::roomDestroyGracePeriod(Milliseconds roomDestroyGracePeriod)
	{
		m_roomDestroyGracePeriod = roomDestroyGracePeriod;
		return *this;
	}

	bool RoomCreateOption::isVisible() const noexcept
	{
		return m_isVisible;
	}

	bool RoomCreateOption::isOpen() const noexcept
	{
		return m_isOpen;
	}

	bool RoomCreateOption::publishUserId() const noexcept
	{
		return m_publishUserId;
	}

	int32 RoomCreateOption::maxPlayers() const noexcept
	{
		return m_maxPlayers;
	}

	const HashTable<String, String>& RoomCreateOption::properties() const noexcept
	{
		return m_properties;
	}

	const Array<String>& RoomCreateOption::visibleRoomPropertyKeys() const noexcept
	{
		return m_visibleRoomPropertyKeys;
	}

	const Optional<Milliseconds>& RoomCreateOption::rejoinGracePeriod() const noexcept
	{
		return m_rejoinGracePeriod;
	}

	Milliseconds RoomCreateOption::roomDestroyGracePeriod() const noexcept
	{
		return m_roomDestroyGracePeriod;
	}

	// TargetGroup

	TargetGroup::TargetGroup(uint8 targetGroup) noexcept
		: m_targetGroup(targetGroup)
	{
	}

	uint8 TargetGroup::value() const noexcept
	{
		return m_targetGroup;
	}

	// MultiplayerEvent

	uint8 MultiplayerEvent::eventCode() const noexcept
	{
		return m_eventCode;
	}

	uint8 MultiplayerEvent::priorityIndex() const noexcept
	{
		return m_priorityIndex;
	}

	uint8 MultiplayerEvent::targetGroup() const noexcept
	{
		return m_targetGroup;
	}

	EventReceiverOption MultiplayerEvent::receiverOption() const noexcept
	{
		return m_receiverOption;
	}

	const Optional<Array<LocalPlayerID>>& MultiplayerEvent::targetList() const noexcept
	{
		return m_targetList;
	}
}

// [Common] Multiplayer_Photon
namespace s3d {
	Multiplayer_Photon::Multiplayer_Photon() = default;

	Multiplayer_Photon::Multiplayer_Photon(std::string_view secretPhotonAppID, const StringView photonAppVersion, const Verbose verbose, const ConnectionProtocol protocol)
	{
		init(Unicode::WidenAscii(secretPhotonAppID), photonAppVersion, verbose, protocol);
	}

	Multiplayer_Photon::Multiplayer_Photon(StringView secretPhotonAppID, const StringView photonAppVersion, const Verbose verbose, const ConnectionProtocol protocol)
	{
		init(secretPhotonAppID, photonAppVersion, verbose, protocol);
	}

	Multiplayer_Photon::Multiplayer_Photon(std::string_view secretPhotonAppID, StringView photonAppVersion, const std::function<void(StringView)>& logger, const Verbose verbose, ConnectionProtocol protocol)
	{
		init(Unicode::WidenAscii(secretPhotonAppID), photonAppVersion, logger, verbose, protocol);
	}

	Multiplayer_Photon::Multiplayer_Photon(StringView secretPhotonAppID, StringView photonAppVersion, const std::function<void(StringView)>& logger, const Verbose verbose, ConnectionProtocol protocol)
	{
		init(secretPhotonAppID, photonAppVersion, logger, verbose, protocol);
	}

	Multiplayer_Photon::~Multiplayer_Photon()
	{
		disconnect();

	# if SIV3D_MULTIPLAYER_PHOTON_LEGACY
		UnregisterTypes();
	# endif

	# if SIV3D_PLATFORM(WEB)
		g_detail = nullptr;
	# endif
	}

	void Multiplayer_Photon::init(const std::string_view secretPhotonAppID, const StringView photonAppVersion, const Verbose verbose, const ConnectionProtocol protocol)
	{
		init(Unicode::WidenAscii(secretPhotonAppID), photonAppVersion, Print, verbose, protocol);
	}

	void Multiplayer_Photon::init(const StringView secretPhotonAppID, const StringView photonAppVersion, const Verbose verbose, const ConnectionProtocol protocol)
	{
		init(secretPhotonAppID, photonAppVersion, Print, verbose, protocol);
	}

	void Multiplayer_Photon::init(const std::string_view secretPhotonAppID, const StringView photonAppVersion, const std::function<void(StringView)>& logger, const Verbose verbose, const ConnectionProtocol protocol)
	{
		init(Unicode::WidenAscii(secretPhotonAppID), photonAppVersion, logger, verbose, protocol);
	}
}

/// [WEB] Multiplayer_Photon
namespace s3d
{
	void Multiplayer_Photon::init(const StringView secretPhotonAppID, const StringView photonAppVersion, const std::function<void(StringView)>& logger, const Verbose verbose, const ConnectionProtocol protocol)
	{
		if (m_detail) // すでに初期化済みであれば何もしない
		{
			return;
		}

		m_detail = std::make_unique<PhotonDetail>(*this);
		g_detail = m_detail.get();

		m_secretPhotonAppID = secretPhotonAppID;
		m_photonAppVersion = photonAppVersion;
		m_logger = logger;
		m_verbose	= verbose.getBool();

		detail::siv3dPhotonInitClient(m_secretPhotonAppID.data(), m_photonAppVersion.data(), m_verbose, static_cast<uint8>(protocol));
	}

	bool Multiplayer_Photon::connect(const StringView userName, const Optional<String>& region)
	{
		m_requestedRegion = region;

		if (region)
		{
			const String userID = U"{}{}"_fmt(userName, Time::GetMillisecSinceEpoch());

			m_detail->m_localPlayer.userID = userID;

			setUserName(userName);

			bool result = detail::siv3dPhotonConnect(getUserID().data(), region.value().data());

			if (not result)
			{
				if (m_verbose)
				{
					logger(U"[Multiplayer_Photon] ConnectToNameServer failed.");
				}	
				return false;
			}
		}
		else
		{
			if (m_verbose)
			{
				logger(U"[Multiplayer_Photon] Region must be specified");
			}
			return false;
		}

		return true;
	}

	void Multiplayer_Photon::disconnect()
	{
		detail::siv3dPhotonDisconnect();
		detail::siv3dPhotonService();
	}

	void Multiplayer_Photon::update()
	{
		if (not m_detail)
		{
			return;
		}

		detail::siv3dPhotonService();
	}

	bool Multiplayer_Photon::isActive() const noexcept
	{
		return getClientState() != ClientState::Disconnected;
	}
	
	ClientState Multiplayer_Photon::getClientState() const
	{
		return m_detail ? m_detail->m_clientState : ClientState::Disconnected;
	}

	bool Multiplayer_Photon::isInLobby() const
	{
		return m_detail->m_clientState == ClientState::InLobby;
	}

	bool Multiplayer_Photon::isInLobbyOrInRoom() const
	{
		return isInLobby() or isInRoom();
	}

	bool Multiplayer_Photon::isInRoom() const
	{
		return m_detail->m_clientState == ClientState::InRoom;
	}

	Array<RoomInfo> Multiplayer_Photon::getRoomList() const
	{
		if (not m_detail)
		{
			return {};
		}

		Array<RoomInfo> result{};

		detail::siv3dPhotonGetRoomList(&result);
		
		return result;
	}

	Array<RoomName> Multiplayer_Photon::getRoomNameList() const
	{
		if (not m_detail)
		{
			return{};
		}

		Array<RoomName> result{};
		
		detail::siv3dPhotonGetRoomNameList(&result);
		
		return result;
	}

	bool Multiplayer_Photon::joinRandomRoom(const int32 expectedMaxPlayers, MatchmakingMode matchmakingMode)
	{
		if (not m_detail)
		{
			return false;
		}

		if (not InRange(expectedMaxPlayers, 0, 255))
		{
			return false;
		}

		return m_detail->joinRandomRoom(expectedMaxPlayers, matchmakingMode, U"{}");
	}

	bool Multiplayer_Photon::joinRandomRoom(const HashTable<String, String>& propertyFilter, int32 expectedMaxPlayers, MatchmakingMode matchmakingMode)
	{
		if (not m_detail)
		{
			return false;
		}

		if (not InRange(expectedMaxPlayers, 0, 255))
		{
			return false;
		}

		return m_detail->joinRandomRoom(expectedMaxPlayers, matchmakingMode, detail::HashTableToJSON(propertyFilter));
	}

	bool Multiplayer_Photon::joinRandomOrCreateRoom(const int32 maxPlayers, const RoomNameView roomName)
	{
		if (not m_detail)
		{
			return false;
		}

		if (not InRange(maxPlayers, 0, 255))
		{
			return false;
		}

		return m_detail->joinRandomOrCreateRoom(roomName, U"{}", U"{}", maxPlayers, MatchmakingMode::FillOldestRoom);
	}

	bool Multiplayer_Photon::joinRandomOrCreateRoom(RoomNameView roomName, const RoomCreateOption& roomCreateOption, const HashTable<String, String>& propertyFilter, int32 expectedMaxPlayers, MatchmakingMode matchmakingMode)
	{
		if (not m_detail)
		{
			return false;
		}

		if (not InRange(expectedMaxPlayers, 0, 255))
		{
			return false;
		}

		return m_detail->joinRandomOrCreateRoom(roomName, detail::RoomCreateOptionToJSON(roomCreateOption), detail::HashTableToJSON(propertyFilter), expectedMaxPlayers, matchmakingMode);
	}

	bool Multiplayer_Photon::joinOrCreateRoom(RoomNameView roomName, const RoomCreateOption& option)
	{
		if (not m_detail)
		{
			return false;
		}

		return m_detail->joinOrCreateRoom(roomName, detail::RoomCreateOptionToJSON(option));
	}

	bool Multiplayer_Photon::joinRoom(const RoomNameView roomName)
	{
		if (not m_detail)
		{
			return false;
		}

		return m_detail->joinRoom(roomName, false);
	}

	bool Multiplayer_Photon::createRoom(const RoomNameView roomName, const int32 maxPlayers)
	{
		if (not m_detail)
		{
			return false;
		}

		if (not InRange(maxPlayers, 0, 255))
		{
			return false;
		}

		return m_detail->createRoom(roomName, detail::RoomCreateOptionToJSON(RoomCreateOption().maxPlayers(maxPlayers)));
	}

	bool Multiplayer_Photon::createRoom(RoomNameView roomName, const RoomCreateOption& option)
	{
		if (not m_detail)
		{
			return false;
		}

		return m_detail->createRoom(roomName, detail::RoomCreateOptionToJSON(option));
	}

	void Multiplayer_Photon::leaveRoom(bool willComeBack)
	{
		if (not m_detail)
		{
			return;
		}

		m_detail->leaveRoom(willComeBack);
	}
	
	bool Multiplayer_Photon::reconnectAndRejoin()
	{
		if (not m_detail)
		{
			return false;
		}

		return detail::siv3dPhotonReconnectAndRejoin();
	}

	int32 Multiplayer_Photon::getServerTimeMillisec() const
	{
		if (not m_detail)
		{
			return 0;
		}

		return detail::siv3dPhotonGetServerTime();
	}

	int32 Multiplayer_Photon::getServerTimeOffsetMillisec() const
	{
		if (not m_detail)
		{
			return 0;
		}

		return detail::siv3dPhotonGetServerTime() - GetSystemTimeMillisec();
	}

	int32 Multiplayer_Photon::getPingMillisec() const
	{
		if (not m_detail)
		{
			return 0;
		}

		return detail::siv3dPhotonGetRoundTripTime();
	}

	int32 Multiplayer_Photon::getPingIntervalMillisec() const
	{
		if (not m_detail)
		{
			return 0;
		}

		return m_detail->getTimePingInterval();
	}

	void Multiplayer_Photon::setPingIntervalMillisec(int32 intervalMillisec)
	{	
		if (not m_detail) {
			return;
		}

		return m_detail->setTimePingInterval(intervalMillisec);
	}

	int32 Multiplayer_Photon::getCountGamesRunning() const
	{
		if (not m_detail)
		{
			return 0;
		}

		return m_detail->m_countGamesRunning;
	}

	int32 Multiplayer_Photon::getCountPlayersIngame() const
	{
		if (not m_detail)
		{
			return 0;
		}

		return m_detail->m_countPlayersIngame;
	}

	int32 Multiplayer_Photon::getCountPlayersOnline() const
	{
		if (not m_detail)
		{
			return 0;
		}

		return m_detail->m_countPlayersOnline;
	}
	
	void Multiplayer_Photon::joinEventTargetGroup(uint8 targetGroup)
	{
		joinEventTargetGroup(Array<uint8>{ targetGroup });
	}

	void Multiplayer_Photon::joinEventTargetGroup(const Array<uint8>& targetGroups)
	{
		if (not m_detail)
		{
			return;
		}

		for (const auto targetGroup : targetGroups)
		{
			if (not InRange(static_cast<int>(targetGroup), 1, 255)) {
				throw Error{ U"[Multiplayer_Photon] targetGroups must be in a range of 1 to 255" };
			}
		}

		detail::siv3dPhotonChangeInterestGroup(static_cast<int32>(targetGroups.size()), targetGroups.data(), 0, nullptr);
	}

	void Multiplayer_Photon::joinAllEventTargetGroups()
	{
		if (not m_detail)
		{
			return;
		}

		detail::siv3dPhotonChangeInterestGroup(-1, nullptr, 0, nullptr);
	}

	void Multiplayer_Photon::leaveEventTargetGroup(const uint8 targetGroup)
	{
		leaveEventTargetGroup(Array<uint8>{ targetGroup });
	}

	void Multiplayer_Photon::leaveEventTargetGroup(const Array<uint8>& targetGroups)
	{
		if (not m_detail)
		{
			return;
		}

		for (const auto targetGroup : targetGroups)
		{
			if (not InRange(static_cast<int>(targetGroup), 1, 255)) {
				throw Error{ U"[Multiplayer_Photon] targetGroups must be in a range of 1 to 255" };
			}
		}

		detail::siv3dPhotonChangeInterestGroup(0, nullptr, static_cast<int32>(targetGroups.size()), targetGroups.data());
	}

	void Multiplayer_Photon::leaveAllEventTargetGroups()
	{
		if (not m_detail)
		{
			return;
		}

		detail::siv3dPhotonChangeInterestGroup(0, nullptr, -1, nullptr);
	}
}

/// [WEB] Multiplayer_Photon::sendEvent
namespace s3d
{
	void Multiplayer_Photon::sendEvent(const MultiplayerEvent& event, const Serializer<MemoryWriter>& writer)
	{
		if (not m_detail)
		{
			return;
		}

		int32 size = static_cast<int32>(writer->size());
		const auto src = static_cast<const void*>(writer->getBlob().data());

		std::string message{};
		Base64::Encode(src, size, message);

		detail::siv3dPhotonRaiseEvent(
			event.eventCode(),
			message.data(),
			detail::MultiplayerEventToJSON(event).data()
		);
	}
}

/// [WEB] Multiplayer_Photon
namespace s3d
{
	template<>
	void Multiplayer_Photon::removeEventCache(uint8 eventCode)
	{
		if (not m_detail)
		{
			return;
		}

		if (not InRange(static_cast<int>(eventCode), 1, 199))
		{
			throw Error{ U"[Multiplayer_Photon] EventCode must be in a range of 1 to 199" };
		}

		detail::siv3dPhotonRaiseEvent(
			eventCode,
			nullptr,
			detail::MultiplayerEventToJSON(detail::EventCaching::RemoveFromRoomCache).data()
		);
	}

	template<>
	void Multiplayer_Photon::removeEventCache(uint8 eventCode, const Array<LocalPlayerID>& targets)
	{
		if (not m_detail)
		{
			return;
		}

		if (not InRange(static_cast<int>(eventCode), 1, 199))
		{
			throw Error{ U"[Multiplayer_Photon] EventCode must be in a range of 1 to 199" };
		}

		detail::siv3dPhotonRaiseEvent(
			eventCode,
			nullptr,
			detail::MultiplayerEventToJSON(detail::EventCaching::RemoveFromRoomCache, targets).data()
		);
	}

	LocalPlayer Multiplayer_Photon::getLocalPlayer() const
	{
		if (not m_detail)
		{
			return{};
		}

		m_detail->updateLocalPlayer();
		
		return m_detail->m_localPlayer;
	}

	LocalPlayer Multiplayer_Photon::getLocalPlayer(LocalPlayerID localPlayerID) const
	{
		if (not m_detail)
		{
			return{};
		}

		if (not isInRoom())
		{
			return{};
		}

		Array<LocalPlayer> players;

		detail::siv3dPhotonGetRoomPlayerList(&players, localPlayerID);

		if (players.isEmpty())
		{
			return{};
		}
		
		return players[0];
	}

	String Multiplayer_Photon::getUserName() const
	{
		if (not m_detail)
		{
			return{};
		}

		m_detail->updateLocalPlayer();

		return m_detail->m_localPlayer.userName;
	}
	
	String Multiplayer_Photon::getUserName(LocalPlayerID localPlayerID) const
	{
		if (not m_detail)
		{
			return {};
		}

		return getLocalPlayer(localPlayerID).userName;
	}

	String Multiplayer_Photon::getUserID() const
	{
		if (not m_detail)
		{
			return {};
		}

		return m_detail->m_localPlayer.userID;
	}

	String Multiplayer_Photon::getUserID(LocalPlayerID localPlayerID) const
	{
		if (not m_detail)
		{
			return {};
		}

		return getLocalPlayer(localPlayerID).userID;
	}

	bool Multiplayer_Photon::isHost() const
	{
		if (not m_detail)
		{
			return false;
		}

		return getLocalPlayer().isHost;
	}

	LocalPlayerID Multiplayer_Photon::getLocalPlayerID() const
	{
		if (not m_detail)
		{
			return -1;
		}

		m_detail->updateLocalPlayer();

		return m_detail->m_localPlayer.localID;
	}

	LocalPlayerID Multiplayer_Photon::getHostLocalPlayerID() const
	{
		if (not m_detail)
		{
			return -1;
		}

		return detail::siv3dPhotonGetMasterClientID();
	}

	void Multiplayer_Photon::setUserName(StringView name)
	{
		if (not m_detail)
		{
			return;
		}

		m_detail->m_localPlayer.userName = name;

		detail::siv3dPhotonSetUserName(name.data());
	}

	void Multiplayer_Photon::setHost(LocalPlayerID playerID)
	{
		detail::siv3dPhotonSetMasterClient(playerID);
	}

	RoomInfo Multiplayer_Photon::getCurrentRoom() const
	{
		if (not m_detail)
		{
			return{};
		}

		if (not isInRoom())
		{
			return{};
		}
		
		m_detail->updateCurrentRoom();

		return m_detail->m_currentRoom;
	}

	String Multiplayer_Photon::getCurrentRoomName() const
	{
		m_detail->updateCurrentRoom();

		return m_detail->m_currentRoom.name;
	}

	Array<LocalPlayer> Multiplayer_Photon::getLocalPlayers() const
	{
		if (not m_detail)
		{
			return{};
		}

		if (not isInRoom())
		{
			return{};
		}

		Array<LocalPlayer> result{};

		detail::siv3dPhotonGetRoomPlayerList(&result, -1);

		return result;
	}

	int32 Multiplayer_Photon::getPlayerCountInCurrentRoom() const
	{
		m_detail->updateCurrentRoom();

		return m_detail->m_currentRoom.playerCount;
	}

	int32 Multiplayer_Photon::getMaxPlayersInCurrentRoom() const
	{
		m_detail->updateCurrentRoom();

		return m_detail->m_currentRoom.maxPlayers;
	}

	bool Multiplayer_Photon::getIsOpenInCurrentRoom() const
	{
		m_detail->updateCurrentRoom();

		return m_detail->m_currentRoom.isOpen;
	}

	bool Multiplayer_Photon::getIsVisibleInCurrentRoom() const
	{
		return detail::siv3dPhotonGetIsVisibleInCurrentRoom();
	}

	void Multiplayer_Photon::setIsOpenInCurrentRoom(const bool isOpen)
	{
		if (not m_detail)
		{
			return;
		}

		detail::siv3dPhotonSetCurrentRoomOpen(isOpen);
	}

	void Multiplayer_Photon::setIsVisibleInCurrentRoom(const bool isVisible)
	{
		if (not m_detail)
		{
			return;
		}

		detail::siv3dPhotonSetCurrentRoomVisible(isVisible);
	}

	String Multiplayer_Photon::getPlayerProperty(StringView key) const
	{
		return getPlayerProperty(-1, key);
	}

	String Multiplayer_Photon::getPlayerProperty(LocalPlayerID localPlayerID, StringView key) const
	{
		if (not m_detail)
		{
			return {};
		}

		if (not isInRoom())
		{
			return {};
		}

		char32* result = detail::siv3dPhotonGetPlayerCustomProperty(key.data(), localPlayerID);

		String ret { result ? result : U"" };

		free(result);

		return ret;
	}

	HashTable<String, String> Multiplayer_Photon::getPlayerProperties() const
	{
		return getPlayerProperties(-1);
	}

	HashTable<String, String> Multiplayer_Photon::getPlayerProperties(LocalPlayerID localPlayerID) const
	{
		if (not m_detail)
		{
			return{};
		}

		if (not isInRoom())
		{
			return{};
		}

		HashTable<String, String> result {};

		detail::siv3dPhotonGetPlayerCustomProperties(&result, localPlayerID);

		return result;
	}

	void Multiplayer_Photon::setPlayerProperty(StringView key, StringView value)
	{
		if (not m_detail)
		{
			return;
		}

		if (not isInRoom())
		{
			return;
		}
		
		detail::siv3dPhotonSetPlayerCustomProperty(key.data(), value.data());
	}

	void Multiplayer_Photon::removePlayerProperty(StringView key)
	{
		removePlayerProperty(Array<String>{ String(key) });
	}

	void Multiplayer_Photon::removePlayerProperty(const Array<String>& keys)
	{
		if (not m_detail)
		{
			return;
		}

		if (not isInRoom())
		{
			return;
		}

		Array<const char32*> ckeys { Arg::reserve = keys.size() };

		for (int i = 0; i < keys.size(); i++)
		{
			ckeys.push_back(keys[i].data());
		}

		detail::siv3dPhotonRemovePlayerCustomProperties(keys.size(), ckeys.data());
	}

	String Multiplayer_Photon::getRoomProperty(StringView key) const
	{
		if (not m_detail)
		{
			return {};
		}

		if (not isInRoom())
		{
			return {};
		}

		char32* result = detail::siv3dPhotonGetRoomCustomProperty(key.data());

		String ret { result ? result : U"" };

		free(result);

		return ret;
	}

	HashTable<String, String> Multiplayer_Photon::getRoomProperties() const
	{
		if (not m_detail)
		{
			return{};
		}

		if (not isInRoom())
		{
			return{};
		}

		HashTable<String, String> result {};

		detail::siv3dPhotonGetRoomCustomProperties(&result);

		return result;
	}

	void Multiplayer_Photon::setRoomProperty(StringView key, StringView value)
	{
		if (not m_detail)
		{
			return;
		}

		if (not isInRoom())
		{
			return;
		}
		
		detail::siv3dPhotonSetRoomCustomProperty(key.data(), value.data());
	}

	void Multiplayer_Photon::removeRoomProperty(StringView key)
	{
		removeRoomProperty(Array<String>{ String(key) });
	}

	void Multiplayer_Photon::removeRoomProperty(const Array<String>& keys)
	{
		if (not m_detail)
		{
			return;
		}

		if (not isInRoom())
		{
			return;
		}

		Array<const char32*> ckeys { Arg::reserve = keys.size() };

		for (int i = 0; i < keys.size(); i++)
		{
			ckeys.push_back(keys[i].data());
		}

		detail::siv3dPhotonRemoveRoomCustomProperties(keys.size(), ckeys.data());
	}

	Array<String> Multiplayer_Photon::getVisibleRoomPropertyKeys() const
	{
		if (not m_detail)
		{
			return{};
		}

		if (not isInRoom())
		{
			return{};
		}
		
		Array<String> result {};

		detail::siv3dPhotonGetVisibleRoomPropertyKeys(&result);

		return result;
	}

	void Multiplayer_Photon::setVisibleRoomPropertyKeys(const Array<String>& keys)
	{
		if (not m_detail)
		{
			return;
		}

		if (not isInRoom())
		{
			return;
		}

		Array<const char32*> ckeys { Arg::reserve = keys.size() };

		for (int i = 0; i < keys.size(); i++)
		{
			ckeys.push_back(keys[i].data());
		}

		detail::siv3dPhotonSetVisibleRoomPropertyKeys(keys.size(), ckeys.data());
	}
	
	int32 Multiplayer_Photon::GetSystemTimeMillisec()
	{
		return static_cast<int32>(Time::GetMillisec());
	}
}

namespace s3d
{
	template<>
	void Multiplayer_Photon::sendEvent<>(const MultiplayerEvent& event)
	{
		sendEvent(event, Serializer<MemoryWriter> {});
	}
}
