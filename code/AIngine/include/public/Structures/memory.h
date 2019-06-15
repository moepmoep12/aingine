#pragma once

#include <memory>
#include <stdexcept>
#include <cassert>

namespace AIngine::Memory {

	// Convenience

	// User-defined literals https://en.cppreference.com/w/cpp/language/user_literal
	constexpr std::size_t operator""_kB(unsigned long long v) {
		return 1024u * v;
	}

	constexpr std::size_t operator""_mB(unsigned long long v) {
		return 1024u * 1024u * v;
	}

	class Stack final {
	public:
		explicit Stack(size_t size) {
		    if(size == 0)
		        return;

			data = (std::byte *) malloc(size);
			if (data == nullptr)
				throw std::runtime_error("Stack: Failed to allocate memory!");

			top = data;
			upperBound = data + size;
		}

        Stack& operator=(Stack&& other) {
		    // Free resources
		    if(data != nullptr)
		        free(data);

            this->data = other.data;
            this->top = other.top;
            this->upperBound = other.upperBound;

            // destructor of other will be called at some point
            other.data = nullptr;

            return *this;
        }

		~Stack() {
			free(data);
		}

		void *Allocate(size_t size) { 
			assert(top + size <= upperBound);
			void * ptr = top;
			top = top + size;
			return ptr;
		}

		void Reset() {
			top = data;
		}

	private:
		std::byte *data = nullptr;
		std::byte *top = nullptr;
		std::byte *upperBound = nullptr;
	};

	template<typename T>
	class Pool final {
		union FreeList {
			T data;
			FreeList *next;
		};
	public:
		Pool(size_t size) {
			lowerBound = (FreeList *)malloc(sizeof(FreeList) * size);
			if (lowerBound == nullptr)
				throw std::runtime_error("Pool: Failed to allocate memory!");

			freeList = lowerBound;
			upperBound = lowerBound + size;

			FreeList * fl = freeList;
			for (int i = 0; i < size - 1; i++) {
				fl->next = fl + 1;
				fl = fl->next;
			}
			fl->next = nullptr;
		}

		~Pool() {
			free(lowerBound);
		}

		T *Allocate() {
			if (freeList == nullptr)
				return nullptr;

			T* ptr = (T*) &freeList->data;
			freeList = freeList->next;
			return ptr;
		} 

		void Free(T *ptr) {
			FreeList * tmp = (FreeList *)ptr;
			assert(lowerBound <= tmp && tmp <= upperBound);
			tmp->next = freeList;
			freeList = tmp;
		}

	private:
		FreeList * freeList = nullptr;
		FreeList *lowerBound = nullptr;
		FreeList *upperBound = nullptr;
	};
}