#ifndef ARCHLAB_H_INCLUDED
#define ARCHLAB_H_INCLUDED
#include<stdint.h>

#include <stdlib.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

extern "C" {
	uint64_t si_parse(const char *);
	void archlab_parse_cmd_line(int *argc, char *argv[]);

} // C linkage


#include <boost/program_options.hpp>
namespace po = boost::program_options;

class BaseOptionSpec {
public:
  virtual void assign(const po::variables_map &vm) = 0;
};

template<class PARSED, class STORED>
class OptionSpec : public BaseOptionSpec{
protected:
  const std::string name;
  STORED & dest;
public:
  OptionSpec(const std::string & name,
	     STORED & dest) : name(name), dest(dest) {
  }

  void assign(const po::variables_map &vm) {
    dest = vm[name].template as<PARSED>();
  }
};


class FlagOptionSpec: public OptionSpec<bool, bool> {
public:
  FlagOptionSpec(const std::string & name, bool & dest) : OptionSpec<bool, bool>(name, dest) {
  }

  void assign(const po::variables_map &vm) {
    dest = vm.count(name) > 0;
  }
};

template<class T>
class SIInt {
  typedef T type;
  
public:
  T _value;

  SIInt & operator= (const SIInt &s) {
    _value = s._value;
    return *this;
  }
    
  SIInt() : _value(0) {}
  SIInt(const T v): _value(v) {}
  SIInt(int & v): _value(v) {}
  SIInt(const int & v): _value(v) {}

  operator T&() {return _value;}
  operator const T&() const {return _value;}

  const type &value() const {
    return _value;
  }

  
};


extern std::vector<BaseOptionSpec*> options;
extern po::options_description archlab_cmd_line_options;

template<class STORED>
void archlab_add_option(const std::string & name, STORED & dest, const STORED& def, const std::string & desc) {
  archlab_cmd_line_options.add_options()
    (name.c_str(), po::value<STORED >()->default_value(def), desc.c_str());
  options.push_back(new OptionSpec<STORED, STORED>(name, dest));
}

template<class STORED>
void archlab_add_option(const std::string & name, STORED & dest, const STORED& def, const std::string & default_string, const std::string & desc) {
  archlab_cmd_line_options.add_options()
	  (name.c_str(), po::value<STORED >()->default_value(def, default_string), desc.c_str());
  options.push_back(new OptionSpec<STORED, STORED>(name, dest));
}

template<class STORED>
void archlab_add_multi_option(const std::string & name, STORED & dest, const STORED& def, const std::string & default_string, const std::string & desc) {
  archlab_cmd_line_options.add_options()
	  (name.c_str(), po::value<STORED >()->default_value(def, default_string)->multitoken(), desc.c_str());
  options.push_back(new OptionSpec<STORED, STORED>(name, dest));
}

void archlab_add_flag(const std::string & name, bool & dest, const bool& def, const std::string & desc);

template<class STORED>
void archlab_add_si_option(const std::string & name, STORED & dest, const STORED& def, const std::string & desc) {
  archlab_cmd_line_options.add_options()
    (name.c_str(), po::value<SIInt<STORED> >()->default_value(def), desc.c_str());
  options.push_back(new OptionSpec<SIInt<STORED>, STORED>(name, dest));
}


template<class T>
std::ostream &operator<<( std::ostream &output, const SIInt<T> &i ) {
  output << i._value;
  return output;
}

template<class T>
std::istream &operator>>( std::istream  &input, SIInt<T> &i ) {
  std::string t;
  input >> t;
  i._value = si_parse(t.c_str());
  return input;
}

typedef SIInt<uint64_t> si_uint64_t;
typedef SIInt<uint64_t> si_uint32_t;


#endif
