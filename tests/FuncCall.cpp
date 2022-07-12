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
    int key_var = 10;
    test(key_var);
    return 0;
}