#pragma once
#include <map>
#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <ctype.h>
#include <stdlib.h>
#pragma once

namespace jjson
{
	enum type_t
	{
		e_null,
		e_num,
		e_str,
		e_bool,
		e_float,
		e_obj,
		e_vec,
	};
	struct obj_t;
	class json_t
	{
	public:
		json_t()
		{

		}
		json_t(const json_t&self);
		~json_t();
		obj_t &operator[](const std::string &key);
		json_t &add(const std::string &key, obj_t *obj)
		{
			vals_.insert(std::make_pair(key, obj));
			return *this;
		}
		bool erase(const std::string &key)
		{
			if (vals_.find(key) == vals_.end())
				return false;
			vals_.erase(vals_.find(key));
			return true;
		}
		std::string to_string();

		std::map<std::string, obj_t*> vals_;
	};

	struct obj_t
	{
		type_t type_;
		union value_t
		{
			std::string *str_;
			int64_t num_;
			double double_;
			bool bool_;
			json_t *obj_;
			std::vector<obj_t*> *vec_;
		} val_;

		obj_t()
			:type_(e_null)
		{
			memset(&val_, 0, sizeof(val_));
		}
		virtual ~obj_t()
		{
			switch (type_)
			{
			case jjson::e_null:
			case jjson::e_num:
			case jjson::e_bool:
			case jjson::e_float:
				break;
			case jjson::e_str:
				delete val_.str_;
				break;
			case jjson::e_obj:
				delete val_.obj_;
				break;
			case jjson::e_vec:
				for (auto&itr : *val_.vec_)
					delete itr;
				delete val_.vec_;
				break;
			default:
				break;
			}
		}
		obj_t &operator=(const obj_t&self)
		{
			memset(&val_, 0, sizeof(val_));
			assign(self);
			return *this;
		}
		obj_t(const obj_t &self)
			:type_(e_null)
		{
			memset(&val_, 0, sizeof(val_));
			assign(self);
		}
		void assign(const obj_t &self)
		{
			switch (self.type_)
			{
			case e_bool:
				*this = self.val_.bool_;
				break;
			case e_num:
				*this = self.val_.num_;
				break;
			case e_str:
				*this = *self.val_.str_;
				break;
			case e_obj:
				*this = *self.val_.obj_;
				break;
			case e_float:
				*this = self.val_.double_;
				break;
			case e_vec:
				if (!val_.vec_)
					val_.vec_ = new std::vector<obj_t *>;
				for (auto &itr : *self.val_.vec_)
				{
					val_.vec_->push_back(new obj_t(*itr));
				}
			case e_null:
				break;
			default:
				assert(false);
			}
			type_ = self.type_;
		}
		template<typename T>
		typename std::enable_if<std::is_floating_point<T>::value, void>::type
			operator=(T val)
		{
			assert(type_ == e_null);
			type_ = e_float;
			val_.double_ = val;
		}
		void operator=(const char *val)
		{
			assert(type_ == e_null);
			type_ = e_str;
			val_.str_ = new std::string(val);
		}

		void operator=(const std::string &val)
		{
			assert(type_ == e_null);
			type_ = e_str;
			val_.str_ = new std::string(val);
		}
		template<typename T>
		typename std::enable_if<std::is_integral<T>::value, void>::type
			operator=(T val)
		{
			assert(type_ == e_null);
			type_ = e_num;
			val_.num_ = val;
		}
		void operator=(bool val)
		{
			assert(type_ == e_null);
			type_ = e_bool;
			val_.bool_ = val;
		}
		void operator=(const json_t &obj)
		{
			assert(type_ == e_null);
			type_ = e_obj;
			val_.obj_ = new json_t(obj);
		}
		void operator=(const json_t *obj)
		{
			assert(type_ == e_null);
			type_ = e_obj;
			val_.obj_ = const_cast<json_t*>(obj);
		}
		obj_t &operator[](const std::string & key)
		{
			assert(type_ == e_null || type_ == e_obj);
			type_ = e_obj;
			if (!!!val_.obj_)
				val_.obj_ = new json_t();
			return (*val_.obj_)[key];
		}

		obj_t &operator[](const char *key)
		{
			assert(type_ == e_null || type_ == e_obj);
			type_ = e_obj;
			if (!!!val_.obj_)
				val_.obj_ = new json_t();
			return (*val_.obj_)[key];
		}

		template<typename T>
		obj_t& add(const T &val)
		{
			assert(type_ == e_vec || type_ == e_null);
			type_ = e_vec;
			if (val_.vec_ == NULL)
				val_.vec_ = new std::vector<obj_t *>;
			obj_t *obj = new obj_t;
			*obj = val;
			val_.vec_->push_back(obj);
			return *this;
		}
		obj_t& add(obj_t *obj)
		{
			assert(type_ == e_vec || type_ == e_null);
			type_ = e_vec;
			if (val_.vec_ == NULL)
				val_.vec_ = new std::vector<obj_t *>;
			val_.vec_->push_back(obj);
			return *this;
		}
		template<class T>
		typename std::enable_if<std::is_integral<T>::value &&
			!std::is_same<T, bool>::value, T>::type
			get()
		{
			assert(type_ == e_num);
			return static_cast<T>(val_.num_);
		}
		template<class T>
		typename std::enable_if<std::is_same<T, bool>::value, T>::type
			get()
		{
			assert(type_ == e_bool);
			return val_.bool_;
		}
		template<class T>
		typename std::enable_if<std::is_floating_point<T>::value, T>::type
			get()
		{
			assert(type_ == e_float);
			return static_cast<T>(val_.double_);
		}
		template<class T>
		typename std::enable_if<std::is_same<T, std::string>::value, std::string &>::type
			get()
		{
			assert(type_ == e_str);
			return *val_.str_;
		}
		template<typename T>
		T get(std::size_t idx)
		{
			assert(type_ == e_vec);
			assert(val_.vec_);
			assert(idx < val_.vec_->size());
			return ((*val_.vec_)[idx])->get<T>();
		}
		obj_t &get(std::size_t idx)
		{
			assert(type_ == e_vec);
			assert(val_.vec_);
			assert(idx < val_.vec_->size());
			return *((*val_.vec_)[idx]);
		}
		bool is_null()
		{
			return type_ == e_null;
		}
		std::size_t len()
		{
			assert(type_ == e_vec);
			return val_.vec_->size();
		}
		type_t type()
		{
			return type_;
		}
		std::string str()
		{
			switch (type_)
			{
			case e_bool:
				return val_.bool_ ? "true" : "false";
			case e_num:
				return std::to_string(val_.num_);
			case e_str:
				return "\"" + *val_.str_ + "\"";
			case e_obj:
				return val_.obj_->to_string();
			case e_float:
				return std::to_string(val_.double_);
			case e_vec:
				return vec2str();
			}
			return "null";
		}
		std::string vec2str()
		{
			std::string str("[");
			assert(type_ == e_vec);
			assert(val_.vec_);

			for (auto &itr : *val_.vec_)
			{
				str += itr->str();
				str += ", ";
			}
			str.pop_back();
			str.pop_back();
			str += "]";
			return str;
		}
	};
	static const obj_t null;
	inline json_t::json_t(const json_t&self)
	{
		for (auto &itr : self.vals_)
		{
			obj_t *val = new obj_t(*itr.second);
			vals_[itr.first] = val;
		}
	}

	inline json_t::~json_t()
	{
		for (auto &itr : vals_)
			delete itr.second;
	}

	inline obj_t &json_t::operator[](const std::string &key)
	{
		if (vals_.find(key) != vals_.end())
			return *vals_[key];
		obj_t *val = new obj_t();
		val->type_ = e_null;
		vals_[key] = val;
		return *val;
	}
	inline std::string json_t::to_string()
	{
		std::string str("{");
		for (auto &itr : vals_)
		{
			str += "\"" + itr.first + "\"";
			str += " : ";
			str += itr.second->str();
			str += ", ";
		}
		str.pop_back();
		str.pop_back();
		str += "}";
		return str;
	}
	namespace json_parser
	{
		static inline bool 
			skip_space(int &pos, int len, const char * str)
		{
			assert(len > 0);
			assert(pos >= 0);
			assert(pos < len);

			while(pos < len &&
				(str[pos] == ' ' ||
				  str[pos] == '\r' ||
				  str[pos] == '\n' ||
				  str[pos] == '\t'))
				++pos;

			return pos < len;
		}
		static inline std::pair<bool, std::string>
			get_text(int &pos, int len, const char *str)
		{
			assert(len > 0);
			assert(pos >= 0);
			assert(pos < len);
			assert(str[pos] == '"');

			std::string key;
			++pos;
			while(pos < len)
			{
				if(str[pos] != '"')
					key.push_back(str[pos]);
				else if(key.size() && key.back() == '\\')
					key.push_back(str[pos]);
				else
				{
					++pos;
					return{ true, key };
				}
				++pos;
			}
			return{ false,"" };
		}
		static int get_bool(int &pos, int len, const char *str)
		{
			assert(len > 0);
			assert(pos >= 0);
			assert(pos < len);
			assert(str[pos] == 't' || str[pos] == 'f');
			std::string key;

			while(pos < len)
			{
				if(isalpha(str[pos]))
					key.push_back(str[pos]);
				else
					break;
				++pos;
			}
			if(key == "true")
				return 1;
			else if(key == "false")
				return 0;
			return -1;
		}
		static inline bool get_null(int &pos, int len, const char *str)
		{
			assert(len > 0);
			assert(pos >= 0);
			assert(pos < len);
			assert(str[pos] == 'n');
			std::string null;

			while(pos < len)
			{
				if(isalpha(str[pos]))
					null.push_back(str[pos]);
				else
					break;
				++pos;
			}
			if(null == "null")
				return true;
			return false;
		}
		static inline std::string get_num(
			bool &sym, int &pos,int len, const char *str)
		{
			sym = false;
			std::string tmp;
			while(pos < len)
			{
				if(str[pos] >= '0' &&str[pos] <= '9')
				{
					tmp.push_back(str[pos]);
					++pos;
				}
				else if(str[pos] == '.')
				{
					tmp.push_back(str[pos]);
					++pos;
					if(sym == false)
						sym = true;
					else
						return false;
				}
				else
					break;
			}
			return tmp;
		}

		static inline json_t *get_json(int &pos, int len, const char * str);
		static inline obj_t* get_vec(int &pos, int len, const char *str)
		{
			obj_t *vec = new obj_t;
			if(str[pos] == '[')
				pos++;
			while(pos < len)
			{
				switch(str[pos])
				{
				case ']':
					++pos;
					return vec;
				case '[':
				{
					obj_t * obj = get_vec(pos, len, str);
					if(!!!obj)
						goto fail;
					vec->add(obj);
					break;
				}
				case '"':
				{
					std::pair<bool, std::string > res = get_text(pos, len, str);
					if(res.first == false)
						goto fail;
					vec->add(res.second);
					break;
				}
				case 'n':
					if(get_null(pos, len, str))
					{
						vec->add(null);
						break;
					}
				case '{':
				{
					json_t *json = get_json(pos, len, str);
					if(!!!json)
						goto fail;
					vec->add(json);
					break;
				}
				case 'f':
				case 't':
				{
					int b = get_bool(pos, len, str);
					if(b == 0)
						vec->add(false);
					else if(b == 1)
						vec->add(true);
					else
						goto fail;
					break;
				}
				case ',':
				case ' ':
				case '\r':
				case '\n':
				case '\t':
					++pos;
					break;
				default:
					if(str[pos] == '-' || str[pos] >= '0' && str[pos] <= '9')
					{
						bool is_float = false;
						std::string tmp = get_num(is_float, pos, len, str);
						errno = 0;
						if(is_float)
						{
							double d = std::strtod(tmp.c_str(), 0);
							if(errno == ERANGE)
								goto fail;
							vec->add(d);
						}
						else
						{
							int64_t val = std::strtoll(tmp.c_str(), 0, 10);
							if(errno == ERANGE)
								goto fail;
							vec->add(val);
						}
					}
				}
			}

		fail:
			delete vec;
			return NULL;
		}

#define check_ahead(ch)\
do{\
		if(!skip_space(pos, len, str))\
			goto fail; \
			if(str[pos] != ch)\
				goto fail;\
} while(0)

		static inline json_t *get_json(int &pos, int len, const char * str)
		{
			json_t *json_ptr = new json_t;
			json_t &json = *json_ptr;
			std::string key;

			skip_space(pos, len, str);
			if(pos >= len)
				goto fail;
			if(str[pos] == '{')
				++pos;
			else
				goto fail;
			while(pos < len)
			{
				switch(str[pos])
				{
				case '"':
				{
					std::pair<bool, std::string > res = get_text(pos, len, str);
					if(res.first == false)
						goto fail;
					if(key.empty())
					{
						key = res.second;
						check_ahead(':');
					}
					else
					{
						json[key] = res.second;
						key.clear();
					}
					break;
				}
				case 'f':
				case 't':
				{
					if(key.empty())
						goto fail;
					int b = get_bool(pos, len, str);
					if(b == 0)
						json[key] = false;
					else if(b == 1)
						json[key] = true;
					else
						goto fail;
					key.clear();
					break;
				}
				case 'n':
				{
					if(key.empty() || get_null(pos, len, str) == false)
						goto fail;
					json[key] = null;
				}
				case '{':
				{
					if(key.empty())
						goto fail;
					json_t *obj = get_json(pos, len, str);
					if(obj == NULL)
						goto fail;
					json[key] = obj;
					key.clear();
					break;
				}
				case '}':
					if(key.size())
						goto fail;
					++pos;
					return json_ptr;
				case ':':
				{
					if(key.empty())
						goto fail;
					++pos;
					break;
				}
				case ',':
				{
					++pos;
					check_ahead('"');
					break;
				}
				case ' ':
				case '\r':
				case '\n':
				case '\t':
					++pos;
					break;
				case '[':
				{
					obj_t *vec = get_vec(pos, len, str);
					if(!!!vec || key.empty())
						goto fail;
					json.add(key, vec);
					key.clear();
					break;
				}
				default:
					if(str[pos] == '-' || str[pos] >= '0' && str[pos] <= '9')
					{
						bool is_float = false;
						std::string tmp = get_num(is_float, pos, len, str);
						errno = 0;
						if(is_float)
						{
							double d = std::strtod(tmp.c_str(), 0);
							if(errno == ERANGE)
								return false;
							json[key] = d;
							key.clear();
						}
						else
						{
							int64_t val = std::strtoll(tmp.c_str(), 0, 10);
							if(errno == ERANGE)
								return false;
							json[key] = val;
							key.clear();
						}
					}
				}
			}

		fail:
			delete json_ptr;
			return NULL;
		}
	}
	json_t *build_json(const std::string &str)
	{
		int pos = 0;
		return json_parser::get_json(pos, (int)str.size(), str.c_str());
	}
	json_t *build_json(const char *str)
	{
		int pos = 0;
		return json_parser::get_json(pos, (int)strlen(str), str);
	}
	void destory_json(json_t *json)
	{
		delete json;
	}
}