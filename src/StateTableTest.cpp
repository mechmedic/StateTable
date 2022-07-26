#include <iostream>
//#include "StateArray.h"
//#include "StateIndex.h"
#include "StateTable.h"

using namespace std;


int main()
{
    cout << "Hello World!" << endl;

//    StateArray<float> xarr(3.14,10);
//    StateIndex id;

//    for(size_t i=0; i<10; i++)
//    {
//        cout << xarr.Element(i) << " ";
//    }
//    cout<<endl;

//    xarr.Set(5,10.0f);

//    std::vector<int> dd(5,10);
//    if(xarr.Set(6,dd))
//    {

//    }
//    else {
//        cout << "Casting failed" << endl;
//    } // CKim - any_cast error

//    for(size_t i=0; i<10; i++)
//    {
//        cout << xarr.Element(i) << " ";
//    }
//    cout<<endl;


    StateTable theTable(10,"TableName");

    // CKim - This is how one adds element to the table. Scope of x
    // needs to be throuought the life of the statetable
    int kkk = 10;
    theTable.AddData(kkk,"Num");

    // CKim - Update the table and advance. Calling advance writes to the table.
    theTable.Start();
    kkk = 100;
    theTable.Advance();

    // CKim - Read from the table. Use mtsXXX for data type.
    // First, obtain 'accessor' of the state table and then
    // use 'GetLatest' method of the accessor class
    StateTable::Accessor<int>* acc = dynamic_cast<StateTable::Accessor<int>*> (theTable.GetAccessorByName("Num"));
    //StateIndex idxRead = theTable.GetIndexReader();
    int k = 0;
    int& kref = k;
    //acc->GetLatest(kref);
    acc->GetLatest(k);
    cout<<"Index has "<<k<<endl;

    kkk = 1000;
    cout<<"Before calling Advance, Index still has "<<k<<endl;
    theTable.Advance();
    //acc->GetLatest(kref);
    acc->GetLatest(k);
    cout<<"Calling Advance updates index and now Index has "<<k<<endl;
    cout<<"Index has "<<k<<endl;

    theTable.ToStream(std::cout);

    // ------------------------

    //vector<any&> anyVec(10);

//    // CKim - This how one initializes mtsStateTable
//    mtsStateTable theTable(10,"TableName");
//    cout<<theTable<<endl;

//    // CKim - This is how one adds element to the table. Scope of x
//    // needs to be throuought the life of the statetable
//    mtsInt x= 10;
//    theTable.AddData(x,"Num");
//    cout<<theTable<<endl;


//    // CKim - Update the table and advance. Calling advance writes to the table.
//    theTable.Start();
//    mtsInt x = 100;
//    theTable.Advance();

//    // CKim - Read from the table. Use mtsXXX for data type.
//    // First, obtain 'accessor' of the state table and then
//    // use 'GetLatest' method of the accessor class
//    mtsStateTable::Accessor<mtsInt>* a = dynamic_cast<mtsStateTable::Accessor<mtsInt>*> (theTable.GetAccessorByName("Num"));
//    mtsStateIndex idxRead = theTable.GetIndexReader();
//    mtsInt k = 0;
//    mtsInt& kref = k;
//    a->GetLatest(kref);
//    cout<<"Index "<<idxRead<<" has "<<k<<endl;


//    x = 200;
//    theTable.Advance();
//    idxRead = theTable.GetIndexReader();
//    a->GetLatest(kref);
//    cout<<"Index "<<idxRead<<" has "<<k<<endl;


//    //mtsStateTable::AccessorBase* accidx = theTable.GetAccessorByName("Num");

////    cout<<idxRead.Index()<<endl;
////    mtsStateIndex idxWrite = theTable.GetIndexWriter();
////    x = 300;
////    cout<<idxRead<<endl;
////    cout<<idxWrite<<endl;
////    theTable.CSVWrite(cout);

    return 0;
}
