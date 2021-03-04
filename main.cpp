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

long long randNum(long long x,long long maxNum)
{
    x = (x * 10007) % maxNum;
    return x + 1;
}
const size_t N = 10005LL;

void error()
{
    std::cout << "Error, mismatch found." << std::endl;
    exit(0);
}


int main()
{
    sjtu::deque<int> a;
    auto it=a.begin();
    for(int i=0;i<100;i++)
    {
        a.insert(it,i);
        it++;
    }
    for(auto it:a)std::cout<<it<<' ';std::cout<<std::endl;
    for(int i=0;i<100;i++)
    {
        a.erase(a.end()-1-5);
        for(auto it:a)std::cout<<it<<' ';std::cout<<std::endl;
    }
    return 0;
}