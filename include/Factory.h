#ifndef FACTORY_H
#define FACTORY_H

#include "User.h"
#include "Report.h"

// User Factory (Factory Method Pattern)
class UserFactory {
public:
    static User* createUser(UserRole role, int id, const string& name, int buildingId = 0) {
        switch (role) {
            case UserRole::INSTRUCTOR:
                return new Instructor(id, name);
            case UserRole::TA:
                return new TA(id, name);
            case UserRole::ATTENDANT:
                return new Attendant(id, name, buildingId);
            case UserRole::ACADEMIC_OFFICER:
                return new AcademicOfficer(id, name);
            case UserRole::HOD:
                return new HOD(id, name);
            default:
                return nullptr;
        }
    }
};

// Report Factory (Factory Method Pattern)
class ReportFactory {
public:
    enum ReportType {
        WEEKLY_SCHEDULE,
        WEEKLY_TIMESHEET,
        SEMESTER_CONTACT_HOURS
    };
    
    static Report* createReport(ReportType type, 
                               SectionRepository* secRepo,
                               LabRepository* labRepo,
                               UserRepository* userRepo,
                               RoomRepository* roomRepo,
                               TimesheetRepository* tsRepo) {
        switch (type) {
            case WEEKLY_SCHEDULE:
                return new WeeklyScheduleReport(secRepo, labRepo, userRepo, roomRepo);
            case WEEKLY_TIMESHEET:
                return new WeeklyTimesheetReport(secRepo, labRepo, tsRepo);
            case SEMESTER_CONTACT_HOURS:
                return new SemesterContactHoursReport(secRepo, labRepo, tsRepo);
            default:
                return nullptr;
        }
    }
};

#endif
