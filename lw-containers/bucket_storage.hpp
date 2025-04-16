#ifndef BUCKET_STORAGE_HPP
#define BUCKET_STORAGE_HPP

#include "my_list.hpp"

template< typename T >
class BucketStorage
{
  public:
	using value_type = T;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;

	class iterator;
	class const_iterator;

	explicit BucketStorage(size_type block_capacity = 64) noexcept;
	BucketStorage(const BucketStorage& other);
	BucketStorage(BucketStorage&& other) noexcept;
	BucketStorage& operator=(const BucketStorage& other);
	BucketStorage& operator=(BucketStorage&& other) noexcept;
	~BucketStorage();

	iterator insert(const T& value);
	iterator insert(T&& value);
	iterator erase(iterator it);
	bool empty() const noexcept;
	[[nodiscard]] size_type size() const noexcept;
	[[nodiscard]] size_type capacity() const noexcept;
	void shrink_to_fit();
	void clear() noexcept;
	void swap(BucketStorage& other) noexcept;

	iterator begin() noexcept;
	const_iterator begin() const noexcept;
	const_iterator cbegin() const noexcept;
	iterator end() noexcept;
	const_iterator end() const noexcept;
	const_iterator cend() const noexcept;

	iterator get_to_distance(iterator it, difference_type distance);

  private:
	struct Block;
	using BlockPtr = Block*;

	List< BlockPtr > blocks;
	size_type total_size;
	size_type total_capacity;
	size_type block_capacity_;
};

template< typename T >
class BucketStorage< T >::iterator
{
	friend class const_iterator;

  public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T*;
	using reference = T&;
	using size_type = typename BucketStorage< T >::size_type;
	pointer m_value;
	BlockPtr m_block;

	explicit iterator(pointer ptr = nullptr, BlockPtr block = nullptr, BucketStorage* storage = nullptr, size_type index = 0) :
		m_value(ptr), m_block(block), m_storage(storage), m_index(index)
	{
		if (m_block && (m_value == nullptr || m_block->m_activity[m_index] == 0))
		{
			find_next_active();
		}
	}

	reference operator*() const { return *m_value; }
	pointer operator->() { return m_value; }

	iterator& operator++()
	{
		++m_index;
		if (m_index >= m_block->capacity || m_block->m_activity[m_index] == 0)
		{
			find_next_active();
		}
		else
		{
			m_value = &m_block->m_elements[m_index];
		}
		return *this;
	}

	iterator operator++(int)
	{
		iterator tmp = *this;
		++(*this);
		return tmp;
	}
	iterator& operator--()
	{
		if (m_value == nullptr && m_block == nullptr)
		{
			find_previous_active();
		}

		if (m_index > 0)
		{
			--m_index;
			if (m_block->m_activity[m_index] == 1)
			{
				m_value = &m_block->m_elements[m_index];
				return *this;
			}
		}

		auto it = std::find(m_storage->blocks.begin(), m_storage->blocks.end(), m_block);
		if (it != m_storage->blocks.begin())
		{
			--it;
			m_block = *it;
			m_index = m_block->capacity - 1;

			while (m_index > 0 && m_block->m_activity[m_index] == 0)
			{
				--m_index;
			}

			if (m_index > 0 && m_block->m_activity[m_index] == 1)
			{
				m_value = &m_block->m_elements[m_index];
			}
			else
			{
				m_value = nullptr;
				m_block = nullptr;
			}
		}
		else
		{
			m_block = nullptr;
			m_value = nullptr;
		}

		return *this;
	}

	iterator operator--(int)
	{
		iterator tmp = *this;
		--(*this);
		return tmp;
	}

	bool operator==(const iterator& other) const { return m_value == other.m_value && m_block == other.m_block; }

	bool operator!=(const iterator& other) const { return !(*this == other); }

	bool operator<(const iterator& other) const
	{
		if (m_block == other.m_block)
		{
			return m_index < other.m_index;
		}
		return m_block < other.m_block;
	}

	bool operator<=(const iterator& other) const { return *this < other || *this == other; }

	bool operator>(const iterator& other) const { return !(*this <= other); }

	bool operator>=(const iterator& other) const { return !(*this < other); }


  private:
	friend class BucketStorage< T >;
	BucketStorage* m_storage;
	size_type m_index;

	void find_next_active()
	{
		while (m_block)
		{
			while (m_index < m_block->capacity)
			{
				if (m_block->m_activity[m_index] == 1)
				{
					m_value = &m_block->m_elements[m_index];
					return;
				}
				++m_index;
			}

			auto it = std::find(m_storage->blocks.begin(), m_storage->blocks.end(), m_block);
			if (++it != m_storage->blocks.end())
			{
				m_block = *it;
				m_index = 0;
			}
			else
			{
				m_block = nullptr;
				m_value = nullptr;
				return;
			}
		}
	}

	void find_previous_active()
	{
		while (m_block)
		{
			while (m_index > 0)
			{
				--m_index;
				if (m_block->m_activity[m_index] == 1)
				{
					m_value = &m_block->m_elements[m_index];
					return;
				}
			}

			auto it = std::find(m_storage->blocks.begin(), m_storage->blocks.end(), m_block);
			if (it != m_storage->blocks.begin())
			{
				--it;
				m_block = *it;
				m_index = m_block->capacity - 1;
			}
			else
			{
				m_block = nullptr;
				m_value = nullptr;
				return;
			}
		}
	}
};

template< typename T >
class BucketStorage< T >::const_iterator
{
	friend class iterator;

  public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = const T*;
	using reference = const T&;
	using size_type = typename BucketStorage< T >::size_type;
	pointer m_value;
	BlockPtr m_block;

	const_iterator(pointer ptr = nullptr, BlockPtr block = nullptr, const BucketStorage* storage = nullptr, size_type index = 0) :
		m_value(ptr), m_block(block), m_storage(storage), m_index(index)
	{
		if (m_block && (m_value == nullptr || m_block->m_activity[m_index] == 0))
		{
			find_next_active();
		}
	}

	reference operator*() const { return *m_value; }
	pointer operator->() const { return m_value; }

	const_iterator& operator++()
	{
		++m_index;
		if (m_index >= m_block->capacity || m_block->m_activity[m_index] == 0)
		{
			find_next_active();
		}
		else
		{
			m_value = &m_block->m_elements[m_index];
		}
		return *this;
	}

	const_iterator operator++(int)
	{
		const_iterator tmp = *this;
		++(*this);
		return tmp;
	}

	const_iterator& operator--()
	{
		if (m_index == 0)
		{
			find_previous_active();
		}
		else
		{
			--m_index;
			m_value = &m_block->m_elements[m_index];
		}
		return *this;
	}

	const_iterator operator--(int)
	{
		const_iterator tmp = *this;
		--(*this);
		return tmp;
	}

	bool operator==(const const_iterator& other) const { return m_value == other.m_value && m_block == other.m_block; }

	bool operator!=(const const_iterator& other) const { return !(*this == other); }

	bool operator<(const const_iterator& other) const
	{
		if (m_block == other.m_block)
		{
			return m_index < other.m_index;
		}
		return m_block < other.m_block;
	}

	bool operator<=(const const_iterator& other) const { return *this < other || *this == other; }

	friend bool operator==(const const_iterator& lhs, const iterator& rhs)
	{
		return lhs.m_value == rhs.m_value && lhs.m_block == rhs.m_block;
	}

	friend bool operator!=(const const_iterator& lhs, const iterator& rhs) { return !(lhs == rhs); }

  private:
	friend class BucketStorage< T >;
	const BucketStorage* m_storage;
	size_type m_index;

	void find_next_active()
	{
		while (m_block)
		{
			while (m_index < m_block->capacity && m_block->m_activity[m_index] == 0)
			{
				++m_index;
			}
			if (m_index < m_block->capacity)
			{
				m_value = &m_block->m_elements[m_index];
				return;
			}

			auto it = std::find(m_storage->blocks.begin(), m_storage->blocks.end(), m_block);
			if (++it != m_storage->blocks.end())
			{
				m_block = *it;
				m_index = 0;
			}
			else
			{
				m_block = nullptr;
				m_value = nullptr;
				return;
			}
		}
	}

	void find_previous_active()
	{
		while (m_block)
		{
			while (m_index > 0 && m_block->m_activity[m_index] == 0)
			{
				--m_index;
			}
			if (m_block->m_activity[m_index] == 1)
			{
				m_value = &m_block->m_elements[m_index];
				return;
			}

			auto it = std::find(m_storage->blocks.begin(), m_storage->blocks.end(), m_block);
			if (it != m_storage->blocks.begin())
			{
				--it;
				m_block = *it;
				m_index = m_block->capacity - 1;
			}
			else
			{
				m_block = nullptr;
				m_value = nullptr;
				return;
			}
		}
	}
};

template< typename T >
BucketStorage< T >::BucketStorage(size_type block_capacity) noexcept :
	total_size(0), total_capacity(0), block_capacity_(block_capacity)
{
}

template< typename T >
BucketStorage< T >::BucketStorage(const BucketStorage& other)
{
}

template< typename T >
BucketStorage< T >::BucketStorage(BucketStorage&& other) noexcept
{
	blocks = std::move(other.blocks);
	total_size = other.total_size;
	total_capacity = other.total_capacity;
	block_capacity_ = other.block_capacity_;

	other.total_size = 0;
	other.total_capacity = 0;
	other.block_capacity_ = 0;
}

template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(const BucketStorage& other)
{
	if (this != &other)
	{
		clear();
		block_capacity_ = other.block_capacity_;
		total_size = other.total_size;
		total_capacity = other.total_capacity;

		for (const auto& block : other.blocks)
		{
			Block* new_block = new Block(block->capacity);
			for (size_type i = 0; i < block->capacity; ++i)
			{
				if (block->m_activity[i] == 1)
				{
					new_block->construct_at(i, block->m_elements[i]);
				}
			}
			blocks.push_back(new_block);
		}
	}
	return *this;
}

template< typename T >
BucketStorage< T >& BucketStorage< T >::operator=(BucketStorage&& other) noexcept
{
	if (this != &other)
	{
		clear();
		blocks = std::move(other.blocks);
		total_size = other.total_size;
		total_capacity = other.total_capacity;
		block_capacity_ = other.block_capacity_;
	}
	return *this;
}

template< typename T >
BucketStorage< T >::~BucketStorage()
{
	clear();
}

template< typename T >
struct BucketStorage< T >::Block
{
	T* m_elements;
	size_type* m_activity;
	size_type active_count;
	size_type capacity;

	Block(size_type cap) :
		m_elements(static_cast< T* >(::operator new[](cap * sizeof(T)))), m_activity(new size_type[cap]{}),
		active_count(0), capacity(cap)
	{}



	~Block()
	{
		for (size_type i = 0; i < capacity; ++i)
		{
			if (m_activity[i] == 1)
			{
				m_elements[i].~T();
			}
		}

		delete[] m_activity;

		 ::operator delete[](m_elements);
	}

	void construct_at(size_type index, const T& value)
	{
		new (&m_elements[index]) T(value);
		m_activity[index] = 1;
		++active_count;
	}

	void construct_at(size_type index, T&& value)
	{
		new (&m_elements[index]) T(std::move(value));
		m_activity[index] = 1;
		++active_count;
	}

	void destroy_at(size_type index)
	{
		if (m_activity[index] == 1)
		{
			m_elements[index].~T();
			m_activity[index] = 0;
			--active_count;
		}
	}
};

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::insert(T&& value)
{
	if (total_size == total_capacity)
	{
		Block* new_block = new Block(block_capacity_);
		blocks.push_back(new_block);
		total_capacity += block_capacity_;
	}

	for (auto& block : blocks)
	{
		for (size_type i = 0; i < block->capacity; ++i)
		{
			if (block->m_activity[i] == 0)
			{
				block->construct_at(i, std::move(value));
				block->m_activity[i] = 1;
				++block->active_count;
				++total_size;
				return iterator(&block->m_elements[i], block, this);
			}
		}
	}

	throw std::runtime_error("Failed to insert value.");
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::insert(const T& value)
{
	return insert(T(value));
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::erase(iterator it)
{
	if (it == end())
	{
		throw std::out_of_range("Iterator points to end of container");
	}

	Block* blk = it.m_block;
	size_type idx = it.m_index;

	if (blk->m_activity[idx] == 0)
	{
		throw std::runtime_error("Element already removed");
	}

	blk->destroy_at(idx);
	blk->m_activity[idx] = 0;
	--blk->active_count;
	--total_size;

	if (blk->active_count == 0)
	{
		blocks.erase(std::find(blocks.begin(), blocks.end(), blk));
		delete blk;
		total_capacity -= block_capacity_;
		return end();
	}

	for (size_type i = idx + 1; i < blk->capacity; ++i)
	{
		if (blk->m_activity[i] == 1)
		{
			return iterator(&blk->m_elements[i], blk, this, i);
		}
	}

	return end();
}

template< typename T >
bool BucketStorage< T >::empty() const noexcept
{
	return total_size == 0;
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::size() const noexcept
{
	return total_size;
}

template< typename T >
typename BucketStorage< T >::size_type BucketStorage< T >::capacity() const noexcept
{
	return blocks.empty() ? 0 : total_capacity;
}

template< typename T >
void BucketStorage< T >::shrink_to_fit()
{
	BucketStorage< T > compacted_storage(block_capacity_);

	for (auto& blk : blocks)
	{
		for (size_type i = 0; i < blk->capacity; ++i)
		{
			if (blk->m_activity[i] == 1)
			{
				compacted_storage.insert(std::move(blk->m_elements[i]));
				blk->m_elements[i].~T();
			}
		}
	}

	swap(compacted_storage);
}

template< typename T >
void BucketStorage< T >::clear() noexcept
{
	for (auto& block : blocks)
	{
		delete block;
	}
	blocks.clear();
	total_size = 0;
	total_capacity = 0;
}

template< typename T >
void BucketStorage< T >::swap(BucketStorage& other) noexcept
{
	blocks.swap(other.blocks);
	std::swap(total_size, other.total_size);
	std::swap(total_capacity, other.total_capacity);
	std::swap(block_capacity_, other.block_capacity_);
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::begin() noexcept
{
	if (blocks.empty())
	{
		return end();
	}

	for (auto& block : blocks)
	{
		for (size_type i = 0; i < block->capacity; ++i)
		{
			if (block->m_activity[i] == 1)
			{
				return iterator(&block->m_elements[i], block, this, i);
			}
		}
	}

	return end();
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::end() const noexcept
{
	return const_iterator(nullptr, nullptr, this);
}

template< typename T >
[[nodiscard]] typename BucketStorage< T >::const_iterator BucketStorage< T >::begin() const noexcept
{
	return cbegin();
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::cbegin() const noexcept
{
	if (blocks.empty())
	{
		return cend();
	}

	for (const auto& block : blocks)
	{
		for (size_type i = 0; i < block->capacity; ++i)
		{
			if (block->m_activity[i] == 1)
			{
				return const_iterator(&block->m_elements[i], block, this, i);
			}
		}
	}

	return cend();
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::end() noexcept
{
	return iterator(nullptr, nullptr, this);
}

template< typename T >
typename BucketStorage< T >::const_iterator BucketStorage< T >::cend() const noexcept
{
	return const_iterator(nullptr, nullptr, this);
}

template< typename T >
typename BucketStorage< T >::iterator BucketStorage< T >::get_to_distance(iterator it, difference_type distance)
{
	while (distance != 0)
	{
		if (distance > 0)
		{
			if (it == end())
			{
				throw std::out_of_range("Attempt to move past the end");
			}
			++it;
			--distance;
		}
		else
		{
			if (it == begin())
			{
				throw std::out_of_range("Attempt to move past the beginning");
			}
			--it;
			++distance;
		}
	}
	return it;
}

namespace std
{
	template< typename T >
	void swap(BucketStorage< T >& a, BucketStorage< T >& b) noexcept
	{
		a.swap(b);
	}
}	 // namespace std

#endif /* BUCKET_STORAGE_HPP */
