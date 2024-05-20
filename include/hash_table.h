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

	bool operator==(const Pair& other){
		return key == other.key && value == other.value && is_filled == other.is_filled;
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
			_data[i] = new Pair{ static_cast<Key>(i), dist(gen), nullptr };
		}
	}

	//  ��� ����������
	HashTable(size_t size, size_t count_pairs, const Value& min, const Value& max, size_t& collision_counter) : _size(size), _count_pairs(count_pairs) {
		// ���������, ��� ������ ���-������� �� ����� ����
		if (size == 0) throw std::invalid_argument("size = 0");
		// �������� ������ ������� ������ �� ��������� ������
		_data.resize(size);
		// ������� ������� ��� ��������� ��������� �����
		std::random_device rd; // ���������� ���������� ��� ��������� ��������� �����
		std::mt19937 gen(rd()); // ������� ��������� ��������� ����� � ������ �� rd()
		std::uniform_int_distribution<> dist(min, max); // ������� ������������� ��� ��������� ����� � ��������� �� min �� max
		// ��������� ���-������� ���������� ������ ����-��������
		for (size_t i = 0; i < count_pairs; ++i) {
			insert_stat(dist(gen), dist(gen), collision_counter);
		}
	}

	// ���������� ��������� ��������� ��� ���-������
	bool operator==(const HashTable& other) {
		// ��������� ��������� �������� � ���������� ���
		if (_size != other._size || _count_pairs != other._count_pairs) return false;
		// ���������� ���������� ������ ���� � ���-��������
		for (size_t i = 0; i < std::min(this->_data.size(), other._data.size()); ++i) {
			if (_data[i] != other._data[i]) return false;
		}
		return true; // ���������� true, ���� ���-������� ���������
	}

	// ����������� ����������� ��� ���-�������
	HashTable(const HashTable& other) : _size(other._size), _count_pairs(other._count_pairs) {
		_data.resize(other._size); // �������� ������ ������� ������
		if (other == *this) return; // ��������� �� ����������������
		for (auto& pair : other._data) {
			if (pair.is_filled) insert(pair.key, pair.value); // ��������� ����������� ��������
		}
	}

	// ���������� ��������� ������������ ��� ���-�������
	HashTable& operator=(const HashTable& other) {
		if (this != &other) {
			_size = other._size; // ����������� ����� ������
			_count_pairs = other._count_pairs; // ����������� ����� ���������� ���
			_data = other._data; // ����������� ������
		}
		return *this; // ���������� ������� ������
	}

	// ������� ���������� ������� ���-�������
	void grow() {
		std::vector<Pair<Key, Value>> tmp;
		tmp.resize(_size * 1.7); // ������� ��������� ������ ������������ �������
		std::swap(tmp, _data); // �������� ������� ������ �� �����
		_size = _size * 1.7; // ��������� ������ ���-�������
		for (auto i = 0; i < tmp.size(); ++i) {
			if (tmp[i].is_filled) {
				insert(tmp[i].key, tmp[i].value); // ��������� ����������� �������� � ����� ������
			}
		}
	}

	// ������� ������� �������� � ���-�������
	void insert(const Key& key, const Value& value) {
		if (_size != 0) {
			double load_factor = _count_pairs / (_size + 0.0);
			if (load_factor > 0.6) grow(); // ����������� ������ ������� ��� ���������� ������������ ��������
		}
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			if (!_data[index].is_filled) {
				_data[index] = Pair{ key, value, true }; // ��������� ����� �������
				++_count_pairs; // ����������� ���������� ���
				return;
			}
			if (_data[index].key == key) {
				throw std::logic_error("key is exist"); // �������� ����������, ���� ���� ��� ����������
			}
		}
	}

	// ������� ������� �������� ��� ��������������� ������� ��������
	void insert_stat(const Key& key, const Value& value, size_t& collision_counter) {
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			// ���������, �������� �� �������
			if (!_data[index].is_filled) {
				_data[index] = Pair{ key, value, true }; // ��������� ����� �������
				++_count_pairs; // ����������� ���������� ���
				return;
			}
			else {
				++collision_counter; // ����������� ������� ��������
			}
		}
	}

	// ������� ������� ��� ���������� �������� � ���-�������
	void insert_or_assign(const Key& key, const Value& value) {
		if (_size != 0) {
			double load_factor = _count_pairs / (_size + 0.0);
			if (load_factor > 0.6) grow(); // ����������� ������ ������� ��� ���������� ������������ ��������
		}
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _size;
			if (!_data[index].is_filled) {
				_data[index] = Pair{ key, value, true }; // ��������� ����� �������
				++_count_pairs; // ����������� ���������� ���
				return;
			}
			if (_data[index].key == key) {
				_data[index].value = value; // ��������� ��������, ���� ���� ��� ����������
				return;
			}
		}
	}

	// ������� �������� ������� �������� � ���-�������
	bool contains(Value value) {
		for (auto& pair : _data) {
			// ���������, �������� �� ������� � ������������� �� ��������
			if (pair.is_filled && (pair.value == value)) return true; // ���� �������� �������, ���������� true
		}
		return false; // ���� �������� �� �������, ���������� false
	}

	// ������� �������� �������� �� ����� �� ���-�������
	bool erase(const Key& key) {
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			// ���������, �������� �� ������� � ������������� �� ����
			if (_data[index].is_filled && _data[index].key == key) {
				_data[index].is_filled = false; // �������� ������� ��� �� �����������
				--_count_pairs; // ��������� ���������� ���
				return true; // ���������� true, ���� ������� ������
			}
		}
		return false; // ���������� false, ���� ������� �� ������ � �� ������
	}

	// ������� ������ �������� �� ����� � ���-�������
	Value* search(const Key& key) {
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			// ���������, �������� �� ������� � ������������� �� ����
			if (_data[index].is_filled && _data[index].key == key) {
				return &(_data[index].value); // ���������� ��������� �� ��������
			}
		}
		return nullptr; // ���� ���� �� ������, ���������� nullptr
	}

	// ������� �������� �������� ��� ������������� ����� � ���-�������
	size_t count(const Key& key) {
		size_t collisions = 0;
		for (size_t i = 0; i < _size; ++i) {
			// ���������, �������� �� ������� � ���������� �� �������� � ������ ������
			if (_data[i].is_filled && (hash(key) == hash(_data[i].key && _data[i].key != key))) {
				++collisions; // ����������� ������� ��������
			}
		}
		return collisions; // ���������� ���������� �������� ��� ������� �����
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
		for (auto& pair : _data){
			if (pair.is_filled) std::cout << pair<< std::endl;
		}
	}

	~HashTable() = default;
};


void statistics(const size_t table_size) {
	size_t count_nodes = 26; // ���������� ������� � ������ (�� �������)
	size_t sum = 0; // ����� ��������
	// �������� �������������� ������ ��� 100 ��������
	for (auto i = 0; i < 100; ++i) {
		size_t collision_counter = 0; // ������� �������� ��� ������� ��������

		// ������� ����� ���-������� � ��������� �������� � ����������� �����
		HashTable<int, int> hashTable(table_size, count_nodes, 0, 1000, collision_counter);
		sum += collision_counter; // ����������� ����� ��������
	}

	// ������� ���������� �� ���������� ��������
	std::cout << "size=" << table_size << "    average count of collisions=" << sum / 100 << "\n";
}