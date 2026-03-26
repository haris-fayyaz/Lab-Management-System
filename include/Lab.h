#ifndef LAB_H
#define LAB_H

#include "Common.h"
#include <vector>

// Lab class
class Lab {
private:
    int id;
    string code;
    string name;
    
public:
    Lab() : id(0), code(""), name("") {}
    Lab(int id, const string& code, const string& name) 
        : id(id), code(code), name(name) {}
    
    int getId() const { return id; }
    string getCode() const { return code; }
    string getName() const { return name; }
    
    void serialize(ofstream& out) const {
        out.write((char*)&id, sizeof(id));
        StringHelper::writeString(out, code);
        StringHelper::writeString(out, name);
    }
    
    void deserialize(ifstream& in) {
        in.read((char*)&id, sizeof(id));
        code = StringHelper::readString(in);
        name = StringHelper::readString(in);
    }
};

// Schedule structure
struct Schedule {
    int dayOfWeek;  // 1=Monday, 7=Sunday
    Time startTime;
    Time endTime;
    
    Schedule() : dayOfWeek(1) {}
    Schedule(int day, Time start, Time end) 
        : dayOfWeek(day), startTime(start), endTime(end) {}
    
    void serialize(ofstream& out) const {
        out.write((char*)&dayOfWeek, sizeof(dayOfWeek));
        startTime.serialize(out);
        endTime.serialize(out);
    }
    
    void deserialize(ifstream& in) {
        in.read((char*)&dayOfWeek, sizeof(dayOfWeek));
        startTime.deserialize(in);
        endTime.deserialize(in);
    }
};

// Section class
class Section {
private:
// Core Identity and Relationships
    int id;               // Unique ID for this specific Section (e.g., 101)
    int labId;            // The ID of the parent Lab (e.g., the ID for "CS-101L")
    string sectionName;   // The name of the section (e.g., "A", "B", "Wednesday Morning")
    
    // Resource Assignments
    int roomId;           // The ID of the physical Room where this section meets
    int instructorId;     // The ID of the main Instructor assigned to teach this section
    vector<int> taIds;    // A list of User IDs for all Teaching Assistants (TAs) assigned
    Schedule schedule;    // The actual meeting time data (day, start time, end time)
    
public:
    Section() : id(0), labId(0), sectionName(""), roomId(0), instructorId(0) {}
    Section(int id, int lId, const string& secName, int rId, int instId) 
        : id(id), labId(lId), sectionName(secName), roomId(rId), instructorId(instId) {}
    
    int getId() const { return id; }
    int getLabId() const { return labId; }
    string getSectionName() const { return sectionName; }
    int getRoomId() const { return roomId; }
    int getInstructorId() const { return instructorId; }
    const vector<int>& getTAIds() const { return taIds; }
    const Schedule& getSchedule() const { return schedule; }
    // Sets the meeting day and time for this section.
    void setSchedule(const Schedule& sch) { schedule = sch; }
    void addTA(int taId) { taIds.push_back(taId); }
    // This process is crucial for saving the object's state to disk (Persistence).
    // It must write the data in a specific order so it can be read back correctly.
    void serialize(ofstream& out) const {
        out.write((char*)&id, sizeof(id));
        out.write((char*)&labId, sizeof(labId));
        StringHelper::writeString(out, sectionName);
        out.write((char*)&roomId, sizeof(roomId));
        out.write((char*)&instructorId, sizeof(instructorId));
        
        size_t taCount = taIds.size();
        out.write((char*)&taCount, sizeof(taCount));
        for (int taId : taIds) {
            out.write((char*)&taId, sizeof(taId));
        }
        
        schedule.serialize(out);
    }
    // This must read the data in the **exact reverse order** it was written by "serialize()"".
    void deserialize(ifstream& in) {
        in.read((char*)&id, sizeof(id));
        in.read((char*)&labId, sizeof(labId));
        sectionName = StringHelper::readString(in);
        in.read((char*)&roomId, sizeof(roomId));
        in.read((char*)&instructorId, sizeof(instructorId));
        
        size_t taCount;
        in.read((char*)&taCount, sizeof(taCount));
        taIds.clear();
        for (size_t i = 0; i < taCount; i++) {
            int taId;
            in.read((char*)&taId, sizeof(taId));
            taIds.push_back(taId);
        }
        
        schedule.deserialize(in);
    }
};

#endif
