#ifndef __READER_H_
#define __READER_H_


template <typename TStream>
class reader
{
	// lexer<TStream> m_lexer;
public:
	reader(){}


	virtual bool match_symbol(const std::string &symbol) = 0;
	virtual bool is_begin_object() = 0;
	virtual bool is_end_object() = 0;


	virtual void enter_object() = 0;
	virtual void leave_object() = 0;

	virtual bool is_begin_array() = 0;
	virtual bool is_end_array() = 0;
	virtual void enter_array() = 0;



	virtual void leave_array() = 0;
	virtual void next_item() = 0;
	virtual void read_prop(std::string &prop) = 0;
	virtual void first_prop(std::string &prop) = 0;
	virtual void next_prop(std::string &prop) = 0;
	virtual bool match_identifier(const std::string& id) = 0;
	

	virtual void read_value (bool& result) = 0;
	
	virtual void read_value(std::string& result) = 0;
	virtual void read_value(int& result) = 0;
	virtual bool read_value(float& result) = 0;
};
#endif /* __READER_H_ */