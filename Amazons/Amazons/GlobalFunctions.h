#pragma once

CString GblGetDirectoryOfExecutableFile();
CString GblGetFileNameFromPathName(CString strPathName);
CString GblGetFileNameWithExFromPathName(CString strPathName);
long GblGetIPLong(LPCTSTR szIP);//返回的long就是addr的格式
CString GblGetIPString(long ip);//返回"127.0.0.1"这样的格式
int GetIntegerFromBuffer(LPCTSTR buffer, int *pStart);
int GetIntegerFromBuffer(LPCTSTR buffer, int default);//如果没有找到数字，返回default值
void GetStringFromBuffer(CString& strDes,LPCTSTR buffer, int& iStart);//从iPtr指定的位置开始读取一个"\r\n"结尾的字符串
void GetStringFromBuffer(LPTSTR bufferDes,LPCTSTR buffer, int& iStart);
int GblRevertByBit(int n);//反转一个int，比如10011，反转为11001
int GblRevertByByte(int n);//反转一个int，比如0xABCD，反转为0xCDAB
