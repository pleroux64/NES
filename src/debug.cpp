#include "debug.h"

// Global variables to manage the custom buffer
std::streambuf *originalCerrBuffer = nullptr;
FilteringStreambuf *customBuffer = nullptr;

// Constructor for FilteringStreambuf
FilteringStreambuf::FilteringStreambuf(std::streambuf *originalBuffer)
    : originalBuffer(originalBuffer) {}

// Set the log filter
void FilteringStreambuf::setFilter(std::function<bool(const std::string &)> filter)
{
    this->filter = filter;
}

// Handle sync calls to process complete lines
int FilteringStreambuf::sync()
{
    if (!buffer.empty())
    {
        if (!filter || filter(buffer)) // Apply filter if set
        {
            originalBuffer->sputn(buffer.c_str(), buffer.size());
            originalBuffer->sputc('\n'); // Add newline for line isolation
        }
        buffer.clear(); // Clear the internal buffer
    }
    return originalBuffer->pubsync(); // Ensure original buffer is flushed
}

// Capture characters and handle newlines
int FilteringStreambuf::overflow(int ch)
{
    if (ch != EOF)
    {
        buffer += static_cast<char>(ch);
        if (ch == '\n') // Trigger sync on newline
        {
            sync();
        }
    }
    return ch;
}

// Initialize the logger
void initializeLogger()
{
    originalCerrBuffer = std::cerr.rdbuf(); // Save the original buffer
    customBuffer = new FilteringStreambuf(originalCerrBuffer);
    std::cerr.rdbuf(customBuffer); // Redirect std::cerr to use the custom buffer
}

// Clean up the logger
void cleanupLogger()
{
    std::cerr.rdbuf(originalCerrBuffer); // Restore the original buffer
    delete customBuffer;
}

// Set the log filter dynamically
void setLogFilter(std::function<bool(const std::string &)> filter)
{
    if (customBuffer)
    {
        customBuffer->setFilter(filter);
    }
}
