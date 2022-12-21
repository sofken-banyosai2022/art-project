#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"

class Scheduler : public ofThread{

public:
	ofxOscSender sender; // oscメッセージsender

	void setPeriodicEvent(uint64_t time) {
		timer.setPeriodicEvent(time);
	}

private:
	ofTimer timer; // タイマー

	void threadedFunction() {
		while (isThreadRunning()) {
			timer.waitNext();
			sendOsc(); // osc送信
		}
	}

	/* osc送信 */
	void sendOsc() {
		ofxOscMessage m; // oscメッセージ

		m.setAddress("/mp/beat");  // アドレス設定
		m.addIntArg(1);

		sender.sendMessage(m); // oscメッセージを送信
		cout << "[osc] send " << m << endl; // ログ出力
	}
};

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
		void exit();

		/* ofxOsc */
		ofxOscReceiver receiver; // oscメッセージreceiver

		/* ofxGui */
		ofxPanel gui;
		ofxTextField ipAddress; // 送信先のip address
		ofxIntField mainPort; // LED Controller側のoscポート番号
		ofxIntField subPort; // Music Player側のoscポート番号
		ofSoundPlayer sound[10]; // シーンの音源配列

		void onIpAddressChanged(string &data); // ip addressを変更
		void onMainPortChanged(int &data); // mainPortを変更
		void onSubPortChanged(int& data); // subPortを変更
		void startScene(int sceneNumber, int bpm); // シーンを開始

		/* other */
		const int OF_KEY_SPACE = 32; // スペースキー
		Scheduler output; // bpmに従って信号を出力
};