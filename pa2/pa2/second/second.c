#include <stdio.h>
#include <stdlib.h>

//EXTRA CREDIT IMPLEMENT BACKTRACKING algorithm

void print(int mat[16][16]);
int complete(int mat[16][16]);  // int is 1 or 0 (true or false)
//int[16][16] backtrack(int mat[16][16]); maybe?

//understand sudoku then do hexadoku
int main(int argc, char** argv) {

  int hexgrid[16][16]; //no malloc necessary, not global
  //use Linked Lists to store possible numbers?
  //check sudoku constraits to see if solvable
  // n^2 linear search. check row then column. then check subgrid
  if (argc < 2) {
    printf("no-solution");
    return 0;
  }
  FILE *fp;
  fp = fopen(argv[1],"r");
  if (fp==NULL) {
    printf("no-solution");
    return 0;
  }
  //hexgrid = allocateGrid(fp);
  int t;
  int old =17;  //set to value that shouldn't exist
  //char sym;
  for (int i=0;i<16;i++) {
    for (int j=0; j<16;j++) {
      old = t;
      if (j!=15) {
        fscanf(fp,"%X\t",&t);
      } else {
        fscanf(fp,"%X",&t);
      }
      if (old==t) {
        hexgrid[i][j] = 16;
      } else {
        hexgrid[i][j] = t;
      }
    }
    fscanf(fp,"\n");
  }
  //print(hexgrid); test if original works
  int ret = complete(hexgrid);
  if (ret == 0) {
    printf("no-solution");  // is \n necessary?? nope!
    return 0;
  }
  print(hexgrid);
  fclose(fp);
  return 0;
  // check each "-"
  // print out complete grid filled with all values with new lines for rows and tabs between values
  // print no-solution if not solvable. hexadoku that have solution without non-deterministic guess are unsolvable
}

void print(int mat[16][16]) {
  for (int i=0; i<16;i++) {
    for (int j=0; j<16;j++) { //check for hex values A-F which are 10-15, otherwise just read integer
      if (mat[i][j] == 10) {
        printf("A");
      } else if (mat[i][j] == 11) {
        printf("B");
      } else if (mat[i][j] == 12) {
        printf("C");
      } else if (mat[i][j] == 13) {
        printf("D");
      } else if (mat[i][j] == 14) {
        printf("E");
      } else if (mat[i][j] == 15) {
        printf("F");
      } else if (mat[i][j] == 16) {
        printf("-");
      }else {
        printf("%d", mat[i][j]);
      }
      if (j!=15) {
        printf("\t");
      }
    }
    if (i!=15) {
      printf("\n");
    }
  }
  return;
}

int complete(int mat[16][16]) {
  int bool = 0;  //1 is true, 0 is false
  for (int i =0;i<16;i++) {  //row
    for (int j=0;j<16;j++) { //col
      if (mat[i][j] == 16) {
        for (int k=0;k<16;k++) { //hex val (0-15)
          bool = 1;
          int rs = (i/4)* 4;
          int cs = (j/4)* 4; //redundant? nope!
          for (int l=0; l<16;l++) {
            if (mat[i][l] == k) {  //check rows
              bool = 0;
            }
            if (mat[l][j] == k) { //check columns
              bool = 0;
            }
            if (mat[rs+(l%4)][cs+(l%4)] == k) {  //make and check subgrids
              bool = 0;
            }
          }
          if (bool == 1) { //that value exsts
            mat[i][j] = k;
            if (complete(mat)==1) {  //recursion, check if new matrix is true
              return 1;
            }
            mat[j][i] = 16;
            return 0;
          }
        }
        return 0;
      //return 0;
      }
    }
      //get error othersie
  }
  return 1;
}
