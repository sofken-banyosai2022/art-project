#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60); // フレームレートを設定
	ofSetWindowTitle("LED Controller"); // ウィンドウタイトルを設定
	ofBackground(0, 0, 0); // 背景色を黒に設定

	/* イベントリスナー */
	ipAddress.addListener(this, &ofApp::onIpAddressChanged); // ip addressのイベントリスナーを追加
	mainPort.addListener(this, &ofApp::onMainPortChanged); // mainPortのイベントリスナーを追加
	subPort.addListener(this, &ofApp::onSubPortChanged); // subPortのイベントリスナーを追加
	number.addListener(this, &ofApp::onNumberChanged); // numberのイベントリスナーを追加
	scene01.addListener(this, &ofApp::startScene01); // scene01のイベントリスナーを追加

	/* ofxGui */

	// colorの初期値、最小値、最大値を設定
	ofColor initColor = ofColor(255, 255, 255, 255);
	ofColor minColor = ofColor(0, 0, 0, 0);
	ofColor maxColor = ofColor(255, 255, 255, 255);

	// 設定
	gui.setup("Settings", "settings"); // GUIを設定
	gui.add(oscSettingsLabel.setup("OSC Settings", "")); // osc settings label
	gui.add(ipAddress.setup("IP address", "127.0.0.1")); // 送信先のip address
	gui.add(mainPort.setup("main port", 12345, 0, 65535)); // oscポート番号
	gui.add(subPort.setup("sub port", 12346, 0, 65535)); // oscポート番号
	gui.add(interval.setup("interval", 1, 1, 10)); // OscTestの送信間隔
	gui.add(oscTest.setup("OSC Test", false)); // osc送信テストボタン

	gui.add(dataSettingsLabel.setup("Data Settings", "")); // data settings label
	gui.add(message.mode1.setup("mode1", 2, 0, 2)); // 送信モードを指定
	gui.add(number.setup("number", "100")); // numberフィールド
	gui.add(color.setup("color", initColor, minColor, maxColor)); // colorスライダ
	gui.add(message.mode2.setup("mode2", 100, 0, 255)); // 遅延時間を指定

    // シーン
	sceneGui.setup("Scene", "scene", 230.0f); // GUIを設定
	sceneGui.add(scene01.setup("scene 01")); // シーン01ボタン

	/* ofxOsc */
	receiver.setup(subPort); // receiverを設定
	sender.setup(ipAddress, mainPort); // senderを設定
}

//--------------------------------------------------------------
// イベントリスナー

// IP addressを変更
void ofApp::onIpAddressChanged(string& ipAddress) {
	sender.setup(ipAddress, mainPort); // senderを設定
}

// mainPortを変更
void ofApp::onMainPortChanged(int& data) {
	sender.setup(ipAddress, mainPort); // senderを設定
}

// subPortを変更
void ofApp::onSubPortChanged(int& data) {
	receiver.setup(subPort); // receiverを設定
}

// numberを変更
void ofApp::onNumberChanged(string& number) {
	vector<string> numberResult = split(number, ','); // numberを配列に変換
	message.number -> clear(); // 要素を全て削除

	// 値を設定
	for (size_t i = 0; i < numberResult.size(); i++) {
		message.number -> push_back(stoi(numberResult[i]));
	}
}

// シーン
// scene01を開始
void ofApp::startScene01() {
	sendToMusicPlayer(1, 76); // Music Playerに送信
}

//--------------------------------------------------------------
/* OSC送信 */
void ofApp::sendOsc() {
	ofxOscMessage m; // oscメッセージ
	int length = message.number -> size(); // message.numberの長さ

	m.setAddress("/of/esp");  // アドレス設定
	m.addIntArg(message.mode1);
	m.addIntArg(length);

	for (size_t i = 0; i < length; i++) m.addIntArg(message.number -> at(i));

	m.addIntArg(message.color[0]);
	m.addIntArg(message.color[1]);
	m.addIntArg(message.color[2]);
	m.addIntArg(message.mode2);

	sender.sendMessage(m); // oscメッセージを送信
	cout << "[osc send] " << m << endl; // ログ出力
}

// Music Playerに送信
void ofApp::sendToMusicPlayer(int sceneNumber, int bpm) {
	ofxOscMessage m; // oscメッセージ

	m.setAddress("/ledc/play");  // アドレス設定
	m.addIntArg(sceneNumber);
	m.addIntArg(bpm);

	sender.sendMessage(m); // oscメッセージを送信

	cout << "[scene] start " << sceneNumber << endl; // ログ出力
	cout << "[osc send] " << m << endl; // ログ出力
};

//--------------------------------------------------------------
/* 文字列を特定文字で分割 */
vector<string> ofApp::split(const string& s, char delim) {
	vector<string> elems;
	stringstream ss(s);
	string item;

	while (getline(ss, item, delim)) {
		if (!item.empty()) {
			elems.push_back(item);
		}
	}

	return elems;
}

//--------------------------------------------------------------
/* midiデータを取得 */
void ofApp::getMidiData(int knob, int value) {
	int adjustedValue = (int)(value * (255.0 / 127.0)); // 最大を255とする

	if (adjustedValue % 15 != 0) return; // 送信数を減らす

	switch (knob) { // ノブ番号
		case 0:
		case 1:
		case 2:
			message.color[knob] = adjustedValue;
			break;
		case 3:
			message.color[0] = adjustedValue;
			message.color[1] = adjustedValue;
			message.color[2] = adjustedValue;
			break;
		default:
			cout << "[getMidiData] 設定されていない動作です" << endl; // ログ出力
			return;
	}

	sendOsc(); // oscを送信
}

//--------------------------------------------------------------
void ofApp::update(){

	/* osc */
	while (receiver.hasWaitingMessages()) { // メッセージが送られてくるまで待機
		ofxOscMessage m;

		receiver.getNextMessage(m);
		cout << "[osc] receive " << m << endl; // ログ出力

		if (m.getAddress() == "/midi") { // midi
			getMidiData(m.getArgAsInt(0), m.getArgAsInt(1)); // midiデータを取得
		}
	}

	/* oscTestがTrueのときinterval秒ごとに実行 */
	if (oscTest && ofGetFrameNum() % ((uint64_t)interval * 60) == 0) {
		vector<string> colorResult = split(color.getParameter().toString(), ','); // colorを配列に変換

		if (oscTestType) {
			message.color[0] = stoi(colorResult[0]);
			message.color[1] = stoi(colorResult[1]);
			message.color[2] = stoi(colorResult[2]);
		}
		else {
			message.color[0] = 0;
			message.color[1] = 0;
			message.color[2] = 0;
		}

		oscTestType = !oscTestType; // oscTestTypeを反転
		sendOsc(); // oscを送信
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	/* ofxGui */
	gui.draw(); // 設定
	sceneGui.draw(); // シーン
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
