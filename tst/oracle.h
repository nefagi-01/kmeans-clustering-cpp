#include <vector> // #include directive
#include <string> // #include directive

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
        std::vector<double> values;

        std::vector<double> lineToVec(std::string &line);

    public:
        Point(int id, std::string line);

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
        std::vector<double> centroid;
        std::vector<Point> points;
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
        std::vector<Cluster> clusters;
        std::string output_dir;
        std::vector<int> indices;

        void clearClusters();

        int getNearestClusterId(Point point);

    public:
        KMeans(int K, int iterations, std::string output_dir);

        KMeans(int K, int iterations);

        void setClusters(std::vector<int> indices, std::vector<Point> &all_points);

        std::vector<int> getLabels(std::vector<Point> &all_points);

        std::vector<std::vector<double>> getCentroids();

        void run(std::vector<Point> &all_points);
    };

}

#endif /* MY_CLASS_H */