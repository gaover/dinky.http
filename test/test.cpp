#include "common.h"

#include "WebApp.h"

using namespace small_http;

WebApp app(":34568");

std::atomic_int g_iNum(0);

void printf__(string& a, string& b)
{
	b += " client ";
	cout << a << b << "\n";
	--g_iNum;
}

bool HelloWorld(CLIENTID id, const char* req, string& rsp)
{
	rsp = "HelloWorld!";
	return true;
}

bool Echo(CLIENTID id, const char* req, string& rsp)
{
	rsp = req;
	return true;
}


bool RootPath(CLIENTID id, const char* req, string& rsp)
{
	rsp = "Root Is Null!";
	return true;
}

int main()
{
	app.Handle("/hello", &HelloWorld);
	app.Handle("/echo", &Echo);
	app.Handle("/", &RootPath);
	app.StartApp();

	app.ASyncCallback();


	std::string tmpStr;
	cin >> tmpStr;

	cout << "g_iNum" << g_iNum << endl;

	app.Stop();
	return 0;

}