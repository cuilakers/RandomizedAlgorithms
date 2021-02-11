//
// Created by CLAKERS on 2021/1/23.
//

#ifndef ICML_IMA_FASTSGS_H
#define ICML_IMA_FASTSGS_H
#include "read_data.h"
pair<double,long long int> fastsgs(double eps,int max_image)
{
    long long int oracle_times=0;
    vector<S_gamma> S;
    //int k=1;//1-system
    int ell=2+sqrt(k+2.0);
    //ell=2;
    //cout<<"ell"<<ell<<endl;
    for(int i=0;i<ell;i++)
        S.push_back(S_gamma());
    double delta_f=0.0;
    for(int iter=0;iter<node_num;iter++)
    {
        //judge {e}\in I or not

        oracle_times++;
        double value=f_u(iter);
        if (value > delta_f) {
            delta_f = value;
        }
    }
    vector<int> available(node_num,1);
    for(double tau=delta_f;tau>((eps/(double)node_num)*delta_f);tau=tau*(1.0-eps))
    {
        int selected_node=0;
        for(int element=0;element<node_num;element++)
        {
            if(available[element]==1)//if e\notin S_i,ie., u\in N_i-1
            {
                //double max_marginal=-999.0;
                //int max_solution=-1;
                for(int j=0;j<S.size();j++)
                {
                    //judge S+{e}\in I or not
                    bool flag=S[j].is_feasible(element,max_image);
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

                        if(marginal>=tau)//if f(e|S_i)>=tau
                        {
                            S[j].Set.push_back(element);
                            S[j].S_revenue+=marginal;
                            S[j].category_sum[lable[element]]++;//S+e\in I

                            available[element]=0;

                            //clear marginal store and initialize
                            S[j].marginal_store.clear();
                            for(int i=0;i<node_num;i++)
                            {
                                S[j].marginal_store.push_back(make_pair(0,-999999.0));
                            }


                            selected_node++;

                            break;
                        }
                    }

                }
            }
        }

        //cout<<"tau: "<<tau<<" selected_node: "<<selected_node<<" oracle times: "<<oracle_times<<endl;
    }

    S_gamma *S_star;
    double max_revenue=-999999999.0;

    cout<<"Fastsgs & max_image: "<<max_image<<endl;
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

    cout<<"Fastsgs ---------end--------- "<<endl<<endl;
    return pair<double,long long int>((*S_star).S_revenue,oracle_times);
}

#endif //ICML_IMA_FASTSGS_H
