int test(int x){
      int res = 1;
       while (x >= 0){
             if(x == 0){
                  break;
             }
             res = res * x;
             x = x - 1;
       }
       return res;
}
int main(){
      int x = test(6);
      return 1;
}