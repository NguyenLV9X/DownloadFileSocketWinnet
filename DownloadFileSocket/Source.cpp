#include "DownloadManager.h"

int main()
{
	string url;
	int connect_count;
	int thread_count;
	string out;
	DownloadManager tool;

	url = "https://github.com/notepad-plus-plus/notepad-plus-plus/releases/download/v7.8.8/npp.7.8.8.Installer.exe";
	cout << "getfile "<< url << endl;

	connect_count = 23;
	cout << "--connection-count=" << connect_count << endl;;
	
	thread_count = 5;
	cout << "--thread-count="<< thread_count << endl;
	
	out = "C:\\Users\\Admin\\Downloads\\";
	cout << "--output=" << out << endl;
	
	tool.SetValid(url, out, connect_count, thread_count);
	tool.AppMain();
	
	return 0;
}