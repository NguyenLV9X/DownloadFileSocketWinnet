#include "Wininet.h"

int Wininet::Initialize(int port, string host, string request)
{
	hOpen = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (!hOpen)
		return 1;

	wstring wsHost = wstring(host.begin(), host.end());
	wstring wsRequest = wstring(request.begin(), request.end());

	if (port == INTERNET_DEFAULT_HTTPS_PORT)
	{
		hConnect = InternetConnect(hOpen, wsHost.c_str(), INTERNET_DEFAULT_HTTPS_PORT,
			TEXT(""), TEXT(""), INTERNET_SERVICE_HTTP, 0, 0);
		if (!hConnect)
			return 1;

		hReq = HttpOpenRequest(hConnect, TEXT("GET"), wsRequest.c_str(),
			HTTP_VERSION, TEXT(""), NULL, INTERNET_FLAG_SECURE, 0);
		if (!hReq)
			return 1;
	}
	else
	{
		hConnect = InternetConnect(hOpen, wsHost.c_str(), INTERNET_DEFAULT_HTTP_PORT,
			TEXT(""), TEXT(""), INTERNET_SERVICE_HTTP, 0, 0);
		if (!hConnect)
			return 1;

		hReq = HttpOpenRequest(hConnect, TEXT("GET"), wsRequest.c_str(),
			HTTP_VERSION, NULL, NULL, NULL, 0);
		if (!hReq)
			return 1;
	}

	return 0;
}

int Wininet::GetContentLength(double &fizesize)
{
	BOOL bStatus = HttpSendRequest(hReq, NULL, 0, NULL, 0);
	if (bStatus == FALSE)
		return 1;

	DWORD dwFileSize = 0;
	DWORD dwQueryBufLen = sizeof(dwFileSize);
	bStatus = HttpQueryInfo(hReq, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwFileSize, &dwQueryBufLen, NULL);
	if (bStatus == FALSE)
		return 1;

	fizesize = dwFileSize;
	return 0;
}

int Wininet::GetData(string range, string& data)
{
	string strRange = range + "\r\n\r\n";
	wstring wsRange = wstring(strRange.begin(), strRange.end());
	BOOL bStatus = HttpAddRequestHeaders(hReq, wsRange.c_str(), -1, HTTP_ADDREQ_FLAG_ADD);
	if (bStatus == FALSE)
		return 1;

	bStatus = HttpSendRequest(hReq, NULL, 0, NULL, 0);
	if (bStatus == FALSE)
		return 1;

	DWORD dwByteRead = 0;
	char szBuffer[1024] = "";
	do
	{
		bStatus = InternetReadFile(hReq, szBuffer, ARRAYSIZE(szBuffer), &dwByteRead);
		if (bStatus == FALSE)
			return 1;
		
		data.append(szBuffer, dwByteRead);
		ZeroMemory(szBuffer, dwByteRead);

	} while (dwByteRead);

	return 0;
}

int Wininet::Destroy()
{
	BOOL status = InternetCloseHandle(hReq);
	if (status == FALSE)
		return 1;

	status = InternetCloseHandle(hConnect);
	if (status == FALSE)
		return 1;

	status = InternetCloseHandle(hOpen);
	if (status == FALSE)
		return 1;

	return 0;
}

Wininet::Wininet():
	hOpen(NULL), hReq (NULL), hConnect(NULL)
{
}

Wininet::~Wininet()
{
}

