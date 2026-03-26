#include "include/Common.h"
#include "include/Observer.h"
#include "include/User.h"
#include "include/Venue.h"
#include "include/Lab.h"
#include "include/Timesheet.h"
#include "include/FileManager.h"
#include "include/Repository.h"
#include "include/Repositories.h"
#include "include/Report.h"
#include "include/Factory.h"
#include "include/Facade.h"

// Makeup Lab Scheduler (acts as Subject for notifications)
class MakeupLabScheduler : public Subject {
public:
    void scheduleMakeupLab(int sectionId, Date date) {
        notify("Makeup lab scheduled for Section " + to_string(sectionId) + 
               " on " + date.toString());
    }
};

class LabsManagementSystem {
private:
    // Repositories
    UserRepository* userRepo;
    LabRepository* labRepo;
    SectionRepository* sectionRepo;
    TimesheetRepository* timesheetRepo;
    BuildingRepository* buildingRepo;
    RoomRepository* roomRepo;
    
    // Facades
    AcademicOfficeFacade* academicFacade;
    AttendantFacade* attendantFacade;
    HODFacade* hodFacade;
    InstructorFacade* instructorFacade;
    
    // Scheduler
    MakeupLabScheduler* makeupScheduler;
    
    // Current user
    User* currentUser;
    
public:
    LabsManagementSystem() {
        // Initialize repositories
        userRepo = new UserRepository();
        labRepo = new LabRepository();
        sectionRepo = new SectionRepository();
        timesheetRepo = new TimesheetRepository();
        buildingRepo = new BuildingRepository();
        roomRepo = new RoomRepository();
        
        // Initialize facades
        academicFacade = new AcademicOfficeFacade(labRepo, sectionRepo, userRepo, roomRepo);
        attendantFacade = new AttendantFacade(timesheetRepo, sectionRepo);
        hodFacade = new HODFacade(sectionRepo, labRepo, userRepo, roomRepo, timesheetRepo);
        instructorFacade = new InstructorFacade(timesheetRepo, sectionRepo);
        
        // Initialize scheduler
        makeupScheduler = new MakeupLabScheduler();
        instructorFacade->setMakeupScheduler(makeupScheduler);
        
        currentUser = nullptr;
        
        initializeSampleData();
    }
    
    ~LabsManagementSystem() {
        delete userRepo;
        delete labRepo;
        delete sectionRepo;
        delete timesheetRepo;
        delete buildingRepo;
        delete roomRepo;
        delete academicFacade;
        delete attendantFacade;
        delete hodFacade;
        delete instructorFacade;
        delete makeupScheduler;
        FileManager::cleanup();
    }
    
    void initializeSampleData() {
        // Check if data already exists
        if (!userRepo->findAll().empty()) {
            return;
        }
        
        cout << "Initializing sample data..." << endl;
        
        // Create users using Factory Pattern
        User* hod = UserFactory::createUser(UserRole::HOD, 1, "Dr. Ahmed Khan");
        User* officer = UserFactory::createUser(UserRole::ACADEMIC_OFFICER, 2, "Sara Ali");
        User* instructor1 = UserFactory::createUser(UserRole::INSTRUCTOR, 3, "Dr. Hassan");
        User* instructor2 = UserFactory::createUser(UserRole::INSTRUCTOR, 4, "Dr. Fatima");
        User* ta1 = UserFactory::createUser(UserRole::TA, 5, "Ali Raza");
        User* ta2 = UserFactory::createUser(UserRole::TA, 6, "Ayesha Khan");
        User* attendant1 = UserFactory::createUser(UserRole::ATTENDANT, 7, "Umer Farooq", 1);
        User* attendant2 = UserFactory::createUser(UserRole::ATTENDANT, 8, "Zain Abbas", 2);
        
        userRepo->save(hod);
        userRepo->save(officer);
        userRepo->save(instructor1);
        userRepo->save(instructor2);
        userRepo->save(ta1);
        userRepo->save(ta2);
        userRepo->save(attendant1);
        userRepo->save(attendant2);
        
        // Create buildings
        Building building1(1, "Computer Science Building", 7);
        Building building2(2, "Engineering Building", 8);
        buildingRepo->save(building1);
        buildingRepo->save(building2);
        
        // Create rooms
        Room room1(1, "CS-101", 1, 30);
        Room room2(2, "CS-102", 1, 25);
        Room room3(3, "EN-201", 2, 35);
        roomRepo->save(room1);
        roomRepo->save(room2);
        roomRepo->save(room3);
        
        // Create labs
        Lab lab1(1, "CS201", "Data Structures");
        Lab lab2(2, "CS301", "Database Systems");
        Lab lab3(3, "CS401", "Software Engineering");
        labRepo->save(lab1);
        labRepo->save(lab2);
        labRepo->save(lab3);
        
        // Create sections
        Section sec1(1, 1, "Section A", 1, 3);
        sec1.setSchedule(Schedule(1, Time(9, 0), Time(11, 0)));
        sec1.addTA(5);
        
        Section sec2(2, 1, "Section B", 2, 3);
        sec2.setSchedule(Schedule(2, Time(14, 0), Time(16, 0)));
        sec2.addTA(6);
        
        Section sec3(3, 2, "Section A", 3, 4);
        sec3.setSchedule(Schedule(3, Time(10, 0), Time(12, 0)));
        sec3.addTA(5);
        
        sectionRepo->save(sec1);
        sectionRepo->save(sec2);
        sectionRepo->save(sec3);
        
        // Create timesheet entries
        TimesheetEntry entry1(1, 1, Date(15, 11, 2024));
        TimesheetEntry entry2(2, 2, Date(16, 11, 2024));
        TimesheetEntry entry3(3, 3, Date(17, 11, 2024));
        
        timesheetRepo->save(entry1);
        timesheetRepo->save(entry2);
        timesheetRepo->save(entry3);
        
        cout << "Sample data initialized successfully!" << endl;
    }
    
    void run() {
        while (true) {
            displayMainMenu();
            int choice;
            cin >> choice;
            cin.ignore();
            
            switch (choice) {
                case 1:
                    login();
                    break;
                case 2:
                    cout << "\nExiting system. Goodbye!" << endl;
                    return;
                default:
                    cout << "\nInvalid choice! Try again." << endl;
            }
        }
    }
    
private:
    void displayMainMenu() {
        cout << "\n============================================" << endl;
        cout << "    LABS MANAGEMENT SYSTEM" << endl;
        cout << "============================================" << endl;
        cout << "1. Login" << endl;
        cout << "2. Exit" << endl;
        cout << "Enter choice: ";
    }
    
    void login() {
        cout << "\n=== Login ===" << endl;
        cout << "Enter User ID: ";
        int userId;
        cin >> userId;
        cin.ignore();
        
        if (!userRepo->findById(userId, currentUser)) {
            cout << "User not found!" << endl;
            return;
        }
        
        cout << "Welcome, " << currentUser->getName() << "!" << endl;
        
        switch (currentUser->getRole()) {
            case UserRole::ACADEMIC_OFFICER:
                academicOfficerMenu();
                break;
            case UserRole::ATTENDANT:
                attendantMenu();
                break;
            case UserRole::HOD:
                hodMenu();
                break;
            case UserRole::INSTRUCTOR:
                instructorMenu();
                break;
            default:
                cout << "Role not implemented yet." << endl;
        }
    }
    
    void academicOfficerMenu() {
        while (true) {
            cout << "\n=== Academic Officer Menu ===" << endl;
            cout << "1. Create Lab" << endl;
            cout << "2. Create Section" << endl;
            cout << "3. Assign Schedule to Section" << endl;
            cout << "4. Assign TA to Section" << endl;
            cout << "5. List All Labs" << endl;
            cout << "6. List All Sections" << endl;
            cout << "7. Logout" << endl;
            cout << "Enter choice: ";
            
            int choice;
            cin >> choice;
            cin.ignore();
            
            switch (choice) {
                case 1: {
                    int id;
                    string code, name;
                    cout << "Enter Lab ID: ";
                    cin >> id;
                    cin.ignore();
                    cout << "Enter Lab Code: ";
                    getline(cin, code);
                    cout << "Enter Lab Name: ";
                    getline(cin, name);
                    academicFacade->createLab(id, code, name);
                    break;
                }
                case 2: {
                    int id, labId, roomId, instructorId;
                    string sectionName;
                    cout << "Enter Section ID: ";
                    cin >> id;
                    cout << "Enter Lab ID: ";
                    cin >> labId;
                    cin.ignore();
                    cout << "Enter Section Name: ";
                    getline(cin, sectionName);
                    cout << "Enter Room ID: ";
                    cin >> roomId;
                    cout << "Enter Instructor ID: ";
                    cin >> instructorId;
                    academicFacade->createSection(id, labId, sectionName, roomId, instructorId);
                    break;
                }
                case 3: {
                    int sectionId, day, startH, startM, endH, endM;
                    cout << "Enter Section ID: ";
                    cin >> sectionId;
                    cout << "Enter Day (1=Mon, 7=Sun): ";
                    cin >> day;
                    cout << "Enter Start Time (Hour Minute): ";
                    cin >> startH >> startM;
                    cout << "Enter End Time (Hour Minute): ";
                    cin >> endH >> endM;
                    academicFacade->assignSchedule(sectionId, day, Time(startH, startM), Time(endH, endM));
                    break;
                }
                case 4: {
                    int sectionId, taId;
                    cout << "Enter Section ID: ";
                    cin >> sectionId;
                    cout << "Enter TA ID: ";
                    cin >> taId;
                    academicFacade->assignTA(sectionId, taId);
                    break;
                }
                case 5:
                    academicFacade->listAllLabs();
                    break;
                case 6:
                    academicFacade->listAllSections();
                    break;
                case 7:
                    return;
                default:
                    cout << "Invalid choice!" << endl;
            }
        }
    }
    
    void attendantMenu() {
        while (true) {
            cout << "\n=== Attendant Menu ===" << endl;
            cout << "1. Create Timesheet Entry" << endl;
            cout << "2. Fill Timesheet" << endl;
            cout << "3. Record Makeup Lab" << endl;
            cout << "4. List Pending Timesheets" << endl;
            cout << "5. Logout" << endl;
            cout << "Enter choice: ";
            
            int choice;
            cin >> choice;
            cin.ignore();
            
            switch (choice) {
                case 1: {
                    int id, sectionId, day, month, year;
                    cout << "Enter Entry ID: ";
                    cin >> id;
                    cout << "Enter Section ID: ";
                    cin >> sectionId;
                    cout << "Enter Date (DD MM YYYY): ";
                    cin >> day >> month >> year;
                    attendantFacade->createTimesheetEntry(id, sectionId, Date(day, month, year));
                    break;
                }
                case 2: {
                    int entryId, startH, startM, endH, endM;
                    cout << "Enter Timesheet Entry ID: ";
                    cin >> entryId;
                    cout << "Enter Actual Start Time (Hour Minute): ";
                    cin >> startH >> startM;
                    cout << "Enter Actual End Time (Hour Minute): ";
                    cin >> endH >> endM;
                    
                    User* hod = nullptr;
                    User* instructor = nullptr;
                    userRepo->findById(1, hod);  // HOD ID = 1
                    
                    TimesheetEntry entry;
                    if (timesheetRepo->findById(entryId, entry)) {
                        Section section;
                        sectionRepo->findById(entry.getSectionId(), section);
                        userRepo->findById(section.getInstructorId(), instructor);
                    }
                    
                    attendantFacade->fillTimesheet(entryId, Time(startH, startM), 
                                                  Time(endH, endM), hod, instructor);
                    break;
                }
                case 3: {
                    int entryId, startH, startM, endH, endM;
                    cout << "Enter Makeup Lab Entry ID: ";
                    cin >> entryId;
                    cout << "Enter Actual Start Time (Hour Minute): ";
                    cin >> startH >> startM;
                    cout << "Enter Actual End Time (Hour Minute): ";
                    cin >> endH >> endM;
                    
                    User* hod = nullptr;
                    User* instructor = nullptr;
                    userRepo->findById(1, hod);
                    
                    TimesheetEntry entry;
                    if (timesheetRepo->findById(entryId, entry)) {
                        Section section;
                        sectionRepo->findById(entry.getSectionId(), section);
                        userRepo->findById(section.getInstructorId(), instructor);
                    }
                    
                    attendantFacade->recordMakeupLab(entryId, Time(startH, startM), 
                                                    Time(endH, endM), hod, instructor);
                    break;
                }
                case 4:
                    attendantFacade->listPendingTimesheets();
                    break;
                case 5:
                    return;
                default:
                    cout << "Invalid choice!" << endl;
            }
        }
    }
    
    void hodMenu() {
        while (true) {
            cout << "\n=== HOD Menu ===" << endl;
            cout << "1. Generate Weekly Schedule Report" << endl;
            cout << "2. Generate Weekly Timesheet Report" << endl;
            cout << "3. Generate Semester Contact Hours Report" << endl;
            cout << "4. Logout" << endl;
            cout << "Enter choice: ";
            
            int choice;
            cin >> choice;
            cin.ignore();
            
            switch (choice) {
                case 1:
                    hodFacade->generateWeeklySchedule();
                    break;
                case 2:
                    hodFacade->generateWeeklyTimesheet();
                    break;
                case 3: {
                    int sectionId;
                    cout << "Enter Section ID: ";
                    cin >> sectionId;
                    hodFacade->generateSemesterContactHours(sectionId);
                    break;
                }
                case 4:
                    return;
                default:
                    cout << "Invalid choice!" << endl;
            }
        }
    }
    
    void instructorMenu() {
        while (true) {
            cout << "\n=== Instructor Menu ===" << endl;
            cout << "1. View My Schedule" << endl;
            cout << "2. Request Makeup Lab" << endl;
            cout << "3. Logout" << endl;
            cout << "Enter choice: ";
            
            int choice;
            cin >> choice;
            cin.ignore();
            
            switch (choice) {
                case 1:
                    instructorFacade->viewSchedule(currentUser->getId());
                    break;
                case 2: {
                    int sectionId, day, month, year;
                    cout << "Enter Section ID: ";
                    cin >> sectionId;
                    cout << "Enter Desired Date (DD MM YYYY): ";
                    cin >> day >> month >> year;
                    
                    User* hod = nullptr;
                    User* attendant = nullptr;
                    userRepo->findById(1, hod);
                    userRepo->findById(7, attendant);  // Attendant ID
                    
                    instructorFacade->requestMakeupLab(sectionId, Date(day, month, year), 
                                                      attendant, hod);
                    break;
                }
                case 3:
                    return;
                default:
                    cout << "Invalid choice!" << endl;
            }
        }
    }
};

int main() {
    LabsManagementSystem system;
    system.run();
    return 0;
}
