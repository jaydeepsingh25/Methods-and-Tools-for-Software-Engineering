#include <algorithm>
#include <iostream>
#include <random>
#include <tuple>
#include <vector>
#include <getopt.h>
#include <chrono>
#include <thread>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>
using namespace std;

class Street
{
public:
    string name;
    vector<tuple<int, int>> streetPoints;
    string printPoints()
    {
        //check the generated input for each street
        // cout << streetName << endl;
        string str = "";
        for (int k = 0; k < (int)this->streetPoints.size(); k++)
        {
            str += "(" + to_string(get<0>(this->streetPoints[k])) + "," + to_string(get<1>(this->streetPoints[k])) + ")";
        }
        return str;
    }
};
// The function will check if the line segment between (x1, y1) and (x2,y2) intersects any of the segment in streetPoints
bool isIntersectingStreet(vector<tuple<int, int>> streetPoints, int x1, int y1, int x2, int y2)
{

    float x;
    float y;
    // We represent first line  as A1x + B1y = C1
    int A1 = y2 - y1;
    int B1 = x1 - x2;
    int C1 = A1 * x1 + B1 * y1;

    for (int k = 0; (k >= 0 && k < (int)streetPoints.size() - 2); k++)
    {
        //cout << k << endl;
        int x3 = get<0>(streetPoints[k]);
        int y3 = get<1>(streetPoints[k]);
        int x4 = get<0>(streetPoints[k + 1]);
        int y4 = get<1>(streetPoints[k + 1]);

        //Similarly , We represent second as A2x + B2y = C2
        int A2 = y4 - y3;
        int B2 = x3 - x4;
        int C2 = A2 * x3 + B2 * y3;

        float det = A1 * B2 - A2 * B1;
        //cout << det << endl;
        if (det == 0)
        {
            return false;
        }
        else
        {
            x = (B2 * C1 - B1 * C2) / det;
            y = (A1 * C2 - A2 * C1) / det;
        }
        //Checking that x, y lies on first line segment:
        bool x_present_1 = ((x >= min(x1, x2)) && (x <= max(x1, x2)));
        bool y_present_1 = ((y >= min(y1, y2)) && (y <= max(y1, y2)));

        // Checking that x, y lies on second line segment:
        bool x_present_2 = ((x >= min(x3, x4)) && (x <= max(x3, x4)));
        bool y_present_2 = ((y >= min(y3, y4)) && (y <= max(y3, y4)));
        if (x_present_1 and y_present_1 and x_present_2 and y_present_2)
        {
            //cout << "Found Self intersection" << endl;
            return true;
        }
    }
    return false;
}

// function to check if the segment between x1, y1 and x2,y2 overlaps with any street in the input, returns true if it is overlapping street
bool isOverlappingStreet(vector<Street> allStreets, int x1, int y1, int x2, int y2)
{

    // if there is one line segment between (x1,y1) and (x2,y2) , and other line segment between (x3,y3) and (x4,y4)
    // overlapping can occur if slopes of both lines are equal and any of the following condition is satisfied:
    // 1. (x3,y3) lies between (x1, y1) and (x2,y2)
    // 2. (x4,y4) lies between (x1, y1) and (x2,y2)
    // 3. (x1,y1) lies between (x3, y3) and (x4,y4)
    // We represent first line  as A1x + B1y = C1
    int A1 = y2 - y1;
    int B1 = x1 - x2;
    //int C1 = A1 * x1 + B1 * y1;

    for (int st = 0; st < (int)allStreets.size(); st++)
    {
        vector<tuple<int, int>> streetPoints = allStreets[st].streetPoints;
        for (int k = 0; k < (int)streetPoints.size(); k++)
        {
            int x3 = get<0>(streetPoints[k]);
            int y3 = get<1>(streetPoints[k]);
            int x4 = get<0>(streetPoints[k + 1]);
            int y4 = get<1>(streetPoints[k + 1]);

            //Similarly , We represent second as A2x + B2y = C2
            int A2 = y4 - y3;
            int B2 = x3 - x4;
            //int C2 = A2 * x3 + B2 * y3;

            float det = A1 * B2 - A2 * B1;
            // det = 0 if lines are parallel
            // 1. Checking that (x3,y3) lies between (x1, y1) and (x2,y2)
            bool x3_present_x1_x2 = ((x3 >= min(x1, x2)) && (x3 <= max(x1, x2)));
            bool y3_present_y1_y2 = ((y3 >= min(y1, y2)) && (y3 <= max(y1, y2)));

            // 2. Checking that (x4,y4) lies between (x1, y1) and (x2,y2)
            bool x4_present_x1_x2 = ((x4 >= min(x1, x2)) && (x4 <= max(x1, x2)));
            bool y4_present_y1_y2 = ((y4 >= min(y1, y2)) && (y4 <= max(y1, y2)));

            // 3. Checking that (x1,y1) lies between (x3, y3) and (x4,y4)
            bool x1_present_x3_x4 = ((x1 >= min(x3, x4)) && (x1 <= max(x3, x4)));
            bool y1_present_y3_y4 = ((y1 >= min(y3, y4)) && (y1 <= max(y3, y4)));

            if (det == 0)
            {
                if ((x3_present_x1_x2 && y3_present_y1_y2) ||
                    (x4_present_x1_x2 && y4_present_y1_y2) ||
                    (x1_present_x3_x4 && y1_present_y3_y4))
                {
                    float intercept_1 = float(y1) + float(A1 / B1);
                    float intercept_2 = float(y3) + float(A2 / B2);
                    if (intercept_2 == intercept_1)
                    {
                        //cout << "Found Overlap" << endl;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

int main(int argc, char **argv)
{

    //srand(time(0));
    // upper bound for no.of streets
    int streets = 10;
    // upper bound for no. of line segments in each street
    int segments = 5;
    // time interval between 2 random inputs
    int seconds = 5;
    // Co-ordinates would be generated randomly between [-coordinateRange, +coordinateRange]
    int coordinateRange = 20;

    const int maxAttempt = 25;
    int invalidOutput = 0;

    vector<Street> randomInput;

    random_device rd2("/dev/urandom");

    int c;
    while ((c = getopt(argc, argv, "s:n:l:c:")) != -1)
    {
        switch (c)
        {
        case 's':
            if (optarg)
                streets = atoi(optarg);
            if (streets < 2)
            {
                cerr << "Error: argument -s must have value >= 2" << endl;
                return 1;
            }
            break;
        case 'n':
            if (optarg)
                segments = atoi(optarg);
            if (segments < 1)
            {
                cerr << "Error: argument -n must have value >= 1" << endl;
                return 1;
            }
            break;
        case 'l':
            if (optarg)
                seconds = atoi(optarg);
            if (seconds < 5)
            {
                cerr << "Error: argument -l must have value >= 5" << endl;
                return 1;
            }
            break;
        case 'c':
            if (optarg)
                coordinateRange = atoi(optarg);
            if (coordinateRange < 1)
            {
                cerr << "Error: argument -c must have value >= 1" << endl;
                return 1;
            }
            break;
        }
    }

    uniform_int_distribution<int> streetDist(2, streets);
    uniform_int_distribution<int> waitSecondsDist(5, seconds);
    uniform_int_distribution<int> lineSegDist(1, segments);
    uniform_int_distribution<int> coordinateDist(-coordinateRange, coordinateRange);
    vector<string> pythonInput;
    vector<string> removeInput;
    while (true)
    {
        pythonInput.clear();
        randomInput.clear();
        removeInput.clear();
        int noOfStreets = streetDist(rd2);
        int waitSeconds = waitSecondsDist(rd2);
        // cout << "Randomly Generated Inputs" << endl;
        // cout << "no_of_streets" <<  noOfStreets << endl;
        // cout << "waitseconds" <<  waitSeconds << endl;

        char dictionary[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                             'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                             's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' '};
        // generating random input for each street
        for (int index = 0; index < noOfStreets; index++)
        {
            Street streetObj;
            string streetName = "";
            vector<tuple<int, int>> streetPoints;
            // default length of each street name
            for (int _k = 0; _k < 15; _k++)
            {
                streetName = streetName + dictionary[rand() % 27];
                // cout << streetName << endl;
            }
            streetObj.name = streetName;

            int noOfLineSegments = lineSegDist(rd2);
            //cout << "no_of_line_segments" <<  noOfLineSegments << endl;
            // construct input for each line segment
            for (int j = 0; j <= noOfLineSegments; j++)
            {
                int cordinate_x = coordinateDist(rd2);
                int cordinate_y = coordinateDist(rd2);
                if (invalidOutput == maxAttempt)
                {
                    cerr << "Error: failed to generate valid input for " << maxAttempt << " simultaneous attempts" << endl;
                    exit(0);
                }
                //check for zero length line segments
                if (streetPoints.size() > 0)
                {
                    //cout << to_string(get<1>(streetPoints.back())) << endl;
                    int lastPoint_x = get<0>(streetPoints.back());
                    int lastPoint_y = get<1>(streetPoints.back());
                    if (
                        ((cordinate_x != lastPoint_x) && (cordinate_y != lastPoint_y)) &&
                        !(find(streetPoints.begin(), streetPoints.end(), make_tuple(int(cordinate_x), int(cordinate_y))) != streetPoints.end()))
                    {
                        if (isIntersectingStreet(streetPoints, lastPoint_x, lastPoint_y, cordinate_x, cordinate_y))
                        {
                            j--;
                            invalidOutput += 1;
                        }
                        else
                        {
                            if (randomInput.size() > 0)
                            {
                                if (isOverlappingStreet(randomInput, lastPoint_x, lastPoint_y, cordinate_x, cordinate_y))
                                {
                                    j--;
                                    invalidOutput += 1;
                                }
                                else
                                {
                                    invalidOutput = 0;
                                    streetPoints.push_back(make_tuple(int(cordinate_x), int(cordinate_y)));
                                }
                            }
                            else
                            {
                                invalidOutput = 0;
                                streetPoints.push_back(make_tuple(int(cordinate_x), int(cordinate_y)));
                            }
                        }
                    }
                    else
                    {
                        j--;
                        invalidOutput += 1;
                    }
                }
                else
                {
                    invalidOutput = 0;
                    streetPoints.push_back(make_tuple(int(cordinate_x), int(cordinate_y)));
                }
            }
            // if (invalidOutput == maxAttempt)
            // {
            //     break;
            // }
            streetObj.streetPoints = streetPoints;
            randomInput.push_back(streetObj);
        }
        // if (invalidOutput == maxAttempt)
        // {
        //     cerr << "Error: failed to generate valid input for " << maxAttempt << " simultaneous attempts" << endl;
        //     break;
        // }
        for (int st = 0; st < (int)randomInput.size(); st++)
        {
            pythonInput.push_back(string("a ") + '\"' + randomInput[st].name + '\"' + ' ' + randomInput[st].printPoints() + "\n");
        }
        for (int i = 0; i < (int)pythonInput.size(); i++)
        {
            cout << pythonInput[i];
        }
        cout << "g" << endl;
        for (int index = 0; index < (int)randomInput.size(); index++)
        {
            removeInput.push_back(string("r ") + '\"' + randomInput[index].name + '\"' + "\n");
        }
        for (int i = 0; i < (int)removeInput.size(); i++)
        {
            cout << removeInput[i];
        }
        sleep(waitSeconds);
    }
}
