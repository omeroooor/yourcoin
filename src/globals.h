#ifndef BITCOIN_GLOBALS_H
#define BITCOIN_GLOBALS_H

#include <string>
#include <vector>
#include <pubkey.h>
// #include <support_repository.h>

extern std::string g_support_status;

extern std::vector<uint32_t> g_default_tx_support_range;

extern CPubKey g_worker_pubkey;
extern CPubKey g_support_pubkey;

// extern SupportTicketRepository supportTicketRepository;
// extern TransactionRepository transactionRepository;

#endif // BITCOIN_GLOBALS_H
