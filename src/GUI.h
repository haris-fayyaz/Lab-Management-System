#ifndef GUI_H
#define GUI_H

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include "../include/Common.h"
#include "../include/Observer.h"
#include "../include/User.h"
#include "../include/Venue.h"
#include "../include/Lab.h"
#include "../include/Timesheet.h"
#include "../include/FileManager.h"
#include "../include/Repository.h"
#include "../include/Repositories.h"
#include "../include/Report.h"
#include "../include/Factory.h"
#include "../include/Facade.h"

#include <string>
#include <vector>
#include <ctime>

// Notification system
struct Notification {
    string message;
    float displayTime;
    ImVec4 color;
    
    Notification(const string& msg, float time = 3.0f, ImVec4 col = ImVec4(0.2f, 0.8f, 0.2f, 1.0f))
        : message(msg), displayTime(time), color(col) {}
};

// Makeup Lab Scheduler (acts as Subject for notifications)
class MakeupLabScheduler : public Subject {
public:
    void scheduleMakeupLab(int sectionId, Date date) {
        notify("Makeup lab scheduled for Section " + to_string(sectionId) + 
               " on " + date.toString());
    }
};

class LabManagementGUI {
private:
    // GLFW Window
    GLFWwindow* window;
    
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
    bool isLoggedIn;
    
    // UI State
    int selectedUserIndex;
    vector<User*> allUsers;
    vector<Notification> notifications;
    
    // Modal states
    bool showCreateLabModal;
    bool showCreateSectionModal;
    bool showAssignScheduleModal;
    bool showAssignTAModal;
    bool showCreateTimesheetModal;
    bool showFillTimesheetModal;
    bool showMakeupLabModal;
    bool showRequestMakeupModal;
    bool showReportViewer;
    string reportContent;
    string reportTitle;
    
    // Form data
    char labCodeBuf[64];
    char labNameBuf[128];
    char sectionNameBuf[64];
    int formLabId;
    int formSectionId;
    int formRoomId;
    int formInstructorId;
    int formTAId;
    int formDay;
    int formStartHour, formStartMin;
    int formEndHour, formEndMin;
    int formTimesheetId;
    int formDateDay, formDateMonth, formDateYear;
    
public:
    LabManagementGUI() 
        : window(nullptr), currentUser(nullptr), isLoggedIn(false),
          selectedUserIndex(0), showCreateLabModal(false), showCreateSectionModal(false),
          showAssignScheduleModal(false), showAssignTAModal(false),
          showCreateTimesheetModal(false), showFillTimesheetModal(false),
          showMakeupLabModal(false), showRequestMakeupModal(false),
          showReportViewer(false), formLabId(1), formSectionId(1), formRoomId(1),
          formInstructorId(1), formTAId(1), formDay(1), formStartHour(9),
          formStartMin(0), formEndHour(11), formEndMin(0), formTimesheetId(1),
          formDateDay(1), formDateMonth(1), formDateYear(2024) {
        
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
        
        // Initialize buffers
        memset(labCodeBuf, 0, sizeof(labCodeBuf));
        memset(labNameBuf, 0, sizeof(labNameBuf));
        memset(sectionNameBuf, 0, sizeof(sectionNameBuf));
        
        initializeSampleData();
        loadAllUsers();
    }
    
    ~LabManagementGUI() {
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
        
        if (window) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            glfwDestroyWindow(window);
            glfwTerminate();
        }
    }
    
    bool initialize() {
        // Initialize GLFW
        if (!glfwInit()) {
            return false;
        }
        
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        
        // Create window
        window = glfwCreateWindow(1600, 900, "Lab Management System", NULL, NULL);
        if (window == NULL) {
            glfwTerminate();
            return false;
        }
        
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync
        
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        
        // Setup style
        setupStyle();
        
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        
        return true;
    }
    
    void setupStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        
        // Modern colors
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.50f, 0.80f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.60f, 0.90f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.45f, 0.75f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.50f, 0.80f, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.60f, 0.90f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.55f, 0.85f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.60f, 0.90f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.50f, 0.80f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.15f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
        
        // Rounding
        style.WindowRounding = 6.0f;
        style.FrameRounding = 4.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;
        style.ScrollbarRounding = 4.0f;
        
        // Padding
        style.WindowPadding = ImVec2(12, 12);
        style.FramePadding = ImVec2(8, 4);
        style.ItemSpacing = ImVec2(8, 6);
    }
    
    void run() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            
            // Start ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            // Render UI
            if (!isLoggedIn) {
                renderLoginScreen();
            } else {
                renderMainInterface();
            }
            
            // Render notifications
            renderNotifications();
            
            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.10f, 0.10f, 0.12f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            glfwSwapBuffers(window);
            
            // Update notifications
            updateNotifications();
        }
    }
    
private:
    void initializeSampleData() {
        if (!userRepo->findAll().empty()) {
            return;
        }
        
        // Create users
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
    }
    
    void loadAllUsers() {
        allUsers = userRepo->findAll();
    }
    
    void addNotification(const string& message, ImVec4 color = ImVec4(0.2f, 0.8f, 0.2f, 1.0f)) {
        notifications.push_back(Notification(message, 3.0f, color));
    }
    
    void updateNotifications() {
        float deltaTime = ImGui::GetIO().DeltaTime;
        for (auto it = notifications.begin(); it != notifications.end();) {
            it->displayTime -= deltaTime;
            if (it->displayTime <= 0) {
                it = notifications.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void renderNotifications() {
        ImGuiIO& io = ImGui::GetIO();
        float padding = 10.0f;
        float yOffset = io.DisplaySize.y - padding;
        
        for (int i = notifications.size() - 1; i >= 0; i--) {
            const Notification& notif = notifications[i];
            ImVec2 textSize = ImGui::CalcTextSize(notif.message.c_str());
            ImVec2 windowSize(textSize.x + 20, textSize.y + 10);
            
            yOffset -= windowSize.y + 5;
            
            ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - windowSize.x - padding, yOffset));
            ImGui::SetNextWindowSize(windowSize);
            
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f);
            ImGui::PushStyleColor(ImGuiCol_WindowBg, notif.color);
            
            string windowName = "##notification" + to_string(i);
            ImGui::Begin(windowName.c_str(), NULL, 
                        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
            
            ImGui::TextUnformatted(notif.message.c_str());
            
            ImGui::End();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();
        }
    }
    
    void renderLoginScreen();
    void renderMainInterface();
    void renderAcademicOfficerTab();
    void renderAttendantTab();
    void renderHODTab();
    void renderInstructorTab();
    
    // Modal dialogs
    void renderCreateLabModal();
    void renderCreateSectionModal();
    void renderAssignScheduleModal();
    void renderAssignTAModal();
    void renderCreateTimesheetModal();
    void renderFillTimesheetModal();
    void renderMakeupLabModal();
    void renderRequestMakeupModal();
    void renderReportViewer();
};

#endif // GUI_H