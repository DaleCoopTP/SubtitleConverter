#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include <initializer_list>
template< typename T >
class DynamicArray
{
private:
  T* data;
  int _size;
  int _capacity;

  void resize(int newCapacity)
  {
    T* newData = new T[newCapacity];
    for (int i = 0; i < _size; ++i)
    {
      newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    _capacity = newCapacity;
  }

public:
  DynamicArray(std::initializer_list< T > init)
  {
    _size = init.size();
    _capacity = init.size();
    data = new T[_capacity];
    int i = 0;
    for (const auto& elem : init)
    {
      data[i++] = elem;
    }
  }

  DynamicArray()
  {
    _size = 0;
    _capacity = 4;
    data = new T[_capacity];
  }

  ~DynamicArray() { delete[] data; }

  void push_back(const T& value)
  {
    if (_size == _capacity)
    {
      resize(_capacity * 2);
    }
    data[_size++] = value;
  }

  T& operator[](int index) { return data[index]; }

  const T& operator[](int index) const { return data[index]; }

  int size() const { return _size; }

  int capacity() const { return _capacity; }

  void clear() { _size = 0; }

  T* begin() { return data; }

  T* end() { return data + _size; }

  const T* begin() const { return data; }

  const T* end() const { return data + _size; }
};

#endif
