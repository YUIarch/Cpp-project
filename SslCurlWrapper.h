#include <mutex>
#include <openssl/err.h>
#include <vector>
#include <stdio.h> 
#include <stdlib.h> 
#include <pthread.h>


class SslCurlWrapper
{
private:
    static std::vector<std::mutex> vectorOfSslMutex;
    static auto id_function() -> pthread_t { return (pthread_self()); }
    static auto locking_function(int, int, const char*, int) -> void;

public:
    SslCurlWrapper();
    ~SslCurlWrapper();
};

std::vector<std::mutex> SslCurlWrapper::vectorOfSslMutex(CRYPTO_num_locks());

//----------------------------------------
auto SslCurlWrapper::locking_function(int mode,
    int n,
    const char* file,
    int line
) -> void
//----------------------------------------
{
    if (mode & CRYPTO_LOCK)   vectorOfSslMutex[n].lock();
    else                        vectorOfSslMutex[n].unlock();
}

//------------------------------
SslCurlWrapper::SslCurlWrapper()
//------------------------------
{
    CRYPTO_set_id_callback(id_function);
    CRYPTO_set_locking_callback(locking_function);
}

//-------------------------------
SslCurlWrapper::~SslCurlWrapper()
//-------------------------------
{
    CRYPTO_set_id_callback(nullptr);
    CRYPTO_set_locking_callback(nullptr);
}