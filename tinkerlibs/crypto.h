#ifndef CRYPTO_H
#define CRYPTO_H
#include <string>
#include <libcryptsetup.h>

namespace Tinker_Crypto {


    class Crypto
	{
		public:

            /*Crypto();

            ~Crypto()
            {
                // Código para liberar recursos
            }*/

            int Build(const char *drivePath,std::string prtKey,const char * cryptoName);
            int closeCryptoDrive(const char *device_name);
            int openCryptoDrive(const char *path, char* passkey, int passSize, int flag, const char *device_name);

		private:

            struct crypt_device *cd = NULL;

            int format_and_add_keyslots(const char *path, const char* passkey, int passSize);
            int activate_and_check_status(const char *path, const char *device_name);
            int handle_active_device(const char *device_name);

	};

}
#endif // CRYPTO_H
