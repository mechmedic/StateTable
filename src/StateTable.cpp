/**
 * @file StateTable.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "StateTable.h"
#include <iostream>
#include <string>


StateTable::StateTable(size_t size, const std::string & name):
    HistoryLength(size),
    IndexWriter(0),
    IndexReader(0),
    IndexDelayed(0),
    Delay(0.0),
    AutomaticAdvanceFlag(true),
    StateVector(0),
    StateVectorDataNames(0),
    Ticks(size, StateIndex::TimeTicksType(0)),
    Tic(0.0),
    Toc(0.0),
    Period(0.0),
    SumOfPeriods(0.0),
    AveragePeriod(0.0),
    Name(name)
{
    // make sure history length is at least 3
    if (this->HistoryLength < 3) {
        //CMN_LOG_CLASS_INIT_VERBOSE << "constructor: history lenght sets to 3 (minimum required)" << std::endl;
        this->HistoryLength = 3;
    }

    // Get a pointer to the time server
    //TimeServer = &mtsTaskManager::GetInstance()->GetTimeServer();
    ClockServer = new TimeServer();
    ClockServer->SetTimeOrigin();

    // Add Tic and Toc to the StateTable. We add Toc first, to make things easier
    // in mtsStateTable::Advance.  Do not change this.
    TocId = NewElement("Toc", &Toc);
    TicId = NewElement("Tic", &Tic);
    // Add Period to the StateTable.
    PeriodId = NewElement("Period", &Period);

    // Add statistics
    //NewElement("PeriodStatistics", &PeriodStats);
}

StateTable::~StateTable()
{
}

// CKim - Resize each column (StateVector[j])
bool StateTable::SetSize(const size_t size)
{
    if(this->HistoryLength == size){     return true;       }

    this->HistoryLength = size;

    for (unsigned int j = 0; j < StateVector.size(); j++)  {
        if (StateVector[j]) {
            StateVector[j]->SetSize(this->HistoryLength);
        }
    }
    return true;
}

// CKim - Start Update??? Record time and update period
void StateTable::Start(void)
{
    if (ClockServer)
    {
        Tic = ClockServer->GetRelativeTime(); // in seconds
        // Since IndexReader and IndexWriter are initialized to 0,
        // the first period will be 0
        double oldTic;
        StateVector[TicId]->Get(IndexReader, &oldTic);
        Period = Tic - oldTic;  // in seconds
    }
    // update "started" status
    mStarted = true;
}

void StateTable::StartIfAutomatic(void)
{
    if (this->AutomaticAdvanceFlag) {
        this->Start();
    }
}

// CKim - Save the record and advance writing pointer??
void StateTable::Advance(void) {
    size_t i;
    size_t tmpIndex;
    // newIndexWriter is the next row of the State Table.  Note that this
    // also corresponds to the row with the oldest data.
    size_t newIndexWriter = (IndexWriter + 1) % HistoryLength;

    // Update SumOfPeriods (add newest and subtract oldest)
    SumOfPeriods += Period;
    // If the table is full (all entries valid), subtract the oldest one
    if (Ticks[IndexWriter] == (Ticks[newIndexWriter] + HistoryLength - 1)) {
        std::any oldPeriod;
        StateVector[PeriodId]->Get(newIndexWriter, oldPeriod);
        SumOfPeriods -= std::any_cast<double>(oldPeriod);
        AveragePeriod = SumOfPeriods / (HistoryLength-1);
    }
    else if (Ticks[IndexWriter] > 0) {
        AveragePeriod = SumOfPeriods / Ticks[IndexWriter];
    }

    /* If for all cases, IndexReader is behind IndexWriter, we don't
    need critical sections. This is based on the assumption that
    there is only one Writer that has access to Advance method and
    this is the only place where IndexReader is modified.  Oh ya!
    another assumption, we don't have a buffer of size less than 3.
    */

    /* So far IndexReader < IndexWriter.
    The following block doesn't modify IndexReader,
    so the above condition still holds.
    */
    tmpIndex = IndexWriter;

    // Write data in the state table from the different state data objects.
    // Note that we start at TicId, which should correspond to the second
    // element in the array (after Toc).
    for (i = TicId; i < StateVector.size(); i++) {
        //if (StateVectorElements[i]) {
            //StateVectorElements[i]->SetTimestampIfAutomatic(Tic.Data);
          //  Write(static_cast<StateDataId>(i), *(StateVectorElements[i]));
          Write(static_cast<StateDataId>(i), StateVectorElements[i]);
        //}
    }

    // Get the Toc value and write it to the state table.
    if (ClockServer) {
        Toc = ClockServer->GetRelativeTime(); // in seconds
    }

    // Update Period Statistics (last time interval, current compute time)
    //PeriodStats.Update(Period.Data, this->Toc - this->Tic);

    //Write(TocId, Toc);
    Write(TocId, &Toc);
    // now increment the IndexWriter and set its Tick value
    IndexWriter = newIndexWriter;
    Ticks[IndexWriter] = Ticks[tmpIndex] + 1;
    // move index reader to recently written data
    IndexReader = tmpIndex;

    // compute index delayed, ideally a valid index
    if (IndexReader > Delay) {
        IndexDelayed = IndexReader - Delay;
    }

    // update "started" status
    mStarted = false;
}


void StateTable::AdvanceIfAutomatic(void) {
    if (this->AutomaticAdvanceFlag) {
        this->Advance();
    }
}

// CKim - Writes data in pData at IndexWriter of StateVector[id]
bool StateTable::Write(StateDataId id, const std::any & pData) {
    bool result;
    if (id == -1) {
        std::cout << "Write: object must be created using NewElement " << std::endl;
        return false;
    }
    if (!StateVector[id]) {
        std::cout << "Write: no state data array corresponding to given id: " << id << std::endl;
        return false;
    }
    result = StateVector[id]->Set(IndexWriter, pData);
    if (!result) {
        std::cout << "Write: error setting data array value in id: " << id << std::endl;
    }
    return result;
}


// --------------------------------------------------------------------------- //
/* All the const methods that can be called from reader or writer */
// CKim - What does GetIndex Exactly Do??
StateIndex StateTable::GetIndexReader(void) const {
    size_t tmp = IndexReader;
    return StateIndex(this->Tic, static_cast<int>(tmp), Ticks[tmp], static_cast<int>(HistoryLength));
}

StateIndex StateTable::GetIndexDelayed(void) const {
    size_t tmp = IndexDelayed;
    return StateIndex(this->Tic, static_cast<int>(tmp), Ticks[tmp], static_cast<int>(HistoryLength));
}

size_t StateTable::SetDelay(size_t newDelay) {
    size_t currentDelay = this->Delay;
    this->Delay = newDelay;
    return currentDelay;
}

// CKim - GetAccessor is used to access each element of StateTable
StateTable::AccessorBase * StateTable::GetAccessorByName(const std::string & name) const
{
    for (size_t i = 0; i < StateVectorDataNames.size(); i++) {
        if (name == StateVectorDataNames[i]) {
            return StateVectorAccessors[i];
        }
    }
    return 0;
}

StateTable::AccessorBase * StateTable::GetAccessorByName(const char * name) const
{
    return GetAccessorByName(std::string(name));
}

StateTable::AccessorBase * StateTable::GetAccessorById(const size_t id) const{
    return StateVectorAccessors[id];
}

/* All the non-const methods that can be called from writer only */
StateIndex StateTable::GetIndexWriter(void) const {
    return StateIndex(this->Tic, static_cast<int>(IndexWriter), Ticks[IndexWriter], static_cast<int>(HistoryLength));
}

bool StateTable::ReplayAdvance(void)
{
    IndexReader++;
    if (IndexReader > HistoryLength) {
        IndexReader = 0;
        return false;
    }
    return true;
}

//void StateTable::Cleanup(void) {
//    CMN_LOG_CLASS_INIT_DEBUG << "Cleanup: state table \"" << this->Name << "\"" << std::endl;
//    // if the state table is still set to collect data, send error message, should have been stopped
//    if (this->DataCollection.Collecting) {
//        CMN_LOG_CLASS_INIT_ERROR << "Cleanup: data collection for state table \"" << this->Name
//                                 << "\" has not been stopped.  It is possible that the state collector will look for this state table after it has been deleted." << std::endl;
//    }
//}


void StateTable::ToStream(std::ostream & outputStream) const {
    outputStream << "State Table: " << this->GetName() << std::endl;
    size_t i;
    outputStream << "Ticks : ";
    for (i = 0; i < StateVector.size() - 1; i++) {
        if (!StateVectorDataNames[i].empty())
            outputStream << "[" << i << "]"
                         << StateVectorDataNames[i].c_str() << " : ";
    }
    outputStream << "[" << i << "]"
                 << StateVectorDataNames[i].c_str() << std::endl;
//#if 0
    // the following is a data dump, it should go in ToStreamRaw
    for (i = 0; i < HistoryLength; i++) {
        outputStream << i << ": ";
        outputStream << Ticks[i] << ": ";
        for (unsigned int j = 0; j < StateVector.size(); j++)  {
            if (StateVector[j]) {
                outputStream << " [" << j << "] "
                    << (*StateVector[j])[i]
                    << " : ";
            }
        }
        if (i == IndexReader)
            outputStream << "<-- Index for Reader";
        if (i == IndexWriter)
            outputStream << "<== Index for Writer";
        outputStream << std::endl;
    }
//#endif
}


//void mtsStateTable::Debug(std::ostream & out, unsigned int * listColumn, unsigned int number) const {
//    unsigned int i, j;

//    for (i = 0; i < number; i++) {
//        if (!StateVectorDataNames[listColumn[i]].empty()) {
//            out << "["
//                << listColumn[i] << "]" <<
//                StateVectorDataNames[listColumn[i]].c_str() << " : ";
//        }
//    }
//    out << std::endl;

//    for (i = 0; i < HistoryLength; i++) {
//        out << i << " ";
//        out << Ticks[i] << " ";
//        for (j = 0; j < number; j++) {
//            if (listColumn[j] < StateVector.size() && StateVector[listColumn[j]]) {
//                out << " [" << listColumn[j] << "] "
//                    <<(*StateVector[listColumn[j]])[i] << " : ";
//            }
//        }
//        if (i == IndexReader) {
//            out << "<-- Index for Reader";
//        }
//        if (i == IndexWriter) {
//            out << "<== Index for Writer";
//        }
//        out << std::endl;
//    }
//}

// This method is to dump the state data table in the csv format, allowing easy import into matlab.
// Assumes that individual columns are also printed in csv format.

// By default print all rows, if nonZeroOnly == true then print only those rows which have a nonzero Ticks
// value i.e, those rows that have been written to at least once.
//void mtsStateTable::CSVWrite(std::ostream& out, bool nonZeroOnly) {
//    unsigned int i;
//    for (i = 0; i < HistoryLength; i++) {
//        bool toSave = true;
//        if (nonZeroOnly && Ticks[i] ==0) toSave = false;
//        if (toSave) {
//            out << i << " " << Ticks[i] << " ";
//            for (unsigned int j = 0; j < StateVector.size(); j++)  {
//                if (StateVector[j]) {
//                    out << (*StateVector[j])[i] << " ";
//                }
//            }
//            out << std::endl;
//        }
//    }
//}

//void mtsStateTable::CSVWrite(std::ostream& out, unsigned int *listColumn, unsigned int number, bool nonZeroOnly) {
//    unsigned int i, j;

//    for (i = 0; i < HistoryLength; i++) {
//        bool toSave = true;
//        if (nonZeroOnly && Ticks[i] ==0) toSave = false;
//        if (toSave) {
//            out << i << " " << Ticks[i] << " ";
//            for (j = 0; j < number; j++) {
//                if (listColumn[j] < StateVector.size() && StateVector[listColumn[j]]) {
//                    out << (*StateVector[listColumn[j]])[i] << " ";
//                }
//            }
//            out << std::endl;
//        }
//    }
//}

//void mtsStateTable::CSVWrite(std::ostream& out, mtsGenericObject ** listColumn, unsigned int number, bool nonZeroOnly)
//{
//    unsigned int *listColumnId = new unsigned int[number];
//    for (unsigned int i = 0; i < number; i++) {
//        listColumnId[i] = static_cast<unsigned int>(StateVectorElements.size());  // init to invalid value
//        for (unsigned int j = 0; j < StateVectorElements.size(); j++) {
//            if (StateVectorElements[j] == listColumn[i])
//                listColumnId[i] = j;
//        }
//    }
//    CSVWrite(out, listColumnId, number, nonZeroOnly);
//    delete [] listColumnId;
//}


//int StateTable::GetStateVectorID(const std::string & dataName) const
//{
//    for (size_t i = 0; i < StateVectorDataNames.size(); i++) {
//        if (StateVectorDataNames[i] == dataName) {
//            return static_cast<int>(i);
//        }
//    }
//    return -1;
//}


//void mtsStateTable::DataCollectionStart(const mtsDouble & delay)
//{
//    CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStart: received request to start data collection in "
//                            << delay.Data << " seconds" << std::endl;
//    // set start time based on current time and delay
//    const double startTime = this->TimeServer->GetRelativeTime() + delay.Data;
//    // if we are not yet collection
//    if (!this->DataCollection.Collecting) {
//        // if there is no collection scheduled
//        if (this->DataCollection.StartTime == 0) {
//            // set time to start
//            CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStart: data collection scheduled to start at "
//                                    << startTime << " for state table \"" << this->GetName() << "\"" << std::endl;
//            this->DataCollection.StartTime = startTime;
//        } else {
//            // we are set to collect but later, advance the collection
//            // time.  this is a conservative approach, if we have 2
//            // different start times, we take the earliest of both to
//            // collect more data.
//            if (this->DataCollection.StartTime > startTime) {
//                CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStart: data collection scheduled to start at "
//                                        << startTime << " (moved forward) for state table \"" << this->GetName() << "\"" << std::endl;
//                this->DataCollection.StartTime = startTime;
//            } else {
//                CMN_LOG_CLASS_RUN_WARNING << "DataCollectionStart: received a new request to start data collection after previous request, ignored"
//                                          << std::endl;
//            }
//        }
//    } else {
//        // we are already collecting, see if a time to stop has been set
//        if (this->DataCollection.StopTime == 0) {
//            CMN_LOG_CLASS_RUN_WARNING << "DataCollectionStart: received request to start collection while still set to collect, ignored" << std::endl;
//        } else {
//            // make sure the request to start comes after the next scheduled stop
//            if (startTime <= this->DataCollection.StartTime) {
//                CMN_LOG_CLASS_RUN_WARNING << "DataCollectionStart: received request to start collection before next scheduled stop, ignored" << std::endl;
//            } else {
//                // this will schedule a start after the scheduled stop
//                CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStart: data collection scheduled to start at "
//                                        << startTime << " (after a scheduled stop) for state table \""
//                                        << this->GetName() << "\"" << std::endl;
//                this->DataCollection.StartTime = startTime;
//            }
//        }
//    }
//}


//void mtsStateTable::DataCollectionStop(const mtsDouble & delay)
//{
//    CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStop: received request to stop data collection in "
//                            << delay.Data << " seconds for state table \""
//                            << this->GetName() << "\"" << std::endl;
//    // set stop time based on current time and delay
//    const double stopTime = this->TimeServer->GetRelativeTime() + delay.Data;
//    // check is there is already a stop time scheduled
//    if (this->DataCollection.StopTime == 0) {
//        CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStop: data collection scheduled to stop at "
//                                << stopTime << " for state table \""
//                                << this->GetName() << "\"" << std::endl;
//        DataCollection.StopTime = stopTime;
//    } else {
//        // we are set to stop but earlier, delay the collection stop
//        // time.  this is a conservative approach, if we have 2
//        // different stop times, we take the latest of both to collect
//        // more data.
//        if (this->DataCollection.StopTime < stopTime) {
//            CMN_LOG_CLASS_RUN_DEBUG << "DataCollectionStop: data collection scheduled to stop at "
//                                    << stopTime << " (moved back) for state table \""
//                                    << this->GetName() << "\"" << std::endl;
//            DataCollection.StopTime = stopTime;
//        } else {
//            CMN_LOG_CLASS_RUN_WARNING << "DataCollectionStop: received a new request to stop data collection before previous request, ignored"
//                                      << std::endl;
//        }
//    }
//}
