# My_Language


## Сборка и запуск

Для отдельной сборки компилятора языка достаточно этого репозитория. Для объединённой цели `run_program` дополнительно нужен [проект процессора](https://github.com/Arseny-Dernovich/Processor), клонированный в соседнюю папку `Processor-reference`. Если репозиторий языка уже клонирован, выполни из родительской папки:

```sh
git clone https://github.com/Arseny-Dernovich/Processor.git Processor-reference
```

Репозитории `My_Language` и `Processor-reference` должны лежать рядом. Из корня `My_Language` собери проект:

```sh
cmake -S . -B build
cmake --build build
```

Чтобы скомпилировать исходный файл, передать его ассемблеру Processor и запустить на эмуляторе, используй цель:

```sh
cmake --build build --target run_program
```

По умолчанию запускается `recursion_example.txt`, процессор выводит `120`. Промежуточные файлы находятся в `build/`: `output.asm` (ассемблер с комментариями), `processor_input.asm` (копия для ассемблера процессора), `output.txt` (машинный код) и `output.bin`.

Чтобы выбрать программу, например `complex_test.txt`, повторно сконфигурируй CMake:

```sh
cmake -S . -B build -DLANGUAGE_PROGRAM="$PWD/complex_test.txt"
cmake --build build --target run_program
```

Этот тест проверяет Фибоначчи с двумя рекурсивными вызовами, рекурсивную функцию с двумя параметрами и цикл `while`. Ожидаемый вывод: `21`, `1024`, `55`.

Чтобы получить изображение дерева Graphviz, установи Graphviz и выполни:

```sh
cmake --build build --target example
```

Картинка появится в `build/tree.png`. На Ubuntu/Debian Graphviz устанавливается командой `sudo apt install graphviz`.

Старый Makefile также поддерживается (нужны `g++` и `make`):

```sh
make
./read.exe example.txt
```

## Пример программы

```text
/:first_half
    start_game
        SSUUII 1 whistle
    end_game pass stadium_open n pressing 1 stadium_close

    SSUUII n assist transfer $factorial$ stadium_open offside n foul 1 stadium_close whistle
/:second_half GOAT $factorial$ stadium_open GOAT n stadium_close

/:first_half
    VAR transfer $factorial$ stadium_open offside 5 stadium_close whistle
/:second_half GOAT $main$ stadium_open stadium_close
```

Функция `$factorial$` рекурсивно вычисляет факториал, а `$main$` печатает `120`. Тело функции записывается **до** её заголовка. Готовые примеры: `recursion_example.txt` и более сложный `complex_test.txt`.

## Ключевые слова и операторы

| Запись | Смысл |
| --- | --- |
| `GOAT` | Объявить целочисленную переменную или параметр |
| `score` | Присваивание `=` |
| `goal`, `foul` | Сложение и вычитание |
| `assist`, `substitute` | Умножение и целочисленное деление |
| `leader`, `underdog` | `>`, `<` |
| `advantage`, `pressing` | `>=`, `<=` |
| `draw`, `gap` | `==`, `!=` |
| `stadium_open`, `stadium_close` | Круглые скобки |
| `start_game`, `end_game` | Границы блока `if`/`while` |
| `pass`, `dribble` | Условие `if`, цикл `while` |
| `VAR` | Вывести выражение |
| `SSUUII` | Вернуть значение из функции |
| `transfer` | Вызов функции |
| `offside` | Ввести очередной аргумент вызова |
| `whistle` | Закончить простую инструкцию |
| `/:first_half`, `/:second_half` | Начало и конец тела функции |

Имена переменных пишутся без `$`, имена функций — в долларах, например `$main$`. Слово `shoot` распознаётся лексером, но ветка `else` пока не реализована.

## Грамматика

Ниже `*` означает повторение, `?` — необязательную часть, `|` — выбор.

```ebnf
program       = { global_decl | function } ;
global_decl   = declaration, "whistle" ;
function      = "/:first_half", statements, "/:second_half",
                "GOAT", function_name, "stadium_open",
                { "GOAT", identifier }, "stadium_close" ;
statements    = statement, { statement } ;
statement     = declaration, "whistle"
              | identifier, "score", expression, "whistle"
              | "VAR", expression, "whistle"
              | "SSUUII", expression, "whistle"
              | call, "whistle"
              | block, ( "pass" | "dribble" ), condition ;
declaration   = "GOAT", identifier, [ "score", expression ] ;
block         = "start_game", statements, "end_game" ;
condition     = "stadium_open", expression, comparison,
                expression, "stadium_close" ;
comparison    = "leader" | "underdog" | "advantage"
              | "pressing" | "draw" | "gap" ;
expression    = term, { ( "goal" | "foul" ), term } ;
term          = primary, { ( "assist" | "substitute" ), primary } ;
primary       = integer | identifier
              | "stadium_open", expression, "stadium_close"
              | call ;
call          = "transfer", function_name, "stadium_open",
                { "offside", expression }, "stadium_close" ;
```

Например: `GOAT x score 2 whistle`, затем `x score x goal 3 whistle`. Вызов с двумя аргументами: `transfer $sum$ stadium_open offside x offside 3 stadium_close`. В заголовке функции параметры перечисляются через `GOAT`.

Операции `assist` и `substitute` имеют больший приоритет, чем `goal` и `foul`. Сравнения применяются в `pass` и `dribble`. Отрицательных числовых литералов и унарного минуса пока нет; отрицательное значение можно получить выражением `0 foul 1`.

## Области видимости, функции и рекурсия

- Глобальные переменные доступны из функций, если не перекрыты локальным объявлением.
- Параметры и локальные переменные каждого вызова изолированы; рекурсивные и взаимно рекурсивные вызовы поддерживаются.
- Тело функции и каждый блок `pass`/`dribble` создают область видимости. Вложенное объявление может скрыть внешнее имя.
- Повторное объявление имени в одной области, неизвестное имя и неверное число аргументов приводят к ошибке компиляции.
- Точка входа — функция `$main$` без параметров; если её нет, используется первая функция без параметров.
- `SSUUII` завершает текущий вызов и возвращает выражение; если выполнение дошло до конца тела, возвращается `0`.

Backend назначает параметрам и локальным переменным функции диапазон ячеек RAM. Перед вложенным вызовом текущие ячейки кадра сохраняются на стеке процессора и восстанавливаются после возврата. Результат возвращается через регистр `ax`. Поэтому рекурсия не затирает локальные значения предыдущего вызова. Глобальные ячейки общие; Processor предоставляет 10 000 ячеек RAM.

## Файлы проекта

| Файл | Назначение |
| --- | --- |
| `Read.cpp`, `Read.h` | Таблица имён, лексер и общие структуры |
| `Syntax_Analis.cpp` | Рекурсивный спуск и построение AST |
| `Build_Table_Name_Func.cpp` | Проверка имён и областей видимости |
| `Simply.cpp` | Свёртка констант и упрощение AST |
| `Back_end.cpp`, `Back_end.h` | Генерация ассемблера, вызовы и возвраты |
| `DOT.cpp` | Экспорт дерева в Graphviz DOT |
| `Main.cpp` | Последовательность стадий компиляции |
| `Processor-reference/Comp.h` | Интерфейс сборки ассемблера процессора |

## Ограничения

Нет `else`, логических операторов и типов кроме целого числа; унарный минус и вещественные литералы не поддерживаются. Сообщения о синтаксических ошибках не всегда указывают позицию во входном файле.
