#include "fastsgs.h"
#include <stdlib.h>
#include "time.h"
#include "multirandom.h"
#include "repeatedgreedy.h"
int main(int argc,char *argv[]) {

    //double B=atof(argv[1]);
    //double eps=atof(argv[2]);
    double eps=atof(argv[1]);
    cout<<"eps: "<<eps<<endl;

    string filename = "movies_1793.txt";
    dataset *da = new dataset(filename);

    time_t nowtime;
    struct tm* p;;
    time(&nowtime);
    p = localtime(&nowtime);
    string outtext="./result/movie_result_"+to_string((int)ave_num)+"_"+to_string(p->tm_mon+1)+"."+to_string(p->tm_mday)+"_"+to_string(p->tm_hour)+"_"+to_string(p->tm_min)+"_"+to_string(p->tm_sec)+".txt";

    vector<pair<double,long long int>> twingreedy_result;
    vector<pair<double,long long int>> fastsgs_result;
    vector<pair<double,long long int>> multirandom_result;
    vector<pair<double,long long int>> multirandom_acc_result;
    vector<pair<double,long long int>> repeatedgreedy_result;

    int m_start=8;
    int m_end=26;
    int m_step=2;
    lambda_f=1.0;

    //test value
    /*
    vector<int> t1={1464 ,1378 ,850 ,867 ,1376};
    cout<<f_S(t1)<<endl;
    double sum_cost=0.0;

    vector<int> t2={1464 ,1378 ,850 ,867 };
    cout<<f_S(t2)+marginal_gain(1376,t2)<<endl;

    vector<int> t3={1026 ,1651 ,812 ,989 ,1414};
    cout<<f_S(t3)<<endl;
//*/

    //test
    /*
    int node_num=da->index.size();
    cout<<"f(u)"<<endl;
    double sum_density=0.0;
    for(int iter=0;iter<node_num;iter++)
    {
        cout<<f_u(iter,node_num)<<endl;
    }
    cout<<"f(u)/cost"<<endl;
    for(int iter=0;iter<node_num;iter++)
    {
        cout<<f_u(iter,node_num)/da->rate_costs[iter]<<endl;
        sum_density+=f_u(iter,node_num)/da->rate_costs[iter];
    }
    sum_density/=node_num;
    cout<<"ave density:"<<sum_density<<endl;
    //*/
    for(int m=m_start;m<=m_end;m+=m_step)
    {
        multirandom_acc_result.push_back(multi_random_acc(eps,da,m));
        //multirandom_result.push_back(multi_random(eps,da,m));
        twingreedy_result.push_back(twingreedy(eps,da,m));
        fastsgs_result.push_back(fastsgs(eps,da,m));
        repeatedgreedy_result.push_back(repeatedgreedy(da,m));
    }
    ofstream out(outtext);
    out<<"eps: "<<eps<<" lambda_f:"<<lambda_f<<" lambda:"<<da->lambda<<" genres limit:"<<genres_limit<<endl;
    out<<"m: "<<endl;
    for(int m=m_start;m<=m_end;m+=m_step)
    {
        out<<m<<"\t";
    }
    out<<endl;

    out<<"MultiRandomAcc "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:multirandom_acc_result)
    {
        out<<p.first<<"\t";
    }
    out<<endl;
    out<<"oracle times: "<<endl;
    for(auto p:multirandom_acc_result)
    {
        out<<p.second<<"\t";
    }
    out<<endl;
/*
    out<<"MultiRandom "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:multirandom_result)
    {
        out<<p.first<<"\t";
    }
    out<<endl;
    out<<"oracle times: "<<endl;
    for(auto p:multirandom_result)
    {
        out<<p.second<<"\t";
    }
    out<<endl;
*/
    out<<"Repeated Greedy "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:repeatedgreedy_result)
    {
        out<<p.first<<"\t";
    }
    out<<endl;
    out<<"oracle times: "<<endl;
    for(auto p:repeatedgreedy_result)
    {
        out<<p.second<<"\t";
    }
    out<<endl;

    out<<"twingreedy "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:twingreedy_result)
    {
        out<<p.first<<"\t";
    }
    out<<endl;
    out<<"oracle times: "<<endl;
    for(auto p:twingreedy_result)
    {
        out<<p.second<<"\t";
    }
    out<<endl;

    out<<"fastsgs "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:fastsgs_result)
    {
        out<<p.first<<"\t";
    }
    out<<endl;
    out<<"oracle times: "<<endl;
    for(auto p:fastsgs_result)
    {
        out<<p.second<<"\t";
    }
    out<<endl;

    return 0;
}
