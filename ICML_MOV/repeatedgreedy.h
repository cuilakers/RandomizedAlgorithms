//
// Created by 10164 on 2021/1/30.
//

#ifndef ICML_MOV_REPEATEDGREEDY_H
#define ICML_MOV_REPEATEDGREEDY_H
#include "utility_functions.h"

S_gamma greedy(list<int> &N,dataset* data,long long int &oracle_times,int max_movie)
{
    int node_num=data->index.size();
    //copy omega to N for Greedy algorithm
    double max_marginal=-999999999;
    list<int>::iterator max_node;
    S_gamma S;
    while(!N.empty())
    {
        max_marginal=-999999999;
        //only visit node in N, which is avaiable now
        for(list<int>::iterator p=N.begin();p!=N.end();p++)
        {
            if(S.is_feasible(*p,max_movie)) {
                oracle_times++;

                double temp_marginal = S.marginal(*p);
                if (temp_marginal > max_marginal) {
                    max_marginal = temp_marginal;
                    max_node = p;
                }
            }
        }
        if(max_marginal==-999999999)
            break;

        S.Set.push_back(*max_node);
        //cout<<*max_node<<" "<<max_marginal<<endl;
        S.S_revenue+=max_marginal;
        for (int i = 0; i < S.genres_sum.size(); i++)//S+e\in I
        {
            S.genres_sum[i] += genres_index[*max_node][i];
        }
        max_node=N.erase(max_node);
    }
    return S;
}
S_gamma random_USM(const S_gamma &N,dataset* data,long long int &oracle_times)
{
    //int node_num=data->index.size();
    S_gamma X;
    S_gamma Y;
    Y.Set.assign(N.Set.begin(),N.Set.end());
    //Y.selected.assign(N.selected.begin(),N.selected.end());
    //Y.S_cost=N.S_cost;
    Y.S_revenue=N.S_revenue;
    default_random_engine e(1234);
    for(auto u=N.Set.rbegin();u!=N.Set.rend();u++)
    {
        //calculate ai
        double f_Xi_1=X.S_revenue;
        //X.selected[*u]=1;
        X.Set.push_back(*u);

        oracle_times++;

        double f_Xi_1_and_u=X.f_S();
        X.Set.pop_back();
        //X.selected[*u]=0;
        double ai=f_Xi_1_and_u-f_Xi_1;

        //calculate bi
        double f_Yi_1=Y.S_revenue;
        //Y.selected[*u]=0;

        oracle_times++;

        double f_Yi_1_sub_u=Y.S_sub_u(*u);
        //Y.selected[*u]=1;
        double bi=f_Yi_1_sub_u-f_Yi_1;

        double ai1=max(ai,0.0);
        double bi1=max(bi,0.0);
        double probability=0.0;
        if(ai1==0&&bi1==0)
            probability=1.0;
        else
        {
            probability=ai1/(ai1+bi1);
        }
        bernoulli_distribution r(probability);

        //if(bi>0.0) cout<<"bi>0 !!!"<<endl;
        if(r(e)==1)
        {
            //X.selected[*u]=1;
            X.Set.push_back(*u);
            X.S_revenue=f_Xi_1_and_u;
            for (int i = 0; i < X.genres_sum.size(); i++)//S+e\in I
            {
                X.genres_sum[i] += genres_index[*u][i];
            }

            //X.S_cost+=data->rate_costs[*u];
        }
        else
        {
            cout<<"ai<bi !!!"<<endl;
            //Y.selected[*u]=0;
            //delete u_i
            for(vector<int>::iterator p=Y.Set.begin();p!=Y.Set.end();)
            {
                if((*p)==(*u))
                {
                    p=Y.Set.erase(p);
                    break;
                }
                else{
                    p++;
                }
            }
            Y.S_revenue=f_Yi_1_sub_u;
            //Y.S_cost-=data->rate_costs[*u];
        }
    }
    return X;
}
pair<double,long long int> repeatedgreedy(dataset* data,int max_movie)
{
    long long int oracle_times=0;
    int node_num=data->index.size();
    vector<S_gamma> U;
    vector<int> node_available(node_num,1);
    list<int> omega;
    for(int i=0;i<node_num;i++)//omega has all nodes at first time
        omega.push_back(i);
    //int available_num=node_num;
    int iteration=ceil(sqrt(k));
    for(int i=0;i<iteration;i++)
    {
        S_gamma Si=greedy(omega,data,oracle_times,max_movie);
        S_gamma Si_=random_USM(Si,data,oracle_times);

        U.push_back(Si);
        U.push_back(Si_);
        for(int j=0;j<Si.Set.size();j++)//mark omega=omega-S_i
        {
            node_available[Si.Set[j]] = 0;
        }
        omega.clear();
        for(int iter=0;iter<node_num;iter++)//rebuild omega
        {
            if(node_available[iter]==1)
                omega.push_back(iter);
        }
    }
    cout<<"Repeated Greedy & m: "<<max_movie<<endl;
    double max_value=-999999999;
    S_gamma *max_S;
    for(int i=0;i<U.size();i++)
    {
        if(U[i].S_revenue>max_value)
        {
            max_S = &U[i];
            max_value=U[i].S_revenue;
        }
//*
        cout<<"U_"<<i<<": "<<endl;
        cout<<"  revenue: "<<U[i].S_revenue<<" size: "<<U[i].Set.size()<<endl;
        cout<<"  all nodes: "<<endl;
        for(int j=0;j<U[i].Set.size();j++)
            cout<<U[i].Set[j]<<" ";
        cout<<endl;

        cout<<"  genre limit: "<<endl;
        for(int j=0;j<U[i].genres_sum.size();j++)
        {
            cout<<U[i].genres_sum[j]<<" ";
        }
        cout<<endl;

        //*/
    }
    cout<<"S*:"<<endl;
    cout<<"  revenue: "<<(*max_S).S_revenue<<" size: "<<(*max_S).Set.size()<<endl;
    cout<<"  all nodes: "<<endl;
    for(int i=0;i<(*max_S).Set.size();i++)
        cout<<(*max_S).Set[i]<<" ";
    cout<<endl;
    cout<<"oracle times: "<<oracle_times<<endl;
    cout<<"Repeated Greedy ---------end--------- "<<endl;
    return pair<double,long long int>((*max_S).S_revenue,oracle_times);
}
#endif //ICML_MOV_REPEATEDGREEDY_H
