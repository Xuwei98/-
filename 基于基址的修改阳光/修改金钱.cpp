#include "pch.h"

#include <iostream>
using namespace std;

HWND hwnd_process;
DWORD ProcessId;
HANDLE hProcess;
int Base;
int offset[2];
void  GetGameInfo() {
	hwnd_process = FindWindow(NULL, L"植物大战僵尸中文版");
	GetWindowThreadProcessId(hwnd_process, &ProcessId);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
	Base = 0x006A9EC0;
	offset[0] = 0x082C;
	offset[1] = 0x28;
}

int GetAddress(int Base, int offset[], int len) {
	int buffer;
	ReadProcessMemory(hProcess, (LPVOID)Base, &buffer, 4, NULL);
	for (size_t i = 0; i < len; i++)
	{
		if (i == len - 1)
			buffer += offset[i];
		else
			ReadProcessMemory(hProcess, (LPVOID)(buffer + offset[i]),  &buffer, 4,  NULL);
	}
	return buffer;
}

void ChangeMoney(int num) {
	int Address = GetAddress(Base, offset, 2);
	int ret = WriteProcessMemory(hProcess, (LPVOID)Address,&num , 4, NULL);
	if (ret)
		cout << "succeed" << endl;
	else
		cout << "falied" << endl;
}

int main(int argc, char *argv) {
	cout << "输入修改的金币量: " << endl;
	int num = 0;
	cin >> num;
	GetGameInfo();
	ChangeMoney(num);

	system("pause");
	return 0;
}
