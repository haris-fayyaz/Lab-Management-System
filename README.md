# Labs Management System

## Project Overview
A comprehensive system to manage university lab information including schedules, timesheets, instructors, TAs, and venues.

## Design Patterns Implemented

### 1. **Singleton Pattern**
- `FileManager`: Ensures single instance for file operations

### 2. **Factory Method Pattern**
- `UserFactory`: Creates different types of users (Instructor, TA, Attendant, etc.)
- `ReportFactory`: Creates different types of reports

### 3. **Observer Pattern**
- `TimesheetEntry`: Notifies HOD and Instructors when timesheet is filled
- `MakeupLabScheduler`: Notifies attendants and HOD about makeup labs

### 4. **Repository Pattern**
- `UserRepository`, `LabRepository`, `SectionRepository`
- `TimesheetRepository`, `BuildingRepository`, `RoomRepository`
- All repositories implement `IRepository<T>` interface

### 5. **Template Method Pattern**
- `Report` abstract class with `generate()` template method
- `WeeklyScheduleReport`, `WeeklyTimesheetReport`, `SemesterContactHoursReport`

### 6. **Facade Pattern**
- `AcademicOfficeFacade`: Simplifies lab and section management
- `AttendantFacade`: Simplifies timesheet operations
- `HODFacade`: Simplifies report generation
- `InstructorFacade`: Simplifies instructor operations

## SOLID Principles

### Single Responsibility Principle (SRP)
- Each class has one reason to change
- `User` handles user data, `UserRepository` handles persistence
- `Report` handles report generation, repositories handle data

### Open/Closed Principle (OCP)
- Reports can be extended without modifying base `Report` class
- New user types can be added without changing `UserFactory`

### Liskov Substitution Principle (LSP)
- All user types (Instructor, TA, etc.) can substitute `User`
- All repositories implement `IRepository<T>` interface

### Interface Segregation Principle (ISP)
- Facades provide role-specific interfaces
- No class is forced to implement unused methods

### Dependency Inversion Principle (DIP)
- High-level modules depend on `IRepository<T>` abstraction
- FileManager is injected, not hardcoded

## Features

### Academic Officer
1. Create labs and sections
2. Assign schedules to sections
3. Assign TAs to sections
4. List all labs and sections

### Attendant
1. Create timesheet entries
2. Fill timesheets (notifies HOD and Instructor)
3. Record makeup labs
4. List pending timesheets

### HOD
1. Generate weekly schedule report
2. Generate weekly timesheet report
3. Generate semester contact hours report (with leaves)

### Instructor
1. View personal schedule
2. Request makeup labs (notifies attendant and HOD)

## File Structure
```
LabsManagementSystem/
├── include/
│   ├── Common.h           # Common types and utilities
│   ├── Observer.h         # Observer pattern
│   ├── User.h             # User hierarchy
│   ├── Venue.h            # Building and Room
│   ├── Lab.h              # Lab and Section
│   ├── Timesheet.h        # Timesheet entries
│   ├── FileManager.h      # Singleton file manager
│   ├── Repository.h       # Repository interface
│   ├── Repositories.h     # Concrete repositories
│   ├── Report.h           # Template method reports
│   ├── Factory.h          # Factory methods
│   └── Facade.h           # Facade patterns
├── data/                  # Binary files (auto-created)
├── main.cpp               # Main application
├── Makefile               # Build configuration
└── README.md              # This file
```

## Compilation & Execution

### Compile
```bash
make
```

### Run
```bash
make run
```

### Clean
```bash
make clean
```

## Sample Data

### Users
- ID 1: Dr. Ahmed Khan (HOD)
- ID 2: Sara Ali (Academic Officer)
- ID 3: Dr. Hassan (Instructor)
- ID 4: Dr. Fatima (Instructor)
- ID 5: Ali Raza (TA)
- ID 6: Ayesha Khan (TA)
- ID 7: Umer Farooq (Attendant - CS Building)
- ID 8: Zain Abbas (Attendant - Engineering Building)

### Labs
- Lab 1: CS201 - Data Structures
- Lab 2: CS301 - Database Systems
- Lab 3: CS401 - Software Engineering

### Sections
- Section 1: CS201 Section A (Monday 9:00-11:00)
- Section 2: CS201 Section B (Tuesday 14:00-16:00)
- Section 3: CS301 Section A (Wednesday 10:00-12:00)

## Usage Example

1. Login as Academic Officer (ID: 2)
   - Create labs, sections
   - Assign schedules and TAs

2. Login as Attendant (ID: 7)
   - Fill timesheets for completed labs
   - Record makeup labs

3. Login as HOD (ID: 1)
   - Generate weekly schedule
   - Generate timesheet reports
   - Generate contact hours reports

4. Login as Instructor (ID: 3)
   - View your schedule
   - Request makeup labs

## Binary File Storage

All data is stored in binary files in the `data/` directory:
- `users.dat` - User information
- `labs.dat` - Lab information
- `sections.dat` - Section information
- `timesheets.dat` - Timesheet entries
- `buildings.dat` - Building information
- `rooms.dat` - Room information

## Reports Generated

Reports are saved in the `data/` directory:
- `weekly_schedule.txt` - Complete weekly lab schedule
- `weekly_timesheet.txt` - Filled timesheets for the week
- `semester_contact_hours.txt` - Contact hours and leaves for a section

## Code Quality

- Follows SOLID principles
- Clean separation of concerns
- Minimal external dependencies
- Well-commented and structured
- Uses design patterns appropriately
- Binary file serialization/deserialization

## Future Enhancements

1. Add authentication and authorization
2. Implement date range filtering for reports
3. Add search and filter functionality
4. Implement data validation
5. Add more detailed error handling
6. Create a configuration file for system settings
