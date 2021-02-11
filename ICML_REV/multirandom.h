//
// Created by CLAKERS on 2021/1/24.
//

#ifndef ICML_REV_MULTIRANDOM_H
#define ICML_REV_MULTIRANDOM_H
#include "utility.h"
#include "algorithm"
//*
class Ai
{
public:
    Ai(pair<int,int> node_temp)
    {
        node=node_temp;
        weight=f_u(node);
        l=1;
    }
    pair<int,int> node;
    double weight;
    int l;
};
bool cmp(const Ai &a,const Ai &b)
{
    return a.weight>b.weight;
}
pair<pair<int,int>,double> GetMax(const int per_product_max,long long int &oracle_times,double eps,vector<Ai> &A,S_class &Si,const vector<vector<int>> &avaiable)
{
    double eta=1.0;
    double eps_apostrophe=eta*eps;
    for(auto e=A.begin();e!=A.end();)//remove unfeasible element
    {
        if((avaiable[(*e).node.first][(*e).node.second]==0)||(!Si.is_feasible(pair<int,int>((*e).node.first,(*e).node.second), per_product_max)))
        {
            e=A.erase(e);
        }
        else
        {
            e++;
        }
    }
    //sort
    sort(A.begin(),A.end(),cmp);
    double m=0.0;//always record weight of aij
    pair<int,int> ai(-1,-1);
    for(auto &u:A)
    {
        double wt=u.weight;
        if(m>=wt) break;
        //update weight
        oracle_times++;
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
    int r=per_product_max*product_types;
    for(auto e=A.begin();e!=A.end();)
    {
        if((*e).l>(log((float)r/eps_apostrophe)/log(1.0+eps_apostrophe)))
        {
            e=A.erase(e);
        }
        else
        {
            e++;
        }
    }
    return pair<pair<int,int>,double>(ai,m);

}
int ran=1111;
default_random_engine e_const((unsigned)time(NULL));
pair<pair<double,double>,long long int> multi_random_acc(double eps,int per_product_max)
{
    //initialize node_ratio
    fill(node_ratio.begin(), node_ratio.end(), 1);

    //int k=1;
    double p=2.0/(1.0+sqrt((float)k));
    //cout<<p<<endl;
    p=1;

    bernoulli_distribution u(p);
    //default_random_engine e((unsigned)time(NULL)*ran);
    ran++;
    //default_random_engine e(1);

    long long int oracle_times=0;
    vector<S_class> S;
    vector<vector<Ai>> A;
    int ell=2;
    //ell=1;
    for(int i=0;i<ell;i++) {
        S.push_back(S_class());
        A.push_back(vector<Ai>());
    }
    //initial Ai
    for(int q=0;q<product_types;q++)
    {
        for (int i = 0; i < node_num; i++)
        {
            oracle_times++;
            Ai temp(pair<int,int>(q,i));
            for (int j = 0; j < ell; j++) {
                A[j].push_back(temp);
            }
        }
    }
    vector<vector<int>> available(product_types,vector<int>(node_num,1));//mark S_i selected and discarded
    while(1)
    {
        for (int j = 0; j < S.size(); j++)
        {
            S[j].max_marginal=-999999999.0;
            S[j].max_element=make_pair(-1,-1);

            //A=empty, then return element=-1 and marginal gain =0
            pair<pair<int,int>,double> temp=GetMax(per_product_max,oracle_times,eps,A[j],S[j],available);
            S[j].max_marginal=temp.second;
            S[j].max_element=temp.first;
        }
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
        }
        if (max_solution == -1||max_marginal<=0)//non element or marginal gain<=0
            break;
        //S<-S\cup {u}
        if(u(e_const)==1)
        {
            S[max_solution].Set.push_back(max_element);
            S[max_solution].S_revenue += max_marginal;
            S[max_solution].per_product_used[max_element.first]++;//S+e\in I
            S[max_solution].selected[max_element.first][max_element.second] = 1;
        }
        available[max_element.first][max_element.second] = 0;//discarded or selected
    }

    S_class *S_star;
    double max_revenue=-999999999.0;

    cout<<"MultiRandomAcc & per_product_max: "<<per_product_max<<endl;
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

        multirandom_acc_realization[(per_product_max-m_start)/m_step].push_back(realization);

        cout<<"realization "<<r<<": "<<realization<<" ";
    }
    cout<<endl;
          //*/
    double real_revenue_ave=real_revenue/(double)realization;
    cout<<"realization average: "<<real_revenue_ave<<endl;
    cout<<"MultiRandomAcc ---------end--------- "<<endl<<endl;
    return pair<pair<double,double>,long long int>(pair<double,double>((*S_star).S_revenue,real_revenue_ave),oracle_times);
    //return pair<double,long long int>((*S_star).S_revenue,oracle_times);
}
pair<double,long long int> multi_random(double eps,int per_product_max)
{
    //initialize edge_weight
    /*
    for(int i=0;i<node_num;i++)
    {
        for(int j=0;j<edge_weight[i].size();j++)
        {
            edge_weight[i][j].second=edge_weight_expectation;
        }
    }*/

    fill(node_ratio.begin(), node_ratio.end(), 1);
    int k=2;
    double p=2.0/(1.0+sqrt((float)k));
    //p=1.0;
    bernoulli_distribution u(p);
    default_random_engine e((unsigned)time(NULL));
    //default_random_engine e(1);

    long long int oracle_times=0;
    vector<S_class> S;
    int ell=2;

    //ell=1;

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
                            oracle_times++;
                            double marginal = S[j].marginal(pair<int,int>(q,element));

                            if (marginal > S[j].max_marginal) {
                                S[j].max_marginal = marginal;
                                S[j].max_element=make_pair(q,element);
                            }
                        }
                    }
                }
            }
        }
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
        }
        if (max_solution == -1||max_marginal<=0)//non element or marginal gain<=0
            break;
        //S<-S\cup {u}
        if(u(e)==1)
        {
            S[max_solution].Set.push_back(max_element);
            S[max_solution].S_revenue += max_marginal;
            S[max_solution].per_product_used[max_element.first]++;//S+e\in I
            S[max_solution].selected[max_element.first][max_element.second] = 1;
        }
        available[max_element.first][max_element.second] = 0;//discarded or selected
    }

    S_class *S_star;
    double max_revenue=-999999999.0;

    cout<<"MultiRandom & per_product_max: "<<per_product_max<<endl;
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

    cout<<"MultiRandom ---------end--------- "<<endl<<endl;
    return pair<double,long long int>((*S_star).S_revenue,oracle_times);
}


#endif //ICML_REV_MULTIRANDOM_H
