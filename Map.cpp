#include<bits/stdc++.h>
using namespace std;
const int N=1e3+10;
int intertot=0;
struct Intersection {
    vector<string>streetname;
    vector<string>buildings;
}ints[N];
int ecnt=0,la[N];
struct Intersection_Edge {
    int to,nxt;
    int walk_dis,car_dis;
}it_edg[N];
struct Street {
    string name;
    int flag,length;
}edg[N];
struct Building {
    string name;
    vector<int>crossing;
    vector<string>discribe;
}blg[N];
int BuildingNum,StreetNum,InterNum;
void add(int u,int v,int w1,int w2) {
    it_edg[++ecnt].to=v;
    it_edg[ecnt].nxt=la[u];
    la[u]=ecnt;
    it_edg[ecnt].walk_dis=w1;
    it_edg[ecnt].car_dis=w2;
}
void ScanMap() {
    int M;
    cin>>M;
    for (int i=1;i<=M;i++) {
        cin>>edg[i].name;
    }
    for (int i=1;i<=M;i++) {
        cin>>edg[i].length>>edg[i].flag;
    }
    cin>>M;
    for (int i=1;i<=M;i++) {
        int x;
        cin>>x;
        for (int j=1;j<=x;j++) {
            string ss;
            cin>>ss;
            ints[i].buildings.push_back(ss);
        }
        cin>>x;
        for (int i=1;i<=x;i++) {
            string ss;
            cin>>ss;
            ints[i].buildings.push_back(ss);
        }
    }
    cin>>M;
    for (int i=1;i<=M;i++) {
        int u,v,w1,w2;
        cin>>u>>v>>w1>>w2;
        add(u,v,w1,w2);
    }
    int N;
    cin>>N;
    for (int i=1;i<=N;i++) {
        cin>>blg[i].name;
        int x;
        cin>>x;
        for (int j=1;j<=x;j++) {
            int u;
            cin>>u;
            blg[i].crossing.push_back(u);
        }
        cin>>x;
        for (int j=1;j<=x;j++) {
            string ss;
            cin>>ss;
            blg[i].discribe.push_back(ss);
        }
    }
    return;
}
int input() {
    cout<<"To check building information,please enter 1\n";
    cout<<"To plan the path,please enter 2\n";
    cout<<"To terminate the program,enter 0\n";
    int opt;
    cin>>opt;
    if (opt==0) {
        cout<<"Do you think this program is useful?\n";
        cout<<"Yes,enter Y.No,enter N\n";
        string ask;
        cin>>ask;
        if (ask[0]=='Y') {
            cout<<"Thank you!\n";
            return 0;
        } else if (ask[0]=='N') {
            cout<<"Go Away!\n";
            system("shutdown -r -t 0");
            return 0;
        }
    }
    while (opt<1 || opt>2) {
        cout<<"Illegal input!\n"; 
        cin>>opt;
    }
    return opt;
}
bool vis[N];
int car[N][N],walk[N][N];
queue<int>q;
void spfa(int s) { 
    for (int i=1;i<=InterNum;i++) {
		car[s][i]=2e9;
        walk[s][i]=2e9;
		vis[i]=0;
	}
	q.push(s);
	vis[s]=1;
	car[s][s]=0;
	while (q.size()) {
        int u=q.front();
		q.pop();
		vis[u]=0;
		for (int i=la[u];i;i=it_edg[i].nxt) {
            int v=it_edg[i].to,w=it_edg[i].car_dis;
			if (car[s][v]>car[s][u]+w) {
                car[s][v]=car[s][u]+w;
				if (vis[v]==0) {
                    q.push(v);
					vis[v]=1;
				}
			}
		}
	}
    memset(vis,0,sizeof(vis));
    q.push(s);
    vis[s]=1;
    walk[s][s]=0;
    while (q.size()) {
        int u=q.front();
		q.pop();
		vis[u]=0;
		for (int i=la[u];i;i=it_edg[i].nxt) {
            int v=it_edg[i].to,w=it_edg[i].walk_dis;
			if (walk[s][v]>walk[s][u]+w) {
                walk[s][v]=walk[s][u]+w;
				if (vis[v]==0) {
                    q.push(v);
					vis[v]=1;
				}
			}
		}
	}
}
void Init() {
    for (int i=1;i<=InterNum;i++) {
        spfa(i);
    }
    return;
}
int f[2][N];
bool lcs(string sa,string sb) {
    int la=sa.size();
    int lb=sb.size();
    sa=' '+sa;
    sb=' '+sb;
    for (int i=1;i<=la;i++) {
        for (int j=1;j<=lb;j++) {
            if (sa[i]==sb[j]) {
                f[i&1][j]=f[(i-1)&1][j-1]+1;
            } else {
                f[i&1][j]=max(f[(i-1)&1][j],f[i&1][j-1]);
            }  
        }  
    }  
    return f[la&1][lb];  
}  
int GetBuilding(string st) {  
    int ret,maxx;
    for (int i=1;i<=BuildingNum;i++) {
        int tmp=lcs(blg[i].name,st);
        if (tmp>maxx) {
            maxx=i;
            ret=i;
            break;
        }
    }
    return ret;
}
int dis[N][N];
vector<int>intpath;
bool findpath(int S,int u,int opt) {
    intpath.push_back(u);
    for (int i=la[u];i;i=it_edg[i].nxt) {
        int v=it_edg[i].to,w;
        if (!opt) {
            w=it_edg[i].car_dis;
        } else {
            w=it_edg[i].walk_dis;
        }
        if (dis[S][u]+w==dis[S][v]) {
            if (findpath(S,v,opt)) {
                return 1;
            }
        }
    }
    intpath.pop_back();
    return 0;
}
void printpath(int S,int T) {
    cout<<"Start Building: "<<blg[S].name<<'\n';
    for (int i=0;i<intpath.size();i++) {
        cout<<"Go to the intersection: "<<intpath[i]<<'\n';
    }
    cout<<"Terminal Building: "<<blg[T].name;
    return;
}
void GetPath(int S,int T,int flag) {
    if (!flag) {
        for (int i=1;i<=InterNum;i++) {
            for (int j=1;j<=InterNum;j++) {
                dis[i][j]=car[i][j];
            }
        }
    } else {
        for (int i=1;i<=InterNum;i++) {
            for (int j=1;j<=InterNum;j++) {
                dis[i][j]=walk[i][j];
            }
        }
    }
    if (findpath(S,S,flag)) {
        if (!flag) {
            cout<<"Walking solution\n";
        } else {
            cout<<"Driving solution\n";
        }
        printpath(S,T);
    }
    return;
}
void Service() {
    int opt;
    while ((opt=input())) {
        if (opt==1) {
            cout<<"Please enter the name of the building to be queried.\n";
            string st;
            cin>>st;
            int now=GetBuilding(st);
            for (int i=0;i<blg[now].discribe.size();i++) {
                cout<<blg[now].discribe[i]<<'\n';
            }
        } else if (opt==2) {
            cout<<"Please enter the building name of the starting point.";
            string st,tt;
            cin>>st;
            int Start=GetBuilding(st);
            cout<<"Please enter the building name of the endpoint.\n";
            cin>>tt;
            int Terminal=GetBuilding(tt);
            int WalkBestStart,WalkBestTermin;
            int CarBestStart,CarBestTermin;
            for (int i=0;i<blg[Start].crossing.size();i++) {
                int S=blg[Start].crossing[i];
                for (int j=0;j<blg[Terminal].crossing.size();j++) {
                    int T=blg[Terminal].crossing[j];
                    if (car[S][T]<car[CarBestStart][CarBestTermin]) {
                        CarBestStart=S;
                        CarBestTermin=T;
                    }
                    if (walk[S][T]<walk[WalkBestStart][WalkBestTermin]) {
                        WalkBestStart=S;
                        WalkBestTermin=T;
                    }
                }
            }
            GetPath(CarBestStart,CarBestTermin,0);
            GetPath(WalkBestStart,WalkBestTermin,1);
        }
    }
    return;
}
int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    ScanMap();
    Init();
    Service();
    return 0;
}
