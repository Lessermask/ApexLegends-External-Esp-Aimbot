#include "proc_help.h"
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <windows.h>
#include <random>
#include "imgui/imgui.h"
#include "hackerThreads.h"

ColorStruct colors;//物品颜色结构体
AppConfigs appConfigs;//菜单
int menu_zuobian;//左边
int menu_dingbian;//顶边
int menu_kuan;//宽
int menu_gao;//高
int menu_hangju;//行距
int menu_select;
int menu_size;//大小
int menu_fenge;//分隔
int nengnenglvse;
int nengnenglanse;
std::hash_map<int, ApexItem> entityNames;
HANDLE hAimThread;//瞄准线程句柄

int 寻血[3][16] = {};
int 直布[3][16] = {};
int 命脉[3][16] = {};
int 探路[3][16] = {};
int 动力[3][16] = {};

int 恶灵[3][16] = {};
int 班加[3][16] = {};
int 侵蚀[3][16] = {};
int 幻象[3][16] = {};
int 华森[3][16] = {};

typedef struct __KeyBoardHookStruct {
	int vk_Code;
	int scan_Code;
	int flags;
	int time;
	int dwExtraInfo;
} KeyBoardHookStruct;

using namespace std;

LRESULT CALLBACK keyboardHook(_In_  int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
DWORD WINAPI KeyBoardHookThread(LPVOID lpParam);

HHOOK keyHook = 0;
/*用户层面获取软件pid*/
DWORD GetProcessIDByName(const char* pName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot) {
		return 0;
	}
	PROCESSENTRY32 pe = { sizeof(pe) };
	for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe)) {
		_bstr_t info_p_Name(pe.szExeFile);
		if (strcmp(info_p_Name, pName) == 0) {
			CloseHandle(hSnapshot);
			return pe.th32ProcessID;
		}
		//printf("%-6d %s\n", pe.th32ProcessID, pe.szExeFile);
	}
	CloseHandle(hSnapshot);
	return 0;
}

void initColor() {
	colors = { ImColor({255, 255, 95}), ImColor({255, 0, 255}), ImColor({188, 102, 255}), ImColor({0, 191, 255}),
			  ImColor({255, 255, 255}), ImColor({197, 219, 70}), ImColor({111, 205, 168}), ImColor({106, 221, 34}),
			  ImColor({255, 188, 0}) };
}
void initConfig() {//初始化信息
	appConfigs = { true, false, 300.f, true,  false, 50.f, true, true, true, false, true, true, 150.f, 2, true };
	DWORD tid = 0;
	HANDLE tHandle = CreateThread(NULL, 0, KeyBoardHookThread, 0, 0, &tid);
	CloseHandle(tHandle);
}

DWORD WINAPI KeyBoardHookThread(LPVOID lpParam) {
	keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, 0, 0);
	if (keyHook == 0) {
		MessageBoxA(NULL, "安装键盘钩子失败", NULL, 0);
		return 0;
	}
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT) {
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}
		Sleep(1);
	}
	return 0;
}

LRESULT CALLBACK keyboardHook(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	if (nCode == 0 && wParam == 256) {
		KeyBoardHookStruct* kbh = (KeyBoardHookStruct*)lParam;
		switch (kbh->vk_Code) {
		case VK_F11:
			appConfigs.FangKuang = !appConfigs.FangKuang;
			break;
		case VK_F10:
			appConfigs.GUGETOUSHI = !appConfigs.GUGETOUSHI;
			break;
		case VK_F2:
			if (appConfigs.TouShiFanWei >= 1000) {
				appConfigs.TouShiFanWei = 100;
				break;
			}
			appConfigs.TouShiFanWei += 100;
			break;
		case VK_F3:
			appConfigs.WuPingTouShi = !appConfigs.WuPingTouShi;
			break;
		case VK_F4:
			if (appConfigs.WuPingFanWei >= 600) {
				appConfigs.WuPingFanWei = 0;
				break;
			}
			appConfigs.WuPingFanWei += 50;
			break;
		case VK_F5:
			appConfigs.PeiJianTouShi = !appConfigs.PeiJianTouShi;
			break;
		case VK_F6:
			appConfigs.ZiDongMiaoZhun = !appConfigs.ZiDongMiaoZhun;
			break;
		case VK_F7:
			appConfigs.XianShiZhaZhu = !appConfigs.XianShiZhaZhu;
			break;
		case VK_F12:
			appConfigs.KaiFaZheXuanXiang = !appConfigs.KaiFaZheXuanXiang;
			break;
		case VK_F9:
			appConfigs.ZIDONGGUAJI = !appConfigs.ZIDONGGUAJI;
			break;
		case VK_HOME:
			appConfigs.MenuStatus = !appConfigs.MenuStatus;
			break;
		}
	}
	return CallNextHookEx(keyHook, nCode, wParam, lParam);
}

void initMenu() {
	int ju = fontSize * 6;
	menu_zuobian = 10;
	menu_kuan = (int)(ju + fontSize * 2.65 + 10);
	menu_gao = fontSize * 13 + 7;
	menu_fenge = 2;
	menu_dingbian = 150;
	menu_hangju = 16;
	menu_select = 1;
	menu_size = 27;
	nengnenglvse = 10091863;
	nengnenglanse = 16088846;
}

void startThreads() {
	DWORD tid = 0;
	HANDLE tHandle = CreateThread(NULL, 0, InfoThread, 0, 0, &tid);
	CloseHandle(tHandle);
	tHandle = CreateThread(NULL, 0, EntityManager, 0, 0, &tid);
	CloseHandle(tHandle);
	//tHandle = CreateThread(NULL, 0, HentaiThread, 0, 0, &tid);
	//CloseHandle(tHandle);
	//tHandle = CreateThread(NULL, 0, ZiDongGuaJi, 0, 0, &tid);
	//CloseHandle(tHandle);
	hAimThread = CreateThread(NULL, 0, SuperAim, 0, 0, &tid);
}

void rand_str(char* str, const int len) {
	int i;
	for (i = 0; i < len; ++i) {
		switch ((getRandomInt(0, RAND_MAX) % 3)) {
		case 1:
			str[i] = 'A' + getRandomInt(0, RAND_MAX) % 26;
			break;
		case 2:
			str[i] = 'a' + getRandomInt(0, RAND_MAX) % 26;
			break;
		default:
			str[i] = '0' + getRandomInt(0, RAND_MAX) % 10;
			break;
		}
	}
	str[++i] = '\0';
}

void usleep(unsigned int usec) {
	HANDLE timer;
	LARGE_INTEGER ft;
	ft.QuadPart = -(10 * (__int64)usec);
	timer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
	WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);
}

int getRandomInt(int min, int max) {
	std::random_device rd;
	std::mt19937 mt(rd());
	return mt() % (max - min + 1) + min;
}

void Print_Memory(const unsigned char* start, unsigned int length) {
	//create row, col, and i.  Set i to 0
	int row, col, i = 0;
	//iterate through the rows, which will be 16 bytes of memory wide
	for (row = 0; (i + 1) < length; row++) {
		//print hex representation
		for (col = 0; col < 16; col++) {
			//calculate the current index
			i = row * 16 + col;
			//divides a row of 16 into two columns of 8
			if (col == 8) {
				printf(" ");
			}
			//print the hex value if the current index is in range.
			if (i < length) {
				printf("%02X", start[i]);
			}
			//print a blank if the current index is past the end
			else {
				printf("  ");
			}
			//print a space to keep the values separate
			printf(" ");
		}
		//create a vertial seperator between hex and ascii representations
		printf(" ");
		//print ascii representation
		for (col = 0; col < 16; col++) {
			//calculate the current index
			i = row * 16 + col;
			//divides a row of 16 into two coumns of 8
			if (col == 8) {
				printf("  ");
			}
			//print the value if it is in range
			if (i < length) {
				//print the ascii value if applicable
				if (start[i] > 0x20 && start[i] < 0x7F)  //A-Z
				{
					printf("%c", start[i]);
				}
				//print a period if the value is not printable
				else {
					printf(".");
				}
			}
			//nothing else to print, so break out of this for loop
			else {
				break;
			}
		}
		//create a new row
		printf("\n");
	}
}

void initEntityNames() {
	entityNames[1] = { u8"KRABER", colors.ShiShi };
	//entityNames[2] = { u8"MASTIFF", colors.ShiShi };
	//entityNames[3] = { u8"哈沃克", colors.ShiShi };
	entityNames[4] = { u8"L-STAR", colors.NengLiang };
	//entityNames[5] = { u8"哈沃克[金]", colors.ShiShi };
	//entityNames[6] = { u8"HAVOC", colors.NengLiang };
	//entityNames[7] = { u8"HAVOC", colors.NengLiang };
	entityNames[8] = { u8"DEVOTION", colors.NengLiang };
	//entityNames[9] = { u8"HAVOC", colors.NengLiang };
	//entityNames[10] = { u8"TRIPLE TAKE", colors.ZhongXing };
	//entityNames[12] = { u8"VK-47", colors.ZhongXing };
	//entityNames[13] = { u8"G7狙击枪", colors.QingXing };
	//entityNames[15] = { u8"转换者冲锋枪", colors.QingXing };
	//entityNames[16] = { u8"HEMLOK", colors.ZhongXing };
	entityNames[17] = { u8"R99冲锋枪", colors.QingXing };
	//entityNames[18] = { u8"猎兽冲锋枪", colors.ZhongXing };
	//entityNames[19] = { u8"潜袭冲锋枪[金]", colors.ShiShi };
	//entityNames[20] = { u8"G7", colors.ZhongXing };
	//entityNames[22] = { u8"ALTERANTOR", colors.ShiShi };
	entityNames[23] = { u8"R301", colors.QingXing };
	//entityNames[28] = { u8"莫桑比克", colors.SanDan };
	entityNames[26] = { u8"R-99", colors.ShiShi };
	//entityNames[25] = { u8"EVA-8", colors.SanDan };
	//entityNames[30] = { u8"PROWLER", colors.ZhongXing };
	//entityNames[31] = { u8"无敌左轮[金]", colors.ShiShi };
	entityNames[32] = { u8"VOLT SMG", colors.NengLiang };
	entityNames[33] = { u8"VOLT SMG", colors.QingXing };
	//entityNames[34] = { u8"LONG BOW", colors.QingXing };
	//entityNames[35] = { u8"LONG BOW", colors.QingXing };
	//entityNames[36] = { u8"CHARGE", colors.QingXing };

	//entityNames[37] = { u8"轻型弹药", colors.QingXing };
	entityNames[38] = { u8"M600", colors.ZhongXing };
	//entityNames[39] = { u8"霰弹枪弹药", colors.SanDan };
	entityNames[40] = { u8"R301", colors.QingXing };

	//entityNames[42] = { u8"凤凰", colors.ShengQi };
	//entityNames[43] = { u8"大急救箱", colors.ShengQi };
	//entityNames[44] = { u8"EVA-8", colors.ShengQi };
	//entityNames[45] = { u8"大护盾电池", colors.ShengQi };
	//entityNames[46] = { u8"小护盾电池", colors.ShengQi };

	//entityNames[47] = { u8"头 lv.1", colors.PuTong };
	//entityNames[48] = { u8"头 lv.2", colors.GaoJi };
	//entityNames[49] = { u8"PEACE KEEPER", colors.ShiShi };
	//entityNames[50] = { u8"MOZAMBIQUE", colors.ShiShi };//头 lv.4

	//entityNames[51] = { u8"甲 lv.1", colors.PuTong };
	//entityNames[52] = { u8"WINGMAN", colors.GaoJi };
	//entityNames[53] = { u8"甲 lv.3", colors.ShiShi };
	//entityNames[54] = { u8"甲 lv.4", colors.ShiShi };

	//entityNames[55] = { u8"盾 lv.1", colors.PuTong };
	//entityNames[56] = { u8"P2020", colors.QingXing };
	//entityNames[57] = { u8"盾 lv.3", colors.ShengQi };
	//entityNames[58] = { u8"RE-45", colors.ShiShi };

	//entityNames[59] = { u8"包 lv.1", colors.PuTong };
	//entityNames[60] = { u8"SENTINEL", colors.GaoJi };
	//entityNames[61] = { u8"包 lv.3", colors.ShiShi };
	//entityNames[62] = { u8"LIGHT", colors.ShiShi };

	//entityNames[63] = { u8"ENERGY", colors.NengLiang };
	/*entityNames[64] = { u8"SHOTGUN", colors.GaoJi };
	entityNames[65] = { u8"HEAVY", colors.GaoJi };
	entityNames[66] = { u8"SNIPER", colors.GaoJi };*/

	//entityNames[67] = { u8"ULTIMATE ACCELERANT", colors.GaoJi };
	//entityNames[68] = { u8"PHOENIX KIT", colors.GaoJi };
	//entityNames[69] = { u8"MED KIT", colors.ShengQi };
	//entityNames[70] = { u8"SYRINGR", colors.ShengQi };
	//entityNames[71] = { u8"SHILED BATTERY", colors.GaoJi };
	//entityNames[72] = { u8"SHILED CELL", colors.GaoJi };
	//entityNames[73] = { u8"HELMET LV.1", colors.ShengQi };
	//entityNames[74] = { u8"HELMET LV.2", colors.ShengQi };
	//entityNames[75] = { u8"HELMET LV.3", colors.ShengQi };
	entityNames[76] = { u8"HELMET LV.4", colors.ShiShi };
	/*entityNames[77] = { u8"SHILED LV.1", colors.GaoJi };
	entityNames[78] = { u8"SHILED LV.2", colors.ShengQi };
	entityNames[79] = { u8"SHILED LV.3", colors.ShengQi };*/
	//entityNames[80] = { u8"SHILED LV.4", colors.PuTong };
	//entityNames[81] = { u8"SHILED LV.4", colors.GaoJi };
	/*entityNames[82] = { u8"EVO SHILED LV.1", colors.ShiShi };
	entityNames[83] = { u8"EVO SHILED LV.2", colors.PuTong };
	entityNames[84] = { u8"EVO SHILED LV.3", colors.GaoJi };*/
	//entityNames[85] = { u8"EVO SHILED LV.4", colors.ShiShi };

	//entityNames[86] = { u8"KNOWDOWN S lv.1", colors.PuTong }; //能量扩容Lv.1
	//entityNames[87] = { u8"KNOWDOWN S lv.2", colors.GaoJi };
	//entityNames[88] = { u8"KNOWDOWN S lv.3", colors.ShengQi };
	//entityNames[89] = { u8"KNOWDOWN S lv.4", colors.PuTong };//霰弹枪头Lv.1

	//entityNames[90] = { u8"BACKPACK lv.1", colors.GaoJi };//霰弹枪头Lv.2
	//entityNames[91] = { u8"BACKPACK lv.2", colors.ShengQi };//霰弹枪头Lv.3
	//entityNames[92] = { u8"BACKPACK lv.3", colors.ShengQi };//枪屁股Lv.1
	entityNames[93] = { u8"BACKPACK lv.4", colors.ShiShi };//枪屁股Lv.2

	/*entityNames[94] = { u8"THERMITE GRENADE", colors.ShengQi };
	entityNames[95] = { u8"FRAG GRENADE", colors.PuTong };
	entityNames[96] = { u8"ARC STAR", colors.GaoJi };*/

	/*entityNames[97] = { u8"1X", colors.ShengQi };
	entityNames[98] = { u8"2X", colors.ShengQi };
	entityNames[99] = { u8"1X HOLO", colors.ShengQi };
	entityNames[100] = { u8"1-2X", colors.ShengQi };
	entityNames[101] = { u8"1X-DIGITAL", colors.ShengQi };
	entityNames[102] = { u8"3X", colors.ShengQi };
	entityNames[103] = { u8"2-4X", colors.ShengQi };

	entityNames[104] = { u8"1X", colors.ShengQi };
	entityNames[105] = { u8"2X", colors.ShengQi };
	entityNames[106] = { u8"1X HOLO", colors.ShengQi };
	entityNames[107] = { u8"1-2X", colors.ShengQi };
	entityNames[108] = { u8"1X-DIGITAL", colors.ShengQi };
	entityNames[109] = { u8"3X", colors.ShengQi };*/
	entityNames[110] = { u8"BARREL STABLE LV.4", colors.ShengQi };

	//entityNames[111] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	//entityNames[112] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	//entityNames[113] = { u8"LIGHT MAG LV.3", colors.ShengQi };

	/*entityNames[114] = { u8"HEAVY MAG LV.1", colors.ShengQi };
	entityNames[115] = { u8"HEAVY MAG LV.2", colors.ShengQi };
	entityNames[116] = { u8"HEAVY MAG LV.3", colors.ShengQi };

	entityNames[117] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[118] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[119] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[120] = { u8"BARREL STABLE LV.4", colors.ShengQi };

	entityNames[121] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[122] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[123] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[124] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[125] = { u8"BARREL STABLE LV.4", colors.ShengQi };*/

	//entityNames[126] = { u8"STANDER STOCK LV.1", colors.ShengQi };
	//entityNames[127] = { u8"STANDER STOCK LV.2", colors.ShengQi };
	entityNames[128] = { u8"STANDER STOCK LV.3", colors.ShengQi };

	/*entityNames[129] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[131] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[132] = { u8"BARREL STABLE LV.4", colors.ShengQi };

	entityNames[133] = { u8"SELECTFIRE", colors.ShengQi };*/
	entityNames[134] = { u8"SKULLPIERCER", colors.ShengQi };

	entityNames[135] = { u8"HAMMERPOINT", colors.ShengQi };
	/*entityNames[136] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[137] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[138] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[139] = { u8"BARREL STABLE LV.4", colors.ShengQi };
	entityNames[140] = { u8"BARREL STABLE LV.4", colors.ShengQi };*/
}

void initBones() {
	//寻血
	{
		寻血[0][0] = Bones::head;
		寻血[0][1] = Bones::neck;
		寻血[0][2] = Bones::chest;
		寻血[0][3] = Bones::abdomen;
		寻血[0][4] = Bones::屌;

		寻血[1][0] = Bones::寻血左肩 + 7;
		寻血[1][1] = Bones::寻血左肩 + 5;
		寻血[1][2] = Bones::寻血左肩 + 4;
		寻血[1][3] = Bones::寻血左肩 + 2;
		寻血[1][4] = Bones::寻血左肩 + 3;
		寻血[1][5] = Bones::寻血左肩 + 1;
		寻血[1][6] = Bones::寻血左肩;
		寻血[1][7] = Bones::寻血左肩 - 2;

		寻血[1][8] = Bones::寻血右肩 - 2;
		寻血[1][9] = Bones::寻血右肩;
		寻血[1][10] = Bones::寻血右肩 + 1;
		寻血[1][11] = Bones::寻血右肩 + 3;
		寻血[1][12] = Bones::寻血右肩 + 2;
		寻血[1][13] = Bones::寻血右肩 + 4;
		寻血[1][14] = Bones::寻血右肩 + 5;
		寻血[1][15] = Bones::寻血右肩 + 7;

		寻血[2][0] = Bones::寻血左膝 + 2;
		寻血[2][1] = Bones::寻血左膝 + 1;
		寻血[2][2] = Bones::寻血左膝;
		寻血[2][3] = Bones::屌;
		寻血[2][4] = Bones::寻血右膝;
		寻血[2][5] = Bones::寻血右膝 + 1;
		寻血[2][6] = Bones::寻血右膝 + 2;
	}
	//直布
	{
		直布[0][0] = Bones::head;
		直布[0][1] = Bones::neck;
		直布[0][2] = Bones::chest;
		直布[0][3] = Bones::abdomen;
		直布[0][4] = Bones::屌;

		直布[1][0] = Bones::直布左肩 + 7;
		直布[1][1] = Bones::直布左肩 + 5;
		直布[1][2] = Bones::直布左肩 + 4;
		直布[1][3] = Bones::直布左肩 + 2;
		直布[1][4] = Bones::直布左肩 + 3;
		直布[1][5] = Bones::直布左肩 + 1;
		直布[1][6] = Bones::直布左肩;
		直布[1][7] = Bones::直布左肩 - 2;

		直布[1][8] = Bones::直布右肩 - 2;
		直布[1][9] = Bones::直布右肩;
		直布[1][10] = Bones::直布右肩 + 1;
		直布[1][11] = Bones::直布右肩 + 3;
		直布[1][12] = Bones::直布右肩 + 2;
		直布[1][13] = Bones::直布右肩 + 4;
		直布[1][14] = Bones::直布右肩 + 5;
		直布[1][15] = Bones::直布右肩 + 7;

		直布[2][0] = Bones::直布左膝 + 2;
		直布[2][1] = Bones::直布左膝 + 1;
		直布[2][2] = Bones::直布左膝;
		直布[2][3] = Bones::屌;
		直布[2][4] = Bones::直布右膝;
		直布[2][5] = Bones::直布右膝 + 1;
		直布[2][6] = Bones::直布右膝 + 2;
	}
	//命脉
	{
		命脉[0][0] = Bones::head;
		命脉[0][1] = Bones::neck;
		命脉[0][2] = Bones::chest;
		命脉[0][3] = Bones::abdomen;
		命脉[0][4] = Bones::屌;

		命脉[1][0] = Bones::命脉左肩 + 7;
		命脉[1][1] = Bones::命脉左肩 + 5;
		命脉[1][2] = Bones::命脉左肩 + 4;
		命脉[1][3] = Bones::命脉左肩 + 2;
		命脉[1][4] = Bones::命脉左肩 + 3;
		命脉[1][5] = Bones::命脉左肩 + 1;
		命脉[1][6] = Bones::命脉左肩;
		命脉[1][7] = Bones::命脉左肩 - 2;

		命脉[1][8] = Bones::命脉右肩 - 2;
		命脉[1][9] = Bones::命脉右肩;
		命脉[1][10] = Bones::命脉右肩 + 1;
		命脉[1][11] = Bones::命脉右肩 + 3;
		命脉[1][12] = Bones::命脉右肩 + 2;
		命脉[1][13] = Bones::命脉右肩 + 4;
		命脉[1][14] = Bones::命脉右肩 + 5;
		命脉[1][15] = Bones::命脉右肩 + 7;

		命脉[2][0] = Bones::命脉左膝 + 2;
		命脉[2][1] = Bones::命脉左膝 + 1;
		命脉[2][2] = Bones::命脉左膝;
		命脉[2][3] = Bones::屌;
		命脉[2][4] = Bones::命脉右膝;
		命脉[2][5] = Bones::命脉右膝 + 1;
		命脉[2][6] = Bones::命脉右膝 + 2;
	}
	//探路
	{
		探路[0][0] = Bones::head;
		探路[0][1] = Bones::neck;
		探路[0][2] = Bones::chest;
		探路[0][3] = Bones::abdomen;
		探路[0][4] = Bones::屌;

		探路[1][0] = Bones::探路左肩 + 6;
		探路[1][1] = Bones::探路左肩 + 5;
		探路[1][2] = Bones::探路左肩 + 4;
		探路[1][3] = Bones::探路左肩 + 3;
		探路[1][4] = Bones::探路左肩 + 2;
		探路[1][5] = Bones::探路左肩 + 1;
		探路[1][6] = Bones::探路左肩;
		探路[1][7] = Bones::探路左肩 - 1;

		探路[1][8] = Bones::探路右肩 - 1;
		探路[1][9] = Bones::探路右肩;
		探路[1][10] = Bones::探路右肩 + 1;
		探路[1][11] = Bones::探路右肩 + 2;
		探路[1][12] = Bones::探路右肩 + 3;
		探路[1][13] = Bones::探路右肩 + 4;
		探路[1][14] = Bones::探路右肩 + 5;
		探路[1][15] = Bones::探路右肩 + 6;

		探路[2][0] = Bones::探路左膝 + 2;
		探路[2][1] = Bones::探路左膝 + 1;
		探路[2][2] = Bones::探路左膝;
		探路[2][3] = Bones::屌;
		探路[2][4] = Bones::探路右膝;
		探路[2][5] = Bones::探路右膝 + 1;
		探路[2][6] = Bones::探路右膝 + 2;
	}
	//动力
	{
		动力[0][0] = Bones::head;
		动力[0][1] = Bones::neck;
		动力[0][2] = Bones::chest;
		动力[0][3] = Bones::abdomen;
		动力[0][4] = Bones::屌;

		动力[1][0] = Bones::动力左肩 + 7;
		动力[1][1] = Bones::动力左肩 + 5;
		动力[1][2] = Bones::动力左肩 + 4;
		动力[1][3] = Bones::动力左肩 + 2;
		动力[1][4] = Bones::动力左肩 + 3;
		动力[1][5] = Bones::动力左肩 + 1;
		动力[1][6] = Bones::动力左肩;
		动力[1][7] = Bones::动力左肩 - 2;

		动力[1][8] = Bones::动力右肩 - 2;
		动力[1][9] = Bones::动力右肩;
		动力[1][10] = Bones::动力右肩 + 1;
		动力[1][11] = Bones::动力右肩 + 3;
		动力[1][12] = Bones::动力右肩 + 2;
		动力[1][13] = Bones::动力右肩 + 4;
		动力[1][14] = Bones::动力右肩 + 5;
		动力[1][15] = Bones::动力右肩 + 7;

		动力[2][0] = Bones::动力左膝 - 3;
		动力[2][1] = Bones::动力左膝 - 2;
		动力[2][2] = Bones::动力左膝;
		动力[2][3] = Bones::屌;
		动力[2][4] = Bones::动力右膝;
		动力[2][5] = Bones::动力右膝 + 1;
		动力[2][6] = Bones::动力右膝 + 2;
	}
	//恶灵
	{
		恶灵[0][0] = Bones::head;
		恶灵[0][1] = Bones::neck;
		恶灵[0][2] = Bones::chest;
		恶灵[0][3] = Bones::abdomen;
		恶灵[0][4] = Bones::屌;

		恶灵[1][0] = Bones::恶灵左肩 + 7;
		恶灵[1][1] = Bones::恶灵左肩 + 5;
		恶灵[1][2] = Bones::恶灵左肩 + 4;
		恶灵[1][3] = Bones::恶灵左肩 + 2;
		恶灵[1][4] = Bones::恶灵左肩 + 3;
		恶灵[1][5] = Bones::恶灵左肩 + 1;
		恶灵[1][6] = Bones::恶灵左肩;
		恶灵[1][7] = Bones::恶灵左肩 - 2;

		恶灵[1][8] = Bones::恶灵右肩 - 2;
		恶灵[1][9] = Bones::恶灵右肩;
		恶灵[1][10] = Bones::恶灵右肩 + 1;
		恶灵[1][11] = Bones::恶灵右肩 + 3;
		恶灵[1][12] = Bones::恶灵右肩 + 2;
		恶灵[1][13] = Bones::恶灵右肩 + 4;
		恶灵[1][14] = Bones::恶灵右肩 + 5;
		恶灵[1][15] = Bones::恶灵右肩 + 7;

		恶灵[2][0] = Bones::恶灵左膝 + 2;
		恶灵[2][1] = Bones::恶灵左膝 + 1;
		恶灵[2][2] = Bones::恶灵左膝;
		恶灵[2][3] = Bones::屌;
		恶灵[2][4] = Bones::恶灵右膝;
		恶灵[2][5] = Bones::恶灵右膝 + 1;
		恶灵[2][6] = Bones::恶灵右膝 + 2;
	}
	//班加
	{
		班加[0][0] = Bones::head;
		班加[0][1] = Bones::neck;
		班加[0][2] = Bones::chest;
		班加[0][3] = Bones::abdomen;
		班加[0][4] = Bones::屌;

		班加[1][0] = Bones::班加左肩 + 7;
		班加[1][1] = Bones::班加左肩 + 5;
		班加[1][2] = Bones::班加左肩 + 4;
		班加[1][3] = Bones::班加左肩 + 2;
		班加[1][4] = Bones::班加左肩 + 3;
		班加[1][5] = Bones::班加左肩 + 1;
		班加[1][6] = Bones::班加左肩;
		班加[1][7] = Bones::班加左肩 - 2;

		班加[1][8] = Bones::班加右肩 - 2;
		班加[1][9] = Bones::班加右肩;
		班加[1][10] = Bones::班加右肩 + 1;
		班加[1][11] = Bones::班加右肩 + 3;
		班加[1][12] = Bones::班加右肩 + 2;
		班加[1][13] = Bones::班加右肩 + 4;
		班加[1][14] = Bones::班加右肩 + 5;
		班加[1][15] = Bones::班加右肩 + 7;

		班加[2][0] = Bones::班加左膝 + 2;
		班加[2][1] = Bones::班加左膝 + 1;
		班加[2][2] = Bones::班加左膝;
		班加[2][3] = Bones::屌;
		班加[2][4] = Bones::班加右膝;
		班加[2][5] = Bones::班加右膝 + 1;
		班加[2][6] = Bones::班加右膝 + 2;
	}
	//侵蚀
	{
		侵蚀[0][0] = Bones::head;
		侵蚀[0][1] = Bones::neck;
		侵蚀[0][2] = Bones::chest;
		侵蚀[0][3] = Bones::abdomen;
		侵蚀[0][4] = Bones::屌;

		侵蚀[1][0] = Bones::侵蚀左肩 + 7;
		侵蚀[1][1] = Bones::侵蚀左肩 + 5;
		侵蚀[1][2] = Bones::侵蚀左肩 + 4;
		侵蚀[1][3] = Bones::侵蚀左肩 + 2;
		侵蚀[1][4] = Bones::侵蚀左肩 + 3;
		侵蚀[1][5] = Bones::侵蚀左肩 + 1;
		侵蚀[1][6] = Bones::侵蚀左肩;
		侵蚀[1][7] = Bones::侵蚀左肩 - 2;

		侵蚀[1][8] = Bones::侵蚀右肩 - 2;
		侵蚀[1][9] = Bones::侵蚀右肩;
		侵蚀[1][10] = Bones::侵蚀右肩 + 1;
		侵蚀[1][11] = Bones::侵蚀右肩 + 3;
		侵蚀[1][12] = Bones::侵蚀右肩 + 2;
		侵蚀[1][13] = Bones::侵蚀右肩 + 4;
		侵蚀[1][14] = Bones::侵蚀右肩 + 5;
		侵蚀[1][15] = Bones::侵蚀右肩 + 7;

		侵蚀[2][0] = Bones::侵蚀左膝 + 2;
		侵蚀[2][1] = Bones::侵蚀左膝 + 1;
		侵蚀[2][2] = Bones::侵蚀左膝;
		侵蚀[2][3] = Bones::屌;
		侵蚀[2][4] = Bones::侵蚀右膝;
		侵蚀[2][5] = Bones::侵蚀右膝 + 1;
		侵蚀[2][6] = Bones::侵蚀右膝 + 2;
	}
	//幻象
	{
		幻象[0][0] = Bones::head;
		幻象[0][1] = Bones::neck;
		幻象[0][2] = Bones::chest;
		幻象[0][3] = Bones::abdomen;
		幻象[0][4] = Bones::屌;

		幻象[1][0] = Bones::幻象左肩 + 7;
		幻象[1][1] = Bones::幻象左肩 + 5;
		幻象[1][2] = Bones::幻象左肩 + 4;
		幻象[1][3] = Bones::幻象左肩 + 2;
		幻象[1][4] = Bones::幻象左肩 + 3;
		幻象[1][5] = Bones::幻象左肩 + 1;
		幻象[1][6] = Bones::幻象左肩;
		幻象[1][7] = Bones::幻象左肩 - 1;

		幻象[1][8] = Bones::幻象右肩 - 1;
		幻象[1][9] = Bones::幻象右肩;
		幻象[1][10] = Bones::幻象右肩 + 1;
		幻象[1][11] = Bones::幻象右肩 + 3;
		幻象[1][12] = Bones::幻象右肩 + 2;
		幻象[1][13] = Bones::幻象右肩 + 4;
		幻象[1][14] = Bones::幻象右肩 + 5;
		幻象[1][15] = Bones::幻象右肩 + 7;

		幻象[2][0] = Bones::幻象左膝 + 2;
		幻象[2][1] = Bones::幻象左膝 + 1;
		幻象[2][2] = Bones::幻象左膝;
		幻象[2][3] = Bones::屌;
		幻象[2][4] = Bones::幻象右膝;
		幻象[2][5] = Bones::幻象右膝 + 1;
		幻象[2][6] = Bones::幻象右膝 + 2;
	}
	//华森
	{
		华森[0][0] = Bones::head;
		华森[0][1] = Bones::neck;
		华森[0][2] = Bones::chest;
		华森[0][3] = Bones::abdomen;
		华森[0][4] = Bones::屌;

		华森[1][0] = Bones::华森左肩 + 7;
		华森[1][1] = Bones::华森左肩 + 5;
		华森[1][2] = Bones::华森左肩 + 4;
		华森[1][3] = Bones::华森左肩 + 2;
		华森[1][4] = Bones::华森左肩 + 3;
		华森[1][5] = Bones::华森左肩 + 1;
		华森[1][6] = Bones::华森左肩;
		华森[1][7] = Bones::华森左肩 - 2;

		华森[1][8] = Bones::华森右肩 - 2;
		华森[1][9] = Bones::华森右肩;
		华森[1][10] = Bones::华森右肩 + 1;
		华森[1][11] = Bones::华森右肩 + 3;
		华森[1][12] = Bones::华森右肩 + 2;
		华森[1][13] = Bones::华森右肩 + 4;
		华森[1][14] = Bones::华森右肩 + 5;
		华森[1][15] = Bones::华森右肩 + 7;

		华森[2][0] = Bones::华森左膝 + 2;
		华森[2][1] = Bones::华森左膝 + 1;
		华森[2][2] = Bones::华森左膝;
		华森[2][3] = Bones::屌;
		华森[2][4] = Bones::华森右膝;
		华森[2][5] = Bones::华森右膝 + 1;
		华森[2][6] = Bones::华森右膝 + 2;
	}
}