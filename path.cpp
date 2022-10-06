#include<iostream>
#include<cstring>
#include<utility>
#include<vector>
#include<bitset>
#include<cstdio>

#define ans(i) std::min(sma[i].ans , sma[i].d1)
const int maxn = 1000000;
const int inf = 0xfffffff;

typedef struct node
{
    /* data */
    int up , d1 , d2 , ans , ans1; 
}node;

int top[maxn] , fa[maxn] , dep[maxn] , siz[maxn] ,son[maxn];
node sma[maxn] ; 
int n , q , a , b , m=inf , last=0;
std::vector<std::vector<int> > edge;
std::bitset<maxn> v;

void rep_li(int x , int i)
{
    sma[x].d2 = std::min(sma[x].d2 , i);
    if(sma[x].d1 > sma[x].d2)   std::swap(sma[x].d1 , sma[x].d2);
}

void dfs1(int x , int depth)
{
    sma[x].d1 = sma[x].d2 = inf;
    v[x] = 1;
    dep[x] = depth , siz[x] = 1 , son[x] = 0;
    for (auto i : edge[x])
    {
        if(v[i])   fa[x] = i;
        else
        {
            rep_li(x , i);
            dfs1(i , depth+1);
            rep_li(x , sma[i].d1);
            siz[x]+=siz[i];
            son[x] = (siz[son[x]] > siz[i] ? son[x] : i);
        }
    }
}

void dfs2(int x , int tt)
{
    top[x] = tt;
    if(x==tt)
    {
        sma[x].up = (tt==1 ? inf : tt);
        sma[x].ans = inf;
    }   
    else    
    {
        if(fa[x]==1)    sma[x].ans = inf;
        else
        {
            sma[x].ans = sma[fa[x]].ans;
            int minx = std::min(x , sma[x].d1);
            if(minx == sma[fa[x]].d1)   sma[x].ans = std::min(sma[x].ans , sma[fa[x]].d2);
            else    sma[x].ans = std::min(sma[x].ans , sma[fa[x]].d1);
        }
        sma[x].up = std::min(sma[fa[x]].up , x);
    } 
    if(son[x])
    {
        dfs2(son[x] , tt);
        for(auto i : edge[x])
        {
            if(i != son[x] && i != fa[x])     dfs2(i , i);
        }
    }
}

void merge(int &p , int &pans , int &pup)
{
    if(fa[top[p]]==1)   
    {
        p=1;
        return;
    }
    if(sma[fa[top[p]]].ans1 == pup)
    {
        pans = std::min(sma[p].ans1 , sma[fa[top[p]]].ans);
        pans = std::min(pans , sma[fa[top[p]]].d2);
    }
    else
    {
        pans = sma[fa[top[p]]].ans1;
    }
    pup = std::min(pup , sma[fa[top[p]]].up);
    p=fa[top[p]];
}

int lca(int x , int y)
{
    int xans = sma[x].ans1 , xup = sma[x].up , yans = sma[y].ans1 , yup = sma[y].up;
    while(top[x] != top[y])
    {
        if(dep[top[x]] > dep[top[y]])
        {
            merge(x , xans , xup);
        }
        else
        {
            merge(y , yans , yup);
        }
    }
    if((dep[x] > dep[y] ? y : x) != 1)     return 1;
    else
    {
        if(x > y)
        {
            if(sma[x].ans1 == xup)
            {
                xans = std::min(sma[x].ans1 , sma[x].ans);
                xans = std::min(xans , sma[x].d2);
            }
            else
            {   
                xans = sma[x].ans1;
            }
            xup = std::min(xup , sma[x].up);
        }
        else if(y > x)
        {
            if(sma[y].ans1 == yup)
            {
                yans = std::min(sma[y].ans1 , sma[y].ans);
                yans = std::min(yans , sma[y].d2);
            }
            else
            {   
                yans = sma[y].ans1;
            }
            yup = std::min(yup , sma[y].up);
        }
        int c1 , c2;
        if(sma[1].d1 == xup)    c1 = xans;
        else if(sma[1].d1 == yup)   c1 = yans;   
        else    return sma[1].d1;
        if(sma[1].d2 == xup)    c2 = xans;
        else if(sma[1].d2 == yup)   c2 = yans;
        else return sma[1].d2;
        return std::min(m , std::min(c1 , c2));
    }
}

int main()
{
    //freopen("in.txt","r",stdin);
    //freopen("out.txt","w",stdout);
    while(scanf("%d%d" , &n ,&q) != EOF)
    {
        edge.clear();
        edge.assign(n+1 , std::vector<int>());
        memset(top , 0 , sizeof(top));
        memset(fa , 0 , sizeof(fa));
        memset(dep , 0 , sizeof(dep));
        memset(siz , 0 , sizeof(siz));
        memset(son , 0 , sizeof(son));
        memset(sma , 0 , sizeof(sma));
        v.reset();
        last = 0;
        for(int i=1 ; i<n ; i++)
        {
            scanf("%d%d" , &a , &b);
            edge[a].emplace_back(b);
            edge[b].emplace_back(a);
        }
        dfs1(1 , 1);
        dfs2(1 , 1);
        for(int i=1 ; i<=n ; i++)   sma[i].ans1 = std::min(sma[i].ans , sma[i].d1);
        for(auto i : edge[1])
        {
            int mini = std::min(sma[i].d1 , i);
            if(mini != sma[1].d1 && mini != sma[1].d2)    
                m = std::min(m , mini);
        }   
        while(q--)
        {
            scanf("%d%d" , &a , &b);
            a^=last;
            b^=last;
            last = lca(a , b);
            printf("%d\n" , last);
        }
    }
    return 0;
}