#ifndef ReadEPSCsv_h
#define ReadEPSCsv_h
#include <map>
#include <fstream>
#include <iostream>
#include <vector>
#include "Stock.h"
#include <sstream>
using namespace std;

void ReadCsv(string filename, string Group, vector<string>& TickerList, vector<IndivStock>& IndividualStock)
{
	ifstream EPS(filename);

	string line, Ticker, AnnouncementDate, Estimate, Reported, Surprise, SurprisePec;

	while (getline(EPS, line))
	{
		istringstream each(line);
		getline(each, Ticker, ',');
		getline(each, AnnouncementDate, ',');
		getline(each, Estimate, ',');
		getline(each, Reported, ',');
		getline(each, Surprise, ',');
		getline(each, SurprisePec, ',');

		TickerList.push_back(Ticker);
		IndivStock S = IndivStock(Ticker, AnnouncementDate, Group, strtod(Estimate.c_str(), NULL), strtod(Reported.c_str(), NULL), strtod(Surprise.c_str(), NULL), strtod(SurprisePec.c_str(), NULL));
		IndividualStock.push_back(S);
	}
}



#endif
