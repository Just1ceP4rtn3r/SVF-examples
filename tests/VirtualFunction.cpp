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
    Father *text;
    if (flag)
    {
        Father father;
        text = &father;
    }
    else
    {
        Son son;
        text = &son;
    }
    text->Fun();
    return 0;
}

