#ifndef __ARRAY_TYPE_DESCRIPTOR__
#define __ARRAY_TYPE_DESCRIPTOR__

#include <cstddef>


/**
 * forward declaration for the class_descriptor template
 * */
template <typename T>
class class_descriptor;

/**
 * forward declaration for the array_type_descriptor template
 * */

template <typename T>
class array_type_descriptor;

/**
* keeps track element in the array being read, 
* insertion continues until there's nothing 
* more to read or the size of array is reached
* */ 

template <typename T, int N>
class array_inserter
{
	T* m_curr;
	T* m_end;
public:
	array_inserter(T (&t)[N])
	{
		m_curr = t;
		m_end = t + N;
	}

	/**
	 * checks if the limit of the array is reached
	 * */
	bool ok()
	{
		return m_curr != m_end;
	}
	/**
	 * advances the array pointer to the next position
	 * and returns the inserted element
	 * */
	T& insert() 
	{
		return *(m_curr++);
	}
};

/**
 * provides type-specific (array) descriptor necessary 
 * for disptach operations overloads along with facilities
 * necessary for array I/O
 * */

template <typename T, int N>
class array_type_descriptor<T[N]>
{
public:
	/**
 	* returns an instance of array inserter that keeps track 
	* element in the array being read, operation continues
	* until there's nothing more to read or the 
	* size of array is reached
 	* */ 
	array_inserter<T, N> begin_insert(T (&t)[N]) const
	{
		return array_inserter<T, N>(t);
	}

	/**
	 * returns the size of array
	 * */
	size_t get_size(const T (&t)[N]) const
	{
		return N;
	}
};

/**
 * array-specific class type descriptor, has a 
 * overrides the descriptor_t member to represent 
 * the array_type_descriptor type
 * */

template <typename T, int N>
class class_descriptor<T[N]>
{
public:
	using descriptor_t = array_type_descriptor<T[N]>;
};

/**
 * an overload for the 
 * writing operation of array and vector type using a writer  
 * which wraps an ostream object inside, this used to 
 * write array types for all possible (visitable) kinds of 
 * classes
 * */

template <typename TWriter, typename T>
void dispatch_write_object(const array_type_descriptor<T>& desc,
							TWriter &writer, const T& t)
{
	writer.begin_array();
	auto size = desc.get_size(t);
	for (decltype(size) n = 0; n < size; n++)
	{
		writer.begin_item();
		// write_json(writer, t[n]); //TODO: this should be more general 
		write_xml(writer, t[n]);
		writer.end_item();
	}
	writer.end_array();
}

/**
 * an overload for the 
 * reading operation of array and vector type using a reader  
 * read array types for all possible (visitable) kinds of 
 * classes
 * */

template <typename TReader, typename T>
void dispatch_read_object(const array_type_descriptor<T>& desc,
							TReader &reader, T& t)
{
	reader.enter_array();
	auto it = desc.begin_insert(t); //this is OUTSIDE THE LOOP
	if (it.ok() && !reader.is_end_array())
	{
		do {
			read_object(reader, it.insert());
			if (reader.is_end_array())
				break;
			reader.next_item();
		}while (it.ok());
	}
	reader.leave_array();
}
#endif /* __ARRAY_TYPE_DESCRIPTOR__ */
