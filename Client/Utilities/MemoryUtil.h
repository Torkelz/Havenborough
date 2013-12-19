#pragma once
#define SAFE_RELEASE(x) { if(x) { x->Release(); x = nullptr; } }
#define SAFE_SHUTDOWN(x) { if(x) { x->shutdown(); x = nullptr; } }
#define SAFE_DELETE(x) { if(x) { delete x; x = nullptr; } }
#define SAFE_DELETE_ARRAY(x) { if(x) { delete[] x; x = nullptr; } }