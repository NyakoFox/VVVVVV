#ifndef VNETWORK_H
#define VNETWORK_H

#include <stdint.h>

#include <naett.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NETWORK_Request naettReq
#define NETWORK_Response naettRes
#define NETWORK_IsResponsePending(response) (!naettComplete(response))
#define NETWORK_GetStatus(response) (naettGetStatus(response))
#define NETWORK_GetBody(response, length) (naettGetBody(response, length))
#define NETWORK_GET naettMethod("GET")
#define NETWORK_RequestMethod naettOption

int32_t NETWORK_init(void);

NETWORK_Response* NETWORK_fetch(const char* url, NETWORK_RequestMethod* method);

void NETWORK_shutdown(void);

void NETWORK_update(void);

void NETWORK_unlockAchievement(const char *name);

int32_t NETWORK_getAchievementProgress(const char *name);

void NETWORK_setAchievementProgress(const char *name, int32_t stat);

#ifdef __cplusplus
}
#endif

#endif /* VNETWORK_H */
