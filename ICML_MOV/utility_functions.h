
#ifndef STREAMING_ALGORITHM_UTILITY_FUNCTIONS_H
#define STREAMING_ALGORITHM_UTILITY_FUNCTIONS_H
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sstream>
#include <climits>
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include "list"
#include "random"
#define pi 3.1415926
using namespace std;
//Adventure Fantasy Animation
vector<float> sim_mat_sum;
vector<vector<float>> sim_mat;
vector<vector<int>> genres_index;//Adventure Fantasy Animation (1,1,1)
const int genres_limit=10;
int node_num=-1;

double min_cost=999999999;
int K;
const int k=3;
double lambda_f;
const double ave_num=10.0;
class dataset{
public:
    vector<int> index;
    set<int> v;
    vector<vector<float>> matrix;
    vector<vector<string>> genres;
    vector<string> titles;
    float lambda;
    vector<int> year;
    vector<float> rating;
    vector<float> year_costs;
    vector<float> rate_costs;
    //vector<vector<float>> sim_mat;
    dataset(string);
    void cost_normalize();
    void readfile(string filename);
    vector<float> feature_extractor(string features,char delim);
    vector<string> genre_extractor(string genres,char delim);
    void sim_mat_generator();
};
dataset::dataset(string filename)
{
    readfile(filename);
    cost_normalize();
    sim_mat_generator();

    node_num=index.size();
    sim_mat_sum = vector<float>(node_num,0.0);
    //for(int it=0;it < node_num;it++)
        //genres_index.push_back(vector<int>(k,0));
    for(int it=0;it < node_num;it++)
    {
        genres_index.push_back(vector<int>(k,0));
        for(auto p: genres[it])
        {
            if(p=="Adventure")
                genres_index[it][0]=1;
            if(p=="Fantasy")
                genres_index[it][1]=1;
            if(p=="Animation")
                genres_index[it][2]=1;
        }
        /*
        cout<<it<<"\t";
        for(auto p:genres_index[it])
            cout<<p<<"\t";
        cout<<endl;
*/
        for (int itv = 0; itv < node_num; itv++) {
            sim_mat_sum[it] += sim_mat[it][itv];
            //cout<<sim_mat_sum[it]<<endl;
        }
    }

}

void dataset::cost_normalize(){
    float rate = rating.size()/ave_num;
    float rate_coef,year_coef;
    float rate_sum=0.0;
    float year_sum=0.0;
    float rate_base = 10.0,year_base = 1985.0;
    for(int i = 0;i < rating.size();i++){
        rate_sum += rating.at(i);
        year_sum += abs(year.at(i)-year_base);
    }
    rate_sum = rating.size()*rate_base - rate_sum;
    //year_sum = rating.size()*year_base;
    rate_coef = rate/rate_sum;
    year_coef = rate/year_sum;

    //normalization
    //*
    for(int i = 0;i < rating.size();i++){
        rate_costs.push_back(fabs(rating.at(i)-rate_base)*rate_coef);
        year_costs.push_back(fabs(year.at(i)-year_base)*year_coef);
    }
    //*/

    for(int i=0;i<rate_costs.size();i++)
    {
        if(rate_costs[i]<min_cost)
            min_cost=rate_costs[i];
    }
}


void dataset::readfile(string filename){
    //dataset* data = new dataset();
    ifstream infofile;
    infofile.open(filename.c_str(), ios::in);
    int count = 0;
    char delim = '|';
    if (infofile.is_open()) {
        while (!infofile.eof()) {
            std::string line;
            getline(infofile, line);
            if (line.empty())
                continue;
            std::string::size_type pos = line.find_first_of(delim);
            v.insert(count++);
            int prevpos = 0;
            int length = line.length();
            // id
            string str = line.substr(prevpos, pos - prevpos);
            index.push_back(atoi(str.c_str()));
            // titles
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            titles.push_back(str);
            // genres
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            genres.push_back(genre_extractor(str,' '));
            // lambda
            lambda = 0.2;
            //features
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            matrix.push_back(feature_extractor(str,' '));

            // year
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            year.push_back(atoi(str.c_str()));

            // rating
            prevpos = line.find_first_not_of(delim, pos);
            pos = line.find_first_of(delim, prevpos);
            str = line.substr(prevpos, pos - prevpos);
            rating.push_back(atof(str.c_str()));
        }
    }
}

void dataset::sim_mat_generator(){ //计算Mij
    //e^-lambda*dist(v_i,v_j)
    //*
    sim_mat = vector<vector<float>>(index.size(),vector<float>(index.size(),0));
    for(int i = 0;i < index.size();i++)
    {
        for(int j = i;j < index.size();j++)
        {
            float distance = 0;
            float sim_dis = 0;
            for(int k = 0;k < matrix[0].size();k++)
            {
                distance += pow((matrix[i][k]-matrix[j][k]),2);
            }
            distance = sqrt(distance);
            sim_dis = exp(-1.0*lambda*distance);
            //cout<<sim_dis<<endl;
            sim_mat[i][j] = sim_dis;
            sim_mat[j][i] = sim_dis;
        }
    }
    //*/
    //inner product
    /*
    sim_mat = vector<vector<float>>(index.size(),vector<float>(index.size(),0));
    for(int i = 0;i < index.size();i++)
    {
        for(int j = i;j < index.size();j++)
        {
            float product = 0;
            //float sim_dis = 0;
            for(int k = 0;k < matrix[0].size();k++)
            {
                product+=(matrix[i][k]*matrix[j][k]);
                //distance += pow((matrix[i][k]-matrix[j][k]),2);
            }
            //distance = sqrt(distance);
            //sim_dis = exp(-1*lambda*distance);
            //cout<<product<<endl;
            sim_mat[i][j] = product;
            sim_mat[j][i] = product;
        }
    }
    //*/
    //consine
    /*
    sim_mat = vector<vector<float>>(index.size(),vector<float>(index.size(),0));
    for(int i = 0;i < index.size();i++)
    {
        for (int j = i; j < index.size(); j++)
        {
            double product = 0.0;
            double module_a=0.0;
            double module_b=0.0;
            for (int k = 0; k < matrix[0].size(); k++)
            {
                product += (matrix[i][k] * matrix[j][k]);
                module_a+=pow(matrix[i][k],2);
                module_b+=pow(matrix[j][k],2);
            }
            module_a=sqrt(module_a);
            module_b=sqrt(module_b);
            sim_mat[i][j] = product/(module_a*module_b);
            sim_mat[j][i] = product/(module_a*module_b);
        }
    }
    //*/
    //dist(v_i,v_j)
    /*
    sim_mat = vector<vector<float>>(index.size(),vector<float>(index.size(),0));
    for(int i = 0;i < index.size();i++)
    {
        for(int j = i;j < index.size();j++)
        {
            float distance = 0;
            for(int k = 0;k < matrix[0].size();k++)
            {
                distance += pow((matrix[i][k]-matrix[j][k]),2);
            }
            distance = sqrt(distance);
            sim_mat[i][j] = distance;
            sim_mat[j][i] = distance;
        }
    }
*/
}

vector<float> dataset::feature_extractor(string features, char delim){
    vector<float> str;
    std::string::size_type pos = features.find_first_of(delim);
    int prevpos = 0;
    while(string::npos != pos || string::npos != prevpos){
        str.push_back(atof(features.substr(prevpos,pos-prevpos).c_str()));
        prevpos = features.find_first_not_of(delim, pos);
        pos = features.find_first_of(delim, prevpos);
    }
    return str;
}

vector<string> dataset::genre_extractor(string genres, char delim) {
    vector<string> str;
    std::string::size_type pos = genres.find_first_of(delim);
    int prevpos = 0;
    while (string::npos != pos || string::npos != prevpos) {
        str.push_back(genres.substr(prevpos, pos - prevpos).c_str());
        prevpos = genres.find_first_not_of(delim, pos);
        pos = genres.find_first_of(delim, prevpos);
    }
    return str;
}
double f_u(int node,int node_num)
{
    double sum_value=sim_mat_sum[node];
    /*for(int i=0;i<node_num;i++)
    {
        sum_value += sim_mat[node][i];
    }*/
    return sum_value-lambda_f*sim_mat[node][node];
}
class S_gamma
{
public:
    S_gamma()
    {
        //sim_mat = simmat;
        //node_num = nodenum;
        max_marginal=-999999999.0;
        max_element=-1;
        S_cost=0.0;
        S_revenue=0.0;
        for(int iter=0;iter<k;iter++)
            genres_sum.push_back(0);
        //selected.resize(node_num,0);
        /*
        sim_mat_sum = vector<float>(nodenum,0);
        for(int it=0;it < nodenum;it++){
            for(int itv=0;itv < nodenum;itv++)
                sim_mat_sum[it] += sim_mat[it][itv];
        }*/
        for(int i=0;i<node_num;i++)
        {
            marginal_store.push_back(make_pair(0,-999999.0));
        }
    }
    vector<int> genres_sum;
    vector<int> Set;
    double max_marginal;
    int max_element;
    double S_cost;
    double S_revenue;
    //vector<int> selected;
    //int node_num;
    //vector<vector<float>> sim_mat;
    //vector<float> sim_mat_sum;
    //optimize
    vector<pair<int,double>> marginal_store;
    double f_S()
    {
        double sum_value=0.0;
        float M1 = 0,M2 = 0;
        for(int it=0;it < Set.size();it++){
            M1 += sim_mat_sum[Set[it]];
            for(int its=0;its<Set.size();its++)
                M2 += sim_mat[Set[it]][Set[its]];
        }
        sum_value = M1-lambda_f*M2;
        return sum_value;
    }
    bool is_feasible(int e,int max_movie)
    {
        if(Set.size()>=max_movie)
            return false;
        bool flag=true;
        for(int iter=0;iter<genres_index[e].size();iter++)
        {
            if(genres_index[e][iter]+genres_sum[iter]>genres_limit)
            {
                flag=false;
                break;
            }
        }
        return flag;
    }
    double marginal(int e)
    {
        //Set.push_back(node);
        //selected[node]=1;
        //double f_S_and_u=f_S();
        //Set.pop_back();
        //selected[node]=0;
        float M1=0,M2=0;
        M1 += sim_mat_sum[e];
        for(int it=0;it < Set.size();it++)
        {
            if(e==Set[it]) return 0;
            M2 += sim_mat[e][Set[it]];
            M2 += sim_mat[Set[it]][e];
        }
        M2 += sim_mat[e][e];
        return M1-lambda_f*M2;
    }
    /*
    double Sij_marginal(int e,int max_node_index)
    {
        //Set.push_back(node);
        //selected[node]=1;
        //double f_S_and_u=f_S();
        //Set.pop_back();
        //selected[node]=0;
        float M1=0,M2=0;
        M1 += sim_mat_sum[e];
        for(int it=0;it <= max_node_index;it++)
        {
            M2 += sim_mat[e][Set[it]];
            M2 += sim_mat[Set[it]][e];
        }
        M2 += sim_mat[e][e];
        return M1-M2;
    }*/
    double S_sub_u(int e)
    {
        double sum_value=0.0;
        float M1 = 0,M2 = 0;
        for(int it=0;it < Set.size();it++)
        {
            if(Set[it]==e)
                continue;
            M1 += sim_mat_sum[Set[it]];
            for(int its=0;its<Set.size();its++)
            {
                if(Set[its]==e)
                    continue;
                M2 += sim_mat[Set[it]][Set[its]];
            }
        }
        sum_value = M1-lambda_f*M2;
        return sum_value;
    }
    void copy(const S_gamma &temp)
    {
        S_revenue=temp.S_revenue;
        S_cost=temp.S_cost;
        Set.assign(temp.Set.begin(),temp.Set.end());
        //S* need not maintain selected and sim_mat to calculate f(S)
        //selected.assign(temp.selected.begin(),temp.selected.end());
    }
};


#endif //STREAMING_ALGORITHM_UTILITY_FUNCTIONS_H
