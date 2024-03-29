#include <iostream>
#include <omp.h>

void FirstPrivateExample() {
    // Перенос значения переменной из глобального уровня в параллельный
    int x = 44;
#pragma omp parallel for firstprivate(x)
    for (int i = 0; i <= 10; i++) {
        printf("x: %d \n", x);
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n", x);
}

void LastPrivateExample() {
    // Перенос значения переменной из параллельного уровня в глобальный
    int x = 44;
#pragma omp parallel for lastprivate(x)
    for (int i = 0; i <= 10; i++) {
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n", x);
}

void PrivateExample() {
    // Просто каждый поток работает со своим экземпляром переменной и уровни никак не взаимодействуют
    int x = 44;
#pragma omp parallel for private(x)
    for (int i = 0; i <= 100; i++) {
        x = i;
        printf("Thread number: %d x: %d\n", omp_get_thread_num(), x);
    }
    printf("x is %d\n", x);
}


int main() {
    PrivateExample();
    //    LastPrivateExample();
    //    FirstPrivateExample();
}
