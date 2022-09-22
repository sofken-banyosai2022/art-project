#include <Windows.h>
#include "ofApp.h"

using namespace std;

//--------------------------------------------------------------
/* split */
vector<string> split(const string& s, char delim) {
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
void ofApp::setup(){
    ofBackground(64);                       // 背景色を設定
    ofSetFrameRate(60);                     // フレームレートを設定
    ofSetWindowTitle("Art_Project");        // ウィンドウタイトルを設定
    ofSetCircleResolution(64);              // 円の解像度を設定

    /* ==============================
       File
    ============================== */
    ofFile file("settings.json");           // JSONファイルを開く

    if (file.exists()) {                    // JSONファイルが存在する場合
        file >> settings;
    }

    /* ==============================
       GUI
    ============================== */

    // colorの初期値、最小値、最大値を設定
    ofColor initColor = ofColor(255, 255, 255, 255);
    ofColor minColor = ofColor(0, 0, 0, 0);
    ofColor maxColor = ofColor(255, 255, 255, 255);

    gui.setup();                                                    // GUI設定
    gui.add(oscSend.setup("OSC Send"));                             // ボタン追加
    gui.add(oscTest.setup("OSC Test", false));                      // トグルボタン追加
    gui.add(color.setup("color", initColor, minColor, maxColor));   // 色スライダ追加
    oscSend.addListener(this, &ofApp::oscSendPressed);              // イベントリスナー追加

    /* ==============================
       OSC
    ============================== */
    receiver.setup(PORT);     // 受信機を設定
    sender.setup(HOST, PORT); // 送信機を設定

    /* ==============================
       Other
    ============================== */
    oscTestType = true;
}

//--------------------------------------------------------------
void ofApp::update(){

    /* ==============================
       OSC
    ============================== */

    // メッセージが送られてくるまで待機
    while (receiver.hasWaitingMessages()) {

        // 次のメッセージを取得
        ofxOscMessage m;
        receiver.getNextMessage(m);
        LOG("OSC Receive", m); // ログ出力

        if (m.getAddress() == "/unity/bgm/play") {        // BGMを再生

            ShellExecute(NULL,"open", settings["bgm"][m.getArgAsInt(0) - 1.0]["path"].dump().c_str(), NULL, NULL, SW_SHOWNORMAL);

        } else if (m.getAddress() == "/unity/bgm/stop") { // BGMを停止

            HWND handle = FindWindow("MPC-BE", NULL);
            PostMessage(handle, WM_CLOSE, 0, 0);

        }
    }

    /* ==============================
       Other
    ============================== */

    // oscTestがTrueのとき1秒ごとに実行
    if (oscTest && ofGetFrameNum() % 60 == 0) {
        vector<string> colorResult = split(color.getParameter().toString(), ','); // colorを配列に変換

        // 値を設定
        message.mode1 = 1;
        message.number -> push_back(100);

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
        sendOSC(); // OSCを送信
    }
}
//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(color); // 色を設定
    ofCircle(ofGetWidth() / 2, ofGetHeight() / 2, 200); // 円を描画

    /* ==============================
       GUI
    ============================== */
    gui.draw(); // Guiを描画
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
/* OSC送信 */
void ofApp::sendOSC() {
    ofxOscMessage m;                            // OSCメッセージ
    int length = message.number -> size();      // message.numberの長さ

    m.setAddress("/of/esp");                    // アドレス設定
    m.addIntArg(message.mode1);
    m.addIntArg(length);

    for (size_t i = 0; i < length; i++){
        m.addIntArg(message.number -> at(i));
    }

    m.addIntArg(message.color[0]);
    m.addIntArg(message.color[1]);
    m.addIntArg(message.color[2]);

    sender.sendMessage(m);                      // OSCメッセージを送信
    message.number -> clear();                  // 要素を全て削除
    LOG("OSC Send", m);                         // ログ出力
}

//--------------------------------------------------------------
/* ボタンが押されたとき */
void ofApp::oscSendPressed() {
    vector<string> colorResult = split(color.getParameter().toString(), ','); // colorを配列に変換

    // 値を設定
    message.mode1 = 1;
    message.number -> push_back(100);
    message.color[0] = stoi(colorResult[0]);
    message.color[1] = stoi(colorResult[1]);
    message.color[2] = stoi(colorResult[2]);

    sendOSC(); // OSCを送信
}

//--------------------------------------------------------------
/* ログ出力 */
void ofApp::LOG(string category, ofxOscMessage message) {
    string text = "[" + category + "] ";
    cout << text << message << endl;
}
