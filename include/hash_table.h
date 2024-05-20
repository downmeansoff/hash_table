#include <iostream>
#include <vector>
#include <cmath>
#include <random>

template<class Key,class Value>
struct Pair {
	Key key;
	Value value;
	bool is_filled;

	friend std::ostream& operator<<(std::ostream& stream,const Pair& pair) {
		stream << "(" << pair.key << "," << pair.value << ")";
		return stream;
	}
};

template <class Key, class Value>
class HashTable {
	std::vector<Pair<Key, Value>> _data;
	size_t _size;
public:
	HashTable(size_t size): _size(size) {
		if (size == 0)  throw std::invalid_argument("invalid size");
		_data.resize(size);
	}

	HashTable(size_t size, const Value& min, const Value& max){
		if (size == 0) throw std::invalid_argument("size = 0");//создаем хеш таблицу, заполненную рандомными значениями
		_size = size;
		_data.resize(size);
		for (size_t i = 0; i < size; ++i) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dist(min, max);//создаем рандомное число в промежутке от мин до макс
			_data[i] = new Pair{ i, dist(gen), true };
		}
	}

	size_t get_size() {
		return _size; 
	}

	size_t hash(const Key& key) {
		return key % _size;
	}

	void print(){
		for (auto& pair : _data)
		{
			if (pair.is_filled) std::cout << pair<< std::endl;
		}
	}
};

