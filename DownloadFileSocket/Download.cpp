#include "Download.h"
#include "Wininet.h"
#include <fstream>
#include <iterator>

stData Download::PushOneConnection(int totalconnect, int IDthread)
{
	stData datathread;
	datathread.numthread = IDthread;

	Wininet* connect = new Wininet();
	connect->Initialize(Port, Host, Request);
	
	string strBuffer;
	string strRange = GetRange(totalconnect, IDthread);
	connect->GetData(strRange, strBuffer);
	connect->Destroy();

	datathread.Data = strBuffer;
	delete connect;
	
	return datathread;
}

bool Download::AddDataInMapIntoFile()
{
	ofstream filedata;
	filedata.open(strOutput, ios::app | ios::binary | ios::out);
	map<int, string>::iterator itr;

	for (itr = mapData.begin(); itr != mapData.end();)
	{
		if (itr->first > inTotalConnectDataIntoFile)
			break;

		if (itr->first == inTotalConnectDataIntoFile)
		{
			filedata << itr->second;
			mapData.erase(itr++);
			inTotalConnectDataIntoFile++;
		}
		else
			++itr;
	}

	filedata.close();

	return true;
}

string Download::GetRange(int totalconnect, int IDthread)
{
	int inStart = (((int)douFilesizeSV - (int)douFilesizeLC) / totalconnect * IDthread) + IDthread;
	if (douFilesizeLC >= 0)
		inStart += (int)douFilesizeLC;
	int inEnd = inStart + ((int)douFilesizeSV - (int)douFilesizeLC) / totalconnect;
	string strStart = to_string(inStart);
	string strEnd = to_string(inEnd);
	if (totalconnect == IDthread + 1)
		strEnd = to_string((int)douFilesizeSV - 1);
	string strRange = "Range: bytes=" + strStart + "-" + strEnd;

	return strRange;
}

Download::Download():
	douFilesizeSV(0.0), douFilesizeLC(0.0), inTotalConnectDataIntoFile(0), inTotalThreadsRunning(0),
	strFilename(""), strOutput(""), Request(""), Host(""), Port(443)
{
}

Download::~Download()
{
}

int Download::SetNameFile(string url)
{
	size_t found = url.find_last_of("/");
	if (found == -1)
	{
		return 1;
	}
	strFilename = url.substr(found + 1);
	return 0;
}

int Download::SetRequest(string url)
{
	size_t found = url.find("https://");
	Port = 443;
	if (found == string::npos)
	{
		found = url.find("http://");
		Port = 80;
		if (found == string::npos)
			return 1;
		else
			found = sizeof("http://");
	}
	else
	{
		found = sizeof("https://");
	}

	string tempUrl = url.substr(found - 1);
	found = tempUrl.find_first_of("/");
	if (found == -1)
		return 1;

	string strHost = tempUrl.substr(0, found);
	bool checkempty = strHost.empty();
	if (checkempty)
		return 1;

	string strRequestLine = tempUrl.substr(found);
	checkempty = strRequestLine.empty();
	if (checkempty)
		return 1;

	Host = strHost;
	Request = strRequestLine;

	return 0;
}

int Download::Initialize(string url)
{
	int iResult = SetRequest(url);
	if (iResult != 0)
		return 1;

	iResult = SetNameFile(url);
	if (iResult != 0)
		return 1;

	return 0;
}

int Download::SetSizeFileServer()
{
	Wininet* connect = new Wininet();
	int iResult = connect->Initialize(Port, Host, Request);
	if (iResult != 0)
		return 1;
	
	double lenght = 0;
	iResult = connect->GetContentLength(lenght);
	if (iResult != 0)
		return 1;

	iResult = connect->Destroy();
	if (iResult != 0)
		return 1;

	douFilesizeSV = lenght;
	delete connect;

	return 0;
}

void Download::CheckSizeFileLocal(string link)
{
	SetLinkOutput(link);
	streampos begin, end;
	ifstream filelc(strOutput, ios::binary);
	begin = filelc.tellg();
	filelc.seekg(0, ios::end);
	end = filelc.tellg();
	filelc.close();
	douFilesizeLC = end - begin;
	if (douFilesizeLC >= douFilesizeSV)
	{
		remove(strOutput.c_str());
		douFilesizeLC = 0;
	}
	return;
}

void Download::StartDownload(int totalconnect, int totalthread)
{
	for (int i = 0; i < totalconnect; i++)
	{
		while (1)
		{
			if (!CheckThreadsRunning(totalthread))
			{
				CheckThreadsCompleted(totalconnect);
				break;
			}
		}

		listThreads.push_back(async(launch::async, &Download::PushOneConnection, this, totalconnect, i));
	}
	while (1)
	{
		if (!CheckThreadsCompleted(totalconnect))
			break;
	}
	return;
}

void Download::SetLinkOutput(string link)
{
	strOutput = link;
	return;
}

bool Download::CheckThreadsRunning(int totalthread)
{
	if (totalthread > inTotalThreadsRunning)
	{
		inTotalThreadsRunning++;
		return false;
	}
	else
	{
		list<future<stData>>::iterator itr;
		chrono::milliseconds span(0);
		for (itr = listThreads.begin(); itr != listThreads.end(); ++itr)
		{
			if (itr->wait_for(span) == future_status::ready)
				return false;
		}
	}
	return true;
}

bool Download::CheckThreadsCompleted(int totalconnect)
{
	list<future<stData>>::iterator itr;
	chrono::milliseconds span(0);
	for (itr = listThreads.begin(); itr != listThreads.end();)
	{
		int size = listThreads.size();
		if (size == 0)
			return true;

		if (itr->wait_for(span) == future_status::ready)
		{
			stData datathread = itr->get();
			mapData.insert(pair<int, string>(datathread.numthread, datathread.Data));
			listThreads.erase(itr++);
			AddDataInMapIntoFile();
		}
		else
			++itr;
	}
	if (inTotalConnectDataIntoFile == totalconnect)
		return false;

	return true;
}