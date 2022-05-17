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
struct triangle {
  struct pos A;
  struct pos B;
  struct pos C;
};
struct pos scale_norm (struct pos A) {
  double norm = A.x * A.x + A.y * A.y + A.z * A.z;
  norm = sqrt(norm);
  struct pos B;
  B.x = A.x/norm;
  B.y = A.y/norm;
  B.z = A.z/norm;
  return B;
  
}
struct pos add_vector (struct pos A, struct pos B) {
  struct pos C;
  C.x = A.x + B.x;
  C.y = A.y + B.y;
  C.z = A.z + B.z;
  return C;
}
struct pos scale_vector (struct pos A, double x) {
  struct pos C;
  C.x = A.x * x;
  C.y = A.y * x;
  C.z = A.z * x;
  return C;
}
struct triangle curve_triangle (struct pos A, struct pos B, struct pos C) {
  struct triangle sub_equilateral;
  sub_equilateral.A = scale_norm(A);
  sub_equilateral.B = scale_norm(B);
  sub_equilateral.C = scale_norm(C);
  return sub_equilateral;
}
struct pos list_midpoints (struct pos A, struct pos B, int m) {
  struct pos *M = malloc(m * (sizeof (struct pos)));
  struct pos A_to_B;
    A_to_B.x = B.x - A.x;
    A_to_B.y = B.y - A.y;
    A_to_B.z = B.z - A.z;
  for (int i = 0; i <= m; i++) {
    M[i] = add_vector(A, scale_vector(A_to_B, i/m));
  }
  return *M;
};
struct triangle row_of_triangles (struct pos *A, struct pos *B, int b) {
  struct triangle *S = malloc(b * (sizeof (struct triangle)));
  for (int i = 0; i < b; i++) {
    S[i] = curve_triangle(A[i], A[i+1], B[i]);
  }
  return *S;
}
// n is defined as the number of segments the sides will be split into. For example, to split a single triangle into 4, we would let n = 2.
// The function does not return ALL the triangles in the partition, only the smallest ones. For example, when n = 3, the function would only return the 9 smallest sub-triangles instead of 13 of them. (You may be familiar with the old math puzzle where you are presented with a diagram and asked to FiNd AlL tHe TrIaNgLeS - This function DOES NOT solve such questions)
struct triangle* list_of_triangles (struct pos A, struct pos B, struct pos C, int n) {
  int triangle_counter = 0;
  struct triangle *ALL = malloc(n * n * (sizeof (struct triangle)));
  struct triangle *R1 = malloc(n * (sizeof (struct triangle)));
  struct pos *S1 = malloc((n+1) * (sizeof (struct pos)));
  *S1 = list_midpoints(A, B, n+1);
  struct pos *S2 = malloc((n+1) * (sizeof (struct pos)));
  *S2 = list_midpoints(B, C, n+1);
  struct pos *V1 = malloc((n+1) * (sizeof (struct pos)));
  struct pos *V2 = malloc(n * (sizeof (struct pos)));
  for (int i = 0; i < n; i++) {
    *V1 = list_midpoints(S1[i], S2[i], n+1-i);
    *V2 = list_midpoints(S1[i+1], S2[i+1], n - i);
    *R1 = row_of_triangles(V1, V2, n - i);
    for (int j = 0; j < n - i; j++) {
      ALL[triangle_counter] = R1[j];
      triangle_counter++;
    }
    if (i < n - 1) {
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
  struct pos *A1 = malloc(sizeof (struct pos));
  struct pos *A2 = malloc(sizeof (struct pos));
  struct pos *A3 = malloc(sizeof (struct pos));
  *A1 = (struct pos) {0, 0, 1};
  *A2 = (struct pos) {0, 1, 0};
  *A3 = (struct pos) {1, 0 ,0};
  list_of_triangles(*A1, *A2, *A3, 2); // should return a list of 4 triangles with the vertices (0.707,0,0.707), (0.707,0.707,0), (0,0.707,0.707), (0,0,1), (0,1,0), (1,0,0)
}