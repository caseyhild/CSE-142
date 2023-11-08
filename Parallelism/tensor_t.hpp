#ifndef TENSOR_INCLUDED
#define TENSOR_INCLUDED

#include <vector>
#include <string.h>
#include <cmath>
#include <fstream>
#include <limits>
#include <limits>
#include "cfiddle.hpp"
#include <iomanip>
#include <sstream>

#ifdef ENABLE_GTEST_FOR_TENSORS
#include <gtest/gtest.h>
#endif
#define TDSIZE_FOR(T,X,Y,Z,B) for(int X = 0; X < T.x; X++) for(int Y = 0; Y < T.y; Y++) for(int Z = 0; Z < T.z; Z++) for(int B = 0; B < T.b; B++)
#define TENSOR_FOR(T,X,Y,Z,B) TDSIZE_FOR((T).size, X, Y, Z, B)

struct point_t
{
	/* 
	   point_t stores the coordinates of an item in a tensor_t.
	*/
	
	int x, y, z, b;
	point_t(int x, int y=0 , int z=0, int b=0) : x(x), y(y), z(z), b(b){}
	point_t(): x(0), y(0), z(0), b(0){}
	bool operator==(const point_t &o) const {
		return (x == o.x && y == o.y && z == o.z && b == o.b);
	}
	bool operator!=(const point_t &o) const {
		return !(*this == o);
	}
};

/* tdsize is an alias for point_t.  It is used to specify the size of
   tensor_t objects. */
using tdsize = point_t;

inline std::ostream& operator<<(std::ostream& os, const point_t & g)
{
	os << std::setw(2) << std::setprecision(2);
	os << "(" << g.x << ", " << g.y << ", " << g.z << ", " << g.b << ")";
	return os;
}

static float rand_f(uint64_t & seed, float maxval) {
	return maxval * ((fast_rand(&seed)&(0xffffffffu))+0.0) / (std::numeric_limits<uint32_t>::max()+0.0);
}

#define EPSILON 1e-8

[[maybe_unused]] static bool almost_equal(float a, float b) {
        return std::abs(a-b) < EPSILON;
}
[[maybe_unused]] static bool almost_equal(double a, double b) {
        return std::abs(a-b) < EPSILON;
}

[[maybe_unused]] static bool almost_equal(uint64_t a, uint64_t b) {
        return a == b;
}

template<typename T>
struct tensor_t
{
	/* tensor_t is a 3D array of values.  It is the main input and
	   output type for the layers in our CNN models.  In almost
	   all cases, it holds floating point values of type float or
	   double.

	   The class has two key members: 
	   
	   - size : is of type tdsize and represents the size of the tensor (x,y,z)
	   
	   - data : data is a pointer to the data in the tensor.  

	   data is a liner (1D) array.

	   tensor_t provides two ways of accessing its contents. 

	   The first is via the () operator that lets you specify the
	   x, y, and z coordinates of the item you wish to access.
	   This is the most common access method. 

	   The code for the () operator (see below) defines how the
	   three dimensions map onto the `data` array.  This mapping
	   has important implications for how looping over the tensor
	   translates into memory accesses.

	   Alternately, you can call `as_vector()` to access it as a
	   linear array.  This method is mostly used in fc_layer_t.

	   The code for both of them is below.  Examine it carefully.
	*/

	tdsize size;
	T * data;
	bool delete_memory;

	size_t element_count() const {
		return size.x * size.y * size.z * size.b;
	}

	inline T& operator()( int _x, int _y = 0, int _z =0, int _b=0 )
	{
		return this->get( _x, _y, _z, _b );
	}

	inline const T& operator()( int _x, int _y, int _z, int _b=0 ) const
	{
		return this->get( _x, _y, _z, _b );
	}

	void* start_address() const {
		return reinterpret_cast<void*>(&data[0]);
	}
	void* end_address() const {
		return reinterpret_cast<void*>(&data[element_count()]);
	}
	uint linearize(int x, int y=0, int z=0, int b = 0) const {
		return 	b * (size.x * size.y * size.z) +
			z * (size.x * size.y) +
			y * (size.x) +
			x;
	}
	uint linearize(const tdsize &s) const {
		return 	s.b * (size.x * size.y * size.z) +
			s.z * (size.x * size.y) +
			s.y * (size.x) +
			s.x;
	}
	T& get( int _x, int _y=0, int _z=0, int _b=0 ) {
		
		return data[
			_b * (size.x * size.y * size.z) +
			_z * (size.x * size.y) +
			_y * (size.x) +
			_x
			];
	}

	// START_GET       
	const T & get( int _x, int _y=0, int _z=0, int _b=0 ) const {
		
		return data[
			_b * (size.x * size.y * size.z) +
			_z * (size.x * size.y) +
			_y * (size.x) +
			_x
			];
	}
	// END_GET

	void clear() {
		memset(
		       data,
		       0,
		       calculate_data_size()
		       );
	}
	static const int version = 1;
        static bool diff_prints_deltas;

	size_t calculate_data_size() const {
		return size.x * size.y * size.z * size.b * sizeof( T );
	}

	tensor_t( int _x, int _y, int _z, int _b, bool zero,  T* memory ) :  size(_x, _y, _z, _b), delete_memory(true) {
		if (memory) {
			data = memory;
			delete_memory=false;
		} else {
			if (zero) {
				data = new T[size.x * size.y * size.z * size.b]();
			} else {
				data = new T[size.x * size.y * size.z * size.b];
			}
				
		}
	}
        // START_CONSTRUCT
	tensor_t( int _x, int _y=1, int _z=1, int _b=1):  size(_x, _y, _z, _b), delete_memory(true) {
		data = new T[size.x * size.y * size.z * size.b]();
	}
	// END_CONSTRUCT
	tensor_t(const tdsize & _size, bool zero=true) : size(_size), delete_memory(true)
	{
		if (size.b == 0) {
			size.b = 1;
		}
		if (zero) {
			data = new T[size.x * size.y * size.z * size.b]();
		} else {
			data = new T[size.x * size.y * size.z * size.b];
		}
	}
	
	tensor_t( const tensor_t& other) : size(other.size), delete_memory(true)
	{
		data = new T[size.x * size.y * size.z * size.b];
		memcpy(
			data,
			other.data,
			calculate_data_size()
		);
	}

	tensor_t( tensor_t&& other ) : size(other.size), data(other.data)
	{
		other.data = nullptr;
	}

	~tensor_t()
	{
		if (delete_memory && data) {
			delete[] data;
		}
	}

	
	size_t get_total_memory_size() const {
		return calculate_data_size();
	}
	
	tensor_t<T> operator+( const tensor_t<T>& other )const 
	{
		tensor_t<T> clone( *this );
		for ( int i = 0; i < other.size.x * other.size.y * other.size.z * other.size.b; i++ )
			clone.data[i] += other.data[i];
		return clone;
	}

	
	tensor_t<T> operator-( const tensor_t<T>& other ) const
	{

		tensor_t<T> clone( *this );
		for ( int i = 0; i < other.size.x * other.size.y * other.size.z * other.size.b; i++ )
			clone.data[i] -= other.data[i];
		return clone;
	}
	


	tensor_t<T> & operator=(const tensor_t& other )
	{
		if (&other != this) {
			delete[] data;
			size = other.size;
			data = new T[other.size.x * other.size.y * other.size.z * other.size.b];
			memcpy(
				this->data,
				other.data,
				calculate_data_size()
				);
		}
		return *this;
	}
	
	tensor_t<T> & operator=(tensor_t<T>&& other) {
		if (&other != this) {
			delete [] data;
			data = other.data;
			size = other.size;
			other.data = nullptr;
		}
		return *this;
	}
	bool operator==(const tensor_t<T> & other) const
	{
		if (other.size != this->size)
			return false;

		TENSOR_FOR(*this, x,y,z,b) 
		        if (!almost_equal(other(x,y,z,b),(*this)(x,y,z,b)))
				return false;
		return true;
	}

	bool operator!=(const tensor_t<T> & o) const {
		return !(*this == o);
	}
	
	tensor_t<T> & paste(const tdsize & where, const tensor_t<T> & in,  bool grow=false) {
#if(0)
		if (grow) {
			throw_assert(false, "Grow not implemented");
		} else {
			throw_assert((where.x + in.size.x <= size.x) &&
				     (where.y + in.size.y <= size.y) &&
				     (where.z + in.size.z <= size.z) &&
				     (where.b + in.size.b <= size.b), "Out of bounds tensor<>.copy_at()");
		}
#endif		
		TDSIZE_FOR(in.size, x,y,z,b) 
			get(where.x + x, where.y + y, where.z + z, where.b + b) = in(x,y,z,b);
		return *this; 
	}

	tensor_t<T> copy(const tdsize & where, const tdsize & s,  bool grow=false) const {
#if(0)
		if (grow) {
			throw_assert(false, "Grow not implemented");
		} else {
			throw_assert((where.x + s.x <= size.x) &&
				     (where.y + s.y <= size.y) &&
				     (where.z + s.z <= size.z) &&
				     (where.b + s.b <= size.b),
				     "Out of bounds tensor<>.copy_at(). where = " << where << "; s = " << s << "; this->size = " << size);
		}
#endif
		tensor_t<T> n(s);
		
		TDSIZE_FOR(n.size, x,y,z,b) 
			n.get(x,y,z,b) = get(where.x + x, where.y + y, where.z + z, where.b + b);
		
		return n;
	}
};



template<class T>
bool tensor_t<T>::diff_prints_deltas = false;

inline void randomize(tensor_t<double> & t, uint64_t & seed, double min = 0.0, double max = 100000000.0 ) {
	TENSOR_FOR(t,x,y,z, b) {
		t(x, y, z, b) = min + rand_f(seed, max-min);
	}
}


inline void randomize(tensor_t<float> & t, uint64_t & seed, float min = 0, float max = 100000000.0 ) {
	TENSOR_FOR(t,x,y,z, b) {
		t(x, y, z, b) = min + rand_f(seed, max-min);
	}
}

inline void randomize(tensor_t<uint64_t> & t, uint64_t & seed, uint64_t min = 0, uint64_t max = 10000) {
	TENSOR_FOR(t,x,y,z, b) {
		t(x, y, z, b) = fast_rand(&seed) % (max - min) + min ;
	}
}


template<class T>
static std::ostream& operator<<(std::ostream& os, const tensor_t<T> & t)
{
	for ( int b = 0; b < t.size.b; b ++ ) {
		os << "b = " << b << ": \n";
		for ( int z = 0; z < t.size.z; z++ ) {
			os << "z = " << z << ": \n";
			for ( int y = 0; y < t.size.y; y++ ) {
				for ( int x = 0; x < t.size.x; x++ ) {
					os << std::setw(8) << std::setprecision(3);
					os << t(x,y,z,b) << " ";
				}
				os << "\n";
			}
		}
	}
	
	return os;
}


template<class T>
static std::string diff(const tensor_t<T> & first, const tensor_t<T> & second) 
{
	std::stringstream out;
	tensor_t<bool> diff(first.size);
	bool found = false;
	bool deltas = tensor_t<T>::diff_prints_deltas;
	
	for ( int b = 0; b < diff.size.b; b ++ ) {
		out << "b = " << b << ": \n";
		for ( int z = 0; z < diff.size.z; z++ ) {
			out << "z = " << z << ": \n";
			for ( int y = 0; y < diff.size.y; y++ ) {
				for ( int x = 0; x < diff.size.x; x++ ) {
				        if (!almost_equal(first(x,y,z,b), second(x,y,z,b)))
						found = true;
					if (deltas) {
					        out  << std::setprecision(2) << first(x,y,z,b) - second(x,y,z,b) << " ";
					} else {
						out << (!almost_equal(first(x,y,z,b), second(x,y,z,b)) ? "#" : ".");
					}
				}
				out << "\n";
			}
		}
	}


	if (found) {
		return "\n" + out.str();
	} else {
		return "<identical>";
	}
	
}


template<class T>
static std::string diff(const std::vector<T> & a, const std::vector<T> & b)
{
	std::stringstream out;
	std::vector<bool> diff(a.size());
	bool found = false;
        bool deltas = tensor_t<double>::diff_prints_deltas;
	for ( uint x = 0; x < diff.size(); x++ ) {
	        if (!almost_equal(a[x], b[x])) found = true;
	        if (deltas) {
		       out << std::setprecision(2) <<  a[x] - b[x] << " ";
		} else {
     		       out << (!almost_equal(a[x], b[x]) ? "#" : ".");
		}
	}
	out << "\n";

	if (found) {
		return "\n" + out.str();
	} else {
		return "<identical>";
	}
	
}

#ifdef ENABLE_GTEST_FOR_TENSORS
// Customized assertion formatter for googletest
template<class T>
::testing::AssertionResult AssertTensorsEqual(const char* m_expr,
					      const char* n_expr,
					      const tensor_t<T> & m,
					      const tensor_t<T> & n) {
	if (m == n) return ::testing::AssertionSuccess();

	return ::testing::AssertionFailure() << "Here's what's different. '#' denotes a position where your result is incorrect.\n" << diff(m, n);
}
#define ASSERT_TENSORS_EQ(T, a,b) ASSERT_PRED_FORMAT2(AssertTensorsEqual<T>, a,b)
#define EXPECT_TENSORS_EQ(T, a,b) EXPECT_PRED_FORMAT2(AssertTensorsEqual<T>, a,b)



#endif
#endif
