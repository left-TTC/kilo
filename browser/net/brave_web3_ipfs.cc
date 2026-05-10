#include "brave/browser/net/brave_web3_ipfs.h"

#include <string.h>

namespace Kilo_Ipfs {

namespace {

    const char* kBase58Alphabet =
        "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

    const char* kBase32Alphabet =
        "abcdefghijklmnopqrstuvwxyz234567";

    }  // namespace

    bool DecodeBase58(const std::string& str,
                    std::vector<uint8_t>* out) {
        out->clear();

        if (str.empty())
            return false;

        std::vector<uint8_t> b256(
            (str.size() * 733) / 1000 + 1);

        for (char ch : str) {

            const char* p1 = strchr(kBase58Alphabet, ch);

            if (!p1)
                return false;

            int carry = static_cast<int>(p1 - kBase58Alphabet);

            for (auto it = b256.rbegin();
                it != b256.rend();
                ++it) {

                carry += 58 * (*it);

                *it = carry % 256;

                carry /= 256;
            }

            if (carry != 0)
                return false;
        }

        auto it = b256.begin();

        while (it != b256.end() && *it == 0)
            ++it;

        out->assign(it, b256.end());

        return true;
    }

    std::string Base32EncodeLower(
        const std::vector<uint8_t>& data) {

        std::string out;

        int buffer = 0;
        int bits_left = 0;

        for (uint8_t byte : data) {

            buffer <<= 8;
            buffer |= byte & 0xff;

            bits_left += 8;

            while (bits_left >= 5) {
                int index = (buffer >> (bits_left - 5)) & 0x1f;

                bits_left -= 5;

                out += kBase32Alphabet[index];
            }
        }

        if (bits_left > 0) {
            int index = (buffer << (5 - bits_left)) & 0x1f;

            out += kBase32Alphabet[index];
        }

        return out;
    }

    bool IsCIDv0(const std::string& cid){

        if (cid.size() != 46)
            return false;

        if (cid[0] != 'Q' || cid[1] != 'm')
            return false;

        for (size_t i = 2; i < cid.size(); ++i) {
            char c = cid[i];

            bool ok =
                (c >= '1' && c <= '9') ||
                (c >= 'A' && c <= 'H') ||
                (c >= 'J' && c <= 'N') ||
                (c >= 'P' && c <= 'Z') ||
                (c >= 'a' && c <= 'k') ||
                (c >= 'm' && c <= 'z');

            if (!ok)
                return false;
        }

        return true;
    }

    std::string CIDv0ToV1(
        const std::string& cidv0) {

        std::vector<uint8_t> multihash;

        if (!DecodeBase58(cidv0, &multihash))
            return "";

        // CIDv0 multihash should be 34 bytes:
        // 0x12 0x20 + 32-byte sha256
        if (multihash.size() != 34)
            return "";

        std::vector<uint8_t> cidv1;

        // CID version 1
        cidv1.push_back(0x01);

        // dag-pb codec
        cidv1.push_back(0x70);

        // append multihash
        cidv1.insert(
            cidv1.end(),
            multihash.begin(),
            multihash.end());

        // multibase base32 lowercase prefix = 'b'
        return "b" + Base32EncodeLower(cidv1);
    }

    GURL get_gateway_record(std::string ipfs){
        return GURL(ipfs + "/ipns/k51qzi5uqu5dgntpmvekgw8nnjownrzxzoyq3wzrrzt30min5p0m2sxniur6pl");
    }


    ActiveCanonical& ActiveCanonical::instance() {
        static base::NoDestructor<ActiveCanonical> instance;
        return *instance;
    }

    ActiveCanonical::ActiveCanonical() = default;
    ActiveCanonical::~ActiveCanonical() = default;

    bool ActiveCanonical::LoadFromTxt(const std::string& txt, const std::string using_gate){

        std::istringstream stream(txt);
        std::string line;

        bool found = false;

        std::string canonical_result;

        while (std::getline(stream, line)) {

            if (line.empty()) continue;

            auto eq_pos = line.find('=');
            if (eq_pos != std::string::npos) {

                std::string key = line.substr(0, eq_pos);
                std::string value = line.substr(eq_pos + 1);

                if (key == "GATEWAY_URL") {
                    std::istringstream gw_stream(value);
                    std::string gw;

                    while (std::getline(gw_stream, gw, ',')) {
                        if (gw == using_gate) {
                            std::cout << "Find using gateway" << using_gate << std::endl;
                            found = true;
                            break;
                        }else {
                            std::cout << "No using gateway" << std::endl;
                        }
                    }
                }

                if (key == "CANONICAL_URL") {
                    canonical_result = value;
                    std::cout << "Using gateway's canonial: " << value << std::endl;
                    if(found) break;
                }
            }
        }
        {
            base::AutoLock guard(lock_);
            if (found) {
                gateWayUrl_ = canonical_result;
            }
        }

        return found;
    }

    std::string ActiveCanonical::ReturnSubdomain(){
        std::cout << "return subdomain" << std::endl;

        base::AutoLock lock(lock_);
        return gateWayUrl_;
    }

    void ActiveCanonical::SetCanonicalUsablityChecked(bool if_support){
        base::AutoLock lock(lock_);
        has_canonical_usablity_checked_ = if_support;
    }

    bool ActiveCanonical::GetCanonicalUsablityChecked() const {
        base::AutoLock lock(lock_);
        return has_canonical_usablity_checked_;
    }


    void ActiveCanonical::SetChecked(){
        base::AutoLock lock(lock_);
        checked_subdomain_ = true;
    }

    bool ActiveCanonical::GetChecked(){
        base::AutoLock lock(lock_);
        return checked_subdomain_;
    }

    void ActiveCanonical::SetCanonicalUsablity(bool if_pass){
        base::AutoLock lock(lock_);
        canonical_usablity_ = if_pass;
    }

    bool ActiveCanonical::GetCanonicalUsablity(){
        base::AutoLock lock(lock_);
        return canonical_usablity_;
    }

    GURL RedirectIpfs(std::string direct_url, std::string gateWayUrl){
        GURL original(direct_url);
        GURL gateway(gateWayUrl);

        std::cout << "Url In: " << direct_url << std::endl;

        if (!original.is_valid() || !gateway.is_valid())
            return GURL();

        // path:
        // /ipns/<name>
        // /ipfs/<cid>
        std::string path = std::string(original.path());

        constexpr char kIpnsPrefix[] = "/ipns/";
        constexpr char kIpfsPrefix[] = "/ipfs/";

        std::string type;
        std::string value;

        if (path.rfind(kIpnsPrefix, 0) == 0) {

            type = "ipns";
            value = path.substr(strlen(kIpnsPrefix));

        } else if (path.rfind(kIpfsPrefix, 0) == 0) {

            type = "ipfs";
            value = path.substr(strlen(kIpfsPrefix));

        } else {

            return GURL();
        }

        // remove trailing slash
        while (!value.empty() && value.back() == '/') {
            value.pop_back();
        }

        // dweb.link
        std::string host = std::string(gateway.host());

        // final:
        // <value>.<type>.<host>
        std::string final_url = std::string(gateway.scheme())  + "://" + value + "." + type + "." + host;

        std::cout << "URL Out: " << final_url << std::endl;

        return GURL(final_url);
    }

}  // namespace Kilo_Ipfs