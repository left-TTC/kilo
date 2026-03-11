#ifndef BRAVE_WEB3_GATE_H_
#define BRAVE_WEB3_GATE_H_

#include "url/gurl.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#include "base/synchronization/lock.h"
#include "base/no_destructor.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/browser_process.h"
#include "brave/components/decentralized_dns/core/utils.h"
#include "services/network/public/cpp/resource_request.h"
#include "services/network/public/cpp/simple_url_loader.h"
#include "services/network/public/cpp/shared_url_loader_factory.h"
#include "services/network/public/mojom/url_loader_factory.mojom.h"
#include "base/json/json_reader.h"     
#include "base/json/json_writer.h"    
#include "base/logging.h"             
#include "base/values.h"
#include "base/functional/callback.h"
#include "services/network/public/mojom/url_response_head.mojom.h"

#include "base/json/json_reader.h" 

#include "base/json/json_writer.h"
#include "base/logging.h"
#include "base/memory/scoped_refptr.h"

namespace Kilo_Gate {

    GURL get_rpc_record_ipfs(std::string ipfs);

    std::vector<std::string> GetRpcFromContent(const std::string& input);

    void AddUsrRpc(std::vector<std::string>& got_rpcs);

    class RpcAgent {
    public:
        static RpcAgent& instance();

        void update(const std::vector<std::string>& values);

        std::vector<std::string> get() const;

        std::string get_active_rpc() const;

        bool next_index();
        void add_fail();

        size_t active_index() const;
        size_t read_fail() const;
        size_t true_use() const;

        bool if_able() const;
        void set_able(size_t use);

    private:
        friend class base::NoDestructor<RpcAgent>;
        RpcAgent();
        ~RpcAgent();

        mutable base::Lock lock_;
        std::vector<std::string> rpc_list_;
        size_t active_index_ = 0;
        size_t can_use = 10086;
        size_t failed = 10086;
        bool now_able = false;
    };

    // // load rpc agent website
    // std::vector<std::string> load_rpc_gate();

    class IPFSGate {
    public:
        static IPFSGate& instance();

        // new ipfa gate lists
        void new_gates();

        std::vector<std::string> get() const;

        bool next_index();

        size_t active_index() const;
        size_t ture_index() const;

        bool if_able() const;
        void set_able();

    private:
        friend class base::NoDestructor<IPFSGate>;
        IPFSGate();
        ~IPFSGate();

        mutable base::Lock lock_;
        std::vector<std::string> ipfs_list_;
        size_t active_index_ = 0;
        // means the active index ipfs gateway is abled
        bool now_able = false;
    };

    void load_Ipfs(
        base::OnceClosure task,
        scoped_refptr<network::SharedURLLoaderFactory> url_loader_factory
    );
}



#endif