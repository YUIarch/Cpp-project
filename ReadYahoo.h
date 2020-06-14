#ifndef ReadYahoo_h
#define ReadYahoo_h
#include <string>
#include <stdio.h>
#include <sstream>  
#include <locale>
#include <iomanip>
#include <iostream>
#include <vector>
#include <map>
#include <mutex>
#include "curl/curl.h"
#include "TimeAdjuster.h"
#include "Stock.h"
#include "Operator.h"
using namespace std;

mutex mut;

struct MemoryStruct {
	char* memory;
	size_t size;
	//Class of memory
};

void* myrealloc(void* ptr, size_t size)
{
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
	//Memory allocation check
}

int write_data(void* ptr, int size, int nmemb, FILE* stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)data;
	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
	//The writing function we will use in libcurl
	//To Write from libcurl network interface to memory in our computer
}

string getTimeinSeconds(string Time)
{
	std::tm t = { 0 };
	std::istringstream ssTime(Time);
	char time[100];
	memset(time, 0, 100);
	if (ssTime >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S"))
	{
		sprintf(time, "%lld", mktime(&t));
		return string(time);
	}
	else
	{
		cout << "Parse failed\n";
		return "";
	}
	//Transfer normal time form to second from 1970/01/01.
}

vector<double> CalAR(vector<IndivStock>::iterator it, SPY MarketIndex, int N)
{

	auto itr = find(MarketIndex.Date.begin(), MarketIndex.Date.end(), it->GetAncDate());
	int start = itr - MarketIndex.Date.begin();

	vector<double>::iterator first = MarketIndex.Return.begin() + start - 1 - N;
	vector<double>::iterator last = MarketIndex.Return.begin() + start - 1 + N + 1;
	vector<double>SPYReturn(first, last);
	vector<double>StockReturn = (*it).Return;

	vector<double> AR;
	AR = StockReturn - SPYReturn;
	return AR;
}

int ReadYahooSPY(SPY& MarketIndex)
{
	ifstream TC("TradingCalendar.csv");
	string line, Date;
	vector<string> DateList;
	while (getline(TC, line))
	{
		istringstream each(line);
		getline(each, Date, ',');
		DateList.push_back(Date);
	}

	struct MemoryStruct data;
	data.memory = NULL;
	data.size = 0;

	// file pointer to create file that store the data
	FILE* fp;


	// declaration of an object CURL 
	CURL* handle;

	CURLcode result;

	// set up the program environment that libcurl needs 
	curl_global_init(CURL_GLOBAL_ALL);

	// curl_easy_init() returns a CURL easy handle 
	handle = curl_easy_init();

	// if everything's all right with the easy handle... 
	if (handle)
	{
		string url_common = "https://eodhistoricaldata.com/api/eod/";
		string start_date = DateList.front();
		string end_date = DateList.back();
		string api_token = "5ebb0f5a0015c2.55590664";


		struct MemoryStruct data;
		data.memory = NULL;
		data.size = 0;

		string symbol = MarketIndex.Ticker;
		string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
		curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

		//adding a user agent
		curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
		curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

		// perform, then store the expected code in result
		result = curl_easy_perform(handle);

		if (result != CURLE_OK)
		{
			// if errors have occured, tell us what is wrong with result
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
			return 1;
		}

		stringstream sData;
		sData.str(data.memory);
		string sValue, sDate;
		double dValue = 0;
		string line;
		cout << MarketIndex.Ticker << endl;
		while (getline(sData, line))
		{
			size_t found = line.find('-');
			if (found != std::string::npos)
			{
				sDate = line.substr(0, line.find_first_of(','));
				line.erase(line.find_last_of(','));
				sValue = line.substr(line.find_last_of(',') + 1);
				dValue = strtod(sValue.c_str(), NULL);
				MarketIndex.ExpandDate(sDate);
				MarketIndex.ExpandPrice(dValue);
			}
		}
		MarketIndex.CalR();
		free(data.memory);
		data.size = 0;
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return 1;
	}

	// cleanup since you've used curl_easy_init  
	curl_easy_cleanup(handle);

	// release resources acquired by curl_global_init() 
	curl_global_cleanup();

	return 0;
}

int ReadYahoo(vector<IndivStock>& IndividualStock, int N, SPY MarketIndex)
{
	struct MemoryStruct data;
	data.memory = NULL;
	data.size = 0;

	// file pointer to create file that store the data
	FILE* fp;

	//const char resultfilename[FILENAME_MAX] = "Results.txt";


	// declaration of an object CURL 
	CURL* handle;

	CURLcode result;

	// set up the program environment that libcurl needs 
	curl_global_init(CURL_GLOBAL_ALL);

	// curl_easy_init() returns a CURL easy handle 
	handle = curl_easy_init();

	// if everything's all right with the easy handle... 
	if (handle)
	{
		auto itr = IndividualStock.begin();
		string url_common = "https://eodhistoricaldata.com/api/eod/";
		string api_token = "5ebb0f5a0015c2.55590664";


		for (itr; itr != IndividualStock.end(); itr++)
		{
			struct MemoryStruct data;
			data.memory = NULL;
			data.size = 0;

			string start_date = TimeAdjuster(itr, N, "startTime");
			string end_date = TimeAdjuster(itr, N, "endTime");
			string symbol = (*itr).Ticker;
			string url_request = url_common + symbol + ".US?" + "from=" + start_date + "&to=" + end_date + "&api_token=" + api_token + "&period=d";
			curl_easy_setopt(handle, CURLOPT_URL, url_request.c_str());

			//adding a user agent
			curl_easy_setopt(handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:74.0) Gecko/20100101 Firefox/74.0");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);

			/*
			fp = fopen(resultfilename, "ab");
			fprintf(fp, "%s\n", symbol.c_str());
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp);
			result = curl_easy_perform(handle);
			fprintf(fp, "%c", '\n');
			fclose(fp);
			if (result != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}
			*/

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

			// perform, then store the expected code in result
			result = curl_easy_perform(handle);

			if (result != CURLE_OK)
			{
				// if errors have occured, tell us what is wrong with result
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}

			stringstream sData;
			sData.str(data.memory);
			string sValue, sDate;
			double dValue = 0;
			string line;
			cout << itr->Ticker << itr->GetAncDate() << endl;
			while (getline(sData, line))
			{
				size_t found = line.find('-');
				if (found != std::string::npos)
				{
					sDate = line.substr(0, line.find_first_of(','));
					line.erase(line.find_last_of(','));
					sValue = line.substr(line.find_last_of(',') + 1);
					dValue = strtod(sValue.c_str(), NULL);
					(*itr).ExpandDate(sDate);
					(*itr).ExpandPrice(dValue);
				}
			}
			if ((*itr).Price.size() != (2 * N + 2) || (*itr).Price.size() == 0)
			{
				cout << "Cannot Get Price" << endl;
			}
			else
			{
				itr->CalR();
				(*itr).AR = CalAR(itr, MarketIndex, N);
			}
			free(data.memory);
			data.size = 0;
		}
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return 1;
	}

	lock_guard<mutex>guard(mut);

	// cleanup since you've used curl_easy_init  
	curl_easy_cleanup(handle);

	// release resources acquired by curl_global_init() 
	curl_global_cleanup();

	return 0;
}

#endif 
