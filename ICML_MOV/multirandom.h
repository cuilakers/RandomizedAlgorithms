//
// Created by CLAKERS on 2021/1/21.
//

#ifndef ICML_MOV_MULTIRANDOM_H
#define ICML_MOV_MULTIRANDOM_H
#include "utility_functions.h"
#include "time.h"
#include "algorithm"
class Ai
{
public:
    Ai(int node_temp,int node_num)
    {
        node=node_temp;
        weight=f_u(node,node_num);
        l=1;
    }
    int node;
    double weight;
    int l;
};
bool cmp(const Ai &a,const Ai &b)
{
    return a.weight>b.weight;
}
pair<int,double> GetMax(const int max_movie,dataset* data,long long int &oracle_times,double eps,vector<Ai> &A,S_gamma &Si,const vector<int> &avaiable)
{
    double eta=0.1;
    double eps_apostrophe=eta*eps;
    for(auto e=A.begin();e!=A.end();)//remove unfeasible element
    {
        if((avaiable[(*e).node]==0)||(!Si.is_feasible((*e).node,max_movie)))
        {
            e=A.erase(e);
        }
        else
        {
            e++;
        }
    }
    //sort
    //*
    sort(A.begin(),A.end(),cmp);
    double m=0.0;//always record weight of aij
    int ai=-1;
    for(auto &u:A)
    {
        double wt=u.weight;
        if(m>=wt) break;
        //update weight
        oracle_times++;
        //synchronous_update[u.node]=1;
        u.weight=Si.marginal(u.node);
        if(u.weight>=(wt/(1+eps_apostrophe)))
        {
            ai=u.node;
            m=u.weight;
            break;
        }
        else
        {
            if(u.weight>m)
            {
                m=u.weight;
                ai=u.node;
            }
            u.l++;
        }
    }
    //if Aij is empty, then return nothing
    //if(aij==-1)
    //{
    //    return pair<int,double>(-1,0);
    //}
    for(auto e=A.begin();e!=A.end();)
    {
        if((*e).l>(log(max_movie/eps_apostrophe)/log(1.0+eps_apostrophe)))
        {
            e=A.erase(e);
        }
        else
        {
            e++;
        }
    }
    return pair<int,double>(ai,m);
    //*/
}
pair<double,long long int> multi_random_acc(double eps,dataset* data,int m)
{
    double p=2.0/(1.0+sqrt((float)k));
    //p=1.0;
    bernoulli_distribution u(p);
    default_random_engine e(1);
    //default_random_engine e((unsigned)time(NULL));

    int node_num=data->index.size();
    long long int oracle_times=0;
    vector<S_gamma> S;
    vector<vector<Ai>> A;
    int ell=2;

    for(int i=0;i<ell;i++) {
        S.push_back(S_gamma());
        A.push_back(vector<Ai>());
    }
    //initial Ai
    for(int i=0;i<node_num;i++)
    {
        oracle_times++;
        Ai temp(i,node_num);
        for(int j=0;j<ell;j++)
        {
            A[j].push_back(temp);
        }
    }
    vector<int> available(node_num,1);//mark S_i selected and discarded
    while(1)
    {
        for (int j = 0; j < S.size(); j++)
        {
            S[j].max_marginal=-999999999.0;
            S[j].max_element=-1;
            //A=empty, then return element=-1 and marginal gain =0
            pair<int,double> temp=GetMax(m,data,oracle_times,eps,A[j],S[j],available);
            S[j].max_marginal=temp.second;
            S[j].max_element=temp.first;
        }
        double max_marginal = 0.0;
        int max_element=-1;
        int max_solution = -1;
        for(int j=0;j<S.size();j++)
        {
            if (S[j].max_marginal > max_marginal) {
                max_solution=j;
                max_marginal=S[j].max_marginal;
                max_element=S[j].max_element;
            }
        }
        if (max_solution == -1||max_marginal<=0)//non element or marginal gain<=0
            break;
        //S<-S\cup {u}
        if(u(e)==1)
        {
            S[max_solution].Set.push_back(max_element);
            S[max_solution].S_revenue += max_marginal;
            for (int i = 0; i < S[max_solution].genres_sum.size(); i++)//S+e\in I
            {
                S[max_solution].genres_sum[i] += genres_index[max_element][i];
            }
        }
        available[max_element] = 0;//discarded or selected
    }

    S_gamma *S_star;
    double max_revenue=-999999999.0;

    cout<<"MultiRandomAcc & m: "<<m<<endl;
    for(int i=0;i<S.size();i++)
    {
        cout<<"S"<<i+1<<endl;
        cout<<"  revenue: "<<S[i].S_revenue<<" size: "<<S[i].Set.size()<<endl;
        cout<<"  all nodes: "<<endl;
        for(int j=0;j<S[i].Set.size();j++)
        {
            cout<<S[i].Set[j]<<" ";
        }
        cout<<endl;
        cout<<"  genre limit: "<<endl;
        for(int j=0;j<S[i].genres_sum.size();j++)
        {
            cout<<S[i].genres_sum[j]<<" ";
        }
        cout<<endl;
        if(S[i].S_revenue>max_revenue)
        {
            S_star=&S[i];
            max_revenue=S[i].S_revenue;
        }
    }
    cout<<"S*:"<<endl;
    cout<<"  revenue: "<<(*S_star).S_revenue<<" size: "<<(*S_star).Set.size()<<endl;
    cout<<"  all nodes: "<<endl;
    for(int i=0;i<(*S_star).Set.size();i++)
        cout<<(*S_star).Set[i]<<" ";
    cout<<endl;
    cout<<"oracle times: "<<oracle_times<<endl;

    cout<<"MultiRandomAcc ---------end--------- "<<endl<<endl;
    return pair<double,long long int>((*S_star).S_revenue,oracle_times);
}
pair<double,long long int> multi_random(double eps,dataset* data,int m)
{
    double p=2.0/(1.0+sqrt((float)k));
    p=0.95;

    bernoulli_distribution u(p);
    default_random_engine e((unsigned)time(NULL));

    int node_num=data->index.size();
    long long int oracle_times=0;
    vector<S_gamma> S;
    int ell=2;
    //ell=1;
    for(int i=0;i<ell;i++)
        S.push_back(S_gamma());

    vector<int> available(node_num,1);//mark S_i selected and discarded
    while(1)
    {
        for (int j = 0; j < S.size(); j++)
        {
            S[j].max_marginal=-999999999.0;
            S[j].max_element=-1;
            for (int element = 0; element < node_num; element++)
            {
                if (available[element] == 1)//if e\notin any S_i and has not been discarded
                {
                    //judge S+{e}\in I or not
                    bool flag=S[j].is_feasible(element,m);
                    //if S+{e}\in I
                    if (flag) {
                        oracle_times++;
                        double marginal = S[j].marginal(element);

                        if (marginal > S[j].max_marginal) {
                            S[j].max_marginal= marginal;
                            S[j].max_element = element;
                        }
                    }
                }
            }
        }
        double max_marginal = 0.0;
        int max_element=-1;
        int max_solution = -1;
        for(int j=0;j<S.size();j++)
        {
            if (S[j].max_marginal > max_marginal) {
                max_solution=j;
                max_marginal=S[j].max_marginal;
                max_element=S[j].max_element;
            }
        }
        if (max_solution == -1||max_marginal<=0)//non element or marginal gain<=0
            break;
        //S<-S\cup {u}
        //cout<<max_element<<" "<<max_marginal<<endl;
        if(u(e)==1)
        {
            S[max_solution].Set.push_back(max_element);
            S[max_solution].S_revenue += max_marginal;

            for (int i = 0; i < S[max_solution].genres_sum.size(); i++)//S+e\in I
            {
                S[max_solution].genres_sum[i] += genres_index[max_element][i];
            }
        }
        available[max_element] = 0;//discarded or selected
    }

    S_gamma *S_star;
    double max_revenue=-999999999.0;

    cout<<"MultiRandom & m: "<<m<<endl;
    for(int i=0;i<S.size();i++)
    {
        cout<<"S"<<i+1<<endl;
        cout<<"  revenue: "<<S[i].S_revenue<<" size: "<<S[i].Set.size()<<endl;
        cout<<"  all nodes: "<<endl;
        for(int j=0;j<S[i].Set.size();j++)
        {
            cout<<S[i].Set[j]<<" ";
        }
        cout<<endl;
        cout<<"  genre limit: "<<endl;
        for(int j=0;j<S[i].genres_sum.size();j++)
        {
            cout<<S[i].genres_sum[j]<<" ";
        }
        cout<<endl;
        if(S[i].S_revenue>max_revenue)
        {
            S_star=&S[i];
            max_revenue=S[i].S_revenue;
        }
    }
    cout<<"S*:"<<endl;
    cout<<"  revenue: "<<(*S_star).S_revenue<<" size: "<<(*S_star).Set.size()<<endl;
    cout<<"  all nodes: "<<endl;
    for(int i=0;i<(*S_star).Set.size();i++)
        cout<<(*S_star).Set[i]<<" ";
    cout<<endl;
    cout<<"oracle times: "<<oracle_times<<endl;

    cout<<"MultiRandom ---------end--------- "<<endl<<endl;
    return pair<double,long long int>((*S_star).S_revenue,oracle_times);
}
#endif //ICML_MOV_MULTIRANDOM_H
