#include "monitoring.h"

namespace monitoring
{

    // create a Kubernetes API Client
    bool Monitoring::createAPI_client(apiClient_t **apiClient) const
    {
        int rc = 0;

        char *baseName = NULL;
        sslConfig_t *sslConfig = NULL;
        list_t *apiKeys = NULL;

        rc = load_kube_config(&baseName, &sslConfig, &apiKeys, NULL);

        if (0 == rc)
        {
            *apiClient = apiClient_create_with_base_path(baseName, sslConfig, apiKeys);
            return true;
            // printf("Basename: %s\nSSLConfig: %s:%s:%s\n", baseName, sslConfig->clientCertFile, sslConfig->clientKeyFile, sslConfig->CACertFile);
        }
        else
        {
            // printf("Cannot load kubernetes configuration.\n");
            return false;
        }
    }

    bool Monitoring::get_namespaced_events(apiClient_t *apiClient, std::string pod_name, std::string namespace) const
    {
        if (apiClient == NULL)
        {
            if (!createAPI_client(&apiClient))
            {
                printf("Cannot load kubernetes configuration.\n");
                return false;
            }
        }
        core_v1_event_list_t *event = CoreV1API_listNamespacedEvent(apiClient, const_cast<char *>(_namespace.c_str()), NULL, 0, NULL, NULL, NULL, 0, NULL, NULL, 60, 0, 0);
    }

    bool Monitoring::check_pod_exists(apiClient_t *apiClient, std::string pod_name) const
    {
        if (apiClient == NULL)
        {
            if (!createAPI_client(&apiClient))
            {
                printf("Cannot load kubernetes configuration.\n");
                return false;
            }
        }

        std::string _namespace = "default";
        v1_pod_list_t *pod_list_t = CoreV1API_listNamespacedPod(apiClient, const_cast<char *>(_namespace.c_str()), NULL, 0, NULL, NULL, NULL, 0, NULL, NULL, 60, 0, 0);

        list_t *pod_list = pod_list_t->items;

        listEntry_t *currentListEntry = pod_list->firstEntry;
        listEntry_t *nextListEntry;

        if (currentListEntry == NULL)
        {
            return false;
        }

        nextListEntry = currentListEntry->nextListEntry;

        if (std::strcmp(((v1_pod_t *)(currentListEntry->data))->metadata->name, const_cast<char *>(pod_name.c_str())) == 0)
        {
            return true;
        }

        currentListEntry = nextListEntry;

        while (currentListEntry != NULL)
        {
            nextListEntry = currentListEntry->nextListEntry;
            if (std::strcmp(((v1_pod_t *)(currentListEntry->data))->metadata->name, const_cast<char *>(pod_name.c_str())) == 0)
            {
                return true;
            }
            currentListEntry = nextListEntry;
        }
        return false;
    }
}

int main()
{
    monitoring::Monitoring ca;

    if (ca.check_pod_exists(NULL, "busybox-deployment-65fd7d9646-7hqc7"))
    {
        printf("Pod exists\n");
    }
    else
    {
        printf("Pod doesn't exists\n");
    }
    return 0;
}