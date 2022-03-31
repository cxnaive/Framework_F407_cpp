#include "can_pc.h"

#include "stdlib.h"

canpc::canpc(const canpc_config& _config) : sender({_config.device, _config.send_identifer}),
                                            recver({_config.device, sizeof(pc_recv), _config.recv_identifer, can_recv::can_recv_notifiy(), _config.lost_callback}) {
    pc_recv_data = (pc_recv*)(recver.data_rx.data.data());
    data_updated = &recver.data_updated;
}

void canpc::send(const pc_send& data){
    sender.send((const uint8_t*)&data,sizeof(pc_send));
}