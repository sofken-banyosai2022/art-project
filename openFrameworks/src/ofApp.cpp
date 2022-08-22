#include <iostream>
#include <Windows.h>
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(64); // 背景色を設定
    ofSetFrameRate(60); // フレームレートを設定
    ofSetWindowTitle("Art_Project"); // ウィンドウタイトルを設定

    /* ==============================
      OSC
    ============================== */
    receiver.setup(PORT);  // 受信機を設定
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

        // ログ出力
        // std::cout << m << endl;

        // アドレス分岐処理
        if (m.getAddress() == "/mouse/button") {

            if (m.getArgAsString(0) == "up") {

                // ログ出力
                std::cout << m.getArgAsString(0) << endl;

                // ソフトを起動し、音源再生
                ShellExecute(NULL, "open", "filePath", NULL, NULL, SW_SHOWNORMAL);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

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
