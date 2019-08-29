#ifndef __TSL_H__
#define __TSL_H__

#include <sstream>
#include "json.h"
#include "xml.h"
/**
 * client interface function, simply deserializes a visitable 
 * object (one that has a class_descriptor) and returns 
 * the result as a reference to the instance object 
 * */

template <typename T>
void from_json(T& t, const std::string& json)
{
    std::stringstream sin(json);
    read_json(sin, t);
}

/**
 * client interface function, simply serializes a visitable 
 * object (one that has a class_descriptor) and returns 
 * the result as a string object
 * */

template <typename T>
std::string to_json(const T& t)
{
    std::stringstream sout;
    write_json(sout, t);
    return sout.str();
}


/**
 * client interface function, simply deserializes a visitable 
 * object (one that has a class_descriptor) and returns 
 * the result as a reference to the instance object 
 * */

template <typename T>
void from_xml(T& t, const std::string& json)
{
    std::stringstream sin(json);
    read_xml(sin, t);
}


/**
 * client interface function, simply serializes a visitable 
 * object (one that has a class_descriptor) and returns 
 * the result as a string object
 * */

template <typename T>
std::string to_xml(const T& t)
{
    std::stringstream sout;
    write_xml(sout, t);
    return sout.str();
}


#endif /* __TSL_H__ */