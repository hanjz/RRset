
typedef pair<double,int> dipair;


#include "iheap.h"
#include <ctime> // Needed for the true randomization
#include <cstdlib>

class InfGraph: public Graph
{
private:
    vector<bool> visit;
    vector<int> visit_mark;
public:
    vector<vector<int>> hyperG;
    vector<vector<int>> hyperGT;
    sfmt_t sfmtSeed;

    InfGraph(string folder, string graph_file): Graph(folder, graph_file)
    {
        srand( time(0)); // This will ensure a really randomized number by help of time.

        int xRan=rand()%15+1; // Randomizing the number between 1-15.
//        sfmt_init_gen_rand(&sfmtSeed , 95082);
        sfmt_init_gen_rand(&sfmtSeed , xRan);
        init_hyper_graph();
        visit = vector<bool> (n);
        visit_mark = vector<int> (n);
    }


    void init_hyper_graph(){
        hyperG.clear();
        for (int i = 0; i < n; i++)
            hyperG.push_back(vector<int>());
        hyperGT.clear();
    }
    void build_hyper_graph_r(int64 R, const Argument & arg)
    {
        if( R > INT_MAX ){
            cout<<"Error:R too large"<<endl;
            exit(1);
        }
        //INFO("build_hyper_graph_r", R);



        int prevSize = hyperGT.size();
        while ((int)hyperGT.size() <= R)
            hyperGT.push_back( vector<int>() );



        vector<int> random_number;
        for (int i = 0; i < R; i++)
        {
            // R random nodes for R influence diffusion or RRsets
            random_number.push_back(  sfmt_genrand_uint32(&sfmtSeed) % n);
        }

        //trying BFS start from same node
        

        for (int i = prevSize; i < R; i++)
        {
#ifdef CONTINUOUS
            BuildHypergraphNode(random_number[i], i, arg );
#endif
#ifdef DISCRETE
            BuildHypergraphNode(random_number[i], i );
#endif
        }

        for (int i = 0; i < (int) hyperGT.size();i++){
            for(int j = 0; j < (int) hyperGT[i].size(); j++){
                cout<<hyperGT[i][j]<<", ";
            }
            cout<<endl;
        }


        int totAddedElement = 0;
        for (int i = prevSize; i < R; i++)
        {
            for (int t : hyperGT[i])
            {
                hyperG[t].push_back(i);
                //hyperG.addElement(t, i);
                totAddedElement++;
            }
        }
    }

#ifdef DISCRETE
#include "discrete_rrset.h"
#endif
#ifdef CONTINUOUS
#include "continuous_rrset.h"
#endif

    //return the number of edges visited
    deque<int> q;
    set<int> seedSet;
    void build_seedset(int k)
    {
        Counter cnt(1);
        vector< int > degree;
        vector< int> visit_local(hyperGT.size());

        seedSet.clear();
        for (int i = 0; i < n; i++)
        {
            degree.push_back( hyperG[i].size() );
        }
        ASSERT(k > 0);
        ASSERT(k < (int)degree.size());
        for (int i = 0; i < k; i++)
        {
            auto t = max_element(degree.begin(), degree.end());
            int id = t - degree.begin();
            seedSet.insert(id);
            degree[id] = 0;
            for (int t : hyperG[id])
            {
                if (!visit_local[t])
                {
                    visit_local[t] = true;
                    for (int node : hyperGT[t])
                    {
                        degree[node]--;
                    }
                }
            }
        }
        TRACE(seedSet);
    }
    double InfluenceHyperGraph()
    {

        set<int> s;
        TRACE(seedSet);
        for (auto t : seedSet)
        {
            for (auto tt : hyperG[t])
            {
                s.insert(tt);
            }
        }
        double inf = (double)n * s.size() / hyperGT.size();
        return inf;
    }

};





