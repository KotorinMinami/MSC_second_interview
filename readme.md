# 路径统计题解

##  变量定义
 - 树剖的深度dep,重儿子son,子树大小siz，链顶top,父亲节点fa
 - 定义一个struct为辅助变量sma
 - 记`sma[i].ans1`为节点i所属子树中，top[i]到i节点的节点外其余节点的最小编号。
 - 记`sma[i].d1`为节点i子树中的节点（向下）的最小编号。
 - 记`sma[i].d2`为节点i子树中的节点（向下）的次小编号（与d1来自不同子树）。
 - 记`sma[i].up`为节点i所在重链中节点（向上）的最小编号。
 - 记`sma[i].ans`为节点i所属子树除去i的子树的情况下，top[i]到i节点的节点外其余节点的最小编号。
 - 以1为根节点，记`m`为根节点子树下的第三个最小编号，同样与d1,d2来自不同子树

## 树剖及辅助变量  
  以1为根节点，第一遍dfs算出dep,son,d1,d2,siz
```
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
```
第二遍dfs算出top,ans,up，其中为了后续讨论便利，ans与up需要与根节点1进行“切割”，up与ans根据是否为top节点分类讨论，up可直接合并；

对于ans：
- 首先可能取到它父亲的ans
- 其次是其兄弟节点的最小值
对于兄弟节点的最小值，若i及其子树的最小值与其父亲的d1不相等，则为这个数，若相等，则应等于d2。
```
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
```
之后直接通过ans和其子树最小值得出ans1，遍历根节点来获得m
```
for(int i=1 ; i<=n ; i++)   sma[i].ans1 = std::min(sma[i].ans , sma[i].d1);
for(auto i : edge[1])
{
    int mini = std::min(sma[i].d1 , i);
    if(mini != sma[1].d1 && mini != sma[1].d2)    
        m = std::min(m , mini);
}
```
## 求解询问
  记`xans`为所求一点x到现在跳上的链的链首的路径上节点外的局部最小值（即同样在对应子树范围中），`xup`为该路径上最小值，`yans`,`yup`同上，不过是对于y。
  
  对于得到的每一个（x ，y）通过向上跳链的方法求lca,每跳上一条链时更新对应的pans与pup，更新时同样进行讨论：
   - 若要跳到节点1则不用合并
   - 若往上的节点的ans1刚好在经过的路径中，则答案会在往上节点的ans,其往下的d2,及之前的pans中取到
   - 若ans1没有在经过路径中，则新的pans为ans1
  ```
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
  ```
  当得到lca后：
  - 若lca不为1,则可直接输出1
  - 若lca为1 ：
  先将为统计到根节点的的链更新到根节点然后进行最后的讨论
  - 若根节点的d1在其中一条路中，取出这条路的pans，否则直接返回d1。
  - 若在一条路的pans被取出的情况下，根节点的d2在另一条路上，则取出另一条路的pans，否则直接返回d2与取出的pans的最小值。
  - 在两条路的ans都被取出的情况下，返回第三小和这两个值的最小值
```
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
        else return std::min(sma[1].d2 , c1);
        return std::min(m , std::min(c1 , c2));
    }
}
```
