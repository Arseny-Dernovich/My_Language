
# My_Language

Учебный компилятор небольшого императивного языка с футбольными ключевыми словами. Исходный текст превращается в токены, синтаксическое дерево, а затем — в ассемблер для стекового [Processor](https://github.com/Arseny-Dernovich/Processor).

## Сборка и запуск

Для отдельной сборки компилятора языка достаточно этого репозитория. Для объединённой цели `run_program` дополнительно нужно скачать [репозиторий процессора](https://github.com/Arseny-Dernovich/Processor) в соседнюю папку `Processor-reference`. Если репозиторий языка уже клонирован, выполни команду из папки, в которой лежит `My_Language`:

```sh
git clone https://github.com/Arseny-Dernovich/Processor.git Processor-reference
```

В итоге `My_Language` и `Processor-reference` должны лежать рядом. Для сборки через CMake нужны CMake 3.21+ и компилятор C++11 (например, `g++`). Из корня `My_Language` выполни:

```sh
cmake -S . -B build
cmake --build build
./build/read example.txt
```

Эти команды создают сборку в `build/`; объектные файлы и исполняемый файл не смешиваются с исходниками. Компилятор пишет `output.asm` и `tree.dot` в текущую папку, откуда он запущен.

В этом же CMake доступны ассемблер и эмулятор проекта `Processor-reference`. Чтобы скомпилировать пример языка, передать ассемблер процессора и сразу выполнить его:

```sh
cmake --build build --target run_program
```

Более полный тест `complex_test.txt` проверяет рекурсию Фибоначчи с двумя рекурсивными вызовами, рекурсивную функцию с двумя параметрами и цикл `while`. Запуск:

```sh
cmake -S . -B build -DLANGUAGE_PROGRAM="$PWD/complex_test.txt"
cmake --build build --target run_program
```

Ожидаемый вывод процессора: `21`, `1024`, `55`. Чтобы вернуться к стандартному рекурсивному примеру, повторно сконфигурируй `build` без параметра `LANGUAGE_PROGRAM` в новом build-каталоге либо укажи путь к `recursion_example.txt`.

По умолчанию запускается `recursion_example.txt`, и процессор печатает `120`. Промежуточные файлы находятся в `build/`: `output.asm` (с комментариями генератора), `processor_input.asm` (копия для ассемблера процессора), `output.txt` (машинный код в текстовом формате) и `output.bin`. Чтобы запустить другой файл, укажи его при конфигурации:

```sh
cmake -S . -B build -DLANGUAGE_PROGRAM="$PWD/example.txt"
cmake --build build --target run_program
```

Чтобы скомпилировать пример и автоматически получить изображение дерева через Graphviz, установи Graphviz и выполни:

```sh
cmake --build build --target example
```

Картинка появится в `build/tree.png`. На Ubuntu/Debian Graphviz устанавливается командой `sudo apt install graphviz`.

Старый Makefile тоже поддерживается. Для него нужны `g++` и `make`:

```sh
make
./read.exe example.txt
```

Если аргумент не указан, компилятор читает `my_programm.txt`. Результаты в текущем рабочем каталоге:

- `output.asm` — программа для Processor;
- `tree.dot` — синтаксическое дерево в формате Graphviz.

Чтобы получить картинку дерева, отдельно выполните `dot -Tpng tree.dot -o tree.png` при установленном Graphviz. Команда `./build/read ...` только генерирует ассемблер; для полной цепочки с запуском Processor используй цель `run_program`. Исходные примеры `my_programm.txt` и `factorial.txt` могут не соответствовать действующим проверкам областей видимости/точки входа; для первого запуска используйте пример ниже.

## Пример

```text
/:first_half
    start_game
        SSUUII 1 whistle
    end_game pass stadium_open n pressing 1 stadium_close

    SSUUII n assist transfer $Factorial$ stadium_open offside n foul 1 stadium_close whistle
/:second_half GOAT $Factorial$ stadium_open GOAT n stadium_close

/:first_half
    VAR transfer $Factorial$ stadium_open offside 5 stadium_close whistle
/:second_half GOAT $main$ stadium_open stadium_close
```

`$Factorial$` рекурсивно вычисляет факториал, а `$main$` печатает `120`. Тело функции в этом языке идёт **до** её заголовка — это важно при чтении грамматики.

Минимальный пример рекурсии также находится в `recursion_example.txt`. Его можно запустить из корня проекта:

```sh
./build/read recursion_example.txt
```

## Слова языка

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
| `VAR` | Вывести значение выражения |
| `SSUUII` | Вернуть значение из функции |
| `transfer` | Вызвать функцию |
| `offside` | Ввести очередной аргумент вызова |
| `whistle` | Закончить простую инструкцию |
| `/:first_half`, `/:second_half` | Начало и конец тела функции |

Имена переменных пишутся без `$`, например `result`; имена функций заключаются в доллары, например `$main$`. `shoot` распознаётся лексером, но ветка `else` пока не реализована.

## Грамматика

Ниже `*` означает повторение, `?` — необязательную часть, а `|` — выбор. Это описание принимаемых конструкций, а не формат входа генератора парсера.

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

Например, `GOAT x score 2 whistle`, затем `x score x goal 3 whistle`. У вызова с двумя аргументами синтаксис `transfer $sum$ stadium_open offside x offside 3 stadium_close`. В заголовке функции соответствующие параметры записываются `GOAT x GOAT y` без `offside`.

`assist` и `substitute` имеют больший приоритет, чем `goal` и `foul`. Сравнения применяются в `pass` и `dribble`, а не как самостоятельные арифметические выражения. Отрицательных числовых литералов и унарного минуса пока нет; отрицательное значение можно получить как `0 foul 1`.

## Области видимости и функции

- Глобальные переменные доступны из функций, если не перекрыты локальным объявлением.
- Каждый вызов функции имеет собственные значения параметров и локальных переменных. Рекурсивные и взаимно рекурсивные вызовы разрешены.
- Тело функции и каждый блок `pass`/`dribble` создают область видимости. Внутреннее объявление может скрыть внешнее; после блока его имя недоступно.
- Повторное объявление имени в одной области, обращение к неизвестной переменной/функции и неверное число аргументов вызывают ошибку компиляции.
- Точка входа — `$main$` без параметров. Если её нет, используется первая объявленная функция без параметров.
- `SSUUII` завершает текущий вызов и возвращает выражение; если выполнение дошло до конца тела, возвращается `0`.

Backend выделяет отдельные ячейки RAM для объявлений. Перед вызовом он сохраняет локальные ячейки вызывающей функции на стеке и восстанавливает их после возврата — поэтому рекурсия не портит локальные значения. Глобальные ячейки общие для всех вызовов. Используется до 10 000 ячеек RAM Processor.

### Как backend сохраняет кадр вызова

Это не классический кадр нативного C++ с `frame pointer`. Компилятор назначает параметрам и локальным переменным функции непрерывный диапазон ячеек RAM: `frame_start` — первая ячейка, `frame_size` — число ячеек. Все вызовы одной функции используют эти же адреса, поэтому при рекурсии новый вызов иначе затёр бы значения предыдущего.

В `Generate_Asm_Call_Func` backend выполняет для вызова следующие действия:

1. Если вызов выполняется из функции, кладёт все её ячейки кадра на стек процессора (`push [slot]`).
2. Вычисляет аргументы слева направо и кладёт их на стек.
3. Вызывает `func_<имя>`. В начале сгенерированной функции аргументы снимаются в ячейки параметров в обратном порядке.
4. После `ret` восстанавливает сохранённые ячейки вызывающей функции (`pop [slot]`, в обратном порядке).
5. Кладёт результат из регистра `ax` на стек выражений, чтобы вызов можно было использовать внутри арифметики.

Так каждый рекурсивный уровень получает обратно свои параметры и локальные значения после вложенного вызова. Глобальные переменные в этот кадр не входят и остаются общими. Для вызова из глобального кода `current == NULL`, поэтому сохранять кадр вызывающей функции не требуется.

## Устройство проекта

| Файл | Назначение |
| --- | --- |
| `Read.cpp`, `Read.h` | Таблица имён, лексер и общие структуры |
| `Syntax_Analis.cpp` | Рекурсивный спуск и построение AST |
| `Build_Table_Name_Func.cpp` | Проверка имён, вложенные области видимости, привязка переменных к ячейкам |
| `Simply.cpp` | Упрощение константных арифметических выражений |
| `Back_end.cpp`, `Back_end.h` | Генерация ассемблера, вызовы и возвраты функций |
| `DOT.cpp` | Экспорт AST в `tree.dot` |
| `Main.cpp` | Последовательность стадий компиляции |

Текущие ограничения: нет `else`, логических операторов и типов кроме целого числа; сообщения о синтаксических ошибках ещё не всегда содержат позицию во входном файле. Проект не очищает весь AST после компиляции, поэтому для длительного процесса потребуется доработать владение узлами дерева.
