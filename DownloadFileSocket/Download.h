#include <map>
#include <string>
#include <list>
#include <future>
using namespace std;

struct stData
{
	string Data;
	int numthread;
};

class Download
{
private:

	list<future<stData>> listThreads;
	map<int, string> mapData;
	double douFilesizeSV;
	double douFilesizeLC;
	string strFilename;
	string strOutput;
	int inTotalConnectDataIntoFile;
	int inTotalThreadsRunning;
	string Request;
	string Host;
	int Port;

	int SetNameFile(string url);
	int SetRequest(string url);
	stData PushOneConnection(int totalconnect, int IDthread);
	string GetRange(int totalconnect, int IDthread);
	bool AddDataInMapIntoFile();
	void SetLinkOutput(string link);
	bool CheckThreadsCompleted(int totalconnect);
	bool CheckThreadsRunning(int totalthread);

public:

	Download();
	~Download();	
	int Initialize(string url);
	int SetSizeFileServer();
	void CheckSizeFileLocal(string link);
	void StartDownload(int totalconnect, int totalthread);

};
