#include <stdio.h>

 int fun(int temp){
    return temp +1;
}
 int main(){
    int (*fun_ptr)(int) = &fun;
    fun_ptr(9);
    int c = 10;
    int n;
    printf("Enter the value of n: \n");
    scanf("%d", &n);
    for (int i=0; i<n; i++){
         c = c + 1;
    }
    return c;
}
// br_7
// br_7
// br_7
// br_13

// br_7: fileX, 5, 6
// br_13: fileX, 5, 8

// br_33552192: fileX.cpp, 5, 6
// br_33552352: fileX.cpp, 5, 8