#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include <map>
#include <set>

#include <algorithm>
#include <ctime>

//#define DRAW_PLOT
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

#ifdef DRAW_PLOT
#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

void drawPlot(const vector<int> &values, const vector<time_t> &times,
              const int max_count)
{
    vector<double> xValues, yValues;

    for(size_t i = 0; i < values.size(); i++)
    {
        double x = times[i];
        double y = values[i];

        double prev_y = i > 0 ? values[i-1] : 0;
        xValues.push_back(x);
        yValues.push_back(prev_y);
        xValues.push_back(x);
        yValues.push_back(y);
    }
    plt::ylim(0, max_count * 2);
    plt::plot(xValues, yValues);
    plt::show();
}

#endif

//--------------------------------------------------------

time_t calcTimestampForTime(const string &timeAsString)
{
    time_t hour, min;
    sscanf(timeAsString.c_str(), "%ld:%ld", &hour, &min);

    return hour * 60 + min;
}

time_t calcTimestampForDate(const string &dateAsString)
{
    tm timestamp;
    sscanf(dateAsString.c_str(), "%u-%u-%uT%u:%u:00", &timestamp.tm_year, &timestamp.tm_mon, &timestamp.tm_mday, &timestamp.tm_hour, &timestamp.tm_min);
    timestamp.tm_sec = 0;
    time_t res = mktime(&timestamp);
    return res;
}

string calcTimestringForTime(const time_t &ts)
{
    time_t hour, min;
    hour = ts / 60;
    min = ts % 60;

    char buf[5];
    sprintf(buf, "%.2ld:%.2ld", hour, min);
    string result = buf;

    return result;
}

string calcTimestringForDate(const time_t &ts)
{
    tm * timeinfo = localtime(&ts);

    char buf[17];
    sprintf(buf, "%d-%.2d-%.2dT%.2d:%.2d:00", timeinfo->tm_year, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min);

    string result = buf;

    return result;
}

//--------------------------------------------------------

typedef map<time_t, int> BusynessMap;

BusynessMap readDataFromTXT(const string &filename)
{
    BusynessMap result;

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

        time_t in_ts = calcTimestampForTime(in_time);
        time_t out_ts = calcTimestampForTime(out_time);


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

    }
    return result;
}

static const string JSONArrivalKey = "ArrivalTime";
static const string JSONLeaveKey = "LeaveTime";

BusynessMap readDataFromJSON(const string &filename)
{
    BusynessMap result;

    std::ifstream instream(filename);
    json json_data;
    instream >> json_data;

    for (json::iterator it = json_data.begin(); it != json_data.end(); ++it)
    {
            auto record = *it;
            string in_time, out_time;

            in_time = record[JSONArrivalKey];
            out_time = record[JSONLeaveKey];

            time_t in_ts = calcTimestampForDate(in_time);
            time_t out_ts = calcTimestampForDate(out_time);

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

    }


    return result;
}
//--------------------------------------------------------

void printIntervals(const vector<int> &values, const vector<time_t> &times,
                    const int max_value,
                    bool use_date)
{
    bool isIntervalStarted = false;
    for (size_t i = 0; i <= values.size(); i++)
    {
        if (values[i] == max_value)
        {
            if (isIntervalStarted)
                continue;

            string ts_string = use_date ? calcTimestringForDate(times[i]) : calcTimestringForTime(times[i]);
            cout << "From " << ts_string;
            isIntervalStarted = true;
        }
        else if (isIntervalStarted)
        {
            string ts_string = use_date ? calcTimestringForDate(times[i]) : calcTimestringForTime(times[i]);
            cout << " to " << ts_string << endl;
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

    BusynessMap data_map;
    bool use_date;

    if (filename.find(".txt") != string::npos)
    {

        data_map = readDataFromTXT(filename);
        use_date = false;
    }
    else if (filename.find(".json") != string::npos)
    {

        data_map = readDataFromJSON(filename);
        use_date = true;
    }
    else
    {
        printf("Wrong file format, use json or txt!");
        return -1;
    }

    vector<int> values;
    vector<time_t> times;
    int current_count = 0;
    int max_count = 0;
    for (auto iter = data_map.begin(); iter != data_map.end(); iter++)
    {
        current_count += iter->second;

        if (current_count > max_count)
            max_count = current_count;

        values.push_back(current_count);
        times.push_back(iter->first);
    }

    printIntervals(values, times,
                   max_count,
                   use_date);

#ifdef DRAW_PLOT
    drawPlot(values, times, max_count);
#endif
    return 0;
}

