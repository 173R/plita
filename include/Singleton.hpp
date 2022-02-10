#pragma once

#include <cassert>

template <class T>
class Singleton {
public:
  Singleton() {
    assert(!instance_);
    instance_ = static_cast<T*>(this);
  }

  ~Singleton() {
    assert(instance_);
    instance_ = nullptr;
  }

  static T* Instance() {
    assert(instance_);
    return instance_;
  }

  Singleton(const Singleton<T>&) = delete;
  Singleton& operator= (const Singleton<T>&) = delete;
protected:
  static T* instance_;
};