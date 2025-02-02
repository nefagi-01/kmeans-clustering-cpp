#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "kmeans.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;


int main(int argc, char **argv)
{
    // Need 3 arguments (except filename) to run, else exit
    if (argc != 4)
    {
        cout << "Error: command-line argument count mismatch. \n ./kmeans <INPUT> <K> <OUT-DIR>" << endl;
        return 1;
    }

    string output_dir = argv[3];

    // Fetching number of clusters
    int K = atoi(argv[2]);

    // Open file for fetching points
    string filename = argv[1];
    ifstream infile(filename.c_str());

    if (!infile.is_open())
    {
        cout << "Error: Failed to open file." << endl;
        return 1;
    }

    // Fetching points from file
    int pointId = 1;
    vector<kmeans::Point> all_points;
    string line;

    while (getline(infile, line))
    {
        kmeans::Point point(pointId, line);
        all_points.push_back(point);
        pointId++;
    }

    infile.close();
    cout << "\nData fetched successfully!" << endl
         << endl;

    // Return if number of clusters > number of points
    if ((int)all_points.size() < K)
    {
        cout << "Error: Number of clusters greater than number of points." << endl;
        return 1;
    }

    // Running K-Means Clustering
    int iters = 100;

    kmeans::KMeans kmeans(K, iters, output_dir);
    kmeans.run(all_points);

    return 0;
}