#include "gtest/gtest.h"
#include "kmeans.h"
#include "oracle.h"

TEST(test_kmeans, test1) {
    // set n, d, k, iter
    int n = 100;
    int d = 2;
    int k = 3;
    int iter = 100;
    
    // create random data as a vector of strings with numbers separated by spaces
    std::vector<std::string> data;
    for (int i = 0; i < n; i++) {
        std::string line;
        for (int j = 0; j < d; j++) {
            line += std::to_string(rand()) + " ";
        }
        data.push_back(line);
    }

    // create vector of kmeans::Point
    std::vector<kmeans::Point> points;
    // create vector of oracle::Point
    std::vector<oracle::Point> oracle_points;
    
    // initialize both vectors with data
    for (int i = 0; i < n; i++) {
        points.push_back(kmeans::Point(i, data[i]));
        oracle_points.push_back(oracle::Point(i, data[i]));
    }

    // create kmeans::KMeans object
    kmeans::KMeans kmeans(k, iter);
    // create oracle::KMeans object
    oracle::KMeans oracle_kmeans(k, iter);

    // sample k random points as centroids
    std::vector<int> centroid_ids;
    for (int i = 0; i < k; i++) {
        centroid_ids.push_back(rand() % n);
    }

    // set centroids for both kmeans objects
    kmeans.setClusters(centroid_ids, points);
    oracle_kmeans.setClusters(centroid_ids, oracle_points);

    // run kmeans algorithm
    kmeans.run(points);
    oracle_kmeans.run(oracle_points);

    // get centroids from both kmeans objects
    std::vector<vector<double>> centroids = kmeans.getCentroids();
    std::vector<vector<double>> oracle_centroids = oracle_kmeans.getCentroids();

    // get labels from both kmeans objects
    std::vector<int> labels = kmeans.getLabels(points);
    std::vector<int> oracle_labels = oracle_kmeans.getLabels(oracle_points);

    // check if centroids are the same
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < d; j++) {
            EXPECT_EQ(centroids[i][j], oracle_centroids[i][j]);
        }
    }

    // check if labels are the same
    for (int i = 0; i < n; i++) {
        EXPECT_EQ(labels[i], oracle_labels[i]);
    }

}