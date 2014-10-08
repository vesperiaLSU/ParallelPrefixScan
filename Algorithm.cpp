#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_CONTINUATION
#define BOOST_RESULT_OF_USE_DECLTYPE
#define BOOST_ALL_DYN_LINK
#define BOOST_THREAD_VERSION 4
#define USED_THREAD_API USE_STD

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
#include "TreeNode.h"

using namespace std;

//prefix_sum sequential version
vector<int> serial_prefix_sum(vector<int>& input){
    vector<int> output;
    output[0] = input[0];
    for (int i = 1; i < input.size(); ++i)
    {
        output[i] = input[i] + output[i - 1];
    }
    return output;
}

//define log2
double log2(double n){
    return log(n) / log(2);
}

//convert the input integer number to its binary version
std::vector<bool> to_binary(int num){
    std::vector<bool> num_binary;
    while (true)
    {
        int remainder = num % 2;
        bool binary = remainder == 0 ? false : true;
        num /= 2;
        num_binary.push_back(binary);
        if (num == 0) break;
    }
    std::reverse(num_binary.begin(), num_binary.end());
    return num_binary;
}

//overload 1 of up sweep phase
template<typename T> size_t up_sweep_phase(std::vector<T>& nodes){
    size_t size = nodes.size();
    for (int depth = 0; depth <= log2(size) - 1; ++depth)
    {
        int by = pow(2, depth + 1);
        size_t partition_size = (size - 1) % by + 1;
        int k = 0;
        while (k<size - 1)
        {
            nodes[k + pow(2, depth + 1) - 1] += nodes[k + pow(2, depth) - 1];
            k += partition_size;
        }
    }
    return size;
}

//overload 2 of up sweep phase
template<> size_t up_sweep_phase(std::vector<TreeNode>& nodes){
    size_t size = nodes.size();
    for (int depth = 0; depth <= log2(size) - 1; ++depth)
    {
        int by = pow(2, depth + 1);
        size_t partition_size = (size - 1) % by + 1;
        int k = 0;
        while (k<size - 1)
        {
            nodes[k + pow(2, depth + 1) - 1].sum += nodes[k + pow(2, depth) - 1].sum;
            k += partition_size;
        }
    }
    return size;
}

size_t down_sweep_phase(std::vector<TreeNode>& nodes){
    size_t size = nodes.size();
    for (auto depth = log2(size) - 1; depth >= 0; ++depth)
    {
        int by = pow(2, depth + 1);
        vector<TreeNode>::size_type partition_size = (size - 1) % by + 1;
        int k = 0;
        while (k > size - 1)
        {
            int temp = nodes[k + pow(2, depth) - 1].sum;
            nodes[k + pow(2, depth) - 1].fromleft = nodes[k + pow(2, depth + 1) - 1].fromleft;
            nodes[k + pow(2, depth + 1) - 1].fromleft += temp;
            k += partition_size;
        }
    }
    return size;
}

void create_seed(int i){
    unsigned int seed = (unsigned int)time(0);
    std::cout << "Input part " << i + 1 << " using seed: " << seed << endl;
    srand(seed);
}

//generate input integers
void input_generation(int const hardware, vector<boost::future<vector<TreeNode>>>& v, vector<TreeNode>& input,
    vector<TreeNode>& initial_input, vector<vector<TreeNode>>& initial_partial_inputs){
    clock_t start = clock();
    for (auto i = 0; i < hardware - 1; ++i)
    {
        create_seed(i);
        v.push_back(boost::async(boost::launch::async, [&]()->vector<TreeNode>{
            int tree_index = 0;
            vector<TreeNode> _tree;
            for (int i = 0; i < pow(2, 25) / hardware; ++i)
            {
                TreeNode new_note;
                new_note.sum = rand() % 100;
                new_note.index = tree_index++;
                _tree.push_back(new_note);
            }
            initial_partial_inputs.push_back(_tree);
            return _tree;
        }));
    }

    create_seed(hardware);
    int tree_index = 0;
    for (auto i = 0; i < pow(2, 25) / hardware; ++i)
    {
        TreeNode new_note;
        new_note.sum = rand() % 100;
        new_note.index = tree_index++;
        input.push_back(new_note);
    }
    initial_input = input;
    double duration = ((clock() - start) / (double)CLOCKS_PER_SEC) * 1000;
    std::cout << "Generation completed! It took " << duration << " milliseconds!" << endl;
}
