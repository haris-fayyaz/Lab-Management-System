#include "src/GUI.h"

// Include the implementation files
#include "src/GUI_impl.cpp"
#include "src/GUI_modals.cpp"

int main() {
    LabManagementGUI gui;
    
    if (!gui.initialize()) {
        cout << "Failed to initialize GUI!" << endl;
        return -1;
    }
    
    cout << "Lab Management System GUI initialized successfully!" << endl;
    gui.run();
    
    return 0;
}