#include "writetinkerform.h"
#include "ui_writetinkerform.h"
#include "login.h"

/*************************************************************
***
*** Este codigo re-utiliza codigo heredado del
*** software STARTTINKER y READTINKER
*** Para mayor informacion, revisar codigo fuente en github
*** https://github.com/InnovoGaming9000/Produccion_Tinker
***
**************************************************************/

WriteTinkerForm::WriteTinkerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WriteTinkerForm)
{
    ui->setupUi(this);

    ProgressBarHide();
    setGameList();
    ui->PrintLabel_Button->setEnabled(false);
    setValoresPeriodo();
    setConfigList();

    qDebug() << "Construtor - Usb0StartWatch";
    if( Usb0StartWatch() )
    {
        /*
        tm tmValue;
        while ( getTime( tmValue, true ) == false )
        {
            qDebug() << "Error getTime 1 *****************";
            usleep(100);
        }

        convertTimeRTC = mktime(&tmValue);
        qDebug() << "convertTimeRTC: " << convertTimeRTC;
        */
        //dateNow = Rtc::getTime();
        //qDebug("dateNow %02d:%02d:%02d  %02d/%02d/%04d", dateNow.hrs, dateNow.min, dateNow.seg, dateNow.day, dateNow.month + 1, dateNow.year + 1900);
    }
    qDebug() << "Construtor2 - Usb0StartWatch";
}


WriteTinkerForm::~WriteTinkerForm()
{
    delete ui;
}

/**
 * @brief WriteTinkerForm::on_Grabar_Button_clicked
 */
void WriteTinkerForm::on_Grabar_Button_clicked()
{
    QString ActContent;
    QString GameNameSelected = ui->comboBox_GameName->currentText();
    int GameIdSelected = getGameId(GameNameSelected);

    ui->PrintLabel_Button->setEnabled(false);
    ProgressBarReset();
    ProgressBarShow();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    Usb0StartWatch();

    if(Usb0FolderExists())
    {
        if(FtdiLatencyModifier())
        {
            if(write(GameIdSelected))
            {
                ActContent = "Make TinkerBoard Sucess: ";
                ActContent.append("GameId:").append(ToLogGameId);
                ActContent.append(" ,MachineId: ").append(ToLogMachineId);
                ActContent.append(" ,Periodo: ").append(ToLogPeriodo);
                ActContent.append(" ,HardSerial: ").append(ToLogHardSerial);
                LogAction(ActContent, UserId , TerminalIp, VmId);
                GameIdToPrint = ToLogGameId;
                MachineIdToPrint = ToLogMachineId;
                ui->PrintLabel_Button->setEnabled(true);
                QMessageBox::information(this, "Write TinkerBoard", "Escritura correcta de TinkerBoard");

            }
            else
            {
                QMessageBox::warning(this, "Error Escritura", "Error en escritura TinkerBoard");
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

    ProgressBarHide();
    QApplication::restoreOverrideCursor();
}

/**
 * @brief WriteTinkerForm::setTime
 * @param bp
 * @param dev_addr
 * @param newDate
 * @return
 */
/*
int WriteTinkerForm::setTime(DateTime newDate)
{

    int resultTime = 1;

    BP * bp;
    unsigned char version;
    unsigned char dev_addr = _DS3231_ADDRESS;

    bp= bp_open("/dev/ttyUSB0");
    if (!bp)
    {
        QMessageBox::warning(this, "Error", "No se puede conectar a Bus Pirate.");
        return 0;
    }

    bp_firmware_version_high(bp);
    bp_firmware_version_low(bp);


    if (bp_bin_init(bp, &version) != BP_SUCCESS)
    {
        //error("No puedo cambiar a modo binario");
        QMessageBox::warning(this, "Error", "No puedo cambiar a modo binario");
        return 0;
    }

    usleep(100);


    if (bp_bin_mode_i2c(bp, &version) != BP_SUCCESS)
    {
        //error("No puedo inicializar modo I2C");
        QMessageBox::warning(this, "Error", "No puedo inicializar modo I2C");
        return 0;
    }

    usleep(100);

    //Es muy importante esta velocidad ya que al usar cable plano velocidades superiores da error
    if (bp_bin_i2c_set_speed(bp, BP_BIN_I2C_SPEED_5K) < 0)
    {
        return -1;
    }

    usleep(100);

    if (bp_bin_i2c_set_periph(bp, (BP_BIN_I2C_PERIPH_POWER)) < 0)
    {
        return -1;
    }

    usleep(100);


    if ( newDate.seg > 59 || newDate.min > 59 || newDate.hrs > 23  )
    {
        qDebug("ERROR SET DATETIME seg:%d, min:%d, hrs:%d", newDate.seg, newDate.min, newDate.hrs);
        resultTime = 0;
    }

    ds3231_write(bp, dev_addr, 0, newDate.seg);
    ds3231_write(bp, dev_addr, 1, newDate.min);
    ds3231_write(bp, dev_addr, 2, newDate.hrs);
    ds3231_write(bp, dev_addr, 4, newDate.day);
    ds3231_write(bp, dev_addr, 5, newDate.month);
    ds3231_write(bp, dev_addr, 6, newDate.year);

    bp_close( bp );

    return resultTime;
}
*/



/**
 * @brief WriteTinkerForm::cripto
 * @return
 */
int WriteTinkerForm::cripto()
{
    const uint16_t public_key_id = 14;
    const uint16_t private_key_id = 0;
    uint8_t random_number[32];
    uint8_t serialnum[ATCA_SERIAL_NUM_SIZE];
    uint8_t revision[4];
    ATCA_STATUS status;
    bool is_locked = false;
    int i;
    static const uint8_t private_key[36] = {
        0x00, 0x00, 0x00, 0x00,
        0xEB, 0xF4, 0xEC, 0x7D, 0xA5, 0x9E, 0x61, 0x8D, 0x8D, 0x56, 0x87, 0xD2, 0xF1, 0xCE, 0xF5, 0xB2,
        0x5E, 0xFF, 0x95, 0xE7, 0x4D, 0x7A, 0x58, 0xF5, 0x02, 0x4E, 0x8F, 0xFB, 0xF0, 0x79, 0x6E, 0x07
    };

    static const uint8_t public_key_ref[64] = {
        0xE5, 0xF7, 0x27, 0xAC, 0x9B, 0xC3, 0xC3, 0x4A, 0x01, 0x54, 0x6A, 0x7B, 0xFF, 0x9D, 0x79, 0x5A,
        0xA6, 0xF6, 0x13, 0xBB, 0x40, 0x98, 0xAA, 0xEF, 0x01, 0x31, 0x68, 0xAD, 0xDE, 0xA6, 0x7B, 0x45,
        0x50, 0xA6, 0x5D, 0x6B, 0xF3, 0xFA, 0x17, 0x79, 0xF2, 0xB3, 0x87, 0x66, 0x85, 0xFA, 0x2F, 0xC8,
        0x92, 0xBD, 0x6F, 0xF3, 0x05, 0x85, 0xC8, 0x5F, 0xEB, 0x8E, 0xD7, 0x8C, 0xF5, 0x8C, 0x44, 0x64
    };
    uint8_t public_key[ATCA_PUB_KEY_SIZE];
    uint8_t message[ATCA_KEY_SIZE];
    uint8_t signature[ATCA_SIG_SIZE];
    uint8_t pubkey[ATCA_PUB_KEY_SIZE];

    // Verifico si la zona de configuracion esta bloqueada
    atcab_is_locked(LOCK_ZONE_CONFIG, &is_locked);
    if (!is_locked)
    {
        qDebug() << "Zona de configuracion desbloqueada. Random y otras funciones no correran";
    }
    else
    {
        qDebug() << "Zona de configuracion bloqueada";
    }

    // Verifico si la zona de datos esta bloqueada
    atcab_is_locked(LOCK_ZONE_DATA, &is_locked);
    if (!is_locked)
    {
        qDebug() << "Zona de datos desbloqueada";
    } else
    {
        qDebug() << "Zona de datos bloqueada";
    }

    //read_config(&cfg_ateccx08a_i2c_default);
    //gen_random(&cfg_ateccx08a_i2c_default);

    // Bloqueo la zona de configuracion. Esto es importante hacerlo la
    // primera vez, sino no se generan numeros aleatorios
    status = atcab_lock_config_zone();
    if (status != ATCA_SUCCESS)
    {
        qDebug("atcab_lock_config_zone() no exitoso with ret=0x%08X", status);
    }

    // Escribo una clave privada en slot 0
    status = atcab_priv_write(private_key_id, private_key, 0, NULL);
    if (status != ATCA_SUCCESS)
    {
        qDebug("atcab_priv_write() no exitoso with ret=0x%08X", status);
    }

    // Obtengo la clave publica generada

    status = atcab_get_pubkey(private_key_id, public_key);
    if (status != ATCA_SUCCESS)
    {
        qDebug("atcab_get_pub_key() no exitoso with ret=0x%08X", status);
    }

    // Guardo la clave publica en un slot distinto al de la clave privada
    status = atcab_write_pubkey(public_key_id, public_key);
    if (status != ATCA_SUCCESS)
    {
        qDebug("atcab_write_pubkey() no exitoso with ret=0x%08X", status);
    }

    //gen_random(&cfg_ateccx08a_i2c_default);

    // Lo firmo con la clave privada y genero una firma
    status = atcab_sign(private_key_id, message, signature);
    if (status != ATCA_SUCCESS)
    {
        qDebug("atcab_sign() no exitoso with ret=0x%08X", status);
    }

}

/**
 * @brief WriteTinkerForm::verify
 * @return int result
 */
int WriteTinkerForm::verify()
{
    ATCA_STATUS status;
    bool is_verified = false;
    const uint16_t public_key_id = 14;
    const uint16_t private_key_id = 0;

    uint8_t pubkey[ATCA_PUB_KEY_SIZE];
    static const uint8_t message[ATCA_KEY_SIZE] = {
        0x3C, 0x58, 0x07, 0x3A, 0xE3, 0x75, 0x2A, 0x51, 0x2C, 0x96,
        0x92, 0x93, 0xBF, 0x79, 0x82, 0x8D, 0x25, 0x61, 0x60, 0x31,
        0x2F, 0xBE, 0xD4, 0x78, 0xD3, 0x2D, 0x3B, 0x90, 0x55, 0x56,
        0xA5, 0xAE
    };
    static const uint8_t signature[ATCA_SIG_SIZE] = {
        0x0E, 0x60, 0x1D, 0x88, 0xB0, 0xC1, 0x93, 0x59, 0x16, 0xF1,
        0x29, 0x48, 0x5D, 0x95, 0x28, 0x08, 0x9B, 0x03, 0xB6, 0x1A,
        0x11, 0x1F, 0xB6, 0x80, 0xF0, 0x09, 0xBB, 0x83, 0xD4, 0x86,
        0xFC, 0xEA, 0x59, 0x60, 0xFE, 0x6B, 0xBC, 0x57, 0x8C, 0xFE,
        0x15, 0x35, 0xAD, 0xCF, 0x93, 0xFD, 0xE1, 0x01, 0x74, 0x56,
        0x47, 0x59, 0x97, 0x3B, 0xF0, 0x81, 0xE0, 0x75, 0xFD, 0xBE,
        0x31, 0xEA, 0xE0, 0x33
    };

    // Obtengo la clave publica generada
    status = atcab_get_pubkey(private_key_id, pubkey);
    if (status != ATCA_SUCCESS)
    {
        qDebug("atcab_get_pub_key() no exitoso with ret=0x%08X", status);
        return 0;
    }

    // Se verifica
    status = atcab_verify_extern(message, signature, pubkey, &is_verified);

    if (status != ATCA_SUCCESS)
    {
        qDebug("atcab_verify_extern() no exitoso with ret=0x%08X", status);
        return 0;
    }


    if(is_verified)
    {
        qDebug() << "Mensaje valido. Mensaje correcto";
        return 1;
    }
    else
    {
        qDebug() << "Mensaje invalido. Mensaje incorrecto";
        return 0;
    }
}

/**
 * @brief WriteTinkerForm::write
 * @param IdGameToUse
 * @return int final_resultHardserial
 *Hardserial
 * esta funcion debe ser optimizada
 * por cuestiones de tiempo se dejara asi por el momento.
 * ->separar las consultas sql en otras funciones
 * ->separar las acciones de grabado de tinkerboard en otras funciones
 */
int WriteTinkerForm::write(int IdGameToUse)
{
    Periodo = getValoresPeriodo(ui->comboBox_Periodo->currentText());
    qDebug() << "Dias de periodo seleccionado: " << Periodo;

    QString GameConfigSelected = ui->comboBox_Config->currentText();
    qDebug() << "GameConfigSelected: " << GameConfigSelected;
    config = getConfig(GameConfigSelected);
    qDebug() << "Config seleccionado: " << config;

    if( !IdGameToUse )
    {
        qDebug() << "Error, no GameID";
        return 0;
    }

    //Date time
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime ); //or: rawtime = time(0);
    timeinfo = localtime ( &rawtime );
    DateTime dateNow;

    dateNow.seg = uint8_t(timeinfo->tm_sec);
    dateNow.min = uint8_t(timeinfo->tm_min);
    dateNow.hrs = uint8_t(timeinfo->tm_hour);
    dateNow.day = uint8_t(timeinfo->tm_mday);
    dateNow.month = uint8_t(timeinfo->tm_mon);
    dateNow.year = uint8_t(timeinfo->tm_year);

   // uint8_t random_number[32];
    uint8_t serialnum[ATCA_SERIAL_NUM_SIZE];
   // uint8_t revision[4];
    ATCA_STATUS status;
    bool is_locked = false;
    int i;

    // Configuro los parametros de la ATECC508A
    cfg_ateccx08a_i2c_default.atcai2c.slave_address  = 0xC0;
    cfg_ateccx08a_i2c_default.atcai2c.bus = 9;
    cfg_ateccx08a_i2c_default.atcai2c.baud= 400000;
    ProgressBarChange(2);

    // Inicializo la ATECC508A
    status = atcab_init(&cfg_ateccx08a_i2c_default);
    if (status != ATCA_SUCCESS)
    {
        qDebug("atcab_init() no exitoso ret=0x%08X\r\n", status);
        return status;
    }
    ProgressBarChange(5);

    clock_t begin, end;
    double time_spent;

    begin = clock();
    cripto();
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    qDebug() << " -> cripto time: " << time_spent;
    //printf(">>cripto time: %lf \r\n", time_spent);

    // Verifico si la zona de configuracion esta bloqueada
    atcab_is_locked(LOCK_ZONE_CONFIG, &is_locked);
    if (!is_locked)
    {
        qDebug() << "Zona de configuracion desbloqueada. Random y otras funciones no correran";
    }
    else
    {
        qDebug() << "Zona de configuracion bloqueada.";
    }
    ProgressBarChange(10);

    // Verifico si la zona de datos esta bloqueada
    atcab_is_locked(LOCK_ZONE_DATA, &is_locked);
    if (!is_locked)
    {
        qDebug() << "Zona de datos desbloqueada.";
    }
    else
    {
        qDebug() << "Zona de datos bloqueada.";
    }
    ProgressBarChange(15);

    //useless cast, but keeping it for safety(and mysticism)
    GameId = IdGameToUse;
    qDebug() << " -> GameId: " << GameId;
    MachineId = 0;

    //Get VersionPass
    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT GamesVersions.PassVersion, PassVersions.intVersion FROM GamesVersions INNER JOIN PassVersions ON GamesVersions.PassVersion = PassVersions.PassVersion WHERE GamesVersions.GameId = :GameId ");
        query.bindValue(":GameId", GameId);
        if(query.exec())
        {
            qDebug() << "query ok, GameId: " << GameId;
            if(query.next())
            {
                keyPass = query.value("intVersion").toULongLong();
                passVersion = query.value("PassVersion").toString();
                qDebug() << "keyPass: " << keyPass << ", PassVersion: " << passVersion;
            }
            else
            {
                qDebug() << "Nothing...";
            }
        }
        else
        {
            qDebug() << "query VersionPass fail";
            //return 0;
        }
    }
    else
    {
        qDebug() << "No se pudo conectar al servidor";
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
        return 0;
    }

    char Buffer[2];
    string hardserial;

    // Leo el numero serial de la ATECC508A
    status = atcab_read_serial_number(serialnum);
    //printf("ATCA_SERIAL_NUM_SIZE%d\n", ATCA_SERIAL_NUM_SIZE );
    if (status != ATCA_SUCCESS)
    {
        qDebug("atcab_read_serial_number() no exitoso ret=0x%08X\r\n", status);
    }
    else
    {
        //printf("Imprimiendo numero serial\r\n");
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
    ProgressBarChange(20);

    //printf("Verificacion: %u\r\n", verify());

    qDebug() << "verificacion: " << verify();
    if(verify())
    {
        qDebug() << "Verificacion Ok";
    }
    else
    {
        QMessageBox::warning(this,"Error","Error de verificacion de seguridad, contactar a soporte.");
        return 0;
    }

    ProgressBarChange(30);

    //printf("Hardserial:%s\n",hardserial.c_str());
    qDebug() << "Hardserial: " << hardserial.c_str();
    if(hardserial == "")
    {
        qDebug() << "error hardserial";
        QMessageBox::warning(this, "HardSerial Error", "HardSerial Vacio");
        return 0;
    }

    if (conn.dbDongle2.open() == true)
    {
        QSqlQuery query(conn.dbDongle2);
        query.prepare("SELECT MachineId from StreetDongle WHERE active = 1 AND HardSerial= :hardserial");
        query.bindValue(":hardserial", hardserial.c_str());

        if(query.exec())
        {
            qDebug() << "query ok, hardserial: " << hardserial.c_str();
            if(query.next())
            {
                unsigned int machineId;
                machineId = query.value("MachineId").toUInt();
                qDebug() << "MachineId: " << machineId;
                QMessageBox::warning(this, "Error, in use.", "Device registered on another server.");
                return 0;
            }
            else
            {
                qDebug() << "No errors, free device, continue...";
                //return 0;
            }
        }
        else
        {
            qDebug() << "query fail";
            return 0;
        }
    }
    else
    {
        qDebug() << "No se pudo conectar a gecko brasil";
        QString dblasterr2 = conn.dbDongle2.lastError().text();
        dbConnectionError(dblasterr2);
        return 0;
    }

    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT MachineId, active from StreetDongle WHERE HardSerial= :hardserial");
        query.bindValue(":hardserial", hardserial.c_str());

        if(query.exec())
        {
            if(query.next())
            {
                qDebug() << "machineId found, UPDATING the found";
                int idfound;
                idfound = query.value("MachineId").toInt();
                unsigned int activeDevice;
                activeDevice = query.value("active").toUInt();
                qDebug() << "idfound: " << idfound;
                qDebug() << "activeDevice: " << activeDevice;
                if( activeDevice == 0 )
                {
                    qDebug() << "Device disabled.";
                    QMessageBox::warning(this, "Error, device disabled.", "Device disabled in the server.");
                    return 0;
                }

                QSqlQuery query(conn.dbDongle);
                query.prepare("UPDATE StreetDongle SET GameId= :GameId, configDefault= :config, mIn='0', mOut='0', creditIn='0', creditOut='0', moneyInWhole='0', moneyInCents='0', moneyOutWhole='0', moneyOutCents='0', period = :period, expirationDate = DATE_ADD(NOW(), INTERVAL :EXCALC DAY), lastUpdate = NOW(), GameCreation = NOW(), GameVersion='V5.0.0', tipo='tinker', PassVersion= :passVersion WHERE MachineId= :idfound");
                query.bindValue(":GameId", GameId);
                query.bindValue(":idfound", idfound);
                query.bindValue(":config", config);
                query.bindValue(":passVersion", passVersion);
                query.bindValue(":period",QString::number(Periodo));
                query.bindValue(":EXCALC",QString::number(Periodo));

                if(query.exec())
                {
                    MachineId = idfound;
                    qDebug() << " ok, we are using this machId: " << MachineId;

                    if(conn.dbSystem.open() == true)
                    {
                        QSqlQuery query(conn.dbSystem);
                        query.prepare("SELECT numero from interface WHERE numero= :MachineId ");
                        query.bindValue(":MachineId",MachineId);

                        if(query.exec())
                        {
                            if(query.next())
                            {
                                QSqlQuery query(conn.dbSystem);
                                query.prepare("UPDATE interface SET id_jogo= :GameId, tipo='tinker' WHERE numero= :MachineId");
                                query.bindValue(":GameId",QString::number(GameId));
                                query.bindValue(":MachineId",QString::number(MachineId));
                                if(query.exec())
                                {
                                    qDebug() << "Updated Tinker in Calabaza, id:" << MachineId;
                                    ToLogGameId = QString::number(GameId);
                                    ToLogMachineId = QString::number(MachineId);
                                    ToLogHardSerial = hardserial.c_str();
                                    ToLogPeriodo = QString::number(Periodo);
                                }
                                else
                                {
                                    qDebug() << "Error update Tinker in Calabaza, id:" << MachineId;
                                    QString querylasterr = query.lastError().text();
                                    dbQueryError(querylasterr);
                                }
                            }
                            else
                            {
                                qDebug() << "numrows2 == 0";
                                QSqlQuery query(conn.dbSystem);
                                query.prepare("INSERT INTO interface (data_inclusao, numero, id_maquina, id_jogo, excluido, serie, tipo) VALUES (CURDATE(), :MachineId, 0, :GameId, 'N', 0, 'tinker') ");
                                query.bindValue(":MachineId",MachineId);
                                query.bindValue(":GameId",GameId);
                                if(query.exec())
                                {
                                    qDebug() << "Inserted new Tinker in Calabaza 1, id:" << MachineId;
                                    ToLogGameId = QString::number(GameId);
                                    ToLogMachineId = QString::number(MachineId);
                                    ToLogHardSerial = hardserial.c_str();
                                    ToLogPeriodo = QString::number(Periodo);
                                }
                                else
                                {
                                    qDebug() << "Error Insert new Tinker in Calabaza, id:" << MachineId;
                                    QString querylasterr = query.lastError().text();
                                    dbQueryError(querylasterr);
                                }
                            }
                        }
                        else
                        {
                            qDebug() << "Error Mysql - Calabaza DB";
                            QString querylasterr = query.lastError().text();
                            dbQueryError(querylasterr);
                            return 0;
                        }
                    }
                    else
                    {
                        QString dblasterr = conn.dbSystem.lastError().text();
                        dbConnectionError(dblasterr);
                    }
                }
                else
                {
                    QString querylasterr = query.lastError().text();
                    dbQueryError(querylasterr);
                }
            }
            else
            {
                unsigned int idfound;
                idfound = query.value("MachineId").toUInt();
                unsigned int activeDevice;
                activeDevice = query.value("active").toUInt();

                /*qDebug() << "activeDevice: " << activeDevice;
                if( activeDevice == 0 )
                {
                    qDebug() << "Device disabled.";
                    QMessageBox::warning(this, "Error, device disabled.", "Device disabled in the server.");
                    return 0;
                }*/

                qDebug() << "numrows == 0";
                QSqlQuery query(conn.dbDongle);
                query.prepare("SELECT MachineId FROM StreetDongle ORDER BY MachineId DESC LIMIT 1");
                if(query.exec())
                {
                    if(query.next())
                    {
                        qDebug() << "resultSelectdId != NULL";
                        unsigned int idfound = query.value("MachineId").toUInt() + 1;
                        QSqlQuery query(conn.dbDongle);
                        query.prepare("INSERT INTO StreetDongle (MachineId, HardSerial, GameId, period, lastUpdate, CreationDate, GameCreation, expirationDate, mIn, mOut, creditIn, creditOut, moneyInWhole, moneyInCents, moneyOutWhole, moneyOutCents, userId, adminId, configDefault, GameVersion, tipo) VALUES(:MachineId, :HardSerial,:GameId, :period, NOW(), NOW(), NOW(), DATE_ADD(NOW(), INTERVAL :period DAY), :mIn, :mOut, :creditIn, :creditOut, :moneyInWhole, :moneyInCents, :moneyOutWhole, :moneyOutCents, :userId, :adminId, :config, 'V5.0.0', 'tinker')");
                        query.bindValue(":MachineId",idfound);
                        query.bindValue(":HardSerial",hardserial.c_str());
                        query.bindValue(":GameId",GameId);
                        query.bindValue(":period",Periodo);
                        query.bindValue(":mIn",QString("0"));
                        query.bindValue(":mOut",QString("0"));
                        query.bindValue(":creditIn",QString("0"));
                        query.bindValue(":creditOut",QString("0"));
                        query.bindValue(":moneyInWhole",QString("0"));
                        query.bindValue(":moneyInCents",QString("0"));
                        query.bindValue(":moneyOutWhole",QString("0"));
                        query.bindValue(":moneyOutCents",QString("0"));
                        query.bindValue(":userId",QString("0"));
                        query.bindValue(":adminId",QString("0"));
                        query.bindValue(":config",config);
                        query.bindValue(":PassVersion",passVersion);
                        if(query.exec())
                        {
                            MachineId = int(idfound);
                            qDebug() << " ok, we are using this NEW machId: " << MachineId;
                            if(conn.dbSystem.open() == true)
                            {
                                QSqlQuery query(conn.dbSystem);
                                query.prepare("SELECT numero from interface WHERE numero= :MachineId ");
                                query.bindValue(":MachineId",MachineId);
                                if(query.exec())
                                {
                                    if(query.next())
                                    {
                                        qDebug() << "machineId already found in interface DB";
                                    }
                                    else
                                    {
                                        qDebug() << "numrows2 == 0";
                                        QSqlQuery query(conn.dbSystem);
                                        query.prepare("INSERT INTO interface (data_inclusao, numero, id_maquina, id_jogo, excluido, serie, tipo) VALUES (CURDATE(), :MachineId, 0, :GameId, 'N', 0, 'tinker') ");
                                        query.bindValue(":MachineId",MachineId);
                                        query.bindValue(":GameId",GameId);
                                        if(query.exec())
                                        {
                                            qDebug() << "Inserted new Tinker in Calabaza 2, id:" << MachineId;
                                            ToLogGameId = QString::number(GameId);
                                            ToLogMachineId = QString::number(MachineId);
                                            ToLogHardSerial = hardserial.c_str();
                                            ToLogPeriodo = QString::number(Periodo);
                                        }
                                        else
                                        {
                                            qDebug() << "Error Insert new Tinker in Calabaza, id:" << MachineId;
                                            QString querylasterr = query.lastError().text();
                                            dbQueryError(querylasterr);
                                        }
                                    }
                                }
                                else
                                {
                                    QString querylasterr = query.lastError().text();
                                    dbQueryError(querylasterr);
                                }
                            }
                            else
                            {
                                qDebug() << "Error servidor calabaza DB System";
                                QString dblasterr = conn.dbSystem.lastError().text();
                                dbConnectionError(dblasterr);
                            }
                        }
                        else
                        {
                            qDebug() << "Error Insert en DB Dongle";
                            QString querylasterr = query.lastError().text();
                            dbQueryError(querylasterr);
                            return 0;
                        }
                    }
                    else
                    {
                        qDebug() << "Error Select last MachineId en DB Dongle";
                        return 0;
                    }
                }
                else
                {
                    qDebug() << "Error Generico";
                    QString querylasterr = query.lastError().text();
                    dbQueryError(querylasterr);
                }

            }

        }
        else
        {
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    ProgressBarChange(40);


    qDebug() << "**********************************";
    qDebug() << "*********TINKER WRITER************";
    qDebug() << "**********************************";

    uint8_t memory[32768];

    //Inicializando el dispositivo
    if( Fram::fram_open("/dev/ttyUSB0") != BP_SUCCESS )
    {
      //Fram::error("No se puede abrir device");
      QMessageBox::warning(this, "FRAM Error", "FRAM no se puede abrir el dispositivo");
      return 0;
    }
    ProgressBarChange(45);

    //Verificamos que haya una FRAM valida en el puerto I2C
    qDebug() << "Comienza inicializacion FRAM";
    if(Fram::fram_is_present() != BP_SUCCESS)
    {
        //Fram::error("FRAM no detectada");
        QMessageBox::warning(this, "FRAM Error", "FRAM no detectada");
        return 0;
    }

    //Borrado total de la FRAM
    qDebug() << "Formateando FRAM";
    if(Fram::fram_format() != BP_SUCCESS)
    {
        //Fram::error("No se puede formatear");
        QMessageBox::warning(this, "FRAM Error", "FRAM no se puede formatear");
        return 0;
    }
    ProgressBarChange(50);

    //Verificando borrado(512 bytes)
    qDebug() << "Verifiying erased blocks (512 bytes)";
    //char BufferRead[50];
    //string StrBufferRead;
    //Inicio ciclo verificacion de borrado. 64 paginas de 512 bytes=32kB
    for( int i=0;i<64;i++ )
    {
        memset(buffer,0xFF,sizeof(buffer));
        Fram::fram_read(i*512,buffer,512);
        for(int j=0;j<512;j++)
        {
            if(buffer[j]!=0)
            {
                qDebug() << "the Fram is not fully erased...";
                return 0;
            }
        }
        /*if(i%16==0)
        {
            StrBufferRead.append("\n");
        }
        //printf("%02u,",i);
        sprintf(BufferRead, "%02u,", i);
        StrBufferRead.append(BufferRead);*/
    }
    //qDebug() << StrBufferRead.c_str();

    qDebug() << "Erased Ok...";
    ProgressBarChange(60);

    qDebug() << "Creating System Storage";

    Storage::Class gameStorage(&sequencer2, 16000,0);
    Storage::Class systemStorage(&sequencer, 512,gameStorage.getMaxPhysicalSize());

    gameStorage.format();
    systemStorage.format();

    //Lectura completa de la FRAM
    if(Fram::fram_read(0x0, &memory[0], 32768) != BP_SUCCESS)
    {
        //Fram::error("No se puede leer FRAM");
        QMessageBox::warning(this, "FRAM Error", "FRAM no se puede leer");
        return 0;
    }

    ProgressBarChange(70);

    qDebug() << "ServerSelected: " << ServerSelected;

    /*switch (ServerSelected)
    {
        //Calabaza Chile
        case 1:
        {
            systemStateOld.in = 0;
            systemStateOld.out = 0;
            systemStateOld.MachineId = uint32_t(MachineId);
            systemStateOld.lockBits = 0;
            systemStateOld.timeStamp = 0;
            systemStateOld.timeValue = 0;
            systemStateOld.gameId = uint32_t(GameId);
            systemStateOld.Hash = 0;

            systemStorage.startUpdate(sizeof(systemStateOld),0,buffer);
            systemStorage.addData(&systemStateOld,sizeof(systemStateOld));
            systemStorage.finalizeUpdate(buffer);

            memset(&systemStateOld,0,sizeof(systemStateOld));

            uint16_t size;
            systemStorage.startReading(size,buffer);
            systemStorage.readData(&systemStateOld,size);

            qDebug("MachineId:%d", systemStateOld.MachineId);
            qDebug("gameId:%d", systemStateOld.gameId);
            break;
        }
        //Gecko Brasil
        case 2:
        {
            systemState.in = 0;
            systemState.out = 0;
            systemState.machineId = uint32_t(MachineId);
            systemState.gameId = uint32_t(GameId);
            memset(systemState.file10,0,64);
            systemState.period = uint32_t(Periodo);
            systemState.validations = 0;
            systemState.config = uint32_t(config);
            systemState.keyPass = keyPass;
            systemState.timeStamp = dateNow;
            systemState.tinkerDate = dateNow;

            systemStorage.startUpdate(sizeof(systemState),0,buffer);
            systemStorage.addData(&systemState,sizeof(systemState));
            systemStorage.finalizeUpdate(buffer);

            memset(&systemState,0,sizeof(systemState));

            uint16_t size;
            systemStorage.startReading(size,buffer);
            systemStorage.readData(&systemState,size);

            qDebug() << systemState.keyPass;
            qDebug("MachineId:%d", systemState.machineId);
            qDebug("gameId:%d", systemState.gameId);
            break;
        }
    }*/

    systemState.in = 0;
    systemState.out = 0;
    systemState.machineId = uint32_t(MachineId);
    systemState.gameId = uint32_t(GameId);
    memset(systemState.file10,0,64);
    systemState.period = uint32_t(Periodo);
    systemState.validations = 0;
    systemState.config = uint32_t(config);
    systemState.keyPass = keyPass;
    systemState.timeStamp = dateNow;
    systemState.tinkerDate = dateNow;

    systemStorage.startUpdate(sizeof(systemState),0,buffer);
    systemStorage.addData(&systemState,sizeof(systemState));
    systemStorage.finalizeUpdate(buffer);

    memset(&systemState,0,sizeof(systemState));

    uint16_t size;
    systemStorage.startReading(size,buffer);
    systemStorage.readData(&systemState,size);

    qDebug() << systemState.keyPass;
    qDebug("MachineId:%d", systemState.machineId);
    qDebug("gameId:%d", systemState.gameId);

    ProgressBarChange(80);


    FILE *fp;

    fp = fopen("FramDump.dmp","w");

    if ( !fp )
    {
        qDebug() << "Failed to create output file";
        return 1;
    }

    for(int i=0;i<64;i++)
    {
        Fram::fram_read(i*512,buffer,512);
        fseek( fp, i*512, SEEK_SET );
        fwrite(buffer,1,512,fp);
    }

    fclose(fp);
    ProgressBarChange(90);

    // Cerrando bus pirate
    if( Fram::fram_close() != BP_SUCCESS )
    {
        //Fram::error("No se puede cerrar device");
        QMessageBox::warning(this, "FRAM Error", "FRAM no se puede cerrar el dispositivo");
        return 0;
    }

    qDebug("dateNow %02d:%02d:%02d  %02d/%02d/%04d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year + 1900);
    qDebug("dateNow %02d:%02d:%02d  %02d/%02d/%04d", dateNow.seg, dateNow.min, dateNow.seg, dateNow.day, dateNow.month + 1, dateNow.year + 1900);

    int final_res = Rtc::setTime(dateNow);
    qDebug() << "After rtc, result is: " << final_res;

    if( final_res == 0)
    {
        QMessageBox::warning(this, "RTC Error", "No se puede grabar la hora en el reloj");
        return 0;
    }

    qDebug() << "process complete";
    ProgressBarChange(98);
    return final_res;
}

/**
 * @brief WriteTinkerForm::FtdiLatencyModifier
 * @return
 */
int WriteTinkerForm::FtdiLatencyModifier()
{
    std::string cmdmod;
    std::string cmdcheck;
    int res;

    cmdmod = "echo 1 > /sys/bus/usb-serial/devices/ttyUSB0/latency_timer";
    cmdcheck = "cat /sys/bus/usb-serial/devices/ttyUSB0/latency_timer";

    QString ParsedOut = GetStdoutFromCommand(cmdcheck).c_str();

    qDebug() << "first current latency: " << ParsedOut;
    if(ParsedOut == "16\n")
    {
        GetStdoutFromCommand(cmdmod);
    }

    QString ParsedOut2 = GetStdoutFromCommand(cmdcheck).c_str();
    qDebug() << "second current latency: " << ParsedOut2;
    if(ParsedOut2 == "1\n")
    {
        qDebug() << "latency correctly modified";
        res = 1;
    }
    else
    {
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
std::string WriteTinkerForm::GetStdoutFromCommand(std::string cmd)
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

/**
 * @brief WriteTinkerForm::getGameId
 * obtiene el Id del juego escogido
 * @param gamename
 * @return
 */
int WriteTinkerForm::getGameId(QString gamename)
{
    int result1 = 0;
    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT GameId FROM Games WHERE Status = 'tinker' AND GameName =:gamename");
        query.bindValue(":gamename",gamename);

        if(query.exec())
        {
            if(query.next())
            {
                 result1 = query.value("GameId").toInt();
            }
        }
        else
        {
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return result1;
}


/**
 * @brief WriteTinkerForm::setGameList
 * llena el combobox con los nombres de los juegos
 */
void WriteTinkerForm::setGameList()
{
    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        //if(query.exec("SELECT DISTINCT Games.GameName FROM Games, GamesVersions WHERE Games.GameId = GamesVersions.GameId AND GamesVersions.tipo = 'tinker' ORDER BY GamesVersions.GameId ASC")){
        if(query.exec("SELECT DISTINCT Games.GameName FROM Games, GamesVersions WHERE Games.GameId = GamesVersions.GameId AND GamesVersions.tipo = 'tinker' AND Games.Status = 'tinker' ORDER BY GamesVersions.GameId ASC"))
        {
            while(query.next())
            {
                result.append(query.value("GameName").toString());
            }
        }
        else
        {
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        ui->comboBox_GameName->clear();
        for(int i = 0; i < result.size(); i++){
            ui->comboBox_GameName->addItem(result[i]);
        }
    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}

/**
 * @brief WriteTinkerForm::LogAction
 * @param action
 * @param UserId
 * @param TerminalIp_
 * @param VmId_
 */
void WriteTinkerForm::LogAction(QString action, int UserId, QString TerminalIp_, QString VmId_)
{
    if(conn.dbSystem.open() == true)
    {
        QSqlQuery query(conn.dbSystem);
        QString act;
        act = "";
        act.append(action);

        query.prepare("INSERT INTO snake_log (user_id, terminal_ip, vm_id, descripcion) VALUES (:userid, :terminalip, :vmid, :desc)");
        query.bindValue(":userid",UserId);
        query.bindValue(":terminalip",TerminalIp_);
        query.bindValue(":vmid",VmId_);
        query.bindValue(":desc",act);
        if(query.exec())
        {
            if(query.next())
            {
                qDebug() << "snake_log success";
            }
        }
        else
        {
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }
    else
    {
        QString dblasterr = conn.dbSystem.lastError().text();
        dbConnectionError(dblasterr);
    }
}

/**
 * @brief WriteTinkerForm::setValoresPeriodo
 */
void WriteTinkerForm::setValoresPeriodo()
{
    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        query.prepare("SELECT * FROM Period");
        if(query.exec())
        {
            while(query.next())
            {
                result.append(query.value("Periodo").toString());
            }

        }
        else
        {
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        ui->comboBox_Periodo->clear();
        for(int i = 0; i < result.size(); i++)
        {
            ui->comboBox_Periodo->addItem(result[i]);
        }
    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}

/**
 * @brief WriteTinkerForm::getValoresPeriodo
 * @param Dias
 * @return int res
 */
int WriteTinkerForm::getValoresPeriodo(QString Dias)
{
    int res = 0;
    qDebug() << "Periodo Seleccionado: " << Dias;
    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT Dias FROM Period WHERE Periodo = :Dias");
        query.bindValue(":Dias",Dias);
        if(query.exec())
        {
            if(query.next())
            {
                res = query.value("Dias").toInt();
            }
            else
            {
                res = 0;
                QString querylasterr = query.lastError().text();
                dbQueryError(querylasterr);
            }
        }
        else
        {
            res = 0;
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }
    else
    {
        res = 0;
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return res;
}

void WriteTinkerForm::setConfigList()
{
    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        QStringList result;
        query.prepare("SELECT * FROM Config");
        if(query.exec())
        {
            while(query.next())
            {
                result.append(query.value("Details").toString());
            }

        }
        else
        {
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
        ui->comboBox_Config->clear();
        for(int i = 0; i < result.size(); i++)
        {
            ui->comboBox_Config->addItem(result[i]);


        }
        ui->comboBox_Config->setCurrentIndex(45);
    }
    else
    {
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
}

int WriteTinkerForm::getConfig(QString Details)
{
    int res = 0;
    qDebug() << "Config Seleccionado: " << Details;
    if (conn.dbDongle.open() == true)
    {
        QSqlQuery query(conn.dbDongle);
        query.prepare("SELECT config_id FROM Config WHERE Details = :Details");
        query.bindValue(":Details",Details);
        if(query.exec())
        {
            if(query.next())
            {
                res = query.value("config_id").toInt();
            }
            else
            {
                res = 0;
                QString querylasterr = query.lastError().text();
                dbQueryError(querylasterr);
            }
        }
        else
        {
            res = 0;
            QString querylasterr = query.lastError().text();
            dbQueryError(querylasterr);
        }
    }
    else
    {
        res = 0;
        QString dblasterr = conn.dbDongle.lastError().text();
        dbConnectionError(dblasterr);
    }
    return res;
}

/**
 * @brief WriteTinkerForm::on_PrintLabel_Button_clicked
 */
void WriteTinkerForm::on_PrintLabel_Button_clicked()
{
    QString ReturnValuePrintPs;
    std::string DayMonthYearToPrintStd;
    DayMonthYearToPrintStd = GetStdoutFromCommand("date +\"%d/%m/%y\"");
    DayMonthYearToPrint = DayMonthYearToPrintStd.c_str();

    if(printer.WritePs("   ",GameIdToPrint,MachineIdToPrint,DayMonthYearToPrint) == false){
        QMessageBox::warning(this, "Error de impresion", "Error en la creacion del archivo a imprimir");
        return;
    }else{
        ReturnValuePrintPs = printer.PrintPs();
        if (ReturnValuePrintPs == "complete"){
            QMessageBox::information(this, "Etiqueta Tinker", "Impresion Completa");
        }else{
            QMessageBox::critical(this, "Error de impresion", ReturnValuePrintPs);
        }
    }
}

/*********************************************************************************** */
/****************************** error message fuctions **************************** */
/*********************************************************************************** */

/**
 * @brief WriteTinkerForm::dbConnectionError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr QString
 */
void WriteTinkerForm::dbConnectionError(QString dblasterr){
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Database Error");
    errorMessageBox.setText("Error en la conexion a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

/**
 * @brief WriteTinkerForm::dbQueryError
 * Se utiliza este objeto para añadir el codigo
 * de detailed text.
 * @param dblasterr QString
 */
void WriteTinkerForm::dbQueryError(QString dblasterr)
{
    QMessageBox errorMessageBox;
    errorMessageBox.setWindowTitle("Query Error");
    errorMessageBox.setText("Error en la Query a la db");
    errorMessageBox.setStandardButtons(QMessageBox::Ok);
    errorMessageBox.setIcon(QMessageBox::Warning);
    errorMessageBox.setDetailedText(dblasterr);
    errorMessageBox.exec();
}

void WriteTinkerForm::ProgressBarHide()
{
    ui->progressBar->hide();
}

void WriteTinkerForm::ProgressBarShow()
{
    ui->progressBar->show();
}

void WriteTinkerForm::ProgressBarChange(int i)
{
    ui->progressBar->setValue(i);
}

void WriteTinkerForm::ProgressBarReset(){
    ui->progressBar->reset();
}

/*********************************************************************************** */
/****************************** USB message fuctions ******************************* */
/*********************************************************************************** */

bool WriteTinkerForm::Usb0StartWatch()
{
    bool resultUsb = false;

    if(Usb0FolderExists() && !alreadyWatched)
    {
        watcher.addPath("/dev/serial");
        QObject::connect(&watcher, SIGNAL(directoryChanged(QString)), this, SLOT(Usb0DisconnectedSlot(QString)));
        alreadyWatched = true;
        resultUsb = true;
    }
    else if(!alreadyWatched)
    {
        QMessageBox::critical(this, "Error Bus Pirate", "el modulo BusPirate se encuentra desconectado");
    }

     qDebug() << "Usb0StartWatch";

     return resultUsb;
}

bool WriteTinkerForm::Usb0FolderExists()
{
    bool result;
    QFile Usb0Folder("/dev/ttyUSB0");
    result = Usb0Folder.exists();
    return result;
}

void WriteTinkerForm::Usb0DisconnectedSlot(QString)
{
    QMessageBox::critical(this, "Error Cable USB Tinkerboard", "El cable de grabacion Bus Pirate a PC ha sido desconectado.\nPor Favor reinicie el Programa si desea grabar más TinkerBoards.");
}



