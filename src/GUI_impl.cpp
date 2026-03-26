// This file contains the implementation of GUI rendering methods
// Add to the end of GUI.h or create as separate file and include

void LabManagementGUI::renderLoginScreen() {
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center = ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
    
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 350));
    
    ImGui::Begin("Lab Management System - Login", NULL, 
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    
    // Title
    ImGui::PushFont(io.Fonts->Fonts[0]);
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("LAB MANAGEMENT SYSTEM").x) * 0.5f);
    ImGui::TextColored(ImVec4(0.3f, 0.7f, 1.0f, 1.0f), "LAB MANAGEMENT SYSTEM");
    ImGui::PopFont();
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Login form
    ImGui::Text("Select User:");
    ImGui::Spacing();
    
    if (ImGui::BeginCombo("##user", allUsers.empty() ? "No users" : allUsers[selectedUserIndex]->getName().c_str())) {
        for (int i = 0; i < allUsers.size(); i++) {
            bool isSelected = (selectedUserIndex == i);
            string label = allUsers[i]->getName() + " (ID: " + to_string(allUsers[i]->getId()) + ")";
            
            if (ImGui::Selectable(label.c_str(), isSelected)) {
                selectedUserIndex = i;
            }
            
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // User info
    if (!allUsers.empty()) {
        User* selectedUser = allUsers[selectedUserIndex];
        ImGui::Text("User ID: %d", selectedUser->getId());
        ImGui::Text("Name: %s", selectedUser->getName().c_str());
        
        string roleStr;
        switch (selectedUser->getRole()) {
            case UserRole::HOD: roleStr = "Head of Department"; break;
            case UserRole::ACADEMIC_OFFICER: roleStr = "Academic Officer"; break;
            case UserRole::INSTRUCTOR: roleStr = "Instructor"; break;
            case UserRole::TA: roleStr = "Teaching Assistant"; break;
            case UserRole::ATTENDANT: roleStr = "Lab Attendant"; break;
        }
        ImGui::Text("Role: %s", roleStr.c_str());
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Login button
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
    if (ImGui::Button("Login", ImVec2(200, 40)) && !allUsers.empty()) {
        currentUser = allUsers[selectedUserIndex];
        isLoggedIn = true;
        addNotification("Welcome, " + currentUser->getName() + "!");
    }
    
    ImGui::End();
}

void LabManagementGUI::renderMainInterface() {
    // Main menu bar
    if (ImGui::BeginMainMenuBar()) {
        ImGui::Text("Lab Management System");
        ImGui::Separator();
        ImGui::Text("    |    User: %s", currentUser->getName().c_str());
        
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 100);
        if (ImGui::Button("Logout")) {
            isLoggedIn = false;
            currentUser = nullptr;
            addNotification("Logged out successfully", ImVec4(0.8f, 0.5f, 0.2f, 1.0f));
        }
        
        ImGui::EndMainMenuBar();
    }
    
    // Main window
    ImGui::SetNextWindowPos(ImVec2(0, 20));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - 20));
    ImGui::Begin("Main", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                              ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
    
    // Tabs based on user role
    if (ImGui::BeginTabBar("MainTabs")) {
        switch (currentUser->getRole()) {
            case UserRole::ACADEMIC_OFFICER:
                if (ImGui::BeginTabItem("Academic Office")) {
                    renderAcademicOfficerTab();
                    ImGui::EndTabItem();
                }
                break;
                
            case UserRole::ATTENDANT:
                if (ImGui::BeginTabItem("Attendance Management")) {
                    renderAttendantTab();
                    ImGui::EndTabItem();
                }
                break;
                
            case UserRole::HOD:
                if (ImGui::BeginTabItem("Reports & Analytics")) {
                    renderHODTab();
                    ImGui::EndTabItem();
                }
                break;
                
            case UserRole::INSTRUCTOR:
                if (ImGui::BeginTabItem("My Labs")) {
                    renderInstructorTab();
                    ImGui::EndTabItem();
                }
                break;
                
            default:
                ImGui::Text("Role interface not implemented");
                break;
        }
        
        ImGui::EndTabBar();
    }
    
    ImGui::End();
    
    // Render modals
    if (showCreateLabModal) renderCreateLabModal();
    if (showCreateSectionModal) renderCreateSectionModal();
    if (showAssignScheduleModal) renderAssignScheduleModal();
    if (showAssignTAModal) renderAssignTAModal();
    if (showCreateTimesheetModal) renderCreateTimesheetModal();
    if (showFillTimesheetModal) renderFillTimesheetModal();
    if (showMakeupLabModal) renderMakeupLabModal();
    if (showRequestMakeupModal) renderRequestMakeupModal();
    if (showReportViewer) renderReportViewer();
}

void LabManagementGUI::renderAcademicOfficerTab() {
    ImGui::BeginChild("AcademicOfficer", ImVec2(0, 0), true);
    
    // Action buttons
    ImGui::Text("Lab Management");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::Button("Create New Lab", ImVec2(200, 40))) {
        showCreateLabModal = true;
        memset(labCodeBuf, 0, sizeof(labCodeBuf));
        memset(labNameBuf, 0, sizeof(labNameBuf));
        formLabId = labRepo->findAll().size() + 1;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Create New Section", ImVec2(200, 40))) {
        showCreateSectionModal = true;
        memset(sectionNameBuf, 0, sizeof(sectionNameBuf));
        formSectionId = sectionRepo->findAll().size() + 1;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Assign Schedule", ImVec2(200, 40))) {
        showAssignScheduleModal = true;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Assign TA", ImVec2(200, 40))) {
        showAssignTAModal = true;
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Labs table
    ImGui::Text("All Labs");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::BeginTable("LabsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 200))) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableSetupColumn("Code", ImGuiTableColumnFlags_WidthFixed, 150);
        ImGui::TableSetupColumn("Name");
        ImGui::TableHeadersRow();
        
        vector<Lab> labs = labRepo->findAll();
        for (const Lab& lab : labs) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%d", lab.getId());
            ImGui::TableNextColumn();
            ImGui::Text("%s", lab.getCode().c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", lab.getName().c_str());
        }
        
        ImGui::EndTable();
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Sections table
    ImGui::Text("All Sections");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::BeginTable("SectionsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 300))) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableSetupColumn("Lab Code");
        ImGui::TableSetupColumn("Section");
        ImGui::TableSetupColumn("Room");
        ImGui::TableSetupColumn("Instructor");
        ImGui::TableSetupColumn("Schedule");
        ImGui::TableHeadersRow();
        
        vector<Section> sections = sectionRepo->findAll();
        string days[] = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        
        for (const Section& section : sections) {
            Lab lab;
            labRepo->findById(section.getLabId(), lab);
            Room room;
            roomRepo->findById(section.getRoomId(), room);
            User* instructor = nullptr;
            userRepo->findById(section.getInstructorId(), instructor);
            
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%d", section.getId());
            ImGui::TableNextColumn();
            ImGui::Text("%s", lab.getCode().c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", section.getSectionName().c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", room.getRoomNumber().c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", instructor ? instructor->getName().c_str() : "N/A");
            ImGui::TableNextColumn();
            if (section.getSchedule().dayOfWeek > 0) {
                ImGui::Text("%s %s-%s", 
                           days[section.getSchedule().dayOfWeek].c_str(),
                           section.getSchedule().startTime.toString().c_str(),
                           section.getSchedule().endTime.toString().c_str());
            } else {
                ImGui::Text("Not scheduled");
            }
        }
        
        ImGui::EndTable();
    }
    
    ImGui::EndChild();
}

void LabManagementGUI::renderAttendantTab() {
    ImGui::BeginChild("Attendant", ImVec2(0, 0), true);
    
    // Action buttons
    ImGui::Text("Timesheet Management");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::Button("Create Timesheet Entry", ImVec2(220, 40))) {
        showCreateTimesheetModal = true;
        formTimesheetId = timesheetRepo->findAll().size() + 1;
        time_t now = time(0);
        tm* ltm = localtime(&now);
        formDateDay = ltm->tm_mday;
        formDateMonth = 1 + ltm->tm_mon;
        formDateYear = 1900 + ltm->tm_year;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Fill Timesheet", ImVec2(220, 40))) {
        showFillTimesheetModal = true;
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Record Makeup Lab", ImVec2(220, 40))) {
        showMakeupLabModal = true;
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Pending timesheets
    ImGui::Text("Pending Timesheets");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::BeginTable("PendingTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 250))) {
        ImGui::TableSetupColumn("Entry ID", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Section");
        ImGui::TableSetupColumn("Date");
        ImGui::TableSetupColumn("Status");
        ImGui::TableHeadersRow();
        
        vector<TimesheetEntry> entries = timesheetRepo->findAll();
        for (const TimesheetEntry& entry : entries) {
            if (!entry.getIsCompleted()) {
                Section section;
                sectionRepo->findById(entry.getSectionId(), section);
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", entry.getId());
                ImGui::TableNextColumn();
                ImGui::Text("%s", section.getSectionName().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", entry.getDate().toString().c_str());
                ImGui::TableNextColumn();
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "PENDING");
            }
        }
        
        ImGui::EndTable();
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Completed timesheets
    ImGui::Text("Completed Timesheets");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::BeginTable("CompletedTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 250))) {
        ImGui::TableSetupColumn("Entry ID", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("Section");
        ImGui::TableSetupColumn("Date");
        ImGui::TableSetupColumn("Time");
        ImGui::TableSetupColumn("Hours");
        ImGui::TableSetupColumn("Type");
        ImGui::TableHeadersRow();
        
        vector<TimesheetEntry> entries = timesheetRepo->findAll();
        for (const TimesheetEntry& entry : entries) {
            if (entry.getIsCompleted()) {
                Section section;
                sectionRepo->findById(entry.getSectionId(), section);
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%d", entry.getId());
                ImGui::TableNextColumn();
                ImGui::Text("%s", section.getSectionName().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", entry.getDate().toString().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s-%s", 
                           entry.getActualStartTime().toString().c_str(),
                           entry.getActualEndTime().toString().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%d", entry.getContactHours());
                ImGui::TableNextColumn();
                if (entry.getIsMakeup()) {
                    ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.0f, 1.0f), "MAKEUP");
                } else {
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "REGULAR");
                }
            }
        }
        
        ImGui::EndTable();
    }
    
    ImGui::EndChild();
}

void LabManagementGUI::renderHODTab() {
    ImGui::BeginChild("HOD", ImVec2(0, 0), true);
    
    ImGui::Text("Report Generation");
    ImGui::Separator();
    ImGui::Spacing();
    
    // Report buttons
    if (ImGui::Button("Generate Weekly Schedule", ImVec2(250, 50))) {
        hodFacade->generateWeeklySchedule();
        
        // Read report
        FileManager* fm = FileManager::getInstance();
        string filepath = fm->getFilePath("weekly_schedule.txt");
        ifstream file(filepath);
        if (file.is_open()) {
            reportContent = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            file.close();
            reportTitle = "Weekly Schedule Report";
            showReportViewer = true;
        }
        addNotification("Weekly schedule generated successfully!");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Generate Weekly Timesheet", ImVec2(250, 50))) {
        hodFacade->generateWeeklyTimesheet();
        
        // Read report
        FileManager* fm = FileManager::getInstance();
        string filepath = fm->getFilePath("weekly_timesheet.txt");
        ifstream file(filepath);
        if (file.is_open()) {
            reportContent = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
            file.close();
            reportTitle = "Weekly Timesheet Report";
            showReportViewer = true;
        }
        addNotification("Weekly timesheet generated successfully!");
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Semester Contact Hours", ImVec2(250, 50))) {
        ImGui::OpenPopup("SelectSection");
    }
    
    // Popup for section selection
    if (ImGui::BeginPopup("SelectSection")) {
        ImGui::Text("Select Section:");
        ImGui::Separator();
        
        vector<Section> sections = sectionRepo->findAll();
        for (const Section& section : sections) {
            Lab lab;
            labRepo->findById(section.getLabId(), lab);
            string label = lab.getCode() + " - " + section.getSectionName();
            
            if (ImGui::Selectable(label.c_str())) {
                hodFacade->generateSemesterContactHours(section.getId());
                
                // Read report
                FileManager* fm = FileManager::getInstance();
                string filepath = fm->getFilePath("semester_contact_hours.txt");
                ifstream file(filepath);
                if (file.is_open()) {
                    reportContent = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
                    file.close();
                    reportTitle = "Semester Contact Hours Report";
                    showReportViewer = true;
                }
                addNotification("Contact hours report generated!");
            }
        }
        
        ImGui::EndPopup();
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Statistics
    ImGui::Text("Overview Statistics");
    ImGui::Separator();
    ImGui::Spacing();
    
    vector<Lab> labs = labRepo->findAll();
    vector<Section> sections = sectionRepo->findAll();
    vector<TimesheetEntry> entries = timesheetRepo->findAll();
    vector<User*> users = userRepo->findAll();
    
    int completedCount = 0;
    int pendingCount = 0;
    int totalHours = 0;
    
    for (const TimesheetEntry& entry : entries) {
        if (entry.getIsCompleted()) {
            completedCount++;
            totalHours += entry.getContactHours();
        } else {
            pendingCount++;
        }
    }
    
    ImGui::BeginGroup();
    ImGui::Text("Total Labs: %zu", labs.size());
    ImGui::Text("Total Sections: %zu", sections.size());
    ImGui::Text("Total Users: %zu", users.size());
    ImGui::EndGroup();
    
    ImGui::SameLine(300);
    ImGui::BeginGroup();
    ImGui::Text("Completed Sessions: %d", completedCount);
    ImGui::Text("Pending Sessions: %d", pendingCount);
    ImGui::Text("Total Contact Hours: %d", totalHours);
    ImGui::EndGroup();
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Recent activity
    ImGui::Text("Recent Timesheet Activity");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::BeginTable("RecentActivity", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 400))) {
        ImGui::TableSetupColumn("Section");
        ImGui::TableSetupColumn("Date");
        ImGui::TableSetupColumn("Time");
        ImGui::TableSetupColumn("Hours");
        ImGui::TableSetupColumn("Status");
        ImGui::TableHeadersRow();
        
        for (const TimesheetEntry& entry : entries) {
            Section section;
            sectionRepo->findById(entry.getSectionId(), section);
            Lab lab;
            labRepo->findById(section.getLabId(), lab);
            
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s - %s", lab.getCode().c_str(), section.getSectionName().c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", entry.getDate().toString().c_str());
            ImGui::TableNextColumn();
            if (entry.getIsCompleted()) {
                ImGui::Text("%s-%s", 
                           entry.getActualStartTime().toString().c_str(),
                           entry.getActualEndTime().toString().c_str());
            } else {
                ImGui::Text("---");
            }
            ImGui::TableNextColumn();
            if (entry.getIsCompleted()) {
                ImGui::Text("%d", entry.getContactHours());
            } else {
                ImGui::Text("---");
            }
            ImGui::TableNextColumn();
            if (entry.getIsCompleted()) {
                ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "COMPLETED");
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "PENDING");
            }
        }
        
        ImGui::EndTable();
    }
    
    ImGui::EndChild();
}

void LabManagementGUI::renderInstructorTab() {
    ImGui::BeginChild("Instructor", ImVec2(0, 0), true);
    
    ImGui::Text("My Lab Schedule");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::Button("Request Makeup Lab", ImVec2(200, 40))) {
        showRequestMakeupModal = true;
        time_t now = time(0);
        tm* ltm = localtime(&now);
        formDateDay = ltm->tm_mday;
        formDateMonth = 1 + ltm->tm_mon;
        formDateYear = 1900 + ltm->tm_year;
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // My sections
    if (ImGui::BeginTable("MySchedule", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 350))) {
        ImGui::TableSetupColumn("Lab");
        ImGui::TableSetupColumn("Section");
        ImGui::TableSetupColumn("Day");
        ImGui::TableSetupColumn("Time");
        ImGui::TableSetupColumn("Room");
        ImGui::TableHeadersRow();
        
        vector<Section> sections = sectionRepo->findAll();
        string days[] = {"", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        
        for (const Section& section : sections) {
            if (section.getInstructorId() == currentUser->getId()) {
                Lab lab;
                labRepo->findById(section.getLabId(), lab);
                Room room;
                roomRepo->findById(section.getRoomId(), room);
                
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s - %s", lab.getCode().c_str(), lab.getName().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s", section.getSectionName().c_str());
                ImGui::TableNextColumn();
                if (section.getSchedule().dayOfWeek > 0) {
                    ImGui::Text("%s", days[section.getSchedule().dayOfWeek].c_str());
                } else {
                    ImGui::Text("Not scheduled");
                }
                ImGui::TableNextColumn();
                if (section.getSchedule().dayOfWeek > 0) {
                    ImGui::Text("%s - %s", 
                               section.getSchedule().startTime.toString().c_str(),
                               section.getSchedule().endTime.toString().c_str());
                } else {
                    ImGui::Text("---");
                }
                ImGui::TableNextColumn();
                ImGui::Text("%s", room.getRoomNumber().c_str());
            }
        }
        
        ImGui::EndTable();
    }
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // My timesheet history
    ImGui::Text("My Timesheet History");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (ImGui::BeginTable("MyTimesheets", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 300))) {
        ImGui::TableSetupColumn("Section");
        ImGui::TableSetupColumn("Date");
        ImGui::TableSetupColumn("Time");
        ImGui::TableSetupColumn("Hours");
        ImGui::TableSetupColumn("Type");
        ImGui::TableHeadersRow();
        
        vector<TimesheetEntry> entries = timesheetRepo->findAll();
        vector<Section> mySections = sectionRepo->findAll();
        
        for (const TimesheetEntry& entry : entries) {
            Section section;
            if (sectionRepo->findById(entry.getSectionId(), section)) {
                if (section.getInstructorId() == currentUser->getId()) {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", section.getSectionName().c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", entry.getDate().toString().c_str());
                    ImGui::TableNextColumn();
                    if (entry.getIsCompleted()) {
                        ImGui::Text("%s-%s", 
                                   entry.getActualStartTime().toString().c_str(),
                                   entry.getActualEndTime().toString().c_str());
                    } else {
                        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Pending");
                    }
                    ImGui::TableNextColumn();
                    if (entry.getIsCompleted()) {
                        ImGui::Text("%d", entry.getContactHours());
                    } else {
                        ImGui::Text("---");
                    }
                    ImGui::TableNextColumn();
                    if (entry.getIsMakeup()) {
                        ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.0f, 1.0f), "MAKEUP");
                    } else {
                        ImGui::Text("Regular");
                    }
                }
            }
        }
        
        ImGui::EndTable();
    }
    
    ImGui::EndChild();
}

// Modal implementations continue in next artifact...