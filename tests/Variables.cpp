#include <iostream>
#include <vector>

using namespace std;

namespace test
{
    struct S1
    {
        /* data */
        int f;
        static int static_var;
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

    class SonFriend;
    // 派生类
    class Son : public Father // 继承
    {
        friend class test::SonFriend;
        static Father *static_instance;
        static int static_var;

    private:
        int secret;

    public:
        struct SonStruct
        {
            /* data */
            int x;
        } y;
        int e;
        int f;
        Son()
        {
            static_var = 10;
            secret = 10;
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

    class SonFriend : public Father // 继承
    {

    public:
        int h;
        test::Son *son;
        SonFriend()
        {
        }

        void TestFriend()
        {
            son = new test::Son();
            h = son->secret;
        }
    };

    class GrandSon : public Son // 继承
    {
    public:
        int g;
        vector<int> j;
        GrandSon()
        {
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

    struct test::S1 struct_array[3];
    struct_array[0].f = 5;
    struct_array[0].static_var = 8;
    /////////////////////////////

    test::Father *fa = new test::Father();
    test::Son *son = new test::Son();
    test::SonFriend *son_friend = new test::SonFriend();
    test::GrandSon *grandson = new test::GrandSon();
    fa->a = 1;
    fa->b = 2;

    son->a = 3;
    son->b = 4;
    son->c = 5;
    son->d = 6.0;
    son->e = 7;
    son->f = 8;

    son_friend->TestFriend();

    grandson->g = 10;
    grandson->j.push_back(1);

    /////////////////////////////

    return 0;
}