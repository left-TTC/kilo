

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

        auto* storage_partition = browser_context->GetDefaultStoragePartition();
        scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory =
            storage_partition->GetURLLoaderFactoryForBrowserProcess();

        if(!rootMap.has_loaded){
            if (rootMap.is_loading) {
                std::move(restart_callback).Run(domain, false);
                return;
            }
            
            rootMap.is_loading = true; 
            std::cout << "will init the root map !!!"  << std::endl;

            base::OnceClosure task = base::BindOnce(
                [](Solana_Rpc::SolanaRootMap* root_map,
                const GURL& domain,
                base::OnceCallback<void(const GURL&, bool)> restart_callback,
                scoped_refptr<network::SharedURLLoaderFactory> factory) {

                    root_map->set_loaded(true); 
                    
                    process_web3_domain_internal(domain, std::move(restart_callback), factory);
                },
                &rootMap,
                domain,
                std::move(restart_callback),
                url_loader_factory 
            );
            
            Solana_Rpc::get_all_root_domain(url_loader_factory, std::move(task));
        } else {
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
            new_url = "https://127.0.0.1:8888";
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

        Kilo_Ipfs::ActiveCanonical& instance = Kilo_Ipfs::ActiveCanonical::instance();
        std::string canonial = instance.ReturnSubdomain();

        GURL last_url;

        if(canonial.size()>0 && instance.GetCanonicalUsablity()){
            std::cout << "Canonial is avaliable, use canonial" << std::endl;
            if(Kilo_Ipfs::IsCIDv0(result.decoded)){
                new_url += Kilo_Ipfs::CIDv0ToV1(result.decoded);
            }else{
                new_url += result.decoded;
            }

            last_url = Kilo_Ipfs::RedirectIpfs(new_url, canonial);
        }else{
            new_url += result.decoded;
            last_url = GURL(new_url);
        }

        return last_url;
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

    KiloTips::KiloTips() = default;
    KiloTips::~KiloTips() = default;

    KiloTips& KiloTips::instance() {
        static base::NoDestructor<KiloTips> instance;
        return *instance;
    }

    bool KiloTips::CheckClocked() const{
        base::AutoLock lock(lock_);
        return clocked_;
    }
    void KiloTips::SetClocked(){
        base::AutoLock lock(lock_);
        clocked_ = true;
    }

    std::string GetKiloTipsAlert() {
    // 使用 R"JS(...)JS" 定义原始字符串
    return R"JS(
        (function() {
            // 1. 获取语言和文案
            const lang = (navigator.language || navigator.userLanguage || 'en').toLowerCase();
            let msg = 'Getting Kilo service, please do not close the page until it is stable.';
            
            if (lang.startsWith('zh')) {
                msg = '正在获取 Kilo 服务，请不要在页面稳定之前关闭页面。';
            } else if (lang.startsWith('es')) {
                msg = 'Obteniendo el servicio Kilo, por favor no cierre la página hasta que esté estable.';
            } else if (lang.startsWith('fr')) {
                msg = 'Obtention du service Kilo, veuillez ne pas fermer la page avant qu\'elle ne soit stable.';
            } else if (lang.startsWith('ja')) {
                msg = 'Kilo サービスを取得しています。ページが安定するまで閉じないでください。';
            } else if (lang.startsWith('ko')) {
                msg = 'Kilo 서비스를 가져오는 중입니다. 페이지가 안정될 때까지 닫지 마십시오.';
            } else if (lang.startsWith('de')) {
                msg = 'Kilo-Dienst wird abgerufen, bitte schließen Sie die Seite nicht, bevor sie stabil ist.';
            } else if (lang.startsWith('ru')) {
                msg = 'Получение службы Kilo, пожалуйста, не закрывайте страницу, пока она не стабилизируется.';
            } else if (lang.startsWith('pt')) {
                msg = 'Obtendo o serviço Kilo, por favor, não feche a página até que ela esteja estável.';
            } else if (lang.startsWith('ar')) {
                msg = 'جاري الحصول على خدمة Kilo، يرجى عدم إغلاق الصفحة حتى تستقر.';
            } else if (lang.startsWith('hi')) {
                msg = 'Kilo सेवा प्राप्त की जा रही है, कृपया पृष्ठ के स्थिर होने तक इसे बंद न करें।';
            } else if (lang.startsWith('it')) {
                msg = 'Ottenimento del servizio Kilo in corso, si prega di non chiudere la pagina finché non è stabile.';
            }

            // 2. 确保 document body 存在（有些极速跳转的请求可能 body 还没准备好）
            if (!document.body) {
                console.warn('Body not ready for Kilo alert');
                return;
            }

            // 3. 动态创建自定义提示框 (Toast UI)
            const alertBox = document.createElement('div');
            alertBox.innerText = msg;
            
            // 设置 CSS 样式，使其居中显示在顶部，且具有 Brave 风格的橙色提示
            Object.assign(alertBox.style, {
                position: 'fixed',
                top: '20px',
                left: '50%',
                transform: 'translateX(-50%)',
                backgroundColor: '#222', 
                color: '#ffffff',
                padding: '12px 24px',
                borderRadius: '8px',
                boxShadow: '0 4px 12px rgba(0, 0, 0, 0.15)',
                zIndex: '2147483647', 
                fontFamily: 'system-ui, -apple-system, sans-serif',
                fontSize: '14px',
                fontWeight: '500',
                textAlign: 'center',
                opacity: '0',
                transition: 'opacity 0.3s ease-in-out, top 0.3s ease-in-out',
                pointerEvents: 'none' // 防止阻挡用户点击下面的元素
            });

            document.body.appendChild(alertBox);

            // 触发淡入动画 (需要一点点延迟让浏览器应用初始样式)
            requestAnimationFrame(() => {
                requestAnimationFrame(() => {
                    alertBox.style.opacity = '1';
                    alertBox.style.top = '30px';
                });
            });

            // 4. 设定 3 秒 (3000ms) 后触发淡出并移除
            setTimeout(() => {
                alertBox.style.opacity = '0';
                alertBox.style.top = '20px';
                
                // 等待淡出动画结束 (300ms) 后将元素从 DOM 中彻底删除
                setTimeout(() => {
                    if (alertBox.parentNode) {
                        alertBox.parentNode.removeChild(alertBox);
                    }
                }, 300);
            }, 3000);
            
        })();
    )JS";
}
}

