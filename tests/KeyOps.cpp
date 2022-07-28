#include <iostream>

using namespace std;

struct S1
{
    /* data */
    int f;
    static int static_var;
};

class Father
{
public:
    int a;
    int b;
    short c;
    double d;
    struct S1 *key_var;
    Father()
    {
        cout << "Father construct\n";
    }

    void func()
    {
        this->key_var = new S1();
        this->key_var->f = 0;
        cout << "Father\n"
             << a << "\n";
    }

    void func2()
    {
        this->key_var->f = 2;
    }
};

int main()
{

    Father *fa = new Father();
    fa->func();
    fa->func2();
    return 0;
}