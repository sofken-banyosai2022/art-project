﻿#include <iostream>
#include <Windows.h>
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(64); // 背景色を設定
    ofSetFrameRate(60); // フレームレートを設定
    ofSetWindowTitle("Art_Project"); // ウィンドウタイトルを設定

    /* ==============================
       GUI
    ============================== */
    gui.setup(); // GUI設定
    gui.add(button.setup("send OSC")); // ボタン追加
    button.addListener(this, &ofApp::buttonPressed); // イベントリスナー追加

    /* ==============================
       OSC
    ============================== */
    receiver.setup(PORT);     // 受信機を設定
    sender.setup(HOST, PORT); // 送信機を設定
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
        LOG("OSC", m); // ログ出力

        // アドレス分岐処理
        if (m.getAddress() == "/mouse/button") {

            if (m.getArgAsString(0) == "up") {

                // ソフトを起動し、音源再生
                ShellExecute(NULL, "open", "filePath", NULL, NULL, SW_SHOWNORMAL);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

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
/* OSCを送信 */
void ofApp::sendOSC() {
    ofxOscMessage m;             // OSCメッセージ
    m.setAddress("/lambda/msg"); // アドレス設定
    m.addIntArg(1);              // 値を設定
    sender.sendMessage(m);       // OSCメッセージを送信
    LOG("OSC", m);               // ログ出力
}

//--------------------------------------------------------------
/* ボタンが押されたとき */
void ofApp::buttonPressed() {
    sendOSC(); // OSCを送信
}

//--------------------------------------------------------------
/* ログ出力 */
void ofApp::LOG(string category, ofxOscMessage message) {
    string text = "[" + category + "] ";
    std::cout << text << message << endl;
}