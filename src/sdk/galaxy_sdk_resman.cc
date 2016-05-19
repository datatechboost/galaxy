// Copyright (c) 2016, Baidu.com, Inc. All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "galaxy_sdk_util.h"
#include "rpc/rpc_client.h"
#include "ins_sdk.h"
#include <gflags/gflags.h>
#include "galaxy_sdk_resman.h"

//nexus
DECLARE_string(nexus_addr);
DECLARE_string(nexus_root);
DECLARE_string(resman_path);
DECLARE_string(appmaster_path);

namespace baidu {
namespace galaxy {
namespace sdk {

ResourceManager::ResourceManager() : rpc_client_(NULL),
 									 res_stub_(NULL) {
	rpc_client_ = new ::baidu::galaxy::RpcClient();
    full_key_ = FLAGS_nexus_root + FLAGS_resman_path;
    nexus_ = new ::galaxy::ins::sdk::InsSDK(FLAGS_nexus_addr);
}

ResourceManager::~ResourceManager() {
	delete rpc_client_;
    if (NULL != res_stub_) {
        delete res_stub_;
    }
    delete nexus_;
}

bool ResourceManager::GetStub() {
    std::string endpoint;
    ::galaxy::ins::sdk::SDKError err;
    bool ok = nexus_->Get(full_key_, &endpoint, &err);
    if (!ok || err != ::galaxy::ins::sdk::kOK) {
        fprintf(stderr, "get appmaster endpoint from nexus failed: %s\n",
                ::galaxy::ins::sdk::InsSDK::StatusToString(err).c_str());
        return false;
    }
    if(!rpc_client_->GetStub(endpoint, &res_stub_)) {
        fprintf(stderr, "connect resmanager fail, resmanager: %s\n", endpoint.c_str());
        return false;
    }
    return true;
}

bool ResourceManager::Login(const std::string& user, const std::string& password) {
	return false;
}

bool ResourceManager::EnterSafeMode(const EnterSafeModeRequest& request, EnterSafeModeResponse* response) {
    
    ::baidu::galaxy::proto::EnterSafeModeRequest pb_request;
    ::baidu::galaxy::proto::EnterSafeModeResponse pb_response;
    
    FillUser(request.user, pb_request.mutable_user());

    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::EnterSafeMode, &pb_request, &pb_response, 5, 1);
    
    if (!StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (pb_response.error_code().status() != ::baidu::galaxy::proto::kOk) {
        return false;
    }
    return true;
}

bool ResourceManager::LeaveSafeMode(const LeaveSafeModeRequest& request, LeaveSafeModeResponse* response) {
    ::baidu::galaxy::proto::LeaveSafeModeRequest pb_request;
    ::baidu::galaxy::proto::LeaveSafeModeResponse pb_response;
    
    FillUser(request.user, pb_request.mutable_user());

    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::LeaveSafeMode, &pb_request, &pb_response, 5, 1);
    
    if (!StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (pb_response.error_code().status() != ::baidu::galaxy::proto::kOk) {
        return false;
    }
    return true;
}

bool ResourceManager::Status(const StatusRequest& request, StatusResponse* response) {
    return false;
}

bool ResourceManager::CreateContainerGroup(const CreateContainerGroupRequest& request, CreateContainerGroupResponse* response) {
    ::baidu::galaxy::proto::CreateContainerGroupRequest pb_request;
    ::baidu::galaxy::proto::CreateContainerGroupResponse pb_response;
    
    FillUser(request.user, pb_request.mutable_user());
    pb_request.set_replica(request.replica);
    pb_request.set_name(request.name);
    FillContainerDescription(request.desc, pb_request.mutable_desc());
    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::CreateContainerGroup, &pb_request, &pb_response, 5, 1);
    
    if (!StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (pb_response.error_code().status() != ::baidu::galaxy::proto::kOk) {
        return false;
    }
    response->id = pb_response.id(); 
    return true;
}

bool ResourceManager::RemoveContainerGroup(const RemoveContainerGroupRequest& request, RemoveContainerGroupResponse* response) {
    ::baidu::galaxy::proto::RemoveContainerGroupRequest pb_request;
    ::baidu::galaxy::proto::RemoveContainerGroupResponse pb_response;
    
    FillUser(request.user, pb_request.mutable_user());
    pb_request.set_id(request.id);

    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::RemoveContainerGroup, &pb_request, &pb_response, 5,1);
    if (!StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (response->error_code.status != kOk) {
        return false;
    }
    return true;
}

bool ResourceManager::UpdateContainerGroup(const UpdateContainerGroupRequest& request, UpdateContainerGroupResponse* response) {
    ::baidu::galaxy::proto::UpdateContainerGroupRequest pb_request;
    ::baidu::galaxy::proto::UpdateContainerGroupResponse pb_response;

    FillUser(request.user, pb_request.mutable_user());
    pb_request.set_replica(request.replica);
    pb_request.set_id(request.id);
    pb_request.set_interval(request.interval);
    FillContainerDescription(request.desc, pb_request.mutable_desc());
    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::UpdateContainerGroup, &pb_request, &pb_response, 5, 1);
    
    if (!StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (response->error_code.status != kOk) {
        return false;
    }
    return true;
}

bool ResourceManager::ListContainerGroups(const ListContainerGroupsRequest& request, ListContainerGroupsResponse* response) {
    ::baidu::galaxy::proto::ListContainerGroupsRequest pb_request;
    ::baidu::galaxy::proto::ListContainerGroupsResponse pb_response;

    FillUser(request.user, pb_request.mutable_user());
    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::ListContainerGroups, &pb_request, &pb_response, 5, 1);
    
    if (!StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (response->error_code.status != kOk) {
        return false;
    }
    for (int i = 0; i < pb_response.containers().size(); ++i) {
        const ::baidu::galaxy::proto::ContainerGroupStatistics& pb_container = pb_response.containers(i);
        ContainerGroupStatistics container;
        container.id = pb_container.id();
        container.replica = pb_container.replica();
        container.ready = pb_container.ready();
        container.pending = pb_container.pending();
        //container.destroying = pb_container.destroying();
        container.cpu.total = pb_container.cpu().total();
        container.cpu.assigned = pb_container.cpu().assigned();
        container.cpu.used = pb_container.cpu().used();
        container.memory.total = pb_container.memory().total();
        container.memory.assigned = pb_container.memory().assigned();
        container.memory.used = pb_container.memory().used();
        
        for (int j = 0; j < pb_container.volums().size(); ++j) {
            const ::baidu::galaxy::proto::VolumResource& pb_volum = pb_container.volums(j);
            VolumResource volum;
            if ( ! VolumMediumSwitch(pb_volum.medium(), &volum.medium)) {
                return false;
            }
            volum.volum.total = pb_volum.volum().total();
            volum.volum.assigned = pb_volum.volum().assigned();
            volum.volum.used = pb_volum.volum().used();
            volum.device_path = pb_volum.device_path();
            container.volums.push_back(volum);
        }

        container.submit_time = pb_container.submit_time();
        container.update_time = pb_container.update_time();
        response->containers.push_back(container);
    }
    return true;
}

bool ResourceManager::ShowContainerGroup(const ShowContainerGroupRequest& request, ShowContainerGroupResponse* response) {
    
    ::baidu::galaxy::proto::ShowContainerGroupRequest pb_request;
    ::baidu::galaxy::proto::ShowContainerGroupResponse pb_response;

    FillUser(request.user, pb_request.mutable_user());
    pb_request.set_id(request.id);
    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::ShowContainerGroup, &pb_request, &pb_response, 5, 1);

    if (!StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (response->error_code.status != kOk) {
        return false;
    }

    response->desc.priority = pb_response.desc().priority();
    response->desc.run_user = pb_response.desc().run_user();
    response->desc.version = pb_response.desc().version();
    response->desc.cmd_line = pb_response.desc().cmd_line();
    response->desc.max_per_host = pb_response.desc().max_per_host();
    response->desc.tag = pb_response.desc().tag();
    response->desc.workspace_volum.size = pb_response.desc().workspace_volum().size();
    
    if (! VolumTypeSwitch(pb_response.desc().workspace_volum().type(), &response->desc.workspace_volum.type) 
            || ! VolumMediumSwitch(pb_response.desc().workspace_volum().medium(), &response->desc.workspace_volum.medium)) {
        return false;
    }
    response->desc.workspace_volum.source_path = pb_response.desc().workspace_volum().source_path();
    response->desc.workspace_volum.dest_path = pb_response.desc().workspace_volum().dest_path();
    response->desc.workspace_volum.readonly = pb_response.desc().workspace_volum().readonly();
    response->desc.workspace_volum.exclusive = pb_response.desc().workspace_volum().exclusive();
    response->desc.workspace_volum.use_symlink = pb_response.desc().workspace_volum().use_symlink();

    for (int i = 0; i < pb_response.containers().size(); ++i) {
        const ::baidu::galaxy::proto::ContainerStatistics& pb_container = pb_response.containers(i);
        ContainerStatistics container;
        container.endpoint = pb_container.endpoint();
        if (! ContainerStatusSwitch(pb_container.status(), &container.status)) {
            return false;
        }
        container.cpu.total = pb_container.cpu().total();
        container.cpu.assigned = pb_container.cpu().assigned();
        container.cpu.used = pb_container.cpu().used();
        container.memory.total = pb_container.memory().total();
        container.memory.assigned = pb_container.memory().assigned();
        container.memory.used = pb_container.memory().used();
        
        for (int j = 0; j < pb_container.volums().size(); ++j) {
            const ::baidu::galaxy::proto::VolumResource& pb_volum = pb_container.volums(j);
            VolumResource volum;
            if ( ! VolumMediumSwitch(pb_volum.medium(), &volum.medium)) {
                return false;
            }
            volum.volum.total = pb_volum.volum().total();
            volum.volum.assigned = pb_volum.volum().assigned();
            volum.volum.used = pb_volum.volum().used();
            volum.device_path = pb_volum.device_path();
            container.volums.push_back(volum);
        }
        response->containers.push_back(container);
    }

    return true;
}

bool ResourceManager::AddAgent(const AddAgentRequest& request, AddAgentResponse* response) {
    ::baidu::galaxy::proto::AddAgentRequest pb_request;
    ::baidu::galaxy::proto::AddAgentResponse pb_response;

    FillUser(request.user, pb_request.mutable_user());
    pb_request.set_endpoint(request.endpoint);
    pb_request.set_pool(request.pool);
    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::AddAgent, &pb_request, &pb_response, 5, 1);

    if (!StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (response->error_code.status != kOk) {
        return false;
    }
    return true;
}

bool ResourceManager::RemoveAgent(const RemoveAgentRequest& request, RemoveAgentResponse* response) {
    ::baidu::galaxy::proto::RemoveAgentRequest pb_request;
    ::baidu::galaxy::proto::RemoveAgentResponse pb_response;

    FillUser(request.user, pb_request.mutable_user());
    pb_request.set_endpoint(request.endpoint);
    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::RemoveAgent, &pb_request, &pb_response, 5, 1);

    if (!StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (response->error_code.status != kOk) {
        return false;
    }
    return true;
}

bool ResourceManager::OnlineAgent(const OnlineAgentRequest& request, OnlineAgentResponse* response) {
    return false;
}

bool ResourceManager::OfflineAgent(const OfflineAgentRequest& request, OfflineAgentResponse* response) {
    return false;
}

bool ResourceManager::ListAgents(const ListAgentsRequest& request, ListAgentsResponse* response) {
    ::baidu::galaxy::proto::ListAgentsRequest pb_request;
    ::baidu::galaxy::proto::ListAgentsResponse pb_response;

    FillUser(request.user, pb_request.mutable_user());
    pb_request.set_pool(request.pool);
    rpc_client_->SendRequest(res_stub_, &::baidu::galaxy::proto::ResMan_Stub::ListAgents, &pb_request, &pb_response, 5, 1);

    if ( ! StatusSwitch(pb_response.error_code().status(), &response->error_code.status)) {
        return false;
    }
    response->error_code.reason = pb_response.error_code().reason();
    if (response->error_code.status != kOk) {
        return false;
    }

    for (int i = 0; i < pb_response.agents().size(); ++i) {
        const ::baidu::galaxy::proto::AgentStatistics& pb_agent = pb_response.agents(i);
        AgentStatistics agent;
        agent.endpoint = pb_agent.endpoint();
        if (!AgentStatusSwitch(pb_agent.status(), &agent.status)) {
            return false;
        }
        agent.pool = pb_agent.pool();
        agent.cpu.total = pb_agent.cpu().total();
        agent.cpu.assigned = pb_agent.cpu().assigned();
        agent.cpu.used = pb_agent.cpu().used();
        agent.memory.total = pb_agent.memory().total();
        agent.memory.assigned = pb_agent.memory().assigned();
        agent.memory.used = pb_agent.memory().used();
        for (int j = 0; j < pb_agent.volums().size(); ++j) {
            const ::baidu::galaxy::proto::VolumResource& pb_volum = pb_agent.volums(j);
            VolumResource volum;
            if ( ! VolumMediumSwitch(pb_volum.medium(), &volum.medium)) {
                return false;
            }
            volum.volum.total = pb_volum.volum().total();
            volum.volum.assigned = pb_volum.volum().assigned();
            volum.volum.used = pb_volum.volum().used();
            volum.device_path = pb_volum.device_path();
            agent.volums.push_back(volum);
        }
        agent.total_containers = pb_agent.total_containers();
        
        for (int j = 0; j < pb_agent.tags().size(); ++j) {
            agent.tags.push_back(pb_agent.tags(j));
        }
        response->agents.push_back(agent);
    }

    return true;
}

bool ResourceManager::CreateTag(const CreateTagRequest& request, CreateTagResponse* response) {
    return false;
}

bool ResourceManager::ListTags(const ListTagsRequest& request, ListTagsResponse* response) {
    return false;
}

bool ResourceManager::ListAgentsByTag(const ListAgentsByTagRequest& request, ListAgentsByTagResponse* response) {
    return false;
}

bool ResourceManager::GetTagsByAgent(const GetTagsByAgentRequest& request, GetTagsByAgentResponse* response) {
    return false;
}

bool ResourceManager::AddAgentToPool(const AddAgentToPoolRequest& request, AddAgentToPoolResponse* response) {
    return false;
}

bool ResourceManager::RemoveAgentFromPool(const RemoveAgentFromPoolRequest& request, RemoveAgentFromPoolResponse* response) {
    return false;
}

bool ResourceManager::ListAgentsByPool(const ListAgentsByPoolRequest& request, ListAgentsByPoolResponse* response) {
    return false;
}

bool ResourceManager::GetPoolByAgent(const GetPoolByAgentRequest& request, GetPoolByAgentResponse* response) {
    return false;
}

bool ResourceManager::AddUser(const AddUserRequest& request, AddUserResponse* response) {
    return false;
}

bool ResourceManager::RemoveUser(const RemoveUserRequest& request, RemoveUserResponse* response) {
    return false;
}

bool ResourceManager::ListUsers(const ListUsersRequest& request, ListUsersResponse* response) {
    return false;
}

bool ResourceManager::ShowUser(const ShowUserRequest& request, ShowUserResponse* response) {
    return false;
}

bool ResourceManager::GrantUser(const GrantUserRequest& request, GrantUserResponse* response) {
    return false;
}

bool ResourceManager::AssignQuota(const AssignQuotaRequest& request, AssignQuotaResponse* response) {
    return false;
}

} //namespace sdk
} //namespace galaxy
} //namespace baidu
