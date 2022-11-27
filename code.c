//  main.c
//  搜索算法
//
//  Created by  周子龙 on 2018/11/29.
//  Copyright © 2018  周子龙. All rights reserved.
//
/**
 * 最后编辑时间 2018.12.11 09:31
 * version 2.4 for 搜索算法，优化了走法更新的方法，优化思考时间
 *
 * version 2.5.0 for 搜索算法，时间优化
 */

double test = 0;
int lastcheese;

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//-------------hash-------------

///////////////////////////////////////

// board information
#define BOARD_SIZE 12
#define EMPTY 0
#define BLACK 1
#define WHITE 2

// bool
typedef int BOOL;
#define TRUE 1
#define FALSE 0

// option
typedef int OPTION;
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define UP_LEFT 4
#define UP_RIGHT 5
#define DOWN_LEFT 6
#define DOWN_RIGHT 7

#define MAX_BYTE 10000

#define START "START"
#define PLACE "PLACE"
#define TURN "TURN"
#define END "END"

struct Command {
  int x;
  int y;
  OPTION option;
};

char buffer[MAX_BYTE] = {0};
char board[BOARD_SIZE][BOARD_SIZE] = {0};
int me_flag;
int other_flag;

const int DIRC[8][2] = {{-1, 0},  {1, 0},  {0, -1}, {0, 1},
                        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

void printBoard() {
  char visual_board[BOARD_SIZE][BOARD_SIZE] = {0};
  for (int i = 0; i < BOARD_SIZE; i++) {
    for (int j = 0; j < BOARD_SIZE; j++) {
      if (board[i][j] == EMPTY) {
        visual_board[i][j] = '.';
      } else if (board[i][j] == BLACK) {
        visual_board[i][j] = 'O';
      } else if (board[i][j] == WHITE) {
        visual_board[i][j] = 'X';
      }
    }
    printf("%s\n", visual_board[i]);
  }
}

BOOL isInBoard(int x, int y) {
  return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

/**
 * YOUR CODE BEGIN
 * 你的代码开始
 */

/**
 * You can define your own struct and variable here
 * 你可以在这里定义你自己的结构体和变量
 */

#define Moveable 1
#define Unmoveable 0

struct Position {
  int type;
  int x;
  int y;
  char avaliable_DIR
      [8];  // avaliable_DIR[i][0]表示i方向可不可走，avaliable_DIR[i][1]表示i方向可以吃多少个棋子
} cheese[16];

int MeValue, OtherValue;
/**
 * 自定义函数
 */

/*查找位置为(x,y)的棋子是第几个棋子*/
int find_x_y(int x, int y) {
  for (int i = 0; i < 16; i++)
    if (cheese[i].x == x && cheese[i].y == y) return i;
  return -1;
}

/*确定初始位置时各个棋子的位置*/
void initiate_position() {
  int i = 0;
  for (int j = 0; j < BOARD_SIZE; j++)
    for (int k = 0; k < BOARD_SIZE; k++) {
      if (board[j][k] == WHITE) {
        cheese[i].type = WHITE;
        cheese[i].x = j;
        cheese[i].y = k;
        i++;
      } else if (board[j][k] == BLACK) {
        cheese[i].type = BLACK;
        cheese[i].x = j;
        cheese[i].y = k;
        i++;
      }
    }
}

/*确定初始位置时各个棋子的可移动方*/
void initial_avaliable_movement() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 8; j++) {
      if (board[cheese[i].x + DIRC[j][0]][cheese[i].y + DIRC[j][1]] == EMPTY) {
        cheese[i].avaliable_DIR[j] = Moveable;
      } else {
        cheese[i].avaliable_DIR[j] = Unmoveable;
      }
    }
  }
}

/**
 * -----------------------数据更新函数---------------------------------
 */
void RenovateAvaliableMove(int x, int y, int num) {
  for (int i = 0; i < 8; i++) {
    int new_x, new_y;
    new_x = x + DIRC[i][0];
    new_y = y + DIRC[i][1];
    if (board[new_x][new_y] == EMPTY && isInBoard(new_x, new_y))
      cheese[num].avaliable_DIR[i] = Moveable;
    else
      cheese[num].avaliable_DIR[i] = Unmoveable;
  }
}

/*
 void RenovateData(void){
 for(int i = 0; i < BOARD_SIZE; i++)
 for(int j = 0; j < BOARD_SIZE; j++){
 if(board[i][j] == me_flag){
 int cheese_number = find_x_y(i, j);
 cheese[cheese_number].type = me_flag;
 RenovateAvaliableMove(i, j, cheese_number);
 }
 else if(board[i][j] == other_flag){
 int cheese_number = find_x_y(i, j);
 cheese[cheese_number].type = other_flag;
 RenovateAvaliableMove(i, j, cheese_number);
 }
 }
 }
 */

/**
 * --------------------评估----------------------------
 */
void LocationEvaluate(void);
void Initialate_Battle_Ground(void);
void EatEvaluate(void);
void FormationEvaluate(void);
void BattleGroundEvaluate(void);
void DistanceEvaluare(void);

int Evaluate_InSearcheFunction(void);

double my_value = 0, other_value = 0;
int my_cheese = 8, other_cheese = 8;

int LastMoveCheese = 0;

const char LOCATION_VALUE[BOARD_SIZE][BOARD_SIZE] = {
    {15, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 15},
    {10, 12, 9, 9, 9, 9, 9, 9, 9, 9, 12, 10},
    {10, 9, 8, 8, 8, 8, 8, 8, 8, 8, 9, 10},
    {10, 9, 8, 7, 7, 7, 7, 7, 7, 8, 9, 10},
    {10, 9, 8, 7, 6, 6, 6, 6, 7, 8, 9, 10},
    {10, 9, 8, 7, 6, 2, 2, 6, 7, 8, 9, 10},
    {10, 9, 8, 7, 6, 2, 2, 6, 7, 8, 9, 10},
    {10, 9, 8, 7, 6, 6, 6, 6, 7, 8, 9, 10},
    {10, 9, 8, 7, 7, 7, 7, 7, 7, 8, 9, 10},
    {10, 9, 8, 8, 8, 8, 8, 8, 8, 8, 9, 10},
    {10, 12, 9, 9, 9, 9, 9, 9, 9, 9, 12, 10},
    {15, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 15}};

char BATTLE_FLIED_VALUE[BOARD_SIZE][BOARD_SIZE];
///////////////////////////////////////////////////////
//---------------位置评估----------
void LocationEvaluate(void) {
  int x, y, flag;
  for (int i = 0; i < 16; i++) {
    x = cheese[i].x;
    y = cheese[i].y;
    flag = cheese[i].type;
    if (flag == me_flag) {
      my_value += LOCATION_VALUE[x][y];
      //            my_value += BATTLE_FLIED_VALUE[x][y];
    } else if (flag == other_flag) {
      other_value += LOCATION_VALUE[x][y];
      //            other_value += BATTLE_FLIED_VALUE[x][y];
    }
  }
}

void EatEvaluate() {  //简单的以子的多少来估计价值
  my_cheese = 0;
  other_cheese = 0;
  for (int i = 0; i < 16; i++) {
    if (cheese[i].type == me_flag) {
      my_value += 4000;
      my_cheese++;
    } else if (cheese[i].type == other_flag) {
      other_value += 4000;
      other_cheese++;
    }
  }
}

void FormationEvaluate() {  //棋子周围有子连接阵形更加稳定
  int x, y, new_x, new_y, flag;
  for (int i = 0; i < 16; i++) {
    x = cheese[i].x;
    y = cheese[i].y;
    flag = cheese[i].type;
    for (int j = 0; j < 4; j++) {
      new_x = x + DIRC[j][0];
      new_y = y + DIRC[j][1];
      if (board[new_x][new_y] == me_flag &&
          flag == me_flag)  //直接连接比斜着间接连接更紧密
        my_value += 150;
      else if (board[new_x][new_y] == other_flag && flag == other_flag)
        other_value += 150;
    }

    for (int j = 4; j < 8; j++) {
      new_x = x + DIRC[j][0];
      new_y = y + DIRC[j][1];
      if (board[new_x][new_y] == me_flag && flag == me_flag)
        my_value += 50;
      else if (board[new_x][new_y] == other_flag && flag == other_flag)
        other_value += 50;
    }
  }
}

void Initialate_Battle_Ground() {
  for (int i = 0; i < BOARD_SIZE; i++)
    for (int j = 0; j < BOARD_SIZE; j++) BATTLE_FLIED_VALUE[i][j] = 0;
}

void BattleGroundEvaluate() {
  Initialate_Battle_Ground();

  int x = cheese[LastMoveCheese].x;
  int y = cheese[LastMoveCheese].y;
  int cur_x, cur_y;
  for (int i = -2; i < 2; i++)
    for (int j = -2; j < 2; j++) {
      cur_x = x + i;
      cur_y = y + i;
      BATTLE_FLIED_VALUE[cur_x][cur_y] = 100;
    }
}

void DistanceEvaluare() {
  int x, y;
  float M_ave_x = 0, M_ave_y = 0, O_ave_x = 0, O_ave_y = 0;
  float my_distance = 0, other_distance = 0;
  my_cheese = 0;
  other_cheese = 0;
  for (int i = 0; i < 16; i++) {
    x = cheese[i].x;
    y = cheese[i].y;
    if (cheese[i].type == me_flag) {
      M_ave_x += x;
      M_ave_y += y;
      my_cheese++;
    } else if (cheese[i].type == other_flag) {
      O_ave_x += x;
      O_ave_y += y;
      other_cheese++;
    }
  }

  M_ave_x /= my_cheese;
  M_ave_y /= my_cheese;
  O_ave_x /= other_cheese;
  O_ave_y /= other_cheese;

  for (int i = 0; i < 16; i++) {
    x = cheese[i].x;
    y = cheese[i].y;
    if (cheese[i].type == me_flag) {
      int dis_to_be_rooted =
          (x - M_ave_x) * (x - M_ave_x) + (y - M_ave_y) * (y - M_ave_y);
      my_distance += sqrt(dis_to_be_rooted);
    } else if (cheese[i].type == other_flag) {
      int dis_to_be_rooted =
          (x - O_ave_x) * (x - O_ave_x) + (y - O_ave_y) * (y - O_ave_y);
      other_distance += sqrt(dis_to_be_rooted);
    }
  }
  my_value += (1000 - 10 * my_distance) / 2;
  other_value += (1000 - 10 * other_distance) / 2;
}

int Evaluate_InSearcheFunction() {
  my_value = 0;
  other_value = 0;

  DistanceEvaluare();
  LocationEvaluate();
  EatEvaluate();
  FormationEvaluate();
  test++;
  //    ClearNode();
  return my_value - other_value;
}

/**
 * --------------------搜索-----------------------------
 */
typedef struct {
  int x, y;
  int option;
  int cpture_cheese[8];
} move;
move MoveStack[128];
int StackTop;

int TOTAL_MOVE = 0;

move BestMove;
int cur_depth;
int MaxDepth;
char cur_flag;

//走法生成相关函数-------------------------
int GenAllMove(move *MoveArray);

//搜索函数--------------------------------
void ComputerThink(void);

int MinMaxSearch(int depth);
int AlphaBetaSearch(int depth, int alpha, int beta);
int AlphaBetaSearch_HASH(int depth, int alpha, int beta);

void MakeMove(move *mv);
void UnMakeMove(void);
void ChangeFlag(void);

void Check_around_cheese(int x1, int y1, int x2, int y2, int cheese_num);

void RenovateCaptureCheese_inte(int i);
void RenovateCaptureCheese_cust(int i);
void renovate_cheese_data_InUnMakeMove(int x, int y, int flag);
//时间控制策略-----------------------------
time_t st, ed;
double Duration;
/////////////////////////////////////////

void ComputerThink(void) {
  BestMove.x = 0;
  BestMove.y = 0;
  StackTop = 0;
  cur_depth = 0;
  MaxDepth = 4;
  // MinMaxSearch(MaxDepth);
  st = clock();

  AlphaBetaSearch_HASH(MaxDepth, -100000000, 100000000);
  //    AlphaBetaSearch(MaxDepth, -100000000, 100000000);
  ed = clock();
  double d = ed - st;
  printf("DEBUG:%lf 估值数:%lf\n", d, test);
}

void ChangeFlag(void) { cur_flag = 3 - cur_flag; }

void Check_around_cheese(int x1, int y1, int x2, int y2,
                         int cheese_num) {  //检查走之前和走之后的f八个步伐
  int x, y;

  for (int i = 0; i < 8; i++) {
    x = x1 + DIRC[i][0];
    y = y1 + DIRC[i][1];
    if (board[x][y] != EMPTY) {
      int num = find_x_y(x, y);
      RenovateAvaliableMove(x, y, num);
    }
  }

  for (int i = 0; i < 8; i++) {
    x = x2 + DIRC[i][0];
    y = y2 + DIRC[i][1];
    if (board[x][y] != EMPTY) {
      int num = find_x_y(x, y);
      RenovateAvaliableMove(x, y, num);
    }
  }
}

int GenAllMove(move *MoveArray) {
  int n = 0;
  for (int i = 0; i < 16; i++) {
    if (cheese[i].type == cur_flag) {
      for (int j = 0; j < 8; j++)
        if (cheese[i].avaliable_DIR[j] == Moveable) {
          MoveArray[n].x = cheese[i].x;
          MoveArray[n].y = cheese[i].y;
          MoveArray[n].option = j;
          n++;
        }
    }
  }
  return n;
}

void MakeMove(move *mv) {
  void RenovateCaptureCheese_inte(int i);
  void RenovateCaptureCheese_cust(int i);
  int x, y, option;

  x = mv->x;
  y = mv->y;
  option = mv->option;
  MoveStack[StackTop].x = x;
  MoveStack[StackTop].y = y;
  MoveStack[StackTop].option = mv->option;  //生成走法栈

  for (int n = 0; n < 8; n++)
    MoveStack[StackTop].cpture_cheese[n] = 0;  //将被吃子全部初始化为0

  int new_x, new_y;
  new_x = mv->x + DIRC[option][0];
  new_y = mv->y + DIRC[option][1];

  int num = find_x_y(x, y);
  cheese[num].x = new_x;
  cheese[num].y = new_y;

  board[x][y] = EMPTY;
  board[new_x][new_y] = cur_flag;

  RenovateAvaliableMove(new_x, new_y, num);
  Check_around_cheese(x, y, new_x, new_y, num);  //更新可移动步伐

  int cur_other_flag = 3 - cur_flag;

  // 挑
  int intervention_dir[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
  for (int i = 0; i < 4; i++) {
    int x1 = new_x + intervention_dir[i][0];
    int y1 = new_y + intervention_dir[i][1];
    int x2 = new_x - intervention_dir[i][0];
    int y2 = new_y - intervention_dir[i][1];
    if (isInBoard(x1, y1) && isInBoard(x2, y2) &&
        board[x1][y1] == cur_other_flag && board[x2][y2] == cur_other_flag) {
      board[x1][y1] = cur_flag;
      board[x2][y2] = cur_flag;

      int cheesenum1, cheesenum2;
      cheesenum1 = find_x_y(x1, y1);
      cheesenum2 = find_x_y(x2, y2);

      cheese[cheesenum1].type = cur_flag;  //更新类型数据
      cheese[cheesenum2].type = cur_flag;

      RenovateCaptureCheese_inte(i);
    }
  }

  // 夹

  int custodian_dir[8][2] = {{-1, 0},  {1, 0},  {0, -1}, {0, 1},
                             {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  for (int i = 0; i < 8; i++) {
    int x1 = new_x + custodian_dir[i][0];
    int y1 = new_y + custodian_dir[i][1];
    int x2 = new_x + custodian_dir[i][0] * 2;
    int y2 = new_y + custodian_dir[i][1] * 2;
    if (isInBoard(x1, y1) && isInBoard(x2, y2) && board[x2][y2] == cur_flag &&
        board[x1][y1] == cur_other_flag) {
      board[x1][y1] = cur_flag;

      int cheesenum;
      cheesenum = find_x_y(x1, y1);

      cheese[cheesenum].type = cur_flag;  //更新类型

      RenovateCaptureCheese_cust(i);
    }
  }

  // RenovateData();  //更新可移动步伐
  // printBoard();

  StackTop++;
  cur_depth++;
  ChangeFlag();
}

void RenovateCaptureCheese_inte(int i) {
  switch (i) {
    case 0:
      MoveStack[StackTop].cpture_cheese[0] = 1;
      MoveStack[StackTop].cpture_cheese[1] = 1;
      break;

    case 1:
      MoveStack[StackTop].cpture_cheese[2] = 1;
      MoveStack[StackTop].cpture_cheese[3] = 1;
      break;

    case 2:
      MoveStack[StackTop].cpture_cheese[4] = 1;
      MoveStack[StackTop].cpture_cheese[7] = 1;
      break;

    case 3:
      MoveStack[StackTop].cpture_cheese[5] = 1;
      MoveStack[StackTop].cpture_cheese[6] = 1;
      break;
  }
}

void RenovateCaptureCheese_cust(int i) {
  MoveStack[StackTop].cpture_cheese[i] = 1;
}

void UnMakeMove(void) {
  void renovate_cheese_data_InUnMakeMove(int x, int y, int flag);
  int x, y, option;

  StackTop--;
  cur_depth--;

  ChangeFlag();

  x = MoveStack[StackTop].x;
  y = MoveStack[StackTop].y;
  option = MoveStack[StackTop].option;

  int cur_other_flag = 3 - cur_flag;

  int new_x, new_y;
  new_x = x + DIRC[option][0];
  new_y = y + DIRC[option][1];

  int num = find_x_y(new_x, new_y);
  cheese[num].x = x;
  cheese[num].y = y;

  board[new_x][new_y] = EMPTY;
  board[x][y] = cur_flag;

  RenovateAvaliableMove(x, y, num);
  Check_around_cheese(x, y, new_x, new_y, num);  //更新可移动步伐

  for (int i = 0; i < 8; i++)
    if (MoveStack[StackTop].cpture_cheese[i] == 1) {
      int eat_x = new_x + DIRC[i][0];
      int eat_y = new_y + DIRC[i][1];
      board[eat_x][eat_y] = cur_other_flag;
      renovate_cheese_data_InUnMakeMove(eat_x, eat_y, cur_other_flag);
    }

  // RenovateData();
}

void renovate_cheese_data_InUnMakeMove(int x, int y, int flag) {
  int cheese_number = find_x_y(x, y);
  cheese[cheese_number].type = flag;
}

//-------------搜索算法-------------
int MinMaxSearch(int depth) {
  int best = 0, value;
  move MoveArray[128];

  if (cur_flag == me_flag)
    best = -1000000;
  else if (cur_flag == other_flag)
    best = 1000000;

  if (depth == 0) return Evaluate_InSearcheFunction();

  int TotalMove = GenAllMove(MoveArray);

  for (int i = 0; i < TotalMove; i++) {
    move *mv;
    mv = MoveArray + i;
    MakeMove(mv);
    value = MinMaxSearch(depth - 1);
    UnMakeMove();
    if (cur_flag == other_flag) {
      if (value < best) {
        best = value;
        if (depth == MaxDepth) BestMove = *mv;
      }
    } else if (cur_flag == me_flag) {
      if (value > best) {
        best = value;
        if (depth == MaxDepth) BestMove = *mv;
      }
    }
  }
  return best;
}

int AlphaBetaSearch(int depth, int alpha, int beta) {
  int value;
  move MoveArray[128];
  move BackUpMove;
  if (depth == 0) return Evaluate_InSearcheFunction();

  int totalmove = GenAllMove(MoveArray);
  for (int i = 0; i < totalmove; i++) {
    ed = clock();
    Duration = ed - st;
    if (Duration >= 1900) BestMove = BackUpMove;

    move *mv;
    mv = MoveArray + i;
    MakeMove(mv);
    value = -AlphaBetaSearch(depth - 1, -beta, -alpha);
    UnMakeMove();
    if (value >= beta) return beta;
    if (value > alpha) {
      alpha = value;
      BackUpMove = *mv;
      if (depth == MaxDepth) BestMove = *mv;
    }
  }
  return alpha;
}

// hash功能开发-------------
#define HashTableSize (1024 * 1024 * 4)
#define hashEXACT 0
#define hashALPHA 1
#define hashBETA 2
#define NOVALUE 987656789

int SIZE_OF_MOVE = sizeof(move);
move NULL_MOVE = {0};

typedef struct HashTable {
  move mv;
  int depth;
  int value;
  int flag;
  char check;
} HashTable;

HashTable hashtable[HashTableSize] = {0};
int Key_m[BOARD_SIZE][BOARD_SIZE];
int Key_o[BOARD_SIZE][BOARD_SIZE];
//-------------HASH相关函数---------
void InitiateHashKey(void);
int GenHashKey(void);
int ReadHashTable(int depth, int alpha, int beta, move *mv);
void RecordHashTable(int value, int depth, int type, move mv);
//////////////////////

void InitiateHashKey(void) {
  for (int i = 0; i < BOARD_SIZE; i++)
    for (int j = 0; j < BOARD_SIZE; j++) {
      Key_m[i][j] = rand();
      Key_o[i][j] = rand();
    }
}

int GenHashKey(void) {
  int key = 0;
  int x, y;
  for (int i = 0; i < 16; i++) {
    x = cheese[i].x;
    y = cheese[i].y;
    if (cheese[i].type == me_flag) {
      key = key ^ Key_m[x][y];
    } else
      key = key ^ Key_o[x][y];
  }
  int pos = (key % HashTableSize);
  return pos;
}

int ReadHashTable(int depth, int alpha, int beta, move *mv) {
  int pos = GenHashKey();
  HashTable *p;
  if (hashtable[pos].flag == 1) {  //哈希表中有对应棋局
    p = &hashtable[pos];
    if (p->depth >= depth) {
      if (p->flag == hashEXACT) return p->value;
      if (p->flag == hashALPHA && p->value <= alpha) return alpha;
      if (p->flag == hashBETA && p->value >= beta) return beta;
      *mv = p->mv;
    }
  }
  return NOVALUE;
}

void RecordHashTable(int value, int depth, int type, move mv) {
  int pos = GenHashKey();
  hashtable[pos].mv = mv;
  hashtable[pos].depth = depth;
  hashtable[pos].value = value;
  hashtable[pos].flag = type;
  hashtable[pos].check = 1;
}

int AlphaBetaSearch_HASH(int depth, int alpha, int beta) {
  int value, best;
  int num;
  move MoveArray[128];
  move mv, goodmove;
  goodmove.x = 999;

  value = ReadHashTable(depth, alpha, beta, &mv);
  if (value != NOVALUE) {
    if (depth == MaxDepth) BestMove = mv;
    return value;
  }

  best = -NOVALUE;
  int alphaFlag = 1;

  if (depth <= 0) {
    value = Evaluate_InSearcheFunction();
    RecordHashTable(value, depth, hashEXACT, NULL_MOVE);
    return value;
  }

  else {
    if (!memcmp(&mv, &NULL_MOVE, SIZE_OF_MOVE)) {  //如果mv读取了哈希表
      MoveArray[0] = mv;
      num = GenAllMove(MoveArray + 1);
      num += 1;
    } else
      num = GenAllMove(MoveArray);

    for (int i = 0; i < num; i++) {
      move *mv;
      mv = MoveArray + i;
      MakeMove(mv);
      value = -AlphaBetaSearch_HASH(depth - 1, -beta, -alpha);
      UnMakeMove();
      if (value >= beta) {
        RecordHashTable(beta, depth, hashBETA, MoveArray[i]);
        return beta;
      }
      if (value > best) {
        best = value;
        goodmove = MoveArray[i];
        if (value > alpha) {
          alphaFlag = 0;
          alpha = value;
          if (depth == MaxDepth) BestMove = *mv;
        }
      }
    }

    if (alphaFlag == 1)
      RecordHashTable(
          alpha, depth, hashALPHA,
          goodmove);  //??? 这里的goodmove可能没有初始化？？？？为什么！
    // TODO: 这里要看一看goodmv什么时候存在写入空的情况
    else
      RecordHashTable(best, depth, hashEXACT, goodmove);
    return alpha;
  }
}

/**
 * 你可以在这里初始化你的AI
 */

void initAI(int me) {
  initiate_position();
  initial_avaliable_movement();
  InitiateHashKey();
  MaxDepth = 4;
  AlphaBetaSearch_HASH(MaxDepth, -100000000, 100000000);
  test = 0;
}
//    int BlackLocation[16][2] =
//    {{2,3},{3,2},{4,2},{6,6},{7,6},{8,6},{2,8},{2,9}}; int
//    WhiteLocation[16][2] = {{3,5},{4,5},{5,5},{9,2},{9,3},{7,9},{8,9},{9,9}};

/**
 * ------------------命令函数-------------------
 */
struct Command COMMAND(const char board[BOARD_SIZE][BOARD_SIZE], int me);
struct Command ForOpening(const char board[BOARD_SIZE][BOARD_SIZE], int me);
struct Command aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me);
//--------------------------------------------

struct Command COMMAND(const char board[BOARD_SIZE][BOARD_SIZE], int me) {
  //    if(TOTAL_MOVE == 0 && me_flag == BLACK){
  //        TOTAL_MOVE++;
  //        return ForOpening((const char (*)[BOARD_SIZE])board, me_flag);
  //    }
  //    else{
  //        TOTAL_MOVE++;
  return aiTurn((const char (*)[BOARD_SIZE])board, me_flag);
  //    }
}

struct Command
aiTurn(const char board[BOARD_SIZE][BOARD_SIZE], int me) {
  struct Command BESTMOVE = {1, 1, 1};
  ComputerThink();
  BESTMOVE.x = BestMove.x;
  BESTMOVE.y = BestMove.y;
  BESTMOVE.option = BestMove.option;

  int cheese_number = find_x_y(BESTMOVE.x, BESTMOVE.y);
  cheese[cheese_number].x = BESTMOVE.x + DIRC[BestMove.option][0];
  cheese[cheese_number].y = BESTMOVE.y + DIRC[BestMove.option][1];
  lastcheese = cheese_number;
  return BESTMOVE;
}

struct Command ForOpening(const char board[BOARD_SIZE][BOARD_SIZE], int me) {
  struct Command OPEN = {9, 7, 0};

  int cheese_number = find_x_y(9, 7);

  lastcheese = cheese_number;

  cheese[cheese_number].x = 9 + DIRC[0][0];
  cheese[cheese_number].y = 7 + DIRC[0][1];

  return OPEN;
}

/**
 * 你的代码结束
 */

BOOL place(int x, int y, OPTION option, int cur_flag) {
  // 移动之前的位置没有我方棋子
  if (board[x][y] != cur_flag) {
    return FALSE;
  }

  int new_x = x + DIRC[option][0];
  int new_y = y + DIRC[option][1];
  // 移动之后的位置超出边界, 或者不是空地
  if (!isInBoard(new_x, new_y) || board[new_x][new_y] != EMPTY) {
    return FALSE;
  }

  int cheese_number = find_x_y(x, y);
  cheese[cheese_number].x = new_x;
  cheese[cheese_number].y = new_y;

  board[x][y] = EMPTY;
  board[new_x][new_y] = cur_flag;

  Check_around_cheese(x, y, new_x, new_y, cheese_number);

  int cur_other_flag = 3 - cur_flag;

  // 挑
  int intervention_dir[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
  for (int i = 0; i < 4; i++) {
    int x1 = new_x + intervention_dir[i][0];
    int y1 = new_y + intervention_dir[i][1];
    int x2 = new_x - intervention_dir[i][0];
    int y2 = new_y - intervention_dir[i][1];
    if (isInBoard(x1, y1) && isInBoard(x2, y2) &&
        board[x1][y1] == cur_other_flag && board[x2][y2] == cur_other_flag) {
      board[x1][y1] = cur_flag;
      board[x2][y2] = cur_flag;
      int num1 = find_x_y(x1, y1), num2 = find_x_y(x2, y2);
      cheese[num1].type = cur_flag;
      cheese[num2].type = cur_flag;
    }
  }

  // 夹
  int custodian_dir[8][2] = {{-1, 0},  {1, 0},  {0, -1}, {0, 1},
                             {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
  for (int i = 0; i < 8; i++) {
    int x1 = new_x + custodian_dir[i][0];
    int y1 = new_y + custodian_dir[i][1];
    int x2 = new_x + custodian_dir[i][0] * 2;
    int y2 = new_y + custodian_dir[i][1] * 2;
    if (isInBoard(x1, y1) && isInBoard(x2, y2) && board[x2][y2] == cur_flag &&
        board[x1][y1] == cur_other_flag) {
      board[x1][y1] = cur_flag;
      int num = find_x_y(x1, y1);
      cheese[num].type = cur_flag;
    }
  }

  return TRUE;
}

/*
 //测试用
 void start(int flag) {
 memset(board, 0, sizeof(board));

 for (int i = 0; i < 3; i++) {
 board[6][6 + i] = WHITE;
 board[5][3 + i] = BLACK;
 board[9][7 + i] = BLACK;
 }
 board[2][2] = board[1][8] = board[2][10] = WHITE;

 for (int i = 0; i < 2; i++) {
 board[8 + i][2] = WHITE;
 board[2 + i][9] = BLACK;
 }

 initAI(flag);
 }
 */
void start(int flag) {
  memset(board, 0, sizeof(board));

  for (int i = 0; i < 3; i++) {
    board[2][2 + i] = WHITE;
    board[6][6 + i] = WHITE;
    board[5][3 + i] = BLACK;
    board[9][7 + i] = BLACK;
  }

  for (int i = 0; i < 2; i++) {
    board[8 + i][2] = WHITE;
    board[2 + i][9] = BLACK;
  }

  initAI(flag);
}

void turn() {
  // AI
  struct Command command = COMMAND((const char(*)[BOARD_SIZE])board, me_flag);
  place(command.x, command.y, command.option, me_flag);
  printf("%d %d %d\n", command.x, command.y, command.option);
  fflush(stdout);
}

void end(int x) {}

char cur_flag;

void loop() {
  void debug(void);
  //  freopen("../input", "r", stdin);
  while (TRUE) {
    memset(buffer, 0, sizeof(buffer));
    gets(buffer);

    if (strstr(buffer, START)) {
      char tmp[MAX_BYTE] = {0};
      sscanf(buffer, "%s %d", tmp, &me_flag);
      other_flag = 3 - me_flag;
      cur_flag = me_flag;
      start(me_flag);  //充分利用时间 填充hash表
      printf("OK\n");
      fflush(stdout);
    } else if (strstr(buffer, PLACE)) {
      char tmp[MAX_BYTE] = {0};
      int x, y;
      OPTION option;
      sscanf(buffer, "%s %d %d %d", tmp, &x, &y, &option);
      place(x, y, option, other_flag);

      int num = find_x_y(x, y);
      lastcheese = num;

      cheese[num].x = x + DIRC[option][0];
      cheese[num].y = y + DIRC[option][1];

      //            RenovateData();

    } else if (strstr(buffer, TURN)) {
      turn();
      ///////////asdasigfsadhkfhsighvdjaioufgyhaijopdhfjakojfhksakdjhklsjahsbjadhbj
      test = 0;

      //            RenovateData();
    } else if (strstr(buffer, END)) {
      char tmp[MAX_BYTE] = {0};
      int x;
      sscanf(buffer, "%s %d", tmp, &x);
      end(x);
    }
    //   printBoard();
  }
}

int main(int argc, char *argv[]) {
  loop();
  return 0;
}
