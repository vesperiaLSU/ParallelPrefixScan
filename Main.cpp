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

int main(){
    vector<TreeNode> initial_main_nodes;                    //store initial input generated from main thread
    vector<vector<TreeNode>> initial_partial_inputs;   //store the vector of initial inputs generated from each thread
    vector<TreeNode> main_nodes;                               //store tree nodes modified by main thread
    vector<TreeNode> total_nodes;                               //store vector of tree nodes from each thread after up-sweep phase
    vector<boost::future<vector<TreeNode>>> _v;        //store vector of tree nodes modified by each thread
    vector<boost::future<vector<TreeNode>>> _v2;      //store vector of tree nodes from each thread after up-sweep phase
    vector<boost::future<vector<TreeNode>>> _v3;      //store vector of tree nodes from each thread after down-sweep phase
    vector<boost::future<vector<int>>> v_ret;               //store vector of output results from each thread
    vector<int> result_output;                                        //store vector of final result after prefix sum
    int hardware_threads = boost::thread::hardware_concurrency();

    /////////input generation for main_nodes and or other partial_nodes////////////
    input_generation(hardware_threads, _v, main_nodes, initial_main_nodes, initial_partial_inputs);

    ///////////Up-Sweep phase/////////
    for (auto i = 0; i < _v.size(); ++i)
    {
        boost::future<vector<TreeNode>> f2 = _v[i].then([&](boost::future<vector<TreeNode>>&& f) ->vector<TreeNode>{
            vector<TreeNode> partial_nodes = f.get();
            size_t size = up_sweep_phase(partial_nodes);
            total_nodes.push_back(partial_nodes[size - 1]);
            return partial_nodes;
        });
        _v2.push_back(boost::move(f2));
    }

    size_t size = up_sweep_phase(main_nodes);
    total_nodes.push_back(main_nodes[size - 1]);

    ///////////prefix sum process for the results from all threads///////////
    size_t core_size = up_sweep_phase(total_nodes);
    total_nodes[core_size - 1].fromleft = 0;
    down_sweep_phase(total_nodes);

    ///////////Down-Sweep phase//////////
    for (auto i = 0; i < _v2.size(); ++i)
    {
        boost::future<vector<TreeNode>> f3 = _v2[i].then([&](boost::future<vector<TreeNode>>&& f)->vector<TreeNode>{
            vector<TreeNode> partial_nodes = f.get();
            size_t size = partial_nodes.size();
            partial_nodes[size - 1].fromleft = total_nodes[i].fromleft;
            down_sweep_phase(partial_nodes);
            return partial_nodes;
        });
        _v3.push_back(boost::move(f3));
    }

    main_nodes[size - 1].fromleft = total_nodes[core_size - 1].fromleft;
    down_sweep_phase(main_nodes);

    //////////////getting the final output//////////////
    for (int i = 0; i < _v3.size(); ++i)
    {
        boost::future<vector<int>> f4 = _v3[i].then([&](boost::future<vector<TreeNode>>&& f)->vector<int>{
            vector<TreeNode> partial_nodes = f.get();
            vector<int> partial_output;
            for (TreeNode node : partial_nodes){
                int result = node.fromleft + initial_partial_inputs[i][node.index].sum;
                partial_output.push_back(result);
            }
            return partial_output;
        });
        v_ret.push_back(boost::move(f4));
    }

    boost::future<string> f5 = boost::async(boost::launch::async, [&]()->string{
        for (int i = 0; i < v_ret.size(); ++i)
        {
            vector<int> result = v_ret[i].get();
            copy(result.begin(), result.end(), back_inserter(result_output));
        }
        return "ready!";
    });

    vector<int> main_output;
    for (TreeNode node : main_nodes){
        int result = node.fromleft + initial_main_nodes[node.index].sum;
        main_output.push_back(result);
    }

    f5.wait();
    copy(main_output.begin(), main_output.end(), back_inserter(result_output));   //getting final result here: result_output

    return 0;
}
