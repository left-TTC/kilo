
#include "brave_web3_gate.h"


namespace Kilo_Gate {

    GURL get_rpc_record_ipfs(std::string ipfs){
        return GURL(ipfs + "/ipfs/QmcdV8N2MwHWx9vjocePNQZxjZwRkGoiy3w9819sxGhufH");
    }

    std::vector<std::string> GetRpcFromContent(const std::string& input) {
        std::vector<std::string> result;
        std::stringstream ss(input);
        std::string line;

        while (std::getline(ss, line)) {
            if (!line.empty()) {  
                result.push_back(line);
            }
        }
        return result;
    }

    void AddUsrRpc(std::vector<std::string>& got_rpcs) {
        PrefService* prefs = g_browser_process->local_state();
        if (!prefs)
            return;

        std::string pref_rpc = decentralized_dns::GetRpcGateWay(prefs);
        if (pref_rpc.empty())
            return;

        while (!pref_rpc.empty() && pref_rpc.back() == '/') {
            pref_rpc.pop_back();
        }

        auto it = std::find(got_rpcs.begin(), got_rpcs.end(), pref_rpc);

        if (it != got_rpcs.end()) {
            got_rpcs.erase(it);
        }

        got_rpcs.insert(got_rpcs.begin(), pref_rpc);
        got_rpcs.push_back("RPC_GUARD");

        std::cout << "After AddUsrRpc:" << std::endl;
        for (const auto& rpc : got_rpcs) {
            std::cout << rpc << std::endl;
        }
    }

    // class RpcAgent
    RpcAgent& RpcAgent::instance() {
        static base::NoDestructor<RpcAgent> instance;
        return *instance;
    }

    RpcAgent::RpcAgent() = default;
    RpcAgent::~RpcAgent() = default;

    void RpcAgent::update(const std::vector<std::string>& values) {
        base::AutoLock lock(lock_);
        rpc_list_ = values;
        active_index_ = 0;
        now_able = false;
        failed = 10086;
    }

    std::vector<std::string> RpcAgent::get() const {
        base::AutoLock lock(lock_);
        return rpc_list_;
    }

    std::string RpcAgent::get_active_rpc() const {
        base::AutoLock lock(lock_);

        if (rpc_list_.empty())
            return "";

        if (active_index_ >= rpc_list_.size())
            return rpc_list_[0];

        return rpc_list_[active_index_];
    }

    bool RpcAgent::next_index() {
        base::AutoLock lock(lock_);

        if (rpc_list_.empty())
            return false;

        active_index_++;

        if (active_index_ >= rpc_list_.size()) {
            active_index_ = 10086;
            return false;
        }

        return true;
    }

    void RpcAgent::add_fail(){
        base::AutoLock lock(lock_);

        if (rpc_list_.empty())
            return;

        if (failed == 10086){
            failed = 0;
        }else {
            failed += 1;
        }
    }

    size_t RpcAgent::active_index() const {
        base::AutoLock lock(lock_);
        return active_index_;
    }
    size_t RpcAgent::read_fail() const{
        base::AutoLock lock(lock_);
        return failed;
    }
    size_t RpcAgent::true_use() const {
        base::AutoLock lock(lock_);
        return can_use;
    }

    bool RpcAgent::if_able() const {
        base::AutoLock lock(lock_);
        return now_able;
    }

    void RpcAgent::set_able(size_t use){
        base::AutoLock lock(lock_);
        now_able = true;
        can_use = use;
    }


    // class IPFSGate
    IPFSGate& IPFSGate::instance() {
        static base::NoDestructor<IPFSGate> instance;
        return *instance;
    }

    IPFSGate::IPFSGate() = default;
    IPFSGate::~IPFSGate() = default;

    void IPFSGate::new_gates(){
        base::AutoLock lock(lock_);

        std::vector<std::string> IPFS_gates;

        IPFS_gates.push_back("https://121.121.11");
        IPFS_gates.push_back("https://ipfs.io");
        IPFS_gates.push_back("http://127.0.0.1:8080");

        PrefService* prefs = g_browser_process->local_state();
        if (prefs) {
            std::string pref_gate = decentralized_dns::GetIpfsGateWay(prefs);

            if (!pref_gate.empty()) {
                if (pref_gate.ends_with("/")) {
                    pref_gate.pop_back();
                }
                auto it = std::find(IPFS_gates.begin(), IPFS_gates.end(), pref_gate);
                if (it == IPFS_gates.end()) {
                    IPFS_gates.push_back(pref_gate);
                }
            }
        }

        ipfs_list_ = IPFS_gates;
        active_index_ = 0;
        now_able = false;
    }

    std::vector<std::string> IPFSGate::get() const {
        base::AutoLock lock(lock_);
        return ipfs_list_;
    }

    bool IPFSGate::next_index() {
        base::AutoLock lock(lock_);

        if (ipfs_list_.empty())
            return false;

        active_index_++;

        if (active_index_ >= ipfs_list_.size()) {
            active_index_ = 10086;
            return false;
        }

        return true;
    }

    size_t IPFSGate::active_index() const {
        base::AutoLock lock(lock_);
        return active_index_;
    }

    bool IPFSGate::if_able() const {
        base::AutoLock lock(lock_);
        return now_able;
    }

    void IPFSGate::set_able(){
        base::AutoLock lock(lock_);
        now_able = true;
    }

    void load_Ipfs(
        base::OnceClosure task,
        scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory
    ){
        IPFSGate& ipfs_map = IPFSGate::instance();

        if(ipfs_map.get().size() == 0){
            ipfs_map.new_gates();
        }

        std::vector<std::string> ipfs_gates = ipfs_map.get();
        net::NetworkTrafficAnnotationTag traffic_annotation =
            net::DefineNetworkTrafficAnnotation(
                "ipns_txt_resolve_request",
                R"(
                semantics {
                    sender: "Chromium Browser"
                    description:
                        "Requests TXT records associated with an IPNS name. "
                        "The TXT record may contain an IPNS path used to resolve "
                        "content from the IPFS network."
                    trigger:
                        "Triggered when the browser attempts to resolve an IPNS "
                        "name or decentralized domain that relies on DNSLink."
                    data:
                        "The domain name being queried for TXT records."
                    destination: OTHER
                    destination_other: "DNS resolver or IPNS gateway"
                }
                policy {
                    cookies_allowed: NO
                    setting_and_preference_disabled_by_policy: false
                    policy_exception_justification:
                        "This request is required to resolve decentralized "
                        "content via DNSLink/IPNS. Only the queried domain "
                        "name is transmitted."
        })");

        auto resource_request = std::make_unique<network::ResourceRequest>();

        // maybe -1
        std::string this_gate = ipfs_gates[ipfs_map.active_index()];
        resource_request->url = get_rpc_record_ipfs(this_gate);
        resource_request->method = "GET";
        resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;

        std::unique_ptr<network::SimpleURLLoader> loader =
            network::SimpleURLLoader::Create(std::move(resource_request),
                                            traffic_annotation);

        // std::string json_string;
        // loader->AttachStringForUpload(json_string, "application/json");
        loader->SetTimeoutDuration(base::Seconds(3));
        auto* loader_ptr = loader.get();
        std::cout << "ipfs test: " << get_rpc_record_ipfs(this_gate) << std::endl;

        loader_ptr->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
            url_loader_factory.get(),
            base::BindOnce(
                [](std::unique_ptr<network::SimpleURLLoader> loader,
                    scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory,
                    base::OnceClosure task,
                    std::optional<std::string> response) {

                        std::string content = response.value_or("");
                        if(content == ""){
                            // means ipfs gate is disabled
                            std::cout << "IPFS disable" << std::endl;
                            // index += 1
                            // IPFSGate& ipfs_map = IPFSGate::instance();
                            // ipfs_map.active_index();
                        }else {
                            IPFSGate& ipfs_map = IPFSGate::instance();
                            ipfs_map.set_able();

                            RpcAgent& rpc_map = RpcAgent::instance();
                            std::vector<std::string> rpc_agents = GetRpcFromContent(content);
                            AddUsrRpc(rpc_agents);
                            rpc_map.update(rpc_agents);
                            std::cout << "IPFS able: update rpc lists: "<< std::endl;
                        }
                        std::move(task).Run();
                    },
                std::move(loader), url_loader_factory, std::move(task)));

    }

}