
#include <iostream>
#include <atomic>
#include "cmdQueue.h"

using namespace std;

int main()
{
    atomic<int*> atomicPtr(0);
    int x = 10;
    cout <<"x = "<< x << " located at "<< hex << &x << endl;

    int* k = &x;//atomicPtr;
    cout <<"int* located at "<< hex << k << " has value " << dec << *k << endl;

    atomicPtr = k;
    //k = atomicPtr;
    cout <<"atomicPtr located at "<< hex << atomicPtr << " has value " << dec << *atomicPtr << endl;

    atomic<float> atomicFloat(3.14);
    cout <<"atomicFloat has value " << atomicFloat << endl;

    const int* p;
    cmdQueue<int> myQueue(11,0);    // CKim - pass desired number of element + 1
    for(int i=0; i<11; i++)
    {
        p = myQueue.Put(5*i);
        if(p)   {
            cout << "Putting " << 5*i << endl;    }
            //cout << "Object in Head " << *p << endl;    }
        else    {
            cout << "Queue Full" << endl;               }
    }

    for(int i=0; i<11; i++)
    {
        p = myQueue.Get();
        if(p)   {
            cout << "Object in Queue " << *p << endl;   }
        else    {
            cout << "Queue Empty" << endl;              }
    }
}
