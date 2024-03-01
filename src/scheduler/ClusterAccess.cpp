#include "ClusterAccess.h"

#include <typeinfo>

namespace scheduler
{
    // create a Deployment
    bool ClusterAccess::createDeployment(apiClient_t *apiClient, cJSON *jsonData, char *_namespace, int verbose) const
    {
        if (!jsonData && verbose >= 1)
        {
            printf("Invalid cJSON data file");
            return false;
        }
        if (!apiClient && verbose >= 1)
        {
            printf("Invalid k8s API Client");
            return false;
        }

        v1_deployment_t *deploymentInfo = v1_deployment_parseFromJSON(jsonData);

        v1_deployment_t *adeployment = AppsV1API_createNamespacedDeployment(apiClient, _namespace, deploymentInfo, NULL, NULL, NULL, NULL);
        v1_deployment_free(adeployment);
        v1_deployment_free(deploymentInfo);
        if (apiClient->response_code == 201 || apiClient->response_code == 200)
        {
            if (verbose >= 3)
            {
                printf("Deployment successfully deployed with response code: %li\n", apiClient->response_code);
            }
            return true;
        }
        return false;
    }

    // delete an existing Deployment
    bool ClusterAccess::deleteDeployment(apiClient_t *apiClient, cJSON *jsonData, char *_namespace, int verbose) const
    {
        if (!jsonData)
        {
            printf("Invalid cJSON data file");
            return false;
        }
        if (!apiClient)
        {
            printf("Invalid k8s API Client");
            return false;
        }

        v1_deployment_t *deploymentInfo = v1_deployment_parseFromJSON(jsonData);

        v1_status_t *deploymentDeleteStatus = AppsV1API_deleteNamespacedDeployment(apiClient, deploymentInfo->metadata->name, _namespace, NULL, NULL, 0, 0, NULL, NULL);

        // printf("Status code: %ld\n", apiClient->response_code);
        v1_status_free(deploymentDeleteStatus);
        v1_deployment_free(deploymentInfo);
        if (apiClient->response_code == 200)
            return true;
        return false;
    }

    // create a pod
    bool ClusterAccess::createPod(apiClient_t *apiClient, cJSON *jsonData, char *_namespace, int verbose) const
    {
        if (!jsonData)
        {
            printf("Invalid cJSON data file");
            return false;
        }
        if (!apiClient)
        {
            printf("Invalid k8s API Client");
            return false;
        }

        v1_pod_t *podInfo = v1_pod_parseFromJSON(jsonData);
        v1_pod_t *apod = CoreV1API_createNamespacedPod(apiClient, _namespace, podInfo, NULL, NULL, NULL, NULL);
        // printf("code=%ld\n", apiClient->response_code);
        v1_pod_free(apod);
        v1_pod_free(podInfo);
        if (apiClient->response_code == 201 || apiClient->response_code == 200)
            return true;
        return false;
    }

    // delete an existing pod
    bool ClusterAccess::deletePod(apiClient_t *apiClient, cJSON *jsonData, char *_namespace, int verbose) const
    {
        if (!jsonData)
        {
            printf("Invalid cJSON data file");
            return false;
        }
        if (!apiClient)
        {
            printf("Invalid k8s API Client");
            return false;
        }

        char *podName = cJSON_GetStringValue(cJSON_GetObjectItem(cJSON_GetObjectItem(jsonData, "metadata"), "name"));
        v1_pod_t *pod = CoreV1API_deleteNamespacedPod(apiClient, podName, _namespace, NULL, NULL, 30, 0, NULL, NULL);

        if (pod)
        {
            v1_pod_free(pod);
            pod = NULL;
        }

        // printf("The return code of HTTP request=%ld\n", apiClient->response_code);
        if (200 == apiClient->response_code)
            return true;
        return false;
    }

    bool ClusterAccess::get_namespaced_events(apiClient_t *apiClient, std::string pod_name, std::string _namespace, int verbose) const
    {
        if (apiClient == NULL)
        {
            if (!createAPI_client(&apiClient))
            {
                printf("Cannot load kubernetes configuration.\n");
                return false;
            }
        }
        core_v1_event_list_t *event_list = CoreV1API_listNamespacedEvent(apiClient, const_cast<char *>(_namespace.c_str()), NULL, 0, NULL, NULL, NULL, 0, NULL, NULL, 60, 0, 0);
        std::cout << (char *)(event_list->items->firstEntry->data) << std::endl;
        return true;
    }

    // create a Kubernetes API Client
    bool ClusterAccess::createAPI_client(apiClient_t **apiClient) const
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

    // create resource with the provided JSON data
    bool ClusterAccess::create(cJSON *jsonData, apiClient_t *apiClient, std::string _namespace, int verbose) const
    {
        if (apiClient == NULL)
        {
            if (!createAPI_client(&apiClient))
            {
                printf("Cannot load kubernetes configuration.\n");
                return false;
            }
        }

        cJSON *kind = cJSON_GetObjectItem(jsonData, "kind");
        if (kind != NULL)
        {
            std::string kindVal = kind->valuestring;
            std::transform(kindVal.begin(), kindVal.end(), kindVal.begin(), ::toupper);

            if (kindVal == "DEPLOYMENT")
            {
                if (createDeployment(apiClient, jsonData, const_cast<char *>(_namespace.c_str()), verbose))
                {
                    std::cout << "Initial Deployment Successful" << std::endl;
                }
                else
                {
                    std::cout << "Initial Deplyoment Failed" << std::endl;
                    return false;
                }
            }
            else if (kindVal == "POD")
            {
                if (createPod(apiClient, jsonData, const_cast<char *>(_namespace.c_str()), verbose))
                {
                    std::cout << "Deplyoment successful" << std::endl;
                }
                else
                {
                    std::cout << "Deplyoment failed" << std::endl;
                    return false;
                }
            }
        }

        return true;
    }

    // patch the existing Deployment
    bool ClusterAccess::patchDeployment(apiClient_t *apiClient, cJSON **jsonData, char *_namespace, driver::Patch &patch, int verbose) const
    {
        if (apiClient == NULL)
        {
            if (!createAPI_client(&apiClient))
            {
                printf("Cannot load kubernetes configuration.\n");
                return false;
            }
        }

        v1_deployment_t *deploymentInfo = v1_deployment_parseFromJSON(*jsonData);

        std::string patchBody = "[{\"op\": \"replace\", \"path\": \"" + patch.getPatchPathString() + "\", \"value\":" + patch.getPatchValue() + "}]";
        genericClient_t *genericClient = genericClient_create(apiClient, "apps", "v1", "deployments");

        list_t *contentType = list_createList();
        list_addElement(contentType, (char *)"application/json-patch+json");

        driver::JsonFileHandler fileHandler;

        char *output = Generic_patchNamespacedResource(genericClient, _namespace, deploymentInfo->metadata->name, const_cast<char *>(patchBody.c_str()), NULL, NULL, NULL, NULL, contentType);

        // std::cout << output << std::endl;

        fileHandler.modifyPatch(*jsonData, patch);
        return true;
    }

    // create a patch
    bool ClusterAccess::patch(cJSON **jsonData, apiClient_t *apiClient, std::string _namespace, driver::Patch &patch, int verbose) const
    {
        if (apiClient == NULL)
        {
            if (!createAPI_client(&apiClient))
            {
                printf("Cannot load kubernetes configuration.\n");
                return false;
            }
        }

        cJSON *kind = cJSON_GetObjectItem(*jsonData, "kind");

        if (verbose >= 10)
        {
            printf("-----------------------------------------\n");
            std::string deploymentName = cJSON_GetObjectItem(cJSON_GetObjectItem(*jsonData, "metadata"), "name")->valuestring;
            std::cout << "Patch for Resource '" << kind->valuestring << "' named '" << deploymentName << "'" << std::endl;
            std::cout << "Patch to be created:" << std::endl;
            std::cout << patch << std::endl;
            printf("-----------------------------------------\n");
        }

        if (kind != NULL)
        {
            std::string kindVal = kind->valuestring;
            std::transform(kindVal.begin(), kindVal.end(), kindVal.begin(), ::toupper);

            if (kindVal == "DEPLOYMENT")
            {
                if (patchDeployment(apiClient, jsonData, const_cast<char *>(_namespace.c_str()), patch, verbose))
                {
                    std::cout << "Patch Deployment successful" << std::endl;
                    // std::cout << cJSON_Print(*jsonData) << std::endl;
                    return true;
                }
                else
                {
                    std::cout << "Patch Deplyoment failed" << std::endl;
                    return false;
                }
            }
            else if (kindVal == "POD")
            {
                if (patchDeployment(apiClient, jsonData, const_cast<char *>(_namespace.c_str()), patch, verbose))
                {
                    std::cout << "Patch Pod successful" << std::endl;
                    return true;
                }
                else
                {
                    std::cout << "Patch Pod failed" << std::endl;
                    return false;
                }
            }
        }

        return true;
    }

    cJSON *ClusterAccess::get_namespaced_deployment(cJSON *jsonData, apiClient_t *apiClient, std::string _namespace, int verbose) const
    {

        if (apiClient == NULL)
        {
            if (!createAPI_client(&apiClient))
            {
                printf("Cannot load kubernetes configuration.\n");
                return cJSON_CreateObject();
            }
        }

        std::string deployment_name = cJSON_GetObjectItem(cJSON_GetObjectItem(jsonData, "metadata"), "name")->valuestring;
        genericClient_t *genericClient = genericClient_create(apiClient, "apps", "v1", "deployments");

        char labelSelector[100];
        snprintf(labelSelector, sizeof(labelSelector), "app.kubernetes.io/name=%s", deployment_name.c_str());

        v1_deployment_t *updatedDeployment = AppsV1API_readNamespacedDeployment(apiClient, const_cast<char *>(deployment_name.c_str()), const_cast<char *>(_namespace.c_str()), NULL);

        cJSON *currentState = v1_deployment_convertToJSON(updatedDeployment);

        // char *output = Generic_readNamespacedResource(genericClient, const_cast<char *>(_namespace.c_str()), const_cast<char *>(deployment_name.c_str()));
        // cJSON *currentState = cJSON_Parse(output);
        // cJSON *currentState = v1_deployment_convertToJSON(deployment);
        // std::cout << cJSON_GetObjectItem(currentState, "metadata")->child->valuestring << std::endl;
        // std::cout << cJSON_Print(currentState) << std::endl;
        return currentState;
    }

    /**
     * @brief Things to check:
     * 1. Confirm the resource allocation
     * 2. Does application has required number of replicas<
     *
     * @return true
     * @return false
     */
    bool ClusterAccess::bugExists(cJSON *initialConfig, apiClient_t *apiClient, std::string _namespace, int verbose) const
    {
        if (apiClient == NULL)
        {
            if (!createAPI_client(&apiClient))
            {
                printf("Cannot load kubernetes configuration.\n");
                return false;
            }
        }

        // -> check the number of pods with time.

        int loop = 0;
        int previous_pod_count = 0;
        int current_pod_count = 0;
        v1_pod_list_t *pod_list;

        do
        {
            previous_pod_count = current_pod_count;
            std::chrono::milliseconds sleepDuration(1000);
            std::this_thread::sleep_for(sleepDuration);
            pod_list = CoreV1API_listNamespacedPod(apiClient,
                                                   const_cast<char *>(_namespace.c_str()), /*namespace */
                                                   NULL,                                   /* pretty */
                                                   0,                                      /* allowWatchBookmarks */
                                                   NULL,                                   /* continue */
                                                   NULL,                                   /* fieldSelector */
                                                   NULL,                                   /* labelSelector */
                                                   0,                                      /* limit */
                                                   NULL,                                   /* resourceVersion */
                                                   NULL,                                   /* resourceVersionMatch */
                                                   0,                                      /* sendInitialEvents */
                                                   0,                                      /* timeoutSeconds */
                                                   0                                       /* watch */
            );
            current_pod_count = pod_list->items->count;
            loop++;
        } while (loop < 5 && previous_pod_count <= current_pod_count);

        std::cout << previous_pod_count << ":" << current_pod_count << std::endl;
        if (loop == 5 && previous_pod_count != current_pod_count)
        {
            printf("Resource bug detected\n");
            // resource bugs detected
            return true;
        }

        // -> check the initial config file values with deployment values

        cJSON *deploymentConfig = get_namespaced_deployment(initialConfig, apiClient, _namespace, verbose);

        if (!compareConfigFile(initialConfig, deploymentConfig, verbose))
        {

            std::cout << "as" << std::endl;
            return true;
        }

        //     // check the deployed cJSON file with the deployment cJSON file.

        return false;
    }

    bool ClusterAccess::compareConfigFile(cJSON *baseConfig, cJSON *checkConfig, int verbose) const
    {
        driver::JsonFileHandler jf;
        std::vector<std::string> keys = {};
        return jf.compareCJSONObjects(baseConfig, checkConfig, keys);
    }

    /**
     * @brief Things to do:
     * 1. Delete the deployment
     * 2. Create the deployment with the initial files
     *
     * @return true
     * @return false
     */
    bool ClusterAccess::reset(cJSON *initialConfig, apiClient_t *apiClient, std::string _namespace, int verbose) const
    {
        if (apiClient == NULL)
        {
            if (!createAPI_client(&apiClient))
            {
                printf("Cannot load kubernetes configuration.\n");
                return false;
            }
        }

        std::string deployment_name = cJSON_GetObjectItem(cJSON_GetObjectItem(initialConfig, "metadata"), "name")->valuestring;

        if (this->deleteDeployment(apiClient, initialConfig, const_cast<char *>(deployment_name.c_str()), verbose) == false)
        {
            return false;
        }

        int count = 0;
        while (count < 10)
        {
            if (this->createDeployment(apiClient, initialConfig, const_cast<char *>(_namespace.c_str()), verbose))
            {
                return true;
            }
            count++;
        }
        return false;
    }

    /**
     * @brief Things to check:
     * 1. Confirm that required number of ready state replicas exist.
     * 2. Confirm that the non-ready state replicas remains constant or have only a very small change.
     *
     * @param initialConfig
     * @param patch
     * @return true
     * @return false
     */
    bool ClusterAccess::isPropagationComplete(cJSON *initialConfig, driver::Patch &patch, int verbose)
    {
        // create the api client for the communication
        apiClient_t *apiClient;
        if (!createAPI_client(&apiClient))
        {
            printf("Cannot load kubernetes configuration.\n");
            return false;
        }

        /**
         * checking the configuration file in etcd
         */
        int desiredReplicaCount = cJSON_GetObjectItem(cJSON_GetObjectItem(initialConfig, "spec"), "replicas")->valueint;
        // std::cout << "Desired Count: " << desiredReplicaCount << std::endl;
        cJSON *currentState = get_namespaced_deployment(initialConfig, apiClient, "default", verbose); // get the current deployment state json file

        int currentReplicaCount = cJSON_GetObjectItem(cJSON_GetObjectItem(currentState, "spec"), "replicas")->valueint;

        // if relicaset count doesn't match; propogation not completed
        if (desiredReplicaCount != currentReplicaCount)
        {
            std::cout << "not matching count" << std::endl;
            return false;
        }

        /**
         * Checking the resources
         */
        char *deploymentName = cJSON_GetObjectItem(initialConfig, "metadata")->child->valuestring;
        char labelSelector[100];
        snprintf(labelSelector, sizeof(labelSelector), "app.kubernetes.io/name=%s", deploymentName);
        char *_namespace = (char *)"default";

        // Construct the field selector for Running pods
        char fieldSelector[100] = "status.phase=Running";

        v1_pod_list_t *pod_list = NULL;
        pod_list = CoreV1API_listNamespacedPod(apiClient,
                                               _namespace,    /*namespace */
                                               NULL,          /* pretty */
                                               0,             /* allowWatchBookmarks */
                                               NULL,          /* continue */
                                               fieldSelector, /* fieldSelector */
                                               NULL,          /* labelSelector */
                                               0,             /* limit */
                                               NULL,          /* resourceVersion */
                                               NULL,          /* resourceVersionMatch */
                                               0,             /* sendInitialEvents */
                                               0,             /* timeoutSeconds */
                                               0              /* watch */
        );

        // cJSON *pod_list_json =  v1_pod_list_convertToJSON(pod_list);

        apiClient_free(apiClient);

        cJSON_free(currentState);

        if (pod_list)
        {
            // printf("Get pod list:\n");
            listEntry_t *listEntry = NULL;
            v1_pod_t *pod = NULL;
            // std::cout << pod_list->items->count << std::endl;
            // std::cout << "ASASSASASASAS" << std::endl;

            int podCount = 0;
            list_ForEach(listEntry, pod_list->items)
            {
                pod = (v1_pod_t *)listEntry->data;
                // printf("\tThe pod name: %s\n", pod->metadata->name);
                podCount++;
            }
            // int podCount = pod_list->items->count;
            // std::cout << "ASASSASASASAS" << std::endl;

            v1_pod_list_free(pod_list);
            // std::cout << "ASASSASASASAS" << std::endl;

            // pod_list = NULL;
            // std::cout << podCount << ":" << desiredReplicaCount << std::endl;
            if (podCount != desiredReplicaCount)
            {
                // std::cout << "ASASSASASASAS" << std::endl;
                return false;
            }
        }
        else
        {
            printf("Cannot get any pod.\n");
            return false;
        }

        driver::JsonFileHandler fileHandler;
        if (!fileHandler.isPatchIncluded(currentState, patch, verbose))
        {
            std::cout << "Patch Not included" << std::endl;
            return false;
        }
        return true;
    }
}
