/* rand example: guess the number */
#include <stdio.h>
#include <string>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include "crypto.h"

namespace Tinker_Crypto {

    /*Crypto::Crypto()
	{
        printf("Constructor **** Crypto\n");
    }*/

    int Crypto::Build(const char *drivePath, std::string prtKey, const char * cryptoName)
    {
        printf("Build **** Crypto\n");
        //int i,j;
        // Disable coredumps
        {
            rlimit rl;
            rl.rlim_cur = rl.rlim_max = 0;
            setrlimit( RLIMIT_CORE, &rl );
        }

        {

            std::cout << "drivePath: " << drivePath << std::endl;
            std::cout << "prtKey: " << prtKey << std::endl;

            /*if(argc != 3)
            {
                printf("Necesita 2 argumentos: /dev/device path_to_key\n");
                return 0;
            }*/

            /*char buff[100];
            FILE *f = fopen(argv[2], "r");
            fgets(buff, 100, f);
            printf("Clave leida: %s\n", buff);
            fclose(f);*/

            //const char *ptr = buff;

            //const char *driveTeste = "/dev/mapper/loop15p9";
            //char driveTmp[256];
            //strcpy(driveTmp, drivePath);

            //const char * cryptoName = "rootfs";

            printf("\nAntes del format\n");

            format_and_add_keyslots(drivePath, prtKey.c_str(),32);

            printf("Antes del openCrypto\n");

            char prtKeyTmp[256];
            strcpy(prtKeyTmp, prtKey.c_str());


            if(openCryptoDrive(drivePath, prtKeyTmp, 32, 0, cryptoName))
            {
                printf("Error montando particion...\n");
                printf("Error..........................2\n");
                return -1;
            }
        }
        return 0;
    }

    int Crypto::format_and_add_keyslots(const char *path, const char* passkey, int passSize)
    {
        //   struct crypt_device *cd;
        struct crypt_params_luks1 params;
        int r;

        printf("\npassKey:%s\n",passkey);
        printf("passSize:%u\n",passSize);
        std::cout << "path: " << path << std::endl;

        r = crypt_init(&cd, path);
        if (r < 0 ) {
            fprintf(stderr, "Erro ao inicializar a estrutura de dados: %s\n", strerror(-r));
            printf("crypt_init() failed for %s, erro:%d\n", path,r);
            return r;
        }

        printf("Context is attached to block device %s.\n", crypt_get_device_name(cd));

        params.hash = "sha1";

        params.data_alignment = 0;

        params.data_device = NULL;

        r = crypt_format(cd,            /* crypt context */
                        CRYPT_LUKS1,   /* LUKS1 is standard LUKS header */
                        "aes",         /* used cipher */
                        "cbc-plain64", /* used block mode and IV generator*/
                        NULL,          /* generate UUID */
                        NULL,          /* generate volume key from RNG */
                        256 / 8,       /* 256bit key - here AES-128 in XTS mode, size is in bytes */
                        &params);      /* parameters above */

        if(r < 0) {
            printf("crypt_format() failed on device %s\n", crypt_get_device_name(cd));
            crypt_free(cd);
            return r;
        }

        r = crypt_keyslot_add_by_volume_key(cd,                 /* crypt context */
                                            CRYPT_ANY_SLOT,     /* just use first free slot */
                                            NULL,               /* use internal volume key */
                                            0,                  /* unused (size of volume key) */
                                            &passkey[0],              /* passphrase - NULL means query*/
                                            passSize);                 /* size of passphrase */

        if (r < 0) {
            printf("Adding keyslot failed.\n");
            crypt_free(cd);
            return r;
        }

        printf("The first keyslot is initialized.\n");

        crypt_free(cd);
        return 0;
    }

    int Crypto::activate_and_check_status(const char *path, const char *device_name)
    {
        struct crypt_active_device cad;
        int r;

        r = crypt_init(&cd, path);
        if (r < 0 ) {
            printf("crypt_init() failed for %s.\n", path);
            return r;
        }

        r = crypt_load(cd,              /* crypt context */
                       CRYPT_LUKS1,     /* requested type */
                       NULL);           /* additional parameters (not used) */

        if (r < 0) {
            printf("crypt_load() failed on device %s.\n", crypt_get_device_name(cd));
            crypt_free(cd);
            return r;
        }

        r = crypt_activate_by_passphrase(cd,            /* crypt context */
                                         device_name,   /* device name to activate */
                                         CRYPT_ANY_SLOT,/* which slot use (ANY - try all) */
                                         "foo", 3,      /* passphrase */
                                         CRYPT_ACTIVATE_READONLY); /* flags */
        if (r < 0) {
            printf("Device %s activation failed.\n", device_name);
            crypt_free(cd);
            return r;
        }
        /*
                printf("LUKS device %s/%s is active.\n", crypt_get_dir(), device_name);
                printf("\tcipher used: %s\n", crypt_get_cipher(cd));
                printf("\tcipher mode: %s\n", crypt_get_cipher_mode(cd));
                printf("\tdevice UUID: %s\n", crypt_get_uuid(cd));
        */

         r = crypt_get_active_device(cd, device_name, &cad);
         if (r < 0) {
            // printf("Get info about active device %s failed.\n", device_name);
            crypt_deactivate(cd, device_name);
            crypt_free(cd);
            return r;
        }
        /*
                printf("Active device parameters for %s:\n"
                        "\tDevice offset (in sectors): %" PRIu64 "\n"
                        "\tIV offset (in sectors)    : %" PRIu64 "\n"
                        "\tdevice size (in sectors)  : %" PRIu64 "\n"
                        "\tread-only flag            : %s\n",
                        device_name, cad.offset, cad.iv_offset, cad.size,
                        cad.flags & CRYPT_ACTIVATE_READONLY ? "1" : "0");
        */
        crypt_free(cd);
        return 0;
    }

    int Crypto::handle_active_device(const char *device_name)
    {

        int r;

        r = crypt_init_by_name(&cd, device_name);
        if (r < 0) {
            return r;
        }

        if (crypt_status(cd, device_name) == CRYPT_ACTIVE)
        {
            printf("Device %s is still active.\n", device_name);

        }
        else {
            printf("Something failed perhaps, device %s is not active.\n", device_name);
            crypt_free(cd);
            return -1;
        }

        return 0;
    }

    int Crypto::openCryptoDrive(const char *path, char* passkey, int passSize, int flag, const char *device_name)
    {

        struct crypt_active_device cad;
        int r;

        r = crypt_init(&cd, path);
        if (r < 0 ) {
            printf("crypt_init() failed for %s.\n", path);
            return r;
        }

        r = crypt_load(cd,              /* crypt context */
                       CRYPT_LUKS1,     /* requested type */
                       NULL);           /* additional parameters (not used) */

        if (r < 0) {
            printf("crypt_load() failed on device %s.\n", crypt_get_device_name(cd));
            crypt_free(cd);
            return r;
        }

        r = crypt_activate_by_passphrase(cd,            /* crypt context */
                                         device_name,   /* device name to activate */
                                         CRYPT_ANY_SLOT,/* which slot use (ANY - try all) */
                                         &passkey[0],
                                         passSize,      /* passphrase */
                                         flag); /* flags */
        if (r < 0) {
            printf("Device %s activation failed.\n", device_name);
            crypt_free(cd);
            return r;
        }

        printf("LUKS device %s/%s is active.\n", crypt_get_dir(), device_name);
        printf("\tcipher used: %s\n", crypt_get_cipher(cd));
        printf("\tcipher mode: %s\n", crypt_get_cipher_mode(cd));
        printf("\tdevice UUID: %s\n", crypt_get_uuid(cd));

        r = crypt_get_active_device(cd, device_name, &cad);
        if (r < 0) {
            printf("Get info about active device %s failed.\n", device_name);
            crypt_deactivate(cd, device_name);
            crypt_free(cd);
            return r;
        }

        printf("Active device parameters for %s:\n"
               "\tDevice offset (in sectors): %" PRIu64 "\n"
               "\tIV offset (in sectors)    : %" PRIu64 "\n"
               "\tdevice size (in sectors)  : %" PRIu64 "\n"
               "\tread-only flag            : %s\n",
               device_name, cad.offset, cad.iv_offset, cad.size,
               cad.flags & CRYPT_ACTIVATE_READONLY ? "1" : "0");


        crypt_free(cd);
        return 0;

    }

    int Crypto::closeCryptoDrive(const char *device_name)
    {

        int r;
        r = crypt_deactivate(cd, device_name);
        if (r < 0) {
            printf("crypt_deactivate() failed.\n");
            crypt_free(cd);
            return r;
        }

        printf("Device %s is now deactivated.\n", device_name);

        crypt_free(cd);
    }


}
