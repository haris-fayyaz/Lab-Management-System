#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>
#include <string>

using namespace std;

// Observer interface
class Observer {
public:
    virtual ~Observer() {}
    virtual void update(const string& message) = 0;
};

// Subject interface
class Subject {
protected:
    vector<Observer*> observers;
    
public:
    virtual ~Subject() {}
    
    void attach(Observer* observer) {
        observers.push_back(observer);
    }
    
    void detach(Observer* observer) {
        for (auto it = observers.begin(); it != observers.end(); ++it) {
            if (*it == observer) {
                observers.erase(it);
                break;
            }
        }
    }
    
    void notify(const string& message) {
        for (Observer* observer : observers) {
            observer->update(message);
        }
    }
};

#endif
