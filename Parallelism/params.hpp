
#define PARAM_LOOP(x)	for(auto & p##x: ps##x ) 
#define PARAM_MARK(x)	theDataCollector->register_tag("p"#x,p##x)
#define PARAM_PASS(x)   p##x
#define PARAM(x)	std::vector<int64_t> ps##x;			\
	std::vector<int64_t> default_ps##x;				\
	default_ps##x.push_back(1);					\
	archlab_add_multi_option<std::vector<int64_t> >("p" #x,		\
							ps##x,		\
							default_ps##x,	\
							"1",		\
							"Parameter " #x ".");
	
