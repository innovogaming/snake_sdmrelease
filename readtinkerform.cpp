#include "readtinkerform.h"
#include "ui_readtinkerform.h"
#include "login.h"

/*************************************************************
***
*** Este codigo re-utiliza codigo heredado del
*** software STARTTINKER y READTINKER
*** Para mayor informacion, revisar codigo fuente en github
*** https://github.com/InnovoGaming9000/Produccion_Tinker
***
**************************************************************/

ReadTinkerForm::ReadTinkerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReadTinkerForm)
{
    ui->setupUi(this);
    ui->PrintLabel_Button->setEnabled(false);
    ui->WriteRtc_Button->setEnabled(false);
    QStringList titles ;
    titles.append("Item");
    titles.append("Valor");
    ui->tableWidget->setHorizontalHeaderLabels(titles);
}

ReadTinkerForm::~ReadTinkerForm()
{
    delete ui;
}

uint8_t ReadTinkerForm::Check_Sum_Param(uint8_t * datas, uint8_t len,uint8_t idx)
{
    uint8_t checksum=0;
    uint8_t i=0;
    for(i=0;i<len;i++)
    {
        if((i%idx)!=0)
        {
            checksum -=datas[i];
        }
        else
        {
            checksum +=datas[i];
        }
    }
    checksum = 0xFF - checksum + 1;
    return(checksum);
}

unsigned long ReadTinkerForm::str_to_long(uint8_t *value)
{
    unsigned long tmp=0;
    uint8_t *ptr_tmp;

    ptr_tmp= (uint8_t *) &tmp;
    ptr_tmp[0]=value[3];
    ptr_tmp[1]=value[2];
    ptr_tmp[2]=value[1];
    ptr_tmp[3]=value[0];

    return(tmp);
}

int ReadTinkerForm::Get_Vigencia_From_Tinker(uint8_t file10[64], uint64_t & date_of_reg)
{
    int i=0;
    int j=0;
    uint8_t *ptr;
    char size_in;
    char size_out;
    uint8_t crc_1_str=0;
    uint8_t crc_2_str;
    uint8_t crc_1_calc=0;
    uint8_t crc_2_calc;
    char size_id_1;
    char size_id_2;
    uint8_t array_id_1[4];
    uint8_t array_id_2[4];
    uint8_t array_in[8];
    uint8_t array_out[8];
    unsigned int vigencia_1;
    unsigned int vigencia_2;
    uint8_t sum_pass_1_c = 0;
    uint8_t sum_pass_2_c = 0;
    uint8_t sum_pass_1;
    uint8_t sum_pass_2;
    uint8_t array_pass_1[8];
    uint8_t array_pass_2[8];
    uint8_t array_pass_3[8];
    uint8_t array_pass_3_1[4];
    unsigned long in;
    unsigned long out;
    unsigned long id;
    unsigned long patern[8]={1225000,962500,2117500,1100000,1537500,1675000,2137500,3082500};
    unsigned long tmp_1;
    unsigned long tmp_2;
    unsigned long tmp_3;
    unsigned long tmp_4;
    unsigned long tmp_5;
    unsigned long key[8];
    unsigned long key_2[4];
    unsigned long key_3[2];
    uint8_t char_tmp1 = 0;
    uint8_t char_tmp2 = 0;
    unsigned long vig_key;
    uint8_t input_frame[64];
    uint8_t output_frame[46];

    uint8_t out_tmp[10];
    uint64_t dias_tmp=0;

    memset(input_frame,0,sizeof(input_frame));
    memset(out_tmp,0,sizeof(out_tmp));
    memset(output_frame,0,sizeof(output_frame));

    //printf("\n Executing Get_Vigencia_From_Tinker\n");

    for(i=0;i<10;i++)
    {
        output_frame[i] = out_tmp[i];
    }

    //Storage::Class gameStorage(&sequencer, 16000,0);
    //Storage::Class systemStorage(&sequencer, 128,gameStorage.getMaxPhysicalSize());


    //uint16_t size;
    //memset(&systemState,0,sizeof(systemState));
    //systemStorage.startReading(size,buffer);
    //systemStorage.readData(&systemState,size);

    for( int tX = 0; tX < 64; tX++ )
    {
        //printf("file10[%d]: %d\n", tX, systemState.file10[tX] );
        input_frame[tX] = file10[tX];
    }

    //printf("File 10, input_frame: %d\n", input_frame[0]);
    /*int sizeCopiedFrame = sizeof(input_frame);
    printf("size of input_frame: %d\n", sizeCopiedFrame);

    for( int rX = 0; rX < sizeCopiedFrame; rX++ )
    {
        printf("input_frame[%d]: %d\n", rX, input_frame[rX] );
    }*/

    vigencia_1 =(unsigned int) (input_frame[52] << 8) & 0xFF00;
    vigencia_1 |= input_frame[53] & 0xFF;
    vigencia_2 =(unsigned int) (input_frame[54] << 8) & 0xFF00;
    vigencia_2 |= input_frame[55] & 0xFF;

    for(i=0;i<8;i++)
    {
        array_in[i]=0;
        array_out[i]=0;
        array_pass_1[i]=0;
        array_pass_2[i]=0;
    }

    for(i=0;i<4;i++)
    {
        array_id_1[i]=0;
        array_id_2[i]=0;
    }

    size_in = (input_frame[0] ^ input_frame[43]) & 0x0F;
    size_out = (input_frame[22] ^ input_frame[21]) & 0x0F;
    input_frame[0] = (input_frame[0] ^ input_frame[43]);

    crc_1_str=input_frame[43];
    crc_2_str=input_frame[21];

    crc_1_calc = Check_Sum_Param(&input_frame[0],21,1);
    crc_1_calc = crc_1_calc + Check_Sum_Param(&input_frame[0],21,2);
    crc_1_calc = crc_1_calc + Check_Sum_Param(&input_frame[0],21,3);
    crc_1_calc = crc_1_calc + Check_Sum_Param(&input_frame[0],21,5);
    crc_1_calc = crc_1_calc + Check_Sum_Param(&input_frame[0],21,7);

    input_frame[22] = (input_frame[22] ^ input_frame[21]);

    crc_1_str=input_frame[43];
    crc_2_str=input_frame[21];

    crc_2_calc = Check_Sum_Param(&input_frame[22],21,1);
    crc_2_calc = crc_2_calc + Check_Sum_Param(&input_frame[22],21,2);
    crc_2_calc = crc_2_calc + Check_Sum_Param(&input_frame[22],21,3);
    crc_2_calc = crc_2_calc + Check_Sum_Param(&input_frame[22],21,5);
    crc_2_calc = crc_2_calc + Check_Sum_Param(&input_frame[22],21,7);
    crc_2_calc = crc_2_calc + crc_1_str;

    //error check
    if(crc_1_str!=crc_1_calc)
    {
        for(i=0;i<10;i++)
        {
            output_frame[i]=0;
        }
        return 1;
    }
    //error check
    if(crc_2_str!=crc_2_calc)
    {
        for(i=0;i<10;i++)
        {
            output_frame[i]=0;
        }
        return 2;
    }

    size_id_1 = (input_frame[17] ^ (input_frame[9] + input_frame[10])) & 0x03;
    size_id_2 = (input_frame[39] ^ (input_frame[31] + input_frame[32])) & 0x03;

    //error check
    if(size_id_1!=size_id_2)
    {
        for(i=0;i<10;i++)
        {
            output_frame[i]=0;
        }
        return 3;
    }

    sum_pass_1 = input_frame[0] & 0xF0;
    sum_pass_1 = sum_pass_1 | ((input_frame[17] & 0xF0) >> 4);
    sum_pass_2 = input_frame[22] & 0xF0;
    sum_pass_2 = sum_pass_2 | ((input_frame[39] & 0xF0) >> 4);

    for(i=0;i<size_in;i++)
    {
        array_in[i] = ( input_frame[i+1] ^ input_frame[i+31] );
    }

    for(i=0;i<size_out;i++)
    {
        array_out[i] = ( input_frame[i+23] ^ input_frame[i+9] );
    }

    for(i=0;i<size_id_1;i++)
    {
        array_id_1[i] = ( input_frame[i+18] ^ (input_frame[(2*i)+11] + input_frame[(2*i+12)] ) );
    }

    for(i=0;i<size_id_2;i++)
    {
        array_id_2[i] = ( input_frame[i+40] ^ (input_frame[(2*i)+33] + input_frame[(2*i+34)] ) );
    }

    sum_pass_1_c=0;

    for(i=0;i<8;i++)
    {
        array_pass_1[i]=input_frame[i+9];
        sum_pass_1_c = sum_pass_1_c + input_frame[i+9];
    }

    for(i=0;i<8;i++)
    {
        array_pass_2[i]=input_frame[i+31];
        sum_pass_2_c = sum_pass_2_c + input_frame[i+31];
    }

    //error_check
    for(i=0;i<3;i++)
    {
        if(array_id_2[i]!=array_id_1[i])
        {
            for(i=0;i<10;i++)
            {
                output_frame[i]=0;
            }
            return 4;
        }
    }

    //error check
    if(sum_pass_1!=sum_pass_2_c)
    {
        for(i=0;i<10;i++)
        {
            output_frame[i]=0;
        }
        return 5;;
    }

    //error check
    if(sum_pass_2!=sum_pass_1_c)
    {
        for(i=0;i<10;i++)
        {
            output_frame[i]=0;
        }
        return 6;
    }

    in=str_to_long(&array_in[0]);
    out=str_to_long(&array_out[0]);
    id=str_to_long(&array_id_1[0]);

    tmp_1 = 0;
    tmp_2 = 0;
    tmp_3 = 0;
    tmp_4 = 0;
    tmp_5 = 0;

    for(i=0;i<8;i++)
    {
        array_pass_3[i] = array_pass_1[i] ^ array_pass_2[i];
    }

    for(i=0;i<4;i++)
    {
        array_pass_3_1[i] = array_pass_3[2*i] ^ array_pass_3[2*i+1];
    }

    //original parentheses
    //vigencia_1 = vigencia_1 ^ ((((array_pass_3_1[0] <<8) & 0xFF00) ^ (array_pass_3_1[1]) & 0xFF)) & 0x1FF;
    //vigencia_2 = vigencia_2 ^ ((((array_pass_3_1[2] <<8) & 0xFF00) ^ (array_pass_3_1[3]) & 0xFF)) & 0x1FF;
    //modded parentheses
    vigencia_1 = (vigencia_1 ^ (((array_pass_3_1[0] <<8) & 0xFF00) ^ ((array_pass_3_1[1]) & 0xFF))) & 0x1FF;
    vigencia_2 = (vigencia_2 ^ (((array_pass_3_1[2] <<8) & 0xFF00) ^ ((array_pass_3_1[3]) & 0xFF))) & 0x1FF;

    //error check
    if(vigencia_1!=vigencia_2)
    {
        for(i=0;i<10;i++)
        {
            output_frame[i]=0;
        }
        return 7;
    }

    for(i=0;i<8;i++)
    {
        tmp_2 = array_pass_2[i] & 0xFF;
        tmp_1 +=  tmp_2;
    }

    for(i=0;i<8;i++)
    {
        tmp_5=patern[i];
        tmp_2 = array_pass_1[i] & 0xFF;
        tmp_5=tmp_5 * tmp_2;
        tmp_5=tmp_5 ^ in;
        tmp_5=tmp_5 ^ out;
        tmp_5=tmp_5 ^ id;
        tmp_4=array_pass_2[i] & 0xFF;
        tmp_4=(array_pass_2[i] & 0xFF) * tmp_1;
        tmp_5=tmp_5 ^ tmp_4;
        key[i]=tmp_5;
    }

    key_2[0]= key[0] ^ key[1];
    key_2[1]= key[2] ^ key[3];
    key_2[2]= key[4] ^ key[5];
    key_2[3]= key[6] ^ key[7];
    key_3[0]= key_2[0] ^ key_2[1];
    key_3[1]= key_2[2] ^ key_2[3];
    tmp_1 = vigencia_1;
    //original parentheses
    //vig_key = ((0x9069 * (tmp_1 & 0x1FF)) << 8) & 0xFFFF0000 | (0xE271 * (tmp_1 & 0x1FF)) & 0xFFFF;
    //modded parentheses
    vig_key = (((0x9069 * (tmp_1 & 0x1FF)) << 8) & 0xFFFF0000) | ((0xE271 * (tmp_1 & 0x1FF)) & 0xFFFF);

    key_3[0] = key_3[0] ^ vig_key;
    key_3[1] = ~(key_3[1] ^ vig_key);

    for(i=0;i<32;i++)
    {
        output_frame[i]=0;
    }

    for(i=0;i<2;i++)
    {
        tmp_3=key_3[i];
        ptr= (uint8_t *) &tmp_3;
        for(j=0;j<4;j++)
        {
            output_frame[j+(i << 2)]=ptr[j];
        }
    }

    for(i=0;i<4;i++)
    {
        char_tmp1=output_frame[i];
        char_tmp2=input_frame[i+44];

        if((char_tmp1!=char_tmp2))
        {
            for(i=0;i<10;i++)
            {
                output_frame[i]=0;
            }
            return 8;
        }
    }

    for(i=0;i<4;i++)
    {
        char_tmp1=output_frame[i+4];
        char_tmp2=input_frame[i+48];
        if(char_tmp1!=char_tmp2)
        {
            for(i=0;i<10;i++)
            {
                output_frame[i]=0;
            }
            return 9;
        }
    }

    //if successful
    for(i=0;i<8;i++)
    {
        output_frame[i] = input_frame[i+56];
    }

    output_frame[8]=(vigencia_1 >> 8) & 0xFF;
    output_frame[9]=(vigencia_1) & 0xFF;

    for(i=0;i<10;i++)
    {
        out_tmp[i] = output_frame[i];
    }
    /// the cheat
    ptr = (uint8_t *) &date_of_reg;
    for(i=0;i<8;i++)
    {
        ptr[i]= out_tmp[i] & 0xFF;
    }

    dias_tmp = ((out_tmp[8] & 0xFF)<< 8) & 0x100;
    dias_tmp |= (out_tmp[9]) & 0xFF;
    //printf("  -> Dias de validacion: %llu\n",dias_tmp);

    // 86400 sec = 1 day
    dias_tmp = dias_tmp * 86400;
    date_of_reg = date_of_reg + dias_tmp;
    qDebug() << "Date of reg de salida: " << date_of_reg;
    qDebug() << "Date of reg de salida: " << date_of_reg;

    return 0;
}

/**
 * @brief read
 * @return int res
 */
int ReadTinkerForm::read()
{

    //try
   // {

        uint8_t serialnum[ATCA_SERIAL_NUM_SIZE];
        ATCA_STATUS status;
        bool is_locked = false;
        int i;

        // Configuro los parametros de la ATECC508A
        cfg_ateccx08a_i2c_default.atcai2c.slave_address  = 0xC0;
        cfg_ateccx08a_i2c_default.atcai2c.bus = 9;
        cfg_ateccx08a_i2c_default.atcai2c.baud= 400000;

        status = atcab_init(&cfg_ateccx08a_i2c_default);
        if (status != ATCA_SUCCESS)
        {
            qDebug("atcab_init() no exitoso ret=0x%08X\r\n", status);
            return status;
        }

        atcab_is_locked(LOCK_ZONE_CONFIG, &is_locked);
        if (!is_locked)
        {
            qDebug() << "Zona de configuracion desbloqueada. Random y otras funciones no correran";
        }
        else
        {
            qDebug() << "Zona de configuracion bloqueada.";
        }

        atcab_is_locked(LOCK_ZONE_DATA, &is_locked);
        if (!is_locked)
        {
              qDebug() << "Zona de datos desbloqueada.";
        }
        else
        {
               qDebug() << "Zona de datos bloqueada.";
        }

        char Buffer[8];
        string hardserial;

        // Leo el numero serial de la ATECC508A
        status = atcab_read_serial_number(serialnum);

        if (status != ATCA_SUCCESS)
        {
              qDebug("atcab_read_serial_number() no exitoso ret=0x%08X\r\n", status);
        }
        else
        {

              for(i=0;i<ATCA_SERIAL_NUM_SIZE;i++)
              {
                   //printf("0x%02X ", serialnum[i]);
                   //printf("%d ",hardserialTMP[i]);
                   if( i == 8 )
                         sprintf( Buffer, "%d",  serialnum[i] );
                    else
                         sprintf( Buffer, "%d.",  serialnum[i] );

                    hardserial.append( Buffer );

                }
                //printf("\r\n");
        }

        qDebug() << "Hardserial: " << hardserial.c_str();

        if(hardserial == "")
        {
            qDebug() << "error hardserial";
             QMessageBox::warning(this, "HardSerial Error", "HardSerial Vacio");
            return 0;
        }

        if( Fram::fram_open("/dev/ttyUSB0") != BP_SUCCESS )
        {
          QMessageBox::warning(this, "FRAM Error", "FRAM no se puede abrir el dispositivo");
          return 0;
        }

        //Verificamos que haya una FRAM valida en el puerto I2C
        qDebug() << "Comienza inicializacion FRAM";
        if(Fram::fram_is_present() != BP_SUCCESS)
        {
            QMessageBox::warning(this, "FRAM Error", "FRAM no detectada");
            //qDebug() << "FRAM no detectada";
            return 0;
        }

        qDebug() << "Start Storage";

        Storage::Class gameStorage(&sequencer, 16000,0);
        Storage::Class systemStorage(&sequencer, 512,gameStorage.getMaxPhysicalSize());

        qDebug() << "ServerSelected: " << ServerSelected;
        /*switch (ServerSelected)
        {
            //Calabaza Chile
            case 1:
            {

                uint16_t size;
                systemStorage.startReading(size,buffer);
                systemStorage.readData(&systemStateOld,size);

                qDebug() << "MachineId: " << systemStateOld.MachineId;
                qDebug() << "gameId: " << systemStateOld.gameId;
                qDebug() << "Entrada: " << systemStateOld.in;
                qDebug() << "Salida: " << systemStateOld.out;
                qDebug() << "MAC: " << hardserial.c_str();

                ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                ui->tableWidget->setItem(0, 0, new QTableWidgetItem(QString("In")));
                ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(systemStateOld.in)));
                ui->tableWidget->setItem(1, 0, new QTableWidgetItem(QString("Out")));
                ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::number(systemStateOld.out)));
                ui->tableWidget->setItem(2, 0, new QTableWidgetItem(QString("MachineId")));
                ui->tableWidget->setItem(2, 1, new QTableWidgetItem(QString::number(systemStateOld.MachineId)));
                ui->tableWidget->setItem(3, 0, new QTableWidgetItem(QString("GameId")));
                ui->tableWidget->setItem(3, 1, new QTableWidgetItem(QString::number(systemStateOld.gameId)));
                ui->tableWidget->setItem(4, 0, new QTableWidgetItem(QString("Mac")));
                ui->tableWidget->setItem(4, 1, new QTableWidgetItem(QString(hardserial.c_str())));
                MachineIdToPrint = QString::number(systemStateOld.MachineId);
                GameIdToPrint = QString::number(systemStateOld.gameId);

                break;
            }
            //Gecko Brasil
            case 2:
            {*/

                uint16_t size;
                systemStorage.startReading(size,buffer);
                systemStorage.readData(&systemState,size);

                qDebug() << "MachineId: " << systemState.machineId;
                qDebug() << "gameId: " << systemState.gameId;
                qDebug() << "Entrada: " << systemState.in;
                qDebug() << "Salida: " << systemState.out;
                qDebug() << "MAC: " << hardserial.c_str();

                DateTime timeStamp;
                timeStamp = systemState.timeStamp;
                QString bufTS;
                bufTS = QString::number(timeStamp.hrs) + ":" +  QString::number(timeStamp.min) + ":" +  QString::number(timeStamp.seg) + " " + QString::number(timeStamp.day) + "/" + QString::number(timeStamp.month + 1) + "/" + QString::number(timeStamp.year + 1900);

                DateTime tinkerDate;
                tinkerDate = systemState.tinkerDate;
                QString bufTD;
                bufTD = QString::number(tinkerDate.hrs) + ":" +  QString::number(tinkerDate.min) + ":" +  QString::number(tinkerDate.seg) + " " + QString::number(tinkerDate.day) + "/" + QString::number(tinkerDate.month + 1) + "/" + QString::number(tinkerDate.year + 1900);

                DateTime dateNow;
                bool resultGetTime = Rtc::getTime( dateNow );
                if( resultGetTime == false)
                {
                    qDebug() << "ERRO RTC";
                    QMessageBox::warning(this, "RTC Error", "no se pudo obtener datos del rtc");
                    return 0;
                }

                QString bufRTC;
                bufRTC = QString::number(dateNow.hrs) + ":" +  QString::number(dateNow.min) + ":" +  QString::number(dateNow.seg) + " " + QString::number(dateNow.day) + "/" + QString::number(dateNow.month + 1) + "/" + QString::number(dateNow.year + 1900);
                qDebug("dateNow %02d:%02d:%02d  %02d/%02d/%04d", dateNow.hrs, dateNow.min, dateNow.seg, dateNow.day, dateNow.month + 1, dateNow.year + 1900);
                //QTextStream(&bufRTC) << "";
                qDebug() << "bufRTC: " << bufRTC;

                uint64_t Vigencia_value = 0;
                //QString bufX;
                Get_Vigencia_From_Tinker(systemState.file10, Vigencia_value);
                //bufX.append(Vigencia_value);
                qDebug() << "Vigencia_value: " << Vigencia_value;
                time_t convertExpired = time_t(Vigencia_value);
                struct tm tX;
                char bufX[80];
                tX = *localtime(&convertExpired);
                if( Vigencia_value == 0 )
                {
                    char str1[]=" Nunca ha sido validado.";
                    strcpy (bufX,str1);
                }
                else
                {
                    strftime(bufX, sizeof(bufX), "%H:%M:%S %d/%m/%Y", &tX);
                }

                qDebug() << "Vigencia: " << bufX;

                ui->tableWidget->insertRow(ui->tableWidget->rowCount());
                ui->tableWidget->setItem(0, 0, new QTableWidgetItem(QString("In")));
                ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::number(systemState.in)));
                ui->tableWidget->setItem(1, 0, new QTableWidgetItem(QString("Out")));
                ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::number(systemState.out)));
                ui->tableWidget->setItem(2, 0, new QTableWidgetItem(QString("MachineId")));
                ui->tableWidget->setItem(2, 1, new QTableWidgetItem(QString::number(systemState.machineId)));
                ui->tableWidget->setItem(3, 0, new QTableWidgetItem(QString("GameId")));
                ui->tableWidget->setItem(3, 1, new QTableWidgetItem(QString::number(systemState.gameId)));
                ui->tableWidget->setItem(4, 0, new QTableWidgetItem(QString("Mac")));
                ui->tableWidget->setItem(4, 1, new QTableWidgetItem(QString(hardserial.c_str())));
                ui->tableWidget->setItem(5, 0, new QTableWidgetItem(QString("Gravacion Tinker")));
                ui->tableWidget->setItem(5, 1, new QTableWidgetItem(QString(bufTD)));
                ui->tableWidget->setItem(6, 0, new QTableWidgetItem(QString("Hora Tinker")));
                ui->tableWidget->setItem(6, 1, new QTableWidgetItem(QString(bufTS)));
                ui->tableWidget->setItem(7, 0, new QTableWidgetItem(QString("RTC")));
                ui->tableWidget->setItem(7, 1, new QTableWidgetItem(QString(bufRTC)));
                ui->tableWidget->setItem(8, 0, new QTableWidgetItem(QString("Vigencia")));
                ui->tableWidget->setItem(8, 1, new QTableWidgetItem(QString(bufX)));
                //ui->tableWidget->setItem(8, 0, new QTableWidgetItem(QString("Periodo")));
                //ui->tableWidget->setItem(8, 1, new QTableWidgetItem(QString::number(systemState.period)));
                ui->tableWidget->setItem(9, 0, new QTableWidgetItem(QString("Validaciones")));
                ui->tableWidget->setItem(9, 1, new QTableWidgetItem(QString::number(systemState.validations)));
                ui->tableWidget->setItem(10, 0, new QTableWidgetItem(QString("Configuracion")));
                ui->tableWidget->setItem(10, 1, new QTableWidgetItem(QString::number(systemState.config)));
                //ui->tableWidget->setItem(10, 0, new QTableWidgetItem(QString("Excepción")));
                //ui->tableWidget->setItem(10, 1, new QTableWidgetItem(QString::number(systemState.exception)));
                MachineIdToPrint = QString::number(systemState.machineId);
                GameIdToPrint = QString::number(systemState.gameId);

                qDebug() << systemState.keyPass;

               // break;

            //}
        //}

        // Cerrando bus pirate
        if( Fram::fram_close() != BP_SUCCESS )
        {
            QMessageBox::warning(this, "FRAM Error", "no se puede cerrar el dispositivo");
            return 0;
        }

    return 1;
}

int ReadTinkerForm::eraseTMSP( DateTime nowDate )
{

    qDebug() << "Erase TMSP";

    if( Fram::fram_open("/dev/ttyUSB0") != BP_SUCCESS )
    {
      QMessageBox::warning(this, "FRAM Error", "FRAM no se puede abrir el dispositivo");
      return 0;
    }

    //Verificamos que haya una FRAM valida en el puerto I2C
    qDebug() << "Comienza inicializacion FRAM";
    if(Fram::fram_is_present() != BP_SUCCESS)
    {
        QMessageBox::warning(this, "FRAM Error", "FRAM no detectada");
        //qDebug() << "FRAM no detectada";
        return 0;
    }

    qDebug() << "Start Storage";

    Storage::Class gameStorage(&sequencer, 16000,0);
    Storage::Class systemStorage(&sequencer, 1024,gameStorage.getMaxPhysicalSize());

    memset(&systemState,0,sizeof(systemState));

    uint16_t size;
    systemStorage.startReading(size,buffer);
    systemStorage.readData(&systemState,size);

    systemState.in = systemState.in;
    systemState.out = systemState.out;

    systemState.machineId = systemState.machineId;
    systemState.gameId = systemState.gameId;
    memset(systemState.file10,0,64);
    systemState.period = systemState.period;
    systemState.validations = systemState.validations;
    systemState.config = systemState.config;
    systemState.keyPass = systemState.keyPass;
    systemState.timeStamp = nowDate;
    systemState.tinkerDate = systemState.tinkerDate;


    systemStorage.startUpdate(sizeof(systemState),1,buffer);

    systemStorage.addData(&systemState,sizeof(systemState));

    systemStorage.finalizeUpdate(buffer);

    return 1;

}

void ReadTinkerForm::on_Read_Button_clicked()
{

    Usb0StartWatch();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    if(Usb0FolderExists())
    {
        ui->PrintLabel_Button->setEnabled(false);
        ui->WriteRtc_Button->setEnabled(false);
        if(FtdiLatencyModifier())
        {
            if(read())
            {
                ui->PrintLabel_Button->setEnabled(true);
                ui->WriteRtc_Button->setEnabled(true);
                //QMessageBox::information(this, "Read TinkerBoard", "Lectura correcta de TinkerBoard");
                qDebug() << "Lectura correcta";
            }
            else
            {
                QMessageBox::warning(this, "Error", "Error de lectura de TinkerBoard");
            }
        }
        else
        {
            QMessageBox::warning(this, "Error FTDI", "Error en la comunicacion a TinkerBoard");
        }
    }
    else
    {
        QMessageBox::warning(this, "Error Bus Pirate", "Error No se encuentra el dispositivo ttyUSB0");
    }

    QApplication::restoreOverrideCursor();
}


void ReadTinkerForm::on_PrintLabel_Button_clicked()
{
    QString ReturnValuePrintPs;
    std::string DayMonthYearToPrintStd;
    DayMonthYearToPrintStd = GetStdoutFromCommand("date +\"%d/%m/%y\"");
    DayMonthYearToPrint = DayMonthYearToPrintStd.c_str();

    if(printer.WritePs("  ",GameIdToPrint,MachineIdToPrint,DayMonthYearToPrint) == false){
        QMessageBox::warning(this, "Error de impresion", "Error en la creacion del archivo a imprimir");
        return;
    }
    else
    {
        ReturnValuePrintPs = printer.PrintPs();
        if(ReturnValuePrintPs == "complete")
        {
            QMessageBox::information(this, "Etiqueta Tinker", "Impresion Completa");
        }
        else
        {
            QMessageBox::critical(this, "Error de impresion", ReturnValuePrintPs);
        }
    }

}

void ReadTinkerForm::on_WriteRtc_Button_clicked()
{

    QMessageBox msgBox;
    msgBox.setWindowTitle("Reloj RTC");
    msgBox.setText("¿Quieres ajustar el reloj? ");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if( ret == 1024 )
    {

        Usb0StartWatch();

        QApplication::setOverrideCursor(Qt::WaitCursor);
        if(Usb0FolderExists())
        {
            ui->PrintLabel_Button->setEnabled(true);
            ui->WriteRtc_Button->setEnabled(true);
            ui->Read_Button->setEnabled(true);
            qDebug() << " Si ";

            //Date time
            time_t rawtime;
            struct tm * timeinfo;
            time ( &rawtime ); //or: rawtime = time(0);
            timeinfo = localtime ( &rawtime );
            DateTime date;

            date.seg = uint8_t(timeinfo->tm_sec);
            date.min = uint8_t(timeinfo->tm_min);
            date.hrs = uint8_t(timeinfo->tm_hour);
            date.day = uint8_t(timeinfo->tm_mday);
            date.month = uint8_t(timeinfo->tm_mon);
            date.year = uint8_t(timeinfo->tm_year);

            qDebug("dateNow %02d:%02d:%02d  %02d/%02d/%04d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year + 1900);
            qDebug("dateNow %02d:%02d:%02d  %02d/%02d/%04d", date.seg, date.min, date.seg, date.day, date.month, date.year + 1900);

            int result = Rtc::setTime(date);
            qDebug() << " result: " << result;
            if( result > 0 )
            {
                qDebug() << " Relogio ajustado ";

                tm tmValue;
                /*while ( Rtc::getTime( tmValue, true ) == false )
                {
                    qDebug() << "Error getTime 1 *****************";
                    usleep(100);
                }*/
                time_t convertTimeRTC;
                convertTimeRTC = mktime(&tmValue);

                qDebug() << " Apagando memoria: ";
                if( eraseTMSP( date ) == 1 )
                {

                    if(FtdiLatencyModifier())
                    {
                        if(read())
                        {
                            ui->PrintLabel_Button->setEnabled(true);
                            ui->WriteRtc_Button->setEnabled(true);
                            ui->Read_Button->setEnabled(true);
                            //QMessageBox::information(this, "Read TinkerBoard", "Lectura correcta de TinkerBoard");
                            qDebug() << "Lectura correcta";
                        }
                        else
                        {
                            QMessageBox::warning(this, "Error", "Error de lectura de TinkerBoard");
                        }
                    }
                    else
                    {
                        QMessageBox::warning(this, "Error FTDI", "Error en la comunicacion a TinkerBoard");
                    }

                }
                else
                {

                    ui->PrintLabel_Button->setEnabled(true);
                    ui->WriteRtc_Button->setEnabled(false);
                    ui->Read_Button->setEnabled(false);

                }

            }
            else
            {
                qDebug() << " Erro saporra ";
            }
        }
        else
        {
            QMessageBox::warning(this, "Error Bus Pirate", "Error No se encuentra el dispositivo ttyUSB0");
        }

        QApplication::restoreOverrideCursor();

    }
    else
    {
         qDebug() << " No ";
    }
}


/**
 * @brief WriteTinkerForm::FtdiLatencyModifier
 * @return
 */
int ReadTinkerForm::FtdiLatencyModifier(){
    std::string cmdmod;
    std::string cmdcheck;
    int res;

    cmdmod = "echo 1 > /sys/bus/usb-serial/devices/ttyUSB0/latency_timer";
    cmdcheck = "cat /sys/bus/usb-serial/devices/ttyUSB0/latency_timer";

    QString ParsedOut = GetStdoutFromCommand(cmdcheck).c_str();

    qDebug() << "first current latency: " << ParsedOut;
    if(ParsedOut == "16\n"){
        GetStdoutFromCommand(cmdmod);
    }

    QString ParsedOut2 = GetStdoutFromCommand(cmdcheck).c_str();
    qDebug() << "second current latency: " << ParsedOut2;
    if(ParsedOut2 == "1\n"){
        qDebug() << "latency correctly modified";
        res = 1;
    }else{
        qDebug() << "error modifying latency";
        res = 0;
    }
    return res;
}

/**
 * @brief WriteTinkerForm::GetStdoutFromCommand
 * @param cmd
 * @return
 */
std::string ReadTinkerForm::GetStdoutFromCommand(std::string cmd)
{
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.std::string::c_str(), "r");
    if (stream)
    {
        while (!feof(stream))
        {
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        }
        pclose(stream);
    }
    else
    {
        QMessageBox::warning(this, "Error", "Problema al ejecutar Sub Proceso");
    }
    return data;
}

bool ReadTinkerForm::Usb0StartWatch()
{
    if(Usb0FolderExists() && !alreadyWatched)
    {
        watcher.addPath("/dev/serial");
        QObject::connect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(Usb0DisconnectedSlot(QString)));
        alreadyWatched = true;
    }
    else if(!alreadyWatched)
    {
        QMessageBox::critical(this, "Error Bus Pirate", "el modulo BusPirate se encuentra desconectado");
    }
}

bool ReadTinkerForm::Usb0FolderExists(){
    bool result;
    QFile Usb0Folder("/dev/ttyUSB0");
    result = Usb0Folder.exists();
    return result;
}
