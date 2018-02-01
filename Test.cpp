//****************************************************************************
// Function : BOOL SetBreakPoint(HANDLE hProcess, LPVOID lpAddr, UINT nNum)
// DESCRIPTION : Use for setting a break point.
// PARAMETERS : 
//               hProcess - The current process handle
//               lpAddr - The address of break point
//               nNum - the index of the break point
//*****************************************************************************
#include <WINDOWS.H>

typedef struct 
{
	LPVOID lpAddr;
	BYTE byData;
	unsigned int nCount;
}BPDATA;

BPDATA  g_bpData[10];

BOOL SetBreakPoint(HANDLE hProcess, LPVOID lpAddr, UINT nNum)
{
	if (nNum >= sizeof(g_bpData) / sizeof(BPDATA)) 
		return FALSE;
	BYTE byTemp;
	DWORD dwNewProt, dwOldProt;
	VirtualProtectEx(hProcess, lpAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProt);
	BOOL bOK = ReadProcessMemory(hProcess, lpAddr, &byTemp, 1, NULL);
	if (bOK)
	{
		g_bpData[nNum].lpAddr = lpAddr;          // ����ԭ��ַ�����ݣ�Ȼ���滻Ϊ�ϵ�ָ��int3�����������е���ʱ���ͻ�ִ�жϵ� 
		g_bpData[nNum].byData = byTemp;         // �������ִ�У���ô�ͽ�EIPָ���һ��Ȼ��ԭԭָ�����������ִ��
		g_bpData[nNum].nCount = 0;
		byTemp = 0xcc;  // int3 �ϵ�ָ��
		bOK = WriteProcessMemory(hProcess, lpAddr, &byTemp, 1, NULL);
	}
	VirtualProtectEx(hProcess, lpAddr, 1, dwOldProt, &dwNewProt);
	return bOK;
}
//****************************************************************************
// Function : BOOL RemoveBreakPoint(HANDLE hProcess, UINT nNum) 
// DESCRIPTION : Use for removing a break point.
// PARAMETERS : 
//               hProcess - The current process handle
//               nNum - the index of the break point
//*****************************************************************************
BOOL RemoveBreakPoint(HANDLE hProcess, UINT nNum)
{
	if (nNum >= sizeof(g_bpData) / sizeof(BPDATA)) return FALSE;
	BYTE byTemp;
	DWORD dwNewProt, dwOldProt;
	LPVOID lpAddr = g_bpData[nNum].lpAddr;
	VirtualProtectEx(hProcess, lpAddr, 1, PAGE_EXECUTE_READWRITE, &dwOldProt);
	BOOL bOK = ReadProcessMemory(hProcess, lpAddr, &byTemp, 1, NULL);
	bOK = (byTemp == 0xcc);
	if (bOK)
	{

		bOK = WriteProcessMemory(hProcess, lpAddr, &g_bpData[nNum].byData, 1, NULL);   // ��ԭԭָ��

		if (bOK) ZeroMemory(g_bpData + nNum, sizeof(BPDATA));   // ���
	}
	VirtualProtectEx(hProcess, lpAddr, 1, dwOldProt, &dwNewProt);
	return bOK;
}

int main(int agrc,char**agrv)
{
	DWORD dwPID = GetCurrentProcessId();
	HANDLE hProcess = ::GetCurrentProcess();
	DWORD dwAdd = 0x410000;
	unsigned int nBreakNum = 0;
	SetBreakPoint(hProcess,(void*)dwAdd,nBreakNum);
	RemoveBreakPoint(hProcess,nBreakNum);

	return 0;
}