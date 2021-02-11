//
// Created by CLAKERS on 2021/1/24.
//

#ifndef ICML_REV_ADAPTIVE_H
#define ICML_REV_ADAPTIVE_H
#include "utility.h"
pair<double,long long int> adaptive(double eps,int per_product_max,int realization_index)
{
    //initialize node_ratio
    fill(node_ratio.begin(), node_ratio.end(), 1);

    //int k=1;
    double p=1.0/(1.0+sqrt((float)k+1.0));
    //cout<<p<<endl;
    //p=1;
    bernoulli_distribution u(p);
    default_random_engine e((unsigned)time(NULL));
    //default_random_engine e(1);

    long long int oracle_times=0;
    vector<S_class> S;
    int ell=1;
    for(int i=0;i<ell;i++)
        S.push_back(S_class());

    vector<vector<int>> available(product_types,vector<int>(node_num,1));//mark S_i selected and discarded
    while(1)
    {
        for (int j = 0; j < S.size(); j++)
        {
            S[j].max_marginal=-999999999.0;
            S[j].max_element=make_pair(-1,-1);
            for(int q=0;q<product_types;q++)
            {
                for (int element = 0; element < node_num; element++)
                {
                    if (available[q][element] == 1)//if e\notin any S_i and has not been discarded
                    {
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

                            if (marginal > S[j].max_marginal) {
                                S[j].max_marginal = marginal;
                                S[j].max_element=make_pair(q,element);
                            }
                        }
                    }
                }
            }
        }

        //*
        double max_marginal = 0.0;
        pair<int,int> max_element(-1,-1);
        int max_solution = -1;
        for(int j=0;j<S.size();j++)
        {
            if (S[j].max_marginal > max_marginal) {
                max_solution=j;
                max_marginal=S[j].max_marginal;
                max_element=S[j].max_element;
            }
        }//*/

        if (max_solution == -1||max_marginal<=0)//non element or marginal gain<=0
            break;
        //S<-S\cup {u}
        //after observing, we need to calculate f(S) and update S.revenue
        if(u(e)==1)
        {
            S[max_solution].observe(max_element.second,realization_index);

            S[max_solution].Set.push_back(max_element);
            S[max_solution].per_product_used[max_element.first]++;//S+e\in I
            S[max_solution].selected[max_element.first][max_element.second] = 1;

            S[max_solution].S_revenue = S[max_solution].f_S();


            //clear marginal store and initialize
            for(int q=0;q<product_types;q++) {
                S[max_solution].marginal_store[q].clear();
                for(int i=0;i<node_num;i++)
                {
                    S[max_solution].marginal_store[q].push_back(make_pair(0,-999999.0));
                }
            }
        }
        available[max_element.first][max_element.second] = 0;//discarded or selected
    }

    S_class *S_star;
    double max_revenue=-999999999.0;

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

    cout<<"-----realization "<<realization_index<<"-----"<<endl;
    double real_revenue=(*S_star).realf_S(realization_index);
    cout<<"  revenue: "<<(*S_star).S_revenue<<endl;
    cout<<"-----end-----"<<endl;
    /*
    cout<<"node_ratio: "<<endl;
    for(int i=0;i<node_num;i++)
    {
        cout <<  node_ratio[i]<<"\t"<< node_ratio_realization[i][realization_index]<<endl;
    }

   */
    return pair<double,long long int>(real_revenue,oracle_times);
    //return pair<double,long long int>((*S_star).S_revenue,oracle_times);
}
pair<double,long long int> adaptive_cycle(double eps,int per_product_max)
{
    double real_revenue=0.0;
    long long int oracle_times=0;
    cout<<"Adaptive & per_product_max: "<<per_product_max<<endl;
    for(int r=0;r<realization;r++) {
        pair<double,long long int> temp=adaptive(eps, per_product_max, r);
        real_revenue+=temp.first;
        oracle_times+=temp.second;

        adaptive_realization[(per_product_max-m_start)/m_step].push_back(temp.first);
    }
    cout<<"Adaptive ---------end--------- "<<endl<<endl;
    return pair<double,long long int>(real_revenue/(float)realization,oracle_times);
}
#endif //ICML_REV_ADAPTIVE_H
