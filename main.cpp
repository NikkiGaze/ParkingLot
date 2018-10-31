#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <map>
#include <algorithm>

//#define DRAW_PLOT
using namespace std;
using uint = unsigned int;

#ifdef DRAW_PLOT
#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

void drawPlot(const vector<int> &values, const vector<uint> &times)
{
    vector<double> xValues, yValues;

    for(size_t i = 0; i < values.size(); i++)
    {
        double x = times[i];
        double y = values[i];
        xValues.push_back(x);
        yValues.push_back(y);
    }
    plt::plot(xValues, yValues);
    plt::xlim(0, 24 * 60);
    plt::show();
}

#endif


//struct TimeStamp
//{
//    uint timestamp;
//    TimeStamp(const string &timeAsString)
//    {
//        uint hour, min;
//        sscanf(timeAsString.c_str(),"%d:%d", &hour, &min);
//        timestamp = hour * 60 + min;
//    }
//};

//--------------------------------------------------------

uint calcTimestamp(const string &timeAsString)
{
    uint hour, min;
    sscanf(timeAsString.c_str(), "%d:%d", &hour, &min);

    return hour * 60 + min;
}

string calcTimestring(const uint &ts)
{
    uint hour, min;
    hour = ts / 60;
    min = ts % 60;

    char buf[5];
    sprintf(buf, "%.2d:%.2d", hour, min);
    string result = buf;

    return result;
}

//--------------------------------------------------------

typedef unordered_map<uint, int> BusynessMap;

BusynessMap readData(const string &filename,
                     uint &min_time,
                     uint &max_time)
{
    BusynessMap result;
    min_time = INT32_MAX;
    max_time = 0;

    ifstream file(filename);

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream instream(line);
        string in_time, out_time;
        if (!(instream >> in_time >> out_time))
        {
            break;
        }

        uint in_ts = calcTimestamp(in_time);
        uint out_ts = calcTimestamp(out_time);

        if (result.find(in_ts) == result.end())
        {
            result[in_ts] = 1;
        }
        else
        {
            result[in_ts]++;
        }

        if (result.find(out_ts) == result.end())
        {
            result[out_ts] = -1;
        }
        else
        {
            result[out_ts]--;
        }

        min_time = std::min(min_time, in_ts);
        max_time = std::max(max_time, out_ts);

    }
    return result;
}

//--------------------------------------------------------

void printIntervals(const vector<int> &values, const vector<uint> &times,
                    const int max_value)
{
    bool isIntervalStarted = false;
    for (uint i = 0; i <= values.size(); i++)
    {
        if (values[i] == max_value)
        {
            if (isIntervalStarted)
                continue;
            cout << "From " << calcTimestring(times[i]);
            isIntervalStarted = true;
        }
        else if (isIntervalStarted)
        {
            cout << " to " << calcTimestring(times[i-1]) << endl;
            isIntervalStarted = false;
        }
    }
}

//--------------------------------------------------------

int main(int, char *argv[])
{
    if (!argv[1])
    {
        printf("No data file passed!");
        return -1;
    }

    string filename = argv[1];
    uint start_time, end_time;

    BusynessMap data_map = readData(filename,
                                    start_time, end_time);
    vector<int> values;
    vector<uint> times;
    int current_count = 0;
    int max_count = 0;
    for (uint i = start_time; i <= end_time; i++)
    {
        current_count += data_map[i];

        if (current_count > max_count)
            max_count = current_count;

        values.push_back(current_count);
        times.push_back(i);
    }

    printIntervals(values, times,
                   max_count);

#ifdef DRAW_PLOT
    drawPlot(values, times);
#endif
    return 0;
}

