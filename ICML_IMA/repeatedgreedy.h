//
// Created by 10164 on 2021/1/31.
//

#ifndef ICML_IMA_REPEATEDGREEDY_H
#define ICML_IMA_REPEATEDGREEDY_H
#include "read_data.h"
#include <algorithm>
#include <list>
S_gamma greedy(list<int> &N,long long int &oracle_times,int max_image)
{
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
            //if (S.selected[*p] == 0)//useless，because N always maintain right
            //{
            if(S.is_feasible(*p,max_image)) {
                oracle_times++;

                double temp_marginal = S.marginal(*p);
                if (temp_marginal > max_marginal) {
                    max_marginal = temp_marginal;
                    max_node = p;
                }
            }
            //}
        }
        if(max_marginal==-999999999)
            break;
        /*
        if(max_marginal<=0)
        {
            break;
        }
//*/

        S.Set.push_back(*max_node);
        //S.selected[*max_node]=1;
        S.S_revenue+=max_marginal;
        S.category_sum[lable[*max_node]]++;//S+e\in I
        max_node=N.erase(max_node);
    }
    return S;
}

S_gamma Random_USM(const S_gamma &N,long long int &oracle_times)
{
    S_gamma X;
    S_gamma Y;
    Y.Set.assign(N.Set.begin(),N.Set.end());
    //Y.selected.assign(N.selected.begin(),N.selected.end());
    Y.S_revenue=N.S_revenue;

    default_random_engine e(1234);
    for(auto u=N.Set.begin();u!=N.Set.end();u++)
        //for(auto u=N.Set.rbegin();u!=N.Set.rend();u++)
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

        if(bi>0) cout<<"bi>0 !!!"<<endl;
        if(r(e)==1)
        {
            //X.selected[*u]=1;
            X.Set.push_back(*u);
            X.S_revenue=f_Xi_1_and_u;
            X.category_sum[lable[*u]]++;//S+e\in I
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
            //Y.S_cost-=contrast_cost[*u];
        }
    }
    return X;
}

pair<double,long long int> repeatedgreedy(int max_image)
{
    long long int oracle_times=0;
    vector<S_gamma> U;
    vector<int> node_available(node_num,1);
    list<int> omega;
    for(int i=0;i<node_num;i++)//omega has all nodes at first time
        omega.push_back(i);
    //int available_num=node_num;
    int iteration=ceil(sqrt(k));
    for(int i=0;i<iteration;i++)
    {
        S_gamma Si=greedy(omega,oracle_times,max_image);
        //S_gamma Si_=USM(Si,oracle_times);
        S_gamma Si_=Random_USM(Si,oracle_times);

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
    cout<<"Repeated Greedy & m: "<<max_image<<endl;
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

#endif //ICML_IMA_REPEATEDGREEDY_H
