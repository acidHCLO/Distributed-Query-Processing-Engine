#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
namespace fs = filesystem;

// How Driver works:
// Scans dataDir(dataset), distributes files to engines,
// collects stats, merges results,
// writes final output to results.txt

struct Stats {
	int minScore;
	int maxScore;
	int sum;
	int count;
};

map<int, Stats> callEngine(string filename, int port) {
	map<int, Stats> result;

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET client = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);

	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr));

	send(client, filename.c_str(), filename.size(), 0);

	char buffer[4096] = { 0 };

	recv(client, buffer, 4096, 0);

	string response = buffer;

	cout << "Received from engine on port " << port << ": " << response << endl;

	stringstream ss(response);

	int year, minScore, maxScore, sum, count;

	while(ss >> year >> minScore >> maxScore >> sum >> count) {
		result[year] = { minScore, maxScore, sum, count };
	}

	closesocket(client);
	WSACleanup();

	return result;
}

int main(int argc, char* argv[]) {
	vector<int> ports;

	for (int i = 1; i < argc; i++) {
		ports.push_back(stoi(argv[i]));
	}

	vector<string> files;

	string dataDir = "sample_dataset/student_scores";

	for (auto& p : fs::directory_iterator(dataDir)) {
		if (p.path().extension() == ".csv" && p.path().filename() != "schema.csv") {
			files.push_back(p.path().string());
		}
	}

	map<int, Stats> global;
	int engineIdx = 0;

	for (auto& file : files) {
		int port = ports[engineIdx % ports.size()];

		cout << "Processing file: " << file << " using engine on port " << port << endl;

		auto result = callEngine(file, port);

		for (auto& [year, stats] : result) {
			if (global.find(year) == global.end()) {
				global[year] = stats;
			}
			else {
				global[year].minScore = min(global[year].minScore, stats.minScore);
				global[year].maxScore = max(global[year].maxScore, stats.maxScore);
				global[year].sum += stats.sum;
				global[year].count += stats.count;
			}
		}
		engineIdx++;
	}

	ofstream outFile("results.txt");

	for (auto& it : global) {
		int year = it.first;
		Stats stats = it.second;

		int avgScore = stats.count > 0 ? stats.sum / stats.count : 0;
		outFile << year << "," << stats.minScore << "," << stats.maxScore << "," << avgScore << endl;
	}

	cout << "Results written to results.txt" << endl;
}