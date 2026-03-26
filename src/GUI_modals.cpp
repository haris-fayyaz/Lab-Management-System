// Modal dialog implementations - Add to GUI.h or include in main

void LabManagementGUI::renderCreateLabModal() {
    ImGui::OpenPopup("Create Lab");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 300));
    
    if (ImGui::BeginPopupModal("Create Lab", &showCreateLabModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create New Lab");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Lab ID:");
        ImGui::SameLine();
        ImGui::InputInt("##labid", &formLabId);
        
        ImGui::Text("Lab Code:");
        ImGui::SameLine();
        ImGui::InputText("##labcode", labCodeBuf, sizeof(labCodeBuf));
        
        ImGui::Text("Lab Name:");
        ImGui::SameLine();
        ImGui::InputText("##labname", labNameBuf, sizeof(labNameBuf));
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            if (strlen(labCodeBuf) > 0 && strlen(labNameBuf) > 0) {
                academicFacade->createLab(formLabId, string(labCodeBuf), string(labNameBuf));
                addNotification("Lab created: " + string(labCodeBuf));
                showCreateLabModal = false;
            } else {
                addNotification("Please fill all fields", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showCreateLabModal = false;
        }
        
        ImGui::EndPopup();
    }
}

void LabManagementGUI::renderCreateSectionModal() {
    ImGui::OpenPopup("Create Section");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(550, 400));
    
    if (ImGui::BeginPopupModal("Create Section", &showCreateSectionModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create New Section");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Section ID:");
        ImGui::SameLine();
        ImGui::InputInt("##secid", &formSectionId);
        
        ImGui::Text("Lab:");
        ImGui::SameLine();
        vector<Lab> labs = labRepo->findAll();
        if (ImGui::BeginCombo("##lab", formLabId > 0 ? to_string(formLabId).c_str() : "Select Lab")) {
            for (const Lab& lab : labs) {
                bool isSelected = (formLabId == lab.getId());
                string label = lab.getCode() + " - " + lab.getName();
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    formLabId = lab.getId();
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Section Name:");
        ImGui::SameLine();
        ImGui::InputText("##secname", sectionNameBuf, sizeof(sectionNameBuf));
        
        ImGui::Text("Room:");
        ImGui::SameLine();
        vector<Room> rooms = roomRepo->findAll();
        if (ImGui::BeginCombo("##room", formRoomId > 0 ? to_string(formRoomId).c_str() : "Select Room")) {
            for (const Room& room : rooms) {
                bool isSelected = (formRoomId == room.getId());
                if (ImGui::Selectable(room.getRoomNumber().c_str(), isSelected)) {
                    formRoomId = room.getId();
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Instructor:");
        ImGui::SameLine();
        vector<User*> users = userRepo->findAll();
        if (ImGui::BeginCombo("##instructor", formInstructorId > 0 ? to_string(formInstructorId).c_str() : "Select Instructor")) {
            for (User* user : users) {
                if (user->getRole() == UserRole::INSTRUCTOR) {
                    bool isSelected = (formInstructorId == user->getId());
                    if (ImGui::Selectable(user->getName().c_str(), isSelected)) {
                        formInstructorId = user->getId();
                    }
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            if (strlen(sectionNameBuf) > 0 && formLabId > 0 && formRoomId > 0 && formInstructorId > 0) {
                academicFacade->createSection(formSectionId, formLabId, string(sectionNameBuf), formRoomId, formInstructorId);
                addNotification("Section created: " + string(sectionNameBuf));
                showCreateSectionModal = false;
            } else {
                addNotification("Please fill all fields", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showCreateSectionModal = false;
        }
        
        ImGui::EndPopup();
    }
}

void LabManagementGUI::renderAssignScheduleModal() {
    ImGui::OpenPopup("Assign Schedule");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 350));
    
    if (ImGui::BeginPopupModal("Assign Schedule", &showAssignScheduleModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Assign Schedule to Section");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Section:");
        ImGui::SameLine();
        vector<Section> sections = sectionRepo->findAll();
        if (ImGui::BeginCombo("##section", formSectionId > 0 ? to_string(formSectionId).c_str() : "Select Section")) {
            for (const Section& section : sections) {
                bool isSelected = (formSectionId == section.getId());
                Lab lab;
                labRepo->findById(section.getLabId(), lab);
                string label = lab.getCode() + " - " + section.getSectionName();
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    formSectionId = section.getId();
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Day:");
        ImGui::SameLine();
        const char* days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
        if (ImGui::BeginCombo("##day", days[formDay - 1])) {
            for (int i = 0; i < 7; i++) {
                bool isSelected = (formDay == i + 1);
                if (ImGui::Selectable(days[i], isSelected)) {
                    formDay = i + 1;
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Start Time:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##starthour", &formStartHour);
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##startmin", &formStartMin);
        
        ImGui::Text("End Time:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##endhour", &formEndHour);
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##endmin", &formEndMin);
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (ImGui::Button("Assign", ImVec2(120, 0))) {
            if (formSectionId > 0) {
                academicFacade->assignSchedule(formSectionId, formDay, 
                                              Time(formStartHour, formStartMin), 
                                              Time(formEndHour, formEndMin));
                addNotification("Schedule assigned successfully");
                showAssignScheduleModal = false;
            } else {
                addNotification("Please select a section", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showAssignScheduleModal = false;
        }
        
        ImGui::EndPopup();
    }
}

void LabManagementGUI::renderAssignTAModal() {
    ImGui::OpenPopup("Assign TA");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(450, 250));
    
    if (ImGui::BeginPopupModal("Assign TA", &showAssignTAModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Assign TA to Section");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Section:");
        ImGui::SameLine();
        vector<Section> sections = sectionRepo->findAll();
        if (ImGui::BeginCombo("##section", formSectionId > 0 ? to_string(formSectionId).c_str() : "Select Section")) {
            for (const Section& section : sections) {
                bool isSelected = (formSectionId == section.getId());
                Lab lab;
                labRepo->findById(section.getLabId(), lab);
                string label = lab.getCode() + " - " + section.getSectionName();
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    formSectionId = section.getId();
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("TA:");
        ImGui::SameLine();
        vector<User*> users = userRepo->findAll();
        if (ImGui::BeginCombo("##ta", formTAId > 0 ? to_string(formTAId).c_str() : "Select TA")) {
            for (User* user : users) {
                if (user->getRole() == UserRole::TA) {
                    bool isSelected = (formTAId == user->getId());
                    if (ImGui::Selectable(user->getName().c_str(), isSelected)) {
                        formTAId = user->getId();
                    }
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (ImGui::Button("Assign", ImVec2(120, 0))) {
            if (formSectionId > 0 && formTAId > 0) {
                academicFacade->assignTA(formSectionId, formTAId);
                addNotification("TA assigned successfully");
                showAssignTAModal = false;
            } else {
                addNotification("Please select section and TA", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showAssignTAModal = false;
        }
        
        ImGui::EndPopup();
    }
}

void LabManagementGUI::renderCreateTimesheetModal() {
    ImGui::OpenPopup("Create Timesheet");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 300));
    
    if (ImGui::BeginPopupModal("Create Timesheet", &showCreateTimesheetModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create Timesheet Entry");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Entry ID:");
        ImGui::SameLine();
        ImGui::InputInt("##entryid", &formTimesheetId);
        
        ImGui::Text("Section:");
        ImGui::SameLine();
        vector<Section> sections = sectionRepo->findAll();
        if (ImGui::BeginCombo("##section", formSectionId > 0 ? to_string(formSectionId).c_str() : "Select Section")) {
            for (const Section& section : sections) {
                bool isSelected = (formSectionId == section.getId());
                Lab lab;
                labRepo->findById(section.getLabId(), lab);
                string label = lab.getCode() + " - " + section.getSectionName();
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    formSectionId = section.getId();
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Date (DD/MM/YYYY):");
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##day", &formDateDay);
        ImGui::SameLine();
        ImGui::Text("/");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##month", &formDateMonth);
        ImGui::SameLine();
        ImGui::Text("/");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::InputInt("##year", &formDateYear);
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            if (formSectionId > 0) {
                attendantFacade->createTimesheetEntry(formTimesheetId, formSectionId, 
                                                     Date(formDateDay, formDateMonth, formDateYear));
                addNotification("Timesheet entry created");
                showCreateTimesheetModal = false;
            } else {
                addNotification("Please select a section", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showCreateTimesheetModal = false;
        }
        
        ImGui::EndPopup();
    }
}

void LabManagementGUI::renderFillTimesheetModal() {
    ImGui::OpenPopup("Fill Timesheet");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 300));
    
    if (ImGui::BeginPopupModal("Fill Timesheet", &showFillTimesheetModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Fill Timesheet Entry");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Entry:");
        ImGui::SameLine();
        vector<TimesheetEntry> entries = timesheetRepo->findAll();
        if (ImGui::BeginCombo("##entry", formTimesheetId > 0 ? to_string(formTimesheetId).c_str() : "Select Entry")) {
            for (const TimesheetEntry& entry : entries) {
                if (!entry.getIsCompleted()) {
                    bool isSelected = (formTimesheetId == entry.getId());
                    string label = "Entry #" + to_string(entry.getId()) + " - " + entry.getDate().toString();
                    if (ImGui::Selectable(label.c_str(), isSelected)) {
                        formTimesheetId = entry.getId();
                    }
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Actual Start Time:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##starthour", &formStartHour);
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##startmin", &formStartMin);
        
        ImGui::Text("Actual End Time:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##endhour", &formEndHour);
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##endmin", &formEndMin);
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (ImGui::Button("Fill", ImVec2(120, 0))) {
            if (formTimesheetId > 0) {
                User* hod = nullptr;
                User* instructor = nullptr;
                userRepo->findById(1, hod);
                
                TimesheetEntry entry;
                if (timesheetRepo->findById(formTimesheetId, entry)) {
                    Section section;
                    sectionRepo->findById(entry.getSectionId(), section);
                    userRepo->findById(section.getInstructorId(), instructor);
                }
                
                attendantFacade->fillTimesheet(formTimesheetId, 
                                              Time(formStartHour, formStartMin), 
                                              Time(formEndHour, formEndMin), 
                                              hod, instructor);
                addNotification("Timesheet filled successfully");
                showFillTimesheetModal = false;
            } else {
                addNotification("Please select an entry", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showFillTimesheetModal = false;
        }
        
        ImGui::EndPopup();
    }
}

void LabManagementGUI::renderMakeupLabModal() {
    ImGui::OpenPopup("Record Makeup Lab");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 300));
    
    if (ImGui::BeginPopupModal("Record Makeup Lab", &showMakeupLabModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Record Makeup Lab");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Entry:");
        ImGui::SameLine();
        vector<TimesheetEntry> entries = timesheetRepo->findAll();
        if (ImGui::BeginCombo("##entry", formTimesheetId > 0 ? to_string(formTimesheetId).c_str() : "Select Entry")) {
            for (const TimesheetEntry& entry : entries) {
                bool isSelected = (formTimesheetId == entry.getId());
                string label = "Entry #" + to_string(entry.getId()) + " - " + entry.getDate().toString();
                if (ImGui::Selectable(label.c_str(), isSelected)) {
                    formTimesheetId = entry.getId();
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Actual Start Time:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##starthour", &formStartHour);
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##startmin", &formStartMin);
        
        ImGui::Text("Actual End Time:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##endhour", &formEndHour);
        ImGui::SameLine();
        ImGui::Text(":");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##endmin", &formEndMin);
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (ImGui::Button("Record", ImVec2(120, 0))) {
            if (formTimesheetId > 0) {
                User* hod = nullptr;
                User* instructor = nullptr;
                userRepo->findById(1, hod);
                
                TimesheetEntry entry;
                if (timesheetRepo->findById(formTimesheetId, entry)) {
                    Section section;
                    sectionRepo->findById(entry.getSectionId(), section);
                    userRepo->findById(section.getInstructorId(), instructor);
                }
                
                attendantFacade->recordMakeupLab(formTimesheetId, 
                                                Time(formStartHour, formStartMin), 
                                                Time(formEndHour, formEndMin), 
                                                hod, instructor);
                addNotification("Makeup lab recorded");
                showMakeupLabModal = false;
            } else {
                addNotification("Please select an entry", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showMakeupLabModal = false;
        }
        
        ImGui::EndPopup();
    }
}

void LabManagementGUI::renderRequestMakeupModal() {
    ImGui::OpenPopup("Request Makeup Lab");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 300));
    
    if (ImGui::BeginPopupModal("Request Makeup Lab", &showRequestMakeupModal, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Request Makeup Lab");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Text("Section:");
        ImGui::SameLine();
        vector<Section> sections = sectionRepo->findAll();
        if (ImGui::BeginCombo("##section", formSectionId > 0 ? to_string(formSectionId).c_str() : "Select Section")) {
            for (const Section& section : sections) {
                if (section.getInstructorId() == currentUser->getId()) {
                    bool isSelected = (formSectionId == section.getId());
                    Lab lab;
                    labRepo->findById(section.getLabId(), lab);
                    string label = lab.getCode() + " - " + section.getSectionName();
                    if (ImGui::Selectable(label.c_str(), isSelected)) {
                        formSectionId = section.getId();
                    }
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::Text("Desired Date (DD/MM/YYYY):");
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##day", &formDateDay);
        ImGui::SameLine();
        ImGui::Text("/");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60);
        ImGui::InputInt("##month", &formDateMonth);
        ImGui::SameLine();
        ImGui::Text("/");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::InputInt("##year", &formDateYear);
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        if (ImGui::Button("Request", ImVec2(120, 0))) {
            if (formSectionId > 0) {
                User* hod = nullptr;
                User* attendant = nullptr;
                userRepo->findById(1, hod);
                userRepo->findById(7, attendant);
                
                instructorFacade->requestMakeupLab(formSectionId, 
                                                  Date(formDateDay, formDateMonth, formDateYear), 
                                                  attendant, hod);
                addNotification("Makeup lab request submitted");
                showRequestMakeupModal = false;
            } else {
                addNotification("Please select a section", ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            showRequestMakeupModal = false;
        }
        
        ImGui::EndPopup();
    }
}

void LabManagementGUI::renderReportViewer() {
    ImGui::OpenPopup("Report Viewer");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(900, 700));
    
    if (ImGui::BeginPopupModal("Report Viewer", &showReportViewer)) {
        ImGui::Text("%s", reportTitle.c_str());
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::BeginChild("ReportContent", ImVec2(0, -40), true);
        ImGui::TextUnformatted(reportContent.c_str());
        ImGui::EndChild();
        
        if (ImGui::Button("Close", ImVec2(120, 0))) {
            showReportViewer = false;
        }
        
        ImGui::EndPopup();
    }
}