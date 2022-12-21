#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		/* ofxOsc */
		ofxOscReceiver receiver; // oscメッセージreceiver
		ofxOscSender sender; // oscメッセージsender
		struct messageStruct {
			ofxIntSlider mode1; // 送信モードを指定
			std::vector<int> number[10]; // 送信するユニットを指定
			int color[3]; // 送信する色を指定
			ofxIntSlider mode2; // 遅延時間を指定
		};
		messageStruct message; // osc送信用メッセージ

		void sendOsc(); // osc送信

		/* ofxGui */

		// 設定
		ofxPanel gui;
		ofxLabel oscSettingsLabel; // osc settings label
		ofxTextField ipAddress; // 送信先のip address
		ofxIntField mainPort; // LED Controller側のoscポート番号
		ofxIntField subPort; // Music Player側のoscポート番号
		ofxToggle oscTest; // osc送信テストボタン
		ofxIntSlider interval; // OscTestの送信間隔
		bool oscTestType = true; // OscTestにおいてLED状態を判定

		ofxLabel dataSettingsLabel; // data settings label
		ofxTextField number; // numberフィールド
		ofxColorSlider color; // colorスライダ

		void onIpAddressChanged(string& ipAddress); // ip addressを変更
		void onMainPortChanged(int& data); // mainPortを変更
		void onSubPortChanged(int& data); // subPortを変更
		void onNumberChanged(string& number); // numberを変更

		// シーン
		ofxPanel sceneGui;
		ofxButton scene01; // シーン01ボタン

		void sendToMusicPlayer(int sceneNumber, int bpm); // Music Playerに送信
		void startScene01(); // シーン01を開始

		/* other */
		const int OF_KEY_SPACE = 32; // スペースキー

		vector<string> split(const string& s, char delim); // 文字列を特定文字で分割
		void getMidiData(int knob, int value); // midiデータを取得
};
