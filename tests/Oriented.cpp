#include <iostream>

using namespace std;
// 基类
class Father
{
public:
    int money;
    Father(int a)
    {
        money = a;
    }

    void func()
    {
        cout << "Father\n"
             << money << "\n";
    }
};

// 派生类
class Son : public Father // 继承
{
public:
    int money; // 与基类同名成员变量
    Son(int a)
    {
        money = a;
    }
    void func()
    {
        cout << "Son\n"
             << money << "\n";
    }
    void myFunc()
    {
        cout << "Son\n";
    }
};

void test(Father *p)
{
    cout << p->money << "\n";
};

int main()
{

    Son *oriented_test;
    oriented_test = new Son(100);
    // oriented_test->func();
    test(oriented_test);

    delete oriented_test;

    return 0;
}
