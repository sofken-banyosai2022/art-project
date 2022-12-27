#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	static const int OF_KEY_SPACE = 32; // スペースキー
	static const int MAX_SCENE = 10; // 最大のシーン数

	/* ofxOsc */
	struct messageStruct {
		ofxIntSlider mode1; // 送信モードを指定
		std::vector<int> number[10]; // 送信するユニットを指定
		int color[3]; // 送信する色を指定
		ofxIntSlider mode2; // 遅延時間を指定
	};

	ofxOscReceiver receiver; // oscメッセージreceiver
	ofxOscSender sender; // 運営側のoscメッセージsender
	ofxOscSender ledSender; // LED側のoscメッセージsender
	messageStruct message; // osc送信用メッセージ

	void sendOsc(); // osc送信

	/* ofxGui */

	// 設定
	ofxPanel gui;
	ofxLabel oscSettingsLabel; // osc settings label
	ofxTextField ipAddress; // 運営側のip address
	ofxTextField ledIpAddress; // LED側のip address
	ofxIntField mainPort; // LED Controller側のoscポート番号
	ofxIntField subPort; // Music Player側のoscポート番号
	ofxToggle oscTest; // osc送信テストボタン
	ofxIntSlider interval; // OscTestの送信間隔
	bool oscTestType = true; // OscTestにおいてLED状態を判定

	ofxLabel dataSettingsLabel; // data settings label
	ofxTextField number; // numberフィールド
	ofxColorSlider color; // colorスライダ

	void onIpAddressChanged(string& ipAddress); // 運営側のip addressを変更
	void onLedIpAddressChanged(string& ledIpAddress); // LED側のip addressを変更
	void onMainPortChanged(int& data); // mainPortを変更
	void onSubPortChanged(int& data); // subPortを変更
	void onNumberChanged(string& number); // numberを変更

	/* シーン */
	ofxPanel sceneGui;
	ofxButton scene[MAX_SCENE]; // シーンボタン
	bool musicPlayerType = true; // music playerにおいてLED状態を判定

	void startScene1(); // シーン1を開始
	void startScene2(); // シーン2を開始
	void startScene3(); // シーン3を開始
	void startScene4(); // シーン4を開始
	void startScene5(); // シーン5を開始
	void startScene6(); // シーン6を開始
	void startScene7(); // シーン7を開始
	void startScene8(); // シーン8を開始
	void startScene9(); // シーン9を開始
	void startScene10(); // シーン10を開始
	void sendToMusicPlayer(int sceneNumber); // Music Playerに送信

	/* other */
	uint8_t currentColor[3] = {0, 0, 0}; // 現在のLEDの色

	vector<string> split(const string& s, char delim); // 文字列を特定文字で分割
	void getMidiData(int knob, int value); // midiデータを取得
	void getMusicPlayerData(int beat); // music playerデータを取得
};
