#include <iostream>

using namespace std;
// 基类
class Father
{
public:
    int money = 5;
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
    int money = 10; // 与基类同名成员变量
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

int main()
{

    Son *oriented_test;
    oriented_test = new Son();
    // oriented_test->func();
    test(oriented_test);

    delete oriented_test;

    return 0;
}

