#ifndef Stock_h
#define Stock_h
#include <string> 
#include <vector>
using namespace std;

class Stock
{
public:
	vector<string> Date;
	vector<double> Price;
	vector<double> Return;
	string Ticker;
	void ExpandDate(string Date_) { Date.push_back(Date_); }
	void ExpandPrice(double Price_) { Price.push_back(Price_); }

	void CalR()
	{
		double R = 0.0;
		auto it = Price.begin();
		for (it; it != Price.end() - 1; it++)
		{
			R = (*(it + 1) - *it) / (*it);
			Return.push_back(R);
		}
	}
};


class IndivStock :public Stock
{
private:
	string Announcementdate;
	string Group;
	double EstimateEPS = 0.0;
	double ReportedEPS = 0.0;
	double Surprise = 0.0;
	double SurprisePercent = 0.0;

public:
	vector<double>AR;
	IndivStock(string Ticker_, string Announcementdate_, string Group_, double EstimateEPS_, double ReportedEPS_, double Surprise_, double SurprisePercent_) { Ticker = Ticker_; Announcementdate = Announcementdate_; Group = Group_; EstimateEPS = EstimateEPS_; ReportedEPS = ReportedEPS_; Surprise = Surprise_; SurprisePercent = SurprisePercent_; }
	string GetAncDate() { return Announcementdate; }
	double GetEstimateEPS() { return EstimateEPS; }
	double GetReportedEPS() { return ReportedEPS; }
	double GetSurprise() { return Surprise; }
	double  GetSurprisePercent() { return SurprisePercent; }
};

class SPY :public Stock
{
public:
	SPY() { Ticker = "SPY"; }
};

#endif
