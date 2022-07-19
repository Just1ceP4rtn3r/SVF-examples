#include <iostream>

using namespace std;

struct S1
{
    /* data */
    int f;
};

struct S2
{

    int a;
    int b;
    short c;
    double d;
    struct S1 e;

} field_test;

class Father
{
public:
    int a;
    int b;
    short c;
    double d;
    Father()
    {
        cout << "Father construct\n";
    }

    void func()
    {
        cout << "Father\n"
             << a << "\n";
    }
};

// 派生类
class Son : public Father // 继承
{
public:
    int e;
    Son()
    {
        a = 10;
    }

    void func()
    {
        cout << "Son\n"
             << a << "\n";
    }
    void myFunc()
    {
        cout << "Son\n";
    }
    int f;
};

int main()
{
    field_test.a = 1;
    field_test.b = 2;
    field_test.c = 2;
    field_test.d = 2.0;
    field_test.e = S1();

    /////////////////////////////

    Father *fa = new Father();
    Son *son = new Son();
    fa->a = 1;
    fa->b = 2;

    son->a = 3;
    son->b = 4;
    son->c = 5;
    son->d = 6.0;

    son->e = 7;
    son->f = 8;

    return 0;
}