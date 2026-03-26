#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "Common.h"
#include <sys/stat.h>
#include <sys/types.h>

// Singleton FileManager (Only class that should be Singleton)
class FileManager {
private:
    static FileManager* instance;
    string dataDirectory;
    
    // Private constructor
    FileManager() : dataDirectory("data/") {
        createDirectoryIfNotExists();
    }
    
    void createDirectoryIfNotExists() {
        struct stat info;
        if (stat(dataDirectory.c_str(), &info) != 0) {
            // Directory doesn't exist, create it
            #ifdef _WIN32
                _mkdir(dataDirectory.c_str());
            #else
                mkdir(dataDirectory.c_str(), 0755);
            #endif
        }
    }
    
public:
    // Delete copy constructor and assignment operator
    FileManager(const FileManager&) = delete;
    FileManager& operator=(const FileManager&) = delete;
    
    static FileManager* getInstance() {
        if (instance == nullptr) {
            instance = new FileManager();
        }
        return instance;
    }
    
    string getFilePath(const string& filename) const {
        return dataDirectory + filename;
    }
    
    bool fileExists(const string& filename) const {
        ifstream file(getFilePath(filename));
        return file.good();
    }
    
    static void cleanup() {
        if (instance != nullptr) {
            delete instance;
            instance = nullptr;
        }
    }
};

// Initialize static member
FileManager* FileManager::instance = nullptr;

#endif