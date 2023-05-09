# Message Coder

Данный модуль представляет собой статическую библиотеку для программ, где необходима сериализация данных для передачи по последовательным интерфейсам (основное применение в Embedded Engineering).  
Модуль принимает на вход буфер определенного размера, который после кодирования превращается в закодированный поток байтов для передачи в последовательный интерфейс. Во-первых, к буферу добавляются символы начала и конца посылки. Во-вторых, любые совпадения данных со значением символов начала, конца посылки и спецсимвола начала закодированной последовательности подвергаются кодированию, что также увеличивает размер выходного потока данных. Наихудший случай кодирования данных размером N - это когда каждый байт в данных необходимо кодировать, т.е. размер удваивается + старт- и стоп-символы. Таким образом, минимальный размер выходного буфера данных: 1 + N + 1 = N + 2 байт. А максимальный размер выходного буфера данных: 1 + 2 * N + 1 = 2N + 2 байт.

### Требования
- glibc
- gcc
- make
- cmake
- doxygen (для генерации документации)
- texlive-latex-base (для генерации документации)
- graphviz (для генерации документации)

### Сборка библиотеки
Открыть командную оболочку (shell) и выполнить указанные команды:  
```bash
git clone git@github.com:VasiliyMatlab/MessageCoder.git
cd MessageCoder/
cmake -B build
cd build/
make
make install
```

### Использование
В результате сборки будет создана директория `bin`, где будет лежать статическая библиотека `libmesscoder.a`. Данную статическую библиотеку можно скопировать в свой проект и добавить флаг при линковке: `-lmesscoder`.

### Пример
Проект также содержит пример по работе с библиотекой (клиент-серверное приложение). Для сборки примера открыть командную оболочку (shell) и выполнить указанные команды:  
```bash
cmake -B build -DEXAMPLE=ON
cd build/
make
make install
```
В результате сборки будет создана директория `bin`, где будет лежать статическая библиотека `libmesscoder.a`, а также исполняемые файлы приложений: `server.elf` и `client.elf`. Сначала запускается сервер, после чего - клиент. На экране можно будет пронаблюдать процесс передачи посылок, которые принимаются клиентом. В нем происходит поиск сообщений и декодирование.

### Python
В проекте также имеется директория `python`, где находится скрипт `messcoder.py`, который может быть использован в качестве импортируемого модуля в проектах на языке Python (> 3.11.0).

### Документация
Для сборки документации открыть командную оболочку (shell) и выполнить указанные команды:  
```bash
cmake -B build -DDOC=ON -DLIB=OFF
cd build/
make documentation
```
В результате сборки появится директория `doc`, где в поддиректории `latex` будет лежать `refman.pdf` - файл с документацией.

### ToDo list:
- сделать возможность кросскомпиляции под другие архитектуры

***
<p align="center"><a href="https://github.com/VasiliyMatlab"><img src="https://github.com/VasiliyMatlab.png" width="100" alt="VasiliyMatlab" /></a></p>
<p align="center"><a href="https://github.com/VasiliyMatlab" style="color: #000000">VasiliyMatlab</a></p>
