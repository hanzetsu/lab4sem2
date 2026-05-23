#include "ArrayStream.hpp"
#include "StringStream.hpp"
#include "FileStream.hpp"
#include "StreamAlgorithms.hpp"
#include "MutableArraySequence.hpp"
#include "test_common.hpp"
#include "DynamicArray.hpp"
#include <ranges>
#include <iostream>

using namespace std;

bool testArrayStream() {
    cout << "  Проверка ArrayStream" << endl;
    int arr[] = {10, 20, 30};
    MutableArraySequence<int> seq(arr, 3);
    auto stream = make_shared<ArrayStream<int>>(seq);
    TEST_ASSERT(!stream->IsEnd(), "Поток не должен быть пустым");
    TEST_ASSERT(stream->Read() == 10, "Первый элемент");
    TEST_ASSERT(stream->Read() == 20, "Второй элемент");
    TEST_ASSERT(stream->Read() == 30, "Третий элемент");
    TEST_ASSERT(stream->IsEnd(), "После третьего должен быть конец");
    try { stream->Read(); TEST_ASSERT(false, "Исключение не выброшено"); }
    catch (const OutOfRangeException&) {}
    cout << "    OK" << endl;
    return true;
}

bool testStringStream() {
    cout << "  Проверка StringStream" << endl;
    auto stream = make_shared<StringStream>("abc");
    TEST_ASSERT(!stream->IsEnd(), "Не пустой");
    TEST_ASSERT(stream->Read() == 'a', "Первый символ");
    TEST_ASSERT(stream->Read() == 'b', "Второй");
    TEST_ASSERT(stream->Read() == 'c', "Третий");
    TEST_ASSERT(stream->IsEnd(), "Конец");
    cout << "    OK" << endl;
    return true;
}

bool testFilterStream() {
    cout << "  Проверка FilterStream" << endl;
    int arr[] = {1,2,3,4,5,6};
    MutableArraySequence<int> seq(arr, 6);
    auto source = make_shared<ArrayStream<int>>(seq);
    auto evenStream = make_shared<FilterStream<int>>(source, [](int x) { return x % 2 == 0; });
    TEST_ASSERT(evenStream->Read() == 2, "Первый чётный");
    TEST_ASSERT(evenStream->Read() == 4, "Второй чётный");
    TEST_ASSERT(evenStream->Read() == 6, "Третий чётный");
    TEST_ASSERT(evenStream->IsEnd(), "Конец");
    cout << "    OK" << endl;
    return true;
}

bool testMapStream() {
    cout << "  Проверка MapStream" << endl;
    int arr[] = {1,2,3};
    MutableArraySequence<int> seq(arr, 3);
    auto source = make_shared<ArrayStream<int>>(seq);
    auto squared = make_shared<MapStream<int,int>>(source, [](int x) { return x * x; });
    TEST_ASSERT(squared->Read() == 1, "1^2");
    TEST_ASSERT(squared->Read() == 4, "2^2");
    TEST_ASSERT(squared->Read() == 9, "3^2");
    TEST_ASSERT(squared->IsEnd(), "Конец");
    cout << "    OK" << endl;
    return true;
}

bool testCollectToSequence() {
    cout << "  Проверка CollectToSequence" << endl;
    int arr[] = {5,6,7};
    MutableArraySequence<int> seq(arr, 3);
    auto stream = make_shared<ArrayStream<int>>(seq);
    auto collected = CollectToSequence<int>(stream);
    TEST_ASSERT(collected->GetLength() == 3, "Длина 3");
    TEST_ASSERT(collected->Get(0) == 5 && collected->Get(2) == 7, "Элементы");
    cout << "    OK" << endl;
    return true;
}

bool testStream() {
    cout << "\n=== Тестирование Stream ===" << endl;
    auto tests = DynamicArray{
        testArrayStream,
        testStringStream,
        testFilterStream,
        testMapStream,
        testCollectToSequence
    };
    return ranges::all_of(tests, [](auto f) { return f(); });
}

int main() {
    try {
        if (testStream())
            cout << "\n=== Все тесты потоков пройдены ===" << endl;
        else
            cerr << "\n=== Тесты потоков не пройдены ===" << endl;
    } catch (const std::exception& e) {
        cerr << "Исключение: " << e.what() << endl;
        return 1;
    }
    return 0;
}