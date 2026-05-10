#ifndef BRAVE_WEB3_IPFS_H_
#define BRAVE_WEB3_IPFS_H_

#pragma once

#if defined(__clang__)
    #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <regex>

#include "url/gurl.h"

#include "base/synchronization/lock.h"
#include "base/memory/scoped_refptr.h"
#include "base/no_destructor.h"

namespace Kilo_Ipfs {

    bool IsCIDv0(const std::string& cid);

    // Convert CIDv0 (Qm...) to CIDv1 (bafy...)
    std::string CIDv0ToV1(const std::string& cidv0);

    // Record the ipfs gateway reflection
    GURL get_gateway_record(std::string ipfs);

    class ActiveCanonical {
    public:
        static ActiveCanonical& instance();

        bool LoadFromTxt(const std::string& txt, const std::string using_gate);

        std::string ReturnSubdomain();

        void SetCanonicalUsablityChecked(bool if_support);

        bool GetCanonicalUsablityChecked() const;

        void SetChecked();

        bool GetChecked();

        void SetCanonicalUsablity(bool if_pass);

        bool GetCanonicalUsablity();

    private:
        friend class base::NoDestructor<ActiveCanonical>;
        ActiveCanonical();
        ~ActiveCanonical();

        mutable base::Lock lock_;

        // the canonical url
        std::string gateWayUrl_;
        // wheatehr the subdomain usable
        bool has_canonical_usablity_checked_ = false;
        // wheather reflection table checked
        bool checked_subdomain_ = false;
        // wheather the gateWayUrl_ can be used
        bool canonical_usablity_ = false;
    };

    GURL RedirectIpfs(std::string direct_url, std::string gateWayUrl);

}  // namespace Kilo_Ipfs

#endif  // BRAVE_WEB3_IPFS_H_