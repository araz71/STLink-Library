#ifndef STLINK_H
#define STLINK_H

#include <QObject>
#include <QProcess>
#include <map>
#include <string>

namespace Jeyran
{

class STLink
{
    QProcess proc;
    bool connection_state;

    std::map<std::string, std::string> informations;

public:
    enum ConnectionMode{
        SWD,
        JTAG
    };

    enum ProtectionLevel {
        NoProtection,
        Level1,
        Level2,
    };

    enum ConnectionInformation {
        DeviceFamily,
        DeviceID,
        DeviceFlashSize,
        STLinkSerialNumber,
    };

    explicit STLink();

    void program(QString filename);
    void verify(QString filename);
    bool set_read_protection(STLink::ProtectionLevel level);
    bool erase();
    std::map<STLink::ConnectionInformation, std::string> get_connection_information(bool &ok);
    bool wait_for_command(int msec);
};

}
#endif // STLINK_H
