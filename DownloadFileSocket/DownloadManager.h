#include <iostream>
using namespace std;

class DownloadManager
{
private:

	string strUrl;
	string strlinkoutput;
	int inConnection_count;
	int inThread_count;

public:

	void SetValid(string url, string out, int connection_count = 1,
		int thread_count = 1);
	void AppMain();
	DownloadManager();
	~DownloadManager();

};
