//
// Created by CLAKERS on 2021/1/23.
//

#ifndef ICML_REV_UTILITY_H
#define ICML_REV_UTILITY_H
#include "generate_random_data.h"
#include <vector>
#include "list"
#include "random"
int node_num=0;
int edge_num=0;
//vector<double> product_ratio={0.8,0.9,1.0,1.1,1.2};
vector<double> product_ratio={1,1,1,1,1};
//vector<double> product_ratio={1,1,1};
//vector<double> product_ratio={1,1,1,1,1,1,1,1,1,1};
const int product_types=5;//product types
const int per_user_max=3;
const int k=2;
const double edge_weight_expectation=0.5;

const int realization=20;
//[node_i,array stored all node linked to node_i]->[array all node linked to node_i,pair<linked nodes index,weight>]
vector<vector<pair<int,double>>> edge_weight;
//[node_i,array stored all node linked to node_i]->[array all node linked to node_i,pair<linked nodes index,weight on all realization(vector<double>)>]
vector<vector<pair<int,vector<double>>>> edge_weight_realization;
vector<double> node_ratio;//ratio for every ratio, expectation or realization only for adaptive
vector<vector<double>> node_ratio_realization;//[user,ratio of NO.realization] realization ratio for every ratio, read from txt

vector<double> node_cost;//cost for every node
double min_cost=999999999;
int K;
string node_ratio_text="./rndata/lastfm/node_ratio.txt";
string edge_weight_text="./rndata/lastfm/renum_random_edge_weight.txt";

string node_cost_text="./rndata/r1/renum_random_node_cost.txt";
string node_text="./rndata/r2/node.txt";
vector<double> self_weight;
void read_data_2()
{
    ifstream in1(node_text);
    int temp_node;
    //double temp_cost;
    node_num=0;
    while(!in1.eof())
    {
        in1>>temp_node;
        if (in1.fail())
            break;
        node_num++;
    }
    in1.close();

    for(int i=0;i<node_num;i++) {
        edge_weight.push_back(vector<pair<int, double>>());
        edge_weight_realization.push_back(vector<pair<int,vector<double>>>());

        node_ratio.push_back(1.0);//all node ratios initialized as expectation
    }
    ifstream in2(edge_weight_text);
    int v1,v2;
    double weight;
    edge_num=0;
    while(!in2.eof())
    {
        in2>>v1>>v2;
        if (in2.fail())
            break;
        edge_num++;
        edge_weight[v1].push_back(pair<int,double>(v2,edge_weight_expectation));
        edge_weight[v2].push_back(pair<int,double>(v1,edge_weight_expectation));

        edge_weight_realization[v1].push_back(make_pair(v2,vector<double>()));
        edge_weight_realization[v2].push_back(make_pair(v1,vector<double>()));
        for(int r=0;r<realization;r++)
        {
            in2>>weight;
            edge_weight_realization[v1].back().second.push_back(weight);
            edge_weight_realization[v2].back().second.push_back(weight);
        }
    }
    in2.close();
    cout<<"node size: "<<node_num<<" map size: "<<edge_weight.size()<<endl;
    cout<<"edge size: "<<edge_num<<endl;
    //for(auto r:edge_weight_realization[1028].back().second)
    //    cout<<r<<"\t";
    //cout<<endl;
    /*
    for(auto q:edge_weight_realization[1028])
    {
        cout<<q.first<<endl;
        for(auto r:q.second)
            cout<<r<<"\t";
        cout<<endl;
    }*/
}
void read_data_0()
{
    ifstream in1(node_cost_text);
    int temp_node;
    double temp_cost;
    node_num=0;
    double node_ratio_temp;
    while(!in1.eof())
    {
        in1>>temp_node>>temp_cost;
        if (in1.fail())
            break;
        node_num++;
    }
    in1.close();

    for(int i=0;i<node_num;i++) {
        edge_weight.push_back(vector<pair<int, double>>());
        node_ratio.push_back(1.0);//all node ratios initialized as expectation
    }
    ifstream in2(edge_weight_text);
    int v1,v2;
    double weight;
    edge_num=0;
    while(!in2.eof())
    {
        in2>>v1>>v2>>weight;
        if (in2.fail())
            break;
        edge_num++;
        edge_weight[v1].push_back(pair<int,double>(v2,weight));
        edge_weight[v2].push_back(pair<int,double>(v1,weight));
    }
    in2.close();
    cout<<"node size: "<<node_num<<" map size: "<<node_ratio_realization.size()<<endl;
    cout<<"edge size: "<<edge_num<<endl;
}
void read_data_1()
{
    ifstream in1(node_ratio_text);
    int temp_node;
    //double temp_cost;
    node_num=0;
    double node_ratio_temp;
    while(!in1.eof())
    {
        in1>>temp_node;
        if (in1.fail())
            break;
        node_ratio_realization.push_back(vector<double>());
        for(int r=0;r<realization;r++)
        {
            in1>>node_ratio_temp;
            node_ratio_realization[node_num].push_back(node_ratio_temp);
        }
        node_num++;
    }
    in1.close();

    //default_random_engine ee(1);
    //uniform_real_distribution<double> uu(0,1);

    for(int i=0;i<node_num;i++) {
        edge_weight.push_back(vector<pair<int, double>>());
        //only test
        //self_weight.push_back(uu(ee));

        node_ratio.push_back(1.0);//all node ratios initialized as expectation
    }
    ifstream in2(edge_weight_text);
    int v1,v2;
    double weight;
    edge_num=0;
    while(!in2.eof())
    {
        in2>>v1>>v2>>weight;
        if (in2.fail())
            break;
        edge_num++;
        //edge_weight[v1].push_back(pair<int,double>(v2,weight));
        edge_weight[v2].push_back(pair<int,double>(v1,weight));
    }
    in2.close();
    cout<<"node size: "<<node_num<<" map size: "<<node_ratio_realization.size()<<endl;
    cout<<"edge size: "<<edge_num<<endl;
}
//fixed
double f_u(pair<int,int> node)
{
    double sum_value=0.0;
    for(int i=0;i<node_num;i++)
    {
        if(i!=node.second)
        {
            double value=0.0;
            for(int j=0;j<edge_weight[i].size();j++)
            {
                if(edge_weight[i][j].first==node.second)
                {
                    value=edge_weight[i][j].second;
                    break;
                }
            }
            //////only test
            //value+=self_weight[i];
            sum_value+=node_ratio[i]*sqrt(value);
        }
    }
    return sum_value*product_ratio[node.first];
}
//node(product,user)
class S_class {
public:
    S_class() {
        max_marginal=-999999999.0;
        max_element.first=-1;
        max_element.second=-1;

        S_revenue = 0.0;
        //selected.resize(node_num, 0);
        for(int i=0;i<product_types;i++) {
            selected.push_back(vector<int>(node_num, 0));
            per_product_used.push_back(0);
        }

        //optimize
        for(int q=0;q<product_types;q++) {
            marginal_store.push_back(vector<pair<int,double>>());
            for(int i=0;i<node_num;i++)
            {
                marginal_store[q].push_back(make_pair(0,-999999.0));
            }
        }
    }

    vector<pair<int,int>> Set;
    double S_revenue;
    vector<vector<int>> selected;
    vector<int> per_product_used;//how many products used

    double max_marginal;
    pair<int,int> max_element;
    //optimize
    vector<vector<pair<int,double>>> marginal_store;

    bool is_feasible(pair<int,int>node,int per_product_max)
    {
        if(per_product_used[node.first]>=per_product_max)//matroid 1
            return false;

        int user_get_product_nums=0;//matroid 2
        for(int q=0;q<product_types;q++)
        {
            if(selected[q][node.second]==1)
                user_get_product_nums++;
        }
        if(user_get_product_nums>=per_user_max)
            return false;

        return true;

    }
    //fixed
    double f_S() {
        double sum_value = 0.0;
        for(int q=0;q<product_types;q++)//f(S)=\sum f^q(S^q)
        {
            double sum_product_value=0.0;
            for (int i = 0; i < node_num; i++)//f^q(S^q)
            {
                if (selected[q][i] == 0) {
                    double value = 0.0;
                    for (int j = 0; j < edge_weight[i].size(); j++) {
                        if (selected[q][edge_weight[i][j].first] == 1) {
                            value += edge_weight[i][j].second;
                        }
                    }
                    ////only test
                    //value+=self_weight[i];
                    sum_product_value += node_ratio[i]*sqrt(value);//v_i()
                }
            }
            sum_value+=sum_product_value*product_ratio[q];//v^q()
        }
        return sum_value;
    }
    ///fixed
    double realf_S(int realization_index) {
        double sum_value = 0.0;
        for(int q=0;q<product_types;q++)//f(S)=\sum f^q(S^q)
        {
            double sum_product_value=0.0;
            for (int i = 0; i < node_num; i++)//f^q(S^q)
            {
                if (selected[q][i] == 0) {
                    double value = 0.0;
                    for (int j = 0; j < edge_weight[i].size(); j++) {
                        if (selected[q][edge_weight[i][j].first] == 1) {
                            value += edge_weight[i][j].second;
                        }
                    }
                    ////only test
                    //value+=self_weight[i];
                    sum_product_value += node_ratio_realization[i][realization_index]*sqrt(value);//v_i()
                }
            }
            sum_value+=sum_product_value*product_ratio[q];//v^q()
        }
        return sum_value;
    }
    //fixed
    double marginal(pair<int,int> node) {
        //Set.push_back(node);
        if(selected[node.first][node.second]==1) return 0.0;

        selected[node.first][node.second]=1;
        double f_S_and_u = f_S();
        //Set.pop_back();
        selected[node.first][node.second]= 0;
        return f_S_and_u - S_revenue;
    }
    //fixed
    void observe(int user,int realization_index)
    {
        for (int j = 0; j < edge_weight[user].size(); j++)
        {
            node_ratio[edge_weight[user][j].first]=node_ratio_realization[edge_weight[user][j].first][realization_index];
        }
    }
    /*
    void copy(const S_class &temp) {
        S_revenue = temp.S_revenue;
        //S_cost = temp.S_cost;
        Set.assign(temp.Set.begin(), temp.Set.end());
        //S* need not maintain selected to calculate f(S)
        selected.assign(temp.selected.begin(),temp.selected.end());
    }*/
};
int m_start=1;
int m_end=10;
int m_step=1;
int length=(double)(m_end-m_start+1.0)/(double)m_step;
vector<vector<double>> multirandom_acc_realization(length,vector<double>());
vector<vector<double>> adaptive_realization(length,vector<double>());
vector<vector<double>> fastsgs_realization(length,vector<double>());
vector<vector<double>> twingreedy_realization(length,vector<double>());
vector<vector<double>> repeatedgreedy_realization(length,vector<double>());

#endif //ICML_REV_UTILITY_H
