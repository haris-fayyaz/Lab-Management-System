#ifndef TIMESHEET_H
#define TIMESHEET_H

#include "Common.h"
#include "Observer.h"

// Timesheet Entry
class TimesheetEntry : public Subject {
private:
    int id;
    int sectionId;
    Date date;
    Time actualStartTime;
    Time actualEndTime;
    bool isCompleted;
    bool isMakeup;
    
public:
    TimesheetEntry() : id(0), sectionId(0), isCompleted(false), isMakeup(false) {}
    TimesheetEntry(int id, int secId, Date dt) 
        : id(id), sectionId(secId), date(dt), isCompleted(false), isMakeup(false) {}
    
    int getId() const { return id; }
    int getSectionId() const { return sectionId; }
    Date getDate() const { return date; }
    Time getActualStartTime() const { return actualStartTime; }
    Time getActualEndTime() const { return actualEndTime; }
    bool getIsCompleted() const { return isCompleted; }
    bool getIsMakeup() const { return isMakeup; }
    
    void setActualTiming(Time start, Time end) {
        actualStartTime = start;
        actualEndTime = end;
        isCompleted = true;
        
        // Notify observers (HOD, Instructor)
        notify("Timesheet filled for Section " + to_string(sectionId) + 
               " on " + date.toString());
    }
    
    void setAsMakeup() { isMakeup = true; }
    
    int getContactHours() const {
        if (!isCompleted) return 0;
        int totalMinutes = (actualEndTime.hour * 60 + actualEndTime.minute) - 
                          (actualStartTime.hour * 60 + actualStartTime.minute);
        return totalMinutes / 60;
    }
    
    void serialize(ofstream& out) const {
        out.write((char*)&id, sizeof(id));
        out.write((char*)&sectionId, sizeof(sectionId));
        date.serialize(out);
        actualStartTime.serialize(out);
        actualEndTime.serialize(out);
        out.write((char*)&isCompleted, sizeof(isCompleted));
        out.write((char*)&isMakeup, sizeof(isMakeup));
    }
    
    void deserialize(ifstream& in) {
        in.read((char*)&id, sizeof(id));
        in.read((char*)&sectionId, sizeof(sectionId));
        date.deserialize(in);
        actualStartTime.deserialize(in);
        actualEndTime.deserialize(in);
        in.read((char*)&isCompleted, sizeof(isCompleted));
        in.read((char*)&isMakeup, sizeof(isMakeup));
    }
};

#endif
