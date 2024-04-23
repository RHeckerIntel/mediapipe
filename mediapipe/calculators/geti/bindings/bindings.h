#ifdef COMPILING_DLL
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

typedef void* CGraphRunner;
typedef void (*CallbackFunction)(const char*);

extern "C" DLLEXPORT const char* GraphRunner_Run(const char* graph_type, const char* model_path, const char* image_path);
extern "C" DLLEXPORT CGraphRunner GraphRunner_Open(const char* graph_type, const char* model_path);
extern "C" DLLEXPORT void GraphRunner_Close(CGraphRunner instance);
extern "C" DLLEXPORT int GraphRunner_Get();
extern "C" DLLEXPORT int GraphRunner_Listen();
extern "C" DLLEXPORT int GraphRunner_Queue(const char* image_path);
