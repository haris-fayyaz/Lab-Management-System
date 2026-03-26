#ifndef USER_H
#define USER_H

#include "Common.h"
#include "Observer.h"

// Base User class (follows SRP)
class User : public Observer {
protected:
    int id;
    string name;
    UserRole role;
    
public:
    User() : id(0), name(""), role(UserRole::INSTRUCTOR) {}
    User(int id, const string& name, UserRole role) 
        : id(id), name(name), role(role) {}
    
    virtual ~User() {}
    
    int getId() const { return id; }
    string getName() const { return name; }
    UserRole getRole() const { return role; }
    
    virtual void serialize(ofstream& out) const {
        out.write((char*)&id, sizeof(id));
        StringHelper::writeString(out, name);
        out.write((char*)&role, sizeof(role));
    }
    
    virtual void deserialize(ifstream& in) {
        in.read((char*)&id, sizeof(id));
        name = StringHelper::readString(in);
        in.read((char*)&role, sizeof(role));
    }
    
    // Observer implementation
    void update(const string& message) override {
        cout << "[" << name << " - Notification]: " << message << endl;
    }
};

// Instructor class
class Instructor : public User {
public:
    Instructor() : User() { role = UserRole::INSTRUCTOR; }
    Instructor(int id, const string& name) 
        : User(id, name, UserRole::INSTRUCTOR) {}
};

// TA class
class TA : public User {
public:
    TA() : User() { role = UserRole::TA; }
    TA(int id, const string& name) 
        : User(id, name, UserRole::TA) {}
};

// Attendant class
class Attendant : public User {
private:
    int buildingId;
    
public:
    Attendant() : User(), buildingId(0) { role = UserRole::ATTENDANT; }
    Attendant(int id, const string& name, int bId) 
        : User(id, name, UserRole::ATTENDANT), buildingId(bId) {}
    
    int getBuildingId() const { return buildingId; }
    
    void serialize(ofstream& out) const override {
        User::serialize(out);
        out.write((char*)&buildingId, sizeof(buildingId));
    }
    
    void deserialize(ifstream& in) override {
        User::deserialize(in);
        in.read((char*)&buildingId, sizeof(buildingId));
    }
};

// Academic Officer class
class AcademicOfficer : public User {
public:
    AcademicOfficer() : User() { role = UserRole::ACADEMIC_OFFICER; }
    AcademicOfficer(int id, const string& name) 
        : User(id, name, UserRole::ACADEMIC_OFFICER) {}
};

// HOD class
class HOD : public User {
public:
    HOD() : User() { role = UserRole::HOD; }
    HOD(int id, const string& name) 
        : User(id, name, UserRole::HOD) {}
};

#endif
