#ifndef VENUE_H
#define VENUE_H

#include "Common.h"

// Building class
class Building {
private:
    int id;
    string name;
    int attendantId;
    
public:
    Building() : id(0), name(""), attendantId(0) {}
    Building(int id, const string& name, int attId) 
        : id(id), name(name), attendantId(attId) {}
    
    int getId() const { return id; }
    string getName() const { return name; }
    int getAttendantId() const { return attendantId; }
    
    void serialize(ofstream& out) const {
        out.write((char*)&id, sizeof(id));
        StringHelper::writeString(out, name);
        out.write((char*)&attendantId, sizeof(attendantId));
    }
    
    void deserialize(ifstream& in) {
        in.read((char*)&id, sizeof(id));
        name = StringHelper::readString(in);
        in.read((char*)&attendantId, sizeof(attendantId));
    }
};

// Room class
class Room {
private:
    int id;
    string roomNumber;
    int buildingId;
    int capacity;
    
public:
    Room() : id(0), roomNumber(""), buildingId(0), capacity(0) {}
    Room(int id, const string& roomNum, int bId, int cap) 
        : id(id), roomNumber(roomNum), buildingId(bId), capacity(cap) {}
    
    int getId() const { return id; }
    string getRoomNumber() const { return roomNumber; }
    int getBuildingId() const { return buildingId; }
    int getCapacity() const { return capacity; }
    
    void serialize(ofstream& out) const {
        out.write((char*)&id, sizeof(id));
        StringHelper::writeString(out, roomNumber);
        out.write((char*)&buildingId, sizeof(buildingId));
        out.write((char*)&capacity, sizeof(capacity));
    }
    
    void deserialize(ifstream& in) {
        in.read((char*)&id, sizeof(id));
        roomNumber = StringHelper::readString(in);
        in.read((char*)&buildingId, sizeof(buildingId));
        in.read((char*)&capacity, sizeof(capacity));
    }
};

#endif
