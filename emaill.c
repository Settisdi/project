#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 1000

size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // writing data to file
    FILE* file = (FILE*)userp;
    size_t written = fwrite(contents, size, nmemb, file);
    return written;
}

char* str_str(const char* haystack, const char* needle) {

    if (*needle == '\0') {
        return (char*)haystack;
    }

    while (*haystack != '\0') {
        const char* h = haystack;
        const char* n = needle;

        while (*h == *n && *n != '\0') {
            h++;
            n++;
        }

        if (*n == '\0') {
            return (char*)haystack;
        }

        haystack++;
    }

    return NULL;
}

void extractBase64(const char* text, const char* outputFileName) {
    const char* base64Start = str_str(text, "X-Attachment-Id:");
    if (base64Start == NULL) {
        printf("It's not a image code!\n");
        return;
    }

    base64Start += strlen("X-Attachment-Id:");

    base64Start = str_str(base64Start, "\r\n\r\n");
    if (base64Start == NULL) {
        printf("Invalid text format. Base64 code end not found.\n");
        return;
    }

    base64Start += strlen("\r\n\r\n");

    const char* base64End = str_str(base64Start, "\r\n--");
    if (base64End == NULL) {
        printf("Invalid text format. Base64 code end not found.\n");
        return;
    }

    size_t base64Length = base64End - base64Start;
    char* base64Code = (char*)malloc(base64Length + 1);
    strncpy(base64Code, base64Start, base64Length);
    base64Code[base64Length] = '\0';

    FILE* outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        printf("Failed to open the output file.\n");
        return;
    }

    fprintf(outputFile, "%s", base64Code);
    fclose(outputFile);

    free(base64Code);
}


int emailpath(char* path) {
    CURL* curl;// for hold curl handle
    CURLcode res; // curl result

    // Set up libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT); // initialize with default libcurl
    curl = curl_easy_init(); // create curl
    if (curl) {
        char email[MAX_SIZE] = {};
        printf("please enter your gmail address:\n");
        scanf("%s", email); //"psprojectsetayesh@gmail.com"
        char pass[MAX_SIZE] = {};
        printf("please enter your password for gmail:\n");
        scanf("%s", pass); //"wcycsxeliefjnclh"
        // Set the path to the CA certificate file
        curl_easy_setopt(curl, CURLOPT_CAINFO, "/opt/homebrew/etc/openssl@3/cert.pem"); //certificate

        // Set the mailbox name and UID
        char mailbox[] = "INBOX"; // name of mailbox
        printf("you have two options to enter the number of gmail you want to connect to image please enter the number of your option:\n1.giving me the number by your self\n2. automatically reading your last email.\n");
        int option = 0 ;
        int uid=0;
        scanf("%d" , &option);
        if( option == 1){
            printf("Now give me the gmail number!:\n");
            scanf("%d", &uid);
            uid++;
        }
        else if ( option == 2 ){
            FILE *file;
            // Open the file in read mode
            file = fopen("../uid.txt", "r");
            if (file == NULL) {
                printf("Failed to open the file on uid.txt .\n");
                return 1;
            }
            // Read the integer from the file
            if (fscanf(file, "%d", &uid) == 1) {
                printf("The number read from the file is: %d\n", uid);
            } else {
                printf("Failed to read an integer from the file.\n");
            }
            // Close the file
            fclose(file);
            FILE *filee;
            filee = fopen("../uid.txt", "w");
            if (filee == NULL) {
                printf("Failed to write on the file on uid.txt .\n");
                return 1;
            }
            uid++;
            fprintf(filee , "%d" , uid);
            fclose(filee);
        }

        // Create the URL with the UID
        char url[100];
        sprintf(url, "imaps://imap.gmail.com/%s;UID=%d", mailbox, uid);// jagpzari!

        // Connect to the IMAP server and log in
        curl_easy_setopt(curl, CURLOPT_USERNAME, email);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, pass);
        curl_easy_setopt(curl, CURLOPT_URL, url); // use the url as the target

        // Open a file to save the fetched email
        // /Users/setti/Desktop/test.txt
        FILE* file = fopen("./test.txt", "wb");
        if (!file) {
            printf("Error opening file for writing.\n");
            return 1;
        }

        // Set the write callback function to write data into the file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback); // use custom write function as the callback
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file); //file as the user data

        // Perform the fetch request and save the email content to the file
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            printf("Failed to fetch email: %s\n", curl_easy_strerror(res));
            fclose(file);
            curl_easy_cleanup(curl);
            return 1;
        }

        fclose(file);

        // Clean up and close the IMAP session
        curl_easy_cleanup(curl);
    } else {
        printf("Error initializing libcurl.\n");
        return 1;
    }

    // Clean up libcurl
    curl_global_cleanup();

    //split base64 code

    //reading text file
    FILE* text;
    text = fopen( "./test.txt", "r");
    if (text == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }
    //find length
    fseek(text, 0, SEEK_END);
    long file_size = ftell(text);
    rewind(text);
    //allocate
    char* file = (char*)malloc(file_size + 1);
    fread(file, sizeof(char), file_size, text);

    // Add a null terminator at the end of the file
    file[file_size] = '\0';

    fclose(text);
    extractBase64(file, "./base64.txt");

    free(file);
    //decode
    size_t commandLength = strlen(path) + strlen("base64 -D -i ./base64.txt -o ");

    char* command = (char*)malloc(commandLength);
    sprintf(command  ,"base64 -D -i ./base64.txt -o %s" ,path  );


    int result = system(command);

    if (result == 0) {
        printf("image executed successfully .\n");
    } else {
        printf("image execution failed.\n");
    }
    free(command);

    return 0;
}