#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <fstream>
#include "FileManager.h"

using namespace std;

// Generic Repository interface (DIP - Dependency Inversion Principle)
template<typename T>
class IRepository {
public:
    virtual ~IRepository() {}
    virtual void save(const T& entity) = 0;
    virtual bool findById(int id, T& entity) = 0;
    virtual vector<T> findAll() = 0;
    virtual void update(const T& entity) = 0;
    virtual bool remove(int id) = 0;
};

#endif
