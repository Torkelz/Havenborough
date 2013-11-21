#define SAFE_RELEASE(x) { if(x) { x->Release(); x = nullptr; } }
#define SAFE_DELETE(x) { if(x) { delete x; x = nullptr; } }
#define SAFE_DELETE_ARRAY(x) { if(x) { delete[] x; x = nullptr; } }