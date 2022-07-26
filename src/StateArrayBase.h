/**
 ***************************************************************************
 * \file  StateArrayBase.h
 * \brief Header file for 'StateArrayBase' class.
 * Based on 'mtsStateTable' of the CISST library.
 * Abstract base class for state data arrays.  This class is mostly
 * pure virtual and is used as a base class for mtsStateArray which is
 * templated.  Using a base class allows to keep all the state arrays
 * in an homogenous container of pointers on different types of state
 * arrays.
 * std::vector<Type T> Type T cannot be a templated so we use a base class
 * Created : 2022.07.11 Chunwoo Kim (CKim)
 * Last Updated : 2022.07.11 Chunwoo Kim (CKim)
 * Contact Info : cwkim@kist.re.kr
 ***************************************************************************
**/

#ifndef STATEARRAYBASE_H
#define STATEARRAYBASE_H

#include <any>
#include <string>
#include <typeinfo>

class StateArrayBase {
protected:
    /*! Protected constructor. Does nothing. */
    inline StateArrayBase(void){};

public:
    typedef size_t index_type;
    typedef size_t size_type;

    /*! Default destructor. Does nothing. */
    inline virtual ~StateArrayBase(void) {};

//    /*! Overloaded subscript operator. */
//    virtual std::any & operator[](index_type index) = 0;

//    /*! Overloaded subscript operator. */
//    virtual const std::any & operator[](index_type index) const = 0;

    virtual const std::string operator[](index_type index) const = 0;


//	/*! Create the array of data.  This is currently unused. */
//	virtual mtsStateArrayBase * Create(const mtsGenericObject * objectExample, size_type size) = 0;

	/*! Copy data from one index to another. */
	virtual void Copy(index_type indexTo, index_type indexFrom) = 0;

	/*! Get data from array. */
    virtual bool Get(index_type index, const std::any & pData) const = 0;

	/*! Set data in array. */
    virtual bool Set(index_type index, const std::any & pData) = 0;

    virtual bool SetDataSize(const size_t size) = 0;

    bool SetSize(const size_t size){
        return SetDataSize(size);
    }

};


#endif // _mtsStateArrayBase_h

