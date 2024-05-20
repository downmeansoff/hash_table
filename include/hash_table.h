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

	//  для статистики
	HashTable(size_t size, size_t count_pairs, const Value& min, const Value& max, size_t& collision_counter) : _size(size), _count_pairs(count_pairs) {
		// Проверяем, что размер хеш-таблицы не равен нулю
		if (size == 0) throw std::invalid_argument("size = 0");
		// Изменяем размер вектора данных на указанный размер
		_data.resize(size);
		// Создаем объекты для генерации случайных чисел
		std::random_device rd; // Используем устройство для генерации случайных чисел
		std::mt19937 gen(rd()); // Создаем генератор случайных чисел с зерном из rd()
		std::uniform_int_distribution<> dist(min, max); // Создаем распределение для случайных чисел в диапазоне от min до max
		// Заполняем хеш-таблицу случайными парами ключ-значение
		for (size_t i = 0; i < count_pairs; ++i) {
			insert_stat(dist(gen), dist(gen), collision_counter);
		}
	}

	// Перегрузка оператора сравнения для хеш-таблиц
	bool operator==(const HashTable& other) {
		// Проверяем равенство размеров и количества пар
		if (_size != other._size || _count_pairs != other._count_pairs) return false;
		// Сравниваем содержимое каждой пары в хеш-таблицах
		for (size_t i = 0; i < std::min(this->_data.size(), other._data.size()); ++i) {
			if (_data[i] != other._data[i]) return false;
		}
		return true; // Возвращаем true, если хеш-таблицы идентичны
	}

	// Конструктор копирования для хеш-таблицы
	HashTable(const HashTable& other) : _size(other._size), _count_pairs(other._count_pairs) {
		_data.resize(other._size); // Изменяем размер вектора данных
		if (other == *this) return; // Проверяем на самоприсваивание
		for (auto& pair : other._data) {
			if (pair.is_filled) insert(pair.key, pair.value); // Вставляем заполненные элементы
		}
	}

	// Перегрузка оператора присваивания для хеш-таблицы
	HashTable& operator=(const HashTable& other) {
		if (this != &other) {
			_size = other._size; // Присваиваем новый размер
			_count_pairs = other._count_pairs; // Присваиваем новое количество пар
			_data = other._data; // Присваиваем данные
		}
		return *this; // Возвращаем текущий объект
	}

	// Функция увеличения размера хеш-таблицы
	void grow() {
		std::vector<Pair<Key, Value>> tmp;
		tmp.resize(_size * 1.7); // Создаем временный вектор увеличенного размера
		std::swap(tmp, _data); // Заменяем текущий вектор на новый
		_size = _size * 1.7; // Обновляем размер хеш-таблицы
		for (auto i = 0; i < tmp.size(); ++i) {
			if (tmp[i].is_filled) {
				insert(tmp[i].key, tmp[i].value); // Переносим заполненные элементы в новый вектор
			}
		}
	}

	// Функция вставки элемента в хеш-таблицу
	void insert(const Key& key, const Value& value) {
		if (_size != 0) {
			double load_factor = _count_pairs / (_size + 0.0);
			if (load_factor > 0.6) grow(); // Увеличиваем размер таблицы при превышении коэффициента загрузки
		}
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			if (!_data[index].is_filled) {
				_data[index] = Pair{ key, value, true }; // Вставляем новый элемент
				++_count_pairs; // Увеличиваем количество пар
				return;
			}
			if (_data[index].key == key) {
				throw std::logic_error("key is exist"); // Вызываем исключение, если ключ уже существует
			}
		}
	}

	// Функция вставки элемента для статистического анализа коллизий
	void insert_stat(const Key& key, const Value& value, size_t& collision_counter) {
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			// Проверяем, заполнен ли элемент
			if (!_data[index].is_filled) {
				_data[index] = Pair{ key, value, true }; // Вставляем новый элемент
				++_count_pairs; // Увеличиваем количество пар
				return;
			}
			else {
				++collision_counter; // Увеличиваем счетчик коллизий
			}
		}
	}

	// Функция вставки или обновления элемента в хеш-таблице
	void insert_or_assign(const Key& key, const Value& value) {
		if (_size != 0) {
			double load_factor = _count_pairs / (_size + 0.0);
			if (load_factor > 0.6) grow(); // Увеличиваем размер таблицы при превышении коэффициента загрузки
		}
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _size;
			if (!_data[index].is_filled) {
				_data[index] = Pair{ key, value, true }; // Вставляем новый элемент
				++_count_pairs; // Увеличиваем количество пар
				return;
			}
			if (_data[index].key == key) {
				_data[index].value = value; // Обновляем значение, если ключ уже существует
				return;
			}
		}
	}

	// Функция проверки наличия значения в хеш-таблице
	bool contains(Value value) {
		for (auto& pair : _data) {
			// Проверяем, заполнен ли элемент и соответствует ли значение
			if (pair.is_filled && (pair.value == value)) return true; // Если значение найдено, возвращаем true
		}
		return false; // Если значение не найдено, возвращаем false
	}

	// Функция удаления элемента по ключу из хеш-таблицы
	bool erase(const Key& key) {
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			// Проверяем, заполнен ли элемент и соответствует ли ключ
			if (_data[index].is_filled && _data[index].key == key) {
				_data[index].is_filled = false; // Помечаем элемент как не заполненный
				--_count_pairs; // Уменьшаем количество пар
				return true; // Возвращаем true, если элемент удален
			}
		}
		return false; // Возвращаем false, если элемент не найден и не удален
	}

	// Функция поиска значения по ключу в хеш-таблице
	Value* search(const Key& key) {
		for (size_t i = 0; i < _data.size(); ++i) {
			size_t index = (hash(key) + i * std::hash<Key>{}(key)) % _data.size();
			// Проверяем, заполнен ли элемент и соответствует ли ключ
			if (_data[index].is_filled && _data[index].key == key) {
				return &(_data[index].value); // Возвращаем указатель на значение
			}
		}
		return nullptr; // Если ключ не найден, возвращаем nullptr
	}

	// Функция подсчета коллизий для определенного ключа в хеш-таблице
	size_t count(const Key& key) {
		size_t collisions = 0;
		for (size_t i = 0; i < _size; ++i) {
			// Проверяем, заполнен ли элемент и происходит ли коллизия с другим ключом
			if (_data[i].is_filled && (hash(key) == hash(_data[i].key && _data[i].key != key))) {
				++collisions; // Увеличиваем счетчик коллизий
			}
		}
		return collisions; // Возвращаем количество коллизий для данного ключа
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
	size_t count_nodes = 26; // Количество человек в группе (по заданию)
	size_t sum = 0; // Сумма коллизий
	// Проводим статистический анализ для 100 итераций
	for (auto i = 0; i < 100; ++i) {
		size_t collision_counter = 0; // Счетчик коллизий для текущей итерации

		// Создаем новую хеш-таблицу с указанным размером и количеством узлов
		HashTable<int, int> hashTable(table_size, count_nodes, 0, 1000, collision_counter);
		sum += collision_counter; // Увеличиваем сумму коллизий
	}

	// Выводим статистику по количеству коллизий
	std::cout << "size=" << table_size << "    average count of collisions=" << sum / 100 << "\n";
}