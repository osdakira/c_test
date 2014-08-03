#include "point.c"

int main(void){
  point point_1 = {1, 2};
  point point_2 = {3, 4};
  point point_3 = add_point(point_1, point_2);
  print_point(point_3);
}
