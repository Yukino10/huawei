#pragma GCC optimize(2)
#pragma GCC optimize(3,"Ofast","inline")
#include<bits/stdc++.h>
#include "unistd.h"
using namespace std;
typedef long long ll;
const int NMAX = 5000;
const int MMAX = 20005;
const int TMAX = 10000;
const int PMAX = 100;
struct ST{
    int s, t, minDis, id;
}st[TMAX];
struct Edge{
    int u, v, d;
}edge[MMAX];
struct DATA{
    int N, M, T, P, D;
    ST st[TMAX];
    Edge edge[MMAX];
}DATA;
int N, M, T, P, D;
ll Y, DC, EC;
map<pair<int, int>, int>minDis;

int16_t g[NMAX][MMAX];
int gLen[NMAX];

bool vis[NMAX];
int group[NMAX];
bool edgePMAP[MMAX][PMAX];
int edgePRE[NMAX];
struct ANS{
    int p;
    vector<int>edge;
    vector<int>costD;
}Ans[TMAX], BestAns[TMAX];
vector<Edge>AnsEdge;

int16_t gShrink[NMAX][MMAX];
int gShrinkLen[NMAX];
ll BestCost = INT64_MAX;

int q[NMAX];
int l, r;

void bfsOver(){
    for(int i = 0; i < r ; i++)vis[q[i]] = false;
    l = 0, r = 0;
}

int bfsPRE(int s, int t){
    q[r++] = s;
    vis[s]=true;
    int dis = 0, flag = 0;
    while(l < r){
        int tot = r - l;
        while(tot--){
            int x = q[l++];
            if(x == t){flag = 1; break;}
            for(int i = 0; i < gLen[x]; i++){
                int y = x ^ edge[g[x][i]].u ^ edge[g[x][i]].v;
                if(!vis[y])vis[y]=true, q[r++] = y;
            }
        }
        if(flag)break;
        dis++;
    }
    bfsOver();
    if(!flag)dis = 0;
    return dis;
}

int bfsSEA(int s, int t, int p){
    q[r++] = s;
    vis[s]=true;
    int dis = 0, flag = 0;
    while(l < r){
        int tot = r - l;
        while(tot--){
            int x = q[l++];
            if(x == t){flag = 1; break;}
            for(int i = 0; i < gLen[x]; i++){
                int y = x ^ edge[g[x][i]].u ^ edge[g[x][i]].v;
                if(!vis[y] && !edgePMAP[g[x][i]][p])vis[y]=true, q[r++] = y;
            }
        }
        if(flag)break;
        dis++;
    }
    bfsOver();
    if(!flag)dis = 0;
    return dis;
}

int  bfsSEL(int s, int t, int p){
    q[r++] = s;
    vis[s]=true;
    int dis = 0, flag = 0;
    while(l < r){
        int tot = r - l;
        while(tot--){
            int x = q[l++];
            if(x == t){flag = 1; break;}
            for(int i = 0; i < gLen[x]; i++){
                int y = x ^ edge[g[x][i]].u ^ edge[g[x][i]].v;
                if(!vis[y] && !edgePMAP[g[x][i]][p])vis[y]=true, q[r++] = y, edgePRE[y] = g[x][i];
            }
        }
        if(flag)break;
        dis++;
    }
    bfsOver();
    if(!flag)dis = 0;
    return dis;
}

void SelectEdge(int s, int t, int p, int stId){
    int len = bfsSEL(s, t ,p);
    int y = t;
    Ans[stId].p = p;
    Ans[stId].edge.resize(len);
    Ans[stId].costD.resize(len);
    while(y != s){
        int preEdge = edgePRE[y];
        Ans[stId].edge[--len] = preEdge;
        edgePMAP[preEdge][p] = true;
        y ^= edge[preEdge].v ^ edge[preEdge].u;
    }
}


void dfsShrinkPoint(int v, int p, int count){
    vis[v]=true;
    group[v] = count;
    for(int i = 0; i < gLen[v]; i++){
        int u = v ^ edge[g[v][i]].u ^ edge[g[v][i]].v;
        if(!vis[u] && !edgePMAP[g[v][i]][p]){
            vis[u]=true;
            group[u] = count;
            dfsShrinkPoint(u, p, count);
        }
    }
}

int bfsSHR(int s, int t){
    q[r++] = s;
    vis[s]=true;
    int dis = 0, flag = 0;
    while(l < r){
        int tot = r - l;
        while(tot--){
            int x = q[l++];
            if(x == t){flag = 1; break;}
            for(int i = 0; i < gShrinkLen[x]; i++){
                int y = x ^ group[edge[gShrink[x][i]].v] ^ group[edge[gShrink[x][i]].u];
                if(!vis[y]){
                    vis[y] = true;
                    q[r++] = y;
                    edgePRE[y] = gShrink[x][i];
                }
            }
        }
        if(flag)break;
        dis++;
    }
    bfsOver();
    if(!flag)dis = 0;
    return dis;
}

int AddEdgeP[PMAX][MMAX];
int AddEdgePLen[PMAX];

void GetAddEdgeP(int s, int t, int p){
    int count = 0;
    for(int i = 0; i < N; i++){
        if(!vis[i]){
            dfsShrinkPoint(i, p, count++);
        }
    }
    memset(vis, 0, sizeof(vis));
    for(int i = 0; i < count; i++){
        gShrinkLen[i] = 0;
    }
    for(int i = 0; i < M; i++){
        int x = group[edge[i].v], y = group[edge[i].u];
        if(x != y){
            gShrink[x][gShrinkLen[x]++] = i;
            gShrink[y][gShrinkLen[y]++] = i;
        }
    }
    bfsSHR(group[s], group[t]);

    AddEdgePLen[p] = 0;
    int y = group[t];
    while(y != group[s]){
        int preEdge = edgePRE[y];
        AddEdgeP[p][AddEdgePLen[p]++] = preEdge;
        y ^= group[edge[preEdge].v] ^ group[edge[preEdge].u] ;
    }
}

void copyAddEdge(int fa){
    edge[M] = edge[fa];
    edge[M].d = minDis[{edge[fa].u, edge[fa].v}];
    g[edge[fa].v][gLen[edge[fa].v]++] = M;
    g[edge[fa].u][gLen[edge[fa].u]++] = M;
    M++;
    Y++;
}
int AddEdgeIndex[PMAX];
void AddEdge(int s, int t, int stId){
    int PNum = 0;
    for(int i = 0; i < P; i++){
        GetAddEdgeP(s, t, i);
        PNum++;
        if(AddEdgePLen[i] == 1)break;
    }
    for(int i = 0;i < P; i++){
        AddEdgeIndex[i] = i;
    }
    sort(AddEdgeIndex, AddEdgeIndex + PNum, [](int x, int y){
        return AddEdgePLen[x] < AddEdgePLen[y];
    });
    for(int i = 0; i < AddEdgePLen[AddEdgeIndex[0]]; i++){
        copyAddEdge(AddEdgeP[AddEdgeIndex[0]][i]);
    }
    SelectEdge(s, t, AddEdgeIndex[0], stId);
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
        int index = 0;
        for(int j = 0; j < Ans[stId].edge.size(); j++){
            cost += edge[Ans[stId].edge[j]].d;
            if(cost > D){
                Ans[stId].costD[index++] = x;
                cost = edge[Ans[stId].edge[j]].d;
            }
            x ^= edge[Ans[stId].edge[j]].v ^ edge[Ans[stId].edge[j]].u;
        }
        Ans[stId].costD.resize(index);
        EC += Ans[stId].edge.size();
        DC += Ans[stId].costD.size();
    }
}

void inDATA(){
    cin >> DATA.N >> DATA.M >> DATA.T >> DATA.P >> DATA.D;
    for(int i = 0; i < DATA.M; i++){
        cin >> DATA.edge[i].u >> DATA.edge[i].v >> DATA.edge[i].d;
    }
    for(int i = 0; i < DATA.T; i++){
        cin >> DATA.st[i].s >> DATA.st[i].t;
    }
}

void init(){
    N = DATA.N;
    M = DATA.M;
    T = DATA.T;
    P = DATA.P;
    D = DATA.D;
    for(int i = 0; i < M; i++){
        edge[i].u = DATA.edge[i].u;
        edge[i].v = DATA.edge[i].v;
        edge[i].d = DATA.edge[i].d;
    }
    for(int i = 0; i < T; i++){
        st[i].s = DATA.st[i].s;
        st[i].t = DATA.st[i].t;
    }
    Y = 0, DC = 0, EC = 0;
    minDis.clear();
    for(int i = 0; i < N; i++)gLen[i] = 0;
    memset(vis, 0, N);
    memset(edgePMAP, 0, sizeof(edgePMAP));
    for(int i = 0; i < T; i++){
        Ans[i].p = 0;
        Ans[i].edge.clear();
        Ans[i].costD.clear();
    }
    for(int i = 0; i < N; i++)gShrinkLen[i] = 0;
}

int Value[TMAX];
int flag = 1;
void Run(){
    init();
    for(int i = 0; i < M; i++){
        if(!minDis[{edge[i].u, edge[i].v}])minDis[{edge[i].u, edge[i].v}] = edge[i].d;
        if(!minDis[{edge[i].v, edge[i].u}])minDis[{edge[i].v, edge[i].u}] = edge[i].d;
        minDis[{edge[i].v, edge[i].u}] = min({minDis[{edge[i].v, edge[i].u}], minDis[{edge[i].u, edge[i].v}], edge[i].d});
        minDis[{edge[i].u, edge[i].v}] = min({minDis[{edge[i].v, edge[i].u}], minDis[{edge[i].u, edge[i].v}], edge[i].d});
        g[edge[i].u][gLen[edge[i].u]++] = i;
        g[edge[i].v][gLen[edge[i].v]++] = i;
    }
    for(int i = 0; i < N; i++){
        sort(g[i], g[i] + gLen[i], [](int x, int y){
            return edge[x].d > edge[y].d;
        });
    }
    for(int i = 0; i < T; i++){
        st[i].id = i;
        if(flag)st[i].minDis = bfsPRE(st[i].s, st[i].t), Value[i] = st[i].minDis;
        else st[i].minDis = Value[i];
    }
    sort(st, st + T, [](ST x, ST y){
        return x.minDis < y.minDis;
    });
    random_shuffle(st, st + T);
    for(int i = 0; i < T; i++){
        Search(st[i].s, st[i].t, st[i].id);
    }

    GetCost();
    flag = 0;
}


void outAns(){
    cout << AnsEdge.size() << endl;
    for(auto i : AnsEdge){
        cout << i.u << ' ' << i.v << ' ' << endl;
    }
    for(int i = 0; i < T; i++){
        cout << BestAns[i].p << ' ' << BestAns[i].edge.size() << ' ' << BestAns[i].costD.size() << ' ';
        for(auto j : BestAns[i].edge)cout << j << ' ';
        for(auto j : BestAns[i].costD)cout << j << ' ';
        cout << endl;
    }
}

ll getCost(){
    return Y * 1000000 + DC * 100 + EC;
}

void outCost(){
    cout << "AC : " << ' ' << Y << ' ' << Y * 1000000 << endl;
    cout << "DC : " << ' ' << DC << ' ' << DC * 100 << endl;
    cout << "EC : " << ' ' << EC << ' ' << EC << endl;
    cout << "ALL COST : " << BestCost << endl;
}

void RUN(int num){
    for(int i = 0; i < num; i++){
        Run();
        ll nowCost = getCost();
        if(nowCost < BestCost){
            for(int i = 0; i < T; i++){
                BestAns[i] = Ans[i];
            }
            AnsEdge.clear();
            for(int i = DATA.M; i < M; i++){
                AnsEdge.push_back(edge[i]);
            }
            BestCost = nowCost;
        }
    }
}


void Work(){
    inDATA();
    RUN(50);
    outAns();
}

void Test();

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);cout.tie(0);
    Work();
    //Test();
//    vector<int>v;
//    for(int i = 0; i < NMAX; i++)v.push_back(i);
//    int a[NMAX];
//    double start = clock();
//    for(int i = 0; i < 1000000; i++){
//        for(auto j : v){
//            int x = j;
//        }
//        for(int j = 0; j < NMAX; j++){
//            int x = v[j];
//        }
//    }
//    cout << (int)(clock() - start) / 1000 << endl;
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
    DATA.N = 500, DATA.M = 5000, DATA.T = 10000, DATA.P = 80, DATA.D = 100;
    set<pair<int, int>>se;
    for(int i = 1; i < DATA.N; i++){
        se.insert({rand(), i});
    }
    int pre = 0;
    int count = 0;
    while(!se.empty()){
        int v = se.begin()->second;
        se.erase(se.begin());
        DATA.edge[count].u = pre, DATA.edge[count].v = v;
        DATA.edge[count].d = rand() % DATA.D;
        count++;
        pre = v;
    }
    while(count < DATA.M){
        int x = rand() % DATA.N, y = rand() % DATA.N;
        while(x == y)y = rand() % DATA.N;
        DATA.edge[count].u = x, DATA.edge[count].v = y;
        DATA.edge[count].d = rand() % DATA.D;
        count++;
    }
    for(int i = 0; i < DATA.T; i++){
        int x = rand() % DATA.N, y = rand() % DATA.N;
        while(x == y)y = rand() % DATA.N;
        DATA.st[i].s = x, DATA.st[i].t = y;
    }
    double start = clock();
    cout << getpid()  << endl;
    RUN(1);
    outCost();
    cout << (int)(clock() - start) / 1000 << endl;
}