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
Для считывания json файлов используется специальная, удобная в использовании рекурсивная структура Json::Document описанная в директории ```/JsonParser''' 

При решении задач об максимизации прибыли чаще всего используются либо динамические, либо жадные алгоритмы. Концепции этих алгоритмом схожи – получить оптимальные значения в зависимости от задачи. Динамический алгоритм разбивает задачу на малые подзадачи при чем решение одной подзадачи основано на выводе из предыдущих решенных задач(в нашем случае мы могли бы посчитать оптимальную расстановку в ангаре, для следующего дня основываясь на данных с прошлого дня посчитать расстановку для всех самолетов текущего дня). Жадный алгоритм исходит суммарно из текущей обстановки и делает вывод о том, стоит ли брать заказ или нет. В нашем случае величиной успеха является прибыль компании от ангаров, которая складывается из успешно проведенных ТО над самолетами. Грубо говоря один самолет приносит разную прибыль в день, находясь в разных ангарах, также следует учитывать время простоя самолета в ангаре, так как по контрактам у компании есть обязательства выполнить минимальное количество ТО. Имеется также и система штрафов, с помощью которой владелец компании может отменять заказы в пользу более прибыльных. И того получается, что при разработке решения требуется учитывать следующие параметры – общая прибыль от самолета, его время, местоположение в ангаре, номер ангара, минимальное кол-во заказов. Такой широкий спектр метрик делает потенциальную реализацию динамического алгоритма, крайне сложной, затратной и требующих определённых дополнительных исследований. В случае жадного алгоритма, мы потенциально можем учесть большую часть всех этих метрик и выбрать наилучший вариант на текущий момент. Кроме того, жадный алгоритм может быть оптимизирован таким образом, что будет возможно получать дополнительные «срочные» заказы и делать вывод исходя из потенциальной прибыли входящего заказа.

### Визуализация решения :pencil2:
Код находится в директории ```/Visualization```.

Визуализация осуществляется при помощи бесплатной кросплатформенной библиотеки ```SFML C++```.
На вход прогармме поступает набор из ангаров с самолётами: в каждый день известно, какие самолёты стоят в каком ангаре.
Данные отрисовываются в виде прямоугольников различных цветов с соответствующими надписями: название компании и самолёта.
