#ifndef Group_h
#define Group_h
#include <vector>
#include <map>
#include <string>
#include<ctime>
#include<cstdlib>
#include <algorithm>
#include <cmath>
#include "Stock.h"
#include "Operator.h"
using namespace std;

class Group
{
private:
	map<string, IndivStock> Map;
	Vector AAR30;//1*61
public:
	Vector AverageAAR;//1*61
	Vector AverageAARSTD;//1*61
	Vector AverageCAAR;//1*61
	Vector AverageCAARSTD;//1*61
	Group() {}; //default constructor 
	void GroupForm(vector<string>TickerList, vector<IndivStock>IndividualStock, int N); //constructor with parameters
	map<string, IndivStock> GetMap() { return Map; }
	Vector AAR30Stocks(); //Calculate AAR of 30 random stocks, can be reused
	Vector CAAR30Stocks(); //Calculate CAAR(This must be quote after AAR30Stocks)
};

void Group::GroupForm(vector<string>TickerList, vector<IndivStock>IndividualStock, int N)
{
	int D = TickerList.size();
	AverageAAR.resize(2 * N + 1);
	AverageAARSTD.resize(2 * N + 1);
	AverageCAAR.resize(2 * N + 1);
	AverageCAARSTD.resize(2 * N + 1);
	for (int i = 0; i < D; i++)
	{
		if (IndividualStock[i].Price.size() == 0 || IndividualStock[i].Price.size() != (2 * N + 2))
		{
			continue;
		}
		else
		{
			Map.insert(pair<string, IndivStock>(TickerList[i], IndividualStock[i]));
		}
	}
}

Vector Group::AAR30Stocks()
{
	srand(time(NULL));
	vector<string>Ticker;
	for (map<string, IndivStock>::iterator it = Map.begin(); it != Map.end(); it++)
	{
		Ticker.push_back(it->first);
	}
	random_shuffle(Ticker.begin(), Ticker.end());


	auto itr = Ticker.begin();
	//Vector AAR = (Map.at(*itr)).Return;
	Vector AAR = (Map.at(*itr)).AR;
	for (itr + 1; itr != Ticker.begin() + 30; itr++)
	{
		//AAR += ((Map.at(*itr)).Return);
		AAR += ((Map.at(*itr)).AR);
	}

	AAR30 = 1.0 / 30.0 * AAR;
	return AAR30;
}
Vector Group::CAAR30Stocks()
{
	Vector CAAR;
	double cumulative = 0.0;
	for (auto it = AAR30.begin(); it != AAR30.end(); it++)
	{
		cumulative += *it;
		CAAR.push_back(cumulative);
	}
	return CAAR;
}

void BootStrap30(Group& Type, int N)
{
	Vector AARsq;
	Vector CAARsq;
	AARsq.resize(2 * N + 1);
	CAARsq.resize(2 * N + 1);
	Vector AAR;
	AAR.resize(2 * N + 1);
	Vector CAAR;
	CAAR.resize(2 * N + 1);
	for (int i = 0; i < 30; i++)
	{
		Vector AAR = Type.AAR30Stocks();
		Type.AverageAAR += AAR;
		Vector CAAR = Type.CAAR30Stocks();
		Type.AverageCAAR += Type.CAAR30Stocks();
		AARsq = AARsq + AAR ^ 2.0;
		CAARsq = CAARsq + CAAR ^ 2.0;
		AAR.clear(); CAAR.clear();
	}

	AARsq = AARsq / 30;
	CAARsq = CAARsq / 30;
	Type.AverageAAR = (1.0 / 30.0) * Type.AverageAAR;
	Type.AverageCAAR = (1.0 / 30.0) * Type.AverageCAAR;
	Type.AverageAARSTD = (AARsq - Type.AverageAAR ^ 2) ^ 0.5;
	Type.AverageCAARSTD = (CAARsq - Type.AverageCAAR ^ 2) ^ 0.5;
}

#endif // !Group_h