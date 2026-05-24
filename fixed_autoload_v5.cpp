#include <bits/stdc++.h>
using namespace std;

#ifdef _WIN32
#  include <windows.h>
#endif

static const int MAXN = 1000 + 10;
static const long long INF = (1LL << 60);

struct Intersection {
    vector<string> streetname;
    vector<string> buildings;
} ints[MAXN];

struct Intersection_Edge {
    int to, nxt;
    int walk_dis, car_dis;
} it_edg[MAXN];

struct Street {
    string name;
    int flag = 0, length = 0;
} edg[MAXN];

struct Building {
    string name;
    vector<int> crossing;
    vector<string> discribe;
} blg[MAXN];

int ecnt = 0, la[MAXN];
int BuildingNum = 0, StreetNum = 0, InterNum = 0;

static void add_edge(int u, int v, int w_walk, int w_car) {
    ++ecnt;
    it_edg[ecnt].to = v;
    it_edg[ecnt].nxt = la[u];
    la[u] = ecnt;
    it_edg[ecnt].walk_dis = w_walk;
    it_edg[ecnt].car_dis = w_car;
}

static void ScanMap() {
    int M;
    if (!(cin >> M)) {
        return;
    }
    StreetNum = M;

    for (int i = 1; i <= M; i++) cin >> edg[i].name;
    for (int i = 1; i <= M; i++) cin >> edg[i].length >> edg[i].flag;

    cin >> M;
    InterNum = M;
    for (int i = 1; i <= M; i++) {
        int x;
        cin >> x;
        for (int j = 0; j < x; j++) {
            string ss;
            cin >> ss;
            ints[i].buildings.push_back(ss);
        }
        cin >> x;
        for (int j = 0; j < x; j++) {
            string ss;
            cin >> ss;
            ints[i].streetname.push_back(ss);
        }
    }

    cin >> M;
    for (int i = 0; i < M; i++) {
        int u, v, w1, w2;
        cin >> u >> v >> w1 >> w2;
        add_edge(u, v, w1, w2);
    }

    cin >> BuildingNum;
    for (int i = 1; i <= BuildingNum; i++) {
        cin >> blg[i].name;
        int x;
        cin >> x;
        for (int j = 0; j < x; j++) {
            int u;
            cin >> u;
            blg[i].crossing.push_back(u);
        }
        cin >> x;
        for (int j = 0; j < x; j++) {
            string ss;
            cin >> ss;
            blg[i].discribe.push_back(ss);
        }
    }
}

static int menu_input() {
    cout << "To check building information,please enter 1\n";
    cout << "To plan the path,please enter 2\n";
    cout << "To terminate the program,enter 0\n";
    cout.flush();

    int opt;
    if (!(cin >> opt)) {
        // EOF
        return 0;
    }
    if (opt == 0) {
        cout << "Do you think this program is useful?\n";
        cout << "Yes,enter Y.No,enter N\n";
        string ask;
        if (!(cin >> ask)) return 0;
        if (!ask.empty() && (ask[0] == 'Y' || ask[0] == 'y')) cout << "Thank you!\n";
        else cout << "OK,bye.\n";
        return 0;
    }
    while (opt < 1 || opt > 2) {
        cout << "Illegal input!\n";
        if (!(cin >> opt)) return 0;
        if (opt == 0) return 0;
    }
    return opt;
}

long long car_dist[MAXN][MAXN], walk_dist[MAXN][MAXN];
bool inq[MAXN];
queue<int> q;

static void spfa(int s) {
    for (int i = 1; i <= InterNum; i++) {
        car_dist[s][i] = INF;
        walk_dist[s][i] = INF;
        inq[i] = false;
    }

    while (!q.empty()) q.pop();
    q.push(s);
    inq[s] = true;
    car_dist[s][s] = 0;

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        inq[u] = false;
        for (int e = la[u]; e; e = it_edg[e].nxt) {
            int v = it_edg[e].to;
            long long w = it_edg[e].car_dis;
            if (car_dist[s][v] > car_dist[s][u] + w) {
                car_dist[s][v] = car_dist[s][u] + w;
                if (!inq[v]) {
                    q.push(v);
                    inq[v] = true;
                }
            }
        }
    }

    for (int i = 1; i <= InterNum; i++) inq[i] = false;
    while (!q.empty()) q.pop();
    q.push(s);
    inq[s] = true;
    walk_dist[s][s] = 0;

    while (!q.empty()) {
        int u = q.front();
        q.pop();
        inq[u] = false;
        for (int e = la[u]; e; e = it_edg[e].nxt) {
            int v = it_edg[e].to;
            long long w = it_edg[e].walk_dis;
            if (walk_dist[s][v] > walk_dist[s][u] + w) {
                walk_dist[s][v] = walk_dist[s][u] + w;
                if (!inq[v]) {
                    q.push(v);
                    inq[v] = true;
                }
            }
        }
    }
}

static void Init() {
    // 让你能分辨“卡住”还是“在算”
    cerr << "[Init] Precomputing shortest paths for " << InterNum << " intersections..." << endl;
    for (int i = 1; i <= InterNum; i++) {
        if (i == 1 || i % 50 == 0 || i == InterNum) {
            cerr << "[Init] " << i << "/" << InterNum << endl;
        }
        spfa(i);
    }
    cerr << "[Init] Done." << endl;
}

static int lcs_len(const string& a, const string& b) {
    const int n = (int)a.size(), m = (int)b.size();
    vector<int> prev(m + 1, 0), cur(m + 1, 0);
    for (int i = 1; i <= n; i++) {
        cur[0] = 0;
        for (int j = 1; j <= m; j++) {
            if (a[i - 1] == b[j - 1]) cur[j] = prev[j - 1] + 1;
            else cur[j] = max(prev[j], cur[j - 1]);
        }
        swap(prev, cur);
    }
    return prev[m];
}

static int GetBuilding(const string& st) {
    // 1) 精确匹配（最符合直觉）
    for (int i = 1; i <= BuildingNum; i++) {
        if (blg[i].name == st) return i;
    }

    // 2) 名称子串匹配：输入“土交”也能命中“土交学院”
    for (int i = 1; i <= BuildingNum; i++) {
        if (blg[i].name.find(st) != string::npos) return i;
    }

    // 3) 描述子串匹配：例如地图里建筑名是“4号楼”，但描述里写了“数学学院”
    for (int i = 1; i <= BuildingNum; i++) {
        for (const auto& d : blg[i].discribe) {
            if (d.find(st) != string::npos) return i;
        }
    }

    // 4) LCS 模糊匹配兜底
    int best = 1;
    int bestScore = -1;
    for (int i = 1; i <= BuildingNum; i++) {
        int s = lcs_len(st, blg[i].name);
        if (s > bestScore) {
            bestScore = s;
            best = i;
        }
    }

    // 如果分数很低，提示一下（不打断流程）
    if (bestScore <= 1) {
        cerr << "[Warn] Building '" << st << "' not found. Using closest match: '" << blg[best].name
             << "' (score=" << bestScore << ")\n";
    }
    return best;
}

static int targetT;
static vector<int> intpath;
static bool instk[MAXN];

static bool findpath(int now, int S, int opt_walk) {
    instk[now] = true;
    intpath.push_back(now);
    if (now == targetT) return true;

    for (int e = la[now]; e; e = it_edg[e].nxt) {
        int v = it_edg[e].to;
        if (instk[v]) continue;
        long long w = opt_walk ? it_edg[e].walk_dis : it_edg[e].car_dis;
        auto& dist = opt_walk ? walk_dist : car_dist;
        // 从起点 S 正向沿最短路走：dist[S][now] + w == dist[S][v]
        if (dist[S][now] + w == dist[S][v]) {
            if (findpath(v, S, opt_walk)) return true;
        }
    }

    instk[now] = false;
    intpath.pop_back();
    return false;
}

static void printpath(const string& startName, const string& endName) {
    cout << "Start Building: " << startName << '\n';
    for (int x : intpath) cout << "Go to the intersection: " << x << '\n';
    cout << "Terminal Building: " << endName << '\n';
}

static void GetPath(const string& startName, const string& endName, int S, int T, int opt_walk) {
    targetT = T;
    intpath.clear();
    memset(instk, 0, sizeof(instk));

    if ((opt_walk ? walk_dist : car_dist)[S][T] >= INF / 2) {
        cout << (opt_walk ? "Walking" : "Driving") << " solution\n";
        cout << "No route found.\n";
        return;
    }

    if (findpath(S, S, opt_walk)) {
        cout << (opt_walk ? "Walking" : "Driving") << " solution\n";
        printpath(startName, endName);
        cout << "Total distance: " << (opt_walk ? walk_dist[S][T] : car_dist[S][T]) << "\n";
    } else {
        cout << (opt_walk ? "Walking" : "Driving") << " solution\n";
        cout << "No route found.\n";
    }
}

static void Service() {
    int opt;
    while ((opt = menu_input())) {
        if (opt == 1) {
            cout << "Please enter the name of the building to be queried.\n";
            cout.flush();
            string st;
            if (!(cin >> st)) return;
            int now = GetBuilding(st);
            for (const auto& s : blg[now].discribe) cout << s << '\n';
        } else if (opt == 2) {
            cout << "Please enter the building name of the starting point.\n";
            cout.flush();
            string st, tt;
            if (!(cin >> st)) return;
            int Start = GetBuilding(st);
            cout << "Please enter the building name of the endpoint.\n";
            cout.flush();
            if (!(cin >> tt)) return;
            int Terminal = GetBuilding(tt);

            long long bestCar = INF, bestWalk = INF;
            int CarBestStart = -1, CarBestTermin = -1;
            int WalkBestStart = -1, WalkBestTermin = -1;

            for (int S : blg[Start].crossing) {
                for (int T : blg[Terminal].crossing) {
                    if (car_dist[S][T] < bestCar) {
                        bestCar = car_dist[S][T];
                        CarBestStart = S;
                        CarBestTermin = T;
                    }
                    if (walk_dist[S][T] < bestWalk) {
                        bestWalk = walk_dist[S][T];
                        WalkBestStart = S;
                        WalkBestTermin = T;
                    }
                }
            }

            if (CarBestStart == -1 || WalkBestStart == -1) {
                cout << "No available intersections for path planning.\n";
                continue;
            }

            GetPath(blg[Start].name, blg[Terminal].name, CarBestStart, CarBestTermin, 0);
            GetPath(blg[Start].name, blg[Terminal].name, WalkBestStart, WalkBestTermin, 1);
        }
    }
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

#ifdef _WIN32
    // 让 Windows 控制台用 UTF-8（避免看到“鍔涘惎...”这种乱码）
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    setlocale(LC_ALL, "");

    // 你想要的“程序内部自动读 txt”：这里默认就读当前目录的 map.txt
    // 若找不到 map.txt，我们会明确报错并退出（避免你以为“卡死”，实际上是在等输入）。
    const string defaultMap = "map.txt";
    const string mapPath = (argc >= 2 ? string(argv[1]) : defaultMap);

    ifstream fin(mapPath);
    if (!fin) {
        cerr << "[Error] Failed to open map file: " << mapPath << "\n";
        cerr << "- Put 'map.txt' next to the executable, OR run: ./app your_map.txt\n";
        return 1;
    }

    cerr << "[Load] Reading map from: " << mapPath << endl;
    streambuf* old = cin.rdbuf(fin.rdbuf());
    ScanMap();
    cin.rdbuf(old);
    // 关键：从文件读到 EOF 后，cin 会带着 eofbit/failbit，必须 clear() 才能继续从键盘读。
    cin.clear();
    cerr << "[Load] Loaded. StreetNum=" << StreetNum
         << ", InterNum=" << InterNum
         << ", BuildingNum=" << BuildingNum
         << ", EdgeCnt=" << ecnt << endl;

    if (InterNum <= 0 || BuildingNum <= 0) {
        cerr << "[Error] Map data not loaded (InterNum/BuildingNum is 0).\n";
        return 1;
    }

    Init();
    Service();
    return 0;
}
