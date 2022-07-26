/**
 ***************************************************************************
 * \file  StateTable.h
 * \brief Header file for 'StateTable' (State table) class.
 * This class is based on 'mtsStateTable' of the CISST library which is a
 * table maintaining time history of state variables, and can be accessed
 * in a thread-safe manner for multiple reader, single writer assuming that
 * that pointer updates are atomic.
 * Each component will own one 'StateTable' that keeps record of its state
 * Each column of a table is a StateVariable and Each row is a time
 * For example, StateTable of a sensor component
 *   Name    "Tic"   "ErrorCode"     "ADCVal"    "Flag"     : StateVectorDataName
 *          10 us      0x36            520          1
 *          30 us      0x00            201          0
 * Each column is "StateVector"
 * Each Row is vector of heterogeneous data type, implemented by using std::any
 * Only one thread (usually the thread of a component that owns the table)
 * should call 'Advance' function which updates the writing pointer.
  * Created : 2022.07.07 Chunwoo Kim (CKim)
 * Last Updated : 2022.07.09 Chunwoo Kim (CKim)
 * Contact Info : cwkim@kist.re.kr
 ***************************************************************************
**/

// Comment from cisst library
// The state data table is the storage for the state of the task that
// the table is associated with. It is a heterogenous circular buffer
// and can contain data of any type so long as it is derived from
// mtsGenericObject.  The state data table also resolves conflicts
// between reads and writes to the state, by ensuring that the reader
// head is always one behind the write head. To ensure this we have an
// assumption here that there is only one writer, though there can be
// multiple readers. State Data Table is also refered as Data Table or
// State Table elsewhere in the documentation.


#ifndef STATETABLE_H
#define STATETABLE_H

#include "StateArray.h"
#include "StateIndex.h"
#include "TimeServer.h"

#include <vector>
#include <any>
#include <iostream>

/*! StateDataId.  Unique identifier for the columns of the State
  Data Table.  Typedef'ed to an int */
typedef int StateDataId;

// CKim - StateTable has 4 std::vector
// 1. StateVector -> Column vector storing time histroy of table entry
// 1. StateVectorDataName -> Stores name of each entry
// 3. StateVectorElement -> Row vector storing current value of each entry
// 4. StateVectorAccessor -> Stores Accessor for each entry
class StateTable
{
    // CKim - Utility classes (AccessorBase, Accessor, .... )
    // They are used for accessing the StateTable. Defined inside the definition of the
    // StateTable class. These are called 'NestedClass'. They are in namespace of 'StateTable'
    // For example, StateTable::AccessorBase
public:
    // CKim - Nested class 'AccessorBase' inherited by 'Accessor' class
    class AccessorBase
    {
    protected:
        const StateTable &Table;
        StateDataId Id;   // Not currently used
    public:
        AccessorBase(const StateTable &table, StateDataId id): Table(table), Id(id) {}
        virtual ~AccessorBase() {}
//        virtual void ToStream(std::ostream & outputStream, const StateIndex & when) const = 0;
    };

    // ---- Beginning of 'Accessor' Class Definition ----------------
    // CKim - Nested class 'Accessor' inherits 'AccessorBase' class, templated by element it stores
    template <class _elementType>
    class Accessor : public AccessorBase
    {
        //typedef typename _elementType value_type;
        //typedef typename _elementType& value_ref_type; // ???? Check
        typedef typename StateTable::Accessor<_elementType> ThisType;
        const StateArray<_elementType> & History;
        _elementType * Current;

    public:
        // CKim - Accessor is initialized by the column of the table ('History')
        Accessor(const StateTable & table, StateDataId id,
                 const StateArray<_elementType> * history, _elementType * data):
            AccessorBase(table, id), History(*history), Current(data) {}

//        void ToStream(std::ostream & outputStream, const StateIndex & when) const {
//            History.Element(when.Index()).ToStream(outputStream);
//        }

        bool Get(const StateIndex & when, _elementType& data) const {
            data = History.Element(when.Index());
            return Table.ValidateReadIndex(when);
        }

        //This should be used with caution because
        //the state table mechanism could override the data that the pointer is pointing to.
        const _elementType * GetPointer(const StateIndex & when) const {
            if (!Table.ValidateReadIndex(when))
                return 0;
            else
                return  &(History.Element(when.Index()));
        }

//        bool Get(const StateIndex & when, std::any & data) const {
//            _elementType* pdata = std::any_cast<_elementType>(&data);
//            if (pdata)  {    return Get(when, *pdata);                          }
//            else        {   std::cout << "any_cast error\n";    return false;   }
//        }

        bool GetLatest(_elementType & data) const {
            return Get(Table.GetIndexReader(), data);
        }
//        bool GetLatest(std::any & data) const {
//            return Get(Table.GetIndexReader(), data);
//        }

        bool GetDelayed(_elementType & data) const {
            return Get(Table.GetIndexDelayed(), data);
        }
//        bool GetDelayed(std::any & data) const {
//            return Get(Table.GetIndexDelayed(), data);
//        }
        void SetCurrent(const _elementType & data) {
            *Current = data;
        }
    };
    // ---- End of 'Accessor' Class Definition ----------------

 protected:

    /*! Flag to indicate if the table has started.  True between
      Start() and Advance() calls, false otherwise. */
    bool mStarted;

    /*! The number of rows of the state data table. */
    size_t HistoryLength;

    /*! The index of the writer in the data table. */
    size_t IndexWriter;

    /*! The index of the reader in the table. */
    size_t IndexReader;

    /*! The index of the delayed reader in the table. */
    size_t IndexDelayed;

    /*! Delay used for GetIndexDelayed and GetDelayed.  In number of
      rows in state tables. */
    size_t Delay;

    /*! Automatic advance flag.  This flag is used by the method
      AdvanceIfAutomatic to decide if this state table should advance
      or not.  The method AdvanceIsAutomatic is used by mtsTask on all
      the registered state tables.  If a user wishes to Advance the
      state table manually, he or she will have to set this flag to
      false (see SetAutomaticAdvance).  This flag is set to true by
      default. */
    bool AutomaticAdvanceFlag;

    /*! The vector contains pointers to individual columns. */
    std::vector<StateArrayBase*> StateVector;

    /*! The vector contains pointers to the current values
      of elements that are to be added to the state when we
      advance.
      */
    std::vector<std::any> StateVectorElements;

    /*! The columns entries can be accessed by name. This vector
      stores the names corresponding to the columns. */
    std::vector<std::string> StateVectorDataNames;

    /*! The vector contains pointers to the accessor methods
      (e.g., Get, GetLatest) from which command objects are created. */
    std::vector<AccessorBase *> StateVectorAccessors;

    /*! The vector contains the time stamp in counts or ticks per
      period of the task that the state table is associated with. */
    std::vector<StateIndex::TimeTicksType> Ticks;

    /*! The state table indices for Tic, Toc, and Period. */
    StateDataId TicId, TocId;
    StateDataId PeriodId;

    /*! The time server used to provide absolute and relative times. */
    //const TimeServer* ClockServer;
    TimeServer* ClockServer;


 public:

    /* The start/end times for the current row of data. */
    double Tic, Toc;

    /*! The measured task period (difference between current Tic and
        previous Tic). */
    double Period;

   /*! Periodicist Statistics */
//    mtsIntervalStatistics PeriodStats;

 protected:
    /*! The sum of all the periods (time differences between
        consecutive Tic values); used to compute average period. */
    double SumOfPeriods;

    /*! The average period over the last HistoryLength samples. */
    double AveragePeriod;

    /*! The name of this state table. */
    std::string Name;

    /*! Write specified data. */
    bool Write(StateDataId id, const std::any & pData);


 public:
    /*! Constructor. Constructs a state table with a default
      size of 256 rows. */
    StateTable(size_t size, const std::string & name);

    /*! Default destructor. */
    ~StateTable();

    /*! Method to change the size of the table*/
    bool SetSize(const size_t size);

    /*! Get a handle for data to be used by a reader.  All the const
      methods, that can be called from a reader and writer. */
   StateIndex GetIndexReader(void) const;

    inline void GetIndexReader(StateIndex & timeIndex) const {
        timeIndex = GetIndexReader();
    }

    /*! Get a handle for data to be used by a reader with a given
      delay.  All the const methods, that can be called from a reader
      and writer. */
    StateIndex GetIndexDelayed(void) const;

    /*! Set delay in number of rows. */
    size_t SetDelay(size_t newDelay);

    /*! Verifies if the data is valid. */
    inline bool ValidateReadIndex(const StateIndex &timeIndex) const {
        return (Ticks[timeIndex.Index()] == timeIndex.Ticks());
    }

    /*! Get method for auto advance flag. See AutomaticAdvanceFlag */
    inline const bool & AutomaticAdvance(void) const {
        return this->AutomaticAdvanceFlag;
    }

    /*! Set method for auto advance flag.  See AutoAdvanceFlag. */
    inline void SetAutomaticAdvance(bool automaticAdvance) {
        this->AutomaticAdvanceFlag = automaticAdvance;
    }

//    /*! Check if the signal has been registered. */
//    int GetStateVectorID(const std::string & dataName) const;

    /*! Add an element to the table. Should be called during startup
      of a real time loop.  All the non-const methods, that can be
      called from a writer only. Returns index of data within state
      data table. */
    template <class _elementType>
    StateDataId NewElement(const std::string & name = "", _elementType * element = 0);

    /*! Add an element to the table (alternative to NewElement). */
    template <class _elementType>
    void AddData(_elementType & element, const std::string & name = "") {
        NewElement(name, &element);
    }

//    /*! Return pointer to the state data element specified by the id.
//      This element is the same type as the state data table entry. */
//    template <class _elementType>
//    _elementType * GetStateDataElement(StateDataId id) const {
//        return StateVectorElements[id]; // WEIRD???
//    }


//    std::any GetStateVectorElement(size_t id) const {
//        return StateVectorElements[id];
//    }

    /*! Return pointer to accessor functions for the state data element.
      \param element Pointer to state data element (i.e., working copy)
      \returns Pointer to accessor class (0 if not found)
    */
    template<class _elementType>
    StateTable::AccessorBase * GetAccessorByInstance(const _elementType & element) const;

    /*! Return pointer to accessor functions for the state data element.
      \param name Name of state data element
      \returns Pointer to accessor class (0 if not found)
    */
    //@{
    StateTable::AccessorBase * GetAccessorByName(const std::string & name) const;
    StateTable::AccessorBase * GetAccessorByName(const char * name) const;
    //@}

    /*! Return pointer to accessor functions for the state data element.
      \param id Id of state data element
      \returns Pointer to accessor class (0 if not found)
    */
    StateTable::AccessorBase * GetAccessorById(const size_t id) const;

    /*! Get a handle for data to be used by a writer */
    StateIndex GetIndexWriter(void) const;

    /*! Start the current cycle. This just records the starting timestamp (Tic). */
    void Start(void);

    /*! Start if automatic advance is set and does nothing otherwise. */
    void StartIfAutomatic(void);

    /*! Check if state table is "started", i.e. between Start() and Advance() calls. */
    inline bool Started(void) const {
        return mStarted;
    }

    /*! Advance the pointers of the circular buffer. Note that since
      there is only a single writer, it is not necessary to use mutual
      exclusion primitives; the critical section can be handled by
      updating (incrementing) the write index before the read index.
    */
    void Advance(void);

    /*! Advance if automatic advance is set and does nothing otherwise. */
    void AdvanceIfAutomatic(void);

    /*! Advance for replay mode, be very careful this should only be
      used in replay mode as this method only increments the reader
      index. */
    bool ReplayAdvance(void);

    /*! Cleanup called when the task is being stopped. */
    //void Cleanup(void);

    inline double GetTic(void) const {
        return this->Tic;//.Data;
    }

    inline double GetToc(void) const {
        return this->Toc;//.Data;
    }

    inline size_t GetHistoryLength(void) const {
        return this->HistoryLength;
    }

    inline size_t GetNumberOfElements(void) const {
        return this->StateVector.size();
    }

    /*! Return the moving average of the measured period (i.e., average of last
      HistoryLength values). */
    inline double GetAveragePeriod(void) const {
        return AveragePeriod;
    }

    /*! For debugging, dumps the current data table to output stream. */
    void ToStream(std::ostream & out) const;

//    /*! For debugging, dumps some values of the current data table to
//      output stream. */
//    void Debug(std::ostream & out, unsigned int * listColumn, unsigned int number) const;

//    /*! This method is to dump the state data table in the csv format,
//        allowing easy import into matlab.
//        Assumes that individual columns are also printed in csv format.
//     By default print all rows, if nonZeroOnly == true then print only those rows which have a nonzero Ticks
//     value i.e, those rows that have been written to at least once.
//     */
//    void CSVWrite(std::ostream & out, bool nonZeroOnly = false);
//    void CSVWrite(std::ostream & out, unsigned int * listColumn, unsigned int number, bool nonZeroOnly = false);

//    void CSVWrite(std::ostream & out, mtsGenericObject ** listColumn, unsigned int number, bool nonZeroOnly = false);

    /*! Return the name of this state table. */
    inline const std::string & GetName(void) const { return Name; }
};


// CKim - Add new element to the State Table and returns the id of the column
// Adding a new column (whose length is HistoryLength, name is 'name' and stores '_elementType' data) to the table,
// The pointer to this column is stored in 'StateVector'. 'Name' is stored in 'StateVectorDataNames'
// 'Accessor' to read data of each column is stored in 'StateVectorAccessors' and
// Pointer to the data that will be recorded to each column are casted inton std::any and stored in
// 'StateVectorElements'
template <class _elementType>
StateDataId StateTable::NewElement(const std::string & name, _elementType* pElement) {
//    typedef typename mtsGenericTypes<_elementType>::FinalType FinalType;
//    typedef typename mtsGenericTypes<_elementType>::FinalRefType FinalRefType;
//    mtsStateArray<FinalType> * elementHistory =
//        new mtsStateArray<FinalType>(*element, HistoryLength);

    // CKim - 'StateVectorDataName' stores the name of the column
    StateVectorDataNames.push_back(name);

    // CKim - Create a column vector storing '_elementType' data and 'HistoryLength' long
    StateArray<_elementType> * elementHistory = new StateArray<_elementType>(*pElement, HistoryLength);
    StateVector.push_back(elementHistory);

    // CKim - Assign Id of the column
    StateDataId id = static_cast<StateDataId>(StateVector.size() - 1);

    // CKim - 'StateVectorAccessors' store the 'Accessor' of the added column
    AccessorBase * accessor = new Accessor<_elementType>(*this, id, elementHistory, pElement);
    StateVectorAccessors.push_back(accessor);

    // CKim - 'StateVectorElement' is a row of the StateTable, which will
    // consists of pointers to different data type, and the data will be
    // written to the table when 'Advance' is called.
    // Original implementation in CISST library used vector of 'mtsGenericObject*',
    // in this implementation, we will use std::any introduced in C++17
    // set(CMAKE_CXX_STANDARD 17) / set(CMAKE_CXX_STANDARD_REQUIRED ON)
    // in CMake to include C++ 17 lbraries
    // https://en.cppreference.com/w/cpp/utility/any/type
    // FinalRefType *pdata = mtsGenericTypes<_elementType>::ConditionalWrap(*element);
    // StateVectorElements.push_back(pdata);
    // Pointer to the state variable will be converted to std::any
    StateVectorElements.push_back(pElement);


    return id;
}

#endif // STATETABLE_H

