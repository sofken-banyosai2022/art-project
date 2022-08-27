#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

/* グローバル変数 */
#define HOST "***.***.***.***" // 送信先のIPアドレス
#define PORT ***** // ポート番号

class ofApp : public ofBaseApp{

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

	/* OSC */
	ofxOscReceiver receiver; // 受信機
	ofxOscSender sender;	 // 送信機
};
