#ifndef BITCOIN_PRIMITIVES_SUPPORT_H
#define BITCOIN_PRIMITIVES_SUPPORT_H

#include <uint256.h>
#include <serialize.h>
#include <key.h>
#include <hash.h>
#include <vector>
#include <string>
#include <stdint.h>
#include <util/strencodings.h>
#include <iostream>

class CSupportTicket {
public:
    std::string supportedHash;
    std::string workerPubKey;
    std::string supportPubKey;
    uint32_t timestamp;
    uint32_t nonce;

    CSupportTicket(std::string supportedHashIn, std::string workerPubKeyIn, std::string supportPubKeyIn, uint32_t timestampIn, uint32_t nonceIn)
        : supportedHash(supportedHashIn), workerPubKey(workerPubKeyIn), supportPubKey(supportPubKeyIn), timestamp(timestampIn), nonce(nonceIn) {}
    
    // Default constructor
    CSupportTicket() = default;

    void SetNull() {
        supportedHash.clear();
        workerPubKey.clear();
        supportPubKey.clear();
        nonce = 0;
    }

    bool IsNull() const {
        return supportedHash.empty();
    }

    uint256 GetHash() const {
        HashWriter ss;
        ss << supportedHash << workerPubKey << supportPubKey << timestamp << nonce;
        // std::cout << "\nsupported Hash:\n";
        // std::cout << HexStr(supportedHash);
        // std::cout << "\ng_worker_pubkey: \n";
        // std::cout << HexStr(workerPubKey);
        // std::cout << "\ng_support_pubkey: \n";
        // std::cout << HexStr(supportPubKey);
        // std::cout << "\nTimestamp: \n";
        // std::cout << timestamp;
        // std::cout << "\nNonce: \n";
        // std::cout << nonce;
        // std::cout << "\nHash: \n";
        // std::cout << HexStr(ss.GetHash().GetHex());
        // std::cout << std::endl;
        return ss.GetHash();
    }

    void SetNonce(uint32_t nonceIn)
    {
        nonce = nonceIn;
    }

    bool VerifyPoW(int amount) const {
        // reconstruct the solution
        uint256 powHash = GetHash();
        // count the zeros
        int count = 0;
        for (int i = 0; i < 32; ++i) { // uint256 has 32 bytes
            uint8_t byte = powHash.begin()[i];
            if (byte == 0) {
                count++;
            } else {
                break;
            }
        }
        return count >= amount;
    }

    uint32_t GetValue() const {
        // reconstruct the solution
        uint256 powHash = GetHash();
        std::cout << "\nGetting support value:\n";
        std::cout << GetHash().GetHex();
        std::cout << "\nGetting support value:\n";
        std::cout << GetHash().ToString();
        std::cout << "\nGetting support value:\n";
        std::cout << HexStr(GetHash().GetHex());
        std::cout << std::endl;
        // count the zeros
        int count = 0;
        for (int i = 0; i < 32; ++i) { // uint256 has 32 bytes
            uint8_t byte = powHash.begin()[i];
            if (byte == 0) {
                count++;
            } else {
                break;
            }
        }
        int bZeros = count * 4;
        if (bZeros)
            return (bZeros * (bZeros + 1)) / 2;
        return 0;
    }

    SERIALIZE_METHODS(CSupportTicket, obj) { READWRITE(obj.supportedHash, obj.workerPubKey, obj.supportPubKey, obj.timestamp, obj.nonce); }
};


// Type alias for shared pointer to const CSupportTicket
using CSupportTicketRef = std::shared_ptr<const CSupportTicket>;

template <typename St>
static inline CSupportTicketRef MakeSupportTicketRef(St&& stIn) {
    return std::make_shared<const CSupportTicket>(std::forward<St>(stIn));
}

#endif // BITCOIN_PRIMITIVES_SUPPORT_H
