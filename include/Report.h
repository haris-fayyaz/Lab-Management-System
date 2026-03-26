#ifndef REPORT_H
#define REPORT_H

#include "Common.h"
#include "Repositories.h"
#include <map>

// Abstract Report class (Template Method Pattern)
class Report {
protected:
    string reportContent;
    
    // Template method steps
    virtual void collectData() = 0;
    virtual void computeTotals() = 0;
    virtual void format() = 0;
    
    void save(const string& filename) {
        FileManager* fm = FileManager::getInstance();
        string filepath = fm->getFilePath(filename);
        ofstream file(filepath);
        file << reportContent;
        file.close();
        cout << "Report saved to: " << filepath << endl;
    }
    
public:
    virtual ~Report() {}
    
    // Template Method
    void generate(const string& filename = "report.txt") {
        reportContent.clear();
        collectData();
        computeTotals();
        format();
        save(filename);
        display();
    }
    
    void display() {
        cout << "\n" << reportContent << endl;
    }
};

// Weekly Schedule Report
class WeeklyScheduleReport : public Report {
private:
    SectionRepository* sectionRepo;
    LabRepository* labRepo;
    UserRepository* userRepo;
    RoomRepository* roomRepo;
    
    map<int, vector<Section>> scheduleByDay;
    
protected:
    void collectData() override {
        vector<Section> sections = sectionRepo->findAll();
        
        for (const Section& section : sections) {
            int day = section.getSchedule().dayOfWeek;
            scheduleByDay[day].push_back(section);
        }
    }
    
    void computeTotals() override {
        // No totals needed for schedule
    }
    
    void format() override {
        reportContent += "==============================================\n";
        reportContent += "       COMPLETE LAB SCHEDULE (WEEK)          \n";
        reportContent += "==============================================\n\n";
        
        string days[] = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        
        for (int day = 1; day <= 7; day++) {
            reportContent += "\n--- " + days[day] + " ---\n";
            
            if (scheduleByDay.find(day) == scheduleByDay.end() || scheduleByDay[day].empty()) {
                reportContent += "  No labs scheduled\n";
                continue;
            }
            
            for (const Section& section : scheduleByDay[day]) {
                Lab lab;
                User* instructor = nullptr;
                Room room;
                
                labRepo->findById(section.getLabId(), lab);
                userRepo->findById(section.getInstructorId(), instructor);
                roomRepo->findById(section.getRoomId(), room);
                
                reportContent += "  Lab: " + lab.getCode() + " - " + lab.getName() + "\n";
                reportContent += "  Section: " + section.getSectionName() + "\n";
                reportContent += "  Time: " + section.getSchedule().startTime.toString() + 
                               " - " + section.getSchedule().endTime.toString() + "\n";
                reportContent += "  Room: " + room.getRoomNumber() + "\n";
                if (instructor) {
                    reportContent += "  Instructor: " + instructor->getName() + "\n";
                }
                reportContent += "  ---\n";
            }
        }
    }
    
public:
    WeeklyScheduleReport(SectionRepository* sr, LabRepository* lr, 
                        UserRepository* ur, RoomRepository* rr)
        : sectionRepo(sr), labRepo(lr), userRepo(ur), roomRepo(rr) {}
};

// Weekly Timesheet Report
class WeeklyTimesheetReport : public Report {
private:
    SectionRepository* sectionRepo;
    LabRepository* labRepo;
    TimesheetRepository* timesheetRepo;
    
    vector<TimesheetEntry> weekEntries;
    int totalLabs;
    int completedLabs;
    
protected:
    void collectData() override {
        weekEntries = timesheetRepo->findAll();
        // In real system, filter by week date range
    }
    
    void computeTotals() override {
        totalLabs = weekEntries.size();
        completedLabs = 0;
        
        for (const TimesheetEntry& entry : weekEntries) {
            if (entry.getIsCompleted()) {
                completedLabs++;
            }
        }
    }
    
    void format() override {
        reportContent += "==============================================\n";
        reportContent += "       WEEKLY TIMESHEET REPORT               \n";
        reportContent += "==============================================\n\n";
        
        reportContent += "Total Labs Scheduled: " + to_string(totalLabs) + "\n";
        reportContent += "Labs Completed: " + to_string(completedLabs) + "\n";
        reportContent += "Labs Pending: " + to_string(totalLabs - completedLabs) + "\n\n";
        
        reportContent += "Details:\n";
        reportContent += "--------\n";
        
        for (const TimesheetEntry& entry : weekEntries) {
            Section section;
            Lab lab;
            
            sectionRepo->findById(entry.getSectionId(), section);
            labRepo->findById(section.getLabId(), lab);
            
            reportContent += "\nLab: " + lab.getCode() + " - " + section.getSectionName() + "\n";
            reportContent += "Date: " + entry.getDate().toString() + "\n";
            
            if (entry.getIsCompleted()) {
                reportContent += "Status: Completed\n";
                reportContent += "Actual Time: " + entry.getActualStartTime().toString() + 
                               " - " + entry.getActualEndTime().toString() + "\n";
                reportContent += "Contact Hours: " + to_string(entry.getContactHours()) + "\n";
                if (entry.getIsMakeup()) {
                    reportContent += "(Makeup Lab)\n";
                }
            } else {
                reportContent += "Status: Pending\n";
            }
        }
    }
    
public:
    WeeklyTimesheetReport(SectionRepository* sr, LabRepository* lr, TimesheetRepository* tr)
        : sectionRepo(sr), labRepo(lr), timesheetRepo(tr), totalLabs(0), completedLabs(0) {}
};

// Semester Contact Hours Report
class SemesterContactHoursReport : public Report {
private:
    SectionRepository* sectionRepo;
    LabRepository* labRepo;
    TimesheetRepository* timesheetRepo;
    
    int sectionId;
    vector<TimesheetEntry> sectionEntries;
    int totalContactHours;
    int totalLeaves;
    
protected:
    void collectData() override {
        sectionEntries = timesheetRepo->findBySectionId(sectionId);
    }
    
    void computeTotals() override {
        totalContactHours = 0;
        totalLeaves = 0;
        
        for (const TimesheetEntry& entry : sectionEntries) {
            if (entry.getIsCompleted()) {
                totalContactHours += entry.getContactHours();
            } else {
                totalLeaves++;
            }
        }
    }
    
    void format() override {
        Section section;
        Lab lab;
        
        sectionRepo->findById(sectionId, section);
        labRepo->findById(section.getLabId(), lab);
        
        reportContent += "==============================================\n";
        reportContent += "    SEMESTER CONTACT HOURS REPORT            \n";
        reportContent += "==============================================\n\n";
        
        reportContent += "Lab: " + lab.getCode() + " - " + lab.getName() + "\n";
        reportContent += "Section: " + section.getSectionName() + "\n\n";
        
        reportContent += "Total Sessions: " + to_string(sectionEntries.size()) + "\n";
        reportContent += "Total Contact Hours: " + to_string(totalContactHours) + "\n";
        reportContent += "Total Leaves: " + to_string(totalLeaves) + "\n\n";
        
        reportContent += "Session Details:\n";
        reportContent += "----------------\n";
        
        for (const TimesheetEntry& entry : sectionEntries) {
            reportContent += "\nDate: " + entry.getDate().toString() + "\n";
            
            if (entry.getIsCompleted()) {
                reportContent += "Time: " + entry.getActualStartTime().toString() + 
                               " - " + entry.getActualEndTime().toString() + "\n";
                reportContent += "Hours: " + to_string(entry.getContactHours()) + "\n";
                if (entry.getIsMakeup()) {
                    reportContent += "(Makeup Lab)\n";
                }
            } else {
                reportContent += "Status: Leave/Absent\n";
            }
        }
    }
    
public:
    SemesterContactHoursReport(SectionRepository* sr, LabRepository* lr, TimesheetRepository* tr)
        : sectionRepo(sr), labRepo(lr), timesheetRepo(tr), 
          sectionId(0), totalContactHours(0), totalLeaves(0) {}
    
    void setSectionId(int id) { sectionId = id; }
};

#endif
