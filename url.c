#include <stdio.h>
#include <curl/curl.h>

int url(char* file_path) {
    // URL of the image to download
    printf("please enter your url of the image you want to download:\n");
    char url[MAX_SIZE];
    scanf("%s" , url);

    // File path to save the downloaded image
    printf("please enter your desired path to save the downloaded image:\n");
    scanf("%s" , file_path);

    // Create a file to save the downloaded image
    FILE* file = fopen(file_path, "wb");
    if (!file) {
        fprintf(stderr, "Failed to create file: %s\n", file_path);
        return 1;
    }

    // Initialize the libcurl library
    CURL* curl = curl_easy_init();
    if (curl) {
        // Set the URL to download
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the write function to save the downloaded image
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

        // Perform the request and download the image
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "Failed to download image: %s\n", curl_easy_strerror(res));
            fclose(file);
            remove(file_path);
            return 1;
        }

        // Cleanup libcurl
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Failed to initialize libcurl.\n");
        fclose(file);
        remove(file_path);
        return 1;
    }

    // Close the file
    fclose(file);

    printf("Image downloaded successfully.\n");

    return 0;
}
