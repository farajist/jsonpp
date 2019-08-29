#ifndef __VECTOR_TYPE_DESCRIPTOR__
#define __VECTOR_TYPE_DESCRIPTOR__

#include <vector>
#include "array_type_descriptor.h"

/**
* keeps track element in the array being read, 
* insertion continues until there's nothing 
* more to read (vectors are dynamic)
* */ 

template <typename T>
class vector_inserter
{
	std::vector<T>& m_vec;
public:
	vector_inserter(std::vector<T>& vec) : m_vec(vec)
	{
		vec.clear();
	}

	/**
	 * checks if the limit of the vector is reached 
	 * oops ! sorry no limit
	 * */
	bool ok()
	{
		return true;
	}

	/**
	 * advances the array pointer to the next position
	 * and returns the pushed element
	 * */

	T& insert() 
	{
		m_vec.emplace_back();
		return m_vec.back();
	}
};

/**
 * provides type-specific (array) descriptor for vectors 
 * necessary for disptach operations overloads along with 
 * facilities necessary for vector I/O
 * */

template<typename T>
class array_type_descriptor<std::vector <T>>
{
public:

	/**
 	* returns an instance of vector inserter that keeps track 
	* element in the vector being read, operation continues
	* until there's nothing more to read
 	* */ 

	vector_inserter<T> begin_insert(std::vector<T>& vec) const 
	{
		return vector_inserter<T>(vec);
	}
	
	/**
	 * returns the size of vector
	 * */
	size_t get_size(const std::vector<T>& vec) const 
	{
		return vec.size();	
	}
};

/**
 * vector-specific class type descriptor, has a 
 * overrides the descriptor_t member to represent 
 * the array_type_descriptor type (vectors and 
 * arrays use the same dispatch overloads)
 * */

template <typename T>
class class_descriptor <std::vector<T>>
{
public:
	using  descriptor_t = array_type_descriptor<std::vector<T>>;
};

#endif /* __VECTOR_TYPE_DESCRIPTOR__ */