#ifndef FILTERING_STREAMBUF_H
#define FILTERING_STREAMBUF_H

#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <functional>

// Custom stream buffer class
class FilteringStreambuf : public std::streambuf
{
public:
    FilteringStreambuf(std::streambuf *originalBuffer);
    void setFilter(std::function<bool(const std::string &)> filter);

protected:
    int sync() override;        // Handle complete lines
    int overflow(int ch) override; // Capture characters into a buffer

private:
    std::string buffer;          // Stores the current line being logged
    std::streambuf *originalBuffer; // The original stream buffer to forward logs
    std::function<bool(const std::string &)> filter; // Log filter function
};

// Functions to initialize and clean up the logger
void initializeLogger();
void cleanupLogger();
void setLogFilter(std::function<bool(const std::string &)> filter);

#endif
