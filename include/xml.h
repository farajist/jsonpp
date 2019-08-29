#ifndef __XML_H__
#define __XML_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "xml/xml_writer.h"
#include "xml/xml_reader.h"
#include "common/class_descriptor.h"
#include "common/map_type_descriptor.h"
#include "common/array_type_descriptor.h"
#include "common/vector_type_descriptor.h"
/**
 * forward declaration for the write_object function
 * defined in class_descriptor.h
 * */
template <typename TWriter, typename T>
void write_object(TWriter& writer, const T& t);

/**
 * forward declaration for the read_object function
 * defined in class_descriptor.h
 * */
template <typename TReader, typename T>
void read_object(TReader& reader, T& t);

/**
 * json writing helper, dumps json-serialized object
 * to the stream being wrapped in the writer  
 * ......
 * */
template <typename TStream, typename T>
void write_xml(xml_writer<TStream> &writer, const T& t)
{
    write_object(writer, t);
}

/**
 * json writing helper, dumps json-serialized object
 * to the stream passed as a parameter 
 * ......
 * */

template <typename TStream, typename T>
void write_xml(TStream &stream, const T& t)
{
    auto writer = get_xml_writer(stream);
    write_object(writer, t);
}

/**
 * client interface function, simply serializes a visitable 
 * object (one that has a class_descriptor) and returns 
 * the result as a string object
 * */

// template <typename T>
// std::string to_xml(const T& t)
// {
//     std::stringstream sout;
//     write_xml(sout, t);
//     return sout.str();
// }

template <typename TStream, typename T>
void read_xml(xml_reader<TStream>& reader, T& t)
{
    read_object(reader, t);
}

template <typename TStream, typename T>
void read_xml(TStream& stream, T& t)
{
    auto reader = get_xml_reader(stream); 
    read_object(reader, t);
}

/**
 * client interface function, simply deserializes a visitable 
 * object (one that has a class_descriptor) and returns 
 * the result as a reference to the instance object 
 * */

// template <typename T>
// void from_xml(T& t, const std::string& json)
// {
//     std::stringstream sin(json);
//     read_xml(sin, t);
// }

#endif /* __JSON_H__ */