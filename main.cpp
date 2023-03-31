#include<bits/stdc++.h>
#include "unistd.h"
using namespace std;
typedef long long ll;
const int NMAX = 200005;
const int PMAX = 100;
int N, M, T, P, D;
ll Y, DC, EC;
map<pair<int, int>, int>minDis;
struct ST{
    int s, t, minDis, id;
}st[NMAX];
struct Edge{
    int u, v, d;
}edge[NMAX];
vector<int>g[NMAX];

struct BitMap{
    char a[NMAX];
    bool operator[](int index){
        return (a[index / 8] >> (index % 8)) & 1;
    }
    void set(int index){
        a[index / 8] |=  1 << (index % 8);
    }
    void reset(int index){
        a[index / 8] &=  ~(1 << (index % 8));
    }
    void clear(int tot){
        memset(a, 0, tot / 8 + 1);
    }
    void clear(){
        memset(a, 0, sizeof(a));
    }
};
BitMap vis;
int group[NMAX];
bool edgePMAP[NMAX][PMAX];
int edgePRE[NMAX];
struct ANS{
    int p;
    vector<int>edge;
    vector<int>costD;
}Ans[NMAX];
vector<int> gShrink[NMAX];


int bfsPRE(int s, int t){
    vis.clear(N);
    queue<int>q;
    q.push(s);
    vis.set(s);
    int dis = 0;
    while(q.size()){
        int tot = q.size();
        while(tot--){
            int x = q.front();
            if(x == t)return dis;
            q.pop();
            for(auto i : g[x]){
                int y = x ^ edge[i].u ^ edge[i].v;
                if(!vis[y])vis.set(y), q.push(y);
            }
        }
        dis++;
    }
}

int bfsSEA(int s, int t, int p){
    vis.clear(N);
    queue<int>q;
    q.push(s);
    vis.set(s);
    int dis = 0;
    while(q.size()){
        int tot = q.size();
        while(tot--){
            int x = q.front();
            if(x == t)return dis;
            q.pop();
            for(auto i : g[x]){
                int y = x ^ edge[i].u ^ edge[i].v;
                if(!vis[y] && !edgePMAP[i][p])vis.set(y), q.push(y);
            }
        }
        dis++;
    }
    return 0;
}

void bfsSEL(int s, int t, int p){
    vis.clear(N);
    queue<int>q;
    q.push(s);
    vis.set(s);
    while(q.size()){
        int tot = q.size();
        while(tot--){
            int x = q.front();
            if(x == t)return;
            q.pop();
            for(auto i : g[x]){
                int y = x ^ edge[i].u ^ edge[i].v;
                if(!vis[y] && !edgePMAP[i][p]){
                    vis.set(y), q.push(y);
                    edgePRE[y] = i;
                }
            }
        }
    }
}

void SelectEdge(int s, int t, int p, int stId){
    bfsSEL(s, t ,p);
    int y = t;
    Ans[stId].p = p;
    vector<int>trace;
    while(y != s){
        trace.push_back(y);
        int preEdge = edgePRE[y];
        Ans[stId].edge.push_back(preEdge);
        edgePMAP[preEdge][p] = true;
        y ^= edge[preEdge].v ^ edge[preEdge].u;
    }
    reverse(Ans[stId].edge.begin(), Ans[stId].edge.end());
    trace.push_back(y);
    //for(int i = trace.size() - 1; i >= 0; i--)cout << trace[i] << ' '; cout << endl;
}

void dfsShrinkPoint(int v, int p, int count){
    vis.set(v);
    group[v] = count;
    for(auto i : g[v]){
        int u = v ^ edge[i].u ^ edge[i].v;
        if(!vis[u] && !edgePMAP[i][p]){
            vis.set(u);
            group[u] = count;
            dfsShrinkPoint(u, p, count);
        }
    }
}

int bfsSHR(int s, int t){
    vis.clear(N);
    queue<int>q;
    q.push(s);
    int dis = 0;
    vis.set(s);
    while(q.size()){
        int tot = q.size();
        while(tot--){
            int x = q.front();
            if(x == t)return dis;
            q.pop();
            for(auto i : gShrink[x]){
                int y = x ^ group[edge[i].v] ^ group[edge[i].u];
                if(!vis[y]){
                    vis.set(y);
                    q.push(y);
                    edgePRE[y] = i;
                }
            }
        }
        dis++;
    }
}


vector<int> GetAddEdgeP(int s, int t, int p){
    vis.clear(N);
    int count = 0;
    for(int i = 0; i < N; i++){
        if(!vis[i]){
            dfsShrinkPoint(i, p, count++);
        }
    }
    for(int i = 0; i < count; i++){
        gShrink[i].clear();
    }
    for(int i = 0; i < M; i++){
        int x = group[edge[i].v], y = group[edge[i].u];
        if(x != y){
            gShrink[x].push_back(i);
            gShrink[y].push_back(i);
        }
    }
    bfsSHR(group[s], group[t]);
    vector<int>EdgeP;
    int y = group[t];
    while(y != group[s]){
        int preEdge = edgePRE[y];
        EdgeP.push_back(preEdge);
        y ^= group[edge[preEdge].v] ^ group[edge[preEdge].u] ;
    }
    return EdgeP;
}

void copyAddEdge(int fa){
    edge[M] = edge[fa];
    edge[M].d = minDis[{edge[fa].u, edge[fa].v}];
    g[edge[fa].v].push_back(M);
    g[edge[fa].u].push_back(M);
    M++;
    Y++;
}
void AddEdge(int s, int t, int stId){
    vector<pair<vector<int>, int>>addEdgeP;
    for(int i = 0; i < P; i++){
        vector<int>now = GetAddEdgeP(s, t, i);
        addEdgeP.emplace_back(now, i);
        if(now.size() == 1)break;
    }
    sort(addEdgeP.begin(), addEdgeP.end(),
         [](pair<vector<int>, int>&x ,pair<vector<int>, int>&y){
             return x.first.size() < y.first.size();
         });

    for(int i : addEdgeP[0].first){
        copyAddEdge(i);
    }
    SelectEdge(s, t, addEdgeP[0].second, stId);
}

void Search(int s, int t, int stId){
    vector<pair<int ,int >>disList;
    for(int p = 0; p < P; p++){
        int dis = bfsSEA(s, t, p);
        if(dis){
            disList.emplace_back(dis, p);
        }
    }
    sort(disList.begin(), disList.end(), [](pair<int, int >x, pair<int ,int >y){
        return x.first < y.first;
    });
    if(disList.size() == 0){
        AddEdge(s, t, stId);
    } else {
        SelectEdge(s, t, disList[0].second, stId);
    }
}

void GetCost(){
    for(int i = 0; i < T; i++){
        ll cost = 0;
        int x = st[i].s;
        int stId = st[i].id;
        for(int j = 0; j < Ans[stId].edge.size(); j++){
            cost += edge[Ans[stId].edge[j]].d;
            if(cost > D){
                Ans[stId].costD.push_back(x);
                cost = edge[Ans[stId].edge[j]].d;
            }
            x ^= edge[Ans[stId].edge[j]].v ^ edge[Ans[stId].edge[j]].u;
        }
        EC += Ans[stId].edge.size();
        DC += Ans[stId].costD.size();
    }
}

void inData(){
    cin >> N >> M >> T >> P >> D;
    for(int i = 0; i < M; i++){
        cin >> edge[i].u >> edge[i].v >> edge[i].d;
    }
    for(int i = 0; i < T; i++){
        cin >> st[i].s >> st[i].t;
    }
}

void Run(){

    for(int i = 0; i < M; i++){
        if(!minDis[{edge[i].u, edge[i].v}])minDis[{edge[i].u, edge[i].v}] = edge[i].d;
        if(!minDis[{edge[i].v, edge[i].u}])minDis[{edge[i].v, edge[i].u}] = edge[i].d;
        minDis[{edge[i].v, edge[i].u}] = min({minDis[{edge[i].v, edge[i].u}], minDis[{edge[i].u, edge[i].v}], edge[i].d});
        minDis[{edge[i].u, edge[i].v}] = min({minDis[{edge[i].v, edge[i].u}], minDis[{edge[i].u, edge[i].v}], edge[i].d});
        g[edge[i].u].push_back(i);
        g[edge[i].v].push_back(i);
    }
    for(int i = 0; i < T; i++){
        st[i].id = i;
        st[i].minDis = bfsPRE(st[i].s, st[i].t);
    }
    sort(st, st + T, [](ST x, ST y){
        return x.minDis > y.minDis;
    });
    random_shuffle(st, st + T);
    for(int i = 0; i < T; i++){
        Search(st[i].s, st[i].t, st[i].id);
        //cout << i << endl;
    }
    GetCost();
}

void outAns(){
    cout << Y << endl;
    for(int i = 0; i < Y; i++){
        cout << edge[M - Y + i].v << ' ' << edge[M - Y + i].u << endl;
    }
    for(int i = 0; i < T; i++){
        cout << Ans[i].p << ' ' << Ans[i].edge.size() << ' ' << Ans[i].costD.size() << ' ';
        for(auto j : Ans[i].edge)cout << j << ' ';
        for(auto j : Ans[i].costD)cout << j << ' ';
        cout << endl;
    }
}

void outCost(){
    cout << "AC : " << ' ' << Y << ' ' << Y * 1000000 << endl;
    cout << "DC : " << ' ' << DC << ' ' << DC * 100 << endl;
    cout << "EC : " << ' ' << EC << ' ' << EC << endl;
    cout << "ALL COST : " << Y * 1000000 + DC * 100 + EC << endl;
}

void Work(){
    inData();
    Run();
    outAns();
}

void Test();

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);cout.tie(0);
    //Work();
    Test();
}
/*
7 10 6 4 6
0 1 5
0 2 3
1 3 1
1 4 2
2 3 5
2 3 4
2 5 6
3 6 3
5 6 2
5 6 5
0 6
2 4
2 4
2 4
2 4
2 4


 */


void Test(){
    N = 500, M = 1000, T = 10000, P = 80, D = 100;
    set<pair<int, int>>se;
    for(int i = 1; i < N; i++){
        se.insert({rand(), i});
    }
    int pre = 0;
    int count = 0;
    while(!se.empty()){
        int v = se.begin()->second;
        se.erase(se.begin());
        edge[count].u = pre, edge[count].v = v;
        edge[count].d = rand() % D;
        count++;
        pre = v;
    }
    while(count < M){
        int x = rand() % N, y = rand() % N;
        while(x == y)y = rand() % N;
        edge[count].u = x, edge[count].v = y;
        edge[count].d = rand() % D;
        count++;
    }
    for(int i = 0; i < T; i++){
        int x = rand() % N, y = rand() % N;
        while(x == y)y = rand() % N;
        st[i].s = x, st[i].t = y;
    }
    double start = clock();
    cout << getpid()  << endl;
    Run();
    outCost();
    cout << (int)(clock() - start) / 1000 << endl;
}