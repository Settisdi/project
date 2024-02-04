#include <stdio.h>
#include <stdlib.h>
#include "emaill.c"
#define MAX_SIZE 1000
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "url.c"



//effects functions start ------------------------------------------------

unsigned char clampp(int value) {
    if (value < 0) {
        return 0;
    } else if (value > 255) {
        return 255;
    } else {
        return (unsigned char)value;
    }
}

void SepiaTone(unsigned char* image_data, int width, int height, int channels) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Compute the sepia tone values for each pixel
            int index = (y * width + x) * channels;
            unsigned char red = image_data[index];
            unsigned char green = image_data[index + 1];
            unsigned char blue = image_data[index + 2];

            // Calculate the new color values for sepia tone
            // A function to clamp a value to the range of 0 to 255


// Calculate the new color values for sepia tone
            unsigned char sepia_red = clampp((red * 0.393) + (green * 0.769) + (blue * 0.189));
            unsigned char sepia_green = clampp((red * 0.349) + (green * 0.686) + (blue * 0.168));
            unsigned char sepia_blue = clampp((red * 0.272) + (green * 0.534) + (blue * 0.131));

            // Set the new color values for sepia tone
            image_data[index] = sepia_red;
            image_data[index + 1] = sepia_green;
            image_data[index + 2] = sepia_blue;
        }
    }
}

void darken(unsigned char *image_data, int width, int height, int channels, float amount){
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate pixel index
            int pixel_index = (y * width + x) * channels;

            // Add random noise to each channel
            for (int c = 0; c < channels; c++) {
                int value = image_data[pixel_index + c] * amount;
                // Clamp the value between 0 and 255
                if (value < 0) value = 0;
                if (value > 255) value = 255;
                // Assign the new value to the pixel
                image_data[pixel_index + c] = (unsigned char)value;
            }
        }
    }
}

void invert(unsigned char* image_data, int width, int height, int channels) {
    // Apply the invert filter to the image
    for (int i = 0; i < width * height * channels; i++) {
        // Invert the pixel value by subtracting it from 255
        int pixel_value = image_data[i];
        int inverted_value = 255 - pixel_value;

        // Assign the inverted value back to the pixel
        image_data[i] = (unsigned char)inverted_value;
    }
}


void Blur(unsigned char* image_data, int width, int height, int channels, int radius) {
    int image_size = width * height * channels;//total num of bytes
    unsigned char* blurred_data = (unsigned char*)malloc(image_size * sizeof(unsigned char));

    // Compute the blur kernel
    int kernel_size = 2 * radius + 1;
    float* kernel = (float*)malloc(kernel_size * kernel_size * sizeof(float));
    float kernel_sum = 0.0f;

    for (int i = 0; i < kernel_size; i++) {
        for (int j = 0; j < kernel_size; j++) {
            kernel[i * kernel_size + j] = 1.0f;
            kernel_sum += kernel[i * kernel_size + j];
        }
    }

    // Normalize the kernel
    for (int i = 0; i < kernel_size * kernel_size; i++) {
        kernel[i] /= kernel_sum;
    }

    // Apply the blur effect
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channels; c++) {
                float blurred_value = 0.0f;

                for (int i = -radius; i <= radius; i++) {
                    for (int j = -radius; j <= radius; j++) {
                        int neighbor_x = x + j;
                        int neighbor_y = y + i;

                        // Clamp the neighbor coordinates within the image bounds
                        neighbor_x = (neighbor_x < 0) ? 0 : (neighbor_x >= width ? width - 1 : neighbor_x);
                        neighbor_y = (neighbor_y < 0) ? 0 : (neighbor_y >= height ? height - 1 : neighbor_y);

                        // Compute the blurred value using the kernel and the neighbor pixel value
                        blurred_value += kernel[(i + radius) * kernel_size + (j + radius)] * image_data[(neighbor_y * width + neighbor_x) * channels + c];
                    }
                }

                blurred_data[(y * width + x) * channels + c] = (unsigned char)blurred_value;
            }
        }
    }

    // Copy the blurred data back to the original image data
    memcpy(image_data, blurred_data, image_size * sizeof(unsigned char));

    // Free memory
    free(blurred_data);
    free(kernel);
}

void brightness(unsigned char *image_data, int width, int height, int channels, int amount){
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate pixel index
            int pixel_index = (y * width + x) * channels;

            // Add random noise to each channel
            for (int c = 0; c < channels; c++) {
                int value = image_data[pixel_index + c] * amount;
                // Clamp the value between 0 and 255
                if (value < 0) value = 0;
                if (value > 255) value = 255;
                // Assign the new value to the pixel
                image_data[pixel_index + c] = (unsigned char)value;
            }
        }
    }
}

void glitch(unsigned char* img_data, int img_width, int img_height, int img_channels) {
    for (int i = 0; i < 10; i++) {
        // Choose a random source and destination rectangle within the image
        int src_x = rand() % img_width;
        int src_y = rand() % img_height;
        int src_w = rand() % (img_width - src_x);
        int src_h = rand() % (img_height - src_y);
        int dst_x = rand() % img_width;
        int dst_y = rand() % img_height;
        int dst_w = rand() % (img_width - dst_x);
        int dst_h = rand() % (img_height - dst_y);
        // Copy and paste the source rectangle to the destination rectangle
        for (int y = 0; y < src_h && y < dst_h; y++) {
            for (int x = 0; x < src_w && x < dst_w; x++) {
                // Get the pointer to the source pixel
                unsigned char *src_pixel = img_data + ((src_y + y) * img_width + (src_x + x)) * img_channels;
                // Get the pointer to the destination pixel
                unsigned char *dst_pixel = img_data + ((dst_y + y) * img_width + (dst_x + x)) * img_channels;
                // Copy the pixel values from source to destination
                for (int c = 0; c < img_channels; c++) {
                    dst_pixel[c] = src_pixel[c];
                }
            }
        }
    }
}

void EdgeDetection(unsigned char* image_data, int width, int height, int channels) {
    // Create a temporary copy of the image data
    unsigned char* temp_data = malloc(width * height * channels);
    memcpy(temp_data, image_data, width * height * channels);

    // Define the edge detection kernel
    int kernel[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};

    // Apply the edge detection filter to the image
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            for (int c = 0; c < channels; c++) {
                int sum = 0;
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        int index = ((y + i) * width + (x + j)) * channels + c;
                        sum += temp_data[index] * kernel[i + 1][j + 1];
                    }
                }
                int index = (y * width + x) * channels + c;
                image_data[index] = (unsigned char)(sum > 255 ? 255 : (sum < 0 ? 0 : sum));
            }
        }
    }

    // Free the temporary image data
    free(temp_data);
}

void Grain(unsigned char *image_data, int width, int height, int channels, int intensity) {

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate pixel index
            int pixel_index = (y * width + x) * channels;

            // Add random noise to each channel
            for (int c = 0; c < channels; c++) {
                int noise = rand() % intensity;  // Generate random noise
                int pixel_value = image_data[pixel_index + c];  // Get original pixel value
                int new_pixel_value = pixel_value + noise;  // Add noise
                new_pixel_value = (new_pixel_value > 255) ? 255 : new_pixel_value;  // Clamp value to 255
                image_data[pixel_index + c] = (unsigned char)new_pixel_value;  // Update pixel value
            }
        }
    }
}

void grayscale(unsigned char *image_data, int width, int height, int channels){
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Compute the grayscale value for each pixel
            int index = (y * width + x) * channels;
            unsigned char red = image_data[index];
            unsigned char green = image_data[index + 1];
            unsigned char blue = image_data[index + 2];

            // Calculate the grayscale value using a simple average method
            unsigned char grayscale = (unsigned char)((red + green + blue) / 3);

            // Set the red, green, and blue channels to the grayscale value
            image_data[index] = grayscale;
            image_data[index + 1] = grayscale;
            image_data[index + 2] = grayscale;
        }
    }
}

void emboss(unsigned char* image_data, int width, int height, int channels) {
    int embossMatrix[3][3] = {
            {-2, -1, 0},
            {-1, 1, 1},
            {0, 1, 2}
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            // Calculate pixel index
            int pixel_index = (y * width + x) * channels;

            // Apply the emboss filter to each channel
            for (int c = 0; c < channels; c++) {
                int sum = 0;
                for (int j = -1; j <= 1; j++) {
                    for (int i = -1; i <= 1; i++) {
                        int neighbor_index = ((y + j) * width + (x + i)) * channels;
                        int weight = embossMatrix[j + 1][i + 1];
                        sum += image_data[neighbor_index + c] * weight;
                    }
                }
                sum += 128; // Add 128 to shift the range from -255 to 255 to 0 to 255
                sum = sum < 0 ? 0 : (sum > 255 ? 255 : sum); // Clamp the value within 0-255 range
                image_data[pixel_index + c] = (unsigned char)sum;
            }
        }
    }
}

void Mirror(unsigned char* image_data, int width, int height, int channels) {
    int row_size = width * channels;
    unsigned char* row_data = malloc(row_size);

    for (int y = 0; y < height; y++) {
        int start_index = y * row_size;
        int end_index = start_index + row_size - channels;

        // Reverse the pixel order in each row
        while (start_index < end_index) {
            memcpy(row_data, &image_data[start_index], channels);
            memcpy(&image_data[start_index], &image_data[end_index], channels);
            memcpy(&image_data[end_index], row_data, channels);
            start_index += channels;
            end_index -= channels;
        }
    }

    free(row_data);
}

void Posterize(unsigned char* image_data, int width, int height, int channels, int levels) {
    // Calculate the step size for each color channel based on the desired number of levels
    int step = 255 / (levels - 1);

    // Apply the posterize filter to the image
    for (int i = 0; i < width * height * channels; i++) {
        // Round down the pixel value to the nearest step
        int pixel_value = image_data[i];
        int posterized_value = (pixel_value / step) * step;

        // Assign the posterized value back to the pixel
        image_data[i] = (unsigned char)posterized_value;
    }
}

void contrast(unsigned char* image_data, int width, int height, int channels, float amount) {
    float factor = (259.0f * (amount + 255.0f)) / (255.0f * (259.0f - amount));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Calculate pixel index
            int pixel_index = (y * width + x) * channels;

            // Adjust the contrast of each channel
            for (int c = 0; c < channels; c++) {
                int value = image_data[pixel_index + c];
                value = (int)((factor * (value - 128)) + 128);
                // Clamp the value between 0 and 255
                if (value < 0) value = 0;
                if (value > 255) value = 255;
                // Assign the new value to the pixel
                image_data[pixel_index + c] = (unsigned char)value;
            }
        }
    }
}

void Solarize(unsigned char* image_data, int width, int height, int channels) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Compute the solarize filter values for each pixel
            int index = (y * width + x) * channels;
            unsigned char red = image_data[index];
            unsigned char green = image_data[index + 1];
            unsigned char blue = image_data[index + 2];

            // Calculate the new color values for the solarize filter
            unsigned char solarize_red = red < 128 ? 255 - red : red;
            unsigned char solarize_green = green < 128 ? 255 - green : green;
            unsigned char solarize_blue = blue < 128 ? 255 - blue : blue;

            // Set the new color values for the solarize filter
            image_data[index] = solarize_red;
            image_data[index + 1] = solarize_green;
            image_data[index + 2] = solarize_blue;
        }
    }
}

void coloroverlay(unsigned char* image_data, int width, int height, int channels, unsigned char redOverlay, unsigned char greenOverlay, unsigned char blueOverlay, float alpha) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Compute the index for each pixel
            int index = (y * width + x) * channels;

            // Retrieve the original color values
            unsigned char red = image_data[index];
            unsigned char green = image_data[index + 1];
            unsigned char blue = image_data[index + 2];

            // Calculate the blended color values based on the overlay and alpha
            unsigned char blended_red = (unsigned char)((red * (1.0 - alpha)) + (redOverlay * alpha));
            unsigned char blended_green = (unsigned char)((green * (1.0 - alpha)) + (greenOverlay * alpha));
            unsigned char blended_blue = (unsigned char)((blue * (1.0 - alpha)) + (blueOverlay * alpha));

            // Set the new color values for the overlay
            image_data[index] = blended_red;
            image_data[index + 1] = blended_green;
            image_data[index + 2] = blended_blue;
        }
    }
}
// effects end --------------------------------------------------

int effects(unsigned char* image_data , int width , int height , int num_channels ){
    printf("Enter the number of the effect you want to be applied to your photo:");
    printf("\n1.Black & White\n2.Blur\n3.Noise\n4.Brighten\n5.Darken\n6.Sepia\n7.Solarize\n8.glitch\n");
    printf("9.Mirror\n10.Edge Detection\n11.Color Overlay\n12.Posterize\n13.Invert\n14.emboss\n15.contrast\n");
    int request ;
    scanf("%d",&request);
    switch (request) {
        case 1: {
            grayscale(image_data, width, height, num_channels) ;
            return 0;
        }
        case 2: {
            int radius = 1, flag = 1;
            while (flag) {
                printf("Enter the amount that you want your photo to be blurred:\t(the blur range is between 1-10 ,the higher -> the more blurred.)\n");
                scanf("%d",&radius);
                if (radius >= 1 && radius <= 10) {
                    Blur(image_data, width, height, num_channels, radius) ;
                    flag = 0;
                } else {
                    printf("Wrong value entered!\n");
                    return 1;
                }
            }
            return 0;
        }
        case 3: {
            int intensity = 100, flag = 1;
            while (flag) {
                printf("Enter the amount that you want your photo to be noised:\t(the noise range is between 0-100 ,the higher -> the noisier.)\n");
                scanf("%d",&intensity);
                if (intensity >= 0 && intensity <= 100) {
                    Grain(image_data, width, height, num_channels, intensity);
                    flag = 0;
                } else {
                    printf("Wrong value entered!\n");
                    return 1;
                }
            }
            return 0;
        }
        case 4: {
            int bright = 2, flag = 1;
            while (flag) {
                printf("Enter the amount that you want your photo to be brightened:\t(the brightness range is between 1-10 ,the higher -> the brighter.)\n");
                scanf("%d",&bright);
                if (bright >= 1 && bright <= 10) {
                    brightness(image_data, width, height, num_channels, bright);
                    flag = 0;
                } else {
                    printf("Wrong value entered!\n");
                    return 1;
                }
            }
            return 0;
        }
        case 5: {
            float darkness = 0.5;
            int flag = 1;
            while (flag) {
                printf("Enter the amount that you want your photo to be darkened:\t(the darkness range is between 0.0-1.0 ,the lower -> the darker.)\n");
                scanf("%f", &darkness);
                if (darkness >= 0.0 && darkness <= 1.0) {
                    darken(image_data, width, height, num_channels, darkness);
                    flag = 0;
                } else {
                    printf("Wrong value entered!\n");
                    return 1;
                }
            }
            return 0;
        }
        case 6: {
            SepiaTone(image_data, width, height, num_channels);
            return 0;
        }
        case 7: {
            Solarize(image_data, width, height, num_channels);
            return 0;
        }
        case 8: {
            glitch(image_data, width, height, num_channels);
            return 0;
        }
        case 9: {
            Mirror(image_data, width, height, num_channels);
            return 0;
        }
        case 10: {
            EdgeDetection(image_data, width, height, num_channels);
            return 0;
        }
        case 11: {
            printf("Some presaved overlays are:\n1.Red\t2.Green\t3.Blue\n4.Pink\t5.Yellow\t6.Cyan\n");
            printf("If you wish to apply another color please enter 0.\n");
            int c;
            scanf("%d" , &c);
            int red = 0, green = 0, blue = 0;
            switch (c) {
                case 0: {
                    printf("\nPlease enter the decimal value of the RGB color you want:\nRed:");
                    scanf("%d",&red);
                    printf("\nGreen:");
                    scanf("%d",&green);
                    printf("\nBlue:");
                    scanf("%d",&blue);
                    break;
                }
                case 1:
                    red = 255;
                    break;
                case 2:
                    green = 128;
                    break;
                case 3:
                    blue = 255;
                    break;
                case 4: {
                    red = 255;
                    green = 192;
                    blue = 203;
                    break;
                }
                case 5: {
                    green = 255;
                    blue = 255;
                    break;
                }
                case 6: {
                    red = 255;
                    green = 255;
                    break;
                }
                default:
                    break;
            }
            coloroverlay(image_data, width, height, num_channels, red, green, blue, 0.5);
            return 0;
            case 12: {
                int level = 10, flag = 1;
                while (flag) {
                    printf("Enter the level that you want your photo to be posterized:\t(the level range is between 2-255.)\n");
                    scanf("%d",&level);
                    if (level >= 2 && level <= 255) {
                        Posterize(image_data, width, height, num_channels, level);
                        flag = 0;
                    } else {
                        printf("Wrong value entered!\n");
                        return 1;
                    }
                }
                return 0;
            }
            case 13: {
                invert(image_data, width, height, num_channels);
                return 0;
            }
            case 14 : {
                emboss(image_data , width , height , num_channels);
                return 0;
            }
            case 15 : {
                int aamount;
                printf("Enter the amount that you want your photo to be contrasted:\t(the contrast range is between 0-100 ,the higher -> the more contrasty.)\n");
                scanf("%d" , &aamount);
                if(aamount >= 0 && aamount <= 100){
                    contrast(image_data , width , height , num_channels , aamount);
                    return 0;
                }
                else {
                    printf("please enter a valid number!\n");
                    return 0;
                }
            }
            default: {
                printf("Wrong number entered!");
                return 1;
            }
        }
    }
}



int main() {
    int run = 1;
    printf("Hi\nWelcome to our program.\nIn this program you can apply effects to your photo(like photo shop!)\n");
    while (run) {
        char* path = (char*) malloc(MAX_SIZE * sizeof(char)) ;
        //getting image ways
        printf("\nNow!How should we receive your image?\t(enter the number of your request)\n1.By your file path\n2.By your URL address\n3.By your email\n0.Exit the program\n");
        int req ;
        scanf("%d" , &req);
        switch (req) {
            case 1:{
                printf("Please enter the image address of the image: \n");
                printf("WARNING please attention in your image path ! (example : /Users/../example.jpeg\n");
                scanf("%s", path);
                break;
            }
            case 2:{
                if( url(path) != 0 ){
                    printf("failed to download file from the url\n");
                    run=0;
                }
                break;

            }
            case 3:{
                printf("please enter your desired path for saving image without filter from your email:\n");
                printf("WARNING please attention in your image path ! (example : /Users/../imageName.jpeg\n");
                scanf("%s" , path );
                if (path == NULL)
                    printf("Given path not valid\n");
                else{
                    if( emailpath(path) != 0){
                        printf("There is problem in getting your image from your email\n");
                        run = 0;
                    }
                }
                break;
            }
            case 0:{
                printf("Thanks for using our program.\nHope To See You Again ;)");
                run = 0;
                break;
            }
            default:
                printf("Wrong number entered!");
        }
        if(run == 0)
            break;
        //given image format
        printf("What is the format of your file?");
        char format[10] = {};
        scanf("%s", format);
        int len = strlen(format);
        format[len] = '\0';
        if(strcmp( format , "jpeg") == 0 ||strcmp( format , "jpg") == 0 || strcmp(format , "png") ==0){
            int width, height, channels;
            unsigned char* image_data;
            image_data = stbi_load(path, &width, &height, &channels, 0);
            if (image_data == NULL) {
                printf("Failed to load image: %s\n", path);
                break;
            }

            //effects
            if(effects(image_data , width , height , channels) != 0){
                printf("there is a problem in adding filter to your image!\n");
                break;
            }
            // saving image
            printf("Enter the address you want your filtered image to be saved: ");
            printf("WARNING please attention in your image path ! (example : /Users/../imageName.jpeg\n");
            char save_file[100];
            scanf("%s", save_file);
            int result = stbi_write_jpg(save_file, width, height, channels, image_data, 100);
            if (result == 0) {
                printf("Failed to save image: %s\n", save_file);
                stbi_image_free(image_data);
                break;
            }
            stbi_image_free(image_data);
            printf("Image saved successfully as: %s\n", save_file);

        }
        // //////////////////////////////////////////////////////////////////////////////////////////////////////////////
        else if(strcmp(format , "bmp") == 0){
            //bmp
            FILE *bmp_file = fopen(path, "rb"); // open BMP file in binary mode
            if (bmp_file == NULL) {
                printf("Error: could not open BMP file.\n");
                return 1;
            }

            // read BMP file header
            unsigned char header[54];
            fread(header, sizeof(unsigned char), 54, bmp_file);

            // get image width, height, and bit depth from BMP file header
            int width = *(int*)&header[18];
            int height = *(int*)&header[22];
            int bit_depth = *(int*)&header[28];

            // read color table (if any) from BMP file
            if (bit_depth <= 8) {
                int color_table_size = 4 * (1 << bit_depth);
                unsigned char color_table[color_table_size];
                fread(color_table, sizeof(unsigned char), color_table_size, bmp_file);
            }

            // read image data from BMP file
            int row_size = ((width * bit_depth + 31) / 32) * 4;
            unsigned char * image_data =malloc((row_size * height) * sizeof(unsigned char));
            fread(image_data, sizeof(unsigned char), row_size * height, bmp_file);

            // close BMP file
            fclose(bmp_file);

            // read color table (if any) from BMP file
            int color_table_size = 0;
            unsigned char color_table[1024]; // maximum size of color table
            if (bit_depth <= 8) {
                color_table_size = 4 * (1 << bit_depth);
                fread(color_table, sizeof(unsigned char), color_table_size, bmp_file);
            }
            int num_channels;
            // Determine the number of channels based on the color depth
            if (bit_depth == 24) {
                num_channels = 3;
            } else if (bit_depth == 32) {
                num_channels = 4;
            } else {
                printf("Error: Unsupported color depth: %d\n", bit_depth);
                fclose(bmp_file);
            }

            // effects bmp
            if(effects(image_data , width , height , 3) != 0){
                printf("there is a problem in adding filter to your image!\n");
                break;
            }

            //save bmp
            char filename[100];
            printf("Enter the address you want your filtered image to be saved: ");
            printf("WARNING please attention in your image path ! (example : /Users/../imageName.jpeg\n");
            scanf("%s", filename);
            FILE *new_bmp_file = fopen(filename, "wb");
            if (new_bmp_file == NULL) {
                printf("Error: could not create new BMP file.\n");
                return 1;
            }
            printf("Image saved successfully.\n");

            // write BMP file header to new file
            fwrite(header, sizeof(unsigned char), 54, new_bmp_file);

            // write color table (if any) to new BMP file
            if (bit_depth <= 8) {
                int colorTableSize = 4 * (1 << bit_depth);
                fwrite(color_table, sizeof(unsigned char), colorTableSize, new_bmp_file);
            }

            // write modified image data to new BMP file
            fwrite(image_data, sizeof(unsigned char), row_size * height, new_bmp_file);

            // close new BMP file
            fclose(new_bmp_file);

        }
        else {
            printf("format not supported");
        }

    free(path);
    }
    return 0;
}