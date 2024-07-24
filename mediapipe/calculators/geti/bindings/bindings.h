#ifdef _WIN32
#ifdef COMPILING_DLL
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __declspec(dllimport)
#endif
#else
#include <cstddef>
#define DLLEXPORT
#endif

typedef void* CGraphRunner;
typedef void (*CallbackFunction)(const char*);

extern "C" DLLEXPORT const char* GraphRunner_Run(const char* graph_content, const char* image_data, const size_t data_length);
extern "C" DLLEXPORT CGraphRunner GraphRunner_Open(const char* graph_content);
extern "C" DLLEXPORT void GraphRunner_Close(CGraphRunner instance);
extern "C" DLLEXPORT const char* GraphRunner_Get(CGraphRunner instance);
extern "C" DLLEXPORT void GraphRunner_Listen(CGraphRunner instance, CallbackFunction callback);
extern "C" DLLEXPORT void GraphRunner_Queue(CGraphRunner instance, const char* image_data, const size_t data_length);
extern "C" DLLEXPORT void GraphRunner_QueueText(CGraphRunner instance, const char* input);
extern "C" DLLEXPORT void SerializeModel(const char* model_path, const char* model_type, const char* output_filename);
extern "C" DLLEXPORT void GraphRunner_SetLoggingOutput(const char* filename);
extern "C" DLLEXPORT const char** GetAvailableDevices(int* length);


typedef void* CLLMInference;
typedef bool (*StreamerCallbackFunction)(const char*);
extern "C" DLLEXPORT CLLMInference LLM_Init(const char* model_path, const char* device);
extern "C" DLLEXPORT void LLM_SetStreamer(CLLMInference instance, StreamerCallbackFunction callback);
extern "C" DLLEXPORT const char* LLM_Prompt(CLLMInference instance, const char* message);
extern "C" DLLEXPORT void LLM_Close(CLLMInference instance);
extern "C" DLLEXPORT void LLM_ClearHistory(CLLMInference instance);
extern "C" DLLEXPORT void LLM_ForceStop(CLLMInference instance);
