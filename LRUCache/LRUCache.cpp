// LRUCache.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <memory>
#include <list>
#include <unordered_map>
#include <vector>

using namespace std;

namespace std {
	template <typename T>
	class fixed_size_allocator : public allocator<T>
	{
		vector<T> pool_;
		T *prev_deallocated_ = nullptr;
		size_t size_;
		size_t allocated_elems_ = 0;

	public:
		typedef size_t size_type;
		typedef T* pointer;
		typedef const T* const_pointer;

		template<typename _Tp1>
		struct rebind
		{
			typedef fixed_size_allocator<_Tp1> other;
		};

		pointer allocate(size_type n, const void *hint = 0)
		{
			//fprintf(stderr, "Alloc %d bytes.\n", n*sizeof(T));
			//return std::allocator<T>::allocate(n, hint);
			if (allocated_elems_ + n <= size_) {
				auto res = &pool_[allocated_elems_];
				allocated_elems_ += n;
				return res;
			}
			else {
				if (prev_deallocated_ == nullptr || n > 1)
					throw bad_alloc{};
				auto res = prev_deallocated_;
				prev_deallocated_ = nullptr;
				return res;
			}
		}

		void deallocate(pointer p, size_type n)
		{
			//fprintf(stderr, "Dealloc %d bytes (%p).\n", n*sizeof(T), p);
			//return std::allocator<T>::deallocate(p, n);
			prev_deallocated_ = p;
		}

		void construct(pointer p, const T& value) 
		{
			*p = value;
		}

		void destroy(pointer p) 

		{

		}

		fixed_size_allocator(size_t size = 16) noexcept
			: std::allocator<T>(), size_(size)
		{
			//fprintf(stderr, "Hello allocator!\n");
			pool_.resize(size);
		}

		fixed_size_allocator(fixed_size_allocator const &other) = default;

		template<typename U>
		fixed_size_allocator(fixed_size_allocator<U> const &other) :
			fixed_size_allocator(other.max_size())
		{
		}

		~fixed_size_allocator() noexcept {
		}

		size_type max_size() const {
			return size_;
		}

		template <typename U>
		friend bool operator==(fixed_size_allocator<T> const &a, fixed_size_allocator<T> const &b) {
			return false;
		}

		template <typename U>
		friend bool operator!=(fixed_size_allocator<T> const &a, fixed_size_allocator<T> const &b) {
			return true;
		}
	};
}

template <typename T>
class FixedSizeList
{
	struct ListNode {
		unsigned prev;
		unsigned next;
		T value;
	};

	std::vector<ListNode> data_;
	unsigned size_;
	unsigned begin, end;

public:
	FixedSizeList(std::size_t maxsize) : data_(maxsize), size_(0)
	{}

	template <typename... Args>
	unsigned emplace_back(Args&&... args)
	{
		data_[size++].value = T{ args... };
		data_[size].prev = size - 1;
		data_[size].next = size;
		return size;
	}

	unsigned move_front_to_back()
	{
		auto oldprev = data_[begin].prev;
		auto oldnext = data_[begin].next;

		data_[data_[begin].next].prev = -1;
		begin = 
	}
};

class LRUCache {
	typedef list<int, std::fixed_size_allocator<int>> list_type;

	list_type cache_; // list key
	unordered_map<int, pair<int, list_type::iterator>> hash_;
	size_t capacity_;

public:
	LRUCache(int capacity) :
		cache_(std::fixed_size_allocator<int>(capacity + 4)),
		capacity_(capacity)
	{
		hash_.max_load_factor(0.5);
		hash_.reserve(capacity);
	}

	int get(int key) {
		auto hit = hash_.find(key);
		if (hit == hash_.end()) return -1;

		auto &p = hit->second;

		auto lit = p.second;
		cache_.splice(cache_.end(), cache_, lit);
		return p.first;
	}

	void set(int key, int value) {
		auto hit = hash_.find(key);
		if (hit == hash_.end()) {
			if (hash_.size() == capacity_) {
				auto lit = cache_.begin();
				hash_.erase(*lit);
				cache_.pop_front();
			}

			auto lit = cache_.emplace(cache_.end(), key);
			hash_.emplace(piecewise_construct, forward_as_tuple(key), forward_as_tuple(value, lit));

		}
		else {
			auto &p = hit->second;
			auto lit = p.second;
			p.first = value;
			cache_.splice(cache_.end(), cache_, lit);
		}
	}
};

int main()
{
	LRUCache l(1);
	l.set(2, 1);
	l.get(1);
	return 0;
}

