#include "Player.hh"
#include <queue>
#include <set>
#include <cmath>


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME ferran2


struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }

  /**
   * Types and attributes for your player can be defined here.
   */

  using P = pair<int,int>;
  using PB = pair<P,bool>;
  using VP = vector<P>;
  using VPos = vector<Pos>;
  using VVP = vector<VP>;
  using VPB = vector<PB>;
  using VE = vector<int>;
  using VVE = vector<VE>;
  using VM = vector<Cell>;
  using VVM = vector<VM>;
  using VC = vector<char>;
  using VVC = vector<VC>;  
  map<int, int> kind;


  void inside_tresors(Pos p, int i, int j, int d, queue<Pos>& pending, VVE& M, VVE& vist){
    if(i>=0 and i < 60 and j >= 0 and j < 60 and cell(i,j).type != 2 and cell(i,j).type != 3 and not vist[i][j]){
      if(cell(i,j).type == 4) M[i][j] = min(M[i][j], d+=cell(i,j).turns);
      else M[i][j] = min(M[i][j], d);
      Pos pos;
      pos.i = i; pos.j = j;
      pending.push(pos);
      vist[i][j] = true;
    }
  }

  void bfs_tresors(VVE& M, const VPos tresors, int i, VVC& G){
    cout << "TRESORS " << i << endl;
    Pos p = tresors[i];
    M[p.i][p.j] = 0;
    VVE vist(60,VE(60,0));
    queue<Pos> pending;
    pending.push(tresors[i]);
    int d = 0;
    int e = 0;
    while(not pending.empty()){
      Pos q = pending.front();
      int x, y;
      x = q.i; y = q.j;
      d = M[q.i][q.j];
      pending.pop();
      ++d;
      inside_tresors(p,x-1,y-1,d,pending,M,vist);
      inside_tresors(p,x-1,y,d,pending,M,vist);
      inside_tresors(p,x-1,y+1,d,pending,M,vist);
      inside_tresors(p,x,y-1,d,pending,M,vist);
      inside_tresors(p,x,y+1,d,pending,M,vist);
      inside_tresors(p,x+1,y-1,d,pending,M,vist);
      inside_tresors(p,x+1,y,d,pending,M,vist);
      inside_tresors(p,x+1,y+1,d,pending,M,vist);
    }
  }

  bool balrog_around_here(Pos p, VVM& twfour, Pos& b){ //Given id position, explores 5x5 cells around id
                                                       //and finds the balrog, and saves its position.
    int bal = balrog_id();
    bool ishere = false;
    int x,y;
    for(int i = p.i-2; i<=p.i+2; ++i){
      for(int j = p.j-2; j<=p.j+2; ++j){
        Cell c = cell(i,j);
        twfour[i][j] = c;
        if(twfour[i][j].id == bal){
          b.i = i;
          b.j = j;
          ishere = true;
        }
      }
    }
    if(ishere) return true;
    else return false;

  }

  int explore(Pos q, int imax){ //Go inside moria
    bool arrived = false;
    for(int i = 0; not arrived and i<imax; ++i){
      for(int k = 0; k<8; ++k){
        if(pos_ok(q+=Dir(k))){
          int ty = cell(q+=Dir(k)).type;
          int to = cell(q+=Dir(k)).owner;
          int tid = cell(q+=Dir(k)).id;
          int uh = unit(tid).health;
          if(ty != 0 and ty != 2 and ty != 3 and to != me() and uh < 100){
            arrived = true;
            return k;
          }
          if(ty == 2 or ty == 3) return abs(k-4);
          
        }
      }
      q += Dir(random(0,7));
    }
    return 0;
  }

  void move_dwarves(VVE& M, Pos b) {
    VE D = dwarves(me());
    int n = D.size();
    for(int i = 0; i<n; ++i){
      bool action = false;
      int id = D[i];
      bool enemy = false;
      Pos p = unit(id).pos; //My position
      int x = b.i-p.i;
      int y = b.j-p.j;
      if(sqrt(x*x + y*y) < 3){
        cout << "help" << endl;
      }
      //if(balrog_around_here(p, twfour_exploration,b)){

      //}
      //else{

        for(int k = 0; not enemy and k<8; ++k){
          Pos p = unit(id).pos;
          Pos actual = p+Dir(k);
          if(pos_ok(actual)){
            int id2 = cell(actual).id;
            if(id2 != -1 and unit(id2).player != me() and unit(id2).health < unit(id).health){
              enemy = true;
              command(id,Dir(k));
            }
          }
        }
        if(not enemy){
          int imin = 1e9;
          int dir = 0;
          for(int k = 0; k<8; ++k){
            Pos actual = p+Dir(k);
            if(pos_ok(actual)){
              if(M[actual.i][actual.j] < imin){
                imin = M[actual.i][actual.j];
                dir = k;
              }
            }
          }
          if(imin <1e9) command(id,Dir(dir));
          else {
            for(int k = 4; k<8; ++k){
              Pos actual = p+Dir(k);
              if(pos_ok(actual)){
                if(cell(actual).owner != me()){
                  command(id,Dir(k));
                }
              }
            }
          }
        }
      }
    //}
  }


  void inside_bfs(int i, int j, int d, queue<Pos>& q, VVE M){
    if(i>=0 and i<59 and j>=0 and j<59 and M[i][j] == 0){
      M[i][j] = d;
      Pos p;
      p.i = i; p.j = j;
      q.push(p);
    }
  }

  bool search_weak_dwarf(Pos p, VVE& M, Pos& q, const VVC G){
    queue<Pos> pending;
    M[p.i][p.j] = 0;
    pending.push(p);
    int d = 0;
    while(not pending.empty()){
      Pos k = pending.front();
      int x = k.i;
      int y = k.j;
      d = M[x][y];
      if(G[x][y] == 'D') {
        q.i = x; q.j = y;
        return true;
      }
      pending.pop();
      ++d;
      inside_bfs(x-1,y,d,pending,M);
      inside_bfs(x+1,y,d,pending,M);
      inside_bfs(x,y-1,d,pending,M);
      inside_bfs(x,y+1,d,pending,M);
    }
    return false;
  }


  void move_wizards() {
    vector<int> W = wizards(me());
    for (int id : W) {
      if (kind.find(id) == kind.end()) kind[id] = random(0, 1);
      if (kind[id] == 0) command(id, Dir(2*random(0, 3)));
      else command(id, Dir(2*(round()%4)));
   }
  }


  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    VE orc = orcs();
    VE troll = trolls();
    int bal = balrog_id();
    Pos b = unit(bal).pos;
    VPos tresors(0); 
    VVC G(60,VC(60,'.'));
    for(int i = 2; i<58; ++i){
      for(int j = 2; j<58; ++j){
        Pos q;
        q.i = i; q.j = j;
        if(cell(q).treasure == true){
          tresors.push_back(q);
        }
      }
    }
    VE D = dwarves(me());
    VE W = wizards(me());
    set<int> S1;
    set<int> S2;
    set<Pos> S3;
    for(int i = 0; i<D.size(); ++i){
      S1.insert(D[i]);
    }
    for(int i = 0; i<W.size(); ++i){
      S2.insert(W[i]);
    }
    for(int i = 0; i<tresors.size(); ++i){
      S3.insert(tresors[i]);
    }
    for(int i = 0; i<60; ++i){
      for(int j = 0; j<60; ++j){
        Pos q;
        q.i = i; q.j = j;
        if(S1.find(cell(i,j).id) != S1.end()) G[i][j] = 'D';
        if(S2.find(cell(i,j).id) != S2.end()) G[i][j] = 'W';
        if(S3.find(q) != S3.end()) G[i][j] = 'T';
      }
    }
    VVE M(60,VE(60,1e9));
    for(int i = 0; i<tresors.size(); ++i){
      G[tresors[i].i][tresors[i].j] = '.';
      bfs_tresors(M,tresors, i, G);
      G[tresors[i].i][tresors[i].j] = 'T';
    }
    for(int i = -1; i<61; ++i){
      for(int j = -1; j<61; ++j){
        if(i == -1 or i == 60) cout << abs((j)%10) << " ";
        else if(j == -1 or j == 60) cout << abs((i)%10) << " ";
        else {
          if(M[i][j] != 1e9 and G[i][j] == '.') cout << M[i][j] << ' ';
          else cout << G[i][j] << " ";
        }
      }
      cout << endl;
    }
    cout << endl;

    

    move_dwarves(M,b);
    move_wizards();
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
