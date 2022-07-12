#include <iostream>

using namespace std;

class Father
{
public:
    virtual void Fun() { cout << "Father::Fun" << endl; }
};

class Son : public Father
{
public:
    virtual void Fun() { cout << "Son::Fun" << endl; }
};

int main()
{
    bool flag = true; // true调用Father false调用Son
    Father *key_var;
    if (flag)
    {
        Father father;
        key_var = &father;
    }
    else
    {
        Son son;
        key_var = &son;
    }
    key_var->Fun();
    return 0;
}