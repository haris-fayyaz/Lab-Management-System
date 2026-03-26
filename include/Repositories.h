#ifndef REPOSITORIES_H
#define REPOSITORIES_H

#include "Repository.h"
#include "User.h"
#include "Venue.h"
#include "Lab.h"
#include "Timesheet.h"

// User Repository
class UserRepository : public IRepository<User*> {
private:
    string filename;
    FileManager* fileManager;
    
public:
    UserRepository() : filename("users.dat") {
        fileManager = FileManager::getInstance();
    }
    
    void save(User* const& user) override {
        vector<User*> users = findAll();
        users.push_back(user);
        saveAll(users);
    }
    
    bool findById(int id, User*& user) override {
        vector<User*> users = findAll();
        for (User* u : users) {
            if (u->getId() == id) {
                user = u;
                return true;
            }
        }
        return false;
    }
    
    vector<User*> findAll() override {
        vector<User*> users;
        string filepath = fileManager->getFilePath(filename);
        
        if (!fileManager->fileExists(filename)) {
            return users;
        }
        
        ifstream file(filepath, ios::binary);
        if (!file.is_open()) return users;
        
        int count;
        file.read((char*)&count, sizeof(count));
        
        for (int i = 0; i < count; i++) {
            // Read id and name first
            int id;
            file.read((char*)&id, sizeof(id));
            string name = StringHelper::readString(file);
            
            // Read role to determine type
            UserRole role;
            file.read((char*)&role, sizeof(role));
            
            User* user = nullptr;
            switch (role) {
                case UserRole::INSTRUCTOR:
                    user = new Instructor(id, name);
                    break;
                case UserRole::TA:
                    user = new TA(id, name);
                    break;
                case UserRole::ATTENDANT: {
                    int buildingId;
                    file.read((char*)&buildingId, sizeof(buildingId));
                    user = new Attendant(id, name, buildingId);
                    break;
                }
                case UserRole::ACADEMIC_OFFICER:
                    user = new AcademicOfficer(id, name);
                    break;
                case UserRole::HOD:
                    user = new HOD(id, name);
                    break;
            }
            
            if (user) {
                users.push_back(user);
            }
        }
        
        file.close();
        return users;
    }
    
    void update(User* const& user) override {
        vector<User*> users = findAll();
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i]->getId() == user->getId()) {
                delete users[i];
                users[i] = user;
                break;
            }
        }
        saveAll(users);
    }
    
    bool remove(int id) override {
        vector<User*> users = findAll();
        for (auto it = users.begin(); it != users.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                users.erase(it);
                saveAll(users);
                return true;
            }
        }
        return false;
    }
    
private:
    void saveAll(const vector<User*>& users) {
        string filepath = fileManager->getFilePath(filename);
        ofstream file(filepath, ios::binary | ios::trunc);
        
        int count = users.size();
        file.write((char*)&count, sizeof(count));
        
        for (User* user : users) {
            user->serialize(file);
        }
        
        file.close();
    }
};

// Lab Repository
class LabRepository : public IRepository<Lab> {
private:
    string filename;
    FileManager* fileManager;
    
public:
    LabRepository() : filename("labs.dat") {
        fileManager = FileManager::getInstance();
    }
    
    void save(const Lab& lab) override {
        vector<Lab> labs = findAll();
        labs.push_back(lab);
        saveAll(labs);
    }
    
    bool findById(int id, Lab& lab) override {
        vector<Lab> labs = findAll();
        for (const Lab& l : labs) {
            if (l.getId() == id) {
                lab = l;
                return true;
            }
        }
        return false;
    }
    
    vector<Lab> findAll() override {
        vector<Lab> labs;
        string filepath = fileManager->getFilePath(filename);
        
        if (!fileManager->fileExists(filename)) {
            return labs;
        }
        
        ifstream file(filepath, ios::binary);
        if (!file.is_open()) return labs;
        
        int count;
        file.read((char*)&count, sizeof(count));
        
        for (int i = 0; i < count; i++) {
            Lab lab;
            lab.deserialize(file);
            labs.push_back(lab);
        }
        
        file.close();
        return labs;
    }
    
    void update(const Lab& lab) override {
        vector<Lab> labs = findAll();
        for (size_t i = 0; i < labs.size(); i++) {
            if (labs[i].getId() == lab.getId()) {
                labs[i] = lab;
                break;
            }
        }
        saveAll(labs);
    }
    
    bool remove(int id) override {
        vector<Lab> labs = findAll();
        for (auto it = labs.begin(); it != labs.end(); ++it) {
            if (it->getId() == id) {
                labs.erase(it);
                saveAll(labs);
                return true;
            }
        }
        return false;
    }
    
private:
    void saveAll(const vector<Lab>& labs) {
        string filepath = fileManager->getFilePath(filename);
        ofstream file(filepath, ios::binary | ios::trunc);
        
        int count = labs.size();
        file.write((char*)&count, sizeof(count));
        
        for (const Lab& lab : labs) {
            lab.serialize(file);
        }
        
        file.close();
    }
};

// Section Repository
class SectionRepository : public IRepository<Section> {
private:
    string filename;
    FileManager* fileManager;
    
public:
    SectionRepository() : filename("sections.dat") {
        fileManager = FileManager::getInstance();
    }
    
    void save(const Section& section) override {
        vector<Section> sections = findAll();
        sections.push_back(section);
        saveAll(sections);
    }
    
    bool findById(int id, Section& section) override {
        vector<Section> sections = findAll();
        for (const Section& s : sections) {
            if (s.getId() == id) {
                section = s;
                return true;
            }
        }
        return false;
    }
    
    vector<Section> findAll() override {
        vector<Section> sections;
        string filepath = fileManager->getFilePath(filename);
        
        if (!fileManager->fileExists(filename)) {
            return sections;
        }
        
        ifstream file(filepath, ios::binary);
        if (!file.is_open()) return sections;
        
        int count;
        file.read((char*)&count, sizeof(count));
        
        for (int i = 0; i < count; i++) {
            Section section;
            section.deserialize(file);
            sections.push_back(section);
        }
        
        file.close();
        return sections;
    }
    
    void update(const Section& section) override {
        vector<Section> sections = findAll();
        for (size_t i = 0; i < sections.size(); i++) {
            if (sections[i].getId() == section.getId()) {
                sections[i] = section;
                break;
            }
        }
        saveAll(sections);
    }
    
    bool remove(int id) override {
        vector<Section> sections = findAll();
        for (auto it = sections.begin(); it != sections.end(); ++it) {
            if (it->getId() == id) {
                sections.erase(it);
                saveAll(sections);
                return true;
            }
        }
        return false;
    }
    
    vector<Section> findByLabId(int labId) {
        vector<Section> result;
        vector<Section> sections = findAll();
        for (const Section& s : sections) {
            if (s.getLabId() == labId) {
                result.push_back(s);
            }
        }
        return result;
    }
    
private:
    void saveAll(const vector<Section>& sections) {
        string filepath = fileManager->getFilePath(filename);
        ofstream file(filepath, ios::binary | ios::trunc);
        
        int count = sections.size();
        file.write((char*)&count, sizeof(count));
        
        for (const Section& section : sections) {
            section.serialize(file);
        }
        
        file.close();
    }
};

// Timesheet Repository
class TimesheetRepository : public IRepository<TimesheetEntry> {
private:
    string filename;
    FileManager* fileManager;
    
public:
    TimesheetRepository() : filename("timesheets.dat") {
        fileManager = FileManager::getInstance();
    }
    
    void save(const TimesheetEntry& entry) override {
        vector<TimesheetEntry> entries = findAll();
        entries.push_back(entry);
        saveAll(entries);
    }
    
    bool findById(int id, TimesheetEntry& entry) override {
        vector<TimesheetEntry> entries = findAll();
        for (const TimesheetEntry& e : entries) {
            if (e.getId() == id) {
                entry = e;
                return true;
            }
        }
        return false;
    }
    
    vector<TimesheetEntry> findAll() override {
        vector<TimesheetEntry> entries;
        string filepath = fileManager->getFilePath(filename);
        
        if (!fileManager->fileExists(filename)) {
            return entries;
        }
        
        ifstream file(filepath, ios::binary);
        if (!file.is_open()) return entries;
        
        int count;
        file.read((char*)&count, sizeof(count));
        
        for (int i = 0; i < count; i++) {
            TimesheetEntry entry;
            entry.deserialize(file);
            entries.push_back(entry);
        }
        
        file.close();
        return entries;
    }
    
    void update(const TimesheetEntry& entry) override {
        vector<TimesheetEntry> entries = findAll();
        for (size_t i = 0; i < entries.size(); i++) {
            if (entries[i].getId() == entry.getId()) {
                entries[i] = entry;
                break;
            }
        }
        saveAll(entries);
    }
    
    bool remove(int id) override {
        vector<TimesheetEntry> entries = findAll();
        for (auto it = entries.begin(); it != entries.end(); ++it) {
            if (it->getId() == id) {
                entries.erase(it);
                saveAll(entries);
                return true;
            }
        }
        return false;
    }
    
    vector<TimesheetEntry> findBySectionId(int sectionId) {
        vector<TimesheetEntry> result;
        vector<TimesheetEntry> entries = findAll();
        for (const TimesheetEntry& e : entries) {
            if (e.getSectionId() == sectionId) {
                result.push_back(e);
            }
        }
        return result;
    }
    
private:
    void saveAll(const vector<TimesheetEntry>& entries) {
        string filepath = fileManager->getFilePath(filename);
        ofstream file(filepath, ios::binary | ios::trunc);
        
        int count = entries.size();
        file.write((char*)&count, sizeof(count));
        
        for (const TimesheetEntry& entry : entries) {
            entry.serialize(file);
        }
        
        file.close();
    }
};

// Building Repository
class BuildingRepository : public IRepository<Building> {
private:
    string filename;
    FileManager* fileManager;
    
public:
    BuildingRepository() : filename("buildings.dat") {
        fileManager = FileManager::getInstance();
    }
    
    void save(const Building& building) override {
        vector<Building> buildings = findAll();
        buildings.push_back(building);
        saveAll(buildings);
    }
    
    bool findById(int id, Building& building) override {
        vector<Building> buildings = findAll();
        for (const Building& b : buildings) {
            if (b.getId() == id) {
                building = b;
                return true;
            }
        }
        return false;
    }
    
    vector<Building> findAll() override {
        vector<Building> buildings;
        string filepath = fileManager->getFilePath(filename);
        
        if (!fileManager->fileExists(filename)) {
            return buildings;
        }
        
        ifstream file(filepath, ios::binary);
        if (!file.is_open()) return buildings;
        
        int count;
        file.read((char*)&count, sizeof(count));
        
        for (int i = 0; i < count; i++) {
            Building building;
            building.deserialize(file);
            buildings.push_back(building);
        }
        
        file.close();
        return buildings;
    }
    
    void update(const Building& building) override {
        vector<Building> buildings = findAll();
        for (size_t i = 0; i < buildings.size(); i++) {
            if (buildings[i].getId() == building.getId()) {
                buildings[i] = building;
                break;
            }
        }
        saveAll(buildings);
    }
    
    bool remove(int id) override {
        vector<Building> buildings = findAll();
        for (auto it = buildings.begin(); it != buildings.end(); ++it) {
            if (it->getId() == id) {
                buildings.erase(it);
                saveAll(buildings);
                return true;
            }
        }
        return false;
    }
    
private:
    void saveAll(const vector<Building>& buildings) {
        string filepath = fileManager->getFilePath(filename);
        ofstream file(filepath, ios::binary | ios::trunc);
        
        int count = buildings.size();
        file.write((char*)&count, sizeof(count));
        
        for (const Building& building : buildings) {
            building.serialize(file);
        }
        
        file.close();
    }
};

// Room Repository
class RoomRepository : public IRepository<Room> {
private:
    string filename;
    FileManager* fileManager;
    
public:
    RoomRepository() : filename("rooms.dat") {
        fileManager = FileManager::getInstance();
    }
    
    void save(const Room& room) override {
        vector<Room> rooms = findAll();
        rooms.push_back(room);
        saveAll(rooms);
    }
    
    bool findById(int id, Room& room) override {
        vector<Room> rooms = findAll();
        for (const Room& r : rooms) {
            if (r.getId() == id) {
                room = r;
                return true;
            }
        }
        return false;
    }
    
    vector<Room> findAll() override {
        vector<Room> rooms;
        string filepath = fileManager->getFilePath(filename);
        
        if (!fileManager->fileExists(filename)) {
            return rooms;
        }
        
        ifstream file(filepath, ios::binary);
        if (!file.is_open()) return rooms;
        
        int count;
        file.read((char*)&count, sizeof(count));
        
        for (int i = 0; i < count; i++) {
            Room room;
            room.deserialize(file);
            rooms.push_back(room);
        }
        
        file.close();
        return rooms;
    }
    
    void update(const Room& room) override {
        vector<Room> rooms = findAll();
        for (size_t i = 0; i < rooms.size(); i++) {
            if (rooms[i].getId() == room.getId()) {
                rooms[i] = room;
                break;
            }
        }
        saveAll(rooms);
    }
    
    bool remove(int id) override {
        vector<Room> rooms = findAll();
        for (auto it = rooms.begin(); it != rooms.end(); ++it) {
            if (it->getId() == id) {
                rooms.erase(it);
                saveAll(rooms);
                return true;
            }
        }
        return false;
    }
    
private:
    void saveAll(const vector<Room>& rooms) {
        string filepath = fileManager->getFilePath(filename);
        ofstream file(filepath, ios::binary | ios::trunc);
        
        int count = rooms.size();
        file.write((char*)&count, sizeof(count));
        
        for (const Room& room : rooms) {
            room.serialize(file);
        }
        
        file.close();
    }
};

#endif