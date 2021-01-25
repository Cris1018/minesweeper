#include "minesweeper.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int get_arr_length(int dim, int * dim_sizes){
    int length = 1;
    for (int i=0; i<dim; i++){
        length*=dim_sizes[i];
    }
    return length;
}

int is_mined(int dim, int * coords, int * mined_cell){
    for (int i=0; i<dim; i++){
        if (*(coords+i) != *(mined_cell+i)){
            return 0;
        }
    }
    return 1;
}

void init_cells(struct cell * game, int dim, int * dim_sizes, int num_mines, int ** mined_cells){
    int game_size = get_arr_length(dim, dim_sizes);
    
    for (int i=0; i<game_size; i++){
        // i is the index
        int offset = i;
        
        for (int m=dim-1; m>=0; m--){
            int product = 1;
            
            for (int n=0; n<m; n++){
                product*=dim_sizes[n];
            }

            int res = offset/product;
            game[i].coords[m] = res;
            offset -= res*product;
        }
        game[i].selected = 0;
        game[i].hint = 0;
    }

    for (int i=0; i<game_size; i++){
        int* coords_cell = game[i].coords;
        
        for (int j=0; j<num_mines; j++){
            if (is_mined(dim, coords_cell, mined_cells[j]) == 1){
                game[i].mined = 1;
                break;
            }  
        }
    }
}

int is_adjacent(int dim, struct cell c1, struct cell c2){
    for (int i=0; i<dim; i++){
        if (abs(c1.coords[i]-c2.coords[i])>1){
            return 0;
        }
    }
    return 1;
}

void init_adjacent(struct cell * game, int dim, int * dim_sizes){
    int game_size = get_arr_length(dim, dim_sizes);

    for (int i=0; i<game_size; i++){
        for (int j=0; j<MAX_ADJACENT; j++){
            game[i].adjacent[j] = (struct cell*)malloc(sizeof(struct cell*));
        }
    }
    
    // find adjacents for cell i
    for (int i=0; i<game_size; i++){

        game[i].num_adjacent = 0;
        for (int j=0; j<game_size; j++){
            if (i!=j && is_adjacent(dim, game[i], game[j]) == 1){
                int idx_adj = game[i].num_adjacent;
                game[i].num_adjacent++;

                game[i].adjacent[idx_adj] = &game[j];


            }
        }
        printf("----------\n");
    }
}

void update_hint(struct cell* game, int dim, int*dim_sizes){
    int game_size = get_arr_length(dim, dim_sizes);

    for (int i=0; i<game_size; i++){
        printf("----------\ncell:{%d %d %d} mined: %d\n", game[i].coords[0], game[i].coords[1], 
                game[i].coords[2], game[i].mined);
        
        struct cell this_cell = game[i];
        
        for (int j=0; j<this_cell.num_adjacent; j++){
            printf("{%d %d %d}\n", game[i].adjacent[j]->coords[0], game[i].adjacent[j]->coords[1],
                    game[i].adjacent[j]->coords[2]);
            
            if (this_cell.adjacent[j]->mined == 1){
                game[i].hint++;
            }
        }
        printf("hint: %d \n----------\n", game[i].hint);
    }
}

int check_all_selected(struct cell * game, int dim, int*dim_sizes){
    int game_size = get_arr_length(dim, dim_sizes);

    for (int i=0; i<game_size; i++){
        if (game[i].mined == 0 && game[i].selected != 1){
            return 0;
        }
    }
    return 1;
}

void init_game(struct cell * game, int dim, int * dim_sizes, int num_mines, int ** mined_cells){
    init_cells(game, dim, dim_sizes, num_mines, mined_cells);
    init_adjacent(game, dim, dim_sizes);
    update_hint(game, dim, dim_sizes);
}

void recursively(struct cell * game, int dim, int * dim_sizes, int * coords){
    int game_size = get_arr_length(dim, dim_sizes);
    struct cell the_cell;

    // find the selected cell
    for (int i=0; i<game_size; i++){
        int* current = game[i].coords;
        int flag = 1;

        for (int j=0; j<dim; j++){
            if (current[j]!=coords[j]){
                flag = 0;
                break;
            }
        }

        if (flag == 1){
            the_cell = game[i];
            break;
        }
    }

    if (the_cell.hint>0){
        printf("{%d %d %d}, hint is %d\n", the_cell.coords[0], the_cell.coords[1], the_cell.coords[2], the_cell.hint);
        return;
    }

    int flag = 0;
    for (int j=0; j<the_cell.num_adjacent; j++){
        if (the_cell.adjacent[j]->selected == 0){
            flag = 1;
        }
    }
    if (flag == 0){
        printf("{%d %d %d}'s adjacents are all in!\n", the_cell.coords[0], the_cell.coords[1], the_cell.coords[2]);
        return;
    }

    for (int j=0; j<the_cell.num_adjacent; j++){
        if (the_cell.adjacent[j]->selected == 0){
            the_cell.adjacent[j]->selected = 1;
            // printf("hint is %d\n", the_cell.adjacent[j]->hint);
            recursively(game, dim, dim_sizes, the_cell.adjacent[j]->coords);
        }
    }
}

int select_cell(struct cell * game, int dim, int * dim_sizes, int * coords){
    int game_size = get_arr_length(dim, dim_sizes);

    for (int i=0; i<game_size; i++){

        int* current = game[i].coords;
        int flag = 1;

        for (int j=0; j<dim; j++){
            if (current[j]!=coords[j]){
                flag = 0;
                break;
            }
        }

        if (flag == 1){
            game[i].selected = 1;
            
            if (game[i].mined == 1){
                // game over!!!!!!!
                return 1;
            } else {
                // not select a mine
                int the_hint = game[i].hint;
                if (the_hint != 0){
                    
                    printf("hint is %d\n", the_hint);
                    
                    if (check_all_selected(game, dim, dim_sizes) == 1){
                        printf("you win!!!!!!");
                        return 2;
                    } else {
                        return 0;
                    }
                } 
                recursively(game, dim, dim_sizes, coords);
                return 0;
            }
        } else {
            // continue the loop, do nothing
        }
    }
    // select an invalid cell
    return 0;
}

int main(){
    int dims[3] = {3, 6, 4};
    int mined_cells[4][3] = {{2, 0, 1}, {1, 1, 1}, {2, 2, 1}, {0, 0, 1}};
    int number_mines = 4;
    int ** ptr1 = malloc(sizeof(int)*number_mines*MAX_DIM);

    for (int i=0; i<4; i++){
        ptr1[i] = &mined_cells[i][0];
    }

    struct cell*first = malloc(sizeof(void*));
    init_game(first, 3, &dims[0], 4, ptr1);

    int selected1[3] = {2, 2, 2};
    select_cell(first, 3, &dims[0], &selected1[0]);

    // int selected2[3] = {1, 5, 3};
    // select_cell(first, 3, &dims[0], &selected2[0]);
    // printf("\n");
    int selected3[3] = {0, 4, 3};
    select_cell(first, 3, &dims[0], &selected3[0]);

    int selected4[3] = {1, 1, 2};
    select_cell(first, 3, &dims[0], &selected4[0]);

    int selected2[3] = {1, 5, 3};
    select_cell(first, 3, &dims[0], &selected2[0]);

    return 0;
}