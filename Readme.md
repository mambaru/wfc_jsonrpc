[JSONRPC-QUEUE](#JSONRPC-QUEUE) - 
[JSONRPC-BROKER](#JSONRPC-BROKER) - брокер JSONRPC-сообщений по заданному набору правил распределяет их по целям
[JSONRPC-REPLI](#JSONRPC-REPLI) -
[JSONRPC-HUB](#JSONRPC-HUB) -
[JSONRPC-BATCH](#JSONRPC-BATCH) -
[JSONRPC-BACKLOG](#JSONRPC-BACKLOG) -
[JSONRPC-STATISTICS](#JSONRPC-STATISTICS) - 




# JSONRPC-QUEUE
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

# JSONRPC-BROKER

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

## Правила для методов

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

## Правила для параметров

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

# JSONRPC-REPLI
{
  "jsonrpc-repli": [
    {
      "name": "jsonrpc-repli1",
      "target": "",
      "reply_targets": []
    }
  ]
}

# JSONRPC-HUB
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

# JSONRPC-BATCH

{
  "jsonrpc-batch": [
    {
      "name": "jsonrpc-batch1",
      "enabled": true,
      "suspend": false,
      "startup_priority": 0,
      "shutdown_priority": 0,
      "workflow": "",
      "target": ""
    }
  ]
}

# JSONRPC-BACKLOG
{
  "jsonrpc-backlog": [
    {
      "name": "jsonrpc-backlog1",
      "enabled": true,
      "suspend": false,
      "startup_priority": 0,
      "shutdown_priority": 0,
      "target": "",
      "restore_target": "",
      "path": "",
      "restore_trace": 0,
      "log": ""
    }
  ]
}

# JSONRPC-STATISTICS

{
  "jsonrpc-statistics": [
    {
      "name": "jsonrpc-statistics1",
      "enabled": true,
      "suspend": false,
      "statistics": {
        "disabled": true,
        "target": ""
      },
      "target": "",
      "enable_write_size": true,
      "enable_error_stat": true,
      "time_suffix": ".time",
      "read_size_suffix": ".rsize",
      "write_size_suffix": ".wsize",
      "request_prefix": "req:",
      "notify_prefix": "ntf:",
      "other_time": "other.time",
      "other_read_size": "other.rsize",
      "other_write_size": "other.wsize"
    }
  ]
}
