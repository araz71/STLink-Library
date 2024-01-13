#include "STLink.h"

using namespace Jeyran;

bool STLink::wait_for_command(int msec = 5000) {
    if (!proc.waitForStarted(msec)) {
        return false;
    }

    if (!proc.waitForReadyRead(msec)) {
        return false;
    }

    return true;
}
STLink::STLink() {
    connection_state = false;

    proc.start("ST-LINK_CLI.exe", {"-c", "swd"});
    if (wait_for_command()) {
        std::string result = proc.readAll().toStdString();
        if (result.find("No ST-LINK detected!") != std::string::npos) {
            throw std::domain_error("No ST-Link detected");
        } else {
            connection_state = true;
        }
    } else {
        throw std::domain_error("ST-Link_CLI calling failed.");
    }

    proc.close();
}

void STLink::program(QString filename) {
    QStringList args = {"-c", "swd", "UR", "-P", filename};
    proc.start("ST-LINK_CLI.exe", args);
    if (wait_for_command(20000)) {
        QByteArray result = proc.readAll();
        qDebug("%s", result.toStdString().c_str());
    } else {
        qDebug("Failed");
    }

    proc.close();
}

void STLink::verify(QString filename)
{
    proc.start("ST-LINK_CLI.exe", {"-c", "swd", "-V", filename});
    if (wait_for_command()) {
        std::string result = proc.readAll().toStdString();
    }
}

bool STLink::set_read_protection(ProtectionLevel level)
{
    bool ret_value = false;
    proc.start("ST-LINK_CLI.exe", {"-c", "swd", "-OB", "RDP", QString::number(level)});
    if (wait_for_command()) {
        std::string result = proc.readAll().toStdString();
        proc.close();
        if (result.find("Option bytes updated successfully.") != std::string::npos) {
            ret_value = true;
        }
    }

    proc.close();
    return ret_value;
}

bool STLink::erase() {
    bool ret_value = false;
    proc.start("ST-LINK_CLI.exe", {"-c", "swd", "-ME"});
    if (wait_for_command()) {
        std::string result = proc.readAll().toStdString();
        if (result.find_first_of("Flash memory erased.") != std::string::npos) {
            ret_value = true;
        }
    }

    proc.close();
    return ret_value;
}

std::map<STLink::ConnectionInformation, std::string> STLink::get_connection_information(bool& ok)
{
    std::map<STLink::ConnectionInformation, std::string> return_result;
    ok = true;
    proc.start("ST-LINK_CLI.exe", {"-c", "swd"});
    if (wait_for_command()) {
        std::string result = proc.readAll().toStdString();
        while (result.find_first_of("\n") != std::string::npos) {
            auto line = result.substr(0, result.find_first_of("\n"));
            result = result.substr(result.find_first_of("\n") + 1, result.size());

            if (line.find_first_of(":") != std::string::npos) {
                auto key = line.substr(0, line.find_first_of(":"));
                auto value = line.substr(line.find_first_of(":") + 2, line.size());
                if (key == "Device family") {
                    return_result[STLink::ConnectionInformation::DeviceFamily] = value;
                } else if (key == "Device flash Size") {
                    return_result[STLink::ConnectionInformation::DeviceFlashSize] = value;
                } else if (key == "Device ID") {
                    return_result[STLink::ConnectionInformation::DeviceID] = value;
                }
            }
        }
    } else {
        ok = false;
    }
    proc.close();
    return return_result;
}

