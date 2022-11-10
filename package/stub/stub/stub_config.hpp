#pragma once

#include <string>
#include <vector>
#include <memory>

namespace wfc{ namespace jsonrpc{

struct stub_config
{
  enum mode_t{
    forward_response, // Обычный режим с прокидыванием ответ
    ignore_errors,    // Если ответ jsonrpc-ошибка, то в ответ отсылать fake_result
    fake_response,    // Всегда пересылать fake_result, после получения ответа
    advance_response  // То же, что и fake_response но до отправки запроса
  };
  /* Если target="" то возвращает result_value, а не ошибку service_unavailable */
  std::string target;
  /* Преобразует запросы в уведомления, в ответ отправляет result_value, реальный ответ игнорируется */
  bool to_notify = false;
  /* Преобразует уведомления в запросы, полученый ответ игнорируется */
  bool to_request = false;
  /* Сначала отправляет result_value, только потом выполняет запрос/уведомление, реальный ответ игнорируется*/
  //bool advance_response = false;
  /* Преобразует jsonrpc ошибки, в ответ используя result_value*/
  //bool fix_errors = false;
  mode_t mode = mode_t::forward_response;
  /* Значение поля result в ответе */
  std::shared_ptr<std::string> fake_result;
};

}}
