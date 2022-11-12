#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libev-master/ev.h"

#define new_game 1
#define load 2
#define boardsize 9
#define Aposi_0 0 //array position
#define Aposi_1 1 
#define Aposi_2 2
#define Aposi_3 3
#define Aposi_4 4
#define Aposi_5 5
#define Aposi_6 6
#define Aposi_7 7
#define Aposi_8 8
#define for_start 0
#define mychess 1
#define yourchess 2
#define none 0
#define xiang_che 1 //香車
#define gui_ma 2 //桂馬
#define yin_jiang 3 //銀將
#define jin_jiang 4 //金將
#define wang_jiang 5 //王將
#define fei_che 6 //飛車
#define jiao_xing 7 //角行
#define bu_bing 8 //步兵
#define n_Num_2 -2
#define n_Num_1 -1
#define Num_0 0
#define Num_1 1
#define Num_2 2
#define Num_3 3
#define Num_4 4
#define myturn 1
#define yourturn 2
#define true 1
#define false 0
#define fnamesize 21

int indep=Num_0;
int now;
int turn;
int ftime = Num_1;
ev_io stdin_watcher;
ev_timer timeout_watcherx;
ev_timer timeout_watchery;

typedef struct{
    int whose;//誰的棋子
    int chess_name;//棋子種類
} chess;

typedef struct node{
    int o_row;
    int o_col;
    int d_row;
    int d_col;
    int eated;
    int whose_turn;
    struct node *next;
    struct node *pre;
}node;

static void stdin_cb (EV_P_ ev_io *w, int revents){
    ev_timer_stop (loop, &timeout_watcherx);	
    ev_timer_stop (loop, &timeout_watchery);
	if(turn == myturn){	
		ev_timer_stop (loop, &timeout_watchery);	
		ev_timer_start (loop, &timeout_watcherx);
	}
	else{
		ev_timer_stop (loop, &timeout_watcherx);
		ev_timer_start (loop, &timeout_watchery);
	}
}

static void timeout_cbx (EV_P_ ev_timer *w, int revents){
	if(ftime == Num_1){
        printf("The time of player x use: %d\n",(int)ev_now(loop)-now);
        indep=(int)ev_now(loop)-now;
        ftime += Num_1;
    }
    else{
        printf("The time of player x use: %d\n",(int)ev_now(loop)-now-indep);
        indep=(int)ev_now(loop)-now;
    }
    ev_timer_stop (loop, &timeout_watcherx);
    ev_break(loop, EVBREAK_ONE);
}

static void timeout_cby (EV_P_ ev_timer *w, int revents){
    printf("The time of player y use: %d\n",(int)ev_now(loop)-now-indep);
    indep=(int)ev_now(loop)-now;
    ev_timer_stop (loop, &timeout_watchery);
    ev_break(loop, EVBREAK_ONE);
}

void initialize(chess **c);
void num_to_name(int cname, int whose);
void print_board(chess **c);
node *play_chess(int *end, chess **c, node *nptr, char *fname, struct ev_loop *loop);
void move_chess(int *o_row, int *o_col, int *d_row, int *d_col, chess **c);
int move_rule(int *o_row, int *o_col, int *d_row, int *d_col, chess **c);
node *regret(chess **c, node *nptr);
int end_or_not(chess **c, int *d_row, int *d_col);
node *record(node *nptr, int *o_row, int *o_col, int *d_row, int *d_col, int *eated);
void save(node *nptr, char *fname);
node *load_file(node *nptr, char *fname);
node *watch(chess **c, node *nptr, int *end);

int main(int argc, char *argv[]){    
    if(argc == Num_4 && strcmp(argv[Num_1], "-n") == Num_0){
        chess** chessboard = (chess**)malloc(boardsize * sizeof(chess*));
        for(int i = for_start; i < boardsize; i++)
            chessboard[i] = (chess*)malloc(boardsize * sizeof(chess));
        initialize(chessboard);

        turn = myturn;
        int end = false;
        node *nptr = NULL;

        struct ev_loop *loop = EV_DEFAULT;
        now=ev_now(loop);  
        ev_io_init(&stdin_watcher, stdin_cb, /*STDIN_FILENO*/ 0, EV_READ);
	    ev_io_start(loop, &stdin_watcher);
	    ev_timer_init(&timeout_watcherx, timeout_cbx, Num_0, Num_1);
        ev_timer_init(&timeout_watchery, timeout_cby, Num_0, Num_1);

        while(end != true){
            nptr = play_chess(&end, chessboard, nptr, argv[Num_3], loop);
        }

        ev_timer_stop (loop, &timeout_watcherx);
        ev_timer_stop (loop, &timeout_watchery);
    }
    else if(argc == Num_3 && strcmp(argv[Num_1], "-l") == Num_0){
        node *nptr = NULL;
        nptr = load_file(nptr, argv[Num_2]);
        if(nptr != NULL){
            chess** chessboard = (chess**)malloc(boardsize * sizeof(chess*));
            for(int i = for_start; i < boardsize; i++)
                chessboard[i] = (chess*)malloc(boardsize * sizeof(chess));
            initialize(chessboard);

            int end = false;

            while(end != true)
                nptr = watch(chessboard, nptr, &end);
        }    
    }
    else{
        printf("Error input.\n");
    }
}

void initialize(chess **c){
    for(int i = Aposi_0; i <= Aposi_8; i++){
        (*(*(c+Aposi_2)+i)).whose = yourchess;
        (*(*(c+Aposi_2)+i)).chess_name = bu_bing;
    }
    for(int i = Aposi_0; i <= Aposi_8; i++){
        (*(*(c+Aposi_6)+i)).whose = mychess;
        (*(*(c+Aposi_6)+i)).chess_name = bu_bing;
    }
    for(int i = Aposi_3; i <= Aposi_5; i++){
        for(int j = Aposi_0; j <= Aposi_8; j++){
            (*(*(c+i)+j)).whose = none;
            (*(*(c+i)+j)).chess_name = none;
        }
    }
    (*(*(c+Aposi_0)+0)).whose = yourchess;
    (*(*(c+Aposi_0)+0)).chess_name = xiang_che;
    (*(*(c+Aposi_0)+1)).whose = yourchess;
    (*(*(c+Aposi_0)+1)).chess_name = gui_ma;
    (*(*(c+Aposi_0)+2)).whose = yourchess;
    (*(*(c+Aposi_0)+2)).chess_name = yin_jiang;
    (*(*(c+Aposi_0)+3)).whose = yourchess;
    (*(*(c+Aposi_0)+3)).chess_name = jin_jiang;
    (*(*(c+Aposi_0)+4)).whose = yourchess;
    (*(*(c+Aposi_0)+4)).chess_name = wang_jiang;
    (*(*(c+Aposi_0)+5)).whose = yourchess;
    (*(*(c+Aposi_0)+5)).chess_name = jin_jiang;
    (*(*(c+Aposi_0)+6)).whose = yourchess;
    (*(*(c+Aposi_0)+6)).chess_name = yin_jiang;
    (*(*(c+Aposi_0)+7)).whose = yourchess;
    (*(*(c+Aposi_0)+7)).chess_name = gui_ma;
    (*(*(c+Aposi_0)+8)).whose = yourchess;
    (*(*(c+Aposi_0)+8)).chess_name = xiang_che;
    (*(*(c+Aposi_1)+1)).whose = yourchess;
    (*(*(c+Aposi_1)+1)).chess_name = fei_che;
    (*(*(c+Aposi_1)+7)).whose = yourchess;
    (*(*(c+Aposi_1)+7)).chess_name = jiao_xing;
    (*(*(c+Aposi_8)+0)).whose = mychess;
    (*(*(c+Aposi_8)+0)).chess_name = xiang_che;
    (*(*(c+Aposi_8)+1)).whose = mychess;
    (*(*(c+Aposi_8)+1)).chess_name = gui_ma;
    (*(*(c+Aposi_8)+2)).whose = mychess;
    (*(*(c+Aposi_8)+2)).chess_name = yin_jiang;
    (*(*(c+Aposi_8)+3)).whose = mychess;
    (*(*(c+Aposi_8)+3)).chess_name = jin_jiang;
    (*(*(c+Aposi_8)+4)).whose = mychess;
    (*(*(c+Aposi_8)+4)).chess_name = wang_jiang;
    (*(*(c+Aposi_8)+5)).whose = mychess;
    (*(*(c+Aposi_8)+5)).chess_name = jin_jiang;
    (*(*(c+Aposi_8)+6)).whose = mychess;
    (*(*(c+Aposi_8)+6)).chess_name = yin_jiang;
    (*(*(c+Aposi_8)+7)).whose = mychess;
    (*(*(c+Aposi_8)+7)).chess_name = gui_ma;
    (*(*(c+Aposi_8)+8)).whose = mychess;
    (*(*(c+Aposi_8)+8)).chess_name = xiang_che;
    (*(*(c+Aposi_7)+1)).whose = mychess;
    (*(*(c+Aposi_7)+1)).chess_name = jiao_xing;
    (*(*(c+Aposi_7)+7)).whose = mychess;
    (*(*(c+Aposi_7)+7)).chess_name = fei_che;
    for(int i = Aposi_2; i <= Aposi_6; i++){
        (*(*(c+Aposi_1)+i)).whose = none;
        (*(*(c+Aposi_1)+i)).chess_name = none;
    }
    for(int i = Aposi_2; i <= Aposi_6; i++){
        (*(*(c+Aposi_7)+i)).whose = none;
        (*(*(c+Aposi_7)+i)).chess_name = none;
    }
    (*(*(c+Aposi_1)+Aposi_0)).whose = none;
    (*(*(c+Aposi_1)+Aposi_0)).chess_name = none;
    (*(*(c+Aposi_1)+Aposi_8)).whose = none;
    (*(*(c+Aposi_1)+Aposi_8)).chess_name = none;
    (*(*(c+Aposi_7)+Aposi_0)).whose = none;
    (*(*(c+Aposi_7)+Aposi_0)).chess_name = none;
    (*(*(c+Aposi_7)+Aposi_8)).whose = none;
    (*(*(c+Aposi_7)+Aposi_8)).chess_name = none;

    print_board(c);
}// 初始化棋盤

void num_to_name(int cname, int whose){
    if(whose == mychess){
        switch (cname){
        case xiang_che:
            printf(" \033[44m香車\033[m");
            break;
        case gui_ma:
            printf(" \033[44m桂馬\033[m");
            break;
        case yin_jiang:
            printf(" \033[44m銀將\033[m");
            break;
        case jin_jiang:
            printf(" \033[44m金將\033[m");
            break;
        case wang_jiang:
            printf(" \033[44m王將\033[m");
            break;
        case bu_bing:
            printf(" \033[44m步兵\033[m");
            break;
        case fei_che:
            printf(" \033[44m飛車\033[m");
            break;
        case jiao_xing:
            printf(" \033[44m角行\033[m");
            break;
        default:
            break;
        }
    }
    else if(whose == yourchess){
        switch (cname){
        case xiang_che:
            printf(" \033[41m香車\033[m");
            break;
        case gui_ma:
            printf(" \033[41m桂馬\033[m");
            break;
        case yin_jiang:
            printf(" \033[41m銀將\033[m");
            break;
        case jin_jiang:
            printf(" \033[41m金將\033[m");
            break;
        case wang_jiang:
            printf(" \033[41m王將\033[m");
            break;
        case bu_bing:
            printf(" \033[41m步兵\033[m");
            break;
        case fei_che:
            printf(" \033[41m飛車\033[m");
            break;
        case jiao_xing:
            printf(" \033[41m角行\033[m");
            break;
        default:
            break;
        }
    }
    else if(whose == none)
        printf("     ");
}//輸出棋子的名字

void print_board(chess **c){
    for(int i = Aposi_0; i <= Aposi_8; i++){
        for(int j = Aposi_0; j <= Aposi_8; j++){
            printf("(%d,%d)", i+Num_1, boardsize-j);
            num_to_name((*(*(c+i)+j)).chess_name, (*(*(c+i)+j)).whose);
            if(j == Aposi_8)
                printf("\n");
        }
    }
}//輸出棋盤

node *play_chess(int *end,chess **c, node *nptr, char *fname, struct ev_loop *loop){
    int count = Num_1;
    int origen_row, origen_col, destinate_row, destinate_col;
    int save_done = false;
    char save_or_not;
    int eat_king = false;

    if(turn == myturn)
        printf("Now is player x's turn.\n");
    else
        printf("Now is player y's turn.\n");
    do{
        printf("Do you want to save the process to the chess manual file? (S,s / N,n) ");
        scanf(" %c", &save_or_not);
        if(save_or_not == 'S' || save_or_not == 's'){
            save(nptr, fname);
            save_done = true;
            char leave;
            printf("Do you went to leave? (Y,y / N,n) ");
            scanf(" %c", &leave);
            if(leave == 'Y' || leave == 'y'){
                printf("Thank you for using, bye.\n");
                *end = true;
            }
        }
        else if(save_or_not == 'N' || save_or_not == 'n')
            save_done = true;
        else
            printf("Please enter S,s or N,n to answer the question!\n");
    }while(save_done != true);

    while(count <= Num_4 && *end != true){
        switch (count){
        case Num_1:
            printf("Please enter the row where the chess position is located or enter 0 to regret: ");
            scanf("%d", &origen_row);
            if(origen_row >= Num_1 && origen_row <= boardsize)
                count += Num_1;
            else if(origen_row == Num_0){
                nptr = regret(c, nptr);
                if(turn == myturn)
                    printf("Now is player x's turn.\n");
                else
                    printf("Now is player y's turn.\n");
                count = Num_1;
            }
            else
                printf("Please enter the right number!!!\n");
            break;
        case Num_2:
            printf("Please enter the column where the chess position is located or enter 0 to regret: ");
            scanf("%d", &origen_col);
            if(origen_col >= Num_1 && origen_col <= boardsize){
                int memory_row = origen_row - Num_1;
                int memory_col = boardsize - origen_col;
                if((*(*(c+memory_row) + memory_col)).whose == turn)
                    count += Num_1;
                else if((*(*(c+memory_row) + memory_col)).whose == none){
                    printf("There is no chess!\n");
                    count = Num_1;
                }
                else{
                    printf("The chess isn't yours!\n");
                    count = Num_1;
                }
            }    
            else if(origen_col == Num_0){
                nptr = regret(c, nptr);
                if(turn == myturn)
                    printf("Now is player x's turn.\n");
                else
                    printf("Now is player y's turn.\n");
                count = Num_1;
            }
            else
                printf("Please enter the right number!!!\n");
            break;
        case Num_3:
            printf("Please enter the row where you went to move the chess or enter 0 to regret: ");
            scanf("%d", &destinate_row);
            if(destinate_row >= Num_1 && destinate_row <= boardsize)
                count += Num_1;
            else if(destinate_row == Num_0){
                nptr = regret(c, nptr);
                if(turn == myturn)
                    printf("Now is player x's turn.\n");
                else
                    printf("Now is player y's turn.\n");
                count = Num_1;
            }
            else
                printf("Please enter the right number!!!\n");
            break;
        case Num_4:
            printf("Please enter the column where you went to move the chess or enter 0 to regret: ");
            scanf("%d", &destinate_col);
            if(destinate_col >= Num_1 && destinate_col <= boardsize){
                int m_o_r = origen_row - Num_1;
                int m_o_c = boardsize - origen_col;
                int m_d_r = destinate_row - Num_1;
                int m_d_c = boardsize - destinate_col;

                if(move_rule(&m_o_r, &m_o_c, &m_d_r, &m_d_c, c)){
                    count += Num_1;
                    nptr = record(nptr, &origen_row, &origen_col, &destinate_row, &destinate_col, &(*(*(c+m_d_r)+m_d_c)).chess_name);
                    if(end_or_not(c, &m_d_r, &m_d_c)){
                        move_chess(&m_o_r, &m_o_c, &m_d_r, &m_d_c, c);
                        print_board(c);
                        if(turn == myturn)
                            printf("The player x is the winner!\n");
                        else
                            printf("The player y is the winner!\n");
                        eat_king = true;
                    }
                    else{
                        move_chess(&m_o_r, &m_o_c, &m_d_r, &m_d_c, c);
                        print_board(c);
                        if(turn == myturn && *end != true){
                            ev_timer_start(loop, &timeout_watcherx);
                            ev_run(loop, 0);
                        }
                        else if(turn == yourturn && *end != true){
                            ev_timer_start(loop, &timeout_watchery);
                            ev_run(loop, 0);
                        }  
                        if(turn == myturn)
                            turn = yourturn;
                        else
                            turn = myturn;
                    }
                }
                else{
                    printf("Can`t move the chess to this position.\n");
                    count = Num_1;
                }
            } 
            else if(destinate_col == Num_0){
                nptr = regret(c, nptr);
                if(turn == myturn)
                    printf("Now is player x's turn.\n");
                else
                    printf("Now is player y's turn.\n");
                count = Num_1;
            }
            else
                printf("Please enter the right number!!!\n");
            break;
        default:
            break;
        }
    }  
    
    if(eat_king == true){
        char Ans;
        int done = false;

        if(turn == myturn){
            turn = yourturn;
            printf("Do player y want to regret? (Y,y / N,n) ");
        }
        else{
            turn = myturn;
            printf("Do player x want to regret? (Y,y / N,n) ");
        }
        scanf(" %c", &Ans);
        while(done != true){
            if(Ans == 'Y' || Ans == 'y'){
                nptr = regret(c, nptr);
                done = true;
            }
            else if(Ans == 'N' || Ans == 'n'){
                done = true;
                *end = true;
            }
            else{
                printf("Please enter Y,y or N,n to answer the question!\n");
                if(turn == myturn)
                    printf("Do player x want to regret? (Y,y / N,n) ");
                else
                    printf("Do player y want to regret? (Y,y / N,n) ");
                scanf("%c", &Ans);
            }
        }                        
    }

    return(nptr);
}//下棋

void move_chess(int *o_row, int *o_col, int *d_row, int *d_col, chess **c){
    (*(*(c+*d_row)+*d_col)).chess_name = (*(*(c+*o_row)+*o_col)).chess_name;
    (*(*(c+*d_row)+*d_col)).whose = (*(*(c+*o_row)+*o_col)).whose;
    (*(*(c+*o_row)+*o_col)).chess_name = none;
    (*(*(c+*o_row)+*o_col)).whose = none;
}//移動棋子

int move_rule(int *o_row, int *o_col, int *d_row, int *d_col, chess **c){
    if((*(*(c+*d_row)+*d_col)).whose == turn)
        return(false);

    int move_row = *d_row - *o_row;
    int move_col = *d_col - *o_col;

    switch ((*(*(c+*o_row)+*o_col)).chess_name){
    case xiang_che:
        if(turn == myturn){
            if(move_row == n_Num_1 && move_col == Num_0)
                return(true);//前進一格
            else if(move_row < Num_0 && move_col == Num_0){
                for(int i = *o_row - Num_1; i > *d_row; i--){
                    if((*(*(c+i)+*o_col)).whose != none)
                        return(false);
                    else if(i == *d_row + Num_1)
                        return(true);
                }
            }//前進n格
            else
                return(false); 
        }
        else{
            if(move_row == Num_1 && move_col == Num_0)
                return(true);//前進一格
            else if(move_row > Num_0 && move_col == Num_0){
                for(int i = *o_row + Num_1; i < *d_row; i++){
                    if((*(*(c+i)+*o_col)).whose != none)
                        return(false);
                    else if(i == *d_row - Num_1)
                        return(true);
                }
            }//前進n格
            else
                return(false); 
        }
        break;
    case gui_ma:
        if(turn == myturn){
            if(move_row == n_Num_2 && (move_col == n_Num_1 || move_col == Num_1))
                return(true);
            else
                return(false); 
        }
        else{
            if(move_row == Num_2 && (move_col == n_Num_1 || move_col == Num_1))
                return(true);
            else
                return(false); 
        }
        break;
    case yin_jiang:
        if(turn == myturn){
            if((move_row == n_Num_1 && move_col == n_Num_1) ||\
               (move_row == n_Num_1 && move_col == Num_0) ||\
               (move_row == n_Num_1 && move_col == Num_1) ||\
               (move_row == Num_1 && move_col == n_Num_1) ||\
               (move_row == Num_1 && move_col == Num_1))
                return(true);
            else
                return(false); 
        }
        else{
            if((move_row == Num_1 && move_col == n_Num_1) ||\
               (move_row == Num_1 && move_col == Num_0) ||\
               (move_row == Num_1 && move_col == Num_1) ||\
               (move_row == n_Num_1 && move_col == n_Num_1) ||\
               (move_row == n_Num_1 && move_col == Num_1))
                return(true);
            else
                return(false); 
        }
        break;
    case jin_jiang:
        if(turn == myturn){
            if((move_row == n_Num_1 && move_col == n_Num_1) ||\
               (move_row == n_Num_1 && move_col == Num_0) ||\
               (move_row == n_Num_1 && move_col == Num_1) ||\
               (move_row == Num_0 && move_col == n_Num_1) ||\
               (move_row == Num_0 && move_col == Num_1) ||\
               (move_row == Num_1 && move_col == Num_0))
                return(true);
            else
                return(false); 
        }
        else{
            if((move_row == Num_1 && move_col == n_Num_1) ||\
               (move_row == Num_1 && move_col == Num_0) ||\
               (move_row == Num_1 && move_col == Num_1) ||\
               (move_row == Num_0 && move_col == n_Num_1) ||\
               (move_row == Num_0 && move_col == Num_1) ||\
               (move_row == n_Num_1 && move_col == Num_0))
                return(true);
            else
                return(false); 
        }
        break;
    case wang_jiang:
        if((move_row == Num_1 && move_col == n_Num_1) ||\
           (move_row == Num_1 && move_col == Num_0) ||\
           (move_row == Num_1 && move_col == Num_1) ||\
           (move_row == Num_0 && move_col == n_Num_1) ||\
           (move_row == Num_0 && move_col == Num_1) ||\
           (move_row == n_Num_1 && move_col == n_Num_1) ||\
           (move_row == n_Num_1 && move_col == Num_0) ||\
           (move_row == n_Num_1 && move_col == Num_1))
            return(true);
        else
            return(false); 
        break;
    case bu_bing:
        if(turn == myturn){
            if(move_row == n_Num_1 && move_col == Num_0)
                return(true);
            else
                return(false); 
        }
        else{
            if(move_row == Num_1 && move_col == Num_0)
                return(true);
            else
                return(false); 
        }
        break;
    case fei_che:
        if((move_row == Num_1 && move_col == Num_0) ||\
           (move_row == n_Num_1 && move_col == Num_0) ||\
           (move_row == Num_0 && move_col == Num_1) ||\
           (move_row == Num_0 && move_col == n_Num_1))
           return(true);//移動一格
        else if(move_row < Num_0 && move_col == Num_0){
            for(int i = *o_row - Num_1; i > *d_row; i--){
                if((*(*(c+i)+*o_col)).whose != none)
                    return(false);
                else if(i == *d_row + Num_1)
                    return(true);
            }
        }//向上n格
        else if(move_row > Num_0 && move_col == Num_0){
            for(int i = *o_row + Num_1; i < *d_row; i++){
                if((*(*(c+i)+*o_col)).whose != none)
                    return(false);
                else if(i == *d_row - Num_1)
                    return(true);
            }
        }//向下n格
        else if(move_row == Num_0 && move_col > Num_0){
            for(int i = *o_col + Num_1; i < *d_col; i++){
                if((*(*(c+*o_row)+i)).whose != none)
                    return(false);
                else if(i == *d_col - Num_1)
                    return(true);
            }
        }//向右n格
        else if(move_row == Num_0 && move_col < Num_0){
            for(int i = *o_col - Num_1; i > *d_col; i--){
                if((*(*(c+*o_row)+i)).whose != none)
                    return(false);
                else if(i == *d_col + Num_1)
                    return(true);
            }
        }//向左n格
        else
            return(false);        
        break;
    case jiao_xing:
        if(abs(move_row) == abs(move_col)){
            if((move_row == Num_1 && move_col == Num_1) ||\
               (move_row == Num_1 && move_col == n_Num_1) ||\
               (move_row == n_Num_1 && move_col == Num_1) ||\
               (move_row == n_Num_1 && move_col == n_Num_1))
               return(true);//移動一格
            else if(move_row > Num_0 && move_col > Num_0){
                for(int i = *o_row + Num_1, j = *o_col + Num_1; i < *d_row; i++, j++){
                    if((*(*(c+i)+j)).whose != none)
                        return(false);
                    else if(i == *d_row - Num_1)
                        return(true);
                }
            }//向右下n格
            else if(move_row > Num_0 && move_col < Num_0){
                for(int i = *o_row + Num_1, j = *o_col - Num_1; i < *d_row; i++, j--){
                    if((*(*(c+i)+j)).whose != none)
                        return(false);
                    else if(i == *d_row - Num_1)
                        return(true);
                }
            }//向左下n格
            else if(move_row < Num_0 && move_col > Num_0){
                for(int i = *o_row - Num_1, j = *o_col + Num_1; i > *d_row; i--, j++){
                    if((*(*(c+i)+j)).whose != none)
                        return(false);
                    else if(i == *d_row + Num_1)
                        return(true);
                }
            }//向右上n格
            else if(move_row < Num_0 && move_col < Num_0){
                for(int i = *o_row - Num_1, j = *o_col - Num_1; i > *d_row; i--, j--){
                    if((*(*(c+i)+j)).whose != none)
                        return(false);
                    else if(i == *d_row + Num_1)
                        return(true);
                }
            }//向左上n格
        }
        else
            return(false);
        break;
    default:
        break;
    }
}//棋子允許的步法

int end_or_not(chess **c, int *d_row, int *d_col){
    if((*(*(c+*d_row)+*d_col)).chess_name == wang_jiang){
        return(true);
    }
    else
        return(false);
}//判斷王將是否被吃掉

node *record(node *nptr, int *o_row, int *o_col, int *d_row, int *d_col, int *eated){
    node *newnode = (node*)malloc(sizeof(node));
    newnode->o_row = *o_row;
    newnode->o_col = *o_col;
    newnode->d_row = *d_row;
    newnode->d_col = *d_col;
    newnode->eated = *eated;
    if(nptr == NULL){
        newnode->whose_turn = myturn;
        newnode->pre = NULL;
        newnode->next = NULL;
        nptr = newnode;
    }
    else{
        if(nptr->whose_turn == myturn)
            newnode->whose_turn = yourturn;
        else
            newnode->whose_turn = myturn;
        newnode->pre = nptr;
        newnode->next = NULL;
        nptr->next = newnode;
        nptr = newnode;
    }
    return(nptr);
}//紀錄操作至stack

node *regret(chess **c, node *nptr){
    if(nptr == NULL || nptr->pre == NULL){
        printf("There do not have the previous step. Can't not regret.\n");
        return(nptr);
    }
    else{
        for(int i = for_start; i < Num_2; i++){
            node *tmp = nptr;
            int m_o_r = nptr->d_row - Num_1;
            int m_o_c = boardsize - nptr->d_col;
            int m_d_r = nptr->o_row - Num_1;
            int m_d_c = boardsize - nptr->o_col;
            move_chess(&m_o_r, &m_o_c, &m_d_r, &m_d_c, c);
            if(nptr->eated == none){
                nptr = nptr->pre;
                free(tmp);
            }    
            else{
                if(nptr->whose_turn == myturn){
                    (*(*(c+m_o_r)+m_o_c)).whose = yourchess;
                    (*(*(c+m_o_r)+m_o_c)).chess_name = nptr->eated;
                }//恢復P1吃掉的P2的棋子
                else{
                    (*(*(c+m_o_r)+m_o_c)).whose = mychess;
                    (*(*(c+m_o_r)+m_o_c)).chess_name = nptr->eated;
                }//恢復P2吃掉的P1的棋子
                nptr = nptr->pre;
                free(tmp);
            }
        }
        print_board(c);
        return(nptr);
    }
}//悔棋

void save(node *nptr, char *fname){
    if(nptr == NULL)
        printf("Don't have the process of playing chess. Can't not save to the file.\n");
    else{
        int done = false;
        char nametemp[fnamesize];
        FILE *fptr;
        do{
            fptr = fopen(fname, "w");
            if(!fptr){
                printf("Error saved filename. Please enter the right form of filename.(Do not exceed 20 letter) ");
                fname = fgets(nametemp, fnamesize, stdin);
            }
            else{
                while(nptr->pre != NULL)
                    nptr = nptr->pre;//找第一個node
                while(nptr->next != NULL){
                    fprintf(fptr,"%d %d %d %d %d\n", nptr->o_row, nptr->o_col, nptr->d_row, nptr->d_col, nptr->eated);
                    nptr = nptr->next;
                }//印第一個到倒數第二個node
                fprintf(fptr,"%d %d %d %d %d\n", nptr->o_row, nptr->o_col, nptr->d_row, nptr->d_col, nptr->eated);//印最後一個node
                done = true;
            }
        }while(done != true);
        fclose(fptr);
        printf("The process has already save in the file named %s.\n", fname);
    }
}//儲存棋譜

node *load_file(node *nptr, char *fname){
    FILE *fptr = fopen(fname,"r");
    if(!fptr){
        printf("Don't have the chess manual to load.\n");
        return(NULL);
    }
    else{
        int o_r, o_c, d_r, d_c, e;
        while(fscanf(fptr, "%d%d%d%d%d", &o_r, &o_c, &d_r, &d_c, &e) != EOF){
            if(nptr == NULL){
                nptr = (node*)malloc(sizeof(node));
                nptr->next = NULL;
                nptr->pre = NULL;
                nptr->whose_turn = myturn;
                nptr->o_row = o_r;
                nptr->o_col = o_c;
                nptr->d_row = d_r;
                nptr->d_col = d_c;
                nptr->eated = e;
            }
            else{
                node *tmp = (node*)malloc(sizeof(node));
                tmp->next = NULL;
                tmp->pre = nptr;
                nptr->next = tmp;
                if(nptr->whose_turn == myturn)
                    tmp->whose_turn = yourturn;
                else
                    tmp->whose_turn = myturn;
                tmp->o_row = o_r;
                tmp->o_col = o_c;
                tmp->d_row = d_r;
                tmp->d_col = d_c;
                tmp->eated = e;
                nptr = tmp;
            }
        }
        node *tmp = (node*)malloc(sizeof(node));
        tmp->next = NULL;
        tmp->pre = nptr;
        nptr->next = tmp;
        nptr = tmp;//表示結束的node

        while(nptr->pre != NULL)
            nptr = nptr->pre;
        fclose(fptr);
        printf("Loaded successfully\n");
        return(nptr);
    }
}//讀棋譜

node *watch(chess **c, node *nptr, int *end){
    char action;

    printf("Please enter 'f' for the next step, 'b' for the previous step or 'e' for leaving. ");
    scanf(" %c", &action);
    if(action == 'E' || action == 'e'){
        printf("Thank you for using, bye.\n");
        *end = true;
    }
    else if(action == 'F' || action == 'f'){
        if(nptr->next != NULL){
            int m_o_r = nptr->o_row - Num_1;
            int m_o_c = boardsize - nptr->o_col;
            int m_d_r = nptr->d_row - Num_1;
            int m_d_c = boardsize - nptr->d_col;
            move_chess(&m_o_r, &m_o_c, &m_d_r, &m_d_c, c);
            print_board(c);
            nptr = nptr->next;
        }
        else
            printf("There is no next step.\n");
    }
    else if(action == 'B' || action == 'b'){
        if(nptr->pre != NULL){
            nptr = nptr->pre;
            int m_o_r = nptr->d_row - Num_1;
            int m_o_c = boardsize - nptr->d_col;
            int m_d_r = nptr->o_row - Num_1;
            int m_d_c = boardsize - nptr->o_col;
            move_chess(&m_o_r, &m_o_c, &m_d_r, &m_d_c, c);

            if(nptr->eated != none){
                if(nptr->whose_turn == myturn){
                    (*(*(c+m_o_r)+m_o_c)).whose = yourchess;
                    (*(*(c+m_o_r)+m_o_c)).chess_name = nptr->eated;
                }//恢復P1吃掉的P2的棋子
                else{
                    (*(*(c+m_o_r)+m_o_c)).whose = mychess;
                    (*(*(c+m_o_r)+m_o_c)).chess_name = nptr->eated;
                }//恢復P2吃掉的P1的棋子
            }
            print_board(c);
        }
        else
            printf("There is no previous step.\n");
    }
    else
        printf("Please enter the right command!\n");
    return(nptr);
}//讀棋譜後，觀看