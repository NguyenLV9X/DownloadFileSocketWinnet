#include <iostream>
#include <windows.h>
#include <WinInet.h>
using namespace std;
#pragma comment(lib,"WinInet.lib")

class Wininet
{
private:

	HINTERNET hOpen;
	HINTERNET hConnect;
	HINTERNET hReq;

public:

	int Initialize(int port, string host, string request);
	int GetContentLength(double &fizesize);
	int GetData(string range, string& data);
	int Destroy();
	Wininet();
	~Wininet();

};