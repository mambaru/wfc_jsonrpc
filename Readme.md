[JSONRPC-BROKER](#jsonrpc-broker) - брокер JSONRPC-сообщений по заданному набору правил распределяет их по целям

# JSONRPC-BROKER

```cpp
{
  "name": "jsonrpc-broker1",
  "enabled": true,
  /* В режиме suspend отправляет все запросы без проверки в 'target' */
  "suspend": false,
  /* Цель по умолчанию, если сообщение не подошло */
  "target": "<<target-name>>",
  /* Список запрещенных методов (клиент получает "Procedure not found") */
  "reject": ["<<method-name-1>>", "<<method-name-2>>"],
  /* Список правил */
  "rules": [
    {/*правило1*/},
    {/*правило2*/}
  ]
}
```

## Правила для методов

```cpp
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
