#pragma once

#include <memory>

/// <summary>
/// シングルトンの基底クラス. CRTPを使用している
/// <para>GetInstance()でインスタンス取得, Destroy()でインスタンス破棄</para>
/// </summary>
/// <typeparam name="T"></typeparam>
template<class T>
class Singleton
{
public:
	static T& GetInstance()
	{
		if (!_instance)
		{
			_instance.reset(new T());
		}
		return *_instance;
	}

	static void Destroy()
	{
		if (!_instance)
		{
			return;
		}
		_instance->Finalize();
		_instance.reset();
	}

	virtual ~Singleton() {}

	//~ Begin Singleton interface
	virtual void Finalize() {}
	//~ End Singleton interface

protected:
	Singleton()	{}

private:
	// コピーとムーブの禁止
	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton& operator=(Singleton&&) = delete;

	static std::unique_ptr<T> _instance;
};

template<class T>
std::unique_ptr<T> Singleton<T>::_instance = nullptr;