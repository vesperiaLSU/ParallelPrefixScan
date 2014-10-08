#ifndef GUARD_Prefix_tree_h
#define GUARD_Prefix_tree_h
#include <vector>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <time.h>
#include <future>
#include <thread>
#include <string>
#include <boost/thread/future.hpp>
#include <boost/thread.hpp>

class TreeNode {
public:
    TreeNode() : sum(0), fromleft(0), index(0) {}
    int sum;
    int fromleft;
    int index;
};

//Parallel//
std::vector<int> serial_prefix_sum(std::vector<int>&);
std::vector<bool> to_binary(int);
double log2(double);
void create_seed(int);
void input_generation(int const, std::vector<boost::future<std::vector<TreeNode>>>&,
    std::vector<TreeNode>&, std::vector<TreeNode>&, std::vector<std::vector<TreeNode>>&);
template<typename T> size_t up_sweep_phase(std::vector<T>&);
template<> size_t up_sweep_phase(std::vector<TreeNode>&);
size_t down_sweep_phase(std::vector<TreeNode>&);

#endif
