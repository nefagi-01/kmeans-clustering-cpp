#include <vector> // #include directive
#include <string> // #include directive

using std::vector; // using directive
using std::string; // using directive

// my_class.h
#ifndef ORACLE_H // include guard
#define ORACLE_H
namespace oracle
{
    class Point
    {
    private:
        int pointId, clusterId;
        int dimensions;
        vector<double> values;

        vector<double> lineToVec(string &line);

    public:
        Point(int id, string line);

        int getDimensions();

        int getCluster();

        int getID();

        void setCluster(int val);

        double getVal(int pos);
    };

    class Cluster
    {
    private:
        int clusterId;
        vector<double> centroid;
        vector<Point> points;
    public:
        Cluster(int clusterId, Point centroid);

        void addPoint(Point p);

        bool removePoint(int pointId);

        void removeAllPoints();

        int getId();

        Point getPoint(int pos);

        int getSize();

        double getCentroidByPos(int pos);

        void setCentroidByPos(int pos, double val);
    };

    class KMeans
    {
    private:
        int K, iters, dimensions, total_points;
        vector<Cluster> clusters;
        string output_dir;

        void clearClusters();

        int getNearestClusterId(Point point);

    public:
        KMeans(int K, int iterations, string output_dir);

        KMeans(int K, int iterations);

        void setClusters(vector<int> indices, vector<Point> &all_points);

        vector<int> getLabels(vector<Point> &all_points);

        vector<vector<double>> getCentroids();

        void run(vector<Point> &all_points);
    };

}

#endif /* MY_CLASS_H */