#ifndef CONFIG_H
#define CONFIG_H

// C++ standard library headers
#include <windows.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

class Config
{
public:
    // Create configuration file
    bool create();

    // Load configuration file
    bool load();

    // Get configuration item, return default value if not found
    std::string get(const std::string &key, const std::string &defaultValue = "") const;

    // Set configuration item
    void set(const std::string &key, const std::string &value);

    // Save configuration to file
    bool save() const;

    // Print all configurations
    void print() const;

private:
    std::unordered_map<std::string, std::string> configMap;
    std::string filename = "config.txt"; // Default configuration file name
};

#endif // CONFIG_H