/**
 ***************************************************************************
 * \file  StateIndex.h
 * \brief Header file for 'StateIndex' (State table) class.
 * This class defines the index used for the StateTable,
 * based on 'mtsStateTable' of the CISST library
 * Created : 2022.07.08 Chunwoo Kim (CKim)
 * Last Updated : 2022.07.08 Chunwoo Kim (CKim)
 * Contact Info : cwkim@kist.re.kr
 ***************************************************************************
**/

#ifndef STATEINDEX_H
#define STATEINDEX_H

//#include <cisstMultiTask/mtsGenericObject.h>
//#include <cisstCommon/cmnSerializer.h>
//#include <cisstCommon/cmnDeSerializer.h>
//#include <cisstCommon/cmnGenericObjectProxy.h>

//#include <cisstMultiTask/mtsExport.h>


// Comments from CISST library
//  The time indexing of state data is provided by the mtsStateIndex
//  class.

//  Each iteration of the Task Loop increments a tick counter, which is
//  stored as an unsigned long.  (mtsTimeTicks). A mtsStateIndex object
//  contains the following information: The tick value is stored in the
//  mtsStateIndex class to act as a data validity check. This is used to
//  ensure that the circular buffer has not wrapped around and
//  overwritten the index entry between the time that the mtsStateIndex
//  object was created and the time it was used.

class StateIndex
{
 public:
    /*! TimeTicks are typedef'ed as unsigned long long (64 bits)*/
    typedef unsigned long long int TimeTicksType;

//    /*! Base type */
//    typedef mtsGenericObject BaseType;

 private:
    /*! The index into the set of circular buffers corresponding to
      the time */
    int TimeIndex;

    /*! The tick value corresponding to the time. */
    TimeTicksType TimeTicks;

    /*! The size of the circular buffer. */
    int BufferLength;

public:
    /*! Default constructor. Does nothing. */
    inline StateIndex():
        TimeIndex(0), TimeTicks(0), BufferLength(0)
    {}

    /*! Default constructor. Does nothing. */
    inline StateIndex(double timestamp, int index, TimeTicksType ticks, int Length):
        //BaseType(timestamp, false /* automatic timestamp */, true /* valid */),
        TimeIndex(index), TimeTicks(ticks), BufferLength(Length)
    {}

    /*! Default destructor. Does nothing. */
    ~StateIndex() {}

    /*! The length of the circular buffer. */
    int Length(void) const {
        return BufferLength;
    }

    //*! Return the index into the circular buffer.*/
    int Index(void) const {
       return TimeIndex;
    }

    /*! Return the ticks corresponding to the time. */
    TimeTicksType Ticks(void) const {
        return TimeTicks;
    }

    /*!
        Note that the increment operators are not defined for
        this class, since we dont want future times.
     */
    StateIndex& operator--() {
        TimeTicks--;
        TimeIndex--;
        if (TimeIndex<0) {
            TimeIndex = BufferLength-1;
        }
        return *this;
    }

    /*! Overloaded operator */
    StateIndex& operator-=(int number) {
        TimeTicks -= number;
        TimeIndex = static_cast<int>(TimeTicks % BufferLength);
        return *this;
    }

    /*! Overloaded operator */
    StateIndex operator-(int number) {
        StateIndex tmp = *this;
        return (tmp -= number);
    }

    /*! The comparison operators */
    bool operator==(const StateIndex & that) const {
        return (TimeIndex == that.TimeIndex && TimeTicks == that.TimeTicks);
    }

    /*! Overloaded operator */
    bool operator!=(const StateIndex & that) const {
        return !(*this == that);
    }

    /*! Human readable text output */
    virtual void ToStream(std::ostream & outputStream) const {
        outputStream << "Index = " << TimeIndex << ", Ticks = " << Ticks() << ", Length = " << BufferLength;
    };

//    /*! Machine reabable text output */
//    virtual void ToStreamRaw(std::ostream & outputStream, const char delimiter = ' ',
//                             bool headerOnly = false, const std::string & headerPrefix = "") const;

//    /*! Serialize the content of the object without any extra
//      information, i.e. no class type nor format version.  The
//      "receiver" is supposed to already know what to expect. */
//    void SerializeRaw(std::ostream & outputStream) const;

//    /*! De-serialize the content of the object without any extra
//      information, i.e. no class type nor format version. */
//    virtual void DeSerializeRaw(std::istream & inputStream);
};


//void StateIndex::ToStream(std::ostream & outputStream) const
//{
//    outputStream << "Index = " << TimeIndex << ", Ticks = " << Ticks()
//                 << ", Length = " << BufferLength;
//}

//void StateIndex::ToStreamRaw(std::ostream & outputStream, const char delimiter,
//                                bool headerOnly, const std::string & headerPrefix) const
//{
//    mtsGenericObject::ToStreamRaw(outputStream, delimiter, headerOnly, headerPrefix);
//    outputStream << delimiter;
//    if (headerOnly) {
//        outputStream << headerPrefix << "-timeindex" << delimiter
//                     << headerPrefix << "-timeticks" << delimiter
//                     << headerPrefix << "-bufferlength";
//    } else {
//        outputStream << this->TimeIndex << delimiter
//                     << this->TimeTicks << delimiter
//                     << this->BufferLength;
//    }
//}


//void mtsStateIndex::SerializeRaw(std::ostream & outputStream) const
//{
//    cmnSerializeRaw(outputStream, this->TimeIndex);
//    cmnSerializeRaw(outputStream, this->TimeTicks);
//    cmnSerializeRaw(outputStream, this->BufferLength);
//}


//void mtsStateIndex::DeSerializeRaw(std::istream & inputStream)
//{
//    cmnDeSerializeRaw(inputStream, this->TimeIndex);
//    cmnDeSerializeRaw(inputStream, this->TimeTicks);
//    cmnDeSerializeRaw(inputStream, this->BufferLength);
//}


#endif // STATEINDEX_H
