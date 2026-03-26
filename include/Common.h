#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

class Observer;

enum class UserRole {
    INSTRUCTOR,
    TA,
    ATTENDANT,
    ACADEMIC_OFFICER,
    HOD
};

struct Time {
    int hour;
    int minute;
    
    Time() : hour(0), minute(0) {}
    Time(int h, int m) : hour(h), minute(m) {}
    
    string toString() const {
        stringstream ss;
        ss << setfill('0') << setw(2) << hour << ":" 
           << setfill('0') << setw(2) << minute;
        return ss.str();
    }
    
    void serialize(ofstream& out) const {
        out.write((char*)&hour, sizeof(hour));
        out.write((char*)&minute, sizeof(minute));
    }
    
    void deserialize(ifstream& in) {
        in.read((char*)&hour, sizeof(hour));
        in.read((char*)&minute, sizeof(minute));
    }
};

struct Date {
    int day;
    int month;
    int year;
    
    Date() : day(1), month(1), year(2024) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
    
    string toString() const {
        stringstream ss;
        ss << setfill('0') << setw(2) << day << "/" 
           << setfill('0') << setw(2) << month << "/" << year;
        return ss.str();
    }
    
    void serialize(ofstream& out) const {
        out.write((char*)&day, sizeof(day));
        out.write((char*)&month, sizeof(month));
        out.write((char*)&year, sizeof(year));
    }
    
    void deserialize(ifstream& in) {
        in.read((char*)&day, sizeof(day));
        in.read((char*)&month, sizeof(month));
        in.read((char*)&year, sizeof(year));
    }
};

class StringHelper {
public:
    static void writeString(ofstream& out, const string& str) {
        size_t length = str.length();
        out.write((char*)&length, sizeof(length));
        out.write(str.c_str(), length);
    }
    
    static string readString(ifstream& in) {
        size_t length;
        in.read((char*)&length, sizeof(length));
        char* buffer = new char[length + 1];
        in.read(buffer, length);
        buffer[length] = '\0';
        string result(buffer);
        delete[] buffer;
        return result;
    }
};

#endif
