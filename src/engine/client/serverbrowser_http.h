#ifndef ENGINE_CLIENT_SERVERBROWSER_HTTP_H
#define ENGINE_CLIENT_SERVERBROWSER_HTTP_H
#include <base/system.h>

class CServerInfo;
class IConsole;
class IEngine;
class IStorage;

class IServerBrowserHttp
{
public:
	virtual ~IServerBrowserHttp() {}

	virtual void Update() = 0;

	virtual bool IsRefreshing() = 0;
	virtual void Refresh() = 0;

	virtual bool GetBestUrl(const char **pBestUrl) const = 0;

	virtual int NumServers() const = 0;
	virtual const CServerInfo &Server(int Index) const = 0;
	virtual int NumLegacyServers() const = 0;
	virtual const NETADDR &LegacyServer(int Index) const = 0;
};

IServerBrowserHttp *CreateServerBrowserHttp(IEngine *pEngine, IConsole *pConsole, IStorage *pStorage, const char *pPreviousBestUrl);
#endif // ENGINE_CLIENT_SERVERBROWSER_HTTP_H
