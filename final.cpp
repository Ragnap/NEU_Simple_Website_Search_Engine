#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
typedef unsigned int ull;
typedef pair<int, int> pil;
const ll ll_INF = 0x3f3f3f3f3f3f3f3f;
const ll ll_MAX = 0x7fffffffffffffff;
const int int_INF = 0x3f3f3f3f;
const int int_MAX = 0x7fffffff;
const double EPS = 1e-7;
const double PI = acos(-1.0);
const int dy[8] = {-1, 0, 1, 0, 1, -1, 1, -1};
const int dx[8] = {0, -1, 0, 1, 1, -1, -1, 1};
#define R register
#define _chmax(a, b) (a = max(a, (b)))
#define _chmin(a, b) (a = min(a, (b)))
#define _abs(a) ((a) > 0 ? (a) : -(a))

inline int fsign(double x) {
    return ((fabs(x) < EPS) ? 0 : ((x < 0) ? -1 : 1));
}
inline ll _r() {
    ll x = 0, f = 1;
    char c = getchar();
    while (c > '9' || c < '0') { if (c == '-')f = -1; c = getchar(); }
    while (c >= '0' && c <= '9')x = (x << 3) + (x << 1) + (c ^ 48), c = getchar();
    return f == -1 ? -x : x;
}

const int MOD = 1e9+7;
const int MX = 1000010;
void Init() {

}

void solve() {

}
int main() {
    std::ios::sync_with_stdio(false);
#ifdef LOCAL
    freopen("./Input_data/data.in", "r", stdin);
    freopen("./Output_data/data.out", "w", stdout);
#endif
    int t = 1;
    //cin>> t;
    while (t--) {
        Init();
        solve();
    }
    return 0;
}