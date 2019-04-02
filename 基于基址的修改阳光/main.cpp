// 基于基址的修改阳光.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
using namespace std;
HWND hwnd_game;
DWORD process_id;
HANDLE h_process;
LPVOID start_address_plantcall;
LPVOID start_address_plantparam;
int Base;
int offset_subshine[2];
int offset_money[2];

struct PlantParams
{
	UINT p_x_;//种植x坐标
	UINT p_y_;//种植y坐标
	UINT p_id_;// 植物id(种类)
};

//执行call函数关键代码
DWORD ThreadFuncCallPlants(LPVOID plantsparam) {
	PlantParams* plantparam_temp = (PlantParams*)plantsparam;
	UINT p_x= plantparam_temp->p_x_;
	UINT p_y = plantparam_temp->p_y_;
	UINT p_id = plantparam_temp->p_id_;
	_asm {
		pushad
		push - 1
		push p_id
		mov eax, p_y
		push p_x
		mov ebx, dword ptr ds : [0x006A9EC0]
		mov ebx, dword ptr ds : [ebx + 0x0768]
		push ebx
		mov edx, 0x0040D120
		call edx
		popad
	}
	return 0;
}

void CallPlant(PlantParams plantparam) {

	HANDLE h_thread;
	DWORD id_thread;
	//分配函数空间
	start_address_plantcall = VirtualAllocEx(h_process, NULL, 4096, MEM_COMMIT, PAGE_READWRITE);
	if (start_address_plantcall == NULL) {
		cout << "分配函数空间失败 " << endl;
		return;
	}

	//写入函数
	int ret = WriteProcessMemory(h_process, start_address_plantcall, ThreadFuncCallPlants, 4096, NULL);
	if (ret == 0) {
		cout << "写入函数失败" << endl;
	}

	//分配变量空间
	start_address_plantparam = VirtualAllocEx(h_process, NULL, sizeof(PlantParams), MEM_COMMIT, PAGE_READWRITE);
	if (start_address_plantparam == NULL) {
		cout << "分配变量空间失败" << endl;
		return;
	}
	// 写入变量
	ret = WriteProcessMemory(h_process, start_address_plantparam, (LPVOID)&plantparam, sizeof(plantparam), NULL);
	if (ret == 0) {
		cout << "写入变量失败" << endl;
	}
	//创建线程
	h_thread = CreateRemoteThread(h_process, NULL, NULL, (LPTHREAD_START_ROUTINE)start_address_plantcall,(LPVOID)start_address_plantparam, 0, &id_thread);
	cout << "正在调用call" << endl;
	WaitForSingleObject(h_thread, INFINITE);

	//释放虚拟内存
	VirtualFreeEx(h_process, start_address_plantcall, 4096, MEM_RELEASE);
	VirtualFreeEx(h_process, start_address_plantparam, sizeof(PlantParams), MEM_RELEASE);

	CloseHandle(h_thread);

}

//初始化全局信息
void GetGameInfo() {
	hwnd_game = FindWindow(NULL, L"Plants vs. Zombies");
	GetWindowThreadProcessId(hwnd_game, &process_id);
	h_process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);
	Base = 0x006A9EC0;
	offset_subshine[0] = 0x00000768 ;
	offset_subshine[1] = 0x00005560 ;

	offset_money[0] = 0x082C;
	offset_money[1] = 0x28;
};

void Clean() {
	if (h_process != NULL)
		CloseHandle(h_process);
}
//根据偏移读取数据
int GetDymThroughBase(int Base, int offset[],int len) {
	int buffer;
	ReadProcessMemory(h_process, (LPVOID)Base, &buffer, 4, NULL);
	for (int i = 0; i < len; i++)
	{
		if (i == len - 1)
			buffer += offset[i];
		else
			ReadProcessMemory(h_process, (LPVOID)(buffer + offset[i]), &buffer, 4, NULL);

	}
	return buffer;
}
//修改阳光
void ChangeSunshine(int num) {
	int Address = GetDymThroughBase(Base, offset_subshine, 2);
	int ret = WriteProcessMemory(h_process, (LPVOID)Address, &num, 4, NULL);
	if (ret) {
		std::cout << "succeed" << std::endl;
	}
	else {
		std::cout << "failed" << std::endl;
	}
}

//修改金币 
void ChangeMoney(int num) {
	int Address = GetDymThroughBase(Base, offset_money, 2);
	int ret = WriteProcessMemory(h_process, (LPVOID)Address, &num, 4, NULL);
	if (ret)
		cout << "succeed" << endl;
	else
		cout << "falied" << endl;
}

//免CD
void DisableCoolDown() {
	int code = 0x147F;
	int ret = WriteProcessMemory(h_process, (LPVOID)0x00487296, &code, 2, NULL);
	if (ret)
		cout << "succeed" << endl;
	else
		cout << "falied" << endl;
}

//支持后台使用
void BackGroundJob() {
	int address = 0x004502C0;
	int code = 0xC3;
	int ret = WriteProcessMemory(h_process, (LPVOID)0x004502C0, &code, 1, NULL);
	if (ret)
		cout << "succeed" << endl;
	else
		cout << "falied" << endl;
}

int main()
{
	int n_sunshine_change;
	int num;
	PlantParams plantparam;
	GetGameInfo();
	if (hwnd_game == NULL)
		goto end;
	while (TRUE) {
		cout << "欢迎使用植物大战僵尸外挂" << endl
			<< "请输入需要的功能: 1.修改阳光 2.修改金币 3.无冷却 4.种植植物 5.一键铺满 6.支持后台运行 7.结束 : ";
		cin >> num;
		switch (num)
		{
		case 1: {
			std::cout << "请输入你要修改的阳光值：";
			std::cin >> n_sunshine_change;
			ChangeSunshine(n_sunshine_change);
			break;
		}
		case 2: {
			cout << "输入修改的金币量: " << endl;
			int num = 0;
			cin >> num;
			ChangeMoney(num);
			break;
		}
		case 3: {
			DisableCoolDown();
			break;
		}
		case 4: {
			cout << "请输入你要种植的植物ID:";
			cin >> plantparam.p_id_;
			cout << "请输入你要种植的x坐标:";
			cin >> plantparam.p_x_;
			cout << "请输入你要种植的y坐标:";
			cin >> plantparam.p_y_;
			CallPlant(plantparam);
			break;
		}
		case 5: {
			cout << "请输入你要种植的植物ID:";
			cin >> plantparam.p_id_;
			for (size_t i = 0; i < 6; i++)
			{
				plantparam.p_y_ = i;
				for (size_t i = 0; i < 9; i++)
				{
					plantparam.p_x_ = i;
					CallPlant(plantparam);
				}
			}
			break;
		}
		case 6: {
			BackGroundJob();
			break;
		}

		case 7: {
			goto end;
		}

		}
	}

end:
	system("pause");
	Clean();
	return 0;
}

