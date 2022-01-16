#include<iostream>
#include<vector>
#include<utility>
using namespace std;

class InputValidation{

    public:

    bool validEdges(vector<pair<int,int>> edges, int vertices){

        for (auto i = edges.begin(); i != edges.end(); ++i){
            //cout << (*i).first << " " << (*i).second<<" ";
            if((*i).first>vertices-1||(*i).second>vertices-1||(*i).first<0 ||(*i).second<0)
                return false;
            if((*i).first == (*i).second){
                return false;
            }
        }
        return true;
    }
};
