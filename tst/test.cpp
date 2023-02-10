#include <gtest/gtest.h>
#include <fstream>
#include <string>

const std::string input_file = "input.txt";
const std::string output_file_1 = "cluster-details/2-clusters.txt";
const std::string output_file_2 = "cluster-details/2-points.txt";
const std::string cluster_details = "cluster-details";
const int num_clusters = 2;

bool compare_files(const std::string &file1, const std::string &file2) {
    std::ifstream input1(file1);
    std::ifstream input2(file2);
    std::string line1, line2;
    while (std::getline(input1, line1) && std::getline(input2, line2)) {
        if (line1 != line2) {
            return false;
        }
    }
    return true;
}

TEST(KMeansTest, VerifyOutput) {
// Run kmeans.cpp
std::string kmeans_command = "./kmeans " + input_file + " " +
                             std::to_string(num_clusters) + " " + cluster_details;
int result = system(kmeans_command.c_str());
ASSERT_EQ(result, 0);

// Run kmeans_oracle.cpp
std::string kmeans_oracle_command = "./kmeans_oracle " + input_file + " " +
                                    std::to_string(num_clusters) + " " + cluster_details;
result = system(kmeans_oracle_command.c_str());
ASSERT_EQ(result, 0);

// Compare outputs
ASSERT_TRUE(compare_files(output_file_1, output_file_1 + "_oracle"));
ASSERT_TRUE(compare_files(output_file_2, output_file_2 + "_oracle"));
}
