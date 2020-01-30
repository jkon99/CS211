#include <stdio.h>
#include <stdlib.h>

/* ONE SHOT LEARNING PROBLEM */
// Santosh said roughly 150 lines

double** multiply(double** m1, double** m2, int r1, int c1, int r2, int c2); //test individual ops in separate programs
double** trans(double** mat, int r1, int c1);
double** inverse(double** mat, int r1);  //no row swaps necessary for gauss
double** weight(double** x, double** y, int tr, int att);   //return matrix
double** priceEq(int t, double** testm, int att, double** weights);
void freematrix(double** m, int r);
//need method for y?
//need method to print output?
// x column 1 should be all 1s and y should be last column of training data (price)
//global variables remember might not work
//use doubles not floats for values
// use training data to predict price of the houses in test file
int main(int argc, char** argv) {   // DONE!!!!

  if (argc < 3) {
    printf("error");
    return 0;
  }
  FILE *train;
  FILE *test;
  train = fopen(argv[1],"r");
  if (train==NULL) {
    printf("error"); //no train data
    return 0;
  }
  test = fopen(argv[2], "r");
  if (test == NULL) {
    printf("error"); //no test data
    return 0;
  }
  int numAtt = 0;
  int numTrains = 0;
    //read file data
  fscanf(train, "%d\n",&numAtt);  //first line is integer with num of attributes K
  fscanf(train, "%d\n", &numTrains);  // this second line is integer N providng number of training examples in set

  double in = 0;
  double** xmat = malloc(numTrains*sizeof(double*));  //(Nx(K+1)) matrix called X
  double** ymat = malloc(numTrains*sizeof(double*)); //(Nx1) matrix called Y
  for(int i=0; i<numTrains; i++) {
    xmat[i] = malloc((numAtt+1)*sizeof(double));
    ymat[i] = malloc(sizeof(double));
    for(int j=1; j<=numAtt;j++) {
      fscanf(train, "%lf,", &in);   //each are values for the attributes of the house
      xmat[i][j] = in;
    }
    fscanf(train,"%lf\n",&in);  //this is price of house
    ymat[i][0] = in;
    xmat[i][0] = 1;
  }
  int tests = 0;
  fscanf(test, "%d\n",&tests);
  double** testData = malloc(tests*sizeof(double*));
  for (int i =0; i<tests;i++) {
    testData[i] = malloc(numAtt*sizeof(double));
    for (int j=0; j<(numAtt-1);j++) {
      fscanf(test, "%lf,",&in);
      testData[i][j] = in;
    }
    fscanf(test,"%lf\n",&in);
    testData[i][numAtt-1] = in;
  }
  //printf("initialize works");
  double** w = malloc((numAtt+1)*sizeof(double*));
  for (int i=0; i<=numAtt; i++) {
    w[i] = malloc(sizeof(double));
  }
  double** fin = malloc(tests*sizeof(double*));
  for (int i =0; i<tests;i++) {
    fin[i] = malloc(sizeof(double));
  }
  w = weight(xmat, ymat, numTrains,numAtt);
  fin = priceEq(tests, testData, numAtt, w);

  freematrix(w, numAtt+1);
  freematrix(testData, tests);
  freematrix(xmat, numTrains);
  freematrix(ymat,numTrains);
  freematrix(fin, tests);
  //printf("free works");
  fclose(train);
  fclose(test);
  //free allocated memory
  return 0;
}

double** multiply(double** m1, double** m2, int r1, int c1, int r2, int c2) {  //done?

  double** product = malloc(r1*sizeof(double*));
  for (int i=0;i<r1;i++) {
    product[i] = malloc(c2*sizeof(double));
  }
  double thing = 0;
  for (int i=0;i<r1;i++) {
    for (int j=0;j<c2;j++) {
      for(int k=0;k<r2;k++) {
        thing = thing + (m1[i][k]*m2[k][j]);
      }
      product[i][j] = thing;
      thing = 0;
    }
    //product[i][j] = thing;
    //thing = 0.0;
  }
  // find out size of matricies
  // malloc
  return product;
}

double** trans(double** mat, int r1, int c1) {  //done?
  // allocate a matrix where cols and mat rows and vice versa
  double** tr = malloc(c1*sizeof(double*));
  for(int i=0;i<c1;i++) {
    tr[i] = malloc(r1*sizeof(double));
  }
  for(int i = 0; i<r1;i++) {
    for(int j = 0; j<c1;j++) {
      tr[j][i] = mat[i][j];  //maybe swap??
    }
  }
  return tr;
}

double** inverse(double** mat, int r1) {   //fix this one?
  //double** inv;   //format mat|inv then use gauss elimination (Guass-Jordan elimination)
  //dimensions of inv should equal mat
  // think of process to print pyramid or right triangle, check step by step in pdf
  // r1 = c1
  double** inv = malloc(r1*sizeof(double*));
  for(int i=0;i<r1;i++) {
    inv[i] = malloc(r1*sizeof(double));
  }

  double** aug = malloc(r1*sizeof(double*));
  for(int i=0; i<r1;i++) {
    aug[i] = malloc((2*r1)*sizeof(double));
  }
  for (int i = 0; i<r1;i++) {  //concatanate input matrix with an identity matrix before steps
    for (int j=0; j<r1;j++) {
      aug[i][j] = mat[i][j];
      if (i==j) {
        aug[i][j+r1] = 1;
      } else {
        aug[i][j+r1] = 0;
      }
    }
  }
  double c = 1;
  for (int i =0; i<r1;i++) {   //subtract rows by another row multiplied by constant
    for (int j =0; j<r1;j++) {
      if (i!=j) {
        c = aug[j][i] / aug[i][i];
        for (int k =0; k<2*r1;k++) {
          aug[j][k] -= c*aug[i][k];
        }
      }
    }
  }
  for (int i =0; i<r1;i++) {   //division operation
    c= aug[i][i];
    for (int j=0;j<2*r1;j++) {
      aug[i][j] = aug[i][j] / c;
    }
  }

  for (int i=0; i<r1;i++) {  //set inv to finished identity side
    for (int j=0; j<r1;j++) {
      inv[i][j] = aug[i][j+r1];
    }
  }
  //everything above good
/*
  if (multiply(old, inv, r1, r1, r1, r1) != mat) {
    printf("NO GOOD");
    return inv;
  } */
  // see if mat equals identity matrix otherwise this is wrong
  freematrix(aug,r1); //free necessary??
  return inv;
}

double** priceEq(int t, double** testm, int att, double** weights) {    // y = w0 + w1x1 + w2x2 + w3x3 + w4x4
  double price = 0;     //done?
  double** out = malloc(t*sizeof(double*));
  for (int i =0; i<t; i++) {
    out[i] = malloc(sizeof(double));
  }
  for(int i = 0; i<t;i++) {
    price = weights[0][0];
    for(int j=0;j<att;j++) {
      price = price + (weights[j+1][0] * testm[i][j]);
    }
    printf("%0.0lf\n", price);  //LIKE in pdf, rounds properly
    out[i][0] = price;
  }

  return out;
}

double** weight(double** x, double** y, int tr, int att) {   //equation w = (X^T*X)^-1 * X^T * Y  where Y = X * W

  double** xt = trans(x, tr, att +1);
  double** xtx = multiply(xt, x, att+1, tr, tr, att+1);
  double** xinv = inverse(xtx, att+1);
  double** xinvxt = multiply(xinv,xt, att+1, att+1, att+1, tr);
  double** w = multiply(xinvxt,y, att+1, tr, tr, 1);

  freematrix(xt, att+1);
  freematrix(xtx, att+1);
  freematrix(xinv, att +1);
  freematrix(xinvxt, att + 1);
  return w;
}

void freematrix(double** m, int r) {
  for (int i =0; i < r; i++) {
    free(m[i]);
  }
  free(m);
  return;
}
