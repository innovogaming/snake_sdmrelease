#ifndef DONGLE_H
#define DONGLE_H

#include <QObject>
#include <QDebug>
#include <QMessageBox>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include "dic32.h"


class Dongle : public QObject
{
    Q_OBJECT
public:
    explicit Dongle(QObject *parent = 0);

    struct dongle_data_
    {
        QString     hardserial;
        bool        status;
        bool        sesion;
        int         hic;
        int         num;
        bool        error;
    };

    struct dongle_file_
    {
        QByteArray  file_data;
        QString     file_name;
        DWORD       file_id;
        bool        file_exec;
        bool        file_upignore;
        bool        file_internal;
        QString     file_class;
    };

    struct Dongle_In_Value_
    {
        QString     In_Str;
        u_int64_t   In_Value;
        QString     error;
        bool        valid;
    };

    struct Dongle_Out_Value_
    {
        QString     Out_Str;
        u_int64_t   Out_Value;
        QString     error;
        bool        valid;
    };

    struct dongle_info_
    {

        Dongle_In_Value_    in_value;
        Dongle_Out_Value_   out_value;
        QString             hardserial;
        QString             MachineId;
        QString             GameId;
        QString             GameName;
        QString             married;
        QString             PassVer;
        QString             GameVer;
    };

    bool ex;
    bool getDongleConn();
    bool Detect_Dongle();
    dongle_data_ Get_Dongle_Data();
    int Get_Machine_Serial(QString & Machine_Serial_);
    int Get_Id(QString & Id_);
    int Excecle_App_2(int ID, char *data_in, char size_in, char  *data_out, char size_out);
    int Read_Marriage(QString & data );
    int In_Out(uint64_t  & value, char file, char action);
    bool sesion_init(QByteArray pass);
    bool dongle_check_super_pass_status();
    char str_to_char_2(char MSB, char LSB);
    char str_to_char(char value);
    bool dongle_format();
    bool dongle_upload(dongle_file_  file);
    bool dongle_change_pass(QByteArray Oldpass, QByteArray Newpass);
    QString Get_File_Data(QString FileId);
    QByteArray Get_Pass_Version();
    QByteArray Get_Game_Version();
    Dongle_In_Value_ Get_In();
    Dongle_Out_Value_ Get_Out();
    unsigned char Get_Err_Flag(unsigned char &data);
    unsigned char Set_Err_Flag(unsigned char Err, unsigned char mask);
    unsigned char Get_Bad_Shutdown(unsigned int &data);
    unsigned char Set_Bad_Shutdown();

private:
    dongle_data_ dongle_local;

};

#endif // DONGLE_H
