#ifndef FACADE_H
#define FACADE_H

#include "Repositories.h"
#include "Factory.h"
#include "Report.h"
#include "Observer.h"

// Academic Officer Facade
class AcademicOfficeFacade {
private:
// These are the "offices" this facade talks to behind the scenes:
    LabRepository* labRepo;      // Knows how to save and find Labs
    SectionRepository* sectionRepo;  // Knows how to save and find specific class Sections
    UserRepository* userRepo;    // Knows how to save and find Instructors and TAs
    RoomRepository* roomRepo;    // Knows about the physical Room assignments
    
public:
    AcademicOfficeFacade(LabRepository* lr, SectionRepository* sr, 
                        UserRepository* ur, RoomRepository* rr)
        : labRepo(lr), sectionRepo(sr), userRepo(ur), roomRepo(rr) {}
    
    void createLab(int id, const string& code, const string& name) {
        Lab lab(id, code, name);
        labRepo->save(lab);
        cout << "Lab created successfully: " << code << endl;
    }
    
    void createSection(int id, int labId, const string& sectionName, 
                      int roomId, int instructorId) {
        Section section(id, labId, sectionName, roomId, instructorId);
        sectionRepo->save(section);
        cout << "Section created successfully: " << sectionName << endl;
    }
    // This is a multi-step process hidden from the user:
    // 1. Find the existing Section.
    // 2. Update the Section object with the new schedule.
    // 3. Tell the SectionRepository to **update** the record.
    void assignSchedule(int sectionId, int dayOfWeek, Time start, Time end) {
        Section section;
        if (sectionRepo->findById(sectionId, section)) {
            Schedule schedule(dayOfWeek, start, end);
            section.setSchedule(schedule);
            sectionRepo->update(section);
            cout << "Schedule assigned successfully" << endl;
        } else {
            cout << "Section not found!" << endl;
        }
    }
    // Assign a Teaching Assistant (TA)
    void assignTA(int sectionId, int taId) {
        Section section;
        if (sectionRepo->findById(sectionId, section)) {
            section.addTA(taId);
            sectionRepo->update(section);
            cout << "TA assigned successfully" << endl;
        } else {
            cout << "Section not found!" << endl;
        }
    }
    // It just asks the LabRepository for everything and prints it
    void listAllLabs() {
        vector<Lab> labs = labRepo->findAll();
        cout << "\n=== All Labs ===" << endl;
        for (const Lab& lab : labs) {
            cout << "ID: " << lab.getId() << " | Code: " << lab.getCode() 
                 << " | Name: " << lab.getName() << endl;
        }
    }
    // It's slightly more complex: it gets all the Sections, but then has to ask
    // the LabRepository what the Lab's actual code is to make the output readable.
    void listAllSections() {
        vector<Section> sections = sectionRepo->findAll();
        cout << "\n=== All Sections ===" << endl;
        for (const Section& section : sections) {
            Lab lab;
            labRepo->findById(section.getLabId(), lab);
            cout << "ID: " << section.getId() << " | Lab: " << lab.getCode() 
                 << " | Section: " << section.getSectionName() << endl;
        }
    }
};

// Attendant Facade
class AttendantFacade {
private:
    TimesheetRepository* timesheetRepo;
    SectionRepository* sectionRepo;
    
public:
    AttendantFacade(TimesheetRepository* tr, SectionRepository* sr)
        : timesheetRepo(tr), sectionRepo(sr) {}
    
    void createTimesheetEntry(int id, int sectionId, Date date) {
        TimesheetEntry entry(id, sectionId, date);
        timesheetRepo->save(entry);
        cout << "Timesheet entry created for Section " << sectionId << endl;
    }
    
    void fillTimesheet(int entryId, Time actualStart, Time actualEnd, 
                      Observer* hod, Observer* instructor) {
        TimesheetEntry entry;
        if (timesheetRepo->findById(entryId, entry)) {
            // Attach observers
            entry.attach(hod);
            entry.attach(instructor);
            
            // Fill timesheet (this will notify observers)
            entry.setActualTiming(actualStart, actualEnd);
            timesheetRepo->update(entry);
            
            cout << "Timesheet filled successfully" << endl;
        } else {
            cout << "Timesheet entry not found!" << endl;
        }
    }
    
    void recordMakeupLab(int entryId, Time actualStart, Time actualEnd,
                        Observer* hod, Observer* instructor) {
        TimesheetEntry entry;
        if (timesheetRepo->findById(entryId, entry)) {
            entry.setAsMakeup();
            
            // Attach observers
            entry.attach(hod);
            entry.attach(instructor);
            
            entry.setActualTiming(actualStart, actualEnd);
            timesheetRepo->update(entry);
            
            cout << "Makeup lab recorded successfully" << endl;
        } else {
            cout << "Timesheet entry not found!" << endl;
        }
    }
    
    void listPendingTimesheets() {
        vector<TimesheetEntry> entries = timesheetRepo->findAll();
        cout << "\n=== Pending Timesheets ===" << endl;
        bool found = false;
        
        for (const TimesheetEntry& entry : entries) {
            if (!entry.getIsCompleted()) {
                Section section;
                sectionRepo->findById(entry.getSectionId(), section);
                cout << "Entry ID: " << entry.getId() 
                     << " | Section: " << section.getSectionName()
                     << " | Date: " << entry.getDate().toString() << endl;
                found = true;
            }
        }
        
        if (!found) {
            cout << "No pending timesheets" << endl;
        }
    }
};

// HOD Facade
class HODFacade {
private:
    // These are the "offices" this facade talks to behind the scenes:
    SectionRepository* sectionRepo;      // Data about class sections
    LabRepository* labRepo;              // Data about which Labs are offered
    UserRepository* userRepo;            // Data about faculty/TAs
    RoomRepository* roomRepo;            // Data about room assignments
    TimesheetRepository* timesheetRepo;  // Data about recorded working hours
    
public:
    HODFacade(SectionRepository* sr, LabRepository* lr, UserRepository* ur,
             RoomRepository* rr, TimesheetRepository* tr)
        : sectionRepo(sr), labRepo(lr), userRepo(ur), roomRepo(rr), timesheetRepo(tr) {}

    // 1. It asks the **ReportFactory** to create the specific Weekly Schedule report object.
    // 2. It passes *all* the necessary repositories to the report object.
    // 3. It tells the report object to generate the file.
    // 4. It cleans up the report object when done.
    void generateWeeklySchedule() {
        Report* report = ReportFactory::createReport(
            ReportFactory::WEEKLY_SCHEDULE,
            sectionRepo, labRepo, userRepo, roomRepo, timesheetRepo
        );
        
        report->generate("weekly_schedule.txt");
        delete report;
    }
    // It follows the same convenient Factory pattern as the schedule report
    void generateWeeklyTimesheet() {
        Report* report = ReportFactory::createReport(
            ReportFactory::WEEKLY_TIMESHEET,
            sectionRepo, labRepo, userRepo, roomRepo, timesheetRepo
        );
        
        report->generate("weekly_timesheet.txt");
        delete report;
    }
    // This report needs an extra piece of information: the specific "sectionId"
    void generateSemesterContactHours(int sectionId) {
        SemesterContactHoursReport* report = 
            (SemesterContactHoursReport*)ReportFactory::createReport(
                ReportFactory::SEMESTER_CONTACT_HOURS,
                sectionRepo, labRepo, userRepo, roomRepo, timesheetRepo
            );
        report->setSectionId(sectionId);
        report->generate("semester_contact_hours.txt");
        delete report;
    }
};

// Instructor Facade
class InstructorFacade {
private:
    // Dependencies: The backend data stores
    TimesheetRepository* timesheetRepo; // Needed if the instructor logs hours (though not used here)
    SectionRepository* sectionRepo;     // Needed to find and show the instructor's assigned sections

    // The link to the notification system for makeup requests (Subject/Observable role)
    Subject* makeupScheduler;
    
public:
    InstructorFacade(TimesheetRepository* tr, SectionRepository* sr)
        : timesheetRepo(tr), sectionRepo(sr), makeupScheduler(nullptr) {}
    // his method is used to connect the facade to the actual notification
    // system object (the **Subject**) which will handle the makeup requests.
    void setMakeupScheduler(Subject* scheduler) {
        makeupScheduler = scheduler;
    }
    // This method is the facade's key responsibility regarding communication.
    // It sets up the **Observer Pattern** on the fly:
    // 1. It 'attaches' the attendant and HOD as **Observers** (recipients).
    // 2. It tells the `makeupScheduler` (the **Subject**) to **notify** all observers.
    void requestMakeupLab(int sectionId, Date date, Observer* attendant, Observer* hod) {
        cout << "\nMakeup lab request submitted for Section " << sectionId 
             << " on " << date.toString() << endl;
        
        if (makeupScheduler) {
            if (attendant) makeupScheduler->attach(attendant);
            if (hod) makeupScheduler->attach(hod);
            makeupScheduler->notify("Makeup lab requested for Section " + 
                                   to_string(sectionId) + " on " + date.toString());
        }
    }
    // Displays the current class schedule for the given instructor.
    // This encapsulates the data retrieval logic from the SectionRepository
    void viewSchedule(int instructorId) {
        vector<Section> sections = sectionRepo->findAll();
        cout << "\n=== Your Lab Schedule ===" << endl;
        
        string days[] = {"", "Monday", "Tuesday", "Wednesday", "Thursday", 
                        "Friday", "Saturday", "Sunday"};
        
        for (const Section& section : sections) {
            if (section.getInstructorId() == instructorId) {
                cout << "\nSection: " << section.getSectionName() << endl;
                cout << "Day: " << days[section.getSchedule().dayOfWeek] << endl;
                cout << "Time: " << section.getSchedule().startTime.toString() 
                     << " - " << section.getSchedule().endTime.toString() << endl;
            }
        }
    }
};

#endif