//
// Created by 10164 on 2021/1/31.
//

#ifndef ICML_REV_REPEATEDGREEDY_H
#define ICML_REV_REPEATEDGREEDY_H
#include "utility.h"
#include "algorithm"
S_class greedy(list<pair<int,int>> &N,long long int &oracle_times,int per_product_max)
{
    //initialize node_ratio
    fill(node_ratio.begin(), node_ratio.end(), 1);

    //copy omega to N for Greedy algorithm
    double max_marginal=-999999999;
    list<pair<int,int>>::iterator max_node;
    S_class S;

    while(!N.empty())
    {
        max_marginal=-999999999;
        //only visit node in N, which is avaiable now
        for(list<pair<int,int>>::iterator p=N.begin();p!=N.end();p++)
        {
            //if (S.selected[*p] == 0)//useless，because N always maintain right
            //{
            if(S.is_feasible(*p, per_product_max)) {
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

        S.Set.push_back(*max_node);
        S.S_revenue+=max_marginal;
        S.per_product_used[(*max_node).first]++;//S+e\in I
        S.selected[(*max_node).first][(*max_node).second]=1;

        max_node=N.erase(max_node++);
    }
    return S;
}
S_class random_USM(const S_class &N,long long int &oracle_times)
{
    //initialize edge_weight
    fill(node_ratio.begin(), node_ratio.end(), 1);

    S_class X;
    S_class Y;
    Y.Set.assign(N.Set.begin(),N.Set.end());
    Y.selected.assign(N.selected.begin(),N.selected.end());
    Y.S_revenue=N.S_revenue;
    default_random_engine e(1234);
    for(auto u=N.Set.rbegin();u!=N.Set.rend();u++)
    {
        //calculate ai
        double f_Xi_1=X.S_revenue;
        X.selected[(*u).first][(*u).second]=1;

        oracle_times++;

        double f_Xi_1_and_u=X.f_S();
        X.selected[(*u).first][(*u).second]=0;
        double ai=f_Xi_1_and_u-f_Xi_1;

        //calculate bi
        double f_Yi_1=Y.S_revenue;
        Y.selected[(*u).first][(*u).second]=0;

        oracle_times++;

        double f_Yi_1_sub_u=Y.f_S();
        Y.selected[(*u).first][(*u).second]=1;
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
            X.selected[(*u).first][(*u).second]=1;
            X.Set.push_back(*u);
            X.S_revenue=f_Xi_1_and_u;
            X.per_product_used[(*u).first]++;//S+e\in I
        }
        else
        {
            cout<<"ai<bi !!!"<<endl;
            Y.selected[(*u).first][(*u).second]=0;
            //delete u_i
            for(vector<pair<int,int>>::iterator p=Y.Set.begin();p!=Y.Set.end();)
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
        }
    }
    return X;
}
pair<pair<double,double>,long long int> repeatedgreedy(int per_product_max)
{
    long long int oracle_times=0;
    vector<S_class> U;
    vector<vector<int>> node_available(product_types,vector<int>(node_num,1));//mark all nodes are avaiable ie.,N_i
    list<pair<int,int>> omega;//N_i: (q,user)
    //initial omega(N_i), omega has all nodes at first time
    for(int q=0;q<product_types;q++)
    {
        for (int i = 0; i < node_num; i++)
        {
            //oracle_times++;
            omega.push_back(pair<int,int>(q,i));
        }
    }
    int iteration=ceil(sqrt(k));
    for(int i=0;i<iteration;i++)
    {
        S_class Si=greedy(omega,oracle_times,per_product_max);
        S_class Si_=random_USM(Si,oracle_times);

        U.push_back(Si);
        U.push_back(Si_);
        for(int j=0;j<Si.Set.size();j++)//mark omega=omega-S_i
        {
            node_available[Si.Set[j].first][Si.Set[j].second] = 0;
        }
        omega.clear();

        for(int q=0;q<product_types;q++)//rebuild omega
        {
            for (int i = 0; i < node_num; i++)
            {
                //oracle_times++;
                if(node_available[q][i]==1)
                    omega.push_back(pair<int,int>(q,i));
            }
        }
    }
    cout<<"Repeated Greedy & m: "<<per_product_max<<endl;
    double max_value=-999999999;
    S_class *max_S;
    for(int i=0;i<U.size();i++)
    {
        if(U[i].S_revenue>max_value)
        {
            max_S = &U[i];
            max_value=U[i].S_revenue;
        }
//*
        cout<<"U_"<<i<<": "<<endl;
        cout<<"  revenue: "<<U[i].S_revenue<<" cost: "<<" size: "<<U[i].Set.size()<<endl;
        cout<<"  all nodes: "<<endl;
        for(int j=0;j<U[i].Set.size();j++)
            cout<<"("<<U[i].Set[j].first<<","<<U[i].Set[j].second<<") ";
        cout<<endl;
        //*/
    }
    cout<<"S*:"<<endl;
    cout<<"  revenue: "<<(*max_S).S_revenue<<" size: "<<(*max_S).Set.size()<<endl;
    cout<<"  all nodes: "<<endl;
    for(int i=0;i<(*max_S).Set.size();i++)
        cout<<"("<<(*max_S).Set[i].first<<","<<(*max_S).Set[i].second<<") ";
    cout<<endl;
    cout<<"oracle times: "<<oracle_times<<endl;

    //*
    double real_revenue=0.0;
    for(int r=0;r<realization;r++)
    {
        double realization=(*max_S).realf_S(r);
        real_revenue+=realization;
        repeatedgreedy_realization[(per_product_max-m_start)/m_step].push_back(realization);

        cout<<"realization "<<r<<": "<<realization<<" ";
    }
    cout<<endl;
    //*/
    double real_revenue_ave=real_revenue/(double)realization;
    cout<<"realization average: "<<real_revenue_ave<<endl;
    cout<<"Repeated Greedy ---------end--------- "<<endl<<endl;
    return pair<pair<double,double>,long long int>(pair<double,double>((*max_S).S_revenue,real_revenue_ave),oracle_times);

}
#endif //ICML_REV_REPEATEDGREEDY_H
