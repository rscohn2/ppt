enum to_tool_codes {
  START_TRACE,
  STOP_TRACE
};

struct trace_method_info {
  void* address;
  char* method_name;
  char* module_name;
};
