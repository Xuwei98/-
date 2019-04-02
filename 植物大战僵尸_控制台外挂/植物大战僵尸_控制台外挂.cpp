// 植物大战僵尸_控制台外挂.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

/*
WriteProcessMemory
-->OpenProcess 提供hProcess
-->GetWindowThreadProcessID 提供hProcessID
-->FindWindow 	提供hWnd

窗口句柄->进程ID->进程句柄
*/
HWND hwnd_Game;
DWORD ProcessID;
HANDLE h_process;
int address = 0x13F05610;

void GetGameInfo() {
	hwnd_Game = FindWindow(NULL, L"Plants vs. Zombies");
	GetWindowThreadProcessId(hwnd_Game, &ProcessID);
	h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessID);
}

void ChangeSunshine(int num) {
	int ret = WriteProcessMemory(h_process, (LPVOID)address, &num, 4, NULL);
	if (ret) {
		std::cout << "修改成功" << std::endl;
	}
	else {
		std::cout << "修改失败" << std::endl;
	}
}
int main(int argc, char * argv[]) {
	int subshine_num = 0;
	std::cout << "使用阳光修改器 "
		<< std::endl;
	std::cin >> subshine_num;
	GetGameInfo();
	ChangeSunshine(subshine_num);

	system("pause");
	return 0;

}


