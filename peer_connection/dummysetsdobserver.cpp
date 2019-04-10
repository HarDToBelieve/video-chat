#include "dummysetsdobserver.h"

void DummySetSDObserver::OnSuccess()
{
    std::cerr << "DummySetSessionDescriptionObserver: onSuccess" << std::endl;
}

void DummySetSDObserver::OnFailure(const std::string& error)
{
    std::cerr << "DummySetSessionDescriptionObserver: OnFailure (due to " << error << ")" << std::endl;
}
