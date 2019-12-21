int yy;
float temp = 100.0;
void test1(int x, int y){    
      test1(x, y);
      return;
}
int test(int x, int y){
      if(x <= y){
            x = y;
      }else{
            y = x;
      }
      if(y < 1){
            y = x;
      }
      while(x < 100){
            x = x - 1;
      }
      int i = test(x, y) + test(x, y);
      return test(x, y) + 1;
}
