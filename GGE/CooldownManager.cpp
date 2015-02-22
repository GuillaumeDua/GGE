#include "CooldownManager.h"

std::mutex															GGE::Events::ReconductibleCooldownsManager::AsyncCallBackManager::_mutex;
GGE::Events::ReconductibleCooldownsManager::AsyncCallBackManager	GGE::Events::ReconductibleCooldownsManager::_asyncCallBackManager;