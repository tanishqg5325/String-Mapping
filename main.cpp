#include <bits/stdc++.h>
#define pb push_back
#define X first
#define Y second
using namespace std;
typedef long long ll;

ll mod = 1000000007;

char dash = '-';
int v, k, mc[27][27];   // v = |V|, k = # strings, mc = matching cost matrix
ll cc;                  // cc = conversion cost
map<char, int> vocab;   // mapping of vocabulary to {0, 1, 2 ... |V|-1} and dash to |V|
vector<int> act_len;    // original length of strings
ll seed = time(0);
int cnt_tabu = 0;

inline void shuffle_vector(vector<int> &vec)
{
    shuffle(vec.begin(), vec.end(), default_random_engine(seed));
    seed = (seed * 43) % mod;
}

class Node
{
public:
    int n; ll cost;
    vector<string> seq;

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
    }

    ll computeNewCost(int i, int j1, int j2)
    {
        ll ans = cost;
        ans -= (getCost(i, j1) + getCost(i, j2));
        swap(seq[i][j1], seq[i][j2]);
        ans += (getCost(i, j1) + getCost(i, j2));
        swap(seq[i][j1], seq[i][j2]);
        return ans;
    }

    ll getCostOfBestNeighbour(int &id, int &pos1, int &pos2)
    {
        ll min_cost = LLONG_MAX, curr_cost;
        vector<int> v1(k); for(int i=0;i<k;i++) v1[i] = i;
        vector<int> v2(n); for(int i=0;i<n;i++) v2[i] = i;
        shuffle_vector(v1);
        for(int i : v1)
        {
            shuffle_vector(v2);
            for(int j : v2)
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
        }
        return min_cost;
    }

    bool isEqual(Node &other)
    {
        for(int i=0;i<k;i++)
            for(int j=0;j<n;j++)
                if(seq[i][j] != other.seq[i][j])
                    return false;
        return true;
    }

    Node new_node(int i, int j1, int j2)
    {
        Node res; res.n = n; res.cost = cost;
        res.seq = seq;
        swap(res.seq[i][j1], res.seq[i][j2]);
        return res;
    }

    bool isPresentInList(list<Node> &l, Node &n)
    {
        for(Node &tmp : l) if(n.isEqual(tmp)) return true;
        return false;
    }

    bool moveToBestNeighbour()
    {
        int id, j1, j2;
        ll min_cost = getCostOfBestNeighbour(id, j1, j2);
        if(min_cost < cost)
        {
            swap(seq[id][j1], seq[id][j2]);
            cost = min_cost;
            return true;
        }
        if(min_cost > cost) return false;
        list<Node> tabu;
        tabu.push_back(*this);
        Node curr_node = new_node(id, j1, j2);
        tabu.push_back(curr_node);
        int cnt = 0;
        while(cnt < 100)
        {
            min_cost = curr_node.getCostOfBestNeighbour(id, j1, j2);
            if(min_cost < cost)
            {
                cnt_tabu++;
                swap(curr_node.seq[id][j1], curr_node.seq[id][j2]);
                seq = curr_node.seq;
                cost = min_cost;
                return true;
            }
            if(min_cost > cost) return false;
            curr_node = curr_node.new_node(id, j1, j2);
            if(isPresentInList(tabu, curr_node)) return false;
            tabu.push_back(curr_node);
            if(tabu.size() > 50) tabu.pop_front();
            cnt++;
        }
        return false;
    }

    bool moveToFirstBestNeighbour()
    {
        ll curr_cost; int t;
        vector<int> v1(k); for(int i=0;i<k;i++) v1[i] = i;
        vector<int> v2(n); for(int i=0;i<n;i++) v2[i] = i;
        shuffle_vector(v1);
        for(int i : v1)
        {
            shuffle_vector(v2);
            for(int j : v2)
                if(seq[i][j] != dash)
                {
                    t = j-1;
                    while(t >= 0 && seq[i][t] == dash)
                    {
                        curr_cost = computeNewCost(i, j, t);
                        if(curr_cost < cost)
                        {
                            swap(seq[i][j], seq[i][t]);
                            cost = curr_cost;
                            return true;
                        }
                        t--;
                    }
                    t = j+1;
                    while(t < n && seq[i][t] == dash)
                    {
                        curr_cost = computeNewCost(i, j, t);
                        if(curr_cost < cost)
                        {
                            swap(seq[i][j], seq[i][t]);
                            cost = curr_cost;
                            return true;
                        }
                        t++;
                    }
                }
        }
        return false;
    }

    void print()
    {
        for(int i=0;i<k;i++) cout<<seq[i]<<"\n";
    }
};

string random_string(const string &str, int N)
{
    int n = str.size(); assert(N >= n);
    vector<int> v(N); for(int i=0;i<N;i++) v[i] = i;
    shuffle_vector(v);
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
    if(max_time > 20) max_time -= 5;
    else max_time *= 0.95;
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
    ll cnt = 0;
    Node best_node = random_restart(input_str, N);
    vector<pair<ll, int>> cost_len(N+1);
    if(N > 500)
    {
        int i = N+1;
        while(((double)clock()/CLOCKS_PER_SEC) < max_time)
        {
            Node node = random_restart(input_str, i++);
            while(((double)clock()/CLOCKS_PER_SEC) < max_time && node.moveToFirstBestNeighbour()) cnt++;
            if(node.cost < best_node.cost) best_node = node;
        }
        best_node.print();
    	#ifdef PROD
        cout<<best_node.cost<<endl;
        cout<<cnt<<endl;
        cout<<((double)clock()/CLOCKS_PER_SEC)<<endl;
        cout<<cnt_tabu<<"\n";
    	#endif
        return 0;
    }
    for(int i=N;i<=2*N;i++)
    {
        cost_len[i-N] = {(ll)0, i};
        for(int j=0;j<10;j++)
        {
            Node node = random_restart(input_str, i);
            while(((double)clock()/CLOCKS_PER_SEC) < max_time && node.moveToBestNeighbour()) cnt++;
            if(node.cost < best_node.cost) best_node = node;
            cost_len[i-N].X += node.cost;
        }
        if(((double)clock()/CLOCKS_PER_SEC) >= max_time) break;
    }
    sort(cost_len.begin(), cost_len.end());
    vector<int> best_lens;
    for(int i=0;i<min(5, N+1);i++) best_lens.pb(cost_len[i].Y);
    while(((double)clock()/CLOCKS_PER_SEC) < max_time)
    {
        for(int i : best_lens)
        {
            Node node = random_restart(input_str, i);
            while(((double)clock()/CLOCKS_PER_SEC) < max_time && node.moveToBestNeighbour()) cnt++;
            if(node.cost < best_node.cost) best_node = node;
        }
    }
    best_node.print();
	#ifdef PROD
    cout<<best_node.cost<<endl;
    cout<<cnt<<endl;
    cout<<((double)clock()/CLOCKS_PER_SEC)<<endl;
    cout<<cnt_tabu<<"\n";
	#endif
    return 0;
}
