// HW2 shogi
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "libev/ev.h"

/*      timer  begin      */
int attacker = -1;
int indep=0;
int turns=0;
int past_time=0;
int now;
int c=0;
ev_io stdin_watcher;
ev_timer timeout_watcherx;
ev_timer timeout_watchery;

/*        constant        */
#define BWEIGHT 9
#define BHEIGHT 9
#define NAMESIZE 4
#define STRLENGTH 5
#define STACKSIZE 100
#define MOVESIZE 4

static void stdin_cb (EV_P_ ev_io *w, int revents){
    ev_timer_stop (loop, &timeout_watcherx);	
    ev_timer_stop (loop, &timeout_watchery);
	if(attacker==1){	
		ev_timer_stop (loop, &timeout_watcherx);	
		ev_timer_start (loop, &timeout_watchery);
	}
	else{
		ev_timer_stop (loop, &timeout_watchery);
		ev_timer_start (loop, &timeout_watcherx);
	}
}
static void timeout_cbx (EV_P_ ev_timer *w, int revents){

    if(turns==0){
        printf("The time of player BLUE use: ");
        printf("\033[43m %d second \033[m\n",(int)ev_now(loop)-now);
        indep=(int)ev_now(loop)-now;
    }else{
        printf("The time of player BLUE use: ");printf("\033[43m %d second \033[m\n",(int)ev_now(loop)-now-indep);
        indep=(int)ev_now(loop)-now;
    }
    ev_timer_stop (loop, &timeout_watcherx);
    ev_break(loop, EVBREAK_ONE);
    turns=1;

}
static void timeout_cby (EV_P_ ev_timer *w, int revents){

    printf("The time of player RED use: ");
    printf("\033[43m %d second \033[m\n",(int)ev_now(loop)-now-indep);
    indep=(int)ev_now(loop)-now;
    ev_timer_stop (loop, &timeout_watchery);
    ev_break(loop, EVBREAK_ONE);
    turns=-1;

}

/*.....................................Data Structure...................................*/
struct piece {
    char name[NAMESIZE];
    char controller[STRLENGTH];
};

struct stack{
    int top;
    struct piece one_move_chess[STACKSIZE][BWEIGHT][BHEIGHT];
};

struct MOVE_STACK{
    int top;
    int move_top;
    int move_stack[STACKSIZE][MOVESIZE] ;
};
/*.........................................function......................................*/

void board_initial(struct piece board[][BHEIGHT])
{
    for (int i = 0; i < BWEIGHT; i++)
    {
        for (int j = 0; j < BHEIGHT; j++)
        {
            if (j == 2 || j == 6)
                strcpy((*(board + i) + j)->name, "步");
            else if ((i == 1 && j == 7) || (i == 7 && j == 1))
                strcpy((*(board + i) + j)->name, "角");
            else if ((i == 1 && j == 1) || (i == 7 && j == 7))
                strcpy((*(board + i) + j)->name, "飛");
            else if ((i == 0 && j == 0) || (i == 8 && j == 0) || (i == 0 && j == 8) || (i == 8 && j == 8))
                strcpy((*(board + i) + j)->name, "香");
            else if ((i == 1 && j == 0) || (i == 7 && j == 0) || (i == 1 && j == 8) || (i == 7 && j == 8))
                strcpy((*(board + i) + j)->name, "桂");
            else if ((i == 2 && j == 0) || (i == 6 && j == 0) || (i == 2 && j == 8) || (i == 6 && j == 8))
                strcpy((*(board + i) + j)->name, "銀");
            else if ((i == 3 && j == 0) || (i == 5 && j == 0) || (i == 3 && j == 8) || (i == 5 && j == 8))
                strcpy((*(board + i) + j)->name, "金");
            else if ((i == 4 && j == 0) || (i == 4 && j == 8))
                strcpy((*(board + i) + j)->name, "王");
            else
                strcpy((*(board + i) + j)->name, "  ");
        }
    }

    // controller
    for (int i = 0; i < BWEIGHT; i++)
    {
        for (int j = 0; j < BHEIGHT; j++)
        {
            if (j == 0 || (i == 1 && j == 1) || (i == 7 && j == 1) || j == 2)
                strcpy((*(board + i) + j)->controller, "Red");
            else if (j == 6 || (i == 1 && j == 7) || (i == 7 && j == 7) || j == 8)
                strcpy((*(board + i) + j)->controller, "Blue");
            else
                strcpy((*(board + i) + j)->controller, "Free");
        }
    }
}

void board_show(struct piece (*board)[BHEIGHT])
{   
    printf("9 8 7 6 5 4 3 2 1\n");
    for (int j = 0; j < BHEIGHT; j++)
    {
        for (int i = 0; i <= BWEIGHT; i++)
        {
            if (i == BWEIGHT)
                printf("%d\n", j + 1);
            else
            {
                if (strcmp((*(board + i) + j)->controller, "Red") == 0)
                    printf("\033[31m%s\033[m", (*(board + i) + j)->name);
                else if (strcmp((*(board + i) + j)->controller, "Blue") == 0)
                    printf("\033[34m%s\033[m", (*(board + i) + j)->name);
                else
                    printf("%s", (*(board + i) + j)->name);
            }
        }
    }
}

void swap(struct piece *A,struct piece *B)
{
    struct piece temp = *A;
    *A = *B;
    *B = temp;
}

int chess_bu (struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( after_x==before_x && before_y-after_y==1 && strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
        return 1;
    else if( after_x==before_x && after_y-before_y==1 && strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0                   )
        return 1;
    else        
        return 0;
}

int chess_Xiang(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    int chess_stop=0;
    if( after_x==before_x && before_y-after_y>=1 
        && strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
    {   
        for(int i = before_y -1 ; i > after_y ; i-- )
        {
            if(strcmp(chess[after_x][i].name,"  ")==0)
                chess_stop=0;//沒被擋住
            else
            {
                    chess_stop=1;//被擋住了
                    break;
            }
        }
        
        if(chess_stop==0)
            return 1; //移動成功
        else
            return 0; //無效移動
        
    }else 
    if( after_x==before_x && after_y-before_y>=1 && strcmp(chess[before_x][before_y].controller,"Red")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0 )
    {
        for(int i = before_y + 1 ; i < after_y ; i++ ){
            if(strcmp(chess[after_x][i].name,"  ")==0)
                chess_stop=0;//沒被擋住
            else{
                chess_stop=1;//被擋住了
                break;
            }
        }
        
        if(chess_stop==0)
            return 1; //移動成功
        else
            return 0; //無效移動

    }else
        return 0;
}

int chess_Gui (struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( abs(after_x-before_x)==1 && before_y-after_y==2 && strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
        return 1;
    else if( abs(after_x-before_x)==1 && after_y-before_y==2 && strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0                   )
        return 1;
    else        
        return 0;
}

int chess_Yin(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
    {
        if( abs(after_x-before_x)==1 && abs(after_y-before_y)==1  ) 
            return 1;
        else if ( before_x==after_x && before_y-after_y==1  )
            return 1;
        else
            return 0;

    }else if( strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0 )
    {
        if( abs(after_x-before_x)==1 && abs(after_y-before_y)==1  ) 
            return 1;
        else if ( before_x==after_x && before_y-after_y==-1  )
            return 1;
        else
            return 0;
    }else        
        return 0;
}

int chess_Gin(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( ( abs(after_x-before_x)==1 || abs(after_y-before_y)==1 )&&
        abs(after_x-before_x)<=1 && abs(after_y-before_y)<=1 &&
        strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0         )
    {
        if( after_x-before_x ==1 && after_y-before_y == 1  ) 
            return 0;
        else if ( after_x-before_x == -1 && after_y-before_y == 1 )
            return 0;
        else 
            return 1;
            
    }else if( ( abs(after_x-before_x)==1 || abs(after_y-before_y)==1 )&&
              abs(after_x-before_x)<=1 && abs(after_y-before_y)<=1 &&
              strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0         )
    {
        if( after_x-before_x ==1 && after_y-before_y == -1  ) 
            return 0;
        else if ( after_x-before_x == -1 && after_y-before_y == -1 )
            return 0;
        else 
            return 1;
    }else        
        return 0;
}

int chess_Wang(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( ( abs(after_x-before_x)==1 || abs(after_y-before_y)==1 )&&
        abs(after_x-before_x)<=1 && abs(after_y-before_y)<=1 &&
        strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
            return 1;
    else if( ( abs(after_x-before_x)==1 || abs(after_y-before_y)==1 )&&
              abs(after_x-before_x)<=1 && abs(after_y-before_y)<=1 &&
              strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0        )
            return 1;
    else        
            return 0;
}

int chess_Jiao(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    int chess_stop=0;
    if( abs(after_x-before_x) == abs(before_y-after_y) 
        && strcmp(chess[after_x][after_y].name,"  ")==0   )
    {   
        if( after_x-before_x<0 && after_y-before_y<0 )
        {
            for(int i=before_x-1,  j=before_y-1 ; after_x < i ; i-- , j-- )
            {        
                if(strcmp(chess[i][j].name,"  ")==0)
                    chess_stop=0;
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else if( after_x-before_x>0 && after_y-before_y<0 )
        {
            for(int i=before_x+1, j=before_y-1 ; after_x > i ;  i++, j-- )
            {   
                if(strcmp(chess[i][j].name,"  ")==0)
                    chess_stop=0;
                else{   
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else if( after_x-before_x<0 && after_y-before_y>0 )
        {
            for(int i=before_x-1, j=before_y+1 ; after_x < i ; i--,j++ )
            {
                if(strcmp(chess[i][j].name,"  ")==0)
                    chess_stop=0;
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else //if( after_x-before_x>0 && after_y-before_y>0 )
        {
            for(int i=before_x+1, j=before_y+1 ; after_x > i  ; i++,j++ )
            {
                if(strcmp(chess[i][j].name,"  ")==0)
                    chess_stop=0;
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }
        if(chess_stop==0)
            return 1; //移動成功
        else
            return 0; //無效移動    
    }
    else 
        return 0;
}

int chess_Fei(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{   
    int chess_stop=0;
    if( (after_x==before_x || before_y==after_y)
        && strcmp(chess[after_x][after_y].name,"  ")==0 )
    {   
        if( after_y-before_y < 0 )
        {
            for(int i = before_y - 1 ; i > after_y ; i-- )
            {
                if(strcmp(chess[before_x][i].name,"  ")==0)
                    chess_stop=0;//沒被擋住
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else if( after_y-before_y > 0 )
        {
            for(int i = before_y + 1 ; i < after_y ; i++ )
            {
                if(strcmp(chess[before_x][i].name,"  ")==0)
                    chess_stop=0;//沒被擋住
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else if( after_x-before_x < 0 )
        {
            for(int i = before_x - 1 ; i > after_x ; i-- )
            {
                if(strcmp(chess[i][before_y].name,"  ")==0)
                    chess_stop=0;//沒被擋住
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else //if( after_x-before_x > 0 )
        {
            for(int i = before_x + 1 ; i < after_x ; i++ )
            {
                if(strcmp(chess[i][before_y].name,"  ")==0)
                    chess_stop=0;//沒被擋住
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }
        
        if(chess_stop==0)
            return 1; //移動成功
        else
            return 0; //無效移動
    }else       
        return 0;
}

int move_chess_type(struct piece chess[][BHEIGHT] ,int before_x ,int before_y){

    if( strcmp(chess[before_x][before_y].name,"步")==0 )
        return 1;
    else if( strcmp(chess[before_x][before_y].name,"香")==0 )
        return 2;
    else if( strcmp(chess[before_x][before_y].name,"桂")==0 )
        return 3;
    else if( strcmp(chess[before_x][before_y].name,"銀")==0 )
        return 4;
    else if( strcmp(chess[before_x][before_y].name,"金")==0 )
        return 5;
    else if( strcmp(chess[before_x][before_y].name,"王")==0 )
        return 6;
    else if( strcmp(chess[before_x][before_y].name,"角")==0 )
        return 7;
    else if( strcmp(chess[before_x][before_y].name,"飛")==0 )
        return 8;
    else 
        return 0;
}

int eat(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)//移動位置為敵方位置
{
                struct piece temp;   
                strcpy(temp.controller , chess[after_x][after_y].controller );
                strcpy(temp.name       , chess[after_x][after_y].name );

                strcpy(chess[after_x][after_y].controller,"Free" );
                strcpy(chess[after_x][after_y].name      , "  "  );
                int condution=0;

                switch (  move_chess_type( chess , before_x , before_y)  ) 
                {

                    case 1:  if( chess_bu (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 2:  if( chess_Xiang (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 3:  if( chess_Gui (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 4:  if( chess_Yin (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 5:  if( chess_Gin (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;

                    case 6:  if( chess_Wang (chess , before_x , before_y, after_x, after_y) )
                             {     
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;

                    case 7:  if( chess_Jiao (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 8:  if( chess_Fei (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0; 
                             break;
                }    

                if(condution==1)
                    return 1;
                else{
                    strcpy(chess[after_x][after_y].controller , temp.controller );
                    strcpy(chess[after_x][after_y].name       , temp.name );     
                    return 0;               
                }

}

int move_chess(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)//移動位置不為敵方位置
{
    switch (  move_chess_type( chess , before_x , before_y)  ) 
    {
                    case 1:  if( chess_bu (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                    case 2:  if( chess_Xiang (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                    case 3:  if( chess_Gui (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;

                    case 4:  if( chess_Yin (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                             
                    case 5:  if( chess_Gin (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                             
                    case 6:  if( chess_Wang (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                             
                    case 7:  if( chess_Jiao (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    //printf("move_chess function call case 7 \n");
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                             
                    case 8:  if( chess_Fei (chess , before_x , before_y, after_x, after_y) )
                             {         
                                    //printf("move_chess function call case 8 \n");
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;               
    }   
}

int legal_position(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y, int attacker)
{
    if( after_x>=0 && after_x<=8 && after_y>=0 && after_y<=8 )
    {   
        if( strcmp(chess[before_x][before_y].controller,"Blue")==0 && attacker==-1 )//if attacker=Blue
        {   
            if( strcmp(chess[after_x][after_y].controller,"Red")==0) //移動後位置為敵方所在位置
            {   
                if( eat(chess, before_x , before_y,  after_x,  after_y) )
                    return 1;
                else
                    return 0;
            }else { //移動後位置不為敵方所在位置
                if(move_chess(chess, before_x , before_y,  after_x,  after_y))
                    return 1;
                else
                    return 0;
            }
        }else if( strcmp(chess[before_x][before_y].controller,"Red")==0 && attacker==1 )//if attacker=Red
        {   
            if( strcmp(chess[after_x][after_y].controller,"Blue")==0) //移動後位置為敵方所在位置
            {   
                if( eat(chess, before_x , before_y,  after_x,  after_y) )
                    return 1;
                else
                    return 0;
            }else { //移動後位置不為敵方所在位置
                if(move_chess(chess, before_x , before_y,  after_x,  after_y))
                    return 1;
                else
                    return 0;
            }
        }else
            return 0;
    }else 
        return 0;
}

void transfer(int *before_x, int *before_y,int *after_x,int *after_y)
{
    *before_x= 9-(*before_x);
    *after_x = 9-(*after_x ); 
    *before_y= (*before_y)-1;
    *after_y = (*after_y)-1 ;
}

int Overflow(struct stack *all_chess_stack_Ptr)
{
    if(all_chess_stack_Ptr->top==STACKSIZE-1)
        return 1;
    else
        return 0;
}

int Empty(struct stack *all_chess_stack_Ptr){

    if(all_chess_stack_Ptr-> top == 0)
        return 1;
    else
        return 0;
}

int Push(struct stack *all_chess_stack_Ptr,struct piece chess[][BHEIGHT])
{
    if(!Overflow(all_chess_stack_Ptr)){

        (all_chess_stack_Ptr->top)++;

        for(int i=0 ; i<9 ; i++)
        {
            for(int j=0 ; j<9 ;j++)
            {   
                strcpy((all_chess_stack_Ptr->one_move_chess[all_chess_stack_Ptr->top][i][j]).name, chess[i][j].name);
                strcpy((all_chess_stack_Ptr->one_move_chess[all_chess_stack_Ptr->top][i][j]).controller, chess[i][j].controller);
            }
        }
        return 1;
    }else {
        printf("Stack is Overflow\n");
        return 0;
    }
}

int Pop(struct stack *all_chess_stack_Ptr,struct piece chess[][BHEIGHT])
{   
    if(Empty(all_chess_stack_Ptr)){
        printf("Chess Stack is Empty \n");
        return 0;
    } 
    else{
        (all_chess_stack_Ptr->top)--;

        for(int i=0 ; i<9 ; i++){
            for(int j=0 ; j<9 ;j++){   
                strcpy( chess[i][j].name, (all_chess_stack_Ptr->one_move_chess[all_chess_stack_Ptr->top][i][j]).name);
                strcpy( chess[i][j].controller, (all_chess_stack_Ptr->one_move_chess[all_chess_stack_Ptr->top][i][j]).controller);
            }
        }
        return 1;
    }
}

int Push_move(struct MOVE_STACK *chess_move_Ptr, int before_x, int before_y, int after_x, int after_y)
{
        (chess_move_Ptr -> top)++;
        chess_move_Ptr->move_stack[chess_move_Ptr->top][0] = before_x;
        chess_move_Ptr->move_stack[chess_move_Ptr->top][1] = before_y;
        chess_move_Ptr->move_stack[chess_move_Ptr->top][2] = after_x;
        chess_move_Ptr->move_stack[chess_move_Ptr->top][3] = after_y;
        //printf("Chess_MOVE is Pushed on stack \n");
        return 1;
}

int Pop_move(struct MOVE_STACK *chess_move_Ptr)
{
        (chess_move_Ptr->top)--;
        return 1;
}

void Write(FILE *fptr , struct MOVE_STACK * chess_move_Ptr )
{
    fprintf(fptr, "%d", chess_move_Ptr->top);
    fprintf(fptr,"\n");

    for(int i = 0 ; i <= chess_move_Ptr->top ; i++ )
    {
        for(int k = 0 ; k < MOVESIZE ; k++)
        {   
            fprintf(fptr, "%d", chess_move_Ptr->move_stack[i][k]);
            fprintf(fptr," ");
        }
        fprintf(fptr,"\n");
    }
    printf("File is Ready \n");
}

int get_move(struct MOVE_STACK * read_move_Ptr,int *read_x_b, int *read_y_b,int *read_x_a,int *read_y_a)
{
        read_move_Ptr->move_top++;
        *read_x_b=read_move_Ptr->move_stack[read_move_Ptr->move_top][0];
        *read_y_b=read_move_Ptr->move_stack[read_move_Ptr->move_top][1];
        *read_x_a=read_move_Ptr->move_stack[read_move_Ptr->move_top][2];
        *read_y_a=read_move_Ptr->move_stack[read_move_Ptr->move_top][3];
        return 1;   
}

int return_move(struct MOVE_STACK * read_move_Ptr){

    if(  (read_move_Ptr->move_top) == -1 )
    {
        printf("This is the first step \n");
        return 0;
    }else {
        read_move_Ptr->move_top--;
        return 1;
    }
}

int main(int argc, char *argv[])
{
    struct ev_loop *loop = EV_DEFAULT;
    now=ev_now(loop);

    struct piece board[BWEIGHT][BHEIGHT];    
    struct stack all_chess_stack , *all_chess_stack_Ptr ;
    all_chess_stack_Ptr = &all_chess_stack;
    all_chess_stack_Ptr->top = -1;

    struct MOVE_STACK chess_move ,*chess_move_Ptr ;
    chess_move_Ptr = &chess_move;
    chess_move_Ptr->top = -1;

    board_initial(board);
    Push(all_chess_stack_Ptr,board);
    board_show(board);
    
    int before_x ,before_y ,after_x ,after_y;    
    int temp_attacker=1;
    int temp;

    ev_io_init(&stdin_watcher, stdin_cb, /*STDIN_FILENO*/ 0, EV_READ);
	ev_io_start(loop, &stdin_watcher);
	ev_timer_init(&timeout_watcherx, timeout_cbx, 0, 1);
    ev_timer_init(&timeout_watchery, timeout_cby, 0, 1);

    if(argc == 4 && strcmp(argv[1],"-n")==0 && strcmp(argv[2],"-s")==0)
    {
        while(1)
        {
            int recover=1;
            do
            {
                printf("Recover step input [0] | continue move input [1]  | Save the chess [2] = ");
                scanf(" %d", &recover);

                if( recover==1 || recover==2 )
                    break;
                if(recover==0){
                    Pop(all_chess_stack_Ptr,board);
                    Pop_move(chess_move_Ptr);
                    board_show(board); 
                    temp=attacker;
                    attacker=temp_attacker;
                    temp_attacker=temp;
                }
            } while ( recover==0 | recover!=1 | recover!=2 );

            if(recover==2){
                FILE *fptr;
                fptr = fopen("The_MOVE.txt","w");
                printf("The_MOVE.txt is open");
                Write(fptr ,chess_move_Ptr );
                fclose(fptr);
                break;
            }
        
            printf("\n*************** 目前進攻方為 %d (-1為Blue,1為Red) ***************\n ", attacker);
            printf("請輸入移動棋子之[段] = ");       scanf(" %d", &before_x);
            printf(" 請輸入移動棋子之[筋] = ");      scanf(" %d", &before_y);
            printf(" 請輸入欲移動位置之[段] = ");    scanf(" %d", &after_x);
            printf(" 請輸入欲移動位置之[筋] = ");    scanf(" %d", &after_y);

            ////////////////////////////////////timer////////////////////////////////////////////////////////
            if(attacker==-1)
                ev_timer_start(loop, &timeout_watcherx);
            else
                ev_timer_start(loop, &timeout_watchery);
            ev_run(loop, 0);
            ////////////////////////////////////timer/////////////////////////////////////////////////////////

            transfer(&before_x,&before_y,&after_x,&after_y);
            if(  legal_position(board, before_x , before_y,  after_x,  after_y,  attacker)  ){

                printf("\n Correct Move (^-^) !!!\n");
                Push(all_chess_stack_Ptr,board);
                Push_move(chess_move_Ptr,9-before_x,before_y+1,9-after_x,after_y+1);

            }else{
                temp=attacker;
                attacker=temp_attacker;
                temp_attacker=temp;
                printf(" Incorrect Move (T^T) !!!\n");
            }
            board_show(board);         
            temp=attacker;
            attacker=temp_attacker;
            temp_attacker=temp;
        }    
        ev_timer_stop (loop, &timeout_watcherx);
        ev_timer_stop (loop, &timeout_watchery);
    }  
    /****************************end NEW_GAME ***************************/
    if(argc == 3 && strcmp(argv[1],"-l")==0 )
    {
        printf("*********Read File*********\n");
        struct MOVE_STACK read_move ,*read_move_Ptr;
        read_move_Ptr = &read_move;
        read_move_Ptr->move_top = -1;
        
        FILE *fp;
        fp = fopen("The_MOVE.txt", "r");
        
        fscanf(fp, "%d", &read_move_Ptr->top );
        for(int i = 0 ; i <= read_move_Ptr->top ; i++ ){   
            for(int k = 0 ; k < MOVESIZE ; k++){  
                fscanf(fp, "%d", &read_move_Ptr->move_stack[i][k]);
            }
        }
        fclose(fp);

        char control;
        while(1)
        {
            printf("*********************************************************\n");
            printf("輸入[f]移動下一手 | 輸入[b]退回上一手 | 輸入[e]結束程式 = ");
            scanf(" %c", &control);

            if(control=='e')
                break;
            if( control=='f' && all_chess_stack_Ptr->top <= read_move_Ptr->top   ){ // 輸入 ‘f’ 移動下一手，更新棋盤狀態

                get_move( read_move_Ptr,&before_x,&before_y,&after_x,&after_y);
                transfer(&before_x,&before_y,&after_x,&after_y);

                legal_position(board, before_x , before_y,  after_x,  after_y,  attacker);
                Push(all_chess_stack_Ptr,board);
                board_show(board);         

                temp=attacker;
                attacker=temp_attacker;
                temp_attacker=temp;

            }else if( control=='b' && all_chess_stack_Ptr->top >= 0 ){ // 輸入 ‘b’ 退回上一手，更新棋盤狀態

                Pop(all_chess_stack_Ptr,board);
                return_move(read_move_Ptr);

                board_show(board); 
                temp=attacker;
                attacker=temp_attacker;
                temp_attacker=temp;

            }else 
                board_show(board);
        } //end while(1)
    } /************************end old_GAME **********************/
    return 0;
}