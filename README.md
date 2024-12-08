<h1> Лабы по ООП за 5 семак</h1>

<h2> 1 лабораторка </h2>

3. Число-палиндром — сочетание цифр, имеющее одинаковое значение при
записи слева направо и справа налево. Например: 121, 55, 4884. Найдите все
такие числа-палиндромы от 10 до 100, что их кубы – также палиндромы.

<h2> 2 лабораторка </h2>

1. Написать рекурсивную функцию, которая принимает натуральное число n,
цифры которого стоят в порядке возрастания, и возвращает количество
пропущенных цифр в этом числе. Например, для числа 12248 пропущены
цифры 3, 5, 6, 7.

<h2> 3 лабораторка </h2>

<i> Указание. Класс должен быть описан в двух файлах с одинаковыми
именами, которые совпадают с названием класса, – первый файл должен
быть заголовочным с расширением .h, второй файл – с расширением .cpp.
Заголовочный файл должен содержать объявление класса, а файл .cpp –
реализацию методов класса и вспомогательные функции.
Конструкторы класса и методы при необходимости должны выполнять
проверку аргументов и выбрасывать исключение, если проверка не успешна.
Если для членов объекта выделяется динамическая память, то обязательно
должен присутствовать деструктор, освобождающий память. Кроме
указанных в задании функций при необходимости можно создавать
дополнительные методы и вспомогательные функции. Продумать какие из
методов должны иметь модификатор const. Для проверки корректности
работы функций написать несколько примеров их использования. </i>

4. Написать класс для представления матрицы вещественных чисел. Следует
реализовать:
1) Конструктор по умолчанию;
2) Конструкторы копирования и перемещения;
3) Конструктор, принимающий в качестве аргументов число строк, число
столбцов и значение для инициализации всех элементов матрицы;
4) Конструктор, принимающий в качестве аргумента значение типа
initializer_list<initializer_list<double>>;
4) Операторы присваивания копирования и перемещения;
5) Оператор направления в поток вывода;
6) Арифметические действия: +, –, *, +=, –=, *=. Арифметические действия
должны работать корректно как для матриц, так и для вещественных чисел.
7) Метод для получения элемента матрицы по его индексам.
8) Конструктор, принимающий в качестве аргумента initializer_list<double> и
создающий диагональную матрицу.
9) Операторы сравнения: ==, !=.

<h2> 4 лабораторка </h2>

много всего вот [ссылка](https://lms.surgu.ru/pluginfile.php/811009/mod_resource/content/1/ООП.%20Лабораторная%20работа%20№4.pdf)

<h2> 5 лабораторка </h2>

1. 1) Написать концепт container<T>, который накладывает на тип T
следующие ограничения:
a) тип T имеет асоциированный тип iterator, который удовлетворяет
концепту input_or_output_iterator;
b) тип T имеет функции члены begin и end, которые возвращают значение
типа iterator;
Например, этим условиям удовлетворяет стандартные классы vector и list.
2) Написать функцию-шаблон filter, которая принимает два аргумента
sequence и condition. Тип первого аргумента удовлетворяет концепту container;
второй аргумент может быть вызван как функция, возвращающая логическое
значение. На тип второго аргумента наложить при помощи концептов
соответствующие ограничения. Функция filter возвращает в качестве результата
vector, содержащий те элементы аргумента sequence, для которых condition
возвращается значение true, т.е. выполняет фильтрацию набора элементов по
заданному условию.
3) Написать примеры для проверки работы функции filter.
2. Написать концепт same_types, который принимает в качестве параметров
произвольное число типов (вариативный шаблон) и возвращает значение true,
если все указанные типы совпадают. Например,
cout << same_types<int> << endl; // true
cout << same_types<int, int, int> << endl; // true
cout << same_types<int, double, int> << endl; // false
Из стандартных концептов допускается использование лишь концепта same_as.
В качестве примера написать вариативный шаблон функции, который
использует концепт same_types.

<h1>Лабы по СИ обьекты и услуги ос</h1>

<h2> 5.12  </h2>

5.12. Напишите программу, которая принимает произвольное количество (не менее одного) аргументов командной строки и рассматривает их как имя и аргументы для запуска внешней программы; например, если ваша программа называется prog и пользователь запустил её командой
`./prog ls -1 -R`
то ваша программа должна запустить программу ls с аргументами -1 -R. Сделайте так, чтобы в зависимости от того, как запущенная программа завершится, ваша программа печатала либо слово exited и код завершения, либо слово killed и номер сигнала, которым запущенная программа была убита.

<h2> 5.13 </h2>

5.13. Напишите программу, которая через аргументы командной строки получает имена и аргументы для запуска внешних программ (произвольное их количество), разделённые параметром, состоящим из двух символов «;»; чтобы при запуске командный интерпретатор не считал параметр «;;» имеющим особый смысл, заключайте его в апострофы, например:
`./prog ls -1 / ';;' sleep 10 ';;' cat file1.txt file2.txt`
Ваша программа должна запустить на одновременное (параллельное) исполнение все указанные программы с заданными аргументами и напечатать имена тех из них, которые завершились успешно, то есть вызовом _exit с параметром 0. Печатать имена следует по мере завершения запущенных программ. Закончить работу следует сразу после завершения последней из запущенных программ, но не
раньше.

<h2> 5.18 </h2>

5.18. Напишите программу, которая принимает не менее двух аргументов командной строки и рассматривает первый аргумент - как имя файла, остальные - как имя и аргументы для запуска внешней программы; например, если ваша программа называется prog и пользователь запустил её командой
`./prog file1.txt 1s -1 -R`
то здесь имеется в виду файл file1.txt, внешняя программа ls и аргументы -1 -R. Запустите указанную внешнюю программу с указанными аргументами так, чтобы:
a) её стандартный вывод был перенаправлен в указанный файл (если такого файла нет, он должен быть создан, если он уже есть - перезаписан; если открыть файл не удалось, должна быть выдана диагностика, соответствующая возникшей ситуации);
b) её стандартный ввод шёл из указанного файла (если такого файла нет, должна быть зафиксирована ошибка и выдана соответствующая диагностика);
c) стандартный вывод был перенаправлен в указанный файл на добавление в конец (если файла нет, создайте его, но если он есть, новое содержимое должно быть добавлено к старому);
d) стандартный вывод был перенаправлен в указанный файл на добавление в конец, причём если такого файла нет, должна быть зафиксирована ошибка.

<h2> 5.20 </h2>

5.20. Напишите программу, которая после запуска выдаёт сообщение «Press Ctrl-C to quit» и после этого ничего не делает, но и не завершается; при нажатии Ctrl-С выдаёт сообщение «Good bye» и завершается. Обязательно убедитесь с помощью программы top, что ваша программа во время своего «ничегонеделания» не потребляет процессорное время.

<h2> 5.26 </h2>

5.26. Напишите программу, которая через аргументы командной строки получает имена и аргументы для запуска двух внешних программ, разделённые параметром, состоящим из двух символов «;», как в задаче 5.13, и запускает эти программы на одновременное (параллельное) выполнение, связав их конвейером, т. е. стандартный вывод первой программы должен идти на стандартный ввод второй программы.
