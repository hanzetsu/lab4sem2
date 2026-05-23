#include "LazySequence.hpp"
#include "MutableArraySequence.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <vector>

using LS = LazySequence<int, MutableArraySequence>;

void printSequence(const LS &seq, size_t maxCount = 20)
{
    std::cout << "Текущая последовательность: [";
    size_t count = 0;
    for (size_t i = 0; i < maxCount; ++i)
    {
        try
        {
            int val = const_cast<LS &>(seq).Get(Cardinal(i));
            if (count > 0)
                std::cout << ", ";
            std::cout << val;
            ++count;
        }
        catch (const OutOfRangeException &)
        {
            break;
        }
    }
    if (seq.GetSizeSequence().IsInfiniteNumber() && count == maxCount)
    {
        std::cout << ", ...";
    }
    std::cout << "]" << std::endl;
    std::cout << "Длина: ";
    if (seq.GetSizeSequence().IsInfiniteNumber())
    {
        std::cout << "бесконечна";
    }
    else
    {
        std::cout << seq.GetSizeSequence().GetSize();
    }
    std::cout << ", материализовано: " << seq.GetSizeCache() << std::endl;
}

void printMenu()
{
    std::cout << "\n--- Ручное тестирование LazySequence ---\n";
    std::cout << "1. Создать конечную последовательность из списка\n";
    std::cout << "2. Создать бесконечную (натуральные числа)\n";
    std::cout << "3. Показать первые N элементов\n";
    std::cout << "4. Получить элемент по индексу\n";
    std::cout << "5. Append (добавить в конец)\n";
    std::cout << "6. Prepend (добавить в начало)\n";
    std::cout << "7. InsertAt (вставить по индексу)\n";
    std::cout << "8. Concat (конкатенация с [100,200,300])\n";
    std::cout << "9. SkipFirst (пропустить N элементов)\n";
    std::cout << "10. GetSubsequence (извлечь подпоследовательность)\n";
    std::cout << "11. Map (квадрат)\n";
    std::cout << "12. Where (чётные)\n";
    std::cout << "13. Reduce (сумма)\n";
    std::cout << "14. Выход\n";
}

std::vector<int> readInts(const std::string &prompt)
{
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    std::vector<int> res;
    int x;
    while (iss >> x)
        res.push_back(x);
    return res;
}

int main()
{
    std::shared_ptr<LS> current = std::make_shared<LS>();
    int choice;
    do
    {
        printMenu();
        std::cout << "Ваш выбор: ";
        std::cin >> choice;
        std::cin.ignore();
        try
        {
            switch (choice)
            {
            case 1:
            {
                auto vals = readInts("Введите числа через пробел: ");
                MutableArraySequence<int> arr(vals.data(), vals.size());
                current = std::make_shared<LS>(arr);
                std::cout << "Последовательность создана.\n";
                printSequence(*current);
                break;
            }
            case 2:
            {
                auto naturals = LS::Recurrent(
                    [](const MutableArraySequence<int> &prev)
                    { return prev.GetLast() + 1; },
                    MutableArraySequence<int>({0}));
                current = std::make_shared<LS>(naturals);
                std::cout << "Создана бесконечная последовательность натуральных чисел.\n";
                printSequence(*current);
                break;
            }
            case 3:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                size_t n;
                std::cout << "Сколько элементов показать? ";
                std::cin >> n;
                std::cin.ignore();
                std::cout << "Элементы: ";
                for (size_t i = 0; i < n; ++i)
                {
                    try
                    {
                        std::cout << current->Get(Cardinal(i)) << " ";
                    }
                    catch (const OutOfRangeException &)
                    {
                        break;
                    }
                }
                std::cout << std::endl;
                break;
            }
            case 4:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                size_t idx;
                std::cout << "Введите индекс: ";
                std::cin >> idx;
                std::cin.ignore();
                try
                {
                    std::cout << "Элемент [" << idx << "] = " << current->Get(Cardinal(idx)) << std::endl;
                }
                catch (const OutOfRangeException &e)
                {
                    std::cout << "Ошибка: " << e.what() << std::endl;
                }
                break;
            }
            case 5:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                int val;
                std::cout << "Введите значение: ";
                std::cin >> val;
                std::cin.ignore();
                current = std::make_shared<LS>(current->Append(val));
                std::cout << "Элемент добавлен.\n";
                printSequence(*current);
                break;
            }
            case 6:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                int val;
                std::cout << "Введите значение: ";
                std::cin >> val;
                std::cin.ignore();
                current = std::make_shared<LS>(current->Prepend(val));
                std::cout << "Элемент добавлен в начало.\n";
                printSequence(*current);
                break;
            }
            case 7:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                int val;
                size_t idx;
                std::cout << "Введите индекс и значение через пробел: ";
                std::cin >> idx >> val;
                std::cin.ignore();
                current = std::make_shared<LS>(current->InsertAt(val, idx));
                std::cout << "Вставка выполнена.\n";
                printSequence(*current);
                break;
            }
            case 8:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                MutableArraySequence<int> arr2({100, 200, 300});
                LS other(arr2);
                current = std::make_shared<LS>(current->Concat(other));
                std::cout << "Конкатенация выполнена.\n";
                printSequence(*current);
                break;
            }
            case 9:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                size_t count;
                std::cout << "Сколько элементов пропустить? ";
                std::cin >> count;
                std::cin.ignore();
                current = std::make_shared<LS>(current->SkipFirst(count));
                std::cout << "Пропуск выполнен.\n";
                printSequence(*current);
                break;
            }
            case 10:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                size_t start, end;
                std::cout << "Введите start и end (через пробел): ";
                std::cin >> start >> end;
                std::cin.ignore();
                current = std::make_shared<LS>(current->GetSubsequence(start, end));
                std::cout << "Подпоследовательность создана.\n";
                printSequence(*current);
                break;
            }
            case 11:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                current = std::make_shared<LS>(current->Map<int>([](int x)
                                                                 { return x * x; }));
                std::cout << "Map (квадрат) применён.\n";
                printSequence(*current);
                break;
            }
            case 12:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                current = std::make_shared<LS>(current->Where([](int x)
                                                              { return x % 2 == 0; }));
                std::cout << "Фильтрация (чётные) применена.\n";
                printSequence(*current);
                break;
            }
            case 13:
            {
                if (!current)
                {
                    std::cout << "Нет последовательности.\n";
                    break;
                }
                if (current->GetSizeSequence().IsInfiniteNumber())
                {
                    std::cout << "Reduce на бесконечной последовательности невозможен.\n";
                    break;
                }
                std::cin.ignore();
                int sum = current->Reduce<int>([](int a, int b)
                                               { return a + b; }, 0);
                std::cout << "Сумма = " << sum << std::endl;
                printSequence(*current);
                break;
            }
            case 14:
                std::cout << "Выход.\n";
                break;
            default:
                std::cout << "Неверный выбор.\n";
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "Ошибка: " << e.what() << std::endl;
        }
    } while (choice != 14);
    return 0;
}