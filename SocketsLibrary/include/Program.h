#pragma once
#include <iostream>

#include "NetSocket.h"
#include "GetError.h"

namespace Net {
    class Program
    {
    public:
        Program()
        {
            if (startup() != 0)
            {
                std::cerr << GetError::getLastErrorFullMessage() << std::endl;
                throw std::runtime_error("Network startup failed");
            }
            started = true;
        }

        ~Program()
        {
            Stop();
        }
    private:
        bool started = false;

        void Stop()
        {
            if (started)
            {
                if (cleanup() != 0)
                {
                    std::cerr << GetError::getLastErrorFullMessage() << std::endl;
                }
                started = false;
            }
        }
    };

}