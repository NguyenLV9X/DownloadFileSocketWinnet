#include "DownloadManager.h"
#include "Download.h"

DownloadManager::DownloadManager() :
	inConnection_count(1), inThread_count(1), strUrl(""), strlinkoutput("")
{
}

DownloadManager::~DownloadManager()
{
}

void DownloadManager::SetValid(string url, string out, int connection_count, int thread_count)
{
	strUrl = url;
	size_t found = strUrl.find_last_of("/");
	string strFilename = strUrl.substr(found + 1);
	strlinkoutput = out + strFilename;
	inConnection_count = connection_count;
	inThread_count = thread_count;

    return;
}

void DownloadManager::AppMain()
{
	Download* manager = new Download();
	int iResult = manager->Initialize(strUrl);
	if (iResult != 0)
	{
		cout << "Error Init" << endl;
		return;
	}
	iResult = manager->SetSizeFileServer();
	if (iResult != 0)
	{
		cout << "Error Get File From Server" << endl;
	}
	manager->CheckSizeFileLocal(strlinkoutput);
	manager->StartDownload(inConnection_count, inThread_count);

	delete manager;
	return;
}
