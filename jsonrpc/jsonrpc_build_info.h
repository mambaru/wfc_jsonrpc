struct jsonrpc_build_info{
  bool enabled() const;
  const char* name() const;
  const char* version() const;
  const char* branch() const;
  const char* commit() const;
  const char* date() const;
  const char* author() const;
  const char* project() const;
  const char* message() const;
  const char* authors() const;
};
