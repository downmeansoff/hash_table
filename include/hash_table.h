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
	size_t _count_pairs;

public:
	HashTable(size_t size): _size(size), _count_pairs(0) {
		if (size == 0)  throw std::invalid_argument("size = 0");
		_data.resize(size);
	}

	HashTable(size_t size, const Value& min, const Value& max){
		if (size == 0) throw std::invalid_argument("size = 0");
		_size = size * 1.7;
		_count_pairs = size;
		_data.resize(size);
		for (size_t i = 0; i < size; ++i) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> dist(min, max);
			_data[i] = new Pair{ i, dist(gen), true };
		}
	}

	void grow() {
		std::vector<Pair<Key, Value>> tmp;
		tmp.resize(_size * 1.7);
		std::swap(tmp, _data);
		_size = 0;
		for (auto i = 0; i < tmp.size(); ++i) {
			if (tmp[i].is_filled) {
				insert(tmp[i].key, tmp[i].value);
			}
		}
	}

	void insert(const Key& key, const Value& value) {
		if (_size) {
			double load_factor = _count_pairs / (_size + 0.0);
			if (load_factor > 0.6) grow();
		}
		for (size_t i = 0; i < _data.size(); ++i)
		{
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			if (!_data[index].is_filled)
			{
				_data[index] = Pair{ key, value, true };
				++_count_pairs;
				return;
			}
			if (_data[index].key == key) throw std::logic_error("key is exist");
		}
	}

	void insert_or_assign(const Key& key, const Value& value) {
		if (_size) {
			double load_factor = _count_pairs / (_size + 0.0);
			if (load_factor > 0.6) grow();
		}
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			if (!_data[index].is_filled) {
				_data[index] = Pair{ key, value, true };
				++_count_pairs;
				return;
			}
			if (_data[index].key == key) {
				_data[index].value = value;
				return;
			}
		}
	}

	size_t get_size() {
		return _size; 
	}

	size_t get_count() {
		return _count_pairs;
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

