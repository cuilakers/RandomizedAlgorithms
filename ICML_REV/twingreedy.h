//
// Created by CLAKERS on 2021/1/24.
//

#ifndef ICML_REV_TWINGREEDY_H
#define ICML_REV_TWINGREEDY_H
#include "utility.h"

pair<pair<double,double>,long long int> twingreedy(double eps,int per_product_max)
{
    //initialize node_ratio
    fill(node_ratio.begin(), node_ratio.end(), 1);

    long long int oracle_times=0;
    vector<S_class> S;
    int ell=2;
    for(int i=0;i<ell;i++)
        S.push_back(S_class());
    double tau_max=0.0;
    for(int q=0;q<product_types;q++) {
        for (int iter = 0; iter < node_num; iter++) {
            //judge {e}\in I or not
            oracle_times++;
            double value = f_u(pair<int,int>(q,iter));
            if (value > tau_max) {
                tau_max = value;
            }
        }
    }
    int r=product_types*per_product_max;
    vector<vector<int>> available(product_types,vector<int>(node_num,1));
    for(double tau=tau_max;tau>(eps*tau_max/((float)r*(1.0+eps)));tau/=(1.0+eps))
    {
        for(int q=0;q<product_types;q++)
        {
            for (int element = 0; element < node_num; element++)
            {
                if (available[q][element] == 1)//if e\notin S_1 or S_2 ie., e is not selected
                {
                    double max_marginal = -999999999.0;
                    int max_solution = -1;
                    for (int j = 0; j < S.size(); j++) {//choose better from S_1 and S_2
                        //judge S+{e}\in I or not
                        bool flag = S[j].is_feasible(pair<int,int>(q,element), per_product_max);
                        //if S+{e}\in I
                        if (flag) {
                            double marginal=0.0;
                            if(S[j].marginal_store[q][element].first==0) {
                                oracle_times++;
                                marginal = S[j].marginal(pair<int, int>(q, element));

                                //store marginal gain
                                S[j].marginal_store[q][element].first=1;
                                S[j].marginal_store[q][element].second=marginal;
                            }
                            else
                            {
                                marginal=S[j].marginal_store[q][element].second;
                            }

                            if (marginal > max_marginal) {
                                max_marginal = marginal;
                                max_solution = j;
                            }
                        }
                    }
                    if (max_solution == -1)//non element
                        continue;
                    if (max_marginal >= tau) {
                        S[max_solution].Set.push_back(pair<int,int>(q,element));
                        S[max_solution].S_revenue += max_marginal;
                        S[max_solution].per_product_used[q]++;//S+e\in I
                        S[max_solution].selected[q][element] = 1;

                        available[q][element] = 0;

                        //clear marginal store and initialize
                        for(int q=0;q<product_types;q++) {
                            S[max_solution].marginal_store[q].clear();
                            for(int i=0;i<node_num;i++)
                            {
                                S[max_solution].marginal_store[q].push_back(make_pair(0,-999999.0));
                            }
                        }
                    }
                }
            }

        }

    }

    S_class *S_star;
    double max_revenue=-999999999.0;

    cout<<"Twingreedy & per_product_max: "<<per_product_max<<endl;
    for(int i=0;i<S.size();i++)
    {
        cout<<"S"<<i+1<<endl;
        cout<<"  revenue: "<<S[i].S_revenue<<" size: "<<S[i].Set.size()<<endl;
        cout<<"  all nodes: "<<endl;
        for(int j=0;j<S[i].Set.size();j++)
        {
            cout<<"("<<S[i].Set[j].first<<","<<S[i].Set[j].second<<") ";
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
        cout<<"("<<(*S_star).Set[i].first<<","<<(*S_star).Set[i].second<<") ";
    cout<<endl;
    cout<<"oracle times: "<<oracle_times<<endl;
//*
    double real_revenue=0.0;
    for(int r=0;r<realization;r++)
    {
        double realization=(*S_star).realf_S(r);
        real_revenue+=realization;
        cout<<"realization "<<r<<": "<<realization<<" ";

        twingreedy_realization[(per_product_max-m_start)/m_step].push_back(realization);
    }
    cout<<endl;//*/
    double real_revenue_ave=real_revenue/(double )realization;
    cout<<"realization average: "<<real_revenue_ave<<endl;
    cout<<"Twingreedy ---------end--------- "<<endl<<endl;
    return pair<pair<double,double>,long long int>(pair<double,double>((*S_star).S_revenue,real_revenue_ave),oracle_times);
    //return pair<double,long long int>((*S_star).S_revenue,oracle_times);
}

#endif //ICML_REV_TWINGREEDY_H
