#include <hash_table.h>
#include <gtest/gtest.h>
#include <iostream>

TEST(pair_test, constructor_params) {
	Pair<int, int> pair(1, 2);
	ASSERT_EQ(pair.key, 1);
	ASSERT_EQ(pair.value, 2);
}

TEST(hash_table_test, default_constr) {
	HashTable<int,int> ht(11);
	ASSERT_EQ(ht.get_size(),11);
	ASSERT_EQ(ht.get_count(), 0);
}

TEST(hash_table_test, insert) {
	HashTable<int, int> ht(11);
	ASSERT_EQ(ht.get_count(), 0);
	ht.insert(12, 4);
	ASSERT_EQ(ht.get_count(), 1);
	ASSERT_THROW(ht.insert(12, 5), std::logic_error);
}

TEST(hash_table_test, insert_or_assign) {
	HashTable<int, int> ht(11);
	ASSERT_EQ(ht.get_count(), 0);
	ht.insert(12, 4);
	ASSERT_EQ(ht.get_count(), 1);
	ht.insert_or_assign(12, 5);
	ASSERT_EQ(ht.get_count(), 1);
}

TEST(hash_table_test, contains) {
	HashTable<int, int> ht(11);
	ht.insert(12, 4);
	ASSERT_TRUE(ht.contains(4));
	ASSERT_FALSE(ht.contains(3243));
}

TEST(hash_table_test, search) {
	HashTable<int, int> ht(11);
	ht.insert(12, 4);
	ASSERT_EQ(*ht.search(12), 4);
}

TEST(hash_table_test, erase) {
	HashTable<int, int> ht(11);
	ht.insert(12, 4);
	ASSERT_TRUE(ht.erase(12));
	ASSERT_FALSE(ht.erase(12));
	ASSERT_EQ(ht.get_count(), 0);
}


TEST(hash_table_test, count) {
	HashTable<int, int> ht(11);
	ht.insert(12, 4);
	ASSERT_EQ(ht.count(12), 0);
}
