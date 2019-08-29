#ifndef __XML_LEXER__
#define __XML_LEXER__

#include <string>
#include <stdexcept>

/**
 * JSON's tokens representation, json_lexicon contains 
 * all types of possible JSON data types, useful for match
 * and read functions (easy and cheap comparison ;) 
 * the string lexicon represents the current token being
 * read
 * */

class xml_lexicon
{
  public:
	enum lexicon_type
	{
		TAG,
		INTEGER,
		FLOAT,
		STRING,
		IDENTIFIER,
		END
	} type;
	std::string lexicon;
};

/**
 * JSON lexical analyser, wraps an istream object on which it 
 * performs read a read operation in order to build a 
 * token and specifies it's type. The classes also 
 * provide facilities to get and move to lexer forwards
 * */

template <typename TStream>
class xml_lexer
{
	TStream &m_stream;
	xml_lexicon next_lex;
	int n_line;

	/**
	 * reads the next character in the stream, a runtime 
	 * error is generated if eof is hit
	 * */
	char next()
	{
		if (m_stream.eof())
		{
			throw std::runtime_error("end of file");
		}
		m_stream.get();
		return m_stream.peek();
	}

	/**
	 * token reading operation, handles all possible types 
	 * in JSON, based on the first character read, it builds 
	 * the a string (simple tokenizing process using limiters) 
	 * and specifies its lexicon type, lexicon object is 
	 * stored in the next_lex member object
	 * */
	void read()
	{
		next_lex.lexicon = "";
		if (m_stream.eof())
		{
			next_lex.type = xml_lexicon::END;
		}
		char c = m_stream.peek();
		while (isspace(c))
		{
			if (c == '\n')
				n_line++;
			c = next();
		}
		if (c == EOF)
		{
			next_lex.type = xml_lexicon::END;
			return;
		}
		else if ((c == '<')) /*|| (c == '/')   || (c == '>')*/ 
		{
			while ((c = next()) != '>'){
				next_lex.lexicon.push_back(c);
			}
			
			c = next();
			next_lex.type = xml_lexicon::TAG;
			return;
		}
		else if (isalpha(c))
		{
			next_lex.type = xml_lexicon::IDENTIFIER;
			do
			{
				next_lex.lexicon.push_back(c);
				c = next();
			} while (isalnum(c));
			return;
		}
		else if (isdigit(c))
		{
			do
			{
				next_lex.lexicon.push_back(c);
				c = next();
			} while (isdigit(c));
			 if (c != '.')
			{
				next_lex.type = xml_lexicon::INTEGER;
				return;
			}

			do
			{
				next_lex.lexicon.push_back(c);
				c = next();
			} while (isdigit(c));
			next_lex.type = xml_lexicon::FLOAT;
			return;
		}
		else if (c == '"')
		{
			next_lex.type = xml_lexicon::STRING;
			for (;;)
			{
				c = next();
				if (c == '"')
					break;
				if (c == '\\')
				{
					c = next();
					switch (c)
					{
					case '"':
						break;
					case '\\':
						next_lex.lexicon.push_back('\\');
						break;
					case '/':
						next_lex.lexicon.push_back('/');
						break;
					case 'b':
						next_lex.lexicon.push_back('b');
						break;
					case 'f':
						next_lex.lexicon.push_back('f');
						break;
					case 'r':
						next_lex.lexicon.push_back('r');
						break;
					case 't':
						next_lex.lexicon.push_back('t');
						break;
					case 'u':
						throw std::runtime_error("unicode strings not supported");
					default :
						throw std::runtime_error("unexpected escape sequence");
					}
				}
				else 
				{
					next_lex.lexicon.push_back(c);
				}
			}
			c = next();
			return;
		}
		else
		{
			c = next();
			throw std::runtime_error("unexpected character");
		}
	}
	public:
	xml_lexer(TStream& stream) : m_stream(stream)
	{
		n_line = 1;
		read();
	}

	/**
	 * returns the next token read using the "read"
	 * function
	 * */
	const xml_lexicon& get() { return next_lex; }

	/**
	 * advances the lexer by reading the current token value
	 * simply forwards the callt to read method
	 * */
	void advance() { read(); }
};

/**
 * global helper, returns an instance of the lexer 
 * when passed a stream object to conduct a 
 * lexical analysis operation
 * */

template <typename TStream>
xml_lexer<TStream> get_xml_lexer(TStream& stream)
{
	return xml_lexer<TStream>(stream);
}

#endif /* __JSON_LEXER__ */