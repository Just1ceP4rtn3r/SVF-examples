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
    int (*key_var)(int a);
    key_var = test;
    if (flag)
    {
        key_var = test2;
    }
    cout << key_var(10) << endl;
    return 0;
}