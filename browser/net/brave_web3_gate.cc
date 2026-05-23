
#include "brave_web3_gate.h"
#include "brave_web3_ipfs.h"

namespace Kilo_Gate {

    GURL get_rpc_record_ipfs(std::string ipfs){
        return GURL(ipfs + "/ipns/k51qzi5uqu5dj0nlwyfdamszzht21c52x6bgzs7kfp0hk7bca0q6ktggh6kyl7");
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
        auto normalize = [](std::string s) {
            auto trim = [](std::string& str) {
                str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
                str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }).base(), str.end());
            };

            trim(s);

            while (!s.empty() && s.back() == '/') {
                s.pop_back();
            }

            return s;
        };

        PrefService* prefs = g_browser_process->local_state();
        if (!prefs)
            return;

        std::string pref_rpc = decentralized_dns::GetRpcGateWay(prefs);
        pref_rpc = normalize(pref_rpc);

        if (pref_rpc.empty())
            return;

        std::unordered_set<std::string> seen;
        std::vector<std::string> deduped;

        for (const auto& rpc : got_rpcs) {
            std::string norm = normalize(rpc);

            if (norm.empty())
                continue;

            if (seen.insert(norm).second) {
                deduped.push_back(norm);
            }
        }

        got_rpcs = std::move(deduped);

        got_rpcs.erase(
            std::remove(got_rpcs.begin(), got_rpcs.end(), pref_rpc),
            got_rpcs.end()
        );

        got_rpcs.insert(got_rpcs.begin(), pref_rpc);

        const std::string kGuard = "RPC_GUARD";

        if (std::find(got_rpcs.begin(), got_rpcs.end(), kGuard) == got_rpcs.end()) {
            got_rpcs.push_back(kGuard);
        }

        std::cout << "After AddUsrRpc:" << std::endl;
        for (const auto& rpc : got_rpcs) {
            std::cout << "[" << rpc << "]" << std::endl;
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

    bool RpcAgent::IfReloaded() const{
        base::AutoLock lock(lock_);
        return new_tab_page_reloaded;
    }
    
    void RpcAgent::SetReloaded(){
        base::AutoLock lock(lock_);
        new_tab_page_reloaded = true;
    }


    // class IPFSGate
    IPFSGate& IPFSGate::instance() {
        static base::NoDestructor<IPFSGate> instance;
        return *instance;
    }

    IPFSGate::IPFSGate() = default;
    IPFSGate::~IPFSGate() = default;

    void IPFSGate::new_gates() {
        base::AutoLock lock(lock_);

        auto normalize = [](std::string s) {
            // trim
            auto trim = [](std::string& str) {
                str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
                str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }).base(), str.end());
            };

            trim(s);

            while (!s.empty() && s.back() == '/') {
                s.pop_back();
            }

            return s;
        };

        std::vector<std::string> IPFS_gates;

        IPFS_gates.push_back(normalize("http://127.0.0.1:8888"));
        IPFS_gates.push_back(normalize("https://ipfs.io"));

        PrefService* prefs = g_browser_process->local_state();
        if (prefs) {
            std::string pref_gate = normalize(decentralized_dns::GetIpfsGateWay(prefs));

            if (!pref_gate.empty()) {
                auto it = std::find_if(IPFS_gates.begin(), IPFS_gates.end(),
                    [&](const std::string& gate) {
                        return normalize(gate) == pref_gate;
                    });

                if (it == IPFS_gates.end()) {
                    IPFS_gates.push_back(pref_gate);
                }
            }
        }

        IPFS_gates.push_back(normalize("IPFS_GUARD"));

        std::unordered_set<std::string> seen;
        std::vector<std::string> deduped;

        for (auto& gate : IPFS_gates) {
            std::string norm = normalize(gate);
            if (seen.insert(norm).second) {
                deduped.push_back(norm);
            }
        }

        ipfs_list_ = std::move(deduped);
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

    bool IPFSGate::last_index() {
        base::AutoLock lock(lock_);

        if (ipfs_list_.empty())
            return false;
        
        if(active_index_ == 0 || active_index_==10086) return false;

        active_index_--;

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

        // This annotation can be used twice
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

        // frist get the ipfsgateway reflection table get useable ipfs gateway
        Kilo_Ipfs::ActiveCanonical& instance = Kilo_Ipfs::ActiveCanonical::instance();

        if(!instance.GetChecked()){
            // maybe -1
            std::string this_gate = ipfs_gates[ipfs_map.active_index()];

            auto resource_request = std::make_unique<network::ResourceRequest>();
            resource_request->url = Kilo_Ipfs::get_gateway_record(this_gate);
            resource_request->method = "GET";
            resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;

            std::cout << "Get reflection table from " << Kilo_Ipfs::get_gateway_record(this_gate) << std::endl;

            std::unique_ptr<network::SimpleURLLoader> loader =
                network::SimpleURLLoader::Create(std::move(resource_request),
                                                traffic_annotation);

            loader->SetTimeoutDuration(base::Seconds(5));
            auto* loader_ptr = loader.get();

            loader_ptr->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
                url_loader_factory.get(),
                base::BindOnce(
                    [](std::unique_ptr<network::SimpleURLLoader> loader,
                        base::OnceClosure task,
                        std::string this_gate,
                        std::optional<std::string> response) {

                            std::string content = response.value_or("");
                            if(content == ""){
                                std::cout << "Current IPFS disable" << std::endl;
                            }else {
                                IPFSGate& ipfs_map = IPFSGate::instance();
                                ipfs_map.set_able();

                                Kilo_Ipfs::ActiveCanonical& instance = Kilo_Ipfs::ActiveCanonical::instance();
                                instance.LoadFromTxt(content, this_gate);
                                instance.SetChecked();
                            }
                            return std::move(task).Run();
                        },
                    std::move(loader), std::move(task), this_gate)
            );
        }else {
            std::cout << "Loaded Ipfs gateway, now test for subdomain" << std::endl;
            // last loop make index += 1, we should sub 1 here
            std::string this_gate = ipfs_gates[ipfs_map.active_index() - 1];
            ipfs_map.last_index();

            auto resource_request = std::make_unique<network::ResourceRequest>();

            std::string sub_link = instance.ReturnSubdomain();

            GURL direct_url = get_rpc_record_ipfs(this_gate);
            if(sub_link.size() > 0 && !instance.GetCanonicalUsablityChecked()){
                // The gateway has subdomain function and not checked
                GURL rpc_request_site = Kilo_Ipfs::RedirectIpfs(direct_url.spec(), sub_link);

                resource_request->url = rpc_request_site;
                resource_request->method = "GET";
                resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;

                std::unique_ptr<network::SimpleURLLoader> loader =
                    network::SimpleURLLoader::Create(std::move(resource_request),
                                                    traffic_annotation);

                // std::string json_string;
                // loader->AttachStringForUpload(json_string, "application/json");
                loader->SetTimeoutDuration(base::Seconds(5));
                auto* loader_ptr = loader.get();

                loader_ptr->DownloadToStringOfUnboundedSizeUntilCrashAndDie(
                    url_loader_factory.get(),
                    base::BindOnce(
                        [](std::unique_ptr<network::SimpleURLLoader> loader,
                            scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory,
                            base::OnceClosure task,
                            std::optional<std::string> response) {

                                std::string content = response.value_or("");
                                Kilo_Ipfs::ActiveCanonical& instance = Kilo_Ipfs::ActiveCanonical::instance();

                                if(content == ""){
                                    // means ipfs gate is disabled
                                    std::cout << "Subdomain can't be accessed" << std::endl;
                                }else {

                                    std::cout << "Subdomain AVAILABLE" << std::endl;
                                    
                                    RpcAgent& rpc_map = RpcAgent::instance();
                                    std::vector<std::string> rpc_agents = GetRpcFromContent(content);
                                    AddUsrRpc(rpc_agents);
                                    rpc_map.update(rpc_agents);
                                    std::cout << "IPFS able: update rpc lists: "<< std::endl;
                                    
                                    // set sub can be used
                                    instance.SetCanonicalUsablity(true);
                                }
                                // set checked
                                instance.SetCanonicalUsablityChecked(true);
                                std::move(task).Run();
                            },
                        std::move(loader), url_loader_factory, std::move(task)));

            }else {
                // no subdomain visit or visit can't be used
                resource_request->url = direct_url;
                resource_request->method = "GET";
                resource_request->credentials_mode = network::mojom::CredentialsMode::kOmit;

                std::unique_ptr<network::SimpleURLLoader> loader =
                    network::SimpleURLLoader::Create(std::move(resource_request),
                                                    traffic_annotation);

                // std::string json_string;
                // loader->AttachStringForUpload(json_string, "application/json");
                loader->SetTimeoutDuration(base::Seconds(5));
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
                                Kilo_Ipfs::ActiveCanonical& instance = Kilo_Ipfs::ActiveCanonical::instance();

                                if(content == ""){
                                    // means ipfs gate is disabled
                                    std::cout << "Direct can't be accessed" << std::endl;
                                }else {
                                    std::cout << "Subdomain UNAVAILABLE, use Directly" << std::endl;

                                    RpcAgent& rpc_map = RpcAgent::instance();
                                    std::vector<std::string> rpc_agents = GetRpcFromContent(content);
                                    AddUsrRpc(rpc_agents);
                                    rpc_map.update(rpc_agents);
                                    std::cout << "IPFS able: update rpc lists: "<< std::endl;

                                    // set sub can't be used
                                    instance.SetCanonicalUsablity(false);
                                }
                                instance.SetCanonicalUsablityChecked(true);
                                std::move(task).Run();
                            },
                        std::move(loader), url_loader_factory, std::move(task)));
            }

        }
    }

}