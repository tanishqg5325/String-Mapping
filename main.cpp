#include <bits/stdc++.h>
#define pb push_back
using namespace std;
typedef long long ll;

char dash = '-';
int v, k, mc[27][27];   // v = |V|, k = # strings, mc = matching cost matrix
ll cc;                  // cc = conversion cost
map<char, int> vocab;   // mapping of vocabulary to {0, 1, 2 ... |V|-1} and dash to |V|
vector<int> act_len;    // original length of strings

class Node
{
public:
    int n; ll cost;
    vector<string> seq;

    // returns true if dash is present in all strings at position pos
    bool isDashAtPosition(int pos)
    {
        assert(pos < n);
        for(int i=0;i<k;i++)
            if(seq[i][pos] != dash)
                return false;
        return true;
    }

    // matching cost of jth character of idth string with other strings
    ll getCost(int id, int j)
    {
        assert(id < k && j < n);
        ll ans = 0; int idx = vocab[seq[id][j]];
        for(int i=0;i<k;i++)
            ans += mc[idx][vocab[seq[i][j]]];
        return ans;
    }

    void computeOwnCost()
    {
        cost = 0;
        for(int i=0;i<k;i++)
            for(int j=0;j<n;j++)
                cost += getCost(i, j);
        cost /= 2;
        for(int i=0;i<k;i++)
            cost += (cc * (n - act_len[i]));
        for(int j=0;j<n;j++)
            if(isDashAtPosition(j))
                cost -= (cc * k);
    }

    // computes new cost when j1 th and j2th character of ith string are swapped
    ll computeNewCost(int i, int j1, int j2)
    {
        ll ans = cost;
        if(isDashAtPosition(j1)) ans += (cc * k);
        if(isDashAtPosition(j2)) ans += (cc * k);
        ans -= (getCost(i, j1) + getCost(i, j2));
        swap(seq[i][j1], seq[i][j2]);
        if(isDashAtPosition(j1)) ans -= (cc * k);
        if(isDashAtPosition(j2)) ans -= (cc * k);
        ans += (getCost(i, j1) + getCost(i, j2));
        swap(seq[i][j1], seq[i][j2]);
        return ans;
    }

    bool moveToBestNeighbour()
    {
        ll min_cost = cost, curr_cost; int id, pos1, pos2;
        for(int i=0;i<k;i++)
            for(int j=0;j<n;j++)
                if(seq[i][j] != dash)
                {
                    int t = j-1;
                    while(t >= 0 && seq[i][t] == dash)
                    {
                        curr_cost = computeNewCost(i, j, t);
                        if(curr_cost < min_cost)
                        {
                            min_cost = curr_cost;
                            id = i; pos1 = j; pos2 = t;
                        }
                        t--;
                    }
                    t = j+1;
                    while(t < n && seq[i][t] == dash)
                    {
                        curr_cost = computeNewCost(i, j, t);
                        if(curr_cost < min_cost)
                        {
                            min_cost = curr_cost;
                            id = i; pos1 = j; pos2 = t;
                        }
                        t++;
                    }
                }
        if(min_cost == cost) return false;
        swap(seq[id][pos1], seq[id][pos2]);
        cost = min_cost;
        return true;
    }

    void print()
    {
        vector<int> v;
        for(int i=0;i<n;i++) if(!isDashAtPosition(i)) v.pb(i);
        for(int i=0;i<k;i++)
        {
            for(int j : v) cout<<seq[i][j];
            cout<<"\n";
        }
    }
};

string random_string(const string &str, int N)
{
    int n = str.size(); assert(N >= n);
    vector<int> v(N); for(int i=0;i<N;i++) v[i] = i;
    shuffle(v.begin(), v.end(), default_random_engine(time(0)));
    sort(v.begin(), v.begin() + n);
    string ans = ""; int k = 0;
    for(int i=0;i<N;i++)
    {
        if(k < n && i == v[k]) ans += str[k++];
        else ans += dash;
    }
    return ans;
}

Node random_restart(vector<string> &strs, int N)
{
    Node start_node; start_node.n = N; start_node.seq.resize(k);
    for(int i=0;i<k;i++)
        start_node.seq[i] = random_string(strs[i], N);
    start_node.computeOwnCost();
    return start_node;
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);
    double max_time; cin>>max_time; max_time *= 60;
    cin>>v; char c; cin>>c; int N = 0;
    for(int i=0;i<v-1;i++)
    {
        vocab[c] = i;
        cin>>c>>c;
    }
    vocab[c] = v-1; vocab[dash] = v;
    cin>>k; vector<string> input_str(k); act_len.resize(k);
    for(int i=0;i<k;i++)
    {
        cin>>input_str[i];
        act_len[i] = input_str[i].size();
        N = max(N, act_len[i]);
    }
    cin>>cc;
    for(int i=0;i<=v;i++)
        for(int j=0;j<=v;j++)
            cin>>mc[i][j];
    cin>>c; assert(c == '#');
    Node best_node = random_restart(input_str, 2*N);
    while(((double)clock()/CLOCKS_PER_SEC) < 0.9 * max_time)
    {
        Node node = random_restart(input_str, 2*N);
        while(node.moveToBestNeighbour()) {}
        if(node.cost < best_node.cost) best_node = node;
    }
    best_node.print();
    //cout<<best_node.cost<<endl;
    return 0;
}