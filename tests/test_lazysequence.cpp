#include "LazySequence.hpp"
#include "MutableArraySequence.hpp"
#include "exceptions.hpp"
#include "test_common.hpp"
#include <iostream>
#include <ranges>
#include <algorithm>
#include "DynamicArray.hpp"
#include <ranges>
using LS = LazySequence<int, MutableArraySequence>;


bool testEmpty() {
    std::cout << "  Тест пустой последовательности..." << std::endl;
    LS empty;
    TEST_ASSERT(empty.GetSizeSequence().IsFinalNumber() && empty.GetSizeSequence().GetSize() == 0, "длина не 0");
    TEST_ASSERT(empty.GetSizeCache() == 0, "кэш не пуст");
    try { empty.GetFirst(); TEST_ASSERT(false, "не выброшено исключение"); }
    catch (const EmptySequenceException&) {}
    try { empty.GetLast(); TEST_ASSERT(false, "не выброшено исключение"); }
    catch (const EmptySequenceException&) {}
    try { empty.Get(Cardinal(0)); TEST_ASSERT(false, "не выброшено исключение"); }
    catch (const OutOfRangeException&) {}
    std::cout << "    OK" << std::endl;
    return true;
}

bool testFiniteFromArray() {
    std::cout << "  Тест конечной последовательности из массива..." << std::endl;
    int arr[] = {1, 2, 3};
    LS seq(arr, 3);
    TEST_ASSERT(seq.GetSizeSequence().GetSize() == 3, "длина не 3");
    TEST_ASSERT(seq.GetFirst() == 1, "GetFirst");
    TEST_ASSERT(seq.GetLast() == 3, "GetLast");
    TEST_ASSERT(seq.Get(Cardinal(1)) == 2, "Get(1)");
    try { seq.Get(Cardinal(3)); TEST_ASSERT(false, "не выброшено исключение"); }
    catch (const OutOfRangeException&) {}
    std::cout << "    OK" << std::endl;
    return true;
}

bool testInfiniteNatural() {
    std::cout << "  Тест бесконечной последовательности..." << std::endl;
    auto naturals = LS::Recurrent(
        [](const MutableArraySequence<int>& prev) { return prev.GetLast() + 1; },
        MutableArraySequence<int>({0})
    );
    TEST_ASSERT(naturals.GetSizeSequence().IsInfiniteNumber(), "должна быть бесконечной");
    TEST_ASSERT(naturals.GetFirst() == 0, "GetFirst");
    TEST_ASSERT(naturals.Get(Cardinal(5)) == 5, "Get(5)");
    TEST_ASSERT(naturals.Get(Cardinal(100)) == 100, "Get(100)");
    std::cout << "    OK" << std::endl;
    return true;
}

bool testAppend() {
    std::cout << "  Тест Append..." << std::endl;
    try {
        std::cout << "    Создание seq{1, 2}..." << std::endl;
        LS seq({1, 2});
        std::cout << "    seq создана, длина = " << seq.GetSizeSequence().GetSize() << std::endl;
        std::cout << "    Вызов Append(3)..." << std::endl;
        LS seq2 = seq.Append(3);
        std::cout << "    seq2 создана, длина = " << seq2.GetSizeSequence().GetSize() << std::endl;
        std::cout << "    Проверка Get(2)..." << std::endl;
        int val = seq2.Get(Cardinal(2));
        std::cout << "    Получено значение: " << val << std::endl;
        if (seq2.GetSizeSequence().GetSize() != 3) {
            std::cerr << "Ожидалась длина 3, получено " << seq2.GetSizeSequence().GetSize() << std::endl;
            return false;
        }
        if (val != 3) {
            std::cerr << "Ожидался элемент 3, получен " << val << std::endl;
            return false;
        }
        std::cout << "    OK" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Исключение: " << e.what() << std::endl;
        return false;
    }
    return true;
}
bool testPrepend() {
    std::cout << "  Тест Prepend..." << std::endl;
    LS seq({2, 3});
    LS seq2 = seq.Prepend(1);
    TEST_ASSERT(seq2.GetSizeSequence().GetSize() == 3, "длина новой не 3");
    TEST_ASSERT(seq2.GetFirst() == 1, "первый элемент");
    TEST_ASSERT(seq2.Get(Cardinal(1)) == 2, "второй элемент");
    std::cout << "    OK" << std::endl;
    return true;
}

bool testInsertAt() {
    std::cout << "  Тест InsertAt..." << std::endl;
    LS seq({1, 3, 4});
    LS seq2 = seq.InsertAt(2, 1);
    TEST_ASSERT(seq2.GetSizeSequence().GetSize() == 4, "длина новой не 4");
    TEST_ASSERT(seq2.Get(Cardinal(1)) == 2, "вставленный элемент");
    LS seq3 = seq.InsertAt(0, 0);
    TEST_ASSERT(seq3.GetFirst() == 0, "вставка в начало");
    LS seq4 = seq.InsertAt(5, seq.GetSizeSequence().GetSize());
    TEST_ASSERT(seq4.GetLast() == 5, "вставка в конец");
    std::cout << "    OK" << std::endl;
    return true;
}

bool testConcat() {
    std::cout << "  Тест Concat..." << std::endl;
    LS a({1, 2});
    LS b({3, 4});
    LS c = a.Concat(b);
    TEST_ASSERT(c.GetSizeSequence().GetSize() == 4, "длина не 4");
    TEST_ASSERT(c.Get(Cardinal(0)) == 1 && c.Get(Cardinal(2)) == 3, "неправильная конкатенация");
    auto naturals = LS::Recurrent(
        [](const MutableArraySequence<int>& prev) { return prev.GetLast() + 1; },
        MutableArraySequence<int>({0})
    );
    LS combined = a.Concat(naturals);
    TEST_ASSERT(combined.GetSizeSequence().IsInfiniteNumber(), "должна быть бесконечной");
    TEST_ASSERT(combined.Get(Cardinal(0)) == 1 && combined.Get(Cardinal(2)) == 0, "неправильная конкатенация");
    std::cout << "    OK" << std::endl;
    return true;
}

bool testSkipFirst() {
    std::cout << "  Тест SkipFirst..." << std::endl;
    LS seq({1, 2, 3, 4, 5});
    LS skipped = seq.SkipFirst(2);
    TEST_ASSERT(skipped.GetSizeSequence().GetSize() == 3, "длина не 3");
    TEST_ASSERT(skipped.GetFirst() == 3, "первый после пропуска");
    LS empty = seq.SkipFirst(5);
    TEST_ASSERT(empty.GetSizeSequence().GetSize() == 0, "должна быть пустой");
    std::cout << "    OK" << std::endl;
    return true;
}

bool testGetSubsequence() {
    std::cout << "  Тест GetSubsequence..." << std::endl;
    LS seq({0, 1, 2, 3, 4, 5});
    LS sub = seq.GetSubsequence(2, 4);
    TEST_ASSERT(sub.GetSizeSequence().GetSize() == 3, "длина подпоследовательности");
    TEST_ASSERT(sub.GetFirst() == 2 && sub.GetLast() == 4, "неверные границы");
    try { seq.GetSubsequence(5, 2); TEST_ASSERT(false, "не выброшено исключение"); }
    catch (const InvalidArgumentException&) {}
    std::cout << "    OK" << std::endl;
    return true;
}

bool testMap() {
    std::cout << "  Тест Map..." << std::endl;
    LS seq({1, 2, 3});
    auto squares = seq.Map<int>([](int x) { return x * x; });
    TEST_ASSERT(squares.GetSizeSequence().GetSize() == 3, "длина не 3");
    TEST_ASSERT(squares.GetFirst() == 1 && squares.Get(Cardinal(2)) == 9, "неверно");
    std::cout << "    OK" << std::endl;
    return true;
}

bool testWhere() {
    std::cout << "  Тест Where..." << std::endl;
    LS seq({1, 2, 3, 4, 5, 6});
    auto evens = seq.Where([](int x) { return x % 2 == 0; });
    TEST_ASSERT(evens.Get(Cardinal(0)) == 2 && evens.Get(Cardinal(1)) == 4 && evens.Get(Cardinal(2)) == 6, "неверная фильтрация");
    try { evens.Get(Cardinal(3)); TEST_ASSERT(false, "не выброшено исключение"); }
    catch (const OutOfRangeException&) {}
    std::cout << "    OK" << std::endl;
    return true;
}

bool testReduce() {
    std::cout << "  Тест Reduce..." << std::endl;
    LS seq({1, 2, 3, 4});
    int sum = seq.Reduce<int>([](int a, int b) { return a + b; }, 0);
    TEST_ASSERT(sum == 10, "сумма не 10");
    int prod = seq.Reduce<int>([](int a, int b) { return a * b; }, 1);
    TEST_ASSERT(prod == 24, "произведение не 24");
    std::cout << "    OK" << std::endl;
    return true;
}

bool testZip() {
    std::cout << "  Тест Zip..." << std::endl;
    LS a({1, 2, 3});
    LS b({4, 5, 6});
    auto zipped = a.Zip(b);
    TEST_ASSERT(zipped.GetSizeSequence().GetSize() == 3, "длина не 3");
    auto p0 = zipped.Get(Cardinal(0));
    TEST_ASSERT(p0.first == 1 && p0.second == 4, "пара 0");
    auto p2 = zipped.Get(Cardinal(2));
    TEST_ASSERT(p2.first == 3 && p2.second == 6, "пара 2");
    std::cout << "    OK" << std::endl;
    return true;
}

bool testMemoization() {
    std::cout << "  Тест мемоизации..." << std::endl;
    int calls = 0;
    auto gen = [&calls](const MutableArraySequence<int>& prev) {
        ++calls;
        return prev.GetLast() + 1;
    };
    auto seq = LS::Recurrent(gen, MutableArraySequence<int>({0}));
    seq.Get(Cardinal(10));
    int afterFirst = calls;
    TEST_ASSERT(afterFirst == 10, "неправильное количество вызовов");
    seq.Get(Cardinal(10));
    TEST_ASSERT(calls == afterFirst, "генератор вызван повторно");
    seq.Get(Cardinal(11));
    TEST_ASSERT(calls == afterFirst + 1, "генератор не вызван для нового индекса");
    std::cout << "    OK" << std::endl;
    return true;
}

bool testLazySequence() {
    std::cout << "\n=== Тестирование LazySequence ===" << std::endl;
    auto tests = DynamicArray{
        testEmpty,
        testFiniteFromArray,
        testInfiniteNatural,
        testAppend,
        testPrepend,
        testInsertAt,
        testConcat,
        testSkipFirst,
        testGetSubsequence,
        testMap,
        testWhere,
        testReduce,
        testZip,
        testMemoization
    };
    return std::ranges::all_of(tests, [](auto f) { return f(); });
}

int main() {
    try {
        if (testLazySequence())
            std::cout << "\n=== Все тесты пройдены успешно ===" << std::endl;
        else
            std::cerr << "\n=== Тесты не пройдены ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Исключение: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}