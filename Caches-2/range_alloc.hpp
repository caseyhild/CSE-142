#include <memory>
#include <iostream>

template <class T, int I>
class RangeAlloc: public std::allocator<T> {

	static void update_bounds(const T* a) {
		lower_bound = std::min(lower_bound, a);
		upper_bound = std::max(upper_bound, a);
	}
public:

	typedef typename std::allocator<T>::value_type        value_type;
	typedef typename std::allocator<T>::pointer           pointer;
	typedef typename std::allocator<T>::const_pointer     const_pointer;
	typedef typename std::allocator<T>::reference         reference;
	typedef typename std::allocator<T>::const_reference   const_reference;
	typedef typename std::allocator<T>::size_type         size_type;
	typedef typename std::allocator<T>::difference_type   difference_type;

	static T* lower_bound;
	static T* upper_bound;
	
	
	// allocate but don't initialize num elements of type T
	pointer allocate (size_type num, const void* p = 0) {
		pointer r = std::allocator<T>::allocate(num, p);
		update_bounds(r);
		update_bounds(r + num);
		return r;
	}

	// initialize elements of allocated storage p with value value
	void construct (pointer p, const T& value) {
		// initialize memory with placement new
		update_bounds(p);
		update_bounds(p + 1);
		std::allocator<T>construct(p, value);
	}
};

template <class T, int I>
T* RangeAlloc<T, I>::lower_bound = (T*)-1;

template <class T, int I>
T* RangeAlloc<T, I>::upper_bound = (T*)0;

template <class T1, class T2, int I>
bool operator== (const RangeAlloc<T1,I>&,
		 const RangeAlloc<T2,I>&) throw() {
	return true;
}
template <class T1, class T2, int I>
bool operator!= (const RangeAlloc<T1,I>&,
		 const RangeAlloc<T2,I>&) throw() {
	return false;
}
