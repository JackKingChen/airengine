#pragma once 

template <typename T>
class Singleton
{
public:
    inline static T* GetInstance()
    {
        static T singleton;
        return &singleton;    
    }

};

// A helper macro to make a class singleton
#define BE_SINGLETON(klass) friend class Singleton<klass>; \
        private: klass()
