//
// Created by amax on 2020/7/29.
//
#ifndef REVMAX_GENERATE_RANDOM_DATA_H
#define REVMAX_GENERATE_RANDOM_DATA_H

#include <iostream>
#include <fstream>
#include "random"
#include "time.h"
#include "set"
using namespace std;
void renum()
{
    ifstream in1("community_random_selected_node.txt");
    int node;
    vector<int> new_num(651140+1,-1);
    int num=0;
    set<int> V;
    while(!in1.eof())
    {
        in1>>node;
        if (in1.fail())
            break;
        new_num[node]=num;
        num++;
        V.insert(node);
    }
    cout<<"node size: "<<V.size()<<endl;

    ifstream in2("community_random_selected_edge_final.txt");
    int v1,v2;
    vector<vector<int>> new_edge;
    for(int i=0;i<V.size();i++)
        new_edge.push_back(vector<int>());
    while(!in2.eof())
    {
        in2>>v1>>v2;
        if (in2.fail())
            break;
        new_edge[new_num[v1]].push_back(new_num[v2]);
    }
    ofstream out1("renum_random_node.txt");
    for(int i=0;i<V.size();i++)
        out1<<i<<endl;
    ofstream out2("renum_random_edge.txt");
    cout<<"edge set size: "<<new_edge.size()<<endl;
    for(int i=0;i<new_edge.size();i++)
    {
        for(int j=0;j<new_edge[i].size();j++)
        {
            out2<<i<<"\t"<<new_edge[i][j]<<endl;
        }
    }
    in1.close();
    in2.close();
    out1.close();
    out2.close();
}
void random_weight()
{
    ifstream in("renum_random_edge.txt");
    default_random_engine e((unsigned)time(NULL));
    uniform_real_distribution<double> u(0.0, 1.0);
    int v1,v2;
    ofstream out("renum_random_edge_weight.txt");
    while(!in.eof())
    {
        in>>v1>>v2;
        if (in.fail())
            break;
        out<<v1<<'\t'<<v2;
        for(int q=0;q<10;q++)
            out<<'\t'<<u(e);
        out<<endl;
    }
    in.close();
}
void random_cost()
{
    ifstream in("renum_random_node.txt");
    default_random_engine e(1234);
    uniform_real_distribution<double> u(0.0, 1.0);
    int node;
    ofstream out("renum_random_node_cost.txt");
    while(!in.eof())
    {
        in>>node;
        if (in.fail())
            break;
        out<<node<<'\t'<<u(e)<<endl;
    }
    in.close();
}
void cumulate_normalize_cost(double lambda)
{
    /*
    ifstream in1("renum_random_node_cost.txt");
    int temp_node;
    double temp_cost;
    while(!in1.eof())
    {
        in1>>temp_node>>temp_cost;
        node_cost.push_back(temp_cost);
        if (in1.fail())
            break;
    }*/

    int node_num=910;
    vector<double> node_cost;
    vector<vector<pair<int,double>>> edge_weight;
    for(int i=0;i<node_num;i++)
        edge_weight.push_back(vector<pair<int,double>>());
    ifstream in2("renum_random_edge_weight.txt");
    int v1,v2;
    double weight;
    while(!in2.eof())
    {
        in2>>v1>>v2>>weight;
        edge_weight[v1].push_back(pair<int,double>(v2,weight));
        edge_weight[v2].push_back(pair<int,double>(v1,weight));
        if (in2.fail())
            break;
    }
    in2.close();
    ofstream out("renum_cumulate_normalize_node_cost.txt");
    for(int i=0;i<node_num;i++)
    {
        double sum_weight=0.0;
        for(int j=0;j<edge_weight[i].size();j++)
            sum_weight+=edge_weight[i][j].second;
        double cost=sqrt(sum_weight);
        double norm_cost=1-exp(-lambda*cost);
        out<<i<<"\t"<<norm_cost<<endl;
    }
    out.close();
}

#endif //REVMAX_GENERATE_RANDOM_DATA_H
