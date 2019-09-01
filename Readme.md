

* Репозитарий на [github.com](https://github.com/mambaru/wfc_jsonrpc).
* Документация [doxygen](https://mambaru.github.io/wfc_jsonrpc/index.html).
* Отчет [coverage](https://mambaru.github.io/wfc_jsonrpc/cov-report/index.html)

# wfc_jsonrpc: Пает модулей для работы с JSON-RPC 2.0

[JSONRPC-QUEUE](#JSONRPC-QUEUE) - 
[JSONRPC-BROKER](#JSONRPC-BROKER) 
[JSONRPC-REPLI](#JSONRPC-REPLI) -
[JSONRPC-HUB](#JSONRPC-HUB) -
[JSONRPC-BATCH](#JSONRPC-BATCH) -
[JSONRPC-BACKLOG](#JSONRPC-BACKLOG) -
[JSONRPC-STATISTICS](#JSONRPC-STATISTICS) - 


Отправляет входящие запросы в очередь "workflow", а исходящие в "callback_workflow". Если **workflow** не заданны, то входящие
отправляются напрямую в "target" без очереди. Если указан **tracking**==true то включается режим отслеживания входящих подключений, 
в котором сообщение из очереди выкидывается, если клиент отключился. Прочие ограничения на очередь (максимальный размер, управление
управлению потоками) настраиваются в указанном **workflow**. Если цель не задана, то возвращает JSONRPC ошибку -32003 "Service Unavailable".
Если не задан "workflow", то используется общий **workflow**. На высоких нагрузках всегда рекомендуется настроить отдельный workflow 
для очереди.

{
  "jsonrpc-queue": [
    {
      "name": "jsonrpc-queue1",
      "enabled": true,
      /* В режиме suspend сообщения передаются target напрямую */
      "suspend": false,
      /* Включить режим отслеживания входящих подключений */
      "tracking": false,
      /* Для входящих сообщений */
      "workflow": "",
      /* Цель */
      "target": "",
      /* Включить асинхронную обработку исходящих сообщений */
      "callback_queue": false,
      /* Для исходящих сообщений */
      "callback_workflow": ""
    }
  ]
}

```
┌─────────────┐
│ server-tcp  ├─┐
└─────────────┘ │     ┌───────────────┐    ┌──────────┐   ┌────────┐
                ├─────┤ jsonrpc-queue ├────┤ service  ├───┤ domain │
┌─────────────┐ │     └───────────────┘    └──────────┘   └────────┘
│ server-udp  ├─┘   
└─────────────┘
```


{
  "jsonrpc-queue": [
    {
      "name": "jsonrpc-queue1",
      "enabled": true,
      "suspend": false,
      "tracking": false,
      "startup_priority": 0,
      "shutdown_priority": 0,
      "workflow": "",
      "target": "",
      "callback_queue": false,
      "callback_workflow": ""
    }
  ]
}

## JSONRPC-BROKER

Позволяет управлять входящим потоком сообщений jsonrpc-сообщений распределяя их по целям в соответствии с заданными правилами. Например можно вызов конкретного 
метода отправить в отдельную очередь или же запретить его. Правило можно задать для нескольких методов с учетом входящих параметров запроса в том числе и с 
использованием регулярных выражений. Работает только со входящим потоком (запросы и уведомления).

```cpp
{
  "name": "jsonrpc-broker1",
  "enabled": true,

  /* В режиме suspend отправляет все запросы без проверки в 'target' */
  "suspend": false,

  /* Цель по умолчанию (имя объекта), если сообщение не подошло */
  "target": "<<target-name>>",

  /* Список запрещенных методов (клиент получает "Procedure not found") */
  "reject": ["<<method-name-1>>", "<<method-name-2>>"],

  /**/
  std::string target_log;
  /**/
  std::string reject_log;

  /* Список правил */
  "rules": [
    {/*правило1*/},
    {/*правило2*/}
  ]
}
```
* *target* - имя объекта, которому будет передан запрос, который не подошел ни к одному из правил. Может быть пустым, в этом случае в ответ отсылается стандартная для WFC JSONRPC-ошибка, что сервис не доступен
* *reject* - все вызовы методов из этого списка блокируются, правила не проверяются, клиент получает JSONRPC-ошибку, что процедура не найдена. 
* *rules* - список правил (см. следующий раздел)

### Правила для методов

Правила применяются последовательно, в тоже последовательности, как и задано в конфигурации, до тех пор пока не выполнится 
```cpp
/*правило*/
{
  /* Список методов */
  "methods": ["<<method-name-1>>", "<<method-name-2>>"],
  /* Цель для этих методов */
  "target": "<<target-name>>"
}
```

### Правила для параметров

```cpp
 {
  /* Список методов */
  "methods": ["<<method-name-1>>", "<<method-name-2>>"],
  /* Цель для этих методов */
  "target": "<<target-name>>"
  /* Режим сравнения для имен полей и их значений */
  "params_mode": "FullMatchName|PrefixMatchValue",
  /* Список полей запроса */
  "params": {
    "arg1": "value1",
    "arg2": "value2"
  }
}
```

┌─────────────┐                        ┌───────────────┐
│ server-tcp  ├─┐                    ┌─┤ jsonrpc-queue ├─┐
└─────────────┘ │ ┌────────────────┐ │ └───────────────┘ │  ┌─────────┐   ┌────────┐
                ├─┤ jsonrpc-broker ├─┤                   ├──┤ service ├───┤ domain │
┌─────────────┐ │ └────────────────┘ │ ┌───────────────┐ │  └─────────┘   └────────┘
│ server-udp  ├─┘                    └─┤ jsonrpc-queue ├─┘
└─────────────┘                        └───────────────┘
                               

## JSONRPC-REPLI
Дублирует входящие запросы и уведомления по целям перечисленных в "reply_targets", при этом уведомления копируются как есть, 
а запросы преобразуются в уведомления. 

```
{
  "jsonrpc-repli": [
    {
      "name": "jsonrpc-repli1",
      "enabled": true,
      "suspend": false,
      "target": "",
      "reply_targets": []
    }
  ]
}
```

┌─────────────┐                       ┌───────────────┐  ┌─────────┐   ┌────────┐
│ server-tcp  ├─┐                   ┌─┤ jsonrpc-queue ├──┤ service ├───┤ domain │
└─────────────┘ │ ┌───────────────┐ │ └───────────────┘  └─────────┘   └────────┘
                ├─┤ jsonrpc-repli ├─┤                   
┌─────────────┐ │ └───────────────┘ │ ┌────────────┐ 
│ server-udp  ├─┘                   └─┤ client-tcp │
└─────────────┘                       └────────────┘


## JSONRPC-HUB
Дублирует входящие запросы и уведомления по всем зарегистрированным объектам (которые вызвали reg_io) 

{
  "jsonrpc-hub": [
    {
      "name": "jsonrpc-hub1",
      "enabled": true,
      "suspend": false,
      "startup_priority": 0,
      "shutdown_priority": 0,
      "target": ""
    }
  ]
}

```
┌─────────────┐
│ server-tcp  ├─┐
└─────────────┘ │     ┌─────────────┐    ┌──────────┐   ┌────────┐
                ├─────┤ jsonrpc-hub ├────┤ service  ├───┤ domain │
┌─────────────┐ │     └──────┬──────┘    └──────────┘   └────────┘
│ server-udp  ├─┘            │
└─────────────┘              │
                             │
┌─────────────┐              │
│ server-tcp  ├──────────────┘
└─────────────┘
      ^
      |
   telnet
```

## JSONRPC-BATCH

Включает поддержку JSONRPC пакетов (batch https://www.jsonrpc.org/specification#batch). Этот объект должен быть настроен 
первым в цепочке обработке jsonrpc (другие компоненты batch не поддерживают). Этот объект разбивает входящий массив на 
JSONRPC сообщения и отправляет их в "target", далее запросы могут обрабатываться как синхронно так и асинхронно, в любом порядке.
Как только все ответы на запросы из массива будут собраны, будет сформирован и отправлен массив ответов. 

```
{
  "jsonrpc-batch": [
    {
      "name": "jsonrpc-batch1",
      "enabled": true,
      "suspend": false,
      "target": ""
    }
  ]
}
```

## JSONRPC-BACKLOG

Записывает входящий поток в файл или лог. Запись в файл ("path") производиться для последующего восстановления с помощью объектов 
которые умеют это делать (см. документацию этих объектов). Если вам нужен дамп потока, то пишите его в лог "log", который настройте 
в модуле logger (там можно настроить формат вывода и режим ротации). Сам по себе **jsonrpc-backlog** восстановится при запуске не 
может, нужен специальный запрос от прикладного объекта, который может с этим работать. После этого запроса, все записанные сообщения 
отправляются "restore_trace". Как правильно настроить этот объект для восстановления прикладного объекта из бэклога читайте в 
документации этого объекта. 

{
  "jsonrpc-backlog": [
    {
      "name": "jsonrpc-backlog1",
      "enabled": true,
      "suspend": false,
      "target": "",
      /* Цель для восстановления */ 
      "restore_target": "",
      /* Путь файла backlog для записи jsonrpc-сообщений */
      "path": "",
      /* Интенсивность логирования восстановления (каждые restore_trace 
         записей, 0 - без логирования, 1 - на каждую запись, 2 - на каждую 
         вторую запись и т.д. ) */
      "restore_trace": 0,
      /* Имя лога (не файла) для записи jsonrpc-сообщений */
      "log": ""
    }
  ]
}

## JSONRPC-STATISTICS

{
  "jsonrpc-statistics": [
    {
      "name": "jsonrpc-statistics1",
      "enabled": true,
      "suspend": false,
      "statistics": {
        "disabled": true,
        "target": "",
        "time_suffix": ".time",
        "read_size_suffix": ".rsize",
        "write_size_suffix": ".wsize",
        "request_prefix": "req:",
        "notify_prefix": "ntf:",
        "other_time": "other.time",
        "other_read_size": "other.rsize",
        "other_write_size": "other.wsize"
      },
      "target": "",
      "enable_write_size": true,
      "enable_error_stat": true
    }
  ]
}
