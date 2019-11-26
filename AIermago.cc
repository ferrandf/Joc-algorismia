#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME ermago


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
      if(G[q.i][q.j] == 'D') return;
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


  void move_dwarves(VVE& M){
    VE D = dwarves(me());
    int n = D.size();
    for(int i = 0; i<n; ++i){
      int id = D[i];
      bool action = false;
      bool enemy = false;
      Pos p = unit(id).pos;
      for(int k = 0; not action and k<8; ++k){
        if(pos_ok(p+=Dir(k))){
          int id2 = cell(p+=Dir(k)).id;
          int owne = cell(p+=Dir(k)).owner;
          if(id2 != -1 and unit(id2).player != me() and unit(id2).type != 2 and unit(id2).type != 3){
            action = true;
            enemy = true;
            command(id,Dir(k));
          }
          else if(not action and id2 != -1 and unit(id2).type == 2 and unit(id2).health < unit(id).health){
            action = true;
            enemy = true;
            command(id,Dir(k));
          }
        }
      }
      for(int k = 0; not action and k<8; ++k){
        if(pos_ok(p+=Dir(k))){
          int id2 = cell(p+=Dir(k)).id;
          int owne = cell(p+=Dir(k)).owner;
          if(not action and id != -1 and unit(id2).type == 3){
            action = true;
            enemy = true;
            command(id,Dir(abs(k-4)));
          }
        }
      }
      
      for(int k = 0; not action and k<8; ++k){ 
        if(pos_ok(p+=Dir(k))){
          int id2 = cell(p+=Dir(k)).id;
          int owne = cell(p+=Dir(k)).owner;
          if(not action and owne != me() and cell(p+=Dir(k)).type != 2){
            action = true;
            command(id,Dir(k));
          }
        }
      }
      if(not action){
        int imin = 1e9;
        int dir = random(0,7);
        for(int k = 0; k<8; ++k){
          if(pos_ok(p+=Dir(k))){
            Pos q = p+=Dir(k);
            if(M[q.i][q.j] < imin){
              imin = M[q.i][q.j];
              dir = k;
            }
          }
        }
        action = true;
        command(id,Dir(dir));
      }
    }
  }

  void move_wizards(){
    VE W = wizards(me());
    int n = W.size();
    for(int i = 0; i<n; ++i){
      int id = W[i];
      command(id,Dir(8));
    }
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    VE orc = orcs();
    VE troll = trolls();
    int bal = balrog_id();
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

    

    move_dwarves(M);
    move_wizards();
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
