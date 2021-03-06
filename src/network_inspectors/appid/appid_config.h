//--------------------------------------------------------------------------
// Copyright (C) 2014-2016 Cisco and/or its affiliates. All rights reserved.
// Copyright (C) 2005-2013 Sourcefire, Inc.
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License Version 2 as published
// by the Free Software Foundation.  You may not use, modify or distribute
// this program under any other version of the GNU General Public License.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//--------------------------------------------------------------------------

// appid_config.h author Sourcefire Inc.

#ifndef APP_ID_CONFIG_H
#define APP_ID_CONFIG_H

#include "appid.h"
#include "client_plugins/client_app_config.h"
#include "detector_plugins/detector_sip.h"
#include "service_plugins/service_config.h"

#define APP_ID_MAX_DIRS         16
#define APP_ID_PORT_ARRAY_SIZE  65536
#define MAX_ZONES               1024
#define MAX_EVENT_APPNAME_LEN   64

struct NetworkSet;
class AppInfoManager;

extern unsigned appIdPolicyId;
extern uint32_t app_id_netmasks[];

struct PortExclusion
{
    int family;
    ip::snort_in6_addr ip;
    ip::snort_in6_addr netmask;
};

// Modules can use this generic data structure to store their configuration.
// All such generic configurations are stored in genericConfigList. Modules
// are responsible for populating the configuration in init() and cleaning it
// up in clean() function.
//
// Currently, IMAP, PO3 and MDNS use this data structure. Lua modules currently
// do not have any configuration. They can use this data structure in the future,
// if needed.
struct AppidGenericConfigItem
{
    char* name;     ///< Module name
    void* pData;    ///< Module configuration data
};

struct AppIdSessionLogFilter
{
    sfip_t sip;
    bool sip_flag = false;
    sfip_t dip;
    bool dip_flag = false;
    uint16_t sport = 0;
    uint16_t dport = 0;
    PktType protocol = PktType::NONE;
    bool log_all_sessions = false;
};

class AppIdModuleConfig
{
public:
    AppIdModuleConfig() { }
    ~AppIdModuleConfig();

    const char* conf_file = nullptr;
    bool stats_logging_enabled = false;
    unsigned long app_stats_period = 0;
    unsigned long app_stats_rollover_size = 0;
    unsigned long app_stats_rollover_time = 0;
    const char* app_detector_dir = nullptr;
    const char* thirdparty_appid_dir = nullptr;
    uint32_t instance_id = 0;
    uint32_t memcap = 0;
    bool debug = false;
    bool dump_ports = false;
    AppIdSessionLogFilter session_log_filter;

    // FIXIT-L configs below not set from appid preproc config, should they be?
    uint32_t disable_safe_search = 0;
    uint32_t dns_host_reporting = 0;
    uint32_t referred_appId_disabled = 0;
    uint32_t rtmp_max_packets = 0;
    uint32_t mdns_user_reporting = 0;
    uint32_t ftp_userid_disabled = 0;
    uint32_t chp_userid_disabled = 0;
    uint32_t chp_body_collection_disabled = 0;
    uint32_t chp_fflow_disabled = 0;
    uint32_t chp_body_collection_max = 0;
    uint32_t max_tp_flow_depth = 0;
    uint32_t tp_allow_probes = 0;
    bool http2_detection_enabled = false;
};


// App ID Active Configuration
enum RnaFwConfigState
{
    RNA_FW_CONFIG_STATE_UNINIT,
    RNA_FW_CONFIG_STATE_INIT,
    RNA_FW_CONFIG_STATE_PENDING,
};

class AppIdConfig
{
public:
    AppIdConfig( AppIdModuleConfig* config );
    ~AppIdConfig();

    bool init_appid();
    void cleanup();
    void show();

    static AppIdConfig* get_appid_config();
    void set_safe_search_enforcement(int enabled);

    // add, find, remove generic config items...
    void add_generic_config_element(const char* name, void* pData);
    void* find_generic_config_element(const char* name);
    void remove_generic_config_element(const char* name);

    unsigned max_service_info = 0;
    unsigned net_list_count = 0;
    NetworkSet* net_list_list = nullptr;
    NetworkSet* net_list = nullptr;
    NetworkSet* net_list_by_zone[MAX_ZONES] = { nullptr };
    AppId tcp_port_only[65536] = { 0 };            ///< Service IDs for port-only TCP services
    AppId udp_port_only[65536] = { 0 };            ///< Service IDs for port-only UDP services
    AppId ip_protocol[255] = { 0 };                ///< Service IDs for non-TCP / UDP protocol services
    SF_LIST client_app_args;                ///< List of Client App arguments
    // for each potential port, an sflist of PortExclusion structs
    SF_LIST* tcp_port_exclusions_src[APP_ID_PORT_ARRAY_SIZE] = { nullptr };
    SF_LIST* udp_port_exclusions_src[APP_ID_PORT_ARRAY_SIZE] = { nullptr };
    SF_LIST* tcp_port_exclusions_dst[APP_ID_PORT_ARRAY_SIZE] = { nullptr };
    SF_LIST* udp_port_exclusions_dst[APP_ID_PORT_ARRAY_SIZE] = { nullptr };
    AppIdModuleConfig* mod_config;

private:
    void read_port_detectors(const char* files);
    void configure_analysis_networks(char* toklist[], uint32_t flag);
    int add_port_exclusion(SF_LIST* port_exclusions[], const ip::snort_in6_addr* ip,
        const ip::snort_in6_addr* netmask, int family, uint16_t port);
    void process_port_exclusion(char* toklist[]);
    void process_config_directive(char* toklist[], int /* reload */);
    int load_analysis_config(const char* config_file, int reload, int instance_id);
    void display_port_config();

    AppInfoManager& app_info_mgr;
};

#endif
