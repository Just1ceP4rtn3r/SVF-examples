#include <iostream>

using namespace std;
// 基类
class Father
{
public:
    int money;
    Father()
    {
        cout << "Father construct\n";
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

    Son()
    {
        money = 10;
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

    Son *oriented_test = new Son();
    cout << oriented_test->money;

    Father *fa = oriented_test;
    cout << fa->money;

    // test(oriented_test);

    delete oriented_test;

    return 0;
}

// #include <iostream>
// using namespace std;
// class Base
// { //声明基类Base
// public:
//     int i;
//     int j;
//     Base(int x) //基类Base的构造函数
//     {
//         i = x;
//     }
//     Base(const Base &b)
//     {
//         cout << "Base Copyconstructor" << endl;
//     }
//     void show() //成员函数
//     {
//         cout << "i=" << i << ", j=" << j << endl;
//     }
// };
// class Derived : public Base
// { //声明基类Base的公有派生类Derived
// public:
//     Derived(int x) : Base(x) //派生类的构造函数
//     {
//     }
//     Derived(const Derived &d) : Base(d)
//     {
//         cout << "Derived Copyconstructor" << endl;
//     }
// };
// void fun(Base &bb) //普通函数，形参为基类对象的引用
// {
//     cout << bb.i << endl;
// }
// int main()
// {
//     Base b1(100); //定义基类对象b1
//     b1.show();
//     Derived d1(11); //定义派生类对象d1
//     b1 = d1;        //用派生类对象d1给基类对象b1赋值
//     b1.show();

//     Derived d2(22); //定义派生类对象d2
//     Base &b2 = d2;  //用派生类对象d2来对基类对象的引用b2进行初始化
//     b2.show();

//     Derived d3(33); //定义派生类对象d3
//     d3.j = 10;
//     Base *b3 = &d3; //把派生类对象的地址&d3赋值给指向基类对象的指针b3
//     b3->show();

//     Derived d4(44); //定义派生类对象d4
//     fun(d4);        //派生类对象d4作为函数fun的实参

//     Derived d6(d4);
//     return 0;
// }