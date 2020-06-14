#include <stdio.h>
#include <string> 
#include <iostream>
#include <sstream>  
#include <vector>
#include <locale>
#include <map>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <mutex>
#include <thread>
#include "Stock.h"
#include "ReadEPSCsv.h"
#include "ReadYahoo.h"
#include"Group.h"
#include "curl_easy.h"
#include "curl_form.h"
#include "curl_ios.h"
#include "curl_exception.h"
#include "Visualization.h"
#include "SslCurlWrapper.h"

using namespace std;
mutex mu;


void showMenu() {
    cout << "Menu:" << endl;
    cout << "1.Change N to retrieve historical price data for all stocks" << endl
        << "2. Pull information for one stock from one group." << endl
        << "3. Show AAR, AARSD, CAAR, CAARSD for one group." << endl
        << "4. Show the Gunplot graphs with CAAR for all 3 groups." << endl
        << "5. Exit your program" << endl;
    cout << endl;
    cout << "Enter your option: ";

}
void checkticker(string Ticker, map<string, IndivStock> m, string groupname, int& x) {
    for (map<string, IndivStock>::iterator it = m.begin(); it != m.end(); it++) {
        if (it->first == Ticker)
        {
            cout << "This stock is in " << groupname << " Group" << endl;
            map<string, IndivStock>::iterator pos = m.find(Ticker);
            cout << "EstimateEPS:" << pos->second.GetEstimateEPS() << endl;
            cout << "ReportedEPS:" << pos->second.GetReportedEPS() << endl;
            cout << "Surprise:" << pos->second.GetSurprise() << endl;
            cout << "SurprisePercent:" << pos->second.GetSurprisePercent() << endl;
            x = 1;
            break;
        }

    }
}

int main() {
    int N;
    N = 30;
FLAG:
    Group Beat;
    Group Meet;
    Group Miss;

    SPY MarketIndex;

    ReadYahooSPY(MarketIndex);

    vector<string>BeatTickerList;
    vector<IndivStock>BeatIndividualStock;
    ReadCsv("Beat.csv", "Beat", BeatTickerList, BeatIndividualStock);
    vector<string>MeetTickerList;
    vector<IndivStock>MeetIndividualStock;
    ReadCsv("Meet.csv", "Meet", MeetTickerList, MeetIndividualStock);
    vector<string>MissTickerList;
    vector<IndivStock>MissIndividualStock;
    ReadCsv("Miss.csv", "Miss", MissTickerList, MissIndividualStock);

    SslCurlWrapper sslObject;

    thread Task_Beat(ReadYahoo, ref(BeatIndividualStock), N, MarketIndex);
    thread Task_Meet(ReadYahoo, ref(MeetIndividualStock), N, MarketIndex);
    thread Task_Miss(ReadYahoo, ref(MissIndividualStock), N, MarketIndex);

    Task_Beat.join();
    Task_Meet.join();
    Task_Miss.join();

    Beat.GroupForm(BeatTickerList, BeatIndividualStock, N);
    BootStrap30(Beat, N);
    Meet.GroupForm(MeetTickerList, MeetIndividualStock, N);
    BootStrap30(Meet, N);
    Miss.GroupForm(MissTickerList, MissIndividualStock, N);
    BootStrap30(Miss, N);


    cout << "Retrieve completed" << endl;

    int Option;
    while (true)

    {
        showMenu();
        cin >> Option;

        switch (Option)
        {
        case 1: {
            int change;
            cout << "Enter the number N : " << endl;
            cin >> change;
            N = change;
            if (N > 125)
            {
                cout << "Not enough historical price for entered N!" << endl;
                break;
            }
            else
            {
                goto FLAG;
                break;
            }
        }

        case 2: {
            char temp;
            do {
                string Ticker;
                cout << "Please enter the stock's ticker: "; cin >> Ticker;
                int x = 0;
                //Check the tickername from which list(Beat, Meet, Miss) and show details/information for this stock.

                checkticker(Ticker, Beat.GetMap(), "Beat", x);
                checkticker(Ticker, Meet.GetMap(), "Meet", x);
                checkticker(Ticker, Miss.GetMap(), "Miss", x);

                //If the tickername not in the all three list, then tell us and re-decide option.
                if (x == 0) cout << "Your Ticker is invalid" << endl;
                cout << endl;
                cout << "Do you want to look for other stocks' information? (Y/N) "; cin >> temp;
            } while (temp == 'Y' || temp == 'y');
            break;
        }

        case 3: {

            vector<vector<vector<double>>> Summary;

            vector<double> t;
            t.resize(2 * N + 1);
            vector<vector<double>> mid;
            mid.push_back(t);
            mid.push_back(t);
            mid.push_back(t);
            mid.push_back(t);
            Summary.push_back(mid);
            Summary.push_back(mid);
            Summary.push_back(mid);

            Summary[0][0] = Beat.AverageAAR; Summary[0][1] = Beat.AverageAARSTD; Summary[0][2] = Beat.AverageCAAR; Summary[0][3] = Beat.AverageCAARSTD;
            Summary[1][0] = Meet.AverageAAR; Summary[1][1] = Meet.AverageAARSTD; Summary[1][2] = Meet.AverageCAAR; Summary[1][3] = Meet.AverageCAARSTD;
            Summary[2][0] = Miss.AverageAAR; Summary[2][1] = Miss.AverageAARSTD; Summary[2][2] = Miss.AverageCAAR; Summary[2][3] = Miss.AverageCAARSTD;

            int GroupName;
            int DataName;
            string GroupString[3] = { "Beat", "Meet", "Miss" };
            string DataString[4] = { "AAR", "AARSD", "CAAR", "CAARSD" };

            char temp;
            do {
                cout << "1.Beat   2.Meet  3.Miss:  "; cin >> GroupName;
                //according to groupnumber to change the info we provide
                cout << "1.AAR   2.AARSD  3.CAAR  4.CAARSD:  ";
                cin >> DataName;
                cout << GroupString[GroupName - 1] << " " << DataString[DataName - 1] << endl;
                cout << Summary[GroupName - 1][DataName - 1] << endl;
                cout << "Do you want to look for other data? (Y/N)  "; cin >> temp;
                cout << endl;
            } while (temp == 'Y' || temp == 'y');
            break;


        }

        case 4: {
            cout << "Graph CAAR:" << endl
                << endl;
            plotResults(N, Beat.AverageCAAR, Meet.AverageCAAR, Miss.AverageCAAR);
            break;

        }


        case 5: {
            cout << "Promgram exit" << endl;
            return 0;

            break;
        }
        }
    }  while (Option > 0 && Option < 5);
    return 0;

}