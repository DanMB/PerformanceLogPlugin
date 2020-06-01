#pragma comment(lib, "BakkesMod.lib")

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "utils/parser.h"

class PerformanceLog : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	bool hasChanged = false;

	void onLoad();
	void onUnload();

	void getPerformance();

	string steamId;
	string data;
};
