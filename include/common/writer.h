#ifndef __WRITER__
#define __WRITER__

#include <string>
#include <iostream>
template <typename TStream>
class writer
{
    // TStream& m_stream;
public:
    writer() {}
  
    virtual void begin_object(const char* name) = 0;
    virtual void end_object(const char* name = "") = 0;
  
    virtual void begin_prop(const char* name) = 0;
    virtual void end_prop(const char* name = "") = 0;
    
    virtual void write_value(int value) = 0;
    virtual void write_value(float value) = 0;
    virtual void write_value(const std::string& str) = 0;
    virtual void write_value(bool b)  = 0;
   
    virtual void begin_array() = 0;
    virtual void end_array() = 0;
    
    virtual void begin_item() = 0;
    virtual void end_item() = 0;
   
};

#endif /* __JSON_WRITER__ */