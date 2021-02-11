//
// Created by CLAKERS on 2020/8/22.
//
#ifndef IMAGE_READ_DATA_H
#define IMAGE_READ_DATA_H
#define pi 3.1415926
#include <iostream>
#include "fstream"
#include "vector"
#include "random"
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sstream>
#include <climits>
#include <string>
#include <cmath>
#include <set>
using namespace std;

using namespace std;
const int node_num=3000;
const int m=3072;
const int category=3;
const int per_category_max=5;
const int k=1;
vector<vector<int>> feature;
vector<int> lable;
vector<double> contrast_cost(node_num,0.0);
vector<vector<double>> similarity;
int K;
double min_cost=999999999;
double max_cost=-999999999;
//string cost_text="./rdata/2/rd_contrast.txt";
//string feature_text="./rdata/2/rd_feature.txt";
string cost_text="./rdata/category/rd_contrast.txt";
string feature_text="./rdata/category/rd_feature.txt";
const double ave_num=10.0;
void read()
{
    feature.resize(node_num, vector<int>(m));
    ifstream in1(feature_text);
    //ifstream in1("feature.txt");
    int i=0;
    int templable=-1;
    while(!in1.eof())
    {
        in1>>templable;
        if (in1.fail())
            break;
        lable.push_back(templable);
        for(int j=0;j<m;j++)
            in1 >> feature[i][j];

        i++;
    }
    in1.close();

    cout<<"lable num: "<<lable.size()<<" | "<<i<<endl;

    ifstream in2(cost_text);
    //ifstream in2("contrast.txt");
    i=0;
    double temp_cost;
    double sum_cost=0.0;
    while(!in2.eof())
    {
        in2>>temp_cost;
        if (in2.fail())
            break;

        if(temp_cost<min_cost)
            min_cost=temp_cost;
        if(temp_cost>max_cost)
            max_cost=temp_cost;

        contrast_cost[i]=temp_cost;
        sum_cost+=temp_cost;
        i++;
        //cout<<temp_cost<<endl;
    }
    in2.close();

    //cost normalize
    //cout<<"normalize: "<<endl;
    //double ave_cost=sum_cost/node_num;
    //*
    for(auto &x:contrast_cost)
    {
        x=(node_num*x)/(sum_cost*ave_num);
        //cout<<x<<endl;
    }
    //*/

}
void cal_similarity()
{
    similarity.resize(node_num, vector<double>(node_num));
    //inner product
    /*
    for(int i = 0;i < feature.size();i++)
    {
        for(int j = i;j < feature.size();j++)
        {
            double product = 0.0;
            for(int k = 0;k < feature[0].size();k++)
            {
                product+=(feature[i][k]*feature[j][k]);
            }
            similarity[i][j] = product;
            similarity[j][i] = product;
        }
    }
     */
    //distance
    /*
    for(int i = 0;i < feature.size();i++)
    {
        for(int j = i;j < feature.size();j++)
        {
            double distance = 0.0;
            for(int k = 0;k < feature[0].size();k++)
            {
                distance += pow((feature[i][k]-feature[j][k]),2);
            }
            distance = sqrt(distance);
            similarity[i][j] = distance;
            similarity[j][i] = distance;
        }
    }
     //*/
    //consine
    //*
    for(int i = 0;i < feature.size();i++)
    {
        for(int j = i;j < feature.size();j++)
        {
            double product = 0.0;
            double module_a=0.0;
            double module_b=0.0;
            for(int k = 0;k < feature[0].size();k++)
            {
                product+=(feature[i][k]*feature[j][k]);
                module_a+=pow(feature[i][k],2);
                module_b+=pow(feature[j][k],2);
            }
            module_a=sqrt(module_a);
            module_b=sqrt(module_b);
            similarity[i][j] = product/(module_a*module_b);
            similarity[j][i] = product/(module_a*module_b);
        }
    }
    // */
}
double f_u(int node)
{
    double sum_value=0.0;
    for(int i=0;i<node_num;i++)
    {
        sum_value += similarity[i][node];
    }
    return sum_value-similarity[node][node]/(node_num);
}
class S_gamma
{
public:
    S_gamma()
    {
        max_marginal=-999999999.0;
        max_element=-1;
        S_cost=0.0;
        S_revenue=0.0;
        max_similarity.resize(node_num,0.0);
        for(int iter=0;iter<category;iter++)
            category_sum.push_back(0);

        for(int i=0;i<node_num;i++)
        {
            marginal_store.push_back(make_pair(0,-999999.0));
        }

    }
    vector<int> category_sum;
    vector<int> Set;
    double max_marginal;
    int max_element;
    double S_cost;
    double S_revenue;
    vector<double> max_similarity;

    //optimize
    vector<pair<int,double>> marginal_store;
    bool is_feasible(int e,int max_image)
    {
        if(Set.size()>=max_image||category_sum[lable[e]]>=per_category_max)
            return false;
        else
            return true;
    }
    double f_S()
    {
        double v1=0.0;
        double v2=0.0;
        for(int i=0;i<node_num;i++)
        {
            double max_temp=-999999999;
            for(int j=0;j<Set.size();j++)
            {
                if(similarity[i][Set[j]]>max_temp)
                    max_temp=similarity[i][Set[j]];
            }
            v1+=max_temp;
        }
        for(int i=0;i<Set.size();i++)
        {
            for(int j=0;j<Set.size();j++)
            {
                v2+=similarity[Set[i]][Set[j]];
            }
        }
        return v1-v2/(node_num);
    }
    double marginal(int e)
    {
        if(Set.empty())
        {
            return f_u(e);
        }
        //slow
        double v1=0.0;
        double v2=0.0;

        for(int i=0;i<Set.size();i++)
        {
            if(e==Set[i]) return 0.0;

            v2+=similarity[e][Set[i]];
            v2+=similarity[Set[i]][e];
        }
        v2+=similarity[e][e];

        for(int i=0;i<node_num;i++)
        {
            double max_similarity=-999999999;
            for(auto j:Set)
            {
                if(similarity[i][j]>max_similarity)
                    max_similarity=similarity[i][j];
            }

            if(similarity[i][e]>max_similarity)
            {
                v1+=(similarity[i][e]-max_similarity);
            }
        }

        return v1-v2/(node_num);
    }
    double S_sub_u(int e)
    {
        double v1=0.0;
        double v2=0.0;
        for(int i=0;i<node_num;i++)
        {
            double max_temp=-999999999;
            for(int j=0;j<Set.size();j++)
            {
                if(Set[j]==e)
                    continue;
                if(similarity[i][Set[j]]>max_temp)
                    max_temp=similarity[i][Set[j]];
            }
            v1+=max_temp;
        }
        for(int i=0;i<Set.size();i++)
        {
            if(Set[i]==e)
                continue;
            for(int j=0;j<Set.size();j++)
            {
                if(Set[j]==e)
                    continue;
                v2+=similarity[Set[i]][Set[j]];
            }
        }
        return v1-v2/(node_num);
    }
    void copy(const S_gamma &temp)
    {
        S_revenue=temp.S_revenue;
        S_cost=temp.S_cost;
        Set.assign(temp.Set.begin(),temp.Set.end());
        //S* need not maintain max_similarity to calculate f(S)
        max_similarity.assign(temp.max_similarity.begin(),temp.max_similarity.end());
    }
};
#endif //IMAGE_READ_DATA_H
