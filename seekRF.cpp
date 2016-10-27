// seekRF.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#include <stdexcept>
#include "stdafx.h"
#include "DxLib.h"
#include <iostream> 
#include <Windows.h>
#include <sstream>
#include <string>
#include <vector>

#define PI 3.141592654
#define VK_0 0x30
#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33
#define VK_4 0x34
#define VK_5 0x35
#define VK_6 0x36
#define VK_7 0x37
#define VK_8 0x38
#define VK_9 0x39

std::vector<std::string> getImgFiles(const std::string& dir_name, const std::string& extension) noexcept(false)
{
	HANDLE hFind;
	WIN32_FIND_DATA win32fd;//defined at Windwos.h
	std::vector<std::string> file_names;

	//拡張子の設定
	std::string search_name = dir_name + "\\*." + extension;

	hFind = FindFirstFile(search_name.c_str(), &win32fd);

	if (hFind == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("File not found! Check your .ini file!");
	}

	do {
		if (win32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		}
		else {
			file_names.push_back(win32fd.cFileName);
		}
	} while (FindNextFile(hFind, &win32fd));

	FindClose(hFind);
	return file_names;
}

int main()
{
	BYTE KeyState[256];
	HMONITOR hMonitor;
	MONITORINFOEX  MonitorInfoEx;
	POINT pt = { 100, 100 };
	int dirmode = 0;
	int height, width, x, y, point, i, toggle= 0; 
	int SizeX, SizeY = 0;
	double ang = 0;
	unsigned int Handles[2048 * 16];
	unsigned int Handles2[2048 * 16];

	char cfile[256];
	int textc = GetColor(255, 0, 0);
	const std::string dirname  = "gratings";
	const std::string dirname2 = "imgs";
	float ex = 1.0;

	// Monitor Info
	hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
	MonitorInfoEx.cbSize = sizeof(MonitorInfoEx);
	GetMonitorInfo(hMonitor, &MonitorInfoEx);
	height = MonitorInfoEx.rcMonitor.bottom - MonitorInfoEx.rcMonitor.top;
	width = MonitorInfoEx.rcMonitor.right - MonitorInfoEx.rcMonitor.left;

	std::cout << "Width : " + std::to_string(width)  + "\r\n";
	std::cout << "Height: " + std::to_string(height) + "\r\n";
	char str[128];
	std::cout << "Input anything";
	std::cin >> str;
	std::cout << str;

	// ファイル名取得
	std::vector<std::string> filenames;
	std::vector<std::string> filenames2;

	//vector<std::string> filenames;
	filenames = getImgFiles(dirname, "png");
	filenames2 = getImgFiles(dirname2, "png");


	// DXlib
	int bg = 127;
	SetBackgroundColor(bg, bg, bg);
	SetWaitVSyncFlag(true);
	SetGraphMode(width, height, 32);
	ChangeWindowMode(false), DxLib_Init(), SetDrawScreen(DX_SCREEN_BACK); //ウィンドウモード変更,初期化,裏画面設定

	for (size_t i = 0; i < filenames.size(); i++)
	{
		Handles[i] = LoadGraph((dirname + "\\" + filenames[i]).c_str());
		if (0 == i % 128) {
			ProcessMessage();
			ClearDrawScreen();
			DrawFormatString(0, 0, textc, "%d / %d : images, %s", i, filenames.size(), filenames[i].c_str());
			ScreenFlip();
			ScreenFlip();
		}
		if (GetAsyncKeyState(VK_ESCAPE)) {
			ProcessMessage();
			ClearDrawScreen();
			DxLib_End();
			return -1;
		}
	}
	for (size_t i = 0; i < filenames2.size(); i++)
	{
		Handles2[i] = LoadGraph((dirname2 + "\\" + filenames2[i]).c_str());
		if (0 == i % 128) {
			ProcessMessage();
			ClearDrawScreen();
			DrawFormatString(0, 0, textc, "%d / %d : images, %s", i, filenames2.size(), filenames2[i].c_str());
			ScreenFlip();
			ScreenFlip();
		}
		if (GetAsyncKeyState(VK_ESCAPE)) {
			ProcessMessage();
			ClearDrawScreen();
			DxLib_End();
			return -1;
		}
	}


	toggle = 1;
	i = 1;
	while (!ScreenFlip() && !ProcessMessage() && !ClearDrawScreen()) {
		point = GetMouseInput();                //マウスの入力状態取得
		GetMousePoint(&x, &y);

		switch (dirmode)
		{
		case 0:
			GetGraphSize(Handles[i % filenames.size()], &SizeX, &SizeY);
			DrawRotaGraph(x, y, ex*toggle, ang, Handles[i % filenames.size()], FALSE, TRUE);
			DrawFormatString(width / 2, 0, textc, "No: %d  X = %d, Y = %d", i % filenames.size(), x, y);
			i = i + GetMouseWheelRotVol();
			i = i % filenames.size();
			break;
		case 1: 
			GetGraphSize(Handles2[i % filenames2.size()], &SizeX, &SizeY);
			DrawRotaGraph(x, y, ex*toggle, ang, Handles2[i % filenames2.size()], FALSE, TRUE);
			DrawFormatString(width / 2, 0, textc, "No: %d  X = %d, Y = %d", i % filenames2.size(), x, y);
			i = i + GetMouseWheelRotVol();
			i = i % filenames2.size();
			break;
		}

		DrawFormatString(width/2, 20, textc, "(sizeX, sizeY) = %f, Y = %f,  magni = %f", SizeX*ex, SizeY*ex, ex);

		GetKeyboardState(KeyState);
		if (KeyState[VK_ESCAPE] & 0x80) {
			ProcessMessage();
			ClearDrawScreen();
			break;
		}
		if (KeyState[VK_0] & 0x80) {
			dirmode = 0;
		}
		if (KeyState[VK_1] & 0x80) {
			dirmode = 1;
		}

		if (KeyState[VK_UP] & 0x80){
			ex = ex + 0.05;
		}
		if (KeyState[VK_DOWN] & 0x80){
			ex = ex - 0.05;
		}
		if (KeyState[VK_LEFT] & 0x80) {
			ang = ang - PI/18.0;
		}
		if (KeyState[VK_RIGHT] & 0x80) {
			ang = ang +  PI / 18.0;
		}
		if (KeyState[VK_LBUTTON] & 0x80) {
			toggle = 0;
		}
		else {
			toggle = 1;
		}
		if (KeyState[VK_SPACE] & 0x80) {
			ang = 0;
			ex = 1;
		}
		if (KeyState[VK_MBUTTON] & 0x80) {
			i = i + 1;
		}

	}


	DxLib_End();				// ＤＸライブラリ使用の終了処理

    return 0;
}

