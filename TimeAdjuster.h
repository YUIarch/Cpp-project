#ifndef TimeAdjuster_h
#define TimeAdjuster_h
#include <string>
#include <stdio.h>
#include <sstream>  
#include <locale>
#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include<fstream>
#include "Stock.h"
using namespace std;

string TimeAdjuster(vector<IndivStock>::iterator it, int N, string Time)
{
	string OutputTime;

	ifstream TC("TradingCalendar.csv");
	string line, Date;
	vector<string> DateList;
	while (getline(TC, line))
	{
		istringstream each(line);
		getline(each, Date, ',');
		DateList.push_back(Date);
	}

	if (Time == "startTime")
	{
		auto itr = find(DateList.begin(), DateList.end(), it->GetAncDate());
		if (itr != DateList.end()) { OutputTime = *(itr - N - 1); }
	}
	else if (Time == "endTime")
	{
		auto itr = find(DateList.begin(), DateList.end(), it->GetAncDate());
		if (itr != DateList.end()) { OutputTime = *(itr + N); }
	}
	return OutputTime;
}

#endif

