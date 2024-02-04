#include <iostream>
#include <vector>

using namespace std;

struct DSU{
    vector<int> par;
    int n;
    DSU(int n):n(n), par(n+1){
        for(int i = 1; i <= n; i++){
            par[i] = -1;
        }
    }
    int find(int u){
        if(par[u] < 0){
            return u;
        }
        return par[u] = find(par[u]);
    }

    void merge(int u, int v){
        u = find(u);
        v = find(v);
        if(u != v){
            if((-par[u]) < (-par[v])){
                swap(u,v);
            }
            par[u] += par[v];
            par[v] = u;
        }
    }
    int size_of(int u){
        return -par[find(u)];
    }

    bool same_set(int u, int v){
        return find(u) == find(v);
    }
};

int main(){

}