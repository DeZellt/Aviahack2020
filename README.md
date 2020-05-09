# Команда 48%
## AVIAHACKATON  :airplane:
Кейс: *S7 Technics*
### Проблема:
* Существует несколько ангаров, каждый определенной длины и ширины.
* Есть авиакомпании с самолётами, каждый из которых необходимо ремонитровать раз в какое-то время.
* Ремонт каждого самолёта занимает целое число дней.
* С каждой авиакомпанией заключён контракт, по которомы мы обязуемся отремонтировать какое-то число самолётов каждого типа. Если по каким-либо причинам мы ремонтируем меньше самолётов, либо не ремонтируем их вовсе, то выплачивается штраф.
* Если начали ремонтировать самолёт, то обязаны закончить дело, прерывать ремонт нельзя.
* Существуют сроки, в которые авиакомпании готовы отдать свой самолёт на ремонт, за эти сроки выходить нельзя.
### Задача:
Разработать программу, которая позволит максимизировать прибыль от ремонта самолётов с учётом штрафов за невыполнение контракта и времени ремонта.
## Этапы работы
### Парсинг гугл-таблицы в JSON :recycle:
Код парсинга находится в директории ```/ParsingXLSXtoJSON```.

Для парсинга xlsx в json используются библиотеки libxl и rapidjson. Благодаря libxl открываем файл table.xlsx и считываем информацию из всех заданных ячеек. Rapidjson - набор средств, которые позволяют легко сформировать json объект и получить читаемые и удобные списки данных представленных в table.xlsx. В итоге было сформировано 5 json файлов, которые будут использоваться в основном алгоритме программы.
### Обработка JSON, построение оптимальной стратегии :gear:
```put your code here```
### Визуализация решения :pencil2:
Код находится в директории ```/Visualization```.

Визуализация осуществляется при помощи бесплатной кросплатформенной библиотеки ```SFML C++```.
На вход прогармме поступает набор из ангаров с самолётами: в каждый день известно, какие самолёты стоят в каком ангаре.
Данные отрисовываются в виде прямоугольников различных цветов с соответствующими надписями: название компании и самолёта.
