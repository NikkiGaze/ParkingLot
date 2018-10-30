#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <map>

#include "matplotlibcpp.h"

using namespace std;
namespace plt = matplotlibcpp;

struct TimeStamp
{
    int timestamp;
    TimeStamp(const string &timeAsString)
    {
        int hour, min;
        scanf("%d:%d", &hour, &min);
        timestamp = hour * 60 + min;
    }
};

int calcTimestamp(const string &timeAsString)
{
    int hour, min;
    sscanf(timeAsString.c_str(),"%d:%d", &hour, &min);

    return hour * 60 + min;
}

int calcTimestring(const TimeStamp &ts)
{
//    int hour, min;
//    sscanf(timeAsString.c_str(),"%d:%d", &hour, &min);

    return 1;//hour * 60 + min;
}

typedef unordered_map<int, int> BusynessMap;

BusynessMap readData(const string &filename,
                     int &min_time,
                     int &max_time)
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

        int in_ts = calcTimestamp(in_time);
        int out_ts = calcTimestamp(out_time);

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

int main(int argc, char *argv[])
{
    int min_time, max_time;
    if (!argv[1])
        return -1;

    string filename = argv[1];
    BusynessMap data_map = readData(filename,
                                    min_time, max_time);

    vector<int> values;
    vector<int> times;
    int current_count = 0;
    for (int i = min_time; i <= max_time; i++)
    {
        current_count += data_map[i];
//        cout << "Timestamp " << i << "load " << current_count << endl;
        values.push_back(current_count);
        times.push_back(i);
    }

    plt::plot(times, values);
    plt::xlim(0, calcTimestamp("24:00"));
    plt::show();
    return 0;
}

