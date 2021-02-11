//
// Created by CLAKERS on 2021/1/21.
//

#ifndef ICML_MOV_TWINGREEDY_H
#define ICML_MOV_TWINGREEDY_H

#include "utility_functions.h"
pair<double,long long int> twingreedy(double eps,dataset* data,int m)
{
    int node_num=data->index.size();
    long long int oracle_times=0;
    vector<S_gamma> S;
    int ell=2;
    for(int i=0;i<ell;i++)
        S.push_back(S_gamma());
    double tau_max=0.0;
    for(int iter=0;iter<node_num;iter++)
    {
        //judge {e}\in I or not
        bool flag=true;
        for(auto p:genres_index[iter])
        {
            if(p>m)
            {
                flag=false;
                break;
            }
        }
        if(!flag) continue;//if {e}\notin I
        oracle_times++;
        double value=f_u(iter,node_num);
        if (value > tau_max) {
            tau_max = value;
        }
    }
    //int r=k*m;
    vector<int> available(node_num,1);
    for(double tau=tau_max;tau>(eps*tau_max/((float)m*(1.0+eps)));tau/=(1.0+eps))
    {
        //for(int element=node_num-1;element>=0;element--)
        for(int element=0;element<node_num;element++)
        {
            if(available[element]==1)//if e\notin S_1 or S_2
            {
                double max_marginal=-999999999.0;
                int max_solution=-1;
                for(int j=0;j<S.size();j++)
                {
                    //judge S+{e}\in I or not
                    bool flag=S[j].is_feasible(element,m);

                    //if S+{e}\in I
                    if(flag)
                    {
                        //optimize oracle times
                        double marginal=0.0;
                        if(S[j].marginal_store[element].first==0) {
                            oracle_times++;
                            marginal = S[j].marginal(element);
                            //store marginal gain
                            S[j].marginal_store[element].first=1;
                            S[j].marginal_store[element].second=marginal;
                        }
                        else
                        {
                            marginal=S[j].marginal_store[element].second;
                        }

                        if(marginal>max_marginal)
                        {
                            max_marginal=marginal;
                            max_solution=j;
                        }
                    }
                }
                if(max_solution==-1)//non element
                    continue;
                if(max_marginal>=tau)
                {
                    S[max_solution].Set.push_back(element);
                    S[max_solution].S_revenue+=max_marginal;
                    for(int i=0;i<S[max_solution].genres_sum.size();i++)//S+e\in I
                    {
                        S[max_solution].genres_sum[i]+=genres_index[element][i];
                    }
                    available[element]=0;
                    //clear marginal store and initialize
                    S[max_solution].marginal_store.clear();
                    for(int i=0;i<node_num;i++)
                    {
                        S[max_solution].marginal_store.push_back(make_pair(0,-999999.0));
                    }

                }

            }
        }
    }

    S_gamma *S_star;
    double max_revenue=-999999999.0;

    cout<<"Twingreedy & m: "<<m<<endl;
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

    cout<<"Twingreedy ---------end--------- "<<endl<<endl;
    return pair<double,long long int>((*S_star).S_revenue,oracle_times);
}
#endif //ICML_MOV_TWINGREEDY_H
