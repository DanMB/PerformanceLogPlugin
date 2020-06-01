#include "PerformanceLog.h"

BAKKESMOD_PLUGIN(PerformanceLog, "Performance Log", "1.0", 0)

void PerformanceLog::onLoad()
{
	steamId = std::to_string(gameWrapper->GetSteamID());

	string tempData = "timestamp,"; // %Y-%m-%d %H:%M:%S timestamp
	tempData = tempData + "steam,"; // Steam ID of client
	tempData = tempData + "server,"; // Internal ID of server
	tempData = tempData + "playlist,"; // Current playlist ID
	tempData = tempData + "fps,"; // FPS
	tempData = tempData + "ping,"; // Ping
	tempData = tempData + "latency,"; // Latency
	tempData = tempData + "packets lost,"; // Lost packets out
	tempData = tempData + "buffer,"; // Buffer aka iBuf
	tempData = tempData + "buffer target\n"; // Buffer target aka iBuf target

	std::ofstream logfile;
	logfile.open("./bakkesmod/data/"+ steamId +"-performance.csv");
	logfile << tempData;
	logfile.close();

	this->getPerformance();
}

void PerformanceLog::onUnload()
{
}

void PerformanceLog::getPerformance()
{
	bool isInGame = false;

	if (gameWrapper->IsInOnlineGame()) {
		ServerWrapper server = gameWrapper->GetOnlineGame();
		if (!server.IsNull()) {
			if (server.GetbRoundActive()) {
				PlayerControllerWrapper localPrimaryPlayerController = server.GetLocalPrimaryPlayer();
				if (!localPrimaryPlayerController.IsNull()) {
					PriWrapper pri = localPrimaryPlayerController.GetPRI();

					if (!pri.IsNull()) {

						isInGame = true;

						time_t now = time(NULL);
						tm nowInfo;
						localtime_s(&nowInfo, &now);

						char timestamp[32];
						strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &nowInfo);


						StatGraphSystemWrapper statGraphs = gameWrapper->GetEngine().GetStatGraphs();
						NetStatGraphWrapper netGraph = statGraphs.GetNetStatGraph();
						InputBufferGraphWrapper bufGraph = statGraphs.GetInputBufferGraph();
						PerfStatGraphWrapper perfGraph = statGraphs.GetPerfStatGraph();

						RecordedSample latencySample = netGraph.GetLatency().GetPendingSample();
						RecordedSample lostOutSample = netGraph.GetLostPacketsOut().GetPendingSample();
						RecordedSample buffer = bufGraph.GetBuffer().GetPendingSample();
						RecordedSample bufTarget = bufGraph.GetBufferTarget().GetPendingSample();
						RecordedSample fpsSample = perfGraph.GetFPS().GetPendingSample();
						


						data = data + std::string(timestamp) + ","; // %Y-%m-%d %H:%M:%S timestamp
						data = data + steamId + ","; // Steam ID of client
						data = data + server.GetMatchGUID() + ","; // Internal ID of server
						data = data + std::to_string(server.GetPlaylist().GetPlaylistId()) + ","; // Current playlist ID
						data = data + std::to_string(static_cast<int>(((fpsSample.Low + fpsSample.High) / 2) + 0.5)) + ","; // FPS
						data = data + std::to_string(static_cast<int>((pri.GetExactPing() * 1000) + 0.5)) + ","; // Ping
						data = data + std::to_string(static_cast<int>(((latencySample.Low + latencySample.High) / 2) + 0.5)) + ","; // Latency
						data = data + std::to_string(static_cast<int>(lostOutSample.High + 0.5)) + ","; // Lost packets out
						data = data + std::to_string(static_cast<int>(buffer.Low + 0.5)) + ","; // Buffer aka iBuf
						data = data + std::to_string(static_cast<int>(bufTarget.Low + 0.5)) + "\n"; // Buffer target aka iBuf target

						hasChanged = true;

					}

				}
			}
		}
	}

	if (hasChanged && !isInGame) {
		hasChanged = false;
		string tempData = data;
		data = "";
		std::ofstream logfile;
		logfile.open("./bakkesmod/data/" + steamId + "-performance.csv", std::ios_base::app);
		logfile << tempData;
		logfile.close();
		cvarManager->log("Performance data logged");
	}

	gameWrapper->SetTimeout(bind(&PerformanceLog::getPerformance, this), 1.00f);
}
