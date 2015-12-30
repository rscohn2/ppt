enum to_tool_codes {
  TRACE_METHOD
};

struct trace_method_info {
  void* address;
  char* method_name;
  char* module_name;
};
