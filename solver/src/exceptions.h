#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

using namespace std;

class NoChainFoundException {
    private:
         string msg;
    public:
        NoChainFoundException();
        void showErrorMessage();
};

class NoSuchVariableException {
    private:
        int id;
    public:
        NoSuchVariableException(int id);
        void showErrorMessage();
};

#endif
