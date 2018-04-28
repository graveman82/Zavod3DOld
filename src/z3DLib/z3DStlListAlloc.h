#ifndef Z3DSTLLISTALLOC_H
#define Z3DSTLLISTALLOC_H

#include <list>
#include <limits>

#define Z3D_LISTALLOCATOR_DEBUG

#ifdef Z3D_LISTALLOCATOR_DEBUG
#include <stdio.h>
#endif

namespace z3D_priv{

void* StlListAllocateNode();
void StlListDeallocateNode(void* p);
void ClearStlListCache();
} // end of z3D_priv

namespace z3D {

using std::size_t;
using std::ptrdiff_t;

const size_t kMAX_SIZE_OF_LIST_ELEMENT = 3 * sizeof(void*) * 2;

class ListAllocNumException: public std::exception {
    virtual const char* what() const throw()
    {
        return "Too big number of elements to allocate passed";
    }
};

class ListAllocSizeException: public std::exception {
    virtual const char* what() const throw()
    {
        return "Too big size of elements to allocate";
    }
};

template <typename LT >
class ListAllocator {
public:
	// Определения типов
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef LT* pointer;
	typedef const LT* const_pointer;
	typedef LT& reference;
	typedef const LT& const_reference;
	typedef LT value_type;

	/* Таким образом, rebind<T_OTH>::other - это ListAllocator<T_OTH>
	*/
	template<typename T_OTH>
	struct rebind{
		typedef ListAllocator<T_OTH> other;
	};

	/* Конструкторы и деструктор, которые ничего не делают,
	поскольку аллокатор не имеет состояния.
	*/
    ListAllocator() throw() { }

    ListAllocator(const ListAllocator& oth) throw() { }

	template<typename T_OTH>
	ListAllocator(const ListAllocator<T_OTH>&) throw() { }

	~ListAllocator() throw() { }

	pointer address(reference r) const noexcept { return std::addressof(r); }

	const_pointer address(const_reference r) const noexcept { return std::addressof(r); }

	pointer allocate(size_type num, const void* = 0) {
		if (num > 1 || num > this->max_size())
			throw ListAllocNumException();
        if (sizeof(LT) > kMAX_SIZE_OF_LIST_ELEMENT)
            throw ListAllocSizeException();
#ifdef Z3D_LISTALLOCATOR_DEBUG
        printf("allocated %d elements of size %d bytes\n", num, sizeof(LT));
#endif
        void* raw = z3D_priv::StlListAllocateNode();
        return static_cast<LT*> (raw);
	}

	void deallocate(pointer p, size_type) {
	    z3D_priv::StlListDeallocateNode(p);
#ifdef Z3D_LISTALLOCATOR_DEBUG
        printf("deallocated element\n");
#endif
    }

	// Возвращает максимальное количество элементов, для которых может быть выделена память
	size_type max_size() const noexcept {
		return std::numeric_limits<size_t>::max() / sizeof(LT);
	}

	template<typename LT2, typename... Args>
	void construct(LT2* p, Args&&... args) {
		::new((void *)p) LT2(std::forward<Args>(args)...);
	}

	template<typename LT2>
	void destroy(LT2* p) {
		p->~LT2();
	}

	static void ClearStlListCache() {
        z3D_priv::ClearStlListCache();
    }
};

template<typename T1, typename T2>
inline bool operator==(const ListAllocator<T1>&, const ListAllocator<T2>&) noexcept
{ return true; }

template<typename LT>
inline bool operator==(const ListAllocator<LT>&, const ListAllocator<LT>&) noexcept
{ return true; }

template<typename T1, typename T2>
inline bool operator!=(const ListAllocator<T1>&, const ListAllocator<T2>&) noexcept
{ return false; }

template<typename LT>
inline bool operator!=(const ListAllocator<LT>&, const ListAllocator<LT>&) noexcept
{ return false; }


} // end of z3D

#endif // Z3DSTLLISTALLOC_H
