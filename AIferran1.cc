#include "Player.hh"
#include <queue>
#include <set>


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME ferran1


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
  using VVP = vector<VP>;
  using VPB = vector<PB>;
  using VE = vector<int>;
  using VVE = vector<VE>;
  using VM = vector<Cell>;
  using VVM = vector<VM>;
  using VC = vector<char>;
  using VVC = vector<VC>;

  map<int, int> kind;


  void run(Pos p, Pos enemy){

  }

  int give_dir(int i, int j){
    if(i == 1 and j == 0) return 0;
    if(i == 1 and j == 1) return 1;
    if(i == 0 and j == 1) return 2;
    if(i == -1 and j == 1) return 3;
    if(i == -1 and j == 0) return 4;
    if(i == -1 and j == -1) return 5;
    if(i == 0 and j == -1) return 6;
    if(i == 1 and j == -1) return 7;
  }
  int attack(Pos p, Pos objectiu){
    int x, y;
    x = objectiu.i - p.i;
    y = objectiu.j - p.j;
    return give_dir(x,y);
  }

  void conquere(Pos p, Pos cell){

  }

  bool balrog_around_here(Pos p, VVM& twfour, Pos& b){ //Given id position, explores 5x5 cells around id
                                                       //and finds the balrog, and saves its position.
    int bal = balrog_id();
    bool ishere = false;
    int x,y;
    for(int i = p.i-2; i<=p.i+2; ++i){
      for(int j = p.j-2; j<=p.j+2; ++j){
        if(pos_ok(i,j)){
          Cell c = cell(i,j);
          twfour[i-p.i+2][j-p.j+2] = c;
          if(twfour[i-p.i+2][j-p.j+2].id == bal){
            b.i = i;
            b.j = j;
            ishere = true;
          }
        }
      }
    }
    if(ishere) return true;
    else return false;

  }

  int explore(Pos q){ //Go inside moria
    bool arrived = false;
    for(int i = 0; not arrived and i<3; ++i){
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
          if(ty == 2 or ty == 3) return abs(k-5);
          
        }
      }
      q += Dir(random(0,7));
    }
    return 0;
  }

  int search_treasure(Pos p, VPB tresors, set<Pos> S3){
    for(int k = 0; k<8; ++k){
      if(pos_ok(p+=Dir(k))){
        if(S3.find(p+=Dir(k)) != S3.end()) return k;
        Pos q;
        q = p + Dir(k);
        if(S3.find(q+=Dir(k)) != S3.end()) return k;
      }
    }
    
    return -1;
  }

  void move_dwarves(VPB tresors, set<Pos> S3) {
    VE D = dwarves(me());
    int n = D.size();
    VE perm = random_permutation(n);
    
    for(int i = 0; i<n; ++i){
      bool action = false;
      VVM twfour; //5x5 cells exploration
      twfour = VVM(5,VM(5));
      int id = D[perm[i]];
      bool enemy = false;
      Pos p = unit(id).pos; //My position
      Pos b; //Balrog position

      if(balrog_around_here(p, twfour, b)){
        cout<<"help" << endl;
      }
      cout << p.i << ' ' << p.j << endl;
      for(int i = 0; i<5; ++i){
        for(int j = 0; j<5; ++j){
          cout << twfour[i][j].owner << ' ';
        }
        cout << endl;
      }
      cout << endl;
      //else{
        for(int k = 0; not enemy and k<8; ++k){
          if(pos_ok(p += Dir(k)) and not action){
            int id2 = cell(p+=Dir(k)).id;
            if(id2 != -1 and unit(id2).player != me() and unit(id2).health < unit(id).health){
              enemy = true;
              action = true;
              command(id,Dir(k));
            }
            if(id2 != -1 and unit(id2).player != me() and unit(id2).health > unit(id).health){
              enemy = true;
              action = true;
              command(id,Dir(abs(k-4)));
            }
          }
        }
        if(not enemy){
          int k = search_treasure(p, tresors, S3);
          if(k != -1) {
            action = true;
            command(id,Dir(k));
          }
          else{
            for(int k = 0; k<8; ++k){
              if(pos_ok(p+=Dir(k)) and not action){
                if(cell(p+=Dir(k)).owner != me()) {
                  action = true;
                  command(id,Dir(k));
                }
              }
            }
          }
        }
      }
    //}
  }


  void inside_bfs(int i, int j, int d, queue<Pos>& q, VVE& M){
    if(i>=0 and i<7 and j>=0 and j<7 and M[i][j] == 0){
      M[i][j] = d;
      Pos p;
      p.i = i; p.j = j;
      q.push(p);
    }
  }

  bool search_weak_dwarf(Pos p, VVE& M, Pos& q, const VVC G){
    queue<Pos> pending;
    M[3][3] = 0;
    Pos matrix;
    matrix.i = 3; matrix.j = 3;
    pending.push(matrix);
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

  bool dins(Pos p){
    if(p.i>=0 and p.i<7 and p.j>=0 and p.j<=7) return true;
    return false;
  }

  void move_wizards() {
    VE W = wizards(me());
    VE D = dwarves(me());
    set<int> S;
    for(int i = 0; i<D.size(); ++i){
      S.insert(D[i]);
    }
    
    for(int id : W){
      VVE M(7,VE(7,0));
      VVC G(7,VC(7,'.'));
      Pos p = unit(id).pos;
      cout << p.i << ' ' << p.j << endl;
      for(int i = p.i-3; i<=p.i+3; ++i){
        for(int j = p.j-3; j<=p.j+3; ++j){
          if(pos_ok(i,j)){
            if(cell(i,j).type == 2 or cell(i,j).type == 3 or cell(i,j).type == 4) M[i-p.i+3][j-p.j+3] = -1;
            if(S.find(cell(i,j).id) != S.end()) G[i-p.i+3][j-p.j+3] = 'D';
          }
        }
      }
      
      cout << endl;
      Pos q;
      cout << "bool = " << search_weak_dwarf(p,M,q,G) << endl;
      if(search_weak_dwarf(p,M,q,G)){
        cout << p.i << ' ' << p.j << endl;
        cout << q.i << ' ' << q.j << endl;
        if(pos_ok(q+=Pos(1,0)) and dins(q += Pos(1,0)) and M[(q+=Pos(1,0)).i][(q+=Pos(1,0)).j] == M[q.i][q.j]-1) command(id,Dir(2));
        else if(pos_ok(q+=Pos(0,-1)) and dins(q += Pos(0,-1)) and M[(q+=Pos(0,-1)).i][(q+=Pos(0,-1)).j] == M[q.i][q.j]-1) command(id,Dir(4));
        else if(pos_ok(q+=Pos(-1,0)) and dins(q += Pos(-1,0)) and M[(q+=Pos(-1,0)).i][(q+=Pos(-1,0)).j] == M[q.i][q.j]-1) command(id,Dir(6));
        else if(pos_ok(q+=Pos(0,1)) and dins(q += Pos(0,1)) and M[(q+=Pos(0,1)).i][(q+=Pos(0,1)).j] == M[q.i][q.j]-1) command(id,Dir(0));
      }
      else{
        int k = random(0,7);
        if(pos_ok(q+=Dir(k))) command(id,Dir(k));
        else command(id,Dir(8));
      }
    }
  }


  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    VE orc = orcs();
    VE troll = trolls();
    int bal = balrog_id();
    VPB tresors(0); 
    VVC G(60,VC(60,'.'));
    for(int i = 2; i<58; ++i){
      for(int j = 2; j<58; ++j){
        Pos q;
        q.i = i; q.j = j;
        if(cell(q).treasure == true){
          PB p;
          p.first.first = i; p.first.second = j;
          p.second = true;
          tresors.push_back(p);
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
      Pos q;
      q.i = tresors[i].first.first; q.j = tresors[i].first.second;
      S3.insert(q);
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
    cout << endl;
    for(int i = -1; i<61; ++i){
      for(int j = -1; j<61; ++j){
        if(i == -1 or i == 60) cout << abs((j)%10) << " ";
        else if(j == -1 or j == 60) cout << abs((i)%10) << " ";
        else cout << G[i][j] << " ";
      }
      cout << endl;
    }
    cout << endl;

    move_dwarves(tresors, S3);
    move_wizards();
    
  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
