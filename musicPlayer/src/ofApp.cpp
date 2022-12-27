#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60); // フレームレートを設定
	ofSetWindowTitle("Music Player"); // ウィンドウタイトルを設定
	ofBackground(0, 0, 0); // 背景色を黒に設定

	/* イベントリスナー */
	ipAddress.addListener(this, &ofApp::onIpAddressChanged); // ip addressのイベントリスナーを追加
	mainPort.addListener(this, &ofApp::onMainPortChanged); // mainPortのイベントリスナーを追加
	subPort.addListener(this, &ofApp::onSubPortChanged); // subPortのイベントリスナーを追加

	/* ofxGui */
	gui.setup("Settings", "settings"); // GUIを設定
	gui.add(ipAddress.setup("IP address", "127.0.0.1")); // 送信先のip address
	gui.add(mainPort.setup("main port", 12345, 0, 65535)); // oscポート番号
	gui.add(subPort.setup("sub port", 12346, 0, 65535)); // oscポート番号

	/* sound */
	ofFile file("soundList.json"); // JSONファイルを開く

	if (file.exists()) file >> soundList; // JSONファイル読み込み

	// 音源を読み込み
	for (size_t i = 0; i < soundList["sound"].size(); i++) {
		string path = soundList["sound"][i]["path"].dump();

		path = path.substr(1, path.size() - 2).c_str(); // 文字列からpathのみ抜き出す
		sound[i].load(path); // 音源をロード
	}

	/* ofxOsc */
	receiver.setup(mainPort); // receiverを設定
	output.sender.setup(ipAddress, subPort); // senderを設定
}

//--------------------------------------------------------------
// イベントリスナー

// IP addressを変更
void ofApp::onIpAddressChanged(string &data) {
	output.sender.setup(ipAddress, subPort); // senderを設定
}

// mainPortを変更
void ofApp::onMainPortChanged(int &data) {
	receiver.setup(mainPort); // receiverを設定
}

// subPortを変更
void ofApp::onSubPortChanged(int& data) {
	output.sender.setup(ipAddress, subPort); // senderを設定
}

// シーンを開始
void ofApp::startScene(int sceneNumber) {
	currentSoundNumber = sceneNumber - (uint64_t)1; // 現在再生中の音源番号

	int bpm = soundList["sound"][currentSoundNumber]["bpm"];

	ofSoundStopAll(); // 音源を停止
	output.waitForThread(); // outputを停止
	sound[currentSoundNumber].play(); // 音源を再生
	output.setPeriodicEvent(uint64_t((60.0 / bpm) * 1000000000)); // bpmをnanosecondsに変換して設定
	output.startThread(); // outputを開始
	cout << "[scene] start " << sceneNumber << endl; // ログ出力
}

//--------------------------------------------------------------
void ofApp::update(){

	/* osc */
	while (receiver.hasWaitingMessages()) { // メッセージが送られてくるまで待機
		ofxOscMessage m;

		receiver.getNextMessage(m);
		cout << "[osc] receive " << m << endl; // ログ出力

		if (m.getAddress() == "/ledc/play") {
			startScene(m.getArgAsInt(0)); // シーンを開始
		}
	}

	/* soundの終了検知 */
	if (currentSoundNumber >= 0) {
		if (!sound[currentSoundNumber].isPlaying()) {
			output.waitForThread(); // outputを停止
			currentSoundNumber = -1;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	/* ofxGui */
	gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == OF_KEY_SPACE) { // スペースを押したらシーンをリセット
		cout << "[system] stop all" << endl; // ログ出力

		ofSoundStopAll(); // 音源を停止
		output.waitForThread(); // outputを停止
	}
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

//--------------------------------------------------------------
void ofApp::exit() {
	output.waitForThread(); // outputを停止
}
