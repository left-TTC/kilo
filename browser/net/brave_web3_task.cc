

#include "brave/browser/net/brave_web3_task.h"



namespace Brave_web3_solana_task{

    DomainCidMap::DomainCidMap() = default;
    DomainCidMap::~DomainCidMap() = default;

    DomainCidMap& DomainCidMap::instance() {
        static base::NoDestructor<DomainCidMap> instance;
        return *instance;
    }

    void process_web3_domain_internal(
        const GURL& domain,
        base::OnceCallback<void(const GURL&, bool is_web3_domain)> restart_callback,
        scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory
    ) {
        Solana_Rpc::SolanaRootMap& rootMap = Solana_Rpc::SolanaRootMap::instance();
        std::vector<std::string> all_root_domains = rootMap.get_all();

        const auto [maybe_web3_domain, _] = Solana_web3::extract_target_domain(domain);
        auto [index, found, pre_domain] = Solana_web3::fast_find(maybe_web3_domain, all_root_domains);

        if(!found){
            std::move(restart_callback).Run(domain, false);
            return;
        }

        DomainCidMap& domain_cid_map = DomainCidMap::instance();
        const absl::optional<Solana_Rpc::DecodeResult> schroding_cid = domain_cid_map.get_result(maybe_web3_domain);
        if(schroding_cid.has_value()){
            std::move(restart_callback).Run(return_url_from_cid(schroding_cid.value()), true);
            return;
        }

        const std::vector<Solana_web3::Pubkey> roots = rootMap.get_all_pubkey();
        const Solana_web3::Pubkey this_root = roots[index];

        Solana_web3::PDA domain_ipfs_key = Solana_web3::Solana_web3_interface::get_account_from_root(pre_domain, this_root);

        LOG(INFO) << "domain ipfs key: " << domain_ipfs_key.publickey.toBase58();

        const Solana_web3::Pubkey ipfs_pubkey = domain_ipfs_key.publickey;
        ipfs_pubkey.get_pubkey_ipfs(url_loader_factory, std::move(restart_callback), maybe_web3_domain, domain);
    }
    
    void handle_web3_domain(
        const GURL& domain,
        base::OnceCallback<void(const GURL&, bool is_web3_domain)> restart_callback,
        content::BrowserContext* browser_context
    ){
        Solana_Rpc::SolanaRootMap& rootMap = Solana_Rpc::SolanaRootMap::instance();

        // 尽早提取智能指针工厂，后续异步操作全部使用工厂，不再碰 browser_context
        auto* storage_partition = browser_context->GetDefaultStoragePartition();
        scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory =
            storage_partition->GetURLLoaderFactoryForBrowserProcess();

        if(!rootMap.has_loaded){
            // 拦截并发请求，防止同时触发多次 rootMap 初始化
            if (rootMap.is_loading) {
                // 当前正有一个请求在加载 RootMap，为避免并发冲突和内存泄露，
                // 对于这些撞车请求，我们直接让它们走非 Web3 域名的回退路径。
                std::move(restart_callback).Run(domain, false);
                return;
            }
            
            rootMap.is_loading = true; // 上锁
            std::cout << "will init the root map !!!"  << std::endl;

            // 注意这里的闭包捕获：只传 url_loader_factory，不传 browser_context
            base::OnceClosure task = base::BindOnce(
                [](Solana_Rpc::SolanaRootMap* root_map,
                const GURL& domain,
                base::OnceCallback<void(const GURL&, bool)> restart_callback,
                scoped_refptr<network::SharedURLLoaderFactory> factory) {

                    // 1. 设置加载完成状态（解除并发锁）
                    root_map->set_loaded(true); 
                    
                    // 2. 调用核心逻辑处理当前的 Web3 域名
                    process_web3_domain_internal(domain, std::move(restart_callback), factory);
                },
                &rootMap,
                domain,
                std::move(restart_callback),
                url_loader_factory // <-- 核心修复：传递安全的 scoped_refptr 智能指针
            );
            
            Solana_Rpc::get_all_root_domain(url_loader_factory, std::move(task));
        } else {
            // 已经加载完毕，直接处理
            process_web3_domain_internal(domain, std::move(restart_callback), url_loader_factory);
        }
    }


    // https://search.brave.com/search?q=x.web3

    // this is a important function
    // if there are not correspond setting
    // the brower will crashed or the net service will crashed
    void redirect_request(
        network::ResourceRequest* modified_request,
        const GURL& ipfs_url
    ) {
        net::IsolationInfo::RequestType request_type;

        if (modified_request->trusted_params &&
            modified_request->trusted_params->isolation_info.request_type() ==
                net::IsolationInfo::RequestType::kMainFrame) {
            request_type = request_type = net::IsolationInfo::RequestType::kMainFrame;
            LOG(INFO) << "main frame";
        }else if (modified_request->resource_type ==
                static_cast<int>(blink::mojom::ResourceType::kMainFrame)) {
            request_type = request_type = net::IsolationInfo::RequestType::kMainFrame;
            LOG(INFO) << "main frame";
        }else {
            request_type = request_type = net::IsolationInfo::RequestType::kOther;
            LOG(INFO) << "other frame";
        }

        std::string path = std::string(modified_request->url.PathForRequest());
        if (modified_request->url.has_ref()) {
            path += "#";
            path += modified_request->url.ref();
        }
        std::cout << "the origin domain's path: " << path << std::endl;
        // get the real IPFS url
        std::string full_ipfs_url = ipfs_url.spec();
        if (!path.empty()) {
            if (full_ipfs_url.back() != '/' && path.front() != '/') {
                full_ipfs_url += '/';
            }
            if (full_ipfs_url.back() == '/' && path.front() == '/'){
                full_ipfs_url.pop_back(); 
            }

            LOG(INFO) << "this path: " << path;
            full_ipfs_url += path;
        }

        std::cout << "the last domain's url: " << full_ipfs_url << std::endl;
        modified_request->url = GURL(full_ipfs_url);

        // when the type is mainframe => the origin must be the xxx.kilo
        if (request_type == net::IsolationInfo::RequestType::kMainFrame) {
            if (!modified_request->trusted_params) {
                // if npos => create a trusted_params
                modified_request->trusted_params.emplace();
            }

            auto main_frame_origin = url::Origin::Create(GURL(ipfs_url));

            // main frame site_for_cookie set to null
            modified_request->site_for_cookies =
                net::SiteForCookies::FromOrigin(main_frame_origin);

            // by the policy => the request's isolation.cookies mut be equal to 
            // main frame cookie
            modified_request->trusted_params->isolation_info =
                net::IsolationInfo::Create(
                    request_type,
                    main_frame_origin,  // top_frame_origin
                    main_frame_origin,  // frame_origin
                    net::SiteForCookies::FromOrigin(main_frame_origin)
                );
        }
        
    }

    GURL return_url_from_cid(const Solana_Rpc::DecodeResult& result){

        std::string new_url;

        Kilo_Gate::IPFSGate& ipfs_map = Kilo_Gate::IPFSGate::instance();
        size_t index = ipfs_map.active_index();
        if(ipfs_map.if_able() && index != 10086){
            new_url = ipfs_map.get()[index];
        }else{
            new_url = "https://127.0.0.1:8080";
        }
        
        switch(result.record_type){
            case Solana_Rpc::RecordType::IPFS:
                new_url += "/ipfs/";
                break;
            case Solana_Rpc::RecordType::IPNS:
                new_url += "/ipns/";
                break;
            default:
                new_url += "/ipfs/";
                break;
        }

        new_url += result.decoded;
        return GURL(new_url);
    }


    void omnibox_match_judge(
        GURL& frist_destination_url
    ){

        Solana_Rpc::SolanaRootMap& rootMap = Solana_Rpc::SolanaRootMap::instance();
        std::vector<std::string> all_root_domains =  rootMap.get_all();

        const auto [maybe_web3_domain, if_search] = Solana_web3::extract_target_domain(frist_destination_url);
        LOG(INFO) << "extract target:" << maybe_web3_domain;

        if(if_search){
            if(all_root_domains.size() == 0 && !rootMap.has_loaded){
                LOG(INFO) << "ominibox_match_judge: no the root";
                return;
            }

            for(const auto& root: all_root_domains){
                LOG(INFO) << "omnibox root:" << root;
            }
            
            std::vector<std::string> spilit_frist_destination = Solana_web3::split_host_by_dots(maybe_web3_domain);

            auto [index, found, pre_domain] = Solana_web3::fast_find(maybe_web3_domain, all_root_domains);

            if(!found){
                LOG(INFO) << "ominibox_match_judge: not web3";
                return;
            }

            frist_destination_url = GURL("https://" + maybe_web3_domain);
            LOG(INFO) << "ominibox_match_judge: return" << frist_destination_url;
        }
    }

}

