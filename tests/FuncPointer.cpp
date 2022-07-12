#include <iostream>

using namespace std;

int test(int a)
{
    return a;
}

int test2(int a)
{
    return a + 1;
}

int main()
{
    bool flag = false;
    int (*fp)(int a);
    fp = test;
    if (flag)
    {
        fp = test2;
    }
    cout << fp(2) << endl;
    return 0;
}