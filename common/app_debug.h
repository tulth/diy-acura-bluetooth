#ifndef FILE_APP_DEBUG_SEEN
#define FILE_APP_DEBUG_SEEN

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
  void app_debug_putchar(const char arg);
  void app_debug_print(const char *arg);
  void app_debug_printf(const char *format,...);
  
#ifdef __cplusplus
}
#endif // __cplusplus


#endif // FILE_APP_DEBUG_SEEN
