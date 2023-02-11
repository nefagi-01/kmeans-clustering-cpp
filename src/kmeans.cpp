#include <omp.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "kmeans.h"

using std::vector; // using directive
using std::string; // using directive
using std::cout; // using directive
using std::endl; // using directive
using std::ofstream; // using directive
using std::to_string; // using directive


namespace kmeans
{

class Point
{
private:
    int pointId, clusterId;
    int dimensions;
    vector<double> values;

    vector<double> lineToVec(string &line)
    {
        vector<double> values;
        string tmp = "";

        for (int i = 0; i < (int)line.length(); i++)
        {
            if ((48 <= int(line[i]) && int(line[i])  <= 57) || line[i] == '.' || line[i] == '+' || line[i] == '-' || line[i] == 'e')
            {
                tmp += line[i];
            }
            else if (tmp.length() > 0)
            {

                values.push_back(stod(tmp));
                tmp = "";
            }
        }
        if (tmp.length() > 0)
        {
            values.push_back(stod(tmp));
            tmp = "";
        }

        return values;
    }

public:
    Point(int id, string line)
    {
        pointId = id;
        values = lineToVec(line);
        dimensions = values.size();
        clusterId = 0; // Initially not assigned to any cluster
    }

    int getDimensions() { return dimensions; }

    int getCluster() { return clusterId; }

    int getID() { return pointId; }

    void setCluster(int val) { clusterId = val; }

    double getVal(int pos) { return values[pos]; }
};

class Cluster
{
private:
    int clusterId;
    vector<double> centroid;
    vector<Point> points;

public:
    Cluster(int clusterId, Point centroid)
    {
        this->clusterId = clusterId;
        for (int i = 0; i < centroid.getDimensions(); i++)
        {
            this->centroid.push_back(centroid.getVal(i));
        }
        this->addPoint(centroid);
    }

    void addPoint(Point p)
    {
        p.setCluster(this->clusterId);
        points.push_back(p);
    }

    bool removePoint(int pointId)
    {
        int size = points.size();

        for (int i = 0; i < size; i++)
        {
            if (points[i].getID() == pointId)
            {
                points.erase(points.begin() + i);
                return true;
            }
        }
        return false;
    }

    void removeAllPoints() { points.clear(); }

    int getId() { return clusterId; }

    Point getPoint(int pos) { return points[pos]; }

    int getSize() { return points.size(); }

    double getCentroidByPos(int pos) { return centroid[pos]; }

    void setCentroidByPos(int pos, double val) { this->centroid[pos] = val; }
};

class KMeans
{
private:
    int K, iters, dimensions, total_points;
    vector<int> indices;
    vector<Cluster> clusters;
    string output_dir;

    void clearClusters()
    {
        for (int i = 0; i < K; i++)
        {
            clusters[i].removeAllPoints();
        }
    }

    int getNearestClusterId(Point point)
    {
        double sum = 0.0, min_dist;
        int NearestClusterId;
        if(dimensions==1) {
            min_dist = abs(clusters[0].getCentroidByPos(0) - point.getVal(0));
        }
        else
        {
            for (int i = 0; i < dimensions; i++)
            {
                sum += pow(clusters[0].getCentroidByPos(i) - point.getVal(i), 2.0);
                // sum += abs(clusters[0].getCentroidByPos(i) - point.getVal(i));
            }
            min_dist = sqrt(sum);
        }
        NearestClusterId = clusters[0].getId();

        for (int i = 1; i < K; i++)
        {
            double dist;
            sum = 0.0;

            if(dimensions==1) {
                dist = abs(clusters[i].getCentroidByPos(0) - point.getVal(0));
            }
            else {
                for (int j = 0; j < dimensions; j++)
                {
                    sum += pow(clusters[i].getCentroidByPos(j) - point.getVal(j), 2.0);
                    // sum += abs(clusters[i].getCentroidByPos(j) - point.getVal(j));
                }

                dist = sqrt(sum);
                // dist = sum;
            }
            if (dist < min_dist)
            {
                min_dist = dist;
                NearestClusterId = clusters[i].getId();
            }
        }

        return NearestClusterId;
    }

public:
    KMeans(int K, int iterations, string output_dir)
    {
        this->K = K;
        this->iters = iterations;
        this->output_dir = output_dir;
    }

    KMeans(int K, int iterations)
    {
        this->K = K;
        this->iters = iterations;
    }


    void Assignment(vector<Point> &all_points, bool &done)
    {
        // Add all points to their nearest cluster
        #pragma omp parallel for reduction(&&: done) num_threads(16)
        for (int i = 0; i < total_points; i++)
        {
            int currentClusterId = all_points[i].getCluster();
            int nearestClusterId = getNearestClusterId(all_points[i]);

            if (currentClusterId != nearestClusterId)
            {
                all_points[i].setCluster(nearestClusterId);
                done = false;
            }
        }

        // clear all existing clusters
        clearClusters();

        // reassign points to their new clusters
        for (int i = 0; i < total_points; i++)
        {
            // cluster index is ID-1
            clusters[all_points[i].getCluster() - 1].addPoint(all_points[i]);
        }
    }

    void Update()
    {
        for (int i = 0; i < K; i++)
        {
            int ClusterSize = clusters[i].getSize();

            for (int j = 0; j < dimensions; j++)
            {
                double sum = 0.0;
                if (ClusterSize > 0)
                {
                    #pragma omp parallel for reduction(+: sum) num_threads(16)
                    for (int p = 0; p < ClusterSize; p++)
                    {
                        sum += clusters[i].getPoint(p).getVal(j);
                    }
                    clusters[i].setCentroidByPos(j, sum / ClusterSize);
                }
            }
        }
    }

    void setClusters(vector<int> indices, vector<Point> &all_points) { 
        for(int i=0; i<indices.size(); i++) {
            // set cluster id for the sampled point
            all_points[indices[i]].setCluster(i);
            // create a cluster with the sampled point
            Cluster cluster(i, all_points[indices[i]]);
            // add the cluster to the list of clusters
            clusters.push_back(cluster);
        }
    }

    vector<int> getLabels(vector<Point> &all_points) {
        vector<int> labels;
        for(int i=0; i<all_points.size(); i++) {
            labels.push_back(all_points[i].getCluster());
        }
        return labels;
    }

    vector<vector<double>> getCentroids() {
        vector<vector<double>> centroids;
        for(int i=0; i<clusters.size(); i++) {
            vector<double> centroid;
            for(int j=0; j<dimensions; j++) {
                centroid.push_back(clusters[i].getCentroidByPos(j));
            }
            centroids.push_back(centroid);
        }
        return centroids;
    }

    void run(vector<Point> &all_points)
    {
        total_points = all_points.size();
        dimensions = all_points[0].getDimensions();

        if (indices.empty())
        {
            // Initializing Clusters
            vector<int> used_pointIds;

            for (int i = 1; i <= K; i++)
            {
                while (true)
                {
                    int index = rand() % total_points;

                    if (find(used_pointIds.begin(), used_pointIds.end(), index) ==
                        used_pointIds.end())
                    {
                        used_pointIds.push_back(index);
                        all_points[index].setCluster(i);
                        Cluster cluster(i, all_points[index]);
                        clusters.push_back(cluster);
                        break;
                    }
                }
            }
            cout << "Clusters initialized = " << clusters.size() << endl
                << endl;
        }

        cout << "Running K-Means Clustering.." << endl;

        int iter = 1;
        while (true)
        {
            cout << "Iter - " << iter << "/" << iters << endl;
            bool done = true;

            // Assigning points to clusters
            Assignment(all_points, done);

            // Recalculating the center of each cluster
            Update();

            if (done || iter >= iters)
            {
                cout << "Clustering completed in iteration : " << iter << endl
                    << endl;
                break;
            }
            iter++;
        }

        // Write cluster points to file if output directory is specified
        if (!output_dir.empty())
        {   ofstream pointsFile;
            pointsFile.open(output_dir + "/" + to_string(K) + "-points.txt", std::ios::out);

            for (int i = 0; i < total_points; i++)
            {
                pointsFile << all_points[i].getCluster() << endl;
            }

            pointsFile.close();

            // Write cluster centers to file
            ofstream outfile;
            outfile.open(output_dir + "/" + to_string(K) + "-clusters.txt");
            if (outfile.is_open())
            {
                for (int i = 0; i < K; i++)
                {
                    cout << "Cluster " << clusters[i].getId() << " centroid : ";
                    for (int j = 0; j < dimensions; j++)
                    {
                        cout << clusters[i].getCentroidByPos(j) << " ";    // Output to console
                        outfile << clusters[i].getCentroidByPos(j) << " "; // Output to file
                    }
                    cout << endl;
                    outfile << endl;
                }
                outfile.close();
            }
            else
            {
                cout << "Error: Unable to write to clusters.txt";
            }}
    }

};

}