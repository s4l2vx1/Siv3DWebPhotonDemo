# include <Siv3D.hpp> // OpenSiv3D v0.6.4
# include "Multiplayer_Photon.hpp"
# include "PHOTON_APP_ID.SECRET"


struct ScrollBar
{
	RectF rect{};
	Optional<double> dragOffset;

	double viewHeight = 600;
	double pageHeight = 1000;
	double viewTop = 0;
	double viewVelocity = 0;

	double accumulateTime = 0;
	static constexpr double stepTime = 1.0 / 200;
	static constexpr double resistance = 10;

	Transition sliderWidthTransition = Transition(0.1s, 0.1s);

	ScrollBar() = default;

	ScrollBar(const RectF& rect, double viewHeight, double pageHeight)
		: rect(rect)
		, viewHeight(viewHeight)
		, pageHeight(pageHeight)
	{

	}

	ScrollBar(double pageHeight, double viewHeight = Graphics2D::GetRenderTargetSize().y, const RectF& rect = Rect{ Arg::topRight(Graphics2D::GetRenderTargetSize().x - 2, 0), 10 ,Graphics2D::GetRenderTargetSize().y })
		: rect(rect)
		, viewHeight(viewHeight)
		, pageHeight(pageHeight)
	{

	}

	double sliderHeight() const
	{
		return Max(rect.h * viewHeight / pageHeight, 20.0);
	}

	double sliderYPerViewY() const
	{
		return (rect.h - sliderHeight()) / (pageHeight - viewHeight);
	}

	double sliderY() const
	{
		return viewTop * sliderYPerViewY();
	}

	RectF sliderRect() const
	{
		return RectF(rect.x, rect.y + sliderY(), rect.w, sliderHeight());
	}

	bool existSlider() const
	{
		return viewHeight < pageHeight;
	}

	bool isSliderMouseOver() const
	{
		return sliderRect().stretched(5).mouseOver();
	}

	bool isSliderThick() const
	{
		return isSliderMouseOver() || dragOffset;
	}

	Transformer2D createTransformer() const
	{
		return Transformer2D(Mat3x2::Translate(0, -viewTop), TransformCursor::Yes);
	}

	void scrollBy(double h) {
		viewVelocity += resistance * h;
	}

	void scrollTopTo(double y) {
		scrollBy(y - viewTop);
	}

	void scrollBottomTo(double y) {
		scrollBy(y - viewTop - viewHeight);
	}

	void scrollCenterTo(double y) {
		scrollBy(y - viewTop - viewHeight / 2);
	}

	void update(double wheel = Mouse::Wheel(), double delta = Scene::DeltaTime())
	{
		if (not existSlider()) {
			viewTop = 0;
			viewVelocity = 0;
			dragOffset.reset();
			sliderWidthTransition.reset();
			return;
		}

		for (accumulateTime += delta; accumulateTime >= stepTime; accumulateTime -= stepTime)
		{
			if (not dragOffset) {
				viewTop += viewVelocity * stepTime;
			}

			if (viewVelocity != 0)
			{
				viewVelocity += -viewVelocity * stepTime * resistance;
			}
		}

		if (dragOffset)
		{
			const double prevTop = viewTop;
			viewTop = (Cursor::PosF().y - *dragOffset) / sliderYPerViewY();
			viewVelocity = (viewTop - prevTop) / delta;
		}


		if (isSliderMouseOver() and MouseL.down())
		{
			dragOffset = Cursor::PosF().y - sliderY();
		}
		else if (dragOffset && MouseL.up())
		{
			dragOffset.reset();
		}

		if (wheel) {
			viewVelocity = wheel * 2000;
		}

		if (viewTop < 0)
		{
			viewTop = 0;
			viewVelocity = 0;
		}
		else if (viewTop + viewHeight > pageHeight)
		{
			viewTop = pageHeight - viewHeight;
			viewVelocity = 0;
		}

		sliderWidthTransition.update(isSliderThick());

	}

	void draw(const ColorF& color = ColorF(Palette::Dimgray).withAlpha(0.8)) const
	{
		if (not existSlider()) return;

		double w = rect.w * (sliderWidthTransition.value() * 0.5 + 0.5);

		RectF(rect.x - w + rect.w, rect.y + sliderY(), w, sliderHeight()).rounded(rect.w / 2).draw(color);
	}

	double progress0_1() {
		return viewTop / (pageHeight - viewHeight);
	}
};


// ユーザ定義型
struct MyData
{
	String word;

	Point pos;

	// シリアライズに対応させるためのメンバ関数を定義する
	template <class Archive>
	void SIV3D_SERIALIZE(Archive& archive)
	{
		archive(word, pos);
	}
};

enum class EventCode
{
	IntEvent = 1,
	StringEvent,
	StringEvent2,
	CustomDataTest1,
	CustomDataTest2,
	CustomDataTest3,
	CustomDataTest4,
	FallbackTest,
};

class MyNetwork : public Multiplayer_Photon
{
public:

	MyNetwork()
	{
		init(std::string(SIV3D_OBFUSCATE(PHOTON_APP_ID)), U"1.0", Console, Verbose::Yes, ConnectionProtocol::Wss);

		RegisterEventCallback(EventCode::IntEvent, &MyNetwork::onIntEvent);
		RegisterEventCallback(EventCode::StringEvent, &MyNetwork::onStringEvent);
		RegisterEventCallback(EventCode::StringEvent2, &MyNetwork::onStringEvent2);
		RegisterEventCallback(EventCode::CustomDataTest1, &MyNetwork::onCustomDataTest1);
		RegisterEventCallback(EventCode::CustomDataTest2, &MyNetwork::onCustomDataTest2);
		RegisterEventCallback(EventCode::CustomDataTest3, &MyNetwork::onCustomDataTest3);
		RegisterEventCallback(EventCode::CustomDataTest4, &MyNetwork::onCustomDataTest4);
	}

	Optional<LocalPlayer> getLocalPlayerByName(StringView userName) const
	{
		for (const auto& player : m_localPlayers)
		{
			if (player.userName == userName)
			{
				return player;
			}
		}
		return none;
	}

private:

	Array<LocalPlayer> m_localPlayers;

	void onIntEvent(LocalPlayerID sender, int32 value)
	{
		logger(U"<<< IntEvent を受信: {}"_fmt(value));
	}

	void onStringEvent(LocalPlayerID sender, String value)
	{
		logger(U"<<< StringEvent を受信: {}"_fmt(value));
	}

	void onStringEvent2(LocalPlayerID sender, String value)
	{
		logger(U"<<< StringEvent2 を受信: {}"_fmt(value));
	}


	void onCustomDataTest1(LocalPlayerID sender) {
		logger(U"<<< CustomDataTest1 を受信"_fmt());
	}

	void onCustomDataTest2(LocalPlayerID sender, Array<double> a) {
		logger(U"<<< CustomDataTest2 を受信: {}"_fmt(a));
	}

	void onCustomDataTest3(LocalPlayerID sender, Array<double>& a) {
		logger(U"<<< CustomDataTest3 を受信: {}"_fmt());
	}

	void onCustomDataTest4(LocalPlayerID sender, Array<double>&& a) {
		logger(U"<<< CustomDataTest4 を受信: {}"_fmt());
	}

	// シリアライズデータを受信したときに呼ばれる関数をオーバーライドしてカスタマイズする
	void customEventAction(const LocalPlayerID playerID, const uint8 eventCode, Deserializer<MemoryViewReader>& reader) override
	{
		logger(U"<<< {} を受信: {}"_fmt(eventCode));
	}

	void onRoomListUpdate() override
	{
		logger(U"onRoomListUpdate:");
		logger(U"{}"_fmt(getRoomNameList()));
		for (const auto& room : getRoomList())
		{
			logger(U"- name: {}"_fmt(room.name));
			logger(U"- isOpen: {}"_fmt(room.isOpen));
			logger(U"- stats: {} / {}"_fmt(room.playerCount, room.maxPlayers));
			logger(U"- properties: {}"_fmt(Format(room.properties)));
		}
	}
};

void Main()
{
	Scene::Resize(1600, 1200);
	Scene::SetResizeMode(ResizeMode::Keep);

	Window::SetStyle(WindowStyle::Sizable);

	MyNetwork network{};

	ScrollBar scrollBar{ 1200 };

	TextEditState text{};

	Font font{ 20 };

	static constexpr int32 initX = 10;
	static constexpr int32 initY = 10;
	static constexpr int32 marginWidth = 10;
	static constexpr int32 ButtonWidth = 380;
	static constexpr int32 offsetX = marginWidth + ButtonWidth;
	static constexpr int32 offsetY = 50;

	while (System::Update())
	{
		scrollBar.update();
		network.update();

		int x = initX;
		int y = initY;

		SimpleGUI::TextBox(text, { x, y }, ButtonWidth);

		if (SimpleGUI::Button(U"connect", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.connect(U"Siv", U"jp");
		}

		if (SimpleGUI::Button(U"disconnect", { x += offsetX, y }, ButtonWidth))
		{
			network.disconnect();
		}

		if (SimpleGUI::Button(U"leaveRoom", { x += offsetX, y }, ButtonWidth))
		{
			network.leaveRoom(false);
		}

		if (SimpleGUI::Button(U"leaveRoom (rejoin)", { x += offsetX, y }, ButtonWidth))
		{
			network.leaveRoom(true);
		}

		if (SimpleGUI::Button(U"reconnect", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.reconnectAndRejoin();
		}

		if (SimpleGUI::Button(U"stats", { x += offsetX, y }, ButtonWidth))
		{
			network.logger(U"serverTime: {}ms"_fmt(Format(network.getServerTimeMillisec())));
			network.logger(U"serverTimeOffset: {}ms"_fmt(Format(network.getServerTimeOffsetMillisec())));
			network.logger(U"ping: {}ms"_fmt(Format(network.getPingMillisec())));
		}

# if not SIV3D_PLATFORM(WEB)
		font(U"bytesIn: {}"_fmt(
			network.getBytesIn()
		)).drawAt(Rect{ x += offsetX, y, ButtonWidth, 40 }.center());
		font(U"bytesOut: {}"_fmt(
			network.getBytesOut()
		)).drawAt(Rect{ x += offsetX, y, ButtonWidth, 40 }.center());
# endif

		if (SimpleGUI::Button(U"joinRandomOrCreateRoom", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.joinRandomOrCreateRoom(
				text.text,
				RoomCreateOption().maxPlayers(2)
			);
		}

		if (SimpleGUI::Button(U"joinOrCreateRoom", { x += offsetX, y }, ButtonWidth))
		{
			network.joinOrCreateRoom(
				text.text,
				RoomCreateOption().maxPlayers(2)
			);
		}

		if (SimpleGUI::Button(U"createRoom", { x += offsetX, y }, ButtonWidth))
		{
			network.createRoom(
				text.text,
				RoomCreateOption().maxPlayers(2)
			);
		}

		if (SimpleGUI::Button(U"joinRoom", { x += offsetX, y }, ButtonWidth))
		{
			network.joinRoom(text.text);
		}

		if (SimpleGUI::Button(U"joinRandomRoom", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.joinRandomRoom();
		}

		if (SimpleGUI::Button(U"joinEventTargetGroup 1", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.joinEventTargetGroup(1);
		}

		if (SimpleGUI::Button(U"joinEventTargetGroup 2, 3", { x += offsetX, y }, ButtonWidth))
		{
			network.joinEventTargetGroup({2, 3});
		}

		if (SimpleGUI::Button(U"joinEventTargetGroup All", { x += offsetX, y }, ButtonWidth))
		{
			network.joinAllEventTargetGroups();
		}

		if (SimpleGUI::Button(U"leaveEventTargetGroup 1, 2", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.leaveEventTargetGroup({1, 2});
		}

		if (SimpleGUI::Button(U"leaveEventTargetGroup 3", { x += offsetX, y }, ButtonWidth))
		{
			network.joinEventTargetGroup(3);
		}

		if (SimpleGUI::Button(U"leaveEventTargetGroup All", { x += offsetX, y }, ButtonWidth))
		{
			network.leaveAllEventTargetGroups();
		}

		if (SimpleGUI::Button(U"sendEventTest 1, 2, 3, 4", { x = initX, y += offsetY }, ButtonWidth))
		{
			Array<double> arr{ 0.5, 1.0, 1.5 };
			network.sendEvent(MultiplayerEvent(EventCode::CustomDataTest1));
			network.sendEvent(MultiplayerEvent(EventCode::CustomDataTest2), arr);
			network.sendEvent(MultiplayerEvent(EventCode::CustomDataTest3), arr);
			network.sendEvent(MultiplayerEvent(EventCode::CustomDataTest4), arr);
		}

		if (SimpleGUI::Button(U"sendEventTest Fallback", { x += offsetX, y }, ButtonWidth))
		{
			Array<double> arr{ 0.5, 1.0, 1.5 };
			network.sendEvent(MultiplayerEvent(EventCode::FallbackTest));
		}

		if (SimpleGUI::Button(U"sendEventTo TargetGroup 1, 2, 3, 4", { x += offsetX, y }, ButtonWidth))
		{
			network.sendEvent(MultiplayerEvent(EventCode::IntEvent, TargetGroup(1)), 1);
			network.sendEvent(MultiplayerEvent(EventCode::IntEvent, TargetGroup(2)), 2);
			network.sendEvent(MultiplayerEvent(EventCode::IntEvent, TargetGroup(3)), 3);
			network.sendEvent(MultiplayerEvent(EventCode::IntEvent, TargetGroup(4)), 4);
		}

		if (SimpleGUI::Button(U"sendEventTo", { x = initX, y += offsetY }, ButtonWidth))
		{
			auto target = network.getLocalPlayerByName(text.text);
			if (target)
			{
				network.sendEvent(MultiplayerEvent(EventCode::IntEvent, {target.value().localID}), 0);
			}
		}

		if (SimpleGUI::Button(U"sendEvent toAll toHost", { x += offsetX, y }, ButtonWidth))
		{
			network.sendEvent(MultiplayerEvent(EventCode::StringEvent, EventReceiverOption::All), String(U"ToAll"));
			network.sendEvent(MultiplayerEvent(EventCode::StringEvent, EventReceiverOption::Host), String(U"ToHost"));
		}

		if (SimpleGUI::Button(U"sendEvent CacheUntilLeaveRoom", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.sendEvent(MultiplayerEvent(EventCode::StringEvent, EventReceiverOption::Others_CacheUntilLeaveRoom), String(U"CacheUntilLeaveRoom"));
		}

		if (SimpleGUI::Button(U"sendEvent CacheForever", { x += offsetX, y }, ButtonWidth))
		{
			network.sendEvent(MultiplayerEvent(EventCode::StringEvent, EventReceiverOption::Others_CacheForever), String(U"CacheForever"));
		}

		if (SimpleGUI::Button(U"sendEvent CacheUntilLeaveRoom2", { x += offsetX, y }, ButtonWidth))
		{
			network.sendEvent(MultiplayerEvent(EventCode::StringEvent2, EventReceiverOption::Others_CacheUntilLeaveRoom), String(U"CacheUntilLeaveRoom2"));
		}

		if (SimpleGUI::Button(U"sendEvent CacheForever2", { x += offsetX, y }, ButtonWidth))
		{
			network.sendEvent(MultiplayerEvent(EventCode::StringEvent2, EventReceiverOption::Others_CacheForever), String(U"CacheForever2"));
		}

		if (SimpleGUI::Button(U"removeEventCache", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.removeEventCache(EventCode::StringEvent);
		}

		if (SimpleGUI::Button(U"removeEventCache2", { x += offsetX, y }, ButtonWidth))
		{
			network.removeEventCache(EventCode::StringEvent2);
		}

		if (SimpleGUI::Button(U"removeEventCache All", { x += offsetX, y }, ButtonWidth))
		{
			network.removeEventCache(EventCode::StringEvent2, {0});
		}

		if (SimpleGUI::Button(U"removeEventCache Of", { x += offsetX, y }, ButtonWidth))
		{
			auto target = network.getLocalPlayerByName(text.text);
			if (target)
			{
				network.removeEventCache(EventCode::StringEvent2, {target.value().localID});
			}
		}

		if (SimpleGUI::Button(U"getSelf", { x = initX, y += offsetY }, ButtonWidth))
		{
			auto player = network.getLocalPlayer();
			network.logger(U"getSelf: ");
			network.logger(U"- userName: {} ({})"_fmt(player.userName, network.getUserName()));
			network.logger(U"- userID: {} ({})"_fmt(player.userID), network.getUserID());
			network.logger(U"- localID: {} ({})"_fmt(player.localID, network.getLocalPlayerID()));
			network.logger(U"- isHost: {} ({})"_fmt(player.isHost, network.isHost()));
			network.logger(U"- isActive: {}"_fmt(player.isActive));
		}

		if (SimpleGUI::Button(U"getHost", { x = initX, y += offsetY }, ButtonWidth))
		{
			auto hostID = network.getHostLocalPlayerID();
			auto player = network.getLocalPlayer(hostID);
			network.logger(U"getHost: ");
			network.logger(U"- userName: {} {}"_fmt(player.userName, network.getUserName(hostID)));
			network.logger(U"- userID: {} ({})"_fmt(player.userID), network.getUserID(hostID));
			network.logger(U"- localID: {} ({})"_fmt(player.localID, hostID));
			network.logger(U"- isHost: {}"_fmt(player.isHost));
			network.logger(U"- isActive: {}"_fmt(player.isActive));
		}

		if (SimpleGUI::Button(U"setHost", { x += offsetX, y }, ButtonWidth))
		{
			network.setHost(network.getLocalPlayerByName(text.text).value().localID);
		}

		if (SimpleGUI::Button(U"getRoom", { x = initX, y += offsetY }, ButtonWidth))
		{
			auto room = network.getCurrentRoom();
			auto players = network.getLocalPlayers();
			network.logger(U"getRoom: ");
			network.logger(U"- name: {}"_fmt(room.name, network.getCurrentRoomName()));
			network.logger(U"- isOpen: {}"_fmt(room.isOpen));
			network.logger(U"- stats: {} / {}"_fmt(room.playerCount, room.maxPlayers));
			network.logger(U"- properties: {}"_fmt(Format(room.properties)));
			network.logger(U"- visibleRoomPropertyKeys: {}"_fmt(network.getVisibleRoomPropertyKeys()));
			network.logger(U"- players:");
			for (const auto& player : players)
			{
				network.logger(U"-- userName: {}"_fmt(player.userName));
				network.logger(U"-- userID: {}"_fmt(player.userID));
				network.logger(U"-- localID: {}"_fmt(player.localID));
				network.logger(U"-- isHost: {}"_fmt(player.isHost));
				network.logger(U"-- isActive: {}"_fmt(player.isActive));
			}
		}

		if (SimpleGUI::Button(U"GetPlayerProperties", { x = initX, y += offsetY }, ButtonWidth))
		{
			auto playerID = network.getLocalPlayerID();
			auto properties = network.getPlayerProperties(playerID);
			network.logger(U"GetPlayerProperties: ");
			network.logger(U"- properties: {}"_fmt(Format(properties)));
		}

		if (SimpleGUI::Button(U"GetPlayerProperties Of", { x += offsetX, y }, ButtonWidth))
		{
			auto player = network.getLocalPlayerByName(text.text);
			if (player)
			{
				auto playerID = player.value().localID;
				auto properties = network.getPlayerProperties(playerID);
				network.logger(U"GetPlayerProperties: ");
				network.logger(U"- properties: {}"_fmt(Format(properties)));
			}
		}

		if (SimpleGUI::Button(U"SetPlayerProperty 1", { x += offsetX, y }, ButtonWidth))
		{
			network.setPlayerProperty(U"1", U"apple");
		}

		if (SimpleGUI::Button(U"SetPlayerProperty 2", { x += offsetX, y }, ButtonWidth))
		{
			network.setPlayerProperty(U"2", U"banana");
		}

		if (SimpleGUI::Button(U"RemovePlayerProperty 1", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.removePlayerProperty(U"1");
		}

		if (SimpleGUI::Button(U"RemovePlayerProperty 2", { x += offsetX, y }, ButtonWidth))
		{
			network.removePlayerProperty(U"2");
		}

		if (SimpleGUI::Button(U"RemovePlayerProperty 1, 2", { x += offsetX, y }, ButtonWidth))
		{
			network.removePlayerProperty({U"1", U"2"});
		}

		if (SimpleGUI::Button(U"GetRoomProperties", { x = initX, y += offsetY }, ButtonWidth))
		{
			auto properties = network.getRoomProperties();
			network.logger(U"GetRoomProperties: ");
			network.logger(U"- properties: {}"_fmt(Format(properties)));
		}

		if (SimpleGUI::Button(U"SetRoomProperty 1", { x += offsetX, y }, ButtonWidth))
		{
			network.setRoomProperty(U"1", U"apple");
		}

		if (SimpleGUI::Button(U"SetRoomProperty 2", { x += offsetX, y }, ButtonWidth))
		{
			network.setRoomProperty(U"2", U"banana");
		}

		if (SimpleGUI::Button(U"RemoveRoomProperty 1", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.removeRoomProperty(U"1");
		}

		if (SimpleGUI::Button(U"RemoveRoomProperty 2", { x += offsetX, y }, ButtonWidth))
		{
			network.removeRoomProperty(U"2");
		}

		if (SimpleGUI::Button(U"RemoveRoomProperty 1, 2", { x += offsetX, y }, ButtonWidth))
		{
			network.removeRoomProperty({ U"1", U"2" });
		}

		if (SimpleGUI::Button(U"GetPlayerPropertyByKey", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.logger(U"GetPlayerPropertyByKey");
			network.logger(U"- {} : {}"_fmt(text.text, network.getPlayerProperty(text.text)));
		}

		if (SimpleGUI::Button(U"GetRoomPropertyByKey", { x += offsetX, y }, ButtonWidth))
		{
			network.logger(U"GetRoomPropertyByKey");
			network.logger(U"- {} : {}"_fmt(text.text, network.getPlayerProperty(text.text)));
		}

		if (SimpleGUI::Button(U"SetIsOpenInCurrentRoom: true", { x = initX, y += offsetY }, ButtonWidth))
		{
			network.logger(U"SetIsOpenInCurrentRoom: true");
			network.setIsOpenInCurrentRoom(true);
		}

		if (SimpleGUI::Button(U"SetIsOpenInCurrentRoom: false", { x += offsetX, y }, ButtonWidth))
		{
			network.logger(U"SetIsOpenInCurrentRoom: false");
			network.setIsOpenInCurrentRoom(false);
		}

		if (SimpleGUI::Button(U"SetIsVisibleInCurrentRoom: true", { x += offsetX, y }, ButtonWidth))
		{
			network.logger(U"SetIsOpenInCurrentRoom: true");
			network.setIsVisibleInCurrentRoom(true);
		}

		if (SimpleGUI::Button(U"SetIsVisibleInCurrentRoom: false", { x += offsetX, y }, ButtonWidth))
		{
			network.logger(U"SetIsOpenInCurrentRoom: false");
			network.setIsVisibleInCurrentRoom(false);
		}

		{
			String state;

			switch (network.getClientState())
			{
			case ClientState::Disconnected:
				state = U"Disconnected";
				break;
			case ClientState::ConnectingToLobby:
				state = U"ConnectingToLobby";
				break;
			case ClientState::InLobby:
				state = U"InLobby";
				break;
			case ClientState::JoiningRoom:
				state = U"JoiningRoom";
				break;
			case ClientState::InRoom:
				state = U"InRoom";
				break;
			case ClientState::LeavingRoom:
				state = U"LeavingRoom";
				break;
			case ClientState::Disconnecting:
				state = U"Disconnecting";
				break;
			}

			font(state).drawAt(Rect{ x = initX, y += offsetY, ButtonWidth, 40 }.center());
		}

		if (network.isInLobby())
		{
			font(U"Online: {}"_fmt(
				network.getCountPlayersOnline()
			)).drawAt(Rect{ x += offsetX, y, ButtonWidth, 40 }.center());

			font(U"InGame: {}"_fmt(
				network.getCountPlayersIngame()
			)).drawAt(Rect{ x += offsetX, y, ButtonWidth, 40 }.center());

			font(U"Room: {}"_fmt(
				network.getCountGamesRunning()
			)).drawAt(Rect{ x += offsetX, y, ButtonWidth, 40 }.center());
		}
		else if (network.isInRoom())
		{
			font(U"Room: {} [{} / {}]"_fmt(
				network.getCurrentRoomName(),
				network.getPlayerCountInCurrentRoom(),
				network.getMaxPlayersInCurrentRoom()
			)).drawAt(Rect{ x += offsetX, y, ButtonWidth, 40 }.center());

			font(U"isOpen: {}"_fmt(
				network.getIsOpenInCurrentRoom()
			)).drawAt(Rect{ x += offsetX, y, ButtonWidth, 40 }.center());

			font(U"isVisible: {}"_fmt(
				network.getIsVisibleInCurrentRoom()
			)).drawAt(Rect{ x += offsetX, y, ButtonWidth, 40 }.center());
		}

		scrollBar.draw(ColorF(Palette::White).withAlpha(100));
	}
}
