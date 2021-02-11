#include "generate_data.h"
#include "read_data.h"
#include "time.h"
#include "twingreedy.h"
#include "fastsgs.h"
#include "multirandom.h"
#include "repeatedgreedy.h"
int main(int argc,char *argv[]) {

    //generate_data_with_lable();
    //return 0;
    //generate_data();
    string::size_type pos1, pos2,posend;
    pos1=cost_text.find_last_of("/");
    pos2=cost_text.rfind("/",pos1-1);

    //posend=cost_text.find_last_not_of("/");
    string name1=cost_text.substr(pos2+1,pos1-pos2-1);
    //string name2=cost_text.substr(pos1+1,posend);
    //string result_name=name1+"_"+name2;
    string result_name="engine_"+name1;

    read();
    cal_similarity();

    //test
    /*
    cout<<"f(u)"<<endl;
    for(int iter=0;iter<node_num;iter++)
    {
        cout<<f_u(iter)<<endl;
    }
    cout<<"f(u)/cost"<<endl;
    double sum_density=0.0;
    for(int iter=0;iter<node_num;iter++)
    {
        cout<<f_u(iter)/contrast_cost[iter]<<endl;
        sum_density+=f_u(iter)/contrast_cost[iter];
    }
    sum_density/=node_num;
    cout<<"ave density:"<<sum_density<<endl;
    //*/

    //test revenue
    /*
    vector<int> t1={120 ,203 ,160 ,435 ,152 ,225 ,29 ,332 ,111 ,142 ,273 ,422, 163, 397 ,54 ,340 ,124 ,478 ,258 ,267 ,460 ,431, 199 ,12 ,496, 326 ,441 ,377, 237, 274 ,131, 367, 187 ,404 ,90 ,122 ,485 ,393 ,380 ,55 ,270};
    cout<<f_S(t1)<<endl;
    cout<<t1.size()<<endl;

    vector<int> t2={ 120 ,203 ,160 ,435 ,152 ,225 ,29 ,332 ,111 ,142 ,273 ,422, 163, 397 ,54 ,340 ,124 ,478 ,258 ,267 ,460 ,431, 199 ,12 ,496, 326 ,441 ,377,  274 ,131, 367, 187 ,404 ,90 ,122 ,485 ,393 ,380 ,55 ,270};
    cout<<f_S(t2)+marginal_gain(237,t2)<<endl;

return 0;
*/

    double eps=atof(argv[1]);
    //double B=atof(argv[1]);
    //double eps=atof(argv[2]);
    cout<<"eps: "<<eps<<endl;
    double default_p=sqrt(2)-1;

    time_t nowtime;
    struct tm* p;;
    time(&nowtime);
    p = localtime(&nowtime);
    string outtext="./result/image_result_normalize"+to_string((int)ave_num)+"_"+result_name+"_"+to_string(p->tm_mon+1)+"."+to_string(p->tm_mday)+"_"+to_string(p->tm_hour)+"_"+to_string(p->tm_min)+"_"+to_string(p->tm_sec)+".txt";

    vector<pair<double,long long int>> twingreedy_result;
    vector<pair<double,long long int>> fastsgs_result;
    vector<pair<double,long long int>> multirandom_result;
    vector<pair<double,long long int>> multirandom_acc_result;
    vector<pair<double,long long int>> repeatedgreedy_result;

    int m_start=4;
    int m_end=13;
    int m_step=1;

    for(int m=m_start;m<=m_end;m+=m_step)
    {
        multirandom_acc_result.push_back(multi_random_acc(eps,m));
        //multirandom_result.push_back(multi_random(eps,m));
        twingreedy_result.push_back(twingreedy(eps,m));
        fastsgs_result.push_back(fastsgs(eps,m));
        repeatedgreedy_result.push_back(repeatedgreedy(m));
    }

    ofstream out(outtext);
    out<<"eps: "<<eps<<endl;
    out<<"m: "<<endl;
    for(int m=m_start;m<=m_end;m+=m_step)
    {
        out<<m<<"\t";
    }
    out<<endl;

    out<<"MultiRandomAcc "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:multirandom_acc_result)
    {
        out<<p.first<<"\t";
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
    out<<"Repeated Greedy "<<endl;
    out<<"revenue: "<<endl;
    for(auto p:repeatedgreedy_result)
    {
        out<<p.first<<"\t";
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
        out<<p.first<<"\t";
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
        out<<p.first<<"\t";
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
