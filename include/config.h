#ifndef MQTTACTICCONFIG
#define MQTTACTICCONFIG

#include <iostream>

namespace mqttactic
{

    static std::string subs = "mosquitto__subhier::subs";
    static std::string RetainedMsg = "mosquitto_db::retains";
    static std::string status = "mosquitto::state";
    static std::string WillMsg = "mosquitto::will";
    static std::string msgQue = "mosquitto_msg_data::inflight+mosquitto_msg_data::queued";
    static std::string msg = "mosquitto::sock";

    // static std::string clientID = "mosquitto::id";
    // static std::string permission = "Authentication::aclTree";
}

#endif