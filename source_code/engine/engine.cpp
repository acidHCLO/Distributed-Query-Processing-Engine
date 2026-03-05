#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

struct Stats {
	int minScore;
	int maxScore;
	int sum;
	int count;
};

map<int, Stats> processDataFile(string filename) {
	map<int, Stats> result;

	ifstream file(filename);
	string line;

	while (getline(file, line)) {
		stringstream ss(line);

		string id, year, score;

		getline(ss, id, ',');
		getline(ss, year, ',');
		getline(ss, score, ',');

		int yearInt = stoi(year);
		int scoreInt = stoi(score);

		if (result.find(yearInt) == result.end()) {
			result[yearInt] = { scoreInt, scoreInt, scoreInt, 1 };
		}
		else {
			result[yearInt].minScore = min(result[yearInt].minScore, scoreInt);
			result[yearInt].maxScore = max(result[yearInt].maxScore, scoreInt);
			result[yearInt].sum += scoreInt;
			result[yearInt].count++;
		}
	}

	return result;
}

int main(int argc, char* argv[]) {

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	int port = stoi(argv[1]);

	SOCKET server = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	bind(server, (sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(server, 5);

	cout << "Server is listening on port " << port << endl;

	int addrlen = sizeof(serverAddr);

	while (true) {
		SOCKET client = accept(server, (sockaddr*)&serverAddr, &addrlen);

		char buffer[1024] = { 0 };

		recv(client, buffer, 1024, 0);

		string filename = buffer;

		cout << "Received filename: " << filename << endl;

		map<int, Stats> results = processDataFile(filename);

		string response = "";

		for (auto& it : results) {
			response += to_string(it.first) + " ";
			response += to_string(it.second.minScore) + " ";
			response += to_string(it.second.maxScore) + " ";
			response += to_string(it.second.sum) + " ";
			response += to_string(it.second.count) + "\n";
		}

		send(client, response.c_str(), response.size(), 0);

		closesocket(client);
	}

	closesocket(server);

	WSACleanup();
}