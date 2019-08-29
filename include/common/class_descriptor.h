#ifndef __CLASS_DESCRIPTOR__
#define __CLASS_DESCRIPTOR__
#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstddef>


/** =======================================================
 *  primitive_type_descriptor
 *  ===================================================== */

/**
 * class descriptor made for primitive types values
 * (int, float, char, double .....)
 * it's purpose (along with other class descriptors)
 * is to allow json writing process to be
 * overloaded across all possible data types !
 * */

template <typename T>
class primitive_type_descriptor
{
};

/** =======================================================
 *  class_descriptor
 *  ===================================================== */

/**
 *  unless overriden by a specific class type descriptor
 * (see foo and bar demos), a class descriptor has a 
 * primitive type descriptor by default
 * */

template <typename T>
class class_descriptor
{
  public:
    using descriptor_t = primitive_type_descriptor<T>;
};
/** =======================================================
 *  class descriptor marcro stub
 *  ===================================================== */

/**
 * generates class descriptors for each class to make 
 * it a serializable object, class name + target attributes
 * must be provided as arguments !
 * */
#define BEGIN_CLASS_DESCRIPTOR(X) \
    template<> \
    class class_descriptor<X> \
    { \
    public: \
        typedef X class_t; \
        typedef class_descriptor<X> descriptor_t; \
        constexpr const char* get_name() const {return #X; } \
        template <typename TCallback> \
        void foreach_prop(TCallback& callback) const {
#define CLASS_DESCRIPTOR_ENTRY(X) callback(#X, &class_t::X);
#define END_CLASS_DESCRIPTOR() }};

/** =======================================================
 *  class_descriptor getter
 *  ===================================================== */
/**
 * returns an instance of type_descriptor, for example :
 * given an int, this getter will yield a primitive_type_descriptor
 * called by the high level interface functions, to be passed
 * with a dispatch call 
 * */
template <typename T>
typename class_descriptor<T>::descriptor_t get_type_descriptor(const T &t)
{
    return typename class_descriptor<T>::descriptor_t{};
}

/** =======================================================
 *  read/write functors
 *  ===================================================== */

/**
 * serialization object function, holds a reference to 
 * the object to be serialized, target stream is wrapped 
 * inside the writer reference instance, m_first indicates
 * the state of write operation (formatting purpose)
 * */
template <typename TWriter, typename TClass>
class write_object_functor
{
    TWriter &m_writer;
    const TClass &m_t;
    bool m_first;

  public:
    write_object_functor(TWriter &writer, const TClass &t)
        : m_writer(writer), m_t(t)
    {
        m_first = true;
    }
    /**
     * callable class functions, that's called for each serialization 
     * operation of an object
     * */
    template <typename TPropType>
    void operator()(const char *sz_prop, TPropType TClass::*p_prop_offset)
    {
        m_writer.begin_prop(sz_prop);
        write_object(m_writer, m_t.*p_prop_offset);
        m_writer.end_prop(sz_prop);
    }
};

/**
 * deserialization object function, holds a reference to 
 * the object to be serialized, source stream is wrapped 
 * inside the writer reference instance, m_bfound indicates
 * whether the expected attribures was found or not
 * */
template <typename TReader, typename TClass>
class read_object_functor
{
    TReader &m_reader;
    TClass &m_t;
    std::string m_sprop;
    bool m_bfound; // this was public !
  public:
    bool not_found(){ return !m_bfound; }
    read_object_functor(TReader &reader, TClass &t, std::string s_prop)
        : m_reader(reader), m_t(t), m_sprop(s_prop)
    {
        m_bfound = false;
    }

    template <typename TPropType>
    void operator()(const char *sz_prop, TPropType TClass::*p_prop_offset)
    {
        if (m_sprop == sz_prop) 
        {
            // read_json(m_reader, m_t.*p_prop_offset);
            read_xml(m_reader, m_t.*p_prop_offset);
            m_bfound = true;
        }
    }
};

/** =======================================================
 *  class types overloads
 *  ===================================================== */

/**
 * an overload for the 
 * reading operation of class types using a reader  
 * which wraps an istream object inside, this used to 
 * read for any class that provides a class descriptor
 * with proper attributes to serialize (note that the
 * process is recursive i.e. class type attributes need
 * to provide their own descriptors)
 * */

template <typename TReader, typename T>
void dispatch_read_object(const class_descriptor<T>& desc,
                          TReader &reader, T& t)
{
    reader.enter_object(desc.get_name());
    if (!reader.is_end_object(desc.get_name())) 
    {
        std::string s_prop;
        reader.first_prop(s_prop);
        for (;;)
        {
            read_object_functor<TReader, T> functor(reader, t, s_prop);
            desc.foreach_prop(functor);
            if (functor.not_found())
                    throw std::runtime_error("couldn't find property");
            if (reader.is_end_object(desc.get_name()))
                break;
            reader.next_prop(s_prop);
        }
    }
    reader.leave_object(desc.get_name());
}

/**
  * an overload for the 
 * writing operation of class types using a reader
 * which wraps an ostream object inside, this used to 
 * write any class that provides a class descriptor
 * with proper attributes to serialize (note that the
 * process is recursive i.e. class type attributes need
 * to provide their own descriptors) 
 * */
template <typename TWriter, typename T>
void dispatch_write_object(const class_descriptor<T> &desc,
                           TWriter &writer, const T &t)
{
    write_object_functor<TWriter, T> functor(writer, t);
    writer.begin_object(desc.get_name());
    desc.foreach_prop(functor);
    writer.end_object(desc.get_name());
}

/** =======================================================
 *  primitive types overloads
 *  ===================================================== */

/**
 * an overload for the 
 * reading operation of primitive type using a reader  
 * which wraps an istream object inside, this used to 
 * read only primitive types such as (int, float, ...)
 * */
template <typename TReader, typename T>
void dispatch_read_object(const primitive_type_descriptor<T> &desc,
                          TReader &reader, T &t)
{
    reader.read_value(t);
}

/**
 * an overload for the 
 * writing operation of primitive type using a writer  
 * which wraps an ostream object inside, this used to 
 * write only primitive types such as (int, float, ...)
 * */
template <typename TWriter, typename T>
void dispatch_write_object(const primitive_type_descriptor<T>& desc, 
                            TWriter& writer, const T& t)
{
    writer.write_value(t);
}

/** =======================================================
 *  interface functions : read_object and write_object
 *  ===================================================== */

/**
 * object read function, provides an abstract 
 * function that works independently of the type provided 
 * and dispatches the reading process to the most viable
 * candidate of the overloads (based on the descriptor type) !
 * */
template <typename TReader, typename T>
void read_object(TReader& reader, T& t)
{
    dispatch_read_object(get_type_descriptor(t), reader, t);
}

/**
 * object write function, provides an abstract 
 * function that works independently of the type provided 
 * and dispatches the writing process to the most viable
 * candidate of the overloads (based on the descriptor type) !
 * */
template <typename TWriter, typename T>
void write_object(TWriter& writer, const T& t)
{
    dispatch_write_object(get_type_descriptor(t), writer, t);
}

#endif /* __CLASS_DESCRIPTOR__ */