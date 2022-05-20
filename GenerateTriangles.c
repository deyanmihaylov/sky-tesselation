#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
struct pos {
    double x;
    double y;
    double z;
};

// For our purposes, this file will only work effectively if all triangles are equilateral
struct triangle {
  struct pos A;
  struct pos B;
  struct pos C;
  struct pos center;
};

// Adds vectors A and B
struct pos add_vector (struct pos A, struct pos B) {
  struct pos C;
  C.x = A.x + B.x;
  C.y = A.y + B.y;
  C.z = A.z + B.z;
  return C;
}

// Scales a vector by x
struct pos scale_vector (struct pos A, double x) {
  struct pos C;
  C.x = A.x * x;
  C.y = A.y * x;
  C.z = A.z * x;
  return C;
}

// Scales a vector to a size of 1
struct pos scale_norm (struct pos A) {
  double norm = A.x * A.x + A.y * A.y + A.z * A.z;
  norm = sqrt(norm);
  return scale_vector (A, 1/norm);
}

// Given a triangle (not in the form of a struct), locates its center
struct pos tri_center (struct pos A, struct pos B, struct pos C) {
  struct pos center;
  struct pos AB = add_vector(A, B);
  AB = scale_vector(AB, 0.5);
  struct pos NAB = scale_vector(AB, -1);
  struct pos ABC = add_vector(C, NAB);
  ABC = scale_vector(ABC, 0.3333);
  center = add_vector(AB, ABC);
  return center;
};

// Consumes a triangle, then projects it into the sphere and curves it
struct triangle curve_triangle (struct pos A, struct pos B, struct pos C) {
  struct triangle sub_tri;
  sub_tri.A = scale_norm(A);
  sub_tri.B = scale_norm(B);
  sub_tri.C = scale_norm(C);
  sub_tri.center = tri_center(sub_tri.A, sub_tri.B, sub_tri.C);
  sub_tri.center = scale_norm(sub_tri.center);
  return sub_tri;
}

// Given line AB, splits it into m midpoints (including the ends)
// Example: list_midpoints ((0,0,0), (3,3,0), 4) returns { (0,0,0), (1,1,0), (2,2,0), (3,3,0) }
struct pos * list_midpoints (struct pos A, struct pos B, int m) {
  int mm = m - 1;
  struct pos *M = malloc(m * (sizeof (struct pos)));
  double ratio;
  struct pos A_to_B;
    A_to_B.x = B.x - A.x;
    A_to_B.y = B.y - A.y;
    A_to_B.z = B.z - A.z;
  for (int i = 0; i <= mm; i++) {
    ratio = (double) i/mm;
    M[i] = add_vector(A, scale_vector(A_to_B, ratio));
  }
  return M;
};

// Given 2 parallel lines split into b and b + 1 midpoints, produces a list of b triangles that can be made
struct triangle * row_of_triangles (struct pos *A, struct pos *B, int b) {
  struct triangle * S = malloc(b * (sizeof (struct triangle)));
  for (int i = 0; i < b; i++) {
    S[i] = curve_triangle(A[i], A[i+1], B[i]);
  }
  return S;
}

// Given 2 parallel lines split into b and b + 1 midpoints, produces a list of b - 1 upside-down triangles that can be made
struct triangle * upside_down_trianges (struct pos *A, struct pos *B, int b) {
  struct triangle * S = malloc(b * (sizeof (struct triangle)));
  for (int i = 0; i < b - 1; i++) {
    S[i] = curve_triangle(B[i], B[i+1], A[i+1]);
  }
}
/* Given an equilateral triangle that lies on the sphere, and a positive integer n >= 2 , splits the triangle by an n-partition to create a 
list of n*n congruent sub-triangles and projects them into the sphere.
n is defined as the number of segments the sides will be split into. For example, to split a single triangle into 4, we would let n = 2.
The function does not return ALL the triangles in the partition, only the smallest ones. For example, when n = 3, the function would 
only return the 9 smallest sub-triangles instead of 13 of them. (You may be familiar with the old math puzzle where you 
are presented with a diagram and asked to FiNd AlL tHe TrIaNgLeS - This function DOES NOT solve such questions)
*/
struct triangle* list_of_triangles (struct pos A, struct pos B, struct pos C, int n) {
  int triangle_counter = 0;
  struct triangle *ALL = malloc(n * n * (sizeof (struct triangle)));
  struct triangle *R1 = malloc(n * (sizeof (struct triangle)));
  struct triangle *R2 = malloc(n * (sizeof (struct triangle)));
  struct pos *S1 = malloc((n+1) * (sizeof (struct pos)));
  *S1 = *list_midpoints(A, B, n+1);
  struct pos *S2 = malloc((n+1) * (sizeof (struct pos)));
  *S2 = *list_midpoints(B, C, n+1);
  struct pos *V1 = malloc((n+1) * (sizeof (struct pos)));
  struct pos *V2 = malloc(n * (sizeof (struct pos)));
  for (int i = 0; i < n; i++) {
    *V1 = *list_midpoints(S1[i], S2[i], n+1-i);
    *V2 = *list_midpoints(S1[i+1], S2[i+1], n - i);
    *R1 = *row_of_triangles(V1, V2, n - i);
    for (int j = 0; j < n - i; j++) {
      ALL[triangle_counter] = R1[j];
      triangle_counter++;
    }
    if (triangle_counter < n * n - 1) {
      *R2 = *upside_down_trianges(V1, V2, n - i);
      for (int k = 0; k < n - i - 1; k++) {
        ALL[triangle_counter] = R2[k];
        triangle_counter++;
      }
    }
    if (i < n - 1) {
      R2 = realloc(R2, n - i - 2);
      R1 = realloc(R1, n - i - 1);
      V1 = realloc(V1, n - i);
      V2 = realloc(V2, n - i - 1);
    }
  }
  free(R1);
  free(V1);
  free(V2);
  return ALL;
};
int main(void) {
  // Tests:

  /*
  Each function is tested individually
  */
  struct pos *A1 = malloc(sizeof (struct pos));
  struct pos *A2 = malloc(sizeof (struct pos));
  struct pos *A3 = malloc(sizeof (struct pos));
  *A1 = (struct pos) {0, 0, 1};
  *A2 = (struct pos) {0, 1, 0};
  *A3 = (struct pos) {1, 0 ,0};


  struct pos *B1 = malloc(sizeof (struct pos));
  struct pos *B2 = malloc(sizeof (struct pos));
  struct pos *B3 = malloc(sizeof (struct pos));
  *B1 = (struct pos) {0, 0, 0};
  *B2 = (struct pos) {1, 1.732, 0};
  *B3 = (struct pos) {2, 0 ,0};


  struct pos *C1 = malloc(sizeof (struct pos));
  struct pos *C2 = malloc(sizeof (struct pos));
  struct pos *C3 = malloc(sizeof (struct pos));
  /*
  struct pos *CT1 = malloc(sizeof (struct pos));
  struct pos *CT2 = malloc(sizeof (struct pos));
  struct pos *CT3 = malloc(sizeof (struct pos));
  */
  *C1 = (struct pos) {0, 0, 3};
  *C2 = (struct pos) {0, 3, 0};
  *C3 = (struct pos) {3, 0 ,0};

 // list_of_triangles(*A1, *A2, *A3, 2); 
    // should return a list of 4 triangles with the vertices (0.707,0,0.707), (0.707,0.707,0), (0,0.707,0.707), (0,0,1), (0,1,0), (1,0,0)
  struct pos AF = add_vector(*A1, *A2);
  printf("%f %f %f\n", AF.x, AF.y, AF.z);
  struct pos AG = scale_vector(AF, 100);
  printf("%f %f %f\n", AG.x, AG.y, AG.z);
  struct pos AH = scale_norm(AG);
  printf("%f %f %f\n", AH.x, AH.y, AH.z);

  struct pos BA = tri_center(*B1, *B2, *B3);
  printf("%f %f %f\n", BA.x, BA.y, BA.z);


  struct triangle CC = curve_triangle(*C1, *C2, *C3);
  struct pos CT1 = CC.A;
  struct pos CT2 = CC.B;
  struct pos CT3 = CC.C;
  struct pos CT4 = CC.center;
  printf("%f %f %f\n", CT1.x, CT1.y, CT1.z);
  printf("%f %f %f\n", CT2.x, CT2.y, CT2.z);
  printf("%f %f %f\n", CT3.x, CT3.y, CT3.z);
  printf("%f %f %f\n", CT4.x, CT4.y, CT4.z);


  struct pos *D1 = malloc(sizeof (struct pos));
  struct pos *D2 = malloc(sizeof (struct pos));
  *D1 = (struct pos) {0, 0, 0};
  *D2 = (struct pos) {3, 3, 3};
  struct pos * DL = list_midpoints(*D1, *D2, 3);
  struct pos DD1 = DL[0];
  struct pos DD2 = DL[1];
  struct pos DD3 = DL[2];
  printf("%f %f %f\n", DD1.x, DD1.y, DD1.z);
  printf("%f %f %f\n", DD2.x, DD2.y, DD2.z);
  printf("%f %f %f\n", DD3.x, DD3.y, DD3.z);
    
  // Memory freeing may or may not be necessary if working with super computers?
}
