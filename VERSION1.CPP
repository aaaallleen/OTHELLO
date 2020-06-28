#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>

struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard() {
        reset();
    }
    /*void operator =(OthelloBoard &rhs){
        cur_player = rhs.cur_player;
        for(int i = 0 ; i < rhs.next_valid_spots.size(); i ++){
            next_valid_spots[i] = rhs.next_valid_spots[i];
        }
        for(int i = 0 ; i <SIZE; i++){
            for(int j = 0 ; j < SIZE ; j ++){
                board[i][j] = rhs.board[i][j];
            }
        }
    } */
     void operator =(OthelloBoard rhs){
        for(int i = 0; i<SIZE; i++)
        {
            for(int j = 0; j<SIZE; j++){
                board[i][j] = rhs.board[i][j];
            }
        }
        cur_player = rhs.cur_player;
        for(int i = 0; i<rhs.next_valid_spots.size(); i++){
            next_valid_spots[i] = rhs.next_valid_spots[i];
        }
    }

    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        return true;
    }
};
int player;
const int SIZE = 8;
int boardbonus[8][8];
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
OthelloBoard current;

void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
           fin >> board[i][j];
            //fin >> current.board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

int discCount(OthelloBoard cnt){
    int answer = 0 ;
    for(int i = 0 ; i < cnt.SIZE; i ++){
        for(int j = 0; j < cnt.SIZE ; j ++){
            if(cnt.board[i][j] == cnt.cur_player ) answer++;
        }
    }
    return answer;
}
double MiniMax(OthelloBoard state, int depth, double alpha, double beta , bool xo){
    double val ;
    if(state.cur_player == 2){
        if(state.board[0][0]==1){
            boardbonus[0][1] = 3.0;
            boardbonus[1][0] = 3.0;
            boardbonus[1][1] = 2.0;
        } 
        if(state.board[0][7] == 1){
            boardbonus[0][6] = 3.0;
            boardbonus[1][7] = 3.0;
            boardbonus[1][6] = 2.0;
        }
        if(state.board[7][0] == 1){
            boardbonus[6][0] = 3.0;
            boardbonus[7][1] = 3.0;
            boardbonus[6][1] = 2.0;
        }
        if(state.board[7][7] == 1){
            boardbonus[6][7] = 3.0;
            boardbonus[7][6] = 3.0;
            boardbonus[6][6] = 2.0;
        }
        val = 0.00;
        for(int i = 0; i<SIZE; i++){
            for(int j = 0; j<SIZE; j++){  
                if(state.board[i][j] == 1){
                    val += 1;
                    val += boardbonus[i][j];
                } 
            }
        }
        if(depth > 0){
            for(int i = 0; i<state.next_valid_spots.size(); i++){
                OthelloBoard tmp = state;
                tmp.put_disc(tmp.next_valid_spots[i]);
                if(xo){
                    double compare = MiniMax(tmp, depth-1, alpha, beta, !xo) + 1.5*tmp.next_valid_spots.size();
                    double val = (val > compare)?val : compare;
                    alpha  = (alpha>val)?alpha:val;
                    if(alpha >= beta){
                            return val;
                    }
                }
                else{
                    double compare  = MiniMax(tmp, depth-1, alpha, beta, !xo) + 1.5*tmp.next_valid_spots.size();
                    val = (val>compare)?compare:val;
                    beta = (beta>val)?val:beta;
                    if(alpha >= beta){
                        return val;
                    }
                }
            }
        }
        return val;
    }
    else if(state.cur_player == 1){
        if(state.board[0][0]==2){
            boardbonus[0][1] = 3.0;
            boardbonus[1][0] = 3.0;
            boardbonus[1][1] = 2.0;
        } 
        if(state.board[0][7] == 2){
            boardbonus[0][6] = 3.0;
            boardbonus[1][7] = 3.0;
            boardbonus[1][6] = 2.0;
        }
        if(state.board[7][0] == 2){
            boardbonus[6][0] = 3.0;
            boardbonus[7][1] = 3.0;
            boardbonus[6][1] = 2.0;
        }
        if(state.board[7][7] == 1){
            boardbonus[6][7] = 3.0;
            boardbonus[7][6] = 3.0;
            boardbonus[6][6] = 2.0;
        }
        val = 0.0;
        for(int i = 0; i<SIZE; i++){
            for(int j = 0; j<SIZE; j++){
                if(state.board[i][j] == 2){
                    val += 1;
                    val += boardbonus[i][j];
                }
            }  
        }
        if(depth > 0){
            for(int i = 0; i<state.next_valid_spots.size(); i++){
                OthelloBoard tmp = state;
                tmp.put_disc(tmp.next_valid_spots[i]);
                if(xo){
                    double compare = MiniMax(tmp, depth-1, alpha, beta, !xo) + 1.5*tmp.next_valid_spots.size();
                    val = (val>compare)?val:compare;
                    alpha = (val>alpha)?val:alpha;
                    if(alpha >= beta){
                        return val;
                    }
                }
                else{
                    double compare = MiniMax(tmp, depth-1, alpha, beta, !xo) + 1.5*tmp.next_valid_spots.size() ;
                    val = (val>compare)?compare:val;
                    beta = (beta>val)?val:compare;
                    if(alpha >= beta){
                        return val;
                    }
                }
            }
         }
        return val;
    }
    return val;
}

int getmaxval(int n){
    double max = -2147483648;
    int answer = 0;
    current.cur_player = player;
    for(int i = 0; i< n ; i++){
        OthelloBoard temp = current;
        temp.put_disc(next_valid_spots[i]);
        double val = MiniMax(temp, 6, -2147483648.0 , 2147483647.0, true) + 2*temp.next_valid_spots.size();
        if(val>max){
            max = val;
            answer = i;
        }
    }
    return answer;
}
int test(){
    int val = -2147483648;
    int answer = 0;
    for(int i = 0 ; i < next_valid_spots.size(); i ++){
        int value = boardbonus[next_valid_spots[i].x][next_valid_spots[i].y];
        if(value>val){
            val = value;
            answer = i; 
        }   
    }
    return answer;
}

void write_valid_spot(std::ofstream& fout) {
    int n_valid_spots = next_valid_spots.size();
    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    for(int i = 0; i<SIZE; i++){
        for(int j = 0; j<SIZE; j++){
            current.board[i][j] = board[i][j];
        }
    }
    //int index = getmaxval(n_valid_spots);
    int index = test();
    Point p = next_valid_spots[index];
    // Remember to flush the output to ensure the last action is written to file.
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}

void initializebonus(){
    
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 9 ; j ++){
            boardbonus[i][j] = 3;
        }
    }
    for(int i = 0 ; i < 8 ; i++){
        boardbonus[i][1] = -5;
    }
    for(int i = 0 ; i < 8 ; i++){
        boardbonus[1][i] = -5;
    }
    for(int i = 0 ; i < 8 ; i++){
        boardbonus[i][6] = -5;
    }
    for(int i = 0 ; i < 8 ; i++){
        boardbonus[6][i] = -5;
    }
    boardbonus[0][3] = 5;
    boardbonus[0][4] = 5;
    boardbonus[3][0] = 5;
    boardbonus[4][0] = 5;
    boardbonus[7][3] = 5;
    boardbonus[7][4] = 5;
    boardbonus[3][7] = 5;
    boardbonus[4][7] = 5;
    boardbonus[2][2] = 15;
    boardbonus[2][5] = 15;
    boardbonus[5][2] = 15;
    boardbonus[5][5] = 15;
    boardbonus[0][0] = 120;
    boardbonus[0][7] = 120;
    boardbonus[7][0] = 120;
    boardbonus[7][7] = 120;
    boardbonus[1][0] = -20;
    boardbonus[0][1] = -20;
    boardbonus[6][0] = -20;
    boardbonus[7][1] = -20;
    boardbonus[0][6] = -20;
    boardbonus[1][7] = -20;
    boardbonus[6][7] = -20;
    boardbonus[7][6] = -20;
    boardbonus[2][0] = 20;
    boardbonus[0][2] = 20;
    boardbonus[5][0] = 20;
    boardbonus[7][2] = 20;
    boardbonus[0][5] = 20;
    boardbonus[2][7] = 20;
    boardbonus[5][7] = 20;
    boardbonus[7][5] = 20; 
    boardbonus[1][1] = -40;
    boardbonus[6][1] = -40;
    boardbonus[1][6] = -40;
    boardbonus[6][6] = -40;
    /*
    boardbonus[0][0] = 16.16;
    boardbonus[0][1] = -3.03;
    boardbonus[0][2] = 0.99;
    boardbonus[0][3] = 0.43;
    boardbonus[0][4] = 0.43;
    boardbonus[0][5] = 0.99;
    boardbonus[0][6] = -3.03;
    boardbonus[0][7] = 16.16;
    boardbonus[1][0] = -4.12;
    boardbonus[1][1] = -1.81;
    boardbonus[1][2] = -0.08;
    boardbonus[1][3] = -0.27;
    boardbonus[1][4] = -0.27;
    boardbonus[1][5] = -0.08;
    boardbonus[1][6] = -1.81;
    boardbonus[1][7] = -4.12;
    boardbonus[2][0] = 1.33;
    boardbonus[2][1] = -0.04;
    boardbonus[2][2] = 0.51;
    boardbonus[2][3] = 0.07;
    boardbonus[2][4] = 0.07;
    boardbonus[2][5] = 0.51;
    boardbonus[2][6] = -0.04;
    boardbonus[2][7] = 1.33;
    boardbonus[3][0] = 0.63;
    boardbonus[3][1] = -0.18;
    boardbonus[3][2] = -0.04;
    boardbonus[3][3] = -0.01;
    boardbonus[3][4] = -0.01;
    boardbonus[3][5] = -0.04;
    boardbonus[3][6] = -0.18;
    boardbonus[3][7] = 0.63;
    boardbonus[4][0] = 0.63;
    boardbonus[4][1] = -0.18;
    boardbonus[4][2] = -0.04;
    boardbonus[4][3] = -0.01;
    boardbonus[4][4] = -0.01;
    boardbonus[4][5] = -0.04;
    boardbonus[4][6] = -0.18;
    boardbonus[4][7] = 0.63;
    boardbonus[5][0] = 1.33;
    boardbonus[5][1] = -0.04;
    boardbonus[5][2] = 0.51;
    boardbonus[5][3] = 0.07;
    boardbonus[5][4] = 0.07;
    boardbonus[5][5] = 0.51;
    boardbonus[5][6] = -0.04;
    boardbonus[5][7] = 1.33;
    boardbonus[6][0] = -4.12;
    boardbonus[6][1] = -1.81;
    boardbonus[6][2] = -0.08;
    boardbonus[6][3] = -0.27;
    boardbonus[6][4] = -0.27;
    boardbonus[6][5] = -0.08;
    boardbonus[6][6] = -1.81;
    boardbonus[6][7] = -4.12;
    boardbonus[7][0] = 16.16;
    boardbonus[7][1] = -3.03;
    boardbonus[7][2] = 0.99;
    boardbonus[7][3] = 0.43;
    boardbonus[7][4] = 0.43;
    boardbonus[7][5] = 0.99;
    boardbonus[7][6] = -3.03;
    boardbonus[7][7] = 16.16;
    */
}
int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    initializebonus();
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
