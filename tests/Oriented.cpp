#include <iostream>

using namespace std;
// 基类
class Father
{
public:
    int money;
    void func()
    {
        cout << "Father\n";
    }
};

// 派生类
class Son : public Father // 继承
{
public:
    int money; // 与基类同名成员变量
    void func()
    {
        cout << "Son\n";
    }

    void myFunc();
};

int main()
{

    Father *oriented_test;
    oriented_test = new Son();
    oriented_test->func();

    delete oriented_test;

    return 0;
}
