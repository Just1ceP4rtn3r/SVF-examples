#include <iostream>

using namespace std;

class Fruit
{
    public:
        virtual void colour() = 0;
};

class Apple: public Fruit
{
    public:
        virtual void colour(){ cout << "Apple:Red" << endl ; }
};

class Banana: public Fruit
{
    public:
        virtual void colour(){ cout << "Banana:Yellow" << endl ; }
};

int main()
{
    bool flag = true;//true调用Apple false调用Banana
    Fruit *fruit;
    Apple apple;
    Banana banana;
    if(flag)
        fruit = &apple;
    else 
        fruit = &banana;
    fruit->colour();
    return 0;
}