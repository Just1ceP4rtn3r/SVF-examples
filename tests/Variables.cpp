#include <iostream>

using namespace std;

namespace test
{
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
    // namespace test

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
        struct SonStruct
        {
            /* data */
            int x;
        } y;
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

    class Son2 : public Son // 继承
    {
    public:
        int g;
        Son2()
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
    };

}
int main()
{
    test::field_test.a = 1;
    test::field_test.b = 2;
    test::field_test.c = 2;
    test::field_test.d = 2.0;
    test::field_test.e = test::S1();

    /////////////////////////////

    test::Father *fa = new test::Father();
    test::Son *son = new test::Son();
    test::Son2 *son2 = new test::Son2();
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