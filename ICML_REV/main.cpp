#include "twingreedy.h"
#include "fastsgs.h"
#include "multirandom.h"
#include "adaptive.h"
#include <stdlib.h>
#include "time.h"
#include "repeatedgreedy.h"
int main(int argc,char *argv[]) {
    //random_weight();
    //return 0;
    //random_cost();
    //renum();
    //cumulate_normalize_cost(0.2);

    time_t nowtime;
    struct tm* p;;
    time(&nowtime);
    p = localtime(&nowtime);

    //read_data_test();
    read_data_1();
    string::size_type pos1, pos2,posend;
    pos1=node_cost_text.find_last_of("/");
    pos2=node_cost_text.rfind("/",pos1-1);

    posend=node_cost_text.find_last_not_of("/");
    string name1=node_cost_text.substr(pos2+1,pos1-pos2-1);
    string name2=node_cost_text.substr(pos1+1,posend);
    string result_name=name1+"_"+name2;
    //cout<<result_name<<endl;
    string outtext="./result/result_"+result_name+"_"+to_string(p->tm_mon+1)+"."+to_string(p->tm_mday)+"_"+to_string(p->tm_hour)+"_"+to_string(p->tm_min)+"_"+to_string(p->tm_sec)+".txt";

    double eps=atof(argv[1]);
    //double B=atof(argv[1]);
    //double eps=atof(argv[2]);
    cout<<"eps: "<<eps<<endl;
/*
    S_class S;
    S.selected[3][11]=1;
    S.selected[1][249]=1;
    S.selected[2][249]=1;
    S.selected[2][341]=1;
    S.selected[1][341]=1;

    S.selected[4][106]=1;
    S.selected[0][11]=1;
    S.selected[3][10]=1;
    S.selected[4][42]=1;
    S.selected[0][106]=1;

    //cout<<S.f_S()<<endl;
    cout<<"f(S): "<<S.f_S()<<endl;
    return 0;
    //*/
/*
    S_class S;
    cout<<"f(0,1) "<<f_u(pair<int,int>(0,1))<<endl;
    S.selected[0][1]=1;
    cout<<"f(0,1),S: "<<S.f_S()<<endl;
    cout<<"f(0,1)(0,5),S: "<<S.marginal(pair<int,int>(0,5))<<endl;
    S.selected[0][5]=1;
    cout<<"f(0,1)(0,5),S: "<<S.f_S()<<endl;

    cout<<"f(0,1)(2,1)(0,5) "<<S.marginal(pair<int,int>(2,1))<<endl;
    cout<<"f(0,1)(2,1)(0,5) "<<S.f_S()+f_u(pair<int,int>(2,1))<<endl;

    S.selected[2][1]=1;
    cout<<"f(0,1)(2,1)(0,5) "<<S.f_S()<<endl;
    return 0;
    //*/

    vector<pair<pair<double,double>,long long int>> twingreedy_result;
    vector<pair<pair<double,double>,long long int>> fastsgs_result;
    vector<pair<double,long long int>> multirandom_result;
    vector<pair<pair<double,double>,long long int>> multirandom_acc_result;
    vector<pair<double,long long int>> adaptive_result;
    vector<pair<pair<double,double>,long long int>> repeatedgreedy_result;

    for(int m=m_start;m<=m_end;m+=m_step)
    {
        multirandom_acc_result.push_back(multi_random_acc(eps,m));
        //multirandom_result.push_back(multi_random(eps,m));
        twingreedy_result.push_back(twingreedy(eps,m));
        fastsgs_result.push_back(fastsgs(eps,m));
        repeatedgreedy_result.push_back(repeatedgreedy(m));
        adaptive_result.push_back(adaptive_cycle(eps,m));

    }

    ofstream out(outtext);
    out<<"eps: "<<eps<<endl;
    out<<"m: "<<endl;
    for(int m=m_start;m<=m_end;m+=m_step)
    {
        out<<m<<"\t";
    }
    out<<endl;

    out<<"adaptive "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:adaptive_result)
    {
        out<<p.first<<"\t";
    }
    out<<endl;
    out<<"oracle times: "<<endl;
    for(auto p:adaptive_result)
    {
        out<<p.second<<"\t";
    }
    out<<endl;

    out<<"MultiRandomAcc "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:multirandom_acc_result)
    {
        out<<p.first.first<<"\t";
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
    out<<"Repeated Greedy"<<endl;
    out<<"revenue: "<<endl;
    for(auto p:repeatedgreedy_result)
    {
        out<<p.first.first<<"\t";
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
        out<<p.first.first<<"\t";
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
        out<<p.first.first<<"\t";
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
