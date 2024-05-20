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
}
