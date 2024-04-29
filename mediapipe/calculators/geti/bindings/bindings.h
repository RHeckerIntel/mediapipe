#ifdef COMPILING_DLL
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif

typedef void* CGraphRunner;
typedef void (*CallbackFunction)(const char*);

extern "C" DLLEXPORT const char* GraphRunner_Run(const char* graph_content, const char* image_data, const size_t data_length);
extern "C" DLLEXPORT CGraphRunner GraphRunner_Open(const char* graph_content);
extern "C" DLLEXPORT void GraphRunner_Close(CGraphRunner instance);
extern "C" DLLEXPORT const char* GraphRunner_Get(CGraphRunner instance);
extern "C" DLLEXPORT void GraphRunner_Listen(CGraphRunner instance, CallbackFunction callback);
extern "C" DLLEXPORT void GraphRunner_Queue(CGraphRunner instance, const char* image_data, const size_t data_length);
extern "C" DLLEXPORT void SerializeModel(const char* model_path, const char* model_type, const char* output_filename);
