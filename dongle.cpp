#include "dongle.h"
#include <chrono>

/*************************************************************
*** Este codigo ha sido heredado del software SysDongle
*** y se han añadido/arreglado algunos detalles.
*** Para mayor informacion, revisar codigo fuente SysDongle
**************************************************************/

Dongle::Dongle(QObject *parent):
    QObject(parent)
{

}


bool Dongle::Detect_Dongle()
{
    int i;
    qDebug() << "debug Dongle.cpp - start of Detect_Dongle()";
    
    system("/sbin/modprobe usb-storage 2>/dev/null");
    system("/sbin/modprobe sg	   2>/dev/null");

    dongle_local.num = 0;
    dongle_local.hic = 0;
    dongle_local.sesion = 0;
    dongle_local.status = 0;

    //int errCode = 0;

    qDebug() << "debug Dongle.cpp - DIC_FIND1";
    dongle_local.num = DIC_Find();
    qDebug() << "debug Dongle.cpp - DIC_FIND2";
	
	if (dongle_local.num <= 0)
	{
         qDebug() << "dongle no encontrado_1";
         dongle_local.error = true;
         //ex = false;
         return false;
     }

     for (i=0;i<dongle_local.num;++i)
     {
         qDebug() << "debug Dongle.cpp - DIC_OPEN";
         dongle_local.hic = DIC_Open(i, NULL);
         if (dongle_local.hic >= 0)
         {
             dongle_local.error = false;
             dongle_local.status = true;
             qDebug() << "dongle encontrado_1";
             ex = true;
         }
     }/*
     if (i == dongle_local.num){
         dongle_local.error = true;
         ex = false;
     }*/
     return ex;
}


Dongle::dongle_data_ Dongle::Get_Dongle_Data(){
    int errcode;
    char cmddata[256];	// Command buffer
    DWORD HardSerial;
    bool isfound;

    qDebug() << "debug Dongle.cpp - starting  Detect_Dongle";
    if(dongle_local.error || dongle_local.status == false ){
        qDebug() << "debug Dongle.cpp - starting  Detect_Dongle_inside if";
        isfound = Detect_Dongle();
        if (isfound==false) {
            //QMessageBox::warning(NULL, "Error Dongle", "Dongle no conectada");
            qDebug() << "debug Dongle.cpp - Dongle NOT found";
        }
    }

    qDebug() << "debug Dongle.cpp - starting get hardware info/errcode dic_command";
    errcode = DIC_Command(dongle_local.hic, GET_HARDWARE_INFO, cmddata);

    qDebug() << "debug Dongle.cpp - starting get hardserial dic_get";
    HardSerial = DIC_Get(cmddata, HARD_SERIAL, BY_VALUE, NULL);

    qDebug() << "debug Dongle.cpp - passing hardserial to dongle_local";
    dongle_local.hardserial = QString::number(HardSerial);
    dongle_local.status = true;

    if(errcode){
        qDebug() << "debug Dongle.cpp - if errcode";
        dongle_local.hardserial = QString("");
        dongle_local.error = true;
        dongle_local.status = false;
    }

    qDebug() << "debug Dongle.cpp - get_dongle_data complete";
    return dongle_local;
}

bool Dongle::getDongleConn(){
    return ex;
}

int Dongle::Excecle_App_2(int ID, char *data_in, char size_in, char  *data_out, char size_out)
{

    int num, hic,i,errcode;

    //int readsize;
    char cmddata[512];	// Command buffer
    char buffer[512];	// User buffer
    //WORD dirid;

    char datas[258];

    num = DIC_Find();
    num = DIC_Find();

    for (i=0;i<num;i++){
        hic = DIC_Open(i, NULL);
        if (hic >= 0) break;		// Stop searching once one is found
    }

    if (i == num){
        return 1;
    }

    memset(datas,0,sizeof(datas));
    memcpy(datas,data_in,size_in);

    DIC_Set(buffer, 0, BY_ARRAY | size_in, 0, (char*)&datas); 		// Set x
    DIC_Set(cmddata, RUN_DATA, size_in, ID, buffer);			// Set to buffer
    errcode = DIC_Command(hic, RUN, cmddata);

    if(errcode!=0){
        return 2;
    }

    memset(datas,0,sizeof(datas));
    DIC_Get(cmddata, RUN_DATA, BY_ARRAY, buffer);			// Get result
    DIC_Get(buffer, 0, BY_ARRAY | size_out, (char*)&datas);			// Get x

    errcode = DIC_Close(hic);
    memcpy(data_out,datas,size_out);

    return 0;
}

int Dongle::Get_Id(QString & Id_){
    char input_frame[2];
    char output_frame[4];
    memset(output_frame,0,sizeof(output_frame));
    Excecle_App_2(0x7408, &input_frame[0], 1,&output_frame[0],2);

    Id_=QString(output_frame);
}

int Dongle::In_Out(uint64_t  & value, char file, char action)
{
    char *ptr;
    int i=0;
    char data_to_send[9];
    char data_to_get[9];
    char crc=0;
    char mode_tmp;

    memset(data_to_send,0,sizeof(data_to_send));
    memset(data_to_get,0,sizeof(data_to_get));
    mode_tmp=0;

    if(action == 'R' || action == 'r'){
        mode_tmp = mode_tmp + 0;
    }

    if(action == 'W' || action == 'w'){
        mode_tmp = mode_tmp + 1;
    }

    if(file == 'I' || file == 'i'){
        mode_tmp = mode_tmp +  0;
    }

    if(file == 'O' || file == 'o'){
        mode_tmp = mode_tmp +  2;
    }

    data_to_send[8]=mode_tmp;

    switch(mode_tmp){
    case 0: // Read In
    case 2: //Read Out
        Excecle_App_2(0x7400, &data_to_send[0],9, &data_to_get[0], 9);
        for(i=0;i<8;i++){
            crc = crc + data_to_get[i];
        }
        crc = 255 - crc;

        if(crc == data_to_get[8]){
            ptr = (char *)&value;
            for(i=0;i<8;i++)
                ptr[i] = data_to_get[i];
        }else{
            value = 0;
            return 1;
        }
        break;

    case 1: // Write In
    case 3: //Write Out
        ptr = (char *)&value;
        for(i=0;i<8;i++){
            data_to_send[i] = ptr[i];
        }
        Excecle_App_2(0x7400, &data_to_send[0],9, &data_to_get[0], 9);
        break;
    }

}

int Dongle::Read_Marriage(QString & data ){
    char data_to_send[2];
    char data_to_get[10];
    memset(data_to_get,0,sizeof(data_to_get));
    Excecle_App_2(0x7402, &data_to_send[0],1, &data_to_get[0], 7);
    data=QString(data_to_get);
}

int Dongle::Get_Machine_Serial(QString & Machine_Serial_){
    char input_frame[2];
    char output_frame[8];
    memset(output_frame,0,sizeof(output_frame));
    Excecle_App_2(0x7406, &input_frame[0], 1,&output_frame[0],5);

    Machine_Serial_=QString(output_frame);
    return 0;
}

char Dongle::str_to_char_2(char MSB, char LSB){
    char result;
    result= (str_to_char(MSB) << 4) ;
    result= result + str_to_char(LSB);
    return result;
}

char Dongle::str_to_char(char value){
    char result;

    if(value >= '0' && value <= '9'){
        result = value -0x30;
        return result;
    }

    if( value >='A' && value <='F'){
        result = value - 0x37;
        return result;
    }

    if( value >='a' && value <='f'){
        result = value - 0x57;
        return result;
    }
}

bool Dongle::dongle_check_super_pass_status(){
    char cmddata[256];	// Command buffer

    if(dongle_local.status){
        dongle_local.error = DIC_Command(dongle_local.hic, GET_CARD_PRIVILEGE, cmddata);
        if (DIC_Get(cmddata, SUPERPASS_STATUS, BY_VALUE, NULL)){
            qDebug() << "Super password verified\n";
            return true;
        }else{
            qDebug() << "este super pass error se ejecuta solo una vez al principio cuando se hace dongle sesion init";
            qDebug() << "Super password not verified";
            return false;
        }
    }

    return false;
}

bool Dongle::dongle_upload(dongle_file_ file){
    char attr;
    int file_size;
    int name_size;
    QByteArray name_data;
    char name_char[20];
    char file_char[8192];
    char cmddata[8192];	// Caution: buffer size at least DIST_SIZE+4
    //unsigned int app_id_tmp;
    //  char cmddata[2048];	// Caution: buffer size at least DIST_SIZE+4
    // Caution: buffer size should be enougth for all result.
    int  i = 0;
    attr = 0;
    if(file.file_exec){
        attr |= 0x10;
    }

    if(file.file_internal){
        attr |= 0x80;
    }

    if(dongle_check_super_pass_status()){
        file_size = file.file_data.size();
        name_size = file.file_name.size();
        name_data = file.file_name.toLatin1();
        memset(name_char,0,sizeof(name_char));

        for(i=0;i<name_size;i++){
            name_char[i] = name_data[i];
        }

        memset(file_char,0,sizeof(file_char));

        for(i=0;i<file_size;i++){
            file_char[i] = file.file_data[i];
        }

        DIC_Set(cmddata, FILL, 8192, 0, NULL);					// Clear buffer
        DIC_Set(cmddata, FILE_ID, BY_VALUE, file.file_id, NULL);			// File ID
        DIC_Set(cmddata, FILE_CLASS, BY_VALUE, 0xff, NULL);			// File class
        DIC_Set(cmddata, FILE_ATTRIBUTE, BY_VALUE, attr, NULL);	// File attr
        DIC_Set(cmddata, FILE_SIZE, BY_VALUE, file_size, NULL);			// File size
        DIC_Set(cmddata, FILE_NAME, BY_ARRAY, 0, name_char);
        dongle_local.error = DIC_Command(dongle_local.hic, CREATE_FILE, cmddata);
        DIC_Set(cmddata, WRITE_DATA, file_size, 0, (char*)file_char);
        dongle_local.error = DIC_Command(dongle_local.hic, WRITE_FILE, cmddata);

        return true;
    }

    return false;
}

bool Dongle::dongle_change_pass(QByteArray Oldpass, QByteArray Newpass){
    char cmddata[256];	// Command buffer
    char buffer[256];
    int i=0;
    char pass_char[8];
    char test[4];

    if(dongle_check_super_pass_status() == false){
        qDebug() << "Chequeando Super Pass Status";
        if(sesion_init(Oldpass)== false){
            qDebug() << "error Iniciando Super root";
            return false;
        }
    }

    //qDebug() << "lesto...!!!";
    memset(cmddata,0,sizeof(cmddata));
    memset(buffer,0,sizeof(buffer));

    for(i=0;i<8;i++){
        //buffer[i] = Newpass[i];
        pass_char[i]=str_to_char_2( Newpass[2*i],Newpass[2*i+1]);
    }

    for (i=0;i<8;i++){
        buffer[i] = pass_char[i];
    }

    for(i=0;i<8;i++){
        sprintf(test,"%u",buffer[i] & 0xFF);
        //qDebug() << "test:" << test;
    }

    DIC_Set(cmddata, SUPERPASS_DATA, BY_ARRAY, 250, buffer);
    //qDebug() << "cmddata:" << cmddata;

    for(i=0;i<20;i++){
        sprintf(test,"%X",cmddata[i] & 0xFF);
        //qDebug() << "test:" << test;
    }

    dongle_local.error = 0;
    dongle_local.error = DIC_Command(dongle_local.hic, SET_SUPER_PASS, cmddata);
    //qDebug() << "cmddata:" << cmddata;
    if(dongle_local.error){
        //qDebug() << "NO Se cambió la clave correctamente";
        return false;
    }

    return true;
}

bool Dongle::dongle_format(){
    char cmddata[1024];
    int errcode;
    if(dongle_local.sesion){
        DIC_Set(cmddata, VOLUME, BY_ARRAY, 0, "Calabaza");
        DIC_Set(cmddata, ATR, BY_ARRAY, 0, "DIC Co.");
        errcode = DIC_Command(dongle_local.hic, FORMAT_CARD, cmddata);
        if(errcode){
            return false;
        }
        return true;
    }
    return false;
}

bool Dongle::sesion_init(QByteArray pass){
    int errcode;
    int i;
    char cmddata[1024];
    char pass_char[8];
    if(dongle_check_super_pass_status()){
        qDebug() << "dongle_check_super_pass_status is true and return true";
        return true;
    }else{
        memset(pass_char,0,sizeof(pass_char));

        for(i=0;i<8;i++){
            pass_char[i]=str_to_char_2( pass[2*i],pass[2*i+1]);
        }
        for (i=0;i<8;i++){
           cmddata[i] = pass_char[i];
        }			 // Super password is 8 FFs for demo Rockey6Smart

        errcode = DIC_Command(dongle_local.hic, CHECK_SUPER_PASS, cmddata);
        if(errcode){
            dongle_local.error = true;
            dongle_local.sesion = false;
            qDebug() << "Super pass error verificando";
            return false;
        }

        dongle_local.error = false;
        dongle_local.sesion = true;
        qDebug() << "Super pass Granted verificando";
        return true;
    }
}

QString Dongle::Get_File_Data(QString FileId){
    DWORD   File_Id;
    DWORD   App_Id;
    char     size_in;
    char     size_out;
    char    data_out[10];
    char    data_in[10];
    char    cmddata[512];	// Command buffer
    char    buffer[512];	// User buffer

    File_Id = FileId.toInt(0,16);

    switch(File_Id){
    case 0x2020:
        size_in = 1;
        size_out = 2;
        App_Id  = 0x7408;
    break;

    case 0x2022:
        size_in = 1;
        size_out = 5;
        App_Id  = 0x7406;
    break;

    case 0x2024:
        size_in = 1;
        size_out = 5;
        App_Id  = 0x7402;
    break;

    case 0x2026:

    break;

    case 0x2028:

    break;

    case 0x202A:

    break;

    case 0x202B:

    break;
    }

    memset(data_in,0,sizeof(data_in));
    memset(data_out,0,sizeof(data_out));
    memset(buffer,0,sizeof(buffer));
    memset(cmddata,0,sizeof(cmddata));
    DIC_Set(buffer, 0, BY_ARRAY | size_in, 0, (char *)&data_in); 		// Set x
    DIC_Set(cmddata, RUN_DATA, 1, App_Id, buffer);			// Set to buffer
    dongle_local.error = DIC_Command(dongle_local.hic, RUN, cmddata);
    if(dongle_local.error!=0){
    return QString("No data...");
    //qDebug() << "No Data...";
    }
    // memset(buffer,0,sizeof(buffer));
    // memset(cmddata,0,sizeof(cmddata));
    DIC_Get(cmddata, RUN_DATA, BY_ARRAY, buffer);			// Get result
    DIC_Get(buffer, 0, BY_ARRAY | size_out, (char *)&data_out);			// Get x

    //qDebug() << "out:" << data_out;

    return(QString(data_out));
}


QByteArray Dongle::Get_Pass_Version(){
    char cmddata[512];	// Command buffer
    char buffer[512];	// User buffer
    QByteArray data_local;
    int readsize1=0;
    readsize1 = readsize1 + 1;
    int i;
    //qDebug() << "Get Pass Version....:";
    //	printf("\nfile_id:%u\nfile_size:%u",File_Id,File_Size);
    DIC_Set(cmddata, FILL, 1024, 0, NULL);
    DIC_Set(cmddata, DIR_ID, BY_VALUE, 0x3f00, NULL);
    dongle_local.error = DIC_Command(dongle_local.hic, SET_CURRENT_DIR, cmddata);

    DIC_Set(cmddata, FILE_ID, BY_VALUE, 8272, NULL);
    dongle_local.error = DIC_Command(dongle_local.hic, SET_CURRENT_FILE, cmddata);
    // m. Read it.
    DIC_Set(cmddata, READ_DATA, 0x06, 0, NULL);
    dongle_local.error = DIC_Command(dongle_local.hic, READ_FILE, cmddata);
    int readsize;
    readsize = DIC_Get(cmddata, READ_DATA, BY_VALUE, buffer);

    for(i=0;i<6;i++){
        data_local[i]=buffer[i];
    }

    return data_local;
}


QByteArray Dongle::Get_Game_Version(){
    char cmddata[1024];	// Command buffer
    char buffer[1024];	// User buffer
    QByteArray data_local;
    int readsize;
    int i;

    DIC_Set(cmddata, FILL, 1024, 0, NULL);
    DIC_Set(cmddata, DIR_ID, BY_VALUE, (DWORD)(16128), NULL);

    dongle_local.error = false;
    dongle_local.error = DIC_Command(dongle_local.hic, SET_CURRENT_DIR, cmddata);

    memset(cmddata,0,sizeof(cmddata));
    DIC_Set(cmddata, FILE_ID, BY_VALUE,(DWORD)(8274), NULL);

    dongle_local.error = false;
    dongle_local.error = DIC_Command(dongle_local.hic, SET_CURRENT_FILE, cmddata);

    memset(cmddata,0,sizeof(cmddata));
    DIC_Set(cmddata, READ_DATA, 0x06, 0, NULL);
    dongle_local.error = false;
    dongle_local.error = DIC_Command(dongle_local.hic, READ_FILE, cmddata);

    readsize = DIC_Get(cmddata, READ_DATA, BY_VALUE, buffer);


    for(i=0;i<6;i++){
        data_local[i]=buffer[i];
    }

    return data_local;
}


Dongle::Dongle_In_Value_ Dongle::Get_In()
{
    Dongle_In_Value_ local_in;
    char data_to_send[12];
    char data_to_get[12];
    u_int64_t value = 0;
    char    *ptr;
    int i=0;
    char crc;

    //char cmddata[512];	// Command buffer
    //char buffer[512];	// User buffer
    qDebug() << "Dongle_Get_In";

    local_in.valid = 0;

    memset(data_to_send,0,sizeof(data_to_send));
    memset(data_to_get,0,sizeof(data_to_get));
    data_to_send[8] = 0;
    Excecle_App_2(0x7400, &data_to_send[0],9, &data_to_get[0], 9);

    crc = 0;
    for(i=0;i<8;i++)
    {
        crc = crc + data_to_get[i];
    }

    crc = 255 - crc;
    if(crc == data_to_get[8])
    {
        ptr = (char *)&value;
        for(i=0;i<8;i++)
            ptr[i] = data_to_get[i];
    }
    else
    {
        local_in.error = QString("Data is corrupted");
        local_in.In_Str = QString("No Data");
        local_in.In_Value = 0;
        return local_in;
    }

    local_in.valid  = true;
    local_in.In_Value = value;
    qDebug() << "IN_value_Num" << value;
    local_in.In_Str.sprintf("%llu",value);

    return local_in;
}

Dongle::Dongle_Out_Value_ Dongle::Get_Out()
{
	auto start = std::chrono::high_resolution_clock::now();
	
    Dongle_Out_Value_ local_out;
    char data_to_send[9];
    char data_to_get[9];
    u_int64_t value = 0;
    char    *ptr;
    int i=0;
    char crc;

	qDebug() << "Dongle_Get_Out2";

    local_out.valid = 0;

    memset(data_to_send,0,sizeof(data_to_send));
    memset(data_to_get,0,sizeof(data_to_get));

    data_to_send[8] = 2;

    Excecle_App_2(0x7400, &data_to_send[0],9, &data_to_get[0], 9);

    crc = 0;
    for(i=0;i<8;i++)
    {
        crc = crc + data_to_get[i];
    }

    crc = 255 - crc;
    if(crc == data_to_get[8])
    {
        ptr = (char *)&value;
        for(i=0;i<8;i++)
            ptr[i] = data_to_get[i];
    }
    else
    {
        local_out.error = QString("Data is corrupted");
        local_out.Out_Str = QString("No Data");
        local_out.Out_Value = 0;
        return local_out;
    }
    local_out.valid  = true;
    local_out.Out_Value = value;
    qDebug() << "Out_value_Num" << value;
    local_out.Out_Str.sprintf("%llu",value);
	
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	
	qDebug() << "Elapsed time: " << elapsed.count();
	
    return local_out;
}

unsigned char Dongle::Get_Err_Flag(unsigned char &data){
    char data_to_send[3];
    char data_to_get[8];

    data_to_send[0] = 0x02;
    Excecle_App_2(0x7600, &data_to_send[0],3, &data_to_get[0], 3);

    switch(data_to_get[0])
    {
        case 0:
            printf("No Dongle or App\n");
            return 0;
        break;

        case 1:
            printf("Get_Err_Flag Ok...\n");
            data = data_to_get[1];
            return 1;
        break;

        case 2:
            printf("Get_Err_Flag Error General...\n");
            return 2;
        break;

        case 3:
            printf("No Existe El Archivo...\n");
            return 3;
        break;

        case 4:
            printf("El comando Enviado No Existe...\n");
            return 4;
        break;

    }
    return 0;
}


unsigned char Dongle::Set_Err_Flag(unsigned char Err, unsigned char mask){
    char data_to_send[3];
    char data_to_get[8];

    data_to_send[0] = 0x01;
    data_to_send[1] = mask;
    data_to_send[2] = Err;

    Excecle_App_2(0x7600, &data_to_send[0],3, &data_to_get[0], 3);

    switch(data_to_get[0]){
        case 0:
            printf("No Dongle or App\n");
            return 0;
        break;

        case 1:
            printf("Set_Err_Flag Ok...\n");
            return 1;
        break;

        case 2:
            printf("Set_Err_Flag Error General...\n");
            return 2;
        break;

        case 3:
            printf("...\n");
            return 3;
        break;

        case 4:
            printf("El comando Enviado No Existe...\n");
            return 4;
        break;

    }

    return 0;
}


unsigned char Dongle::Get_Bad_Shutdown(unsigned int &data)
{
    char data_to_send[3];
    char data_to_get[8];

    data_to_send[0] = 0x05;

    Excecle_App_2(0x7600, &data_to_send[0],3, &data_to_get[0], 3);

    data = data_to_get[1];

    data |= (unsigned int )(data_to_get[2] >>8);

    return data_to_get[0];
}

unsigned char Dongle::Set_Bad_Shutdown()
{
    char data_to_send[3];
    char data_to_get[8];

    data_to_send[0] = 0x04;

    Excecle_App_2(0x7600, &data_to_send[0],3, &data_to_get[0], 3);

    return data_to_get[0];
}
