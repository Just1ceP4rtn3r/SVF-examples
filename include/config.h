#include <iostream>

using namespace std;

namespace mqttactic
{

    std::string subs = "mosquitto__subhier::subs";
    std::string RetainedMsg = "mosquitto_db::retains";
    std::string status = "mosquitto::state";
    std::string WillMsg = "mosquitto::will";
    std::string msgQue = "mosquitto_msg_data::inflight+mosquitto_msg_data::queued";
    std::string msg = "mosquitto::sock";

    // std::string clientID = "mosquitto::id";
    // std::string permission = "Authentication::aclTree";
}
