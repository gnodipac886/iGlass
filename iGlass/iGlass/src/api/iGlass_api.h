#ifndef iGLASS_API
#define iGLASS_API

class iGlass_api {
	public:
		// iGlass_api();
		// virtual ~iGlass_api() = 0;
		virtual void init() = 0;
		virtual void main_task() = 0;
};

// iGlass_api::iGlass_api(){}
// iGlass_api::~iGlass_api(){}

#endif