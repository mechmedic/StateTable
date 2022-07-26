
/**
 ***************************************************************************
 * \file  StateArray.h
 * \brief Header file for 'StateArrayBase' class.
 * StateArray class used in StateTable for storing data in the table,
 * based on 'mtsStateTable' of the CISST library
 * Created : 2022.07.08 Chunwoo Kim (CKim)
 * Last Updated : 2022.07.08 Chunwoo Kim (CKim)
 * Contact Info : cwkim@kist.re.kr
 ***************************************************************************
**/


#ifndef STATEARRAY_H
#define STATEARRAY_H

#include <vector>
#include <sstream>
#include "StateArrayBase.h"


// CKim - Comments from CISST library!
// Individual state array classes can be created from an instance of
// the following template, where _elementType represents the type of
// data used by the particular state element. It is assumed that
//  _elementType is derived from mtsGenericObject.

template <class _elementType>
class StateArray :public StateArrayBase
{
public:
    typedef size_t index_type;
    typedef size_t size_type;
    typedef _elementType value_type;
    typedef std::vector<value_type> VectorType;

    // CKim - std::vector provides an interator
    typedef typename VectorType::iterator iterator;
    typedef typename VectorType::const_iterator const_iterator;

protected:
    /*! A vector to store the data. These element of the vector
      represents the cell of the state data table. */
    VectorType Data;

public:
    /*! Default constructor. Does nothing */
    inline StateArray(const value_type & objectExample, size_type size = 0):
        Data(size, objectExample)  {}

    /*! Default destructor. */
    virtual ~StateArray() {}


    bool SetDataSize(const size_t size)
    {
        value_type objectExample = Data[0];
        //\todo add try catch for alloc exception
        Data.resize(size, objectExample);
        return true;
    }

    /*! Access element at index. This returns the data of the derived type
      (value_type) rather than the base type (mtsGenericObject), which is
      returned by the overloaded operator []. */
    const value_type & Element(index_type index) const { return Data[index]; }
    value_type & Element(index_type index) { return Data[index]; }

//    /*! Overloaded [] operator. Returns data at index (of type mtsGenericObject).
//        Currently used for data collection (mtsCollectorState). */
//    inline value_type & operator[](index_type index){ return Data[index]; }
//    inline const value_type & operator[](index_type index) const { return Data[index]; }
    inline const std::string operator[](index_type index) const
    {
        std::ostringstream ostr;
        ostr << Data[index];
        return ostr.str();
    }

//    /* Create the array of data. This is currently unused. */
//    inline mtsStateArrayBase * Create(const mtsGenericObject * objectExample,
//                                      size_type size) {
//        const value_type * typedObjectExample = dynamic_cast<const value_type *>(objectExample);
//        if (typedObjectExample) {
//            this->Data.resize(size, *typedObjectExample);
//            this->DataClassServices = objectExample->Services();
//            CMN_ASSERT(this->DataClassServices);
//        } else {
//            CMN_LOG_INIT_ERROR << "mtsStateArray: Create used with an object example of the wrong type, received: "
//                               << objectExample->Services()->GetName()
//                               << " while expecting "
//                               << value_type::ClassServices()->GetName()
//                               << std::endl;
//            return 0;
//        }
//        return this;
//    }


    /*! Copy data from one index to another within the same array.  */
    inline void Copy(index_type indexTo, index_type indexFrom) {
        this->Data[indexTo] = this->Data[indexFrom];
    }


    /*! Get and Set data from array.  The Get and Set member functions
      deserve special mention because they must overcome a limitation
      of C++ -- namely, that it does not fully support containers of
      heterogeneous objects. In particular, we expect the 'object'
      parameter to be of type _elementType& (the derived class) rather
      than mtsGenericObject& (the base class). This can be handled
      using std::any
     */
    //@{
    bool Get(index_type index, const std::any & pData) const;
    bool Set(index_type index, const std::any & pData);
    //@}
};

// CKim - Implementation of Get and Set Method. Use std::any_cast
// any_cast<T>(std::any*) returns T* if successful or 0 if not.

#include <iostream>
template <class _elementType>
bool StateArray<_elementType>::Set(index_type index,  const std::any & pData)
{
    // CKim - Address of std::any and the actual data stored in std::any are different
    // Use any_cast<Type T>(&any) to obtain pointer to actual data
    const _elementType* ptr;
    try {    ptr = std::any_cast<_elementType*>(pData);     }
    catch (const std::bad_any_cast& e)  {
        std::cout << "any_cast error\n";    return false;   }
    Data[index] = *ptr;
    return true;
}

template <class _elementType>
bool StateArray<_elementType>::Get(index_type index, const std::any & pData) const
{
    // CKim - Address of std::any and the actual data stored in std::any are different
    // Use any_cast<Type T>(&any) to obtain pointer to actual data
    _elementType* ptr;
    try {    ptr = std::any_cast<_elementType*>(pData);     }
    catch (const std::bad_any_cast& e)  {
        std::cout << "any_cast error\n";    return false;   }
    *ptr = Data[index];
    return true;
}

#endif // STATEARRAY_H

