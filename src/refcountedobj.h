#ifndef FS_REFCOUNTEDOBJ_H_E2D116BEF1A63CD2F76CA9970E18E44B
#define FS_REFCOUNTEDOBJ_H_E2D116BEF1A63CD2F76CA9970E18E44B
#include <type_traits>
template<typename T>
class RefCountedTypeConstraintChecker;

/** A base class that enables the use of a lightweight reference counting smart pointer
 *  \warning T is required to have a virtual destructor or be the top class in an inheritance
 *  hierarchy. If T fails to satisfy these constraints, Undefined Behaviour is
 *  triggered when reference counts goes to 0 and T is deleted.
 */
template <typename T>
class ReferenceCountedObject
{
public:
	void incrementReferenceCounter() noexcept {
		++referenceCounter;
	}
	void decrementReferenceCounter() {
		if (--referenceCounter == 0) {
			//The cast is required so that we properly call destructors
			delete static_cast<T*>(this);
		}
	}
private:
	uint32_t referenceCounter = 0;
protected:
	ReferenceCountedObject() = default;
};

template<typename T>
class ReferenceCountedPtr final
{
public:
	static_assert(std::is_base_of<ReferenceCountedObject<T>, T>::value, "T is required to inherit from ReferenceCountedObject.");
	ReferenceCountedPtr() noexcept = default;
	
	ReferenceCountedPtr(T* ptr) noexcept {
		this->ptr = ptr;
		internalAcquirePtr();
	}
	ReferenceCountedPtr(const ReferenceCountedPtr& other) noexcept {
		ptr = other.ptr;
		internalAcquirePtr();
	}
	ReferenceCountedPtr(ReferenceCountedPtr&& other) noexcept {
		ptr = other.ptr;
		other.ptr = nullptr;
	}

	ReferenceCountedPtr& operator=(const T* ptr) {
		internalReleasePtr();
		this->ptr = ptr;
		internalAcquirePtr();
		return *this;
	}
	ReferenceCountedPtr& operator=(const ReferenceCountedPtr& other) {
		internalReleasePtr();
		ptr = other.ptr;
		internalAcquirePtr();
		return *this;
	}
	ReferenceCountedPtr& operator=(ReferenceCountedPtr&& other) noexcept {
		internalReleasePtr();
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}
	
	bool operator==(const T* other) const noexcept {
		return ptr == other;
	}

	void reset(const T* newPtr = nullptr) {
		internalReleasePtr();
		ptr = newPtr;
		internalAcquirePtr();
	}
	
	T* get() const noexcept {
		return ptr;
	}
	
	T* release() noexcept {
		auto ret = ptr;
		ptr = nullptr;
		return ret;
	}
	
	T& operator*() const noexcept {
		return *ptr;
	}
	
	T* operator->() const noexcept {
		return ptr;
	}
	
	operator bool() const noexcept {
		return ptr;
	}
	
	~ReferenceCountedPtr() {
		internalReleasePtr();
	}
private:
	void internalAcquirePtr() const noexcept {
		if (ptr) {
			ptr->incrementReferenceCounter();
		}
	}
	void internalReleasePtr() const {
		if (ptr) {
			ptr->decrementReferenceCounter();
		}
	}
	T* ptr = nullptr;
};

template<typename U, typename T>
ReferenceCountedPtr<U> dynamic_pointer_cast(const ReferenceCountedPtr<T>& ptr) {
	return ReferenceCountedPtr<U>(static_cast<U*> (ptr.get()));
}

#endif