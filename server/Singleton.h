#pragma once
#include <memory>

class Erase
{
public:
	Erase() = default;
	Erase(Erase const&) = delete;
	Erase& operator=(Erase const&) = delete;
	Erase(Erase&&) = delete;
	Erase& operator=(Erase&&) = delete;

	virtual ~Erase() = default;
};

template <class T>
class Singleton : private Erase
{
public:
	static T& get_instance()
	{
		if (!instance)
			instance = std::make_shared<T_Instance>();
		return *instance;
	}

protected:
	Singleton() = default;

private:
	struct T_Instance : public T
	{
		T_Instance() : T() {}
	};

	static inline std::shared_ptr<T> instance = nullptr;
};