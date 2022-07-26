#include <iostream>
#include <cisstMultiTask.h>

using namespace std;

int main()
{
    cout << "Hello World!" << endl;

    // CKim - This how one initializes mtsStateTable
    mtsStateTable theTable(10,"TableName");
    cout<<theTable<<endl;

    // CKim - This is how one adds element to the table. Scope of x
    // needs to be throuought the life of the statetable
    mtsInt x= 10;
    theTable.AddData(x,"Num");
    cout<<theTable<<endl;


    // CKim - Update the table and advance. Calling advance writes to the table.
    theTable.Start();
    mtsInt x = 100;
    theTable.Advance();

    // CKim - Read from the table. Use mtsXXX for data type.
    // First, obtain 'accessor' of the state table and then
    // use 'GetLatest' method of the accessor class
    mtsStateTable::Accessor<mtsInt>* a = dynamic_cast<mtsStateTable::Accessor<mtsInt>*> (theTable.GetAccessorByName("Num"));
    mtsStateIndex idxRead = theTable.GetIndexReader();
    mtsInt k = 0;
    mtsInt& kref = k;
    a->GetLatest(kref);
    cout<<"Index "<<idxRead<<" has "<<k<<endl;


    x = 200;
    theTable.Advance();
    idxRead = theTable.GetIndexReader();
    a->GetLatest(kref);
    cout<<"Index "<<idxRead<<" has "<<k<<endl;


    //mtsStateTable::AccessorBase* accidx = theTable.GetAccessorByName("Num");

//    cout<<idxRead.Index()<<endl;
//    mtsStateIndex idxWrite = theTable.GetIndexWriter();
//    x = 300;
//    cout<<idxRead<<endl;
//    cout<<idxWrite<<endl;
//    theTable.CSVWrite(cout);

    return 0;
}
