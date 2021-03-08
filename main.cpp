//Provided by desire2020/Steve Lu

/***********************************************************************
Hint: This test case almost completely tests the correctness of your deque.
So if all tests are passed, feel free to enhance your performance! :)
Yours Sincerely. Rabbit.
***********************************************************************/
#include "deque/data/class-integer.hpp"
#include "deque/data/class-matrix.hpp"
#include "deque/data/class-bint.hpp"
#include <iostream>
#include <vector>
#include <deque>
#include "deque/deque.hpp"
#include <cmath>

class T{
private:
    int x;
public:
    T(int x):x(x){}
    int num()const {return x;}
    void change(int y){
        x = y;
    }
};
bool operator == (const T &a, const T &b){
    return a.num() == b.num();
}
bool operator != (const T &a, const T &b){
    return a.num() != b.num();
}
long long randNum(long long x,long long maxNum)
{
    x = (x * 10007) % maxNum;
    return x + 1;
}
void error()
{
    std::cout << "Error, mismatch found." << std::endl;
    exit(0);
}

const size_t N = 10005LL;
int main(){
    srand(time(NULL));
    puts("test start:");
    {
        std::cout << "Test 4 : Test for copy constructor and operator=...";
        sjtu::deque<long long> *pInt;
        pInt = new sjtu::deque<long long>;
        for (long long i = 0; i < N; ++i) {
            pInt -> push_back(i);
        }
        sjtu::deque<long long> &dInt = *pInt;
        sjtu::deque<long long> dualInt(dInt);
        sjtu::deque<long long> dualInt_oper;
        dualInt_oper = dInt;
        for (long long i = 0; i < N; ++i)
        {
            if (dualInt[i] != dInt[i] || dualInt_oper[i] != dInt[i])
                error();
        }
        dualInt_oper = dualInt_oper;
        delete pInt;
        for (long long i = 0; i < N; ++i)
        {
            if (dualInt_oper[i] != dualInt[i])
                error();
        }
        std::cout << "Correct." << std::endl;
    }
    std::cout<<sjtu::memcheck<<std::endl;
}
