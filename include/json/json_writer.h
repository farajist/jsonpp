#ifndef __JSON_WRITER__
#define __JSON_WRITER__

#include <string>
#include <iostream>

#include "../common/writer.h"
// #include <typeinfo>

/**
 * an ostream wrapper class, writes given object using the JSON
 * formalism through a set of sequantial member functions calls
 * methods are diveded into 2 categories :
 *  syntax output functions : output syntax-related tokens ("{,[..")
 *  value output functions : output actual values such as ints, floats ...
 * */
template <typename TStream>
class json_writer : public writer<TStream>
{
    TStream& m_stream;
    bool m_first;
public:
    /**
     * class constructor, takes as a parameter the 
     * stream being wrapped inside the writer
     * */
    json_writer(TStream& stream) : writer<TStream>(), m_stream(stream)
    {
        m_first = false;
    }

    /**
     * called before any object write operation, to 
     * start the serialization according to json's formalism "{"
     * sets the first boolean value for formatting purpose
     * */
    void begin_object(const char* name)
    {
        m_stream << "{";
        m_first = true;
    }

    /**
     * called after any object write operation, to 
     * end the serialization according to json's formalism "}"
     * */
    void end_object(const char* name = "")
    {
        m_stream << "}";
    }
    
    /**
     * called at the beginning of an object's propertywrite operation,
     *  put a comma (if the property is not first), and then outputs the 
     * attribute name with escaped quotes
     * */
    void begin_prop(const char* name)
    {
        if (m_first)
            m_first = false;
        else
            m_stream << ",";
        m_stream << "\"" << name << "\":";
    }
    
     /**
     * called at the end of an object's write operation,
     * does nothing :3
     * */

    void end_prop(const char* name = "")
    {
    }
    /**
     * writes an integer value to the given stream
     * */
    void write_value(int value)
    {
        m_stream << value;
    }
    
    /**
     * writes a float value to the given stream
     * */
    void write_value(float value)
    {
        m_stream << value;
    }

    /**
     * writes a string value to the given stream,
     * if the stream contains non-visible characters, they'll
     * be escaped (replaced with equivalent escape string)
     * */
    void write_value(const std::string& str)
    {
        m_stream << "\"";   
        for (char c : str)
        {
            switch(c) 
            {
                case '\t' :
                    m_stream << "\\t";
                    break;
                case '\f' :
                    m_stream << "\\f";
                    break;
                case '\r' :
                    m_stream << "\\r";
                    break;
                case '\n' :
                    m_stream << "\\n";
                    break;
                case '\\' :
                    m_stream << "\\\\";
                    break;
                case '\b' :
                    m_stream << "\\b";
                    break;
                default:
                    m_stream << c;
                    break;
            }
        }
        m_stream << "\"";
        // std::cout << typeid(m_stream).name() << std::endl;
    }
    /**
     * writes a boolean value,
     * boolean value is either "true" for true values
     * or "false" otherwise
     * */
    void write_value(bool b) 
    {
        if (b)
        {
            m_stream << "true";
        }
        else 
        {
            m_stream << "false";
        }
    }

    /**
     * called before any array write operation, to 
     * start the serialization according to json's formalism "["
     * */

    void begin_array()
    {
        m_stream << "[";
        m_first = true;
    }

    /**
     * called after any array write operation, to 
     * end the serialization according to json's formalism "]"
     * */
    void end_array()
    {
        m_stream << "]";
    }

    /**
     * this belongs to map stuff maybe xD
     * */
    void begin_item()
    {
        if (m_first)
            m_first = false;
        else
            m_stream << ",";
    }

    void end_item()
    {
    }
};

/**
 * global helper, returns an instance of the writer 
 * when passed a stream object to conduct a write operation
 * */
template <typename TStream>
json_writer<TStream> get_json_writer(TStream& stream)
{
    return json_writer<TStream>(stream);
}
#endif /* __JSON_WRITER__ */