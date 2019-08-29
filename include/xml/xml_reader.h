#ifndef __XML_READER__
#define __XML_READER__

#include <sstream>
#include <stdexcept>

#include "../common/reader.h"
#include "xml_lexer.h"

/**
 * json objects reading class, reads a JSON-formatted stream
 * formalism through a set of sequantial member functions calls
 * this class defines has a set of CFG like grammar functions 
 * that verify the correctness of the stream (see each function)
 * if the stream is corrupted (violated grammar) a runtime error is
 * triggred
 * */

template <typename TStream>
class xml_reader  : public reader<TStream>
{
	xml_lexer<TStream> m_lexer;
	/**
	 * ensure that a string has been read and stored in
	 * the result reference parameter, lexer is advanced
	 * afterwards
	 * */
	bool read_tag(std::string &result)
	{
		if (m_lexer.get().type == xml_lexicon::TAG)
		{
			result = m_lexer.get().lexicon;
			m_lexer.advance();
			return true;
		}
		else
		{
			return false;
		}
	}

	bool read_string(std::string &result)
	{
		if (m_lexer.get().type == xml_lexicon::STRING)
		{
			result = m_lexer.get().lexicon;
			m_lexer.advance();
			return true;
		} 
		else if (is_in_item())
		{
			m_lexer.advance();
			result = m_lexer.get().lexicon;
			m_lexer.advance();
			return true;
		}
		else
		{
			return false;
		}
	}
	bool is_in_item() 
	{
		return match_symbol("item");
	}
public:
	xml_reader(TStream &stream) : reader<TStream>(), m_lexer(stream)
	{
	}

	/**
	 * ensures that the next character to be read is equivalent
	 * to the given (expected) character "symbol", used to 
	 * verify that the stream (wrapped inside the lexer)
	 * is a symbol (defined separator) and that is matching
	 *  JSON's grammar in this case
	 * */
	bool match_symbol(const std::string &symbol)
	{
		return (m_lexer.get().type == xml_lexicon::TAG) &&
			   (symbol == m_lexer.get().lexicon);
	}

	/**
	 * matches the next characters against the "{" 
	 * JSON's object entry
	 * */
	bool is_begin_object(const std::string& name)
	{	
		return match_symbol(name );
	}

	/**
	 * matches the next characters against the "}" 
	 * JSON's object end
	 * */
	bool is_end_object(const std::string& name)
	{
		return match_symbol(name);
	}

	/**
	 * advances the lexer to read the next token
	 * after ensuring object's start
	 * */
	void enter_object(const std::string& name)
	{
		if (match_symbol(name))
			m_lexer.advance();
		else
			throw std::runtime_error("expected tag \"< " + name + "\">\"");
	}

	/**
	 * advances the lexer to read the next token
	 * after ensuring current object's end
	 * */
	void leave_object(const std::string& name)
	{
		if (match_symbol(name))
			m_lexer.advance();
		else
			throw std::runtime_error("expected tag \"<\\" + name + "\">\"");
	}

	/**
	 * matches the next characters against the "[" 
	 * JSON's array entry
	 * */
	bool is_begin_array()
	{
		return match_symbol("array");
	}

	/**
	 * matches the next characters against the "]" 
	 * JSON's array end
	 * */
	bool is_end_array()
	{
		return match_symbol("/array");
	}

	/**
	 * advances the lexer to read the next token
	 * after ensuring array's start
	 * */
	void enter_array()
	{
		if (match_symbol("array"))
			m_lexer.advance();
		else
			throw std::runtime_error("expected <array>");
	}

	/**
	 * advances the lexer to read the next token
	 * after ensuring current array's end
	 * */

	void leave_array()
	{
		if (match_symbol("/array"))
			m_lexer.advance();
		else
			throw std::runtime_error("expected </array>");
	}

	/**
	 * advances the lexer to read the next item in an 
	 *  object/array after checking correctness by matching 
	 * against the ","
	 * */
	void next_item()
	{
		if (match_symbol("/item"))
			m_lexer.advance();
		// else
			// throw std::runtime_error("expected tag \"item\"");
	}

	/**
	 * reads the actual value of the propertie (the string itself)
	 * and matches the the next symbol against the ":" to ensure 
	 * correctness then advances the lexer
	 * */

	void read_prop(std::string &prop)
	{
		if (read_tag(prop))
		{
			// m_lexer.advance();
		}
		else
		{
			throw std::runtime_error("expected property");
		}
	}

	/**
	 * reads the first prop of an objects, simply forwards the call to 
	 * read_prop as now special processing is required
	 * */

	void first_prop(std::string &prop)
	{
		read_prop(prop);
	}

	/**
	 * reads the nth next propertrie (where n >= 1), matching against
	 * the "," is mandatory before read_prop is called
	 * */
	void next_prop(std::string &prop)
	{
		m_lexer.advance();
		read_prop(prop);
	}

	/**
	 * ensures that the token being read is an identifier
	 * (just don't know what's the goddamn purpose it
	 * serves -_-)
	 * */
	bool match_identifier(const std::string& id) 
	{
		return (m_lexer.get().type == xml_lexicon::IDENTIFIER)
				&& (id == m_lexer.get().lexicon);
	}

	/**
	 * read_value overload that handles boolean values 
	 * checks the token type and string representation 
	 * and returns the corresponding boolean value
	 * */

	void read_value (bool& result)
	{
		if ((m_lexer.get().type == xml_lexicon::STRING) && 
			(m_lexer.get().lexicon == "true"))
		{
			result = true;
			m_lexer.advance();
		}
		else if ((m_lexer.get().type == xml_lexicon::STRING) &&
			(m_lexer.get().lexicon == "false"))
		{
			result = false;
			m_lexer.advance();
		}
		else
		{
			throw std::runtime_error("expected boolean");
		}
	}

	/**
	 * read_value overload that handles string values 
	 * simply forwards the call to read_string
	 * */

	void read_value(std::string& result)
	{
		if (!read_string(result) && !is_end_array()){
			throw std::runtime_error("expected string");
		}
	}

	/**
	 * read_value overload that handles integer values 
	 * checks the token type and string representation 
	 * and returns the corresponding integer value
	 * */

	void read_value(int& result)
	{
		if (m_lexer.get().type == xml_lexicon::INTEGER)
		{
			std::stringstream sint(m_lexer.get().lexicon);
			sint >> result;
			m_lexer.advance();
		}
		else
		{
			std::runtime_error("expected integer");
		}
	}

	/**
	 * read_value overload that handles floating point values 
	 * checks the token type and string representation 
	 * and returns the corresponding floating point value
	 * */

	bool read_value(float& result)
	{
		if ((m_lexer.get().type == xml_lexicon::FLOAT) 
			|| (m_lexer.get().type == xml_lexicon::INTEGER))
		{
			std::stringstream sfloat(m_lexer.get().lexicon);
			sfloat >> result;
			m_lexer.advance();
		}
		else
		{
			throw std::runtime_error("expected float");
		}
	}
};

/**
 * global helper, returns an instance of the reader 
 * when passed a stream object to conduct a read operation
 * */
template <typename TStream>
xml_reader<TStream> get_xml_reader(TStream& stream)
{
	return xml_reader<TStream>(stream);
}

#endif /* __XML_READER__ */