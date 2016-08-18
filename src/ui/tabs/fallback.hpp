#ifndef FALLBACK_HPP_
#define FALLBACK_HPP_

#include "ui/tab.hpp"

class Fallback : public Tab
{
public:
    Fallback() {}
    virtual ~Fallback() {}

    void draw();
    void handleEvents(const char *event)
    {
        // Do nothing
    }
};

#endif // FALLBACK_HPP_
