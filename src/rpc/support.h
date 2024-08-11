// support.h
#ifndef BITCOIN_RPC_SUPPORT_H
#define BITCOIN_RPC_SUPPORT_H

#include <rpc/util.h>
#include <validation.h>
#include <rpc/server.h>
#include <node/context.h>
#include <pubkey.h>
#include <globals.h>

using node::NodeContext;
// support settings
extern std::string g_support_status;
extern CPubKey g_worker_pubkey;
extern CPubKey g_support_pubkey;

// static RPCHelpMan getsupporttickets()
// {
//     return RPCHelpMan{"getsupporttickets",
//                       "\nReturns the list of support tickets in the support mempool.\n",
//                       {},
//                       RPCResult{
//                           RPCResult::Type::ARR, "", "List of support tickets",
//                           {
//                               {RPCResult::Type::OBJ, "", "",
//                                {
//                                    {RPCResult::Type::STR_HEX, "supportedHash", "The transaction id of the support ticket"},
//                                    {RPCResult::Type::STR_HEX, "workerpubkey", "The worker public key"},
//                                    {RPCResult::Type::STR_HEX, "supportpubKey", "The supporter public key"},
//                                    {RPCResult::Type::NUM, "timestamp", "The support ticket's time"},
//                                    {RPCResult::Type::NUM, "nonce", "The nonce of the support ticket"},
//                                }},
//                           }},
//                       RPCExamples{
//                           HelpExampleCli("getsupporttickets", "") +
//                           HelpExampleRpc("getsupporttickets", "")
//                       },
//                       [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
//                       {
//                           UniValue result(UniValue::VARR);

//                           LOCK(g_support_mempool->cs);  // Use -> to access the member through the pointer
//                           for (const auto& entry : g_support_mempool->getSupportTickets()) {  // Use -> to access the member function
//                               UniValue ticket(UniValue::VOBJ);
//                               ticket.pushKV("supportedHash", HexStr(entry->supportedHash));
//                               ticket.pushKV("workerpubkey", HexStr(entry->workerPubKey));
//                               ticket.pushKV("supportpubKey", HexStr(entry->pubKey));
//                               ticket.pushKV("timestamp", (int32_t)entry->nonce);
//                               ticket.pushKV("nonce", (int32_t)entry->nonce);
//                               result.push_back(ticket);
//                           }

//                           return result;
//                       }};
// }

static RPCHelpMan getsupportinfo()
{
    /* Please, avoid using the word "pool" here in the RPC interface or help,
     * as users will undoubtedly confuse it with the other "memory pool"
     */
    return RPCHelpMan{"getsupportinfo",
                "Returns an object containing information about support pubkeys (Worker & Supporter).\n",
                {},
                {
                    RPCResult{
                        RPCResult::Type::OBJ, "", "", {
                            {RPCResult::Type::STR, "status", "The status of the support"},
                            {RPCResult::Type::STR_HEX, "workerpubkey", "The pubkey used in support"},
                            {RPCResult::Type::STR_HEX, "supportpubkey", "The pubkey gets the support reward"},
                        }
                    },
                },
                RPCExamples{
                    HelpExampleCli("getsupportinfo", "")
            + HelpExampleRpc("getsupportinfo", "")
                },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
{
    
    UniValue obj(UniValue::VOBJ);
    obj.pushKV("status", g_support_status);
    obj.pushKV("workerpubkey", HexStr(g_worker_pubkey.GetHash()));
    obj.pushKV("supportpubkey", HexStr(g_support_pubkey.GetHash()));
    return obj;
},
    };
}

static RPCHelpMan setsupportstatus()
{
    return RPCHelpMan{"setsupportstatus",
        "\nSets the support status, if set to inactive the node will not participate in the support activites.\n",
        {
            {"status", RPCArg::Type::STR, RPCArg::Optional::NO, "The support status."},
        },
        RPCResult{
            RPCResult::Type::NONE, "", "None"},
        RPCExamples{
            HelpExampleCli("setsupportstatus", "\"active\"") +
            HelpExampleRpc("setsupportstatus", "\"inactive\"")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            std::string value = request.params[0].get_str();
            if (value == "inactive" || value == "active") {
                g_support_status = value;
            }
            return NullUniValue;
        },
    };
}

static RPCHelpMan setworkerpubkey()
{
    return RPCHelpMan{"setworkerpubkey",
        "\nSets worker public key, this key can be used to proof work done in application layer.\n",
        {
            {"workerpubkey", RPCArg::Type::STR_HEX, RPCArg::Optional::NO, "The worker public key."},
        },
        RPCResult{
            RPCResult::Type::NONE, "", "None"},
        RPCExamples{
            HelpExampleCli("setworkerpubkey", "") +
            HelpExampleRpc("setworkerpubkey", "")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            std::string value = request.params[0].get_str();
            if (!value.empty()) {
                std::vector<unsigned char> pubkey_bytes = ParseHex(value);
                g_worker_pubkey.Set(pubkey_bytes.begin(), pubkey_bytes.end());
                NodeContext& node = EnsureAnyNodeContext(request.context);
                myWallet.InitializeFromBlockchain(node);
                if (g_worker_pubkey.IsFullyValid()) {
                    LogPrintf("Worker public key loaded successfully.\n");
                } else {
                    LogPrintf("Invalid worker public key.\n");
                    throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Invalid worker public key");
                }
            } else {
                throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Invalid worker public key");
            }
            return NullUniValue;
        },
    };
}

static RPCHelpMan setsupportpubkey()
{
    return RPCHelpMan{"setsupportpubkey",
        "\nSets supporter public key, this key can claim the support reward later.\n",
        {
            {"supportpubkey", RPCArg::Type::STR_HEX, RPCArg::Optional::NO, "The supporter public key."},
        },
        RPCResult{
            RPCResult::Type::NONE, "", "None"},
        RPCExamples{
            HelpExampleCli("setsupportpubkey", "") +
            HelpExampleRpc("setsupportpubkey", "")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            std::string value = request.params[0].get_str();
            if (!value.empty()) {
                std::vector<unsigned char> pubkey_bytes = ParseHex(value);
                g_support_pubkey.Set(pubkey_bytes.begin(), pubkey_bytes.end());
                if (g_support_pubkey.IsFullyValid()) {
                    LogPrintf("Support public key loaded successfully.\n");
                } else {
                    LogPrintf("Invalid support public key.\n");
                    throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Invalid support public key");
                }
            } else {
                throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Invalid support public key");
            }
            return NullUniValue;
        },
    };
}

static RPCHelpMan createsupportticket()
{
    return RPCHelpMan{"createsupportticket",
        "\nCreates a support ticket for a given output.\n",
        {
            {"nType", RPCArg::Type::NUM, RPCArg::Optional::NO, "The type (0:content, 1:trust)"},
            {"nAmount", RPCArg::Type::NUM, RPCArg::Optional::NO, "The amount"},
            {"nRequestSupport", RPCArg::Type::NUM, RPCArg::Optional::NO, "The requested support"},
            {"lock", RPCArg::Type::STR_HEX, RPCArg::Optional::NO, "The lock script"},
            {"signature", RPCArg::Type::STR_HEX, RPCArg::Optional::NO, "The lock signature"},
            {"ownerkey", RPCArg::Type::STR_HEX, RPCArg::Optional::NO, "The owner public key"},
        },
        RPCResult{
            RPCResult::Type::NONE, "", "None"},
        RPCExamples{
            HelpExampleCli("setsupportpubkey", "") +
            HelpExampleRpc("setsupportpubkey", "")
        },
        [&](const RPCHelpMan& self, const JSONRPCRequest& request) -> UniValue
        {
            uint8_t nType = request.params[0].getInt<int>();
            CAmount nAmount = request.params[1].getInt<uint64_t>();
            CAmount nRequestSupport = request.params[2].getInt<uint64_t>();
            std::vector<unsigned char> lockScript = ParseHex(request.params[3].get_str());
            std::vector<unsigned char> signature = ParseHex(request.params[4].get_str());
            // create and validate the owner public key
            std::vector<unsigned char> value = request.params[5].get_str();
            if (!value.empty()) {
                std::vector<unsigned char> pubkey_bytes = ParseHex(request.params[5].get_str());
                CPubKey ownerKey(pubkey_bytes.begin(), pubkey_bytes.end());
                if (ownerKey.IsFullyValid()) {
                    LogPrintf("Support public key loaded successfully.\n");
                } else {
                    LogPrintf("Invalid support public key.\n");
                    throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Invalid public key");
                }
            } else {
                throw JSONRPCError(RPC_DESERIALIZATION_ERROR, "Invalid support public key");
            }
            return NullUniValue;
        },
    };
}





void RegisterSupportRPCCommands(CRPCTable& t)
{
    static const CRPCCommand commands[]{
        {"support", &getsupportinfo},
        // {"support", &getsupporttickets},
        {"support", &setsupportstatus},
        {"support", &setworkerpubkey},
        {"support", &setsupportpubkey},
    };
    for (const auto& c : commands) {
        t.appendCommand(c.name, &c);
    }
}

#endif // BITCOIN_RPC_SUPPORT_H
