#include <stdio.h>

typedef struct{
  int x;
  int y;
} point;

int add(int a, int b){
  return a + b;
}

point add_point(point a, point b){
  point added_point;
  added_point.x = add(a.x, b.x);
  added_point.y = add(a.y, b.y);
  return added_point;
}

void print_point(point p){
  printf("x=%d, y=%d\n", p.x, p.y);
}

int main(void){
  point point_1 = {1, 2};
  point point_2 = {3, 4};
  point point_3 = add_point(point_1, point_2);
  print_point(point_3);
}
