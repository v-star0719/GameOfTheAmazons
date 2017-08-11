#include "stdafx.h"
#include "GlobalFunctions.h"

CString GblGetDirectoryOfExecutableFile()
{
	CString str('\0',255);
	::GetCurrentDirectory(255,str.GetBuffer());
	::GetModuleFileName(NULL,str.GetBuffer(),255);
	int i=0,j=0;
	while(j!=-1)
	{
		i=j;
		j=str.Find('\\',j+1);
	}
	i++;//保留最后一根'\'
	for(;str.GetAt(i)!='\0';i++) str.SetAt(i,'\0');
	str.Remove('\0');
	return str;
}

CString GblGetFileNameFromPathName(CString strPathName)
{
	int i = strPathName.ReverseFind(_T('\\'));
	int j = strPathName.ReverseFind(_T('.'));
	return strPathName.Mid(i + 1, j - i - 1);
}

CString GblGetFileNameWithExFromPathName(CString strPathName)
{
	int i = strPathName.ReverseFind(_T('\\'));
	return strPathName.Mid(i + 1);
}

int GetIntegerFromBuffer(LPCTSTR buffer, int *pStart)
{
	int nData=0;
	TCHAR c;

	//找到第一个数字
	while (1)
	{
		c = buffer[*pStart];
		if ((c >= '0' && c <= '9') && c != '\0')
			break;
		else
			(*pStart)++;
	}
	
	//解析数字
	while ((c >= '0' && c <= '9') && c != '\0')
	{
		nData*=10;
		nData+=c-'0';
		(*pStart)++;
		c = buffer[*pStart];
	}
	return nData;
}

int GetIntegerFromBuffer(LPCTSTR buffer, int default)
{
	int nData = 0;
	int i = 0;
	bool hasData = false;
	TCHAR c = buffer[i];
	while ((c >= '0' && c <= '9') && c != '\0')
	{
		nData *= 10;
		nData += c - '0';
		i++;
		c = buffer[i];
		hasData = true;
	}
	return hasData ? nData : default;
}

void GetStringFromBuffer(CString& strDes,LPCTSTR buffer, int& iStart)
{
	while((buffer[iStart]!='\r'||buffer[iStart+1]!='\n')&&buffer[iStart]!='\0')
	{//添加对'\0'的判断解决最后一行没有回车的问题
		strDes+=buffer[iStart];
		iStart++;
	}
}
void GetStringFromBuffer(LPTSTR bufferDes,LPCTSTR buffer, int& iStart)
{
	int i=0;
	while((buffer[iStart]!='\r'||buffer[iStart+1]!='\n')&&buffer[iStart]!='\0')
	{//添加对'\0'的判断解决最后一行没有回车的问题
		bufferDes[i]=buffer[iStart];
		i++;
		iStart++;
	}
}

int GblRevertByBit(int n)
{
	int rt = 0;
	while (n)
	{
		rt = rt << 1;
		rt |= n & 1;
		n = n >> 1;
	}
	return rt;
}

int GblRevertByByte(int n)
{
	int rt = 0;
	rt |= (n & 0xFF) << 24; n = n >> 8;
	rt |= (n & 0xFF) << 16; n = n >> 8;
	rt |= (n & 0xFF) << 8; n = n >> 8;
	rt |= n;
	return rt;
}

long GblGetIPLong(LPCTSTR szIP)
{
	int i = 0;
	int b1 = GetIntegerFromBuffer(szIP, &i);
	int b2 = GetIntegerFromBuffer(szIP, &i);
	int b3 = GetIntegerFromBuffer(szIP, &i);
	int b4 = GetIntegerFromBuffer(szIP, &i);

	return (b4 << 24) + (b3 << 16) + (b2 << 8) + b1;
}

CString GblGetIPString(long ip)
{
	CString str;
	str.Format(_T("%d.%d.%d.%d"),
		(ip & 0xff),
		(ip & 0xff00) >> 8,
		(ip & 0xff0000) >> 16,
		(ip & 0xff000000) >> 24
	);
	return str;
}